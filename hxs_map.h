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
#include <stdlib.h>
#include <string.h>


struct hxs_map_element_s{

	void* data;
	char* keystr;
	struct hxs_map_element_s* prev;
	struct hxs_map_element_s* next;

};
typedef struct hxs_map_element_s hxs_map_element_t;

typedef struct {
	hxs_list_t list;
	unsigned int error_flag:1;
}hxs_map_t;

#define HXS_MAP_INIT(map)						\
			HXS_LIST_INIT(&((map)->list));		\
			(map)->error_flag = HXS_OK;

/**
 * Finds an element in the map
 *
 * @param	map is a pointer to hxs_map_t
 * @param	key is a const cstring
 * @param	dataptrptr is a pointer to a pointer to void. Note: you need to cast that pointer
 * @param	type is the type of the data object
 */
#define HXS_MAP_FIND(map,key,dataptrptr, type)																	\
			*(dataptrptr) = NULL;																				\
			for(hxs_map_element_t* iter = (hxs_map_element_t*)((map)->list.head); 								\
							iter != NULL; iter = (hxs_map_element_t*) (iter->next)){							\
					if(strcmp((key),iter->keystr) == 0){ *(dataptrptr) = (const type*) (iter->data); break;}	\
			}




/**
 * Adds an element to the map
 *
 * @param 	map is a pointer to hxs_map_t
 * @param 	key is a const cstring
 * @param	dataptr is a pointer to void
 * @param	size is the size of the data object
 *
 * sets map's error flag to HXS_ERROR if the allocation fails
 */
#define HXS_MAP_ADD(map,key,dataptr,size)										\
				{																\
				hxs_map_element_t* el = malloc(sizeof(hxs_map_element_t));		\
				if(el != NULL){													\
					el->data = malloc(size);									\
					if(el->data != NULL){										\
						el->keystr = malloc(strlen(key)+1);						\
						if(el->keystr != NULL){									\
							memcpy(el->data, dataptr, size);					\
							strcpy(el->keystr,key);								\
							HXS_LIST_ADD(&((map)->list),hxs_map_element_t,el);	\
						}else{													\
							free(el->data);										\
							free(el);											\
							(map)->error_flag = HXS_ERROR;						\
						}														\
					}else{														\
						free(el);												\
						(map)->error_flag = HXS_ERROR;							\
					}															\
				}else{															\
					(map)->error_flag = HXS_ERROR;								\
				}}



#define HXS_MAP_REMOVE(map,key)															\
		for(hxs_map_element_t* iter = (hxs_map_element_t*)((map)->list.head), * next;	\
						iter != NULL; iter = (hxs_map_element_t*) next){				\
				next = iter->next;														\
				if(strcmp((key),iter->keystr) == 0){									\
					HXS_LIST_REMOVE(&((map)->list),hxs_map_element_t,iter);				\
					free(iter->keystr);													\
					free(iter->data);													\
					free(iter);															\
					break;																\
				}																		\
		}																				\

#define HXS_MAP_CLEAR(map)																\
		for(hxs_map_element_t* iter = (hxs_map_element_t*)((map)->list.head), * next;	\
								iter != NULL; iter = (hxs_map_element_t*) next){		\
						next = iter->next;												\
						HXS_LIST_REMOVE(&((map)->list),hxs_map_element_t,iter);			\
						free(iter->keystr);												\
						free(iter->data);												\
						free(iter);														\
																						\
				}



#endif /* HXS_MAP_H_ */
