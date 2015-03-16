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


/*****************************************************************************/

	/* statics go here */

/*****************************************************************************/

canopy_error canopy_cleanup_remote(canopy_remote_t *remote) {
	if (remote == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}


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
	if (remote == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Shutdown a remote object.
// Closes persistent connection to server, if any.
// Frees any allocated memory.
canopy_error canopy_remote_shutdown(canopy_remote_t *remote) {
	if (remote == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Get the remote's clock in milliseconds.  The returned value has no relation
// to wall clock time, but is monotonically increasing and is reported
// consistently by the remote to anyone who asks.
canopy_error canopy_remote_get_time(canopy_remote_t *remote,
        canopy_time_t *time,
        canopy_barrier_t *barrier) {
	if (remote == NULL) {
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
        canopy_time_t *time) {
	if (remote == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}


