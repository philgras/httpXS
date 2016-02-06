/*
 * hxs_server.c
 *
 *  Created on: 04.02.2016
 *      Author: philgras
 */

#include "hxs_server.h"
#include "hxs_module.h"
#include "hxs_map.h"
#include "hxs_std.h"
#include "hxs_buffer.h"
#include <jansson.h>

#define HXS_SERVICES_JSON_FILE "services.json"

#define HXS_JSON_SERVICE_NAME "name"
#define HXS_JSON_SERVICE_PORT "port"
#define HXS_JSON_SERVICE_ROOT "rootpath"
#define HXS_JSON_SERVICE_ACTIVE "active"
#define HXS_JSON_SERVICE_DESC "description"
#define HXS_JSON_SERVICE_MODS "modules"
#define HXS_JSON_MOD_NAME "name"
#define HXS_JSON_MOD_RESTRICTIONS "restrictions"
#define HXS_JSON_RESTRICTION_NAME "name"
#define HXS_JSON_RESTRICTION_VALUE "value"

static struct {

	hxs_map_t service_map;
	hxs_map_t module_meta_map;

}hxs_server;

static int hxs_server_load_services_map();

/**
 * TODO check if port is already in use!!!
 */
static int hxs_server_load_services_map(){


	json_error_t error;
	json_t* services_ary;

	services_ary = json_load_file(HXS_SERVICES_JSON_FILE,0,&error);

	if(!services_ary){
		fprintf(HXS_ERROR_STREAM,
				 "Failed to parse " HXS_SERVICES_JSON_FILE " on line %d: %s\n",
				 error.line, error.text);
		return HXS_ERROR;
	}

	if(!json_is_array(services_ary))
	{
		fprintf(HXS_ERROR_STREAM,
				 "Content of " HXS_SERVICES_JSON_FILE " is unexpected.\n");
	    json_decref(services_ary);
	    return HXS_ERROR;
	}

	for(int i = 0; i < json_array_size(services_ary); i++)
	{
		json_t* service, *name, *root, *desc, *port, *active, *mods;
		hxs_service_t hxs_service;
		hxs_map_t map;
		HXS_MAP_INIT(&map);

		service = json_array_get(services_ary,i);
		if(!json_is_object(service)){
			goto cleanup;
		}

		name = json_object_get(service, HXS_JSON_SERVICE_NAME);
		root = json_object_get(service, HXS_JSON_SERVICE_ROOT);
		desc = json_object_get(service, HXS_JSON_SERVICE_DESC);
		port = json_object_get(service, HXS_JSON_SERVICE_PORT);
		active = json_object_get(service, HXS_JSON_SERVICE_ACTIVE);
		mods = json_object_get(service, HXS_JSON_SERVICE_MODS);

		if(! ( json_is_string(name) && json_is_string(root) && json_is_string(desc)
				&& json_is_string(port) && json_is_boolean(active) && json_is_array(mods) )){
			goto cleanup;
		}

		for(int j = 0; j < json_array_size(mods); j++){

			hxs_restriction_t hxs_restrictions[HXS_MAX_RESTRICTIONS];

			json_t* module_restriction, *name ,* restrictions;
			module_restriction = json_array_get(mods,j);
			if(!json_is_object(module_restriction)){
				goto cleanup;
			}

			name = json_object_get(module_restriction, HXS_JSON_MOD_NAME);
			restrictions = json_object_get(module_restriction, HXS_JSON_MOD_RESTRICTIONS);

			if(! (json_is_string(name) && json_is_array(restrictions) &&
					json_array_size(restrictions)<HXS_MAX_RESTRICTIONS)){
				goto cleanup;
			}

			for(int k = 0; k < json_array_size(restrictions); k++){

				json_t* module_restriction;
				int value;

				module_restriction = json_array_get(restrictions,k);
				if(!json_is_integer(module_restriction)){
					goto cleanup;
				}

				value = json_integer_value(module_restriction);
				if( value > 256 || value < 0){
					goto cleanup;
				}

				hxs_restrictions[k] = value;
			}

			HXS_MAP_ADD(&map,json_string_value(name),hxs_restrictions,HXS_MAX_RESTRICTIONS);

		}


		if( hxs_service_init(&hxs_service,json_string_value(name), json_string_value(port),
			json_string_value(root), json_string_value(desc), json_boolean_value(active)) != HXS_ERROR ){

			HXS_MAP_MOVE(&(hxs_service.module_restriction_map), &map);
			HXS_MAP_ADD(&(hxs_server.service_map), hxs_service.name, &hxs_service, sizeof(hxs_service));

		}else{
			HXS_MAP_CLEAR(&(map));
		}

		continue;

		cleanup:
		fprintf(HXS_ERROR_STREAM,
				"Content of service number %d in " HXS_SERVICES_JSON_FILE " is unexpected.\n", i);
		HXS_MAP_CLEAR(&(map));
	}

	json_decref(services_ary);
	return HXS_OK;
}


int hxs_server_start(){

	HXS_MAP_INIT(&(hxs_server.service_map));
	HXS_MAP_INIT(&(hxs_server.module_meta_map));

	hxs_server_load_services_map();


	return HXS_OK;

}

int hxs_server_shut_down(){


	return HXS_OK;
}
