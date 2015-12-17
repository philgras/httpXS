/*
 * hxs_http.h
 *
 *  Created on: 13.12.2015
 *      Author: philip
 */

#ifndef HXS_HTTP_H_
#define HXS_HTTP_H_

#include "hxs_connection.h"
#include "hxs_service.h"
#include "http-parser/http_parser.h"


typedef struct{



}hxs_http_msg_t;



typedef struct{

    http_parser parser;


}hxs_http_handler_t;

void hxs_handle_http_connection(hxs_connection_t* conn);



#endif /* HXS_HTTP_H_ */
