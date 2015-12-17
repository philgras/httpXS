/*
 * hxs_list.h
 *
 *  Created on: 28.11.2015
 *      Author: philip
 */

#ifndef HXS_LIST_H_
#define HXS_LIST_H_
#include <stddef.h>
#include <assert.h>

typedef struct{

	void* head;
	void* tail;
	size_t size;


}hxs_list_t;

#define HXS_LIST_T(type,element) ((type*)(element))

#define HXS_LIST_INIT(l) 		(l)->head = NULL;																								\
								(l)->tail = NULL;																								\
								(l)->size = 0;

#define HXS_LIST_ADD(l, t , el)																													\
								assert(el != NULL);																								\
								if((l)->head != NULL){HXS_LIST_T(t, (l)->head)->prev = (el);	}												\
								if((l)->tail == NULL){(l)->tail = (el);	}																		\
								(el)->next = (l)->head;																							\
								(el)->prev = NULL;																								\
								(l)->head = (el);																								\
								++((l)->size);


#define HXS_LIST_REMOVE(l, t , el)																												\
								assert(NULL != (l)->head && (l)->tail != NULL);																	\
								if((el) != (l)->tail && (el) != (l)->head ){ 																	\
									(el)->prev->next = (el)->next; (el)->next->prev  = (el)->prev;												\
								}																												\
								else{ 																											\
									if((el) == (l)->tail){ 																						\
										(l)->tail = (el)->prev; 																				\
										if((l)->tail)HXS_LIST_T(t, (l)->tail)->next = NULL;														\
									}																											\
									if((el) == (l)->head){																						\
										(l)->head = (el)->next;																					\
										if((l)->head)HXS_LIST_T(t, (l)->head)->prev = NULL;														\
									}																											\
								}																												\
								--((l)->size);



#define HXS_LIST_FOREACH(l, t, func)																											\
								for(void* iter = (l)->head; iter != NULL; iter = HXS_LIST_T(t, iter)->next)										\
									(func)(HXS_LIST_T(t, iter))																					\



#endif /* HXS_LIST_H_ */
