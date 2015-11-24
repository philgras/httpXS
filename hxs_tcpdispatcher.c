#include "hxs_tcpdispatcher.h"
#include "hxs_std.h"
#include <sys/epoll.h>
#define HXS_MAXEVENTS 64


typedef int hxs_epollfd_t;

static struct {
	/*conns*/
	hxs_conn_list_t conns;
	size_t allowed_conns;

	hxs_listener_t listener;

	/*epoll*/
	hxs_epollfd_t efd;

} tcp_dispatcher;


int hxs_tcpdisp_init( uint16_t port, size_t allowed_conns){

	struct epoll_event event;


	if(hxs_open_listener(&(tcp_dispatcher.listener)) == HXS_ERROR){
		//error
	}

	tcp_dispatcher.efd = epoll_create1(0);
	if(tcp_dispatcher.efd == -1){
		//error
	}

	event.data.fd = tcp_dispatcher.efd;
	event.events = EPOLLIN | EPOLLET;

	if( epoll_ctl(tcp_dispatcher.efd, EPOLL_CTL_ADD,
			tcp_dispatcher.listener.socket, &event) == -1){
		//error
	}

	tcp_dispatcher.allowed_conns = allowed_conns;

	return HXS_OK;
}


int hxs_tcpdisp_handle_conns(hxs_tcp_callback_t handle_conn){

	int n;
	struct epoll_event event, event_ary[HXS_MAXEVENTS];

	while(1){

		n = epoll(tcp_dispatcher.efd,event_ary,HXS_MAXEVENTS,-1);
		if(n == -1){
				/*error*/
		}

		for(size_t i = 0; i < n; ++i){
			if((event_ary[i].events & EPOLLERR )|| (event_ary[i].events & EPOLLHUP )){
				/*error*/

			}else if(event_ary[i].data.fd == tcp_dispatcher.listener.socket){
				/*accept the incoming stuff*/
				hxs_conn_list_t new_conns = hxs_get_new_conns(&(tcp_dispatcher.listener));
				hxs_connection_t * conn ;

				if(new_conns == NULL){
					/*error*/


					return HXS_ERROR;
				}

				conn = new_conns;
				do{
					event.data.fd = conn->socket;
					event.data.ptr = conn;
					event.events = EPOLLIN | EPOLLOUT | EPOLLET;

					if( epoll_ctl(tcp_dispatcher.efd, EPOLL_CTL_ADD, conn->socket, &event) == -1){

						return HXS_ERROR;
					}

					handle_conn(conn);
				}while((conn = conn.next) != new_conns);

				hxs_conn_list_merge(tcp_dispatcher.conns, &new_conns);

				continue;

			}else{

				handle_conn((hxs_connection_t*)(event_ary[i].data.ptr));

			}

		}

	}

	return HXS_OK;

}

int hxs_tcpdisp_cleanup(void){

	hxs_conn_list_clear(&(tcp_dispatcher.conns));
	close(tcp_dispatcher.efd);
	hxs_close_listener(&(tcp_dispatcher.listener));

	return HXS_OK;
}


