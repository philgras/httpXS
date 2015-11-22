#include "hxs_tcpdispatcher.h"
#include "hxs_std.h"
#include <sys/epoll.h>
#define HXS_MAXEVENTS 64


typedef int hxs_epollfd_t;

static struct {
	/*conns*/
	hxs_connection_t* conns;
	size_t n_conns;

	hxs_listener_t listener;

	/*epoll*/
	hxs_epollfd_t efd;

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

	int rc = HXS_OK;
	int n;
	struct epoll_event event, event_ary[HXS_MAXEVENTS];


	if(hxs_open_listener(&(tcp_dispatcher.listener)) == HXS_ERROR){
		//error
	}

	tcp_dispatcher.efd = epoll_create1(0);
	if(tcp_dispatcher.efd == -1){
		//error
	}

	event.data.fd = tcp_dispatcher.efd;
	event.events = EPOLLIN | EPOLLET;
	rc = epoll_ctl(tcp_dispatcher.efd, EPOLL_CTL_ADD, tcp_dispatcher.listener.socket, &event);

	if(rc == -1){
		//error
	}

	while(1){

		n = epoll(tcp_dispatcher.efd,event_ary,HXS_MAXEVENTS,-1);
		if(n == -1){
				//error
		}

		for(size_t i = 0; i < HXS_MAXEVENTS; ++i){
			if((event_ary[i].events & EPOLLERR )|| (event_ary[i].events & EPOLLHUP )){
				//error

			}else if(event_ary[i].data.fd == tcp_dispatcher.listener.socket){
				//accept the incoming stuff
			}else{

				handle_conn((hxs_connection_t*)(event_ary[i].data.ptr));

			}

		}

	}


	close(tcp_dispatcher.efd);
	hxs_close_listener(&(tcp_dispatcher.listener));

	return HXS_OK;

}
