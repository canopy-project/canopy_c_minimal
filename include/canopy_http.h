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

// Send an HTTP POST request.
// <url> is the URL to POST to.
// <payload> is the request body.
// <barrier>, if non-NULL, gets set to the address of a newly-allocated
// promise object that can be used to wait for completion of the request.
canopy_error canopy_http_post(
        struct canopy_remote *remote,
        const char *url,
        const char *payload,
		struct canopy_barrier *barrier);




#endif /* _CANOPY_HTTP_H_ */
