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


#include	<stdint.h>
#include	<stdbool.h>
#include	<string.h>
#include    <stdlib.h>

#include	<canopy_min.h>
#include	<canopy_min_internal.h>
#include	<canopy_os.h>


/*****************************************************************************/

	/* statics go here */

/*****************************************************************************/

canopy_error canopy_cleanup_remote(canopy_remote_t *remote) {
	if (remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_cleanup_remote()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/*
 * memset(&params, 0, sizeof(params));
 * params.credential_type = CANOPY_DEVICE_CREDENTIALS;
 * params.name = TOASTER_UUID;
 * params.password = TOASTER_SECRET_KEY;
 * params.auth_type = CANOPY_BASIC_AUTH;
 * params.remote = REMOTE_ADDR;
 * params.use_ws = false;
 * params.persistent = false;
*/

/*
 * Initializes a new remote object.  Does not necessarily open a TCP or
 *  websocket connection to the server, but initializes an object with the
 *  parameters to do so when needed.
 *
 * 		 <ctx> is the context.
 *
 * 		<params> must be populated by the caller.  Sets all of the options for
 * subsequent connections to the server.
 *
 *      <buffer> is a pointer to storage that gets used as temporary data
 *      primarily as the buffer used for payload and response
 *      communication.  (http)
 *
 *      <buffer_size>	The size of the temporary buffer.
 *
 * 		<remote> is a remote object that is initialized by this call.
 *
 */
extern canopy_error canopy_remote_init(canopy_context_t *ctx,
        canopy_remote_params_t *params,
		char *rcv_buffer,
		size_t rcv_buffer_size,
        canopy_remote_t *remote) {

	if (ctx == NULL) {
		cos_log(LOG_LEVEL_FATAL, "ctx is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (rcv_buffer == NULL) {
		cos_log(LOG_LEVEL_FATAL, "rcv_buffer is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (params == NULL) {
		cos_log(LOG_LEVEL_FATAL, "params is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (params->name == NULL || params->password == NULL) {
		cos_log(LOG_LEVEL_FATAL, "name/password is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (params->remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_remote_init()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	/*
	 * Initialize the remote, and set the ports if not specified.  Append
	 * remote to the ctx.
	 */
	memset(remote, 0, sizeof(canopy_remote_t));
	remote->next = NULL;  /* Not really needed because of the memset() */
	if (params->http_port == 0) {
		params->http_port = 80;
	}
	if (params->https_port == 0) {
		params->https_port = 433;
	}
	remote->params = params;
	remote->ctx = ctx;
	remote->rcv_buffer = rcv_buffer;
	remote->rcv_buffer_size = rcv_buffer_size;
	remote->rcv_end = 0;

	if (ctx->remotes == NULL) {
		ctx->remotes = remote;
	} else {
		canopy_remote_t *tmp = ctx->remotes;
		while (tmp != NULL) {
			if (tmp->next == NULL) {
				tmp->next = remote;
				break;
			}
			tmp = tmp->next;
		}
	}

	return CANOPY_SUCCESS;
}

// Shutdown a remote object.
// Closes persistent connection to server, if any.
// Frees any allocated memory.
canopy_error canopy_remote_shutdown(canopy_remote_t *remote) {
	if (remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_remote_shutdown()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Get the remote's clock in milliseconds.  The returned value has no relation
// to wall clock time, but is monotonically increasing and is reported
// consistently by the remote to anyone who asks.
canopy_error canopy_remote_get_time(canopy_remote_t *remote,
        cos_time_t *time,
        canopy_barrier_t *barrier) {
	if (remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_remote_get_time()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Get our version of the remote's clock in milliseconds.  This is based on the
// time obtained the last time canopy_remote_get_clock_time was called, plus
// however much time has elapsed since then.
//
// Returns CANOPY_ERROR_AGAIN if canopy_remote_get_time() has never been
// called for <remote>.
canopy_error canopy_get_local_time(canopy_remote_t *remote,
        cos_time_t *time) {
	if (remote == NULL) {
		cos_log(LOG_LEVEL_FATAL, "remote is null in call to canopy_get_local_time()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/***************************************************************************
 *  c_json_parse_remote_status(struct canopy_device *device,
 *      char* js, int js_len,
 *       jsmntok_t *token, int tok_len,
 *       int name_offset,
 *       int *next_token)
 *
 *      Parses the JSON status tag from the remote
 *  (in canopy_remote.c)
 */
canopy_error c_json_parse_remote_status(struct canopy_device *device,
        char* js, int js_len,           /* the input JSON and total length  */
        jsmntok_t *token, int tok_len,  /* token array with length */
        int name_offset,                /* token offset for name vardecl */
        int *next_token) {                /* the token after the decls */

    int i;
    int offset = name_offset;
    char name[128];
    char primative[128];
    COS_ASSERT(device != NULL);
    struct canopy_remote *remote = device->remote;
    COS_ASSERT(remote != NULL);

    /*
     * Verify the thing starts with "status"
     */
    COS_ASSERT(token[offset].type == JSMN_STRING);
    COS_ASSERT(
            strncmp((const char*) &js[token[offset].start], TAG_STATUS, (token[offset].end - token[offset].start)) == 0);
    COS_ASSERT(token[offset].size == 1);
    offset++;

    /*
     *   "    \"status\": {"
     *   "        \"last_activity_time\": 1426803897000000,"
     *   "        \"ws_connected\": false"
     *   "        \"active_status\" : \"active\", "
     *   "    },"
     *
     * We should be at the object after the status.  The size of this object indicates
     * the number of entries in the list
     *      If the name is last_activity_time, the following thing should be a
     *      Primitive
     *
     *      If the name is ws_connected, we expect a primitive that's a boolean
     *
     *      If the name is active_status, it's followed by a string
     *
     */
    COS_ASSERT(token[offset].type == JSMN_OBJECT);
    int n_vars = token[offset].size;
    offset++;  /*  points to name */
    for (i = 0; i < n_vars; i++) {
        int sizeof_name;
        memset(&name, 0, sizeof(name));
        sizeof_name = sizeof(name);

        COS_ASSERT(token[offset].type == JSMN_STRING);
        strncpy(name, &js[token[offset].start], (token[offset].end - token[offset].start));

        if (strncmp(name, TAG_WS_CONNECTED, sizeof_name) == 0) {

            /*
             * The next token should be a boolean.
             */
            offset++; /* boolean tag, which is a primitive, not a string */
            COS_ASSERT(token[offset].type == JSMN_PRIMITIVE);
            COS_ASSERT(token[offset].size == 0);
            int t = strncmp(&js[token[offset].start], "true", (token[offset].end - token[offset].start));
            int f = strncmp(&js[token[offset].start], "false", (token[offset].end - token[offset].start));
            if (t || f) {
                remote->ws_connected = (t == 1);
            } else {
                cos_log(LOG_LEVEL_FATAL, "boolean in status WS_CONNECTED not true or false\n");
                return CANOPY_ERROR_FATAL;
            }
            offset++;  /* up to next name */

        } else if (strncmp(name, TAG_ACTIVE_STATUS, sizeof_name) == 0) {

            /*
             * The next token should be a string.  It has a defined content.
             */
            offset++;
            COS_ASSERT(token[offset].type == JSMN_STRING);
            memset(&primative, 0, sizeof(primative));
            strncpy(primative, &js[token[offset].start], (token[offset].end - token[offset].start));
            canopy_active_status status = activity_status_from_string(primative, sizeof(primative));
            remote->active_status = status;
            offset++;  /* to the next name */

        } else if (strncmp(name, TAG_LAST_ACTIVITY_TIME, sizeof_name) == 0) {

            /*
             * The next should be a primative that's an unsigned long long
             */
            offset++;
            COS_ASSERT(token[offset].type == JSMN_PRIMITIVE);
            memset(&primative, 0, sizeof(primative));
            strncpy(primative, &js[token[offset].start], (token[offset].end - token[offset].start));
            unsigned long long ull = atoll(primative);
            remote->last_activity = (cos_time_t)ull;
            offset++; /* next name */
        }

    } /* nvars */

    *next_token = offset;

    return CANOPY_SUCCESS;
}


