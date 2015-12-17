/*
 * hxs_std.h
 *
 *  Created on: 16.11.2015
 *      Author: philgras
 */

#ifndef HXS_STD_H_
#define HXS_STD_H_

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

/*TRACE STREAMS*/
#define HXS_DEBUG_STREAM stdout
#define HXS_DEBUG_MSG(...) fprintf(HXS_DEBUG_STREAM,__VA_ARGS__)
#define HXS_INFO_STREAM stdout
#define HXS_ERROR_STREAM stderr

/*SOCKET DEFINES*/
#define hxs_socket_t int
#define HXS_INVALID_SOCKET -1
#define hxs_closesocket(s) if(close(s) == -1) \
			fprintf(HXS_ERROR_STREAM,"Failed to close socket with error: %s\n", strerror(errno))

typedef struct sockaddr_in sockaddr_in_t ;
typedef struct sockaddr_in6 sockaddr_in6_t ;
typedef struct sockaddr sockaddr_t ;
typedef struct sockaddr_storage sockaddr_storage_t;


typedef int hxs_epollfd_t;

enum hxs_errorcode{
	HXS_OK = 0,
	HXS_ERROR = -1
};



#endif /* HXS_STD_H_ */
