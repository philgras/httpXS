/*
 * hxs_connl_list_test.c
 *
 *  Created on: 22.11.2015
 *      Author: philgras
 */
#include "../hxs_std.h"
#include "../hxs_connection.h"
#include <stdio.h>
#include <stdlib.h>

size_t iterate(hxs_conn_list_t l){
	assert(l != NULL);

	size_t n = 0;
	hxs_connection_t * conn = *l;
	while(conn!= NULL){
		conn = conn->next;
		++n;
	}

	return n;

}


int main(void){

	const num_of_conns = 5;
	const num_of_conns_other = 2;
	hxs_conn_list_t clist;
	hxs_conn_list_t clist_other;
	hxs_connection_t conn;

	//set a value
	conn.socket = HXS_INVALID_SOCKET;

	//test add
	for(int i = 0; i < num_of_conns;++i){
		if(!hxs_conn_list_add(clist,&conn)){
			printf("Failed to add a connection to the list...\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < num_of_conns_other;++i){
		if(!hxs_conn_list_add(clist_other,&conn)){
			printf("Failed to add a connection to the list...\n");
			exit(EXIT_FAILURE);
		}
	}
	printf("Added everything successfully...\n");


	//testing merge
	hxs_conn_list_merge(clist,clist_other);
	if(iterate(clist) != num_of_conns + num_of_conns_other){
		printf("Failed to merge...\n");
		exit(EXIT_FAILURE);
	}
	printf("Merged successfully...\n");

	hxs_conn_list_clear(clist);

	return EXIT_SUCCESS;
}
