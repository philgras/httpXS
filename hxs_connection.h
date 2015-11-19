/*
 * hxs_connection.h
 *
 *  Created on: 16.11.2015
 *      Author: philip
 */

#ifndef HXS_CONNECTION_H_
#define HXS_CONNECTION_H_

#include "hxs_std.h"
#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in_t ;
typedef struct sockaddr_in6 sockaddr_in6_t ;
typedef struct sockaddr sockaddr_t ;

typedef struct{
	hxs_socket socket;

	sockaddr_t host;
	socklen_t hostaddr_size;
	sockaddr_t client;
	socklen_t clientaddr_size;

	hxs_connection_t* prev;
	hxs_connection_t* last;
}hxs_connection_t;


typedef struct{
	hxs_socket socket;

	sockaddr_t host;
	socklen_t hostaddr_size;

	int more_conns_flag;

}hxs_listening_t;



int hxs_delete_conn(hxs_connection_t* conn);


int hxs_start_listening(hxs_listening_t* listening);
hxs_connection_t* hxs_get_new_conn(hxs_listening_t* listening);
int hxs_stop_listening(hxs_listening_t* listening);

#endif /* HXS_CONNECTION_H_ */
