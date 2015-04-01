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

#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

#include <canopy_min.h>
#include <canopy_os.h>
#include <canopy_communication.h>


struct private {
    char *buffer;     /* the buffr being built in */
    int  buffer_len;  /* how big is the raw buffer */
    int  offset;      /* offset is where writting should start from in buffer*/
};
//
// Handler for CURL write callback.  Concatenates chunks of data into the
// provided buffer.
static size_t _curl_write_handler(void *ptr, size_t size, size_t nmemb, 
                                  void *userdata) {

    struct private* http = (struct private*) userdata;
    size_t chunk_size = size * nmemb;

    // buffer_remaining is amount of space left in buffer (not leaving room for
    // NULL terminator).  When http->buffer_len == http->offset we have
    // overfilled our buffer.
    size_t buffer_remaining = http->buffer_len - http->offset;

    // len is number of bytes to copy
    size_t len = LOCAL_MIN(chunk_size, buffer_remaining);

    /*
     * Note..  There's no NULL character at the end of the transfered data.
     * If the payload fits in the provided buffer, we append a NULL character.
     * If the provided buffer is too small, then we fill the buffer (without
     * adding a NULL character) and signal an error.
     */
    memcpy((void*)&http->buffer[http->offset], ptr, len);
    http->offset += len;

    COS_ASSERT(http->buffer_len >= http->offset);
    if (http->buffer_len > http->offset) {
        // There's more room in our buffer.  We're good.
        http->buffer[http->offset + 1] = '\0';
    }
    return len;
}

/*****************************************************************************
 * canopy_remote_http_perform
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
        const char              *remote_name,
        const char              *api,
        const char              *payload,
        struct canopy_barrier   *barrier)
{
    canopy_error err = CANOPY_SUCCESS;
    CURL *curl = NULL;
    CURLcode res;
    char local_buf[256]; // TODO: big enough?
    char url[256]; // TODO: big enough?
    struct private private;
    private.buffer = rcv_buffer;
    private.buffer_len = rcv_buffer_size;
    private.offset = 0;

    if (barrier != NULL) {
        return CANOPY_ERROR_NOT_IMPLEMENTED;
    }
    cos_log(LOG_LEVEL_DEBUG, "Sending payload to %s%s:\n%s\n\n", remote_name, api, payload);

    curl = curl_easy_init();
    if (!curl) {
        cos_log(LOG_LEVEL_WARN, "Initialization of curl failed");
        err = CANOPY_ERROR_NETWORK;
        goto cleanup;
    }

    snprintf(local_buf, sizeof(local_buf), "%s:%s", name, password);
    snprintf(url, sizeof(url), "%s://%s%s", 
            (use_http ? "http" : "https"), remote_name, api);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (payload == NULL || strlen(payload) == 0) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
    } else {
        int len = strlen(payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
    }
    switch (method) {
        case CANOPY_HTTP_GET:
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            break;
        case CANOPY_HTTP_POST:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            break;
        case CANOPY_HTTP_DELETE:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        default:
            COS_ASSERT(!"Unsupported HTTP method");
    }
    if (skip_cert_check) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_handler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &private);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
    /* set user name and password for the authentication */
    curl_easy_setopt(curl, CURLOPT_USERPWD, local_buf);

    res = curl_easy_perform(curl);
    if (res == CURLE_WRITE_ERROR) {
        cos_log(LOG_LEVEL_WARN, "Buffer too small for payload\n");
        err = CANOPY_ERROR_OUT_OF_MEMORY;
        goto cleanup;
    } else if (res != CURLE_OK) {
        cos_log(LOG_LEVEL_WARN, "Transfer failed, res: %d\n", res);
        err = CANOPY_ERROR_NETWORK;
        goto cleanup;
    }
    *rcv_end = private.offset;

cleanup:
    curl_easy_cleanup(curl);
    return err;
}

/*****************************************************************************
 * canopy_remote_http_get
 */
canopy_error canopy_remote_http_get(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        struct canopy_barrier   *barrier)
{
    return canopy_http_perform(
            CANOPY_HTTP_GET,
            remote->params->use_http,
            remote->params->skip_cert_check,
            remote->params->name,
            remote->params->password,
            remote->rcv_buffer,
            remote->rcv_buffer_size,
            &remote->rcv_end,
            remote->params->remote,
            api,
            payload,
            barrier);
}

/*****************************************************************************
 * canopy_remote_http_post
 */
canopy_error canopy_remote_http_post(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        struct canopy_barrier   *barrier)
{
    return canopy_http_perform(
            CANOPY_HTTP_POST,
            remote->params->use_http,
            remote->params->skip_cert_check,
            remote->params->name,
            remote->params->password,
            remote->rcv_buffer,
            remote->rcv_buffer_size,
            &remote->rcv_end,
            remote->params->remote,
            api,
            payload,
            barrier);
}

/*****************************************************************************
 * canopy_remote_http_delete
 */
canopy_error canopy_remote_http_delete(
        struct canopy_remote    *remote,
        const char              *api,
        const char              *payload,
        struct canopy_barrier   *barrier)
{
    return canopy_http_perform(
            CANOPY_HTTP_DELETE,
            remote->params->use_http,
            remote->params->skip_cert_check,
            remote->params->name,
            remote->params->password,
            remote->rcv_buffer,
            remote->rcv_buffer_size,
            &remote->rcv_end,
            remote->params->remote,
            api,
            payload,
            barrier);
}
