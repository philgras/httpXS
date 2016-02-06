/*
 * hxs_tcpdispatcher.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_SERVICE_H_
#define HXS_SERVICE_H_

#include "hxs_connection.h"
#include "hxs_map.h"
#include <stdint.h>
#define HXS_SERVICE_NAME_MAX_LENGTH 40
#define HXS_MAX_ROOT_PATH_LENGTH 128
#define HXS_SERVICE_MAX_DESC_LENGTH 256
#define HXS_MAX_PORT_DIGITS 5

typedef unsigned char hxs_restriction;

struct hxs_service_s{
    /*conns*/
    hxs_list_t conns;


    hxs_listener_t listener;

    char name [HXS_SERVICE_NAME_MAX_LENGTH+1];
    char port [HXS_MAX_PORT_DIGITS+1];
    char root_path [HXS_MAX_ROOT_PATH_LENGTH+1];
    char description[HXS_SERVICE_MAX_DESC_LENGTH+1];
    hxs_map_t module_restriction_map;

    /*epoll*/
    hxs_epollfd_t efd;

    unsigned int active;
} ;

typedef struct hxs_service_s hxs_service_t;



int hxs_service_init( hxs_service_t* service, const char* name, const char* port, const char* root_path,
											  const char* description, unsigned int active);

int hxs_service_start(hxs_service_t* service);

/*int hxs_service_add_module();*/


void hxs_service_remove_conn(hxs_service_t* service, hxs_connection_t* conn);

int hxs_service_cleanup(hxs_service_t* service);

#endif /* HXS_SERVICE_H_ */
