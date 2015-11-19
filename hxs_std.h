/*
 * hxs_std.h
 *
 *  Created on: 16.11.2015
 *      Author: philip
 */

#ifndef HXS_STD_H_
#define HXS_STD_H_


#define hxs_socket int
#define hxs_closesocket(s) close(s)



enum hxs_errorcode{
	HXS_OK = 0,
	HXS_ERROR = -1,
};



#endif /* HXS_STD_H_ */
