/*
 * hxs_map.h
 *
 *  Created on: 24.01.2016
 *      Author: philgras
 */

#ifndef HXS_MAP_H_
#define HXS_MAP_H_

#include "hxs_list.h"
#include "hxs_std.h"


typedef struct {

	void* data;
	char* key;
	hxs_map_element_t* prev;
	hxs_map_element_t* next;

}hxs_map_element_t;

typedef struct {
	hxs_list_t list;
	unsigned int error_flag:1;
}hxs_map_t;

#define HXS_MAP_INIT(map)									\
			HXS_LIST_INIT(&((map)->list));					\
			(map)->error_flag = HXS_OK;

/**
 * Finds an element in the map
 *
 * @param	map is a pointer to hxs_map_t
 * @param	key is a const cstring
 * @param	dataptr is a pointer to a pointer to void. Note: you need to cast that pointer
 */
#define HXS_MAP_FIND(map,key,dataptr)													\
			*(dataptr) = NULL;															\
			for(hxs_map_element_t* iter = (hxs_map_element_t*)((map)->list.head); 		\
							iter != NULL; iter = (hxs_map_element_t*) (iter->next)){	\
					if(strcmp((key),iter->key) == 0){ *(dataptr) = iter; break;}		\
			}


/**
 * Adds an element to the map
 *
 * @param 	map is a pointer to hxs_map_t
 * @param 	key is a non-const cstring
 * @param	data is a pointer to void
 *
 * sets map's error flag to HXS_ERROR if the allocation fails or the element already exists
 */
#define HXS_MAP_ADD(map,key,data)											\
			hxs_map_element_t* el = malloc(sizeof(hxs_map_element_t));		\
			if(el != NULL){													\
				el->data = (data);											\
				el->key = (key);											\
				HXS_LIST_ADD(&((map)->list),hxs_map_element_t,el);			\
			}else{															\
				(map)->error_flag = HXS_ERROR;								\
			}

#define HXS_MAP_REMOVE(map,key)														\
		for(hxs_map_element_t* iter = (hxs_map_element_t*)((map)->list.head); 		\
						iter != NULL; iter = (hxs_map_element_t*) (iter->next)){	\
				if(strcmp((key),iter->key) == 0){ 									\
					HXS_LIST_REMOVE(&((map)->list),hxs_map_element_t,iter); break;}	\
		}



#endif /* HXS_MAP_H_ */
