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
#include <errno.h>
#include <stdio.h>
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
        fprintf(HXS_ERROR_STREAM,"%s : %d: "
        		"Failed to allocate memory "
        		"with error %s\n",
				__FILE__,
				__LINE__,
				strerror(errno));
    	buf->end = NULL;
        buf->read_pos = NULL;
        buf->size = 0;
        return HXS_ERROR;
    }
    HXS_DEBUG_MSG("**********************ALLOCATE BUFFER**************************\n");
    buf->end = buf->data;
    buf->read_pos = buf->data;
    buf->size = size;
    return HXS_OK;

}

static
inline void hxs_buffer_free(hxs_buffer_t* buf){
	if(buf->data != NULL){
		HXS_DEBUG_MSG("**********************FREE BUFFER**************************\n");
		free(buf->data);
		buf->data = NULL;
		buf->end = NULL;
		buf->read_pos = NULL;
		buf->size = 0;
	}
}

static
inline size_t hxs_buffer_used(hxs_buffer_t* buf){
    return (buf->end)-(buf->data);
}

static
inline int hxs_buffer_add_memory(hxs_buffer_t* buf, size_t bytes){

	size_t readpos = buf->read_pos - buf->data;
	size_t endpos = buf->end - buf->data;

	buf->data = realloc(buf->data,buf->size+ bytes);
	if(buf->data == NULL){
		fprintf(HXS_ERROR_STREAM,"%s : %d: "
		        		"Failed to allocate memory "
		        		"with error %s\n",
						__FILE__,
						__LINE__,
						strerror(errno));
		buf->read_pos = NULL;
		buf->end = NULL;
		buf->size+= 0;
		return HXS_ERROR;
	}

	buf->read_pos = buf->data+readpos;
	buf->end = buf->data+endpos;
	buf->size+= bytes;

	return HXS_OK;
}

static
inline int hxs_buffer_init_with_file_data(hxs_buffer_t* buf,const char* filename){

	size_t read_bytes = 0;
	size_t file_length = 0;
	int rc = HXS_OK;

	FILE* file = fopen(filename,"r");
	if(file == NULL){
		int error_code = errno;
		fprintf(HXS_ERROR_STREAM,"%s : %d: "
				        		"Failed to open file "
				        		"with error %s\n",
								__FILE__,
								__LINE__,
								strerror(error_code));

		if(error_code == ENOENT){
			return HXS_FILE_NOT_FOUND;
		}

		return HXS_ERROR;
	}

	fseek(file, 0L, SEEK_END);
	file_length = ftell(file);
	fseek(file, 0L, SEEK_SET);

	if(hxs_buffer_init(buf,file_length) == HXS_ERROR){
		/*fclose*/
		rc = HXS_ERROR;
		goto cleanup;
	}


	read_bytes = fread(buf->data,1,file_length,file);

	if(read_bytes != file_length){
		fprintf(HXS_ERROR_STREAM,"%s : %d: "
								"Failed to read file "
								"with error %s\n",
								__FILE__,
								__LINE__,
								strerror(errno));
		hxs_buffer_free(buf);
		rc = HXS_ERROR;

	}else{
		buf->end = buf->data + read_bytes;
	}

	cleanup:
	if(fclose(file) == -1){
		fprintf(HXS_ERROR_STREAM,"%s : %d: "
						"Failed to close file"
						"with error %s\n",
						__FILE__,
						__LINE__,
						strerror(errno));
	}

	return rc;


}




#endif /* HXS_BUFFER_H_ */
