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

	COS_ASSERT(remote != NULL);
	COS_ASSERT(device != NULL);
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}


// Updates a device object's status and properties from the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// locally.
canopy_error canopy_device_update_from_remote(
        canopy_remote_t *remote,
        canopy_device_t *device,
        canopy_barrier_t *barrier) {

	COS_ASSERT(remote != NULL);
	COS_ASSERT(device != NULL);
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Updates a device object's status and properties to the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// remotely.
canopy_error canopy_device_update_to_remote(
        canopy_remote_t *remote,
        canopy_device_t *device,
        canopy_barrier_t *barrier) {

	COS_ASSERT(remote != NULL);
	COS_ASSERT(device != NULL);
	return CANOPY_ERROR_NOT_IMPLEMENTED;
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


