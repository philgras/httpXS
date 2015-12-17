/*
 * hxs_connl_list_test.c
 *
 *  Created on: 22.11.2015
 *      Author: philgras
 */
#include "../hxs_std.h"
#include "../hxs_connection.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

size_t iterate(hxs_conn_list_t l){
	assert(l != NULL);

	size_t n = 0;
	hxs_connection_t * conn = l;
	while(conn!= NULL){
		++n;
		conn = conn->next;
		if(conn == l){
			break;
		}
	}

	return n;

}


int main(void){

	const size_t num_of_conns = 1;
	const size_t num_of_conns_other = 0;
	hxs_conn_list_t clist = NULL;
	hxs_conn_list_t clist_other = NULL;
	hxs_connection_t conn;

	//set a value
	conn.socket = HXS_INVALID_SOCKET;

	//test add
	for(int i = 0; i < num_of_conns;++i){
		if(!hxs_conn_list_add(&clist,&conn)){
			printf("Failed to add a connection to the list...\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < num_of_conns_other;++i){
		if(!hxs_conn_list_add(&clist_other,&conn)){
			printf("Failed to add a connection to the list...\n");
			exit(EXIT_FAILURE);
		}
	}
	printf("Added everything successfully...\n");


	//testing merge
	hxs_conn_list_merge(clist,&clist_other);
	if(iterate(clist) != num_of_conns + num_of_conns_other){
		printf("Failed to merge...\n");
		exit(EXIT_FAILURE);
	}
	printf("Merged successfully...\n");

	hxs_conn_list_clear(&clist);

	return EXIT_SUCCESS;
}
