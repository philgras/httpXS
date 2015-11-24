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

int hxs_tcpdisp_handle_conns(uint16_t port, hxs_tcp_callback_t handle_conn);

int hxs_tcpdisp_cleanup(void);

#endif /* HXS_TCPDISPATCHER_H_ */
