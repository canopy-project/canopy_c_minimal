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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <canopy_min.h>
#include <canopy_os.h>

#define TOASTER_UUID "e43eb410-48da-421a-b07d-1cd751412fd5"
#define TOASTER_SECRET_KEY "wYI0G+HQN9fj76fpyxwiHKJDQags0dpM"
#define REMOTE_ADDR "dev02.canopy.link"

int main(void) {
	char *buffer;
	size_t buffer_size = 4096;
    canopy_error err;
    canopy_context_t ctx;
    canopy_remote_t remote;
    canopy_remote_params_t params;
    canopy_device_t device;
    struct canopy_var *temp_sensor;
    struct canopy_var *darkness;
    float darkness_val;
    cos_time_t time;

    // Initialize canopy ctx
    err = canopy_ctx_init(&ctx, 0);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing ctx: %s\n", canopy_error_string(err));
        exit(-1);
    }

    buffer = (char *)cos_alloc(buffer_size);
    if (buffer == NULL) {
        cos_log(LOG_LEVEL_ERROR, "Unable to allocate buffer\n");
        exit(-1);
    }

    // Initialize remote
    memset(&params, 0, sizeof(params));
    params.credential_type = CANOPY_DEVICE_CREDENTIALS;
    params.name = TOASTER_UUID;
    params.password = TOASTER_SECRET_KEY;
    params.auth_type = CANOPY_BASIC_AUTH;
    params.remote = REMOTE_ADDR;
    params.skip_cert_check = true;
    params.use_ws = false;
    params.persistent = false;
    err = canopy_remote_init(&ctx, &params, buffer, buffer_size, &remote);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing remote: %s\n", canopy_error_string(err));
        goto cleanup;
    }

    // Get self device (blocking)
    err = canopy_get_self_device(&remote, &device, NULL);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error fetching self device: %s\n", canopy_error_string(err));
        goto cleanup;
    }

    // Create cloud variables "temp_sensor" and "darkness"
    err = canopy_device_var_declare(&device,
            CANOPY_VAR_OUT,
            CANOPY_VAR_DATATYPE_FLOAT32,
            "temp_sensor",
            &temp_sensor);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing var temp_sensor: %s\n", canopy_error_string(err));
        goto cleanup;
    }
    err = canopy_var_set_float32(temp_sensor, 42.0f);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error setting temp_sensor value: %s\n", canopy_error_string(err));
        goto cleanup;
    }

    err = canopy_device_var_declare(&device,
            CANOPY_VAR_IN,
            CANOPY_VAR_DATATYPE_FLOAT32,
            "darkness",
            &darkness);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing var darkness: %s\n", canopy_error_string(err));
        goto cleanup;
    }

    // sync with remote (blocking)
    err = canopy_device_sync_with_remote(&remote, &device, NULL);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error syncing with remote: %s\n", canopy_error_string(err));
        goto cleanup;
    }

    // read darkness level
    err = canopy_var_get_float32(darkness, &darkness_val, &time);
    if (err == CANOPY_ERROR_VAR_NOT_SET) {
        cos_log(LOG_LEVEL_INFO, "darkness not set\n");
    } else if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error reading darkness level\n", canopy_error_string(err));
        goto cleanup;
    } else {
        cos_log(LOG_LEVEL_INFO, "darkness is %f\n", darkness_val);
        cos_log(LOG_LEVEL_INFO, "last updated %d\n", time);
    }


cleanup:
    canopy_ctx_shutdown(&ctx);
    return 0;
}
