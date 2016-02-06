/*
 * hxs_module.h
 *
 *  Created on: 29.01.2016
 *      Author: philgras
 */

#ifndef HXS_MODULE_H_
#define HXS_MODULE_H_

#define HXS_MAX_RESTRICTIONS 128

typedef unsigned char hxs_restriction_t;


struct hxs_module_s{

	const char* name;
	int (*execute)(const char* command, hxs_buffer_t* input, void * module_data);
	int (*init) (void* module_data, const struct hxs_module_s* mod);
	int (*cleanup) (void* module_data);
} ;

typedef struct hxs_module_s hxs_module_t;

#endif /* HXS_MODULE_H_ */
