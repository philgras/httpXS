/*
 * hxs_http.c
 *
 *  Created on: 13.12.2015
 *      Author: philip
 */
#include "hxs_http.h"
#include "http-parser/http_parser.h"

#include <stdlib.h>
#define MAX_BUF 8*1024


static int my_callback(http_parser* parser, const char *at, size_t length) {

    hxs_buffer_t buf;
    hxs_buffer_init(&buf,length+1);
    memcpy(buf.data,at,length);

    buf.data[length] = '\0';
    printf("\nCALL:\n%s\n", buf.data);
    hxs_buffer_free(&buf);
    return 0;
}

static int on_complete(http_parser* parser){

    parser->data = parser;
    return 0;
}


static http_parser_settings settings = {

        .on_url = my_callback
       // .on_status = my_callback,
        //.on_header_field= my_callback,
        //.on_header_value= my_callback,
        //.on_headers_complete = on_complete



};
static void recvloop(hxs_service_t* disp, hxs_connection_t* conn);

static void cleanup(hxs_service_t* disp, hxs_connection_t* conn);

void hxs_handle_http_connection(hxs_service_t* disp, hxs_connection_t* conn){

    switch(conn->status){

    case HXS_CONN_INIT:
        http_parser_init(&(conn->parser), HTTP_REQUEST);
        hxs_buffer_init(&(conn->buf), MAX_BUF);


        recvloop(disp,conn);

        break;

    case HXS_CONN_RECV:

        recvloop(disp,conn);

        break;
    case HXS_CONN_SEND:
        break;
    case HXS_CONN_ERROR:
        cleanup(disp,conn);
        break;
    case HXS_CONN_CLEANUP:
    default: exit(EXIT_FAILURE);

    }




}

static void recvloop(hxs_service_t* disp, hxs_connection_t* conn){
    while(1){
        if(hxs_conn_recv(conn) == HXS_ERROR|| conn->status == HXS_CONN_CLOSED){
               cleanup(disp,conn);
               return;
        }else if(conn->status == HXS_CONN_DONE){


            http_parser_execute(&(conn->parser),&settings,
                                 conn->buf.data,hxs_buffer_length(&(conn->buf)));
            if(conn->parser.data == &(conn->parser)){
                cleanup(disp,conn);
                return;
            }
        }else{
            return;
        }
    }

}

static void cleanup(hxs_service_t* disp, hxs_connection_t* conn){
    hxs_buffer_free(&(conn->buf));
    hxs_service_remove_conn(disp,conn);
}



