/*
 * hxs_service_mod.h
 *
 *  Created on: 29.01.2016
 *      Author: philgras
 */

#ifndef HXS_SERVICE_MOD_H_
#define HXS_SERVICE_MOD_H_

#include "hxs_module.h"

const char* commands[] = {

			"createService"
			"deleteService"
			"changeService"



};

int handle_service_request(const char* command, hxs_buffer_t* data);

#endif /* HXS_SERVICE_MOD_H_ */
