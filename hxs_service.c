#include "hxs_http.h"
#include "hxs_list.h"
#include "hxs_std.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include "hxs_service.h"
#define HXS_MAXEVENTS 64
#define HXS_BACKLOG 20
#define TO_CONN(ptr) (hxs_connection_t*)(ptr)



static hxs_connection_t* add_connection(hxs_service_t* disp, hxs_connection_t* conn){

    struct epoll_event event;

    hxs_connection_t* new_conn = malloc(sizeof(hxs_connection_t));

    if(new_conn == NULL){
        fprintf(HXS_ERROR_STREAM,
                "%s : %d : Failed to malloc "
                "with error: %s\n",
                __FILE__,
                __LINE__,strerror(errno));
        return NULL;
    }
    /*copy*/
    *new_conn = *conn;


    event.data.fd = new_conn->socket;
    event.data.ptr = new_conn;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;

    if (epoll_ctl(disp->efd, EPOLL_CTL_ADD,
            new_conn->socket, &event) == -1) {

        fprintf(HXS_ERROR_STREAM,
                "%s : %d : Failed to add to epoll-fd "
                "with error: %s\n",
                __FILE__,
                __LINE__,strerror(errno));

        return NULL;
    }

    new_conn->service = disp;
    HXS_LIST_ADD(&(disp->conns),hxs_connection_t,new_conn);

    return new_conn;

}

void hxs_service_remove_conn(hxs_service_t* disp, hxs_connection_t* conn){
    HXS_LIST_REMOVE(&(disp->conns),hxs_connection_t,conn);
    hxs_closesocket(conn->socket);
    free(conn);
    HXS_DEBUG_MSG("Freed connection and closed socket...\n");

}



int hxs_service_init(hxs_service_t* service,const char* name, const char* port, const char* root_path,
											const char* description, unsigned int active) {

	struct epoll_event event;

	/*
	 * validate input strings
	 */
	if(strlen(root_path)>HXS_MAX_ROOT_PATH_LENGTH){
		return HXS_ERROR;
	}
	strcpy(service->root_path,root_path);

	if(strlen(port)>HXS_MAX_PORT_DIGITS){
				return HXS_ERROR;
	}
	strcpy(service->port,port);

	if(strlen(name)>HXS_SERVICE_NAME_MAX_LENGTH){
			return HXS_ERROR;
	}
	strcpy(service->name,name);

	if(strlen(description)>HXS_SERVICE_MAX_DESC_LENGTH){
				return HXS_ERROR;
	}
	strcpy(service->description,description);



	/*initialize sockets*/
	if (hxs_listener_open(&(service->listener), port, AF_INET, HXS_BACKLOG)
			== HXS_ERROR) {
		return HXS_ERROR;
	}

	service->efd = epoll_create1(0);
	if (service->efd == -1) {
		fprintf(HXS_ERROR_STREAM, "Failed to create the epoll-descriptor "
				"with error: %s\n", strerror(errno));

		hxs_listener_close(&(service->listener));
		return HXS_ERROR;
	}

	event.data.fd = service->listener.socket;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(service->efd, EPOLL_CTL_ADD,
			service->listener.socket, &event) == -1) {

		fprintf(HXS_ERROR_STREAM,
				"Failed to add the listener to the epoll-descriptor "
						"with error: %s\n", strerror(errno));

		close(service->efd);
		hxs_listener_close(&(service->listener));

		return HXS_ERROR;
	}

	service->active = active;
	HXS_LIST_INIT(&(service->conns));
	HXS_MAP_INIT(&(service->module_restriction_map));

	return HXS_OK;
}

int hxs_service_start(hxs_service_t* service) {

	int n;
	int rc = HXS_OK;
	struct epoll_event e_ary[HXS_MAXEVENTS];

	while (1) { /* need a signal to stop*/
	    HXS_DEBUG_MSG("Start epoll loop...\n");

		n = epoll_wait(service->efd, e_ary, HXS_MAXEVENTS, -1);
		if (n == -1) {

			fprintf(HXS_ERROR_STREAM, "Failed to wait for the epoll-descriptor "
					"with error: %s\n", strerror(errno));

			rc = HXS_ERROR;
			break;
		}

		for (size_t i = 0; i < n; ++i) {

			if ((e_ary[i].events & EPOLLERR)
					|| (e_ary[i].events & EPOLLHUP)) {

				hxs_connection_t* conn = TO_CONN(e_ary[i].data.ptr);

				fprintf(HXS_ERROR_STREAM, "Internal epoll-error on the connection.\n");
				/*raise error event*/
				conn->status =	HXS_CONN_ERROR;
				hxs_handle_http_connection(conn);

				continue;

			} else if (e_ary[i].data.fd == service->listener.socket) {

			    HXS_DEBUG_MSG("Accepting conns...\n");

			    hxs_connection_t conn, *new_conn;

			    while(hxs_listener_accept(&(service->listener),&conn) != HXS_ERROR){
			        if(conn.socket == HXS_INVALID_SOCKET){
			            HXS_DEBUG_MSG("Nothing to accept...\n");
			            break;
			        }

			        if((new_conn = add_connection(service,&conn)) == NULL){
			            HXS_DEBUG_MSG("Failed to add connection...\n");
			            rc = HXS_ERROR;
			            goto cleanup;
			        }
			        HXS_DEBUG_MSG("Added connection...\n");
			        new_conn->status = HXS_CONN_INIT;
			        hxs_handle_http_connection(new_conn);
			    }

				continue;

			} else {

			    hxs_handle_http_connection(e_ary[i].data.ptr);

			}
		}
	}

	cleanup:
	HXS_DEBUG_MSG("Doing cleanup yeah...\n");
	hxs_connection_t* iter = service->conns.head,* conn;

	while(iter != NULL){
	    conn = iter;
	    iter = iter->next;

		conn->status = HXS_CONN_CLEANUP;
		hxs_handle_http_connection(conn);

		hxs_service_remove_conn(service, conn);
	}
	return rc;

}

int hxs_service_cleanup(hxs_service_t* service) {
	HXS_DEBUG_MSG("Cleaning up listener, epoll-fd...\n");
	HXS_MAP_CLEAR(&(service->module_restriction_map))
	close(service->efd);
	hxs_listener_close(&(service->listener));

	return HXS_OK;
}

