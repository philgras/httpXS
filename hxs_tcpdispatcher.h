/*
 * hxs_tcpdispatcher.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_TCPDISPATCHER_H_
#define HXS_TCPDISPATCHER_H_

#include "hxs_connection.h"
#include <stdint.h>

typedef int (*hxs_tcp_callback_t)(hxs_connection_t* conn);

int hxs_tcpdisp_init(size_t allowed_conns);

int hxs_tcpdips_conf_monitor(uint16_t port,
							const char* monitor_ip,
							hxs_tcp_callback_t handle_monitor);

int hxs_tcpdisp_handle_conns(uint16_t port, hxs_tcp_callback_t handle_conn);



#endif /* HXS_TCPDISPATCHER_H_ */
