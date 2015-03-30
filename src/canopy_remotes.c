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

// Initializes a new remote object.  Does not necessarily open a TCP or
// websocket connection to the server, but initializes an object with the
// parameters to do so when needed.
//
// <ctx> is the context.
//
// <params> must be populated by the caller.  Sets all of the options for
// subsequent connections to the server.
//
// <remote> is a remote object that is initialized by this call.
canopy_error canopy_remote_init(canopy_context_t *ctx,
        canopy_remote_params_t *params,
        canopy_remote_t *remote) {
	if (ctx == NULL) {
		cos_log(LOG_LEVEL_FATAL, "ctx is null in call to canopy_remote_init()");
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


