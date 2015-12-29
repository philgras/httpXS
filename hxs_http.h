/*
 * hxs_http.h
 *
 *  Created on: 13.12.2015
 *      Author: philip
 */

#ifndef HXS_HTTP_H_
#define HXS_HTTP_H_

#include "hxs_connection.h"
#include "hxs_buffer.h"
#include "hxs_service.h"
#include "http-parser/http_parser.h"

enum mime_code{
	MIME_HTML = 0,
	MIME_CSS,
	MIME_JS,
	MIME_PLAIN,
	MIME_PNG
};

enum http_code{

	S100 = 0,
	S101,
	S102,
	S200,
	S201,
	S202,
	S300,
	S301,
	S302,
	S400,
	S401,
	S402,
	S403,
	S404,
	S405,
	S500,
	S501
};

typedef struct{

	/*bytes*/
	const char* url;
	size_t url_length;

	/*bytes*/
	const char* body;
	size_t body_length;
	unsigned int done: 1;

}hxs_http_request_t;

typedef struct{

	enum http_code status_code;
	const char* content_type;
//Server: httpXS/1.0 (Unix) (Ubuntu/Linux)
//Connection: close
/*Allow: GET, PUT*/



}hxs_http_response_t;

struct hxs_http_handler_s{


    http_parser parser;
    hxs_http_request_t req;
    hxs_http_response_t resp;
    hxs_buffer_t buf;
    unsigned int keep_alive: 1;
    unsigned int parser_needs_init: 1;


};

typedef struct hxs_http_handler_s hxs_http_handler_t;

void hxs_handle_http_connection(hxs_connection_t* conn);



#endif /* HXS_HTTP_H_ */
