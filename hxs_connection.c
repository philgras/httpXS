/*
 * hxs_connection.c
 *
 *  Created on: 20.11.2015
 *      Author: philgras
 */

#include "hxs_connection.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>


static hxs_socket get_bound_socket(const char* port, int afamily,
							sockaddr_storage_t* addr, socklen_t* len);
static int enable_non_blocking(int socket);

hxs_connection_t* hxs_conn_list_add(hxs_conn_list_t * list, const hxs_connection_t* conn){

	assert(conn!=NULL);


	hxs_connection_t* n_conn = malloc(sizeof(hxs_connection_t));
	if(n_conn){
		*n_conn = *conn;
		if(*list == NULL){
			n_conn->prev = n_conn;
			n_conn->next  =n_conn;
			*list = n_conn;
		}else{
			n_conn->prev  = (*list)->prev;
			n_conn->next = (*list);
			(*list)->prev->next = n_conn;
			(*list)->prev = n_conn;
		}
	}
	return n_conn;

}

void hxs_conn_list_clear(hxs_conn_list_t* listptr){

	if(*listptr == NULL) return;

	hxs_connection_t* iter = *listptr, *next;

	do{
		next = iter->next;
		hxs_closesocket(iter->socket);
		free(iter);
		HXS_DEBUG_MSG("Cleared connection...\n");
	}while((iter = next) != *listptr);

	*listptr = NULL;

}

hxs_conn_list_t hxs_conn_list_merge(hxs_conn_list_t  list_into, hxs_conn_list_t* list_other){

	assert(list_into != NULL);

	if((*list_other) == NULL) return list_into;

	hxs_connection_t* connptr;

	list_into->prev->next = (*list_other);
	(*list_other)->prev->next = list_into;

	connptr = list_into->prev;
	list_into->prev = (*list_other)->prev;
	(*list_other)->prev = connptr;

	(*list_other) = NULL;

	return list_into;

}


hxs_conn_list_t hxs_get_new_conns(const hxs_listener_t* listener){


	hxs_conn_list_t list = NULL;
	hxs_connection_t new_conn = {};

	while(1){

		new_conn->socket = accept(listener->socket,((sockaddr_t*)&(new_conn->clientaddr)), &(new_conn->clientaddr_size));
		if(new_conn->socket == HXS_INVALID_SOCKET){
			fprintf(HXS_ERROR_STREAM,"Failed to accept a socket with error: %s\n", strerror(errno));
			HXS_DEBUG_MSG("Closing already accepted connections...\n");
			hxs_conn_list_clear(&list);
			return NULL;
		}

		if(enable_non_blocking(new_conn->socket)== HXS_ERROR){
			fprintf(HXS_ERROR_STREAM,"Failed to set the socket to non-blocking mode...\n");
			HXS_DEBUG_MSG("Closing already accepted connections...\n");
			hxs_conn_list_clear(&list);
			return NULL;
		}

		new_conn->status = HXS_CONN_INIT;
		hxs_conn_list_add(&list,&new_conn);

	}


	return hxs_conn_list_t;

}

int hxs_open_listener(hxs_listener_t* listener , const char* port , int afamily , int backlog){


	listener->socket = get_bound_socket(port, afamily,
									&(listener->hostaddr), &(listener->hostaddr_size));

	if(listener->socket == HXS_INVALID_SOCKET){
		fprintf(HXS_ERROR_STREAM,"Failed to get a valid socket...\n");
		return HXS_ERROR;
	}else if(enable_non_blocking(listener->socket) == HXS_ERROR){
		fprintf(HXS_ERROR_STREAM,"Failed to set the socket to non-blocking mode...\n");
		hxs_closesocket(listener->socket);
		return HXS_ERROR;
	} else if(listen(listener->socket, backlog) == -1){
		fprintf(HXS_ERROR_STREAM,"Failed to listen to the socket...\n");
		hxs_closesocket(listener->socket);
		return HXS_ERROR;
	}
	listener->available_conns = NO_CONNS_AVAIL;
	return HXS_OK;

}

int hxs_close_listener(hxs_listener_t* listener){

	hxs_closesocket(listener->socket);
	return HXS_OK;
}

static hxs_socket get_bound_socket(const char* port, int afamily,
							sockaddr_storage_t* addr, socklen_t* len){

	struct addrinfo * res = NULL;
	struct addrinfo hints;
	int yes = 1; //used in the setsockopt()-call
	int rc; //used to get gai_strerror
	int sfd = HXS_INVALID_SOCKET;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = afamily;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((rc = getaddrinfo(NULL, port , &hints, &res ))!= 0){

		fprintf(HXS_ERROR_STREAM, "Failed to getaddrinfo with error: %s\n", gai_strerror(rc));
		return sfd;

	}

	/*don't return before releasing the allocated resources*/

	for(struct addrinfo* iter = res; iter != NULL; iter = iter->ai_next){

		//get the descriptor
		sfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if(sfd == HXS_INVALID_SOCKET){
			fprintf(HXS_ERROR_STREAM, "Failed to get a descriptor with error: %s\n", strerror(errno));
			continue;
		}
		HXS_DEBUG_MSG("Got a socket...\n");


		//setsockopt
		if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) == -1){
			fprintf(HXS_ERROR_STREAM, "Failed to setsockopt with error: %s\n", strerror(errno));
			hxs_closesocket(sfd);
			sfd = HXS_INVALID_SOCKET;
			break;
		}
		HXS_DEBUG_MSG("Set sockopt...\n");

		//bind it
		if(bind(sfd, iter->ai_addr, iter->ai_addrlen) == -1){
			fprintf(HXS_ERROR_STREAM, "Failed to bind the descriptor with error: %s\n", strerror(errno));
			hxs_closesocket(sfd);
			sfd = HXS_INVALID_SOCKET;
			continue;
		}
		HXS_DEBUG_MSG("Bound the socket...\n");

		//assign the addr
		switch(iter->ai_addrlen){

		case sizeof(sockaddr_in6_t):
				*((sockaddr_in6_t*)addr) = *((sockaddr_in6_t*)iter->ai_addr);
				break;
		case sizeof(sockaddr_in_t):
				*((sockaddr_in_t*)addr) = *((sockaddr_in_t*)iter->ai_addr);
				break;
		default:
				fprintf(HXS_ERROR_STREAM,"Neither sockin4 nor sockin6 lengths re correct...\nEXIT\n");
				hxs_closesocket(sfd);
				exit(EXIT_FAILURE);
		}
		*len = iter->ai_addrlen;
		break;
	}

	freeaddrinfo(res);

	return sfd;
}

static int enable_non_blocking(int socket){

	int flags;

	flags = fcntl (socket, F_GETFL, 0);
	if (flags == -1){
	  fprintf(HXS_ERROR_STREAM, "Failed to GET fcntl-flags with error: %s\n", strerror(errno));
	  return HXS_ERROR;
	}

	flags |= O_NONBLOCK;
	if (fcntl (socket, F_SETFL, flags) == -1){
	  fprintf(HXS_ERROR_STREAM, "Failed to SET fcntl-flags with error: %s\n", strerror(errno));
	  return HXS_ERROR;
	}

	return HXS_OK;

}
