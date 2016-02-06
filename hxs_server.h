/*
 * hxs_server.h
 *
 *  Created on: 04.02.2016
 *      Author: philgras
 */

#ifndef HXS_SERVER_H_
#define HXS_SERVER_H_


#include "hxs_service.h"

int hxs_server_start();

int hxs_server_add_service();

int hxs_server_shut_down();

#endif /* HXS_SERVER_H_ */
