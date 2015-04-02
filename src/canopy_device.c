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


/****************************************************************************/
/****************************************************************************/
/*
 *
 */
canopy_error initialize_device(struct canopy_device *device, struct canopy_remote *remote) {
	COS_ASSERT(device != NULL);
	memset(device, 0, sizeof(struct canopy_device));
	/*
	 * It's OK that the remote is null....  Well not really, the'res an
	 * assertion in the variable code that checks for null;
	 */
	device->remote = remote;
	return CANOPY_SUCCESS;
}


/****************************************************************************/
/****************************************************************************/

// Get the device based on the authentication information provided to
// canopy_remote_init.
//
// <remote> is the remote object used for connecting to the server.
//
// <device> will store the device object returned from the server.  If
// <barrier> is NULL, this will contain the result upon a successful return.
// If <barrier> is provided, this will be updated when the result is ready, and
// will also be passed along to the barrier object.
//
// <barrier> will store a new barrier object that can be used to obtain the
// result when it is ready.  If NULL, this operation will block the current
// thread.
canopy_error canopy_get_self_device(canopy_remote_t *remote,
        struct canopy_device *device,
        canopy_barrier_t *barrier) {

    jsmntok_t token[512]; // TODO: large enough?
    canopy_error err;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // initialize device
    initialize_device(device, remote);

    // GET /api/device/self
    err = canopy_remote_http_get(remote, "/api/device/self", NULL, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, 
                "Error during GET /api/device/self: %s\n", 
                canopy_error_string(err));
        return err;
    }

    // Parse response and update device object
    err = c_json_parse_device(device, 
            remote->rcv_buffer, 
            remote->rcv_buffer_size,
            token,
            sizeof(token)/sizeof(token[0]),
            true);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, 
                "Error during parse of /api/device/self response: %s\n", 
                canopy_error_string(err));
        return err;
    }

    return CANOPY_SUCCESS;
}


// Updates a device object's status and properties from the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// locally.
canopy_error canopy_device_update_from_remote(
        canopy_remote_t *remote,
        canopy_device_t *device,
        canopy_barrier_t *barrier) {

    jsmntok_t token[512]; // TODO: large enough?
    canopy_error err;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // GET /api/device/self
    err = canopy_remote_http_get(remote, "/api/device/self", NULL, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, 
                "Error during GET /api/device/self: %s\n", 
                canopy_error_string(err));
        return err;
    }

    // Parse response and update device object
    err = c_json_parse_device(device, 
            remote->rcv_buffer, 
            remote->rcv_buffer_size,
            token,
            sizeof(token)/sizeof(token[0]),
            true);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, 
                "Error during parse of /api/device/self response: %s\n", 
                canopy_error_string(err));
        return err;
    }

    return CANOPY_SUCCESS;
}

// Updates a device object's status and properties to the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// remotely.
canopy_error canopy_device_update_to_remote(
        canopy_remote_t *remote,
        canopy_device_t *device,
        canopy_barrier_t *barrier) {

    canopy_error err;
    int ierr;
    char request_payload[2048];
    struct c_json_state state;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // init buffer for payload
    c_json_buffer_init(&state, request_payload, sizeof(request_payload));

    // construct payload
    ierr = c_json_emit_open_object(&state);
    if (ierr != C_JSON_OK) {
        return CANOPY_ERROR_NETWORK;
    }

    err = c_json_emit_vardcl(device, &state, false);
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    err = c_json_emit_vars(device, &state, false);
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    ierr = c_json_emit_close_object(&state);
    if (ierr != C_JSON_OK) {
        return CANOPY_ERROR_NETWORK;
    }

    // send payload
    err = canopy_remote_http_post(
            remote, 
            "/api/device/self", 
            request_payload, 
            barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, 
                "Error during POST /api/device/self: %s\n", 
                canopy_error_string(err));
        return err;
    }

    return CANOPY_SUCCESS;
}

// Synchronizes a device object with the remote server.
// (Potential revisit).  Server should do merge.  Does it need to be atomic?
//
// Roughly equivalent to:
//  canopy_device_update_from_remote(device, remote, NULL);
//  canopy_device_update_to_remote(device, remote, barrier);
//
canopy_error canopy_device_sync_with_remote(
        canopy_remote_t *remote,
        canopy_device_t *device,
        canopy_barrier_t *barrier) {

	COS_ASSERT(remote != NULL);
	COS_ASSERT(device != NULL);
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Get the active status for a device.
canopy_error canopy_device_get_active_status(
        canopy_device_t *device,
        canopy_active_status *active_status,
        canopy_ws_connection_status *ws_status) {

	COS_ASSERT(device != NULL);
	COS_ASSERT(active_status != NULL);
	COS_ASSERT(ws_status != NULL);
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}


