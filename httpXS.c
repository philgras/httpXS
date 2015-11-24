#include "hxs_tcpdispatcher.h"
#include <stdio.h>


int main(void){
	
	printf("httpXS...\n");
	hxs_tcpdisp_init();
	hxs_tcpdisp_handle_conns(33,NULL);
	hxs_tcpdisp_cleanup();


	return 0;

}
