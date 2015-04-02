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

#include    <canopy_min.h>

typedef enum {
    CANOPY_HTTP_GET,
    CANOPY_HTTP_POST,
    CANOPY_HTTP_DELETE
} canopy_http_method;

/*
 * Performs an HTTP request.
 *
 *     <method>         HTTP method to perform (i.e. GET, POST, DELETE)
 *     <use_http>       Uses HTTP if true, HTTPS if false
 *     <skip_cert_check>Skips CA cert check if true.  This parameter is only
 *                      relevant if <use_http> is false (otherwise it is
 *                      ignored).
 *     <name>           Username provided using HTTP BASIC auth
 *     <password>       Password provided using HTTP BASIC auth
 *     <rcv_buffer>     Destination buffer
 *     <rcv_buffer_size>Size of destination buffer
 *     <rcv_end>        Pointer to integer that gets set to the end of the
 *                      recieved buffer.
 *     <status_code>    Pointer to integer that gets set to the HTTP status.
 *     <remote_name>    Hostname/IP of remote (makes up first part of URL)
 *     <api>            API endpoint and query params (makes up 2nd part of URL)
 *     <payload>        Optional payload to deliver, or NULL
 *     <barrier>        Non-null get's used for syncronizing with the remote.
 *                      When NULL, the call blocks.
 *
 *     NOTE: The memory that the response gets put into is the rcv_buffer that
 *     was initialzed in the call to canopy_remote_init().
 */
canopy_error canopy_http_perform(
        canopy_http_method      method,
        bool                    use_http,
        bool                    skip_cert_check,
        const char              *name,
        const char              *password,
        char                    *rcv_buffer,
        size_t                  rcv_buffer_size,
        int                     *rcv_end,
        int                     *status_code,
        const char              *remote_name,
        const char              *api,
        const char              *payload,
        struct canopy_barrier   *barrier);

/*
 * Performs an HTTP GET request using a canopy remote object.
 *
 *     <remote>     Remote server
 *     <api>        API endpoint and query params (ex: "/api/info")
 *     <payload>    Payload to deliver, or NULL
 *     <barrier>    Non-null get's used for syncronizing with the remote.  When
 *                  NULL, the call blocks.
 *
 *     NOTE: The memory that the response gets put into is the rcv_buffer that
 *     was initialzed in the call to canopy_remote_init().
 *
 * Equivlent to:
 *
 *  canopy_http_perform(
 *      CANOPY_HTTP_GET,
 *      remote->params->use_http,
 *      remote->params->name,
 *      remote->params->password,
 *      remote->rcv_buffer,
 *      remote->rcv_buffer_size,
 *      &remote->rcv_end,
 *      status_code,
 *      remote->remote_name,
 *      api,
 *      payload,
 *      barrier);
 */
canopy_error canopy_remote_http_get(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        int                     *status_code,
        struct canopy_barrier   *barrier);

/*
 * Performs an HTTP POST request to the remote.
 *
 *     <remote>     Remote server
 *     <api>        API endpoint and query params (ex: "/api/info")
 *     <payload>    Payload to deliver, or NULL
 *     <barrier>    Non-null get's used for syncronizing with the remote.  When
 *                  NULL, the call blocks.
 *
 *     NOTE:    The memory that the response gets put into is the rcv_buffer that
 *     was initialzed in the call to canopy_remote_init().
 */
canopy_error canopy_remote_http_post(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        int                     *status_code,
        struct canopy_barrier   *barrier);


/*
 * Performs an HTTP DELETE request to the remote.
 *
 *     <remote>     Remote server
 *     <api>        API endpoint and query params (ex: "/api/info")
 *     <payload>    Payload to deliver, or NULL
 *     <barrier>    Non-null get's used for syncronizing with the remote.  When
 *                  NULL, the call blocks.
 *
 *     NOTE:    The memory that the response gets put into is the rcv_buffer that
 *     was initialzed in the call to canopy_remote_init().
 */
canopy_error canopy_remote_http_delete(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        int                     *status_code,
        struct canopy_barrier   *barrier);

#endif /* _CANOPY_HTTP_H_ */
