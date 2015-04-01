// Copyright 2015 Canopy Services, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _CANOPY_HTTP_H_
#define _CANOPY_HTTP_H_

#include	<canopy_min.h>


/*
 * Performs an HTTP POST request to the remote.
 *
 * 	<url>		URL to send to
 * 	<payload>	Payload to deliver
 *
 * 	<barrier>	Non-null get's used for syncronizing with the remote.  When
 * 	NULL, the call blocks.
 *
 * 	NOTE:	The memory that the response gets put into is the rcv_buffer that
 * 	was initialzed in the call to canopy_remote_init().
 */
canopy_error canopy_http_post(
        struct canopy_remote	*remote,
        const char 				*url,
        const char 				*payload,
		struct canopy_barrier	*barrier);



#endif /* _CANOPY_HTTP_H_ */
