/*
 * hxs_http.c
 *
 *  Created on: 13.12.2015
 *      Author: philip
 */
#include "hxs_http.h"
#include "http-parser/http_parser.h"
#include <stdlib.h>

#define HANDLER(parser) ((hxs_http_handler_t*)(parser->data))

static const char * HTTP_CODES_STR[][2] = {
		{"100" ,"Continue"} 				/*S100*/,
		{"101" ,"Switching Protocols"}		/*S101*/,
		{"102" ,"Processing"}				/*S102*/,
		{"200" ,"OK"}						/*S200*/,
		{"201" ,"Created"}			 		/*S201*/,
		{"202" ,"Accepted"}				 	/*S202*/,
		{"300" ,"Multiple Choices"}			/*S300*/,
		{"301" ,"Moved Permanently"}		/*S301*/,
		{"302" ,"Found"}			 		/*S302*/,
		{"400" ,"Bad Request"} 				/*S400*/,
		{"401" ,"Unauthorized"}				/*S401*/,
		{"402" ,"Payment Required"}			/*S402*/,
		{"403" ,"Forbidden"}				/*S403*/,
		{"404" ,"Not Found"}          		/*S404*/,
		{"405" ,"Method Not Allowed"} 		/*S405*/,
		{"500" ,"Internal Server Error"} 	/*S500*/,
		{"501" ,"Not Implemented"}			/*S501*/,
};

static const char* HTTP_MIME_TYPES[][2] = {
		{"text/html","html"},
		{"text/css","css"},
		{"application/javascript","js"},
		{"text/plain", "txt"},
		{"image/png","png"}
};


/*http parser functions*/
static int on_url(http_parser* parser, const char *at, size_t length);
static int on_body(http_parser* parser, const char *at, size_t length);
static int on_complete(http_parser* parser);


static http_parser_settings settings = {
        .on_url = on_url,
		.on_body = on_body,
		.on_message_complete = on_complete
};

static int on_url(http_parser* parser, const char *at, size_t length){
	if(HANDLER(parser)->req.url == NULL){
	    	HANDLER(parser)->req.url = at;

	}
	HANDLER(parser)->req.url_length += length;
	return 0;

}

static int on_body(http_parser* parser, const char *at, size_t length) {

	if(HANDLER(parser)->req.body == NULL){
    	HANDLER(parser)->req.body = at;
	}
	HANDLER(parser)->req.body_length+= length;
    return 0;
}

static int on_complete(http_parser* parser){

	hxs_http_request_t* req = &(HANDLER(parser)->req);
	req->done = 1;
	HANDLER(parser)->keep_alive = http_should_keep_alive(parser);
    return 0;
}


/*http handling functions*/
static int http_handler_init(hxs_http_handler_t* http_handler);
static void inline http_request_init(hxs_http_request_t* req);
static void cleanup(hxs_connection_t* conn);
static int http_handler_process_request(hxs_http_handler_t* http_handler, const hxs_connection_t* conn);
static void inline http_build_std_response(hxs_buffer_t* buf, enum http_code code);
static const char* get_mime_type(const char* url);
static int http_handler_serialize_response(hxs_http_handler_t* handler, hxs_buffer_t* body);

static int http_handler_init(hxs_http_handler_t* http_handler){
	http_parser_init(&(http_handler->parser), HTTP_REQUEST);
	if(hxs_buffer_init(&(http_handler->buf), MAX_BUF_SIZE) == HXS_ERROR){
		return HXS_ERROR;
	}

	http_request_init(&(http_handler->req));

	http_handler->resp.content_type = NULL;

	http_handler->keep_alive= 0;
	http_handler->parser_needs_init =  1;

	return HXS_OK;

}

static void inline http_request_init(hxs_http_request_t* req){
	req->url = NULL;
	req->url_length = 0;
	req->body = NULL;
	req->body_length = 0;

}

static void cleanup(hxs_connection_t* conn){
	if(conn->http_handler != NULL){
		if(conn->http_handler->buf.data != NULL){
			hxs_buffer_free(&(conn->http_handler->buf));
		}
		free(conn->http_handler);
	}
    hxs_service_remove_conn(conn->service,conn);
}

static void inline http_build_std_response(hxs_buffer_t* buf, enum http_code code){

	assert(buf->size >= 2048);
	static const char* str = "HTTP/1.1 %s %s\r\n"
								"Content-Type: text/html\r\n"
								"Connection: close\r\n\r\n"
								"<html><head><title>%s</title></head>"
								"<body>"
								"<h1>%s</h1>"
								"<h2>%s</h2>"
								"</body>"
								"</html>";

	buf->end = buf->data + snprintf(buf->data, buf->size, str,HTTP_CODES_STR[code][0],
												  HTTP_CODES_STR[code][1],
												  HTTP_CODES_STR[code][0],
												  HTTP_CODES_STR[code][0],
												  HTTP_CODES_STR[code][1]);

}

static const char* get_mime_type(const char* url){

	const char* last = url + strlen(url);
	const char* file_type;
	int array_length = sizeof(HTTP_MIME_TYPES)/(2*sizeof(HTTP_MIME_TYPES[0][0]));

	for(int i = 0; i< array_length; ++i){
		file_type = last - strlen(HTTP_MIME_TYPES[i][1]);
		if(strcmp(HTTP_MIME_TYPES[i][1], file_type ) == 0){
			return HTTP_MIME_TYPES[i][0];
		}
	}

	return HTTP_MIME_TYPES[MIME_PLAIN][0];
}

static int http_handler_serialize_response(hxs_http_handler_t* handler, hxs_buffer_t* body){

	static const char* response = "HTTP/1.1 %s %s\r\n"
									"Content-Type: %s\r\n"
									"Content-Length: %zu\r\n"
									"Server: httpXS/1.0 (Unix) (Ubuntu/Linux)\r\n"
									"Connection: %s\r\n\r\n";

	static const char* conn_close =	"close", *conn_keep_alive = "keep-alive";

	static const int MAX_HEADER_SIZE = 512;

	int length;

	if(body->size + MAX_HEADER_SIZE > handler->buf.size){
		if(hxs_buffer_add_memory(&(handler->buf),
				body->size + MAX_HEADER_SIZE - handler->buf.size) == HXS_ERROR){
			return HXS_ERROR;
		}
	}

	length = snprintf(handler->buf.data, handler->buf.size, response,
										   HTTP_CODES_STR[handler->resp.status_code][0],
			 	  	  	  	  	  	  	   HTTP_CODES_STR[handler->resp.status_code][1],
										   handler->resp.content_type,
										   body->size,
										   handler->keep_alive? conn_keep_alive : conn_close);


	handler->buf.end = handler->buf.data + length;

	memcpy(handler->buf.end,body->data,body->size);

	handler->buf.end += body->size;

	return HXS_OK;

}

static int http_handler_process_request(hxs_http_handler_t* http_handler, const hxs_connection_t* conn){


	switch (http_handler->parser.method) {
		case HTTP_GET:
			{
				hxs_buffer_t body;

				size_t path_length = strlen(conn->service->root_path);

				char path [path_length + http_handler->req.url_length + 1];

				strcpy(path,conn->service->root_path);

				memcpy(path+path_length, http_handler->req.url, http_handler->req.url_length);

				path[path_length + http_handler->req.url_length] = '\0';

				HXS_DEBUG_MSG("Requested resource: %s\n", path);

				http_handler->resp.content_type = get_mime_type(path);

				switch(hxs_buffer_init_with_file_data(&body,path)){

				case HXS_OK:

					http_handler->resp.status_code = S200;

					if(http_handler_serialize_response(http_handler, &body)){
						hxs_buffer_free(&body);
						return HXS_ERROR;

					}

					hxs_buffer_free(&body);

					break;

				case HXS_FILE_NOT_FOUND:
					http_build_std_response(&(http_handler->buf),S404);
					http_handler->keep_alive = 0;
					break;

				case HXS_ERROR:
					http_build_std_response(&(http_handler->buf),S500);
					http_handler->keep_alive = 0;
					break;

				default: break;

				}
			}

			break;

		case HTTP_POST:

			http_build_std_response(&(http_handler->buf),S405);
			break;

		default:

			http_build_std_response(&(http_handler->buf),S405);
			break;
	}

	http_handler->buf.read_pos = http_handler->buf.data;

	return HXS_OK;

}

/*for control flow*/
static int send_routine(hxs_connection_t* conn);
static int recv_routine(hxs_connection_t* conn);
static void send_and_recv_loop(hxs_connection_t* conn);

static int recv_routine(hxs_connection_t* conn){

	while(1){

		switch(hxs_conn_recv(conn,&(conn->http_handler->buf))){

		case HXS_OK:{
				size_t length = conn->http_handler->buf.end - conn->http_handler->buf.read_pos;
				size_t rsize = 0;
				rsize = http_parser_execute(&(conn->http_handler->parser),&settings,
						conn->http_handler->buf.read_pos,length);


				conn->http_handler->buf.read_pos += rsize;

				if(rsize != length){
					HXS_DEBUG_MSG("Parser error...\n");
					cleanup(conn);
					return HXS_STOP;
				}


				if(conn->http_handler->req.done){
					return HXS_OK;
				}
			}
			continue;

		case HXS_WAIT: conn->status = HXS_CONN_RECV; return HXS_STOP;
		case HXS_ERROR: cleanup(conn); return HXS_STOP;
		case HXS_CLIENT_CLOSED: cleanup(conn); return HXS_STOP;
		default: exit(EXIT_FAILURE); return HXS_STOP;

		}

	}
}

static int send_routine(hxs_connection_t* conn){

	switch(hxs_conn_send(conn, &(conn->http_handler->buf))){

		case HXS_OK:
			if(conn->http_handler->keep_alive){
				HXS_DEBUG_MSG("Send completed + keep alive...\n");
				conn->http_handler->parser_needs_init = 0;
				return HXS_OK;
			}else{
				HXS_DEBUG_MSG("Send completed + close...\n");
				cleanup(conn);
				return HXS_STOP;
			}
		case HXS_WAIT: conn->status = HXS_CONN_SEND; return HXS_STOP;
		case HXS_ERROR: cleanup(conn); return HXS_STOP;
		default: exit(EXIT_FAILURE); return HXS_STOP;
	}

}

static void send_and_recv_loop(hxs_connection_t* conn){

	HXS_DEBUG_MSG("SEND_RECV_LOOP...\n");

	if(conn->status == HXS_CONN_SEND){
		HXS_DEBUG_MSG("Send Routine...\n");
		if(send_routine(conn) != HXS_OK){
			return;
		}
	}

	while(1){

		if(conn->http_handler->parser_needs_init){
			HXS_DEBUG_MSG("SEND_RECV_LOOP initialization...\n");
			http_parser_init(&(conn->http_handler->parser),HTTP_REQUEST);
			http_request_init(&(conn->http_handler->req));
			conn->http_handler->parser.data = conn->http_handler;
			conn->http_handler->parser_needs_init = 0;
		}

		HXS_DEBUG_MSG("Recv Routine...\n");
		if(recv_routine(conn) != HXS_OK){
			break;
		}
		HXS_DEBUG_MSG("Handle...\n");
		if(http_handler_process_request(conn->http_handler, conn) != HXS_OK){
			cleanup(conn);
			break;
		}
		conn->http_handler->buf.read_pos = conn->http_handler->buf.data;

		HXS_DEBUG_MSG("send routine...\n");
		if(send_routine(conn) != HXS_OK){
			break;
		}

		conn->http_handler->buf.read_pos = conn->http_handler->buf.data;

	}

}

void hxs_handle_http_connection(hxs_connection_t* conn){


    switch(conn->status){

    case HXS_CONN_INIT:

    	conn->http_handler = malloc(sizeof(hxs_http_handler_t));

    	if(conn->http_handler == NULL){

    		cleanup(conn);

    	}else{

    		if( http_handler_init(conn->http_handler) ){

    			cleanup(conn);

    		}else{

    			send_and_recv_loop(conn);

    		}
    	}

        break;

    case HXS_CONN_RECV:

    	send_and_recv_loop(conn);

        break;

    case HXS_CONN_SEND:

    	send_and_recv_loop(conn);

        break;

    case HXS_CONN_ERROR:

        cleanup(conn);
        break;

    case HXS_CONN_CLEANUP:

    default: exit(EXIT_FAILURE);

    }

}




