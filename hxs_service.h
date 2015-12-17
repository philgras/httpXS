/*
 * hxs_tcpdispatcher.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_SERVICE_H_
#define HXS_SERVICE_H_

#include "hxs_connection.h"
#include <stdint.h>





struct hxs_service_s{
    /*conns*/
    hxs_list_t conns;
    size_t allowed_conns;

    hxs_listener_t listener;

    /*epoll*/
    hxs_epollfd_t efd;

} ;

typedef struct hxs_service_s hxs_service_t;



int hxs_service_init( hxs_service_t* disp, const char* port, int family, int backlog, size_t allowed_conns);

int hxs_service_start(hxs_service_t* disp);

void hxs_service_remove_conn(hxs_service_t* disp, hxs_connection_t* conn);

int hxs_service_cleanup(hxs_service_t* disp);

#endif /* HXS_SERVICE_H_ */
