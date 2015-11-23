/*
 * hxs_connection.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_CONNECTION_H_
#define HXS_CONNECTION_H_

#include "hxs_std.h"
#include <netinet/in.h>

#define NO_CONNS_AVAIL 0x0;
#define CONNS_AVAIL 0x1;

/**
 * simple connection structure
 * prev, next are supposed to be used internally
 */
struct hxs_connection_s{
	hxs_socket socket;

	sockaddr_storage_t clientaddr;
	socklen_t clientaddr_size;

	struct hxs_connection_s* prev;
	struct hxs_connection_s* next;

};
typedef struct hxs_connection_s hxs_connection_t;

/**
 * hxs_conn_list_t
 */
typedef hxs_connection_t* hxs_conn_list_t;

/**
 * hxs_listener_t
 */
typedef struct{
	hxs_socket socket;

	sockaddr_storage_t hostaddr;
	socklen_t hostaddr_size;

	unsigned int available_conns: 1;

}hxs_listener_t;


/*hxs_conn_list_t functions*/
hxs_connection_t* hxs_conn_list_add(hxs_conn_list_t* listptr, const hxs_connection_t* conn);
void hxs_conn_list_clear(hxs_conn_list_t* listptr);
hxs_conn_list_t hxs_conn_list_merge(hxs_conn_list_t  list_into, hxs_conn_list_t* list_other);

/**
 * hxs_listener_t
 */
int hxs_open_listener(hxs_listener_t* listener , const char* port , int afamily , int backlog);
hxs_connection_t* hxs_get_new_conn(hxs_listener_t* listener);
int hxs_close_listener(hxs_listener_t* listener);

#endif /* HXS_CONNECTION_H_ */
