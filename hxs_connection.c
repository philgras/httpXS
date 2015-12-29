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


static hxs_socket_t get_bound_socket(const char* port, int afamily,
		sockaddr_storage_t* addr, socklen_t* len);
static int enable_non_blocking(hxs_socket_t socket);


int hxs_conn_recv( hxs_connection_t* conn, hxs_buffer_t* buf){

	/*resize if necessary*/
	if(buf->size - hxs_buffer_used(buf) < MAX_BUF_SIZE){
		HXS_DEBUG_MSG("Increase buffer...\n");
    	if(hxs_buffer_add_memory(buf,MAX_BUF_SIZE) == HXS_ERROR){
    		return HXS_ERROR;
    	}
    }

	int rc = recv(conn->socket, buf->end ,buf->size,0);
    if(rc == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
        	HXS_DEBUG_MSG("Recv waits...\n");
            return HXS_WAIT;
        }

        fprintf(HXS_ERROR_STREAM,
                "%s :  %d : Failed to recv with error %s...\n"
                ,__FILE__,__LINE__, strerror(errno));
        return HXS_ERROR;


    }else if(rc == 0){
        HXS_DEBUG_MSG("Peer closed the connection...\n");
        return HXS_CLIENT_CLOSED;

    }else if(rc > 0){
      buf->end = buf->end +rc;
      HXS_DEBUG_MSG("Received chunk...\n");
    }

    return HXS_OK;
}

int hxs_conn_send(hxs_connection_t* conn, hxs_buffer_t* buf){

	int rc;


	while(buf->read_pos != buf->end){
		rc = send(conn->socket, buf->read_pos, buf->end - buf->read_pos ,0);
		if(rc == -1){
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				HXS_DEBUG_MSG("Send waits...\n");
				return HXS_WAIT;
			}

			fprintf(HXS_ERROR_STREAM,
					"%s :  %d : Failed to recv with error %s...\n"
					,__FILE__,__LINE__, strerror(errno));
			return HXS_ERROR;


		}
		HXS_DEBUG_MSG("Sent chunk...\n");
		buf->read_pos += rc;
	}
    return HXS_OK;

}


int hxs_listener_accept(const hxs_listener_t* listener, hxs_connection_t* connptr){



        socklen_t len = sizeof((connptr->clientaddr));
		connptr->socket = accept(listener->socket,
				                    ((sockaddr_t*) &(connptr->clientaddr)),
				                     &len);

		if (connptr->socket == HXS_INVALID_SOCKET) {

			if(errno == EAGAIN || errno == EWOULDBLOCK){
				return HXS_OK;
			}

			fprintf(HXS_ERROR_STREAM,
				"Failed to accept a socket with error: %s\n",
				strerror(errno));

			return HXS_ERROR;
		}
		HXS_DEBUG_MSG("New conn accepted...\n");

		if (enable_non_blocking(connptr->socket) == HXS_ERROR) {
			return HXS_ERROR;
		}

		connptr->clientaddr_size = len;

	return HXS_OK;

}

int hxs_listener_open(hxs_listener_t* listener, const char* port, int afamily,
		int backlog) {

	listener->socket = get_bound_socket(port, afamily, &(listener->hostaddr),
			&(listener->hostaddr_size));

	if (listener->socket == HXS_INVALID_SOCKET) {
		fprintf(HXS_ERROR_STREAM, "Failed to get a valid socket...\n");
		return HXS_ERROR;
	} else if (enable_non_blocking(listener->socket) == HXS_ERROR) {
		fprintf(HXS_ERROR_STREAM,
				"Failed to set the socket to non-blocking mode...\n");
		hxs_closesocket(listener->socket);
		return HXS_ERROR;
	} else if (listen(listener->socket, backlog) == -1) {
		fprintf(HXS_ERROR_STREAM, "Failed to listen to the socket...\n");
		hxs_closesocket(listener->socket);
		return HXS_ERROR;
	}


	return HXS_OK;

}

int hxs_listener_close(hxs_listener_t* listener) {

	hxs_closesocket(listener->socket);
	return HXS_OK;
}

static hxs_socket_t get_bound_socket(const char* port, int afamily,
		sockaddr_storage_t* addr, socklen_t* len) {

	struct addrinfo * res = NULL;
	struct addrinfo hints;
	int yes = 1; //used in the setsockopt()-call
	int rc; //used to get gai_strerror
	hxs_socket_t sfd = HXS_INVALID_SOCKET;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = afamily;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rc = getaddrinfo(NULL, port, &hints, &res)) != 0) {

		fprintf(HXS_ERROR_STREAM, "Failed to getaddrinfo with error: %s\n",
				gai_strerror(rc));
		return sfd;

	}

	/*don't return before releasing the allocated resources*/

	for (struct addrinfo* iter = res; iter != NULL; iter = iter->ai_next) {

		//get the descriptor
		sfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (sfd == HXS_INVALID_SOCKET) {
			fprintf(HXS_ERROR_STREAM,
					"Failed to get a descriptor with error: %s\n",
					strerror(errno));
			continue;
		}
		HXS_DEBUG_MSG("Got a socket...\n");

		//setsockopt
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
				== -1) {
			fprintf(HXS_ERROR_STREAM, "Failed to setsockopt with error: %s\n",
					strerror(errno));
			hxs_closesocket(sfd);
			sfd = HXS_INVALID_SOCKET;
			break;
		}
		HXS_DEBUG_MSG("Set sockopt...\n");

		//bind it
		if (bind(sfd, iter->ai_addr, iter->ai_addrlen) == -1) {
			fprintf(HXS_ERROR_STREAM,
					"Failed to bind the descriptor with error: %s\n",
					strerror(errno));
			hxs_closesocket(sfd);
			sfd = HXS_INVALID_SOCKET;
			continue;
		}
		HXS_DEBUG_MSG("Bound the socket...\n");

		//assign the addr
		switch (iter->ai_addrlen) {

		case sizeof(sockaddr_in6_t):
			*((sockaddr_in6_t*) addr) = *((sockaddr_in6_t*) iter->ai_addr);
			break;
		case sizeof(sockaddr_in_t):
			*((sockaddr_in_t*) addr) = *((sockaddr_in_t*) iter->ai_addr);
			break;
		default:
			fprintf(HXS_ERROR_STREAM,
					"Neither sockin4 nor sockin6 lengths re correct...\nEXIT\n");
			hxs_closesocket(sfd);
			exit(EXIT_FAILURE);
		}
		*len = iter->ai_addrlen;
		break;
	}

	freeaddrinfo(res);

	return sfd;
}

static int enable_non_blocking(hxs_socket_t socket) {

	int flags;

	flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) {
		fprintf(HXS_ERROR_STREAM, "Failed to GET fcntl-flags with error: %s\n",
				strerror(errno));
		return HXS_ERROR;
	}

	flags |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flags) == -1) {
		fprintf(HXS_ERROR_STREAM, "Failed to SET fcntl-flags with error: %s\n",
				strerror(errno));
		return HXS_ERROR;
	}

	return HXS_OK;

}
