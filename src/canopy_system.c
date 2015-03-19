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


/********************************************************
 * canopy_ctx_init():
 */
canopy_error canopy_ctx_init(canopy_context_t *ctx, int update_period) {
	if (ctx == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	memset(ctx, 0, sizeof(canopy_context_t));
	ctx->remotes = NULL;
	ctx->update_period = update_period;
	return CANOPY_SUCCESS;
}


/********************************************************
 * canopy_ctx_shutdown():
 */
canopy_error canopy_ctx_shutdown(canopy_context_t *ctx) {
	if (ctx == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	canopy_error error = CANOPY_SUCCESS;
	canopy_remote_t *remotes = ctx->remotes;
	while (remotes != NULL) {
		error = canopy_cleanup_remote(remotes);
		if (error != CANOPY_SUCCESS) {
			return error;
		}
		remotes = remotes->next;
	}
	return error;
}

/******************************************************************************/


/******************************************************************
 * canopy_ctx_set_logging()
 */
// Set logging options for a context.
//
// <enabled> enables/disables logging.  Defaults to false.
//
// <logfile> specifies the name of the file to log to.  Pass in NULL to
// use stderr or the system's default logging destination.  Defaults to
// NULL.
//
// <level> defines the logging level.  See canopy_os.h for level definitions.
canopy_error canopy_ctx_set_logging(canopy_context_t *ctx,
				bool enabled,
				const char *logfile,
				int level) {
	if (ctx == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/****************************************************************
 * canopy_ctx_get_logging()
 */
// Get the logging options for a context.
//
// <enabled> is a pointer to a bool, or NULL to not read this option.
//
// <logfile> is a pointer to a buffer at least <logfile_len> bytes long, or
// NULL to not read this option.  This call will set the string to an empty
// string if the default log destination is in use.
//
// <level> is a pointer to an int, or NULL to not read this option.
canopy_error canopy_ctx_get_logging(canopy_context_t *ctx,
				bool *enabled,
				char **logfile,
				size_t *logfile_len,
				int *level) {
	if (ctx == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/*****************************************************************************/

// WARNING! WARNING!
// Stack-scoped barriers can be used, but you must call either
// canopy_barrier_wait_for_complete() or canopy_barrier_cancel() before
// returning from the scope with the barrier.

// Block the current thread until the operation has completed, or timeout
// occurs.  Returns immediately if the requested operation has already
// finished.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <timeout_ms> is number of milliseconds to wait before CANOPY_ERROR_TIMEOUT
// is returned.
//
canopy_error canopy_barrier_wait_for_complete(canopy_barrier_t *barrier,
		int timeout_ms) {
	if (barrier == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Cancels the barrier.
// Any threads blocked in canopy_barrier_wait_for_complete will return with
// CANOPY_ERROR_CANCELLED.
// No further callbacks will be triggered for this barrier.
// After calling this it is safe to deallocate the barrier.
canopy_error canopy_barrier_cancel(canopy_barrier_t *barrier) {
	if (barrier == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Establish a callback that will be triggered when the operation has
// completed.  Triggers the callback immediately if the requested operation has
// already finished.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <cb> is the callback to trigger.
//
// <userdata> is user data that gets passed along to the callback.
canopy_error canopy_barrier_setup_callback(canopy_barrier_t *barrier,
        canopy_barrier_cb cb,
        void *userdata) {
	if (barrier == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Check if an asyncrhonous operation has completed.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// Returns CANOPY_SUCCESS if the operation is complete.
// Returns CANOPY_ERROR_AGAIN if the operation has not yet finished.
canopy_error canopy_barrier_is_complete(canopy_barrier_t *barrier) {
	if (barrier == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/******************************************************************************/

