/*
 * hxs_buffer.h
 *
 *  Created on: 13.12.2015
 *      Author: philip
 */

#ifndef HXS_BUFFER_H_
#define HXS_BUFFER_H_

#include "hxs_std.h"
#include <stdlib.h>
#include <string.h>


typedef struct{

    char* data;
    size_t size; /* size allocated */
    char* read_pos; /* last reading position*/
    char* end; /* points after the last element */

}hxs_buffer_t;

static
inline int hxs_buffer_init(hxs_buffer_t* buf, size_t size){

    buf->data = malloc(size);
    if(buf->data == NULL){
        buf->end = NULL;
        buf->read_pos = NULL;
        buf->size = 0;
        return HXS_ERROR;
    }

    buf->end = buf->data + size;
    buf->read_pos = buf->data;
    buf->size = size;
    return HXS_OK;

}

static
inline size_t hxs_buffer_length(hxs_buffer_t* buf){
    return (buf->end)-(buf->data);
}


static
inline void hxs_buffer_free(hxs_buffer_t* buf){
    free(buf->data);
    buf->data = NULL;
    buf->end = NULL;
    buf->read_pos = NULL;
    buf->size = 0;
}


#endif /* HXS_BUFFER_H_ */
