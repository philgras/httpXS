#include <stdio.h>
#include <stdlib.h>

#include "hxs_service.h"
#define PORT "3333"
#define BUFMAX 2048
#define FAMILY AF_INET
#define MAX_CONN 10


int main(void){
	
    hxs_service_t disp;

	printf("httpXS...\n");
	if(hxs_service_init(&disp,PORT,FAMILY,MAX_CONN,MAX_CONN) == HXS_OK){
		hxs_service_start(&disp);
		hxs_service_cleanup(&disp);
	}



	return 0;

}
