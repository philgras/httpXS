/*
 * hxs_map_test.c
 *
 *  Created on: 24.01.2016
 *      Author: philgras
 */


#include "../hxs_map.h"
#include <stdio.h>
#include <assert.h>

void int_test();
void str_test();

int main(void){

	str_test();
	int_test();


}

void str_test(){
	hxs_map_t map;
		HXS_MAP_INIT(&map);
		const char* value_ary [] = {"value1", "value2", "value3"};
		const char* result;
		const char* key_ary [] = {"key1","key2","key3"};

		for(int i = 0; i < sizeof(value_ary)/sizeof(const char*); ++i){
			HXS_MAP_ADD(&map, key_ary[i], value_ary[i], strlen(value_ary[i]));
		}

		for(int i = 0; i < sizeof(value_ary)/sizeof(const char*); ++i){
			HXS_MAP_FIND(&map, key_ary[i], &result, char);
			if(result == NULL){
				printf("Failed to find...\n");
				exit(-1);
			}
			printf("Result: %s\n", result);
		}

		HXS_MAP_REMOVE(&map, key_ary[0]);

		HXS_MAP_FIND(&map, key_ary[0], &result, char);
		if(result != NULL){
			printf("Failed to delete or find...\n");
			exit(-1);
		}

		HXS_MAP_CLEAR(&map)

}

void int_test(){
	hxs_map_t map;
		HXS_MAP_INIT(&map);
		int value_ary [] = {1,2,3};
		const int* result;
		const char* key_ary [] = {"key1","key2","key3"};

		for(int i = 0; i < sizeof(value_ary)/sizeof(int); ++i){
			HXS_MAP_ADD(&map, key_ary[i], &(value_ary[i]), sizeof(int) );
		}

		for(int i = 0; i < sizeof(value_ary)/sizeof(int); ++i){
			HXS_MAP_FIND(&map, key_ary[i], &result, int);
			if(result == NULL){
				printf("Failed to find...\n");
				exit(-1);
			}
			printf("Result: %d\n", *result);
		}

		HXS_MAP_REMOVE(&map, key_ary[0]);

		HXS_MAP_FIND(&map, key_ary[0], &result, int);
		if(result != NULL){
			printf("Failed to delete or find...\n");
			exit(-1);
		}

		HXS_MAP_CLEAR(&map)

}

