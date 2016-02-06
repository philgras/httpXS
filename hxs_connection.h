/*
 * hxs_connection.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_CONNECTION_H_
#define HXS_CONNECTION_H_

#include "hxs_std.h"
#include "hxs_list.h"
#include "hxs_buffer.h"
#include "hxs_map.h"
#include <netinet/in.h>

#define MAX_BUF_SIZE 8192

typedef struct hxs_service_s hxs_service_t;
typedef struct hxs_http_handler_s hxs_http_handler_t;
/**
 *
 */

enum {
	HXS_CONN_INIT = 0x01,
	HXS_CONN_SEND = 0x02,
	HXS_CONN_RECV = 0x04,
	HXS_CONN_ERROR = 0x08,
	HXS_CONN_CLOSED = 0x10,
	HXS_CONN_CLEANUP = 0x20,
	HXS_CONN_DONE = 0x40
};




/**
 * simple connection structure
 * prev, next are supposed to be used internally
 */
struct hxs_connection_s {
	hxs_socket_t socket;
	unsigned char status;

	sockaddr_storage_t clientaddr;
	socklen_t clientaddr_size;

	hxs_map_t mod_data_map;

	hxs_http_handler_t* http_handler;
	hxs_service_t* service;

	/*PRIVATE*/
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
struct hxs_listener_s{
	hxs_socket_t socket;

	sockaddr_storage_t hostaddr;
	socklen_t hostaddr_size;

};

typedef struct hxs_listener_s hxs_listener_t;

int hxs_conn_recv(hxs_connection_t* conn, hxs_buffer_t* buf);
int hxs_conn_send(hxs_connection_t* conn, hxs_buffer_t* buf);

/**
 * hxs_listener_t
 */
int hxs_listener_open(hxs_listener_t* listener, const char* port, int afamily,
		int backlog);
int hxs_listener_accept(const hxs_listener_t* listener, hxs_connection_t* connptr) ;
int hxs_listener_close(hxs_listener_t* listener);

#endif /* HXS_CONNECTION_H_ */
