#include "hxs_tcpdispatcher.h"
#include "hxs_std.h"
#include <pthread.h>

static struct {

	hxs_connection_t* main_conn;
	size_t n_conns;

	pthread_t* workers;
	size_t n_workers;

} tcp_dispatcher;


int hxs_tcpdisp_init(size_t allowed_conns){

	return HXS_OK;
}

int hxs_tcpdips_conf_monitor(uint16_t port,
							const char* monitor_ip,
							hxs_tcp_callback_t handle_monitor){


	return HXS_OK;
}

int hxs_tcpdisp_handle_conns(uint16_t port, hxs_tcp_callback_t handle_conn){


	return HXS_OK;

}
