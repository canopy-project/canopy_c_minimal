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
#include	<canopy_communication.h>
#include	<canopy_os.h>

/*
 * _construct_device_sync_payload
 *
 *      Constructs the request payload to POST to /api/device/self during
 *      canopy_device_update_from_remote() and canopy_device_sync_with_remote()
 *
 *      <payload> is the destination buffer where the request will be stored.
 *
 *      <len> is the size of payload in bytes.
 */
static canopy_error _construct_device_sync_payload(canopy_device_t *device,
        char *payload, size_t len) {

    int ierr;
    canopy_error err;
    struct c_json_state state;

    // init buffer for payload
    c_json_buffer_init(&state, payload, len);

    // construct payload
    ierr = c_json_emit_open_object(&state);
    if (ierr != C_JSON_OK) {
        return CANOPY_ERROR_NETWORK;
    }

    err = c_json_emit_vardcl(device, &state, false);
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    err = c_json_emit_vars(device, &state, false, true);
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    if (device->friendly_name_dirty) {
        err = c_json_emit_name_and_value(&state, TAG_FRIENDLY_NAMES,
                device->friendly_name);
        if (err != CANOPY_SUCCESS) {
            return err;
        }
    }

    if (device->location_note_dirty) {
        err = c_json_emit_name_and_value(&state, TAG_LOCATION_NOTE,
                device->location_note);
        if (err != CANOPY_SUCCESS) {
            return err;
        }
    }

    ierr = c_json_emit_close_object(&state);
    if (ierr != C_JSON_OK) {
        return CANOPY_ERROR_NETWORK;
    }

    return CANOPY_SUCCESS;
}

/*
 * _clear_dirty_flags
 *
 * Some fields (such as "friendly_name" are only sent to the remote if they
 * have been changed locally since the last sync.
 *
 * This routine should be called after reporting to the remote to clear the
 * dirty flag for these fields.
 */
static void _clear_dirty_flags(struct canopy_device *device) {
    device->friendly_name_dirty = false;
    device->location_note_dirty = false;
}

/****************************************************************************/
/****************************************************************************/
/*
 *
 */
canopy_error canopy_device_init(struct canopy_device *device,
        struct canopy_remote *remote, const char *device_id) {

    C_COND_FATAL_RETURN(device != NULL, CANOPY_ERROR_USAGE);

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

/*
 * canopy_get_self_device
 */
canopy_error canopy_get_self_device(canopy_remote_t *remote,
        struct canopy_device *device, canopy_barrier_t *barrier) {

    jsmntok_t token[512]; // TODO: large enough?
    canopy_error err;
    int http_status;
    int active = 0;
    bool result_code;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // verify that device credentials are in use
    if (remote->params->credential_type != CANOPY_DEVICE_CREDENTIALS) {
        return CANOPY_ERROR_BAD_CREDENTIALS;
    }

    // initialize device object
    canopy_device_init(device, remote, remote->params->name);

    // GET /api/device/self
    err = canopy_remote_http_get(remote, "/api/device/self",
    NULL, &http_status, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error during GET /api/device/self: %s\n",
                canopy_error_string(err));
        return err;
    }

    if (http_status != 200) {
        // TODO: Return the appropriate error based on the response
        return CANOPY_ERROR_UNKNOWN;
    }

    err = c_json_parse_string(
            (char*)remote->rcv_buffer, 
            strlen(remote->rcv_buffer), 
            token, 
            sizeof(token) / sizeof(token[0]),
            &active);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during tokenization of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }

    // Parse response and update device object
    err = c_json_parse_device(device, remote->rcv_buffer,
            remote->rcv_buffer_size, token, sizeof(token) / sizeof(token[0]),
            &result_code,
            true);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during parse of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }

    return CANOPY_SUCCESS;
}

/*
 * canopy_device_update_from_remote
 */
canopy_error canopy_device_update_from_remote(canopy_remote_t *remote,
        canopy_device_t *device, canopy_barrier_t *barrier) {

    jsmntok_t token[512]; // TODO: large enough?
    canopy_error err;
    int http_status;
    bool result_code;
    int active = 0;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // GET /api/device/self
    err = canopy_remote_http_get(remote, "/api/device/self",
    NULL, &http_status, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error during GET /api/device/self: %s\n",
                canopy_error_string(err));
        return err;
    }

    if (http_status != 200) {
        // TODO: Return the appropriate error based on the response
        return CANOPY_ERROR_UNKNOWN;
    }

    // Tokenize response
    err = c_json_parse_string(
            (char*)remote->rcv_buffer, 
            strlen(remote->rcv_buffer), 
            token, 
            sizeof(token) / sizeof(token[0]),
            &active);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during tokenization of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }

    // Parse response and update device object
    err = c_json_parse_device(device, remote->rcv_buffer,
            remote->rcv_buffer_size, token, sizeof(token) / sizeof(token[0]),
            &result_code,
            true);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during parse of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }

    return CANOPY_SUCCESS;
}

/*
 * canopy_device_update_to_remote
 */
canopy_error canopy_device_update_to_remote(canopy_remote_t *remote,
        canopy_device_t *device, canopy_barrier_t *barrier) {

    canopy_error err;
    char request_payload[2048];
    int http_status;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // construct payload
    err = _construct_device_sync_payload(device, request_payload,
            sizeof(request_payload));
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    // send payload
    err = canopy_remote_http_post(remote, "/api/device/self", request_payload,
            &http_status, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error during POST /api/device/self: %s\n",
                canopy_error_string(err));
        return err;
    }

    if (http_status != 200) {
        // TODO: Return the appropriate error based on the response
        return CANOPY_ERROR_UNKNOWN;
    }

    // clear dirty flags
    _clear_dirty_flags(device);

    // ignore response since this isn't a "sync"

    return CANOPY_SUCCESS;
}

/*
 * canopy_device_sync_with_remote
 */
canopy_error canopy_device_sync_with_remote(canopy_remote_t *remote,
        canopy_device_t *device, canopy_barrier_t *barrier) {

    canopy_error err;
    jsmntok_t token[512]; // TODO: large enough?
    char request_payload[2048];
    int http_status;
    bool result_code;
    int active = 0;

    COS_ASSERT(remote != NULL);
    COS_ASSERT(device != NULL);

    // construct payload
    err = _construct_device_sync_payload(device, request_payload,
            sizeof(request_payload));
    if (err != CANOPY_SUCCESS) {
        return err;
    }

    // send payload
    err = canopy_remote_http_post(remote, "/api/device/self", request_payload,
            &http_status, barrier);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error during POST /api/device/self: %s\n",
                canopy_error_string(err));
        return err;
    }

    if (http_status != 200) {
        // TODO: Return the appropriate error based on the response
        return CANOPY_ERROR_UNKNOWN;
    }

    // Tokenize response
    err = c_json_parse_string(
            (char*)remote->rcv_buffer, 
            strlen(remote->rcv_buffer), 
            token, 
            sizeof(token) / sizeof(token[0]),
            &active);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during tokenization of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }


    // Parse response and update device object
    err = c_json_parse_device(device, remote->rcv_buffer,
            remote->rcv_buffer_size, token, sizeof(token) / sizeof(token[0]),
            &result_code,
            true);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR,
                "Error during parse of /api/device/self response: %s\n",
                canopy_error_string(err));
        return err;
    }

    // clear dirty flags
    _clear_dirty_flags(device);
    return CANOPY_SUCCESS;
}

/*
 * canopy_device_get_friendly_name
 */
canopy_error canopy_device_get_friendly_name(canopy_device_t *device,
        char *friendly_name, size_t len) {
    COS_ASSERT(device != NULL);
    COS_ASSERT(friendly_name != NULL);

    if (strnlen(device->friendly_name, CANOPY_FRIENDLY_NAME_MAX_LENGTH) > len) {
        return CANOPY_ERROR_BUFFER_TOO_SMALL;
    }

    strncpy(friendly_name, device->friendly_name, len);
    if (len > 0) {
        friendly_name[len - 1] = '\0';
    }

    return CANOPY_SUCCESS;
}

/*
 * canopy_device_get_location_note
 */
canopy_error canopy_device_get_location_note(canopy_device_t *device,
        char *location_note, size_t len) {
    COS_ASSERT(device != NULL);
    COS_ASSERT(location_note != NULL);

    if (strnlen(device->location_note, CANOPY_NOTE_MAX_LENGTH) > len) {
        return CANOPY_ERROR_BUFFER_TOO_SMALL;
    }

    strncpy(location_note, device->location_note, len);
    if (len > 0) {
        location_note[len - 1] = '\0';
    }

    return CANOPY_SUCCESS;
}

canopy_error canopy_device_set_friendly_name(canopy_device_t *device,
        const char *friendly_name) {
    COS_ASSERT(device != NULL);
    COS_ASSERT(friendly_name != NULL);

    if (strnlen(friendly_name, CANOPY_FRIENDLY_NAME_MAX_LENGTH + 1) >
    CANOPY_FRIENDLY_NAME_MAX_LENGTH) {
        return CANOPY_ERROR_BAD_PARAM;
    }

    // TODO: other input validation
    strcpy(device->friendly_name, friendly_name);

    device->friendly_name_dirty = true;
    return CANOPY_SUCCESS;
}

canopy_error canopy_device_set_location_note(canopy_device_t *device,
        const char *location_note) {
    COS_ASSERT(device != NULL);
    COS_ASSERT(location_note != NULL);

    if (strnlen(location_note, CANOPY_NOTE_MAX_LENGTH + 1) >
    CANOPY_NOTE_MAX_LENGTH) {
        return CANOPY_ERROR_BAD_PARAM;
    }

    // TODO: other input validation
    strcpy(device->location_note, location_note);

    device->location_note_dirty = true;
    return CANOPY_SUCCESS;
}

// Get the active status for a device.
canopy_error canopy_device_get_active_status(canopy_device_t *device,
        canopy_active_status *active_status,
        canopy_ws_connection_status *ws_status) {

    COS_ASSERT(device != NULL);
    COS_ASSERT(active_status != NULL);
    COS_ASSERT(ws_status != NULL);
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/******************************************************************************/
/******************************************************************************/

/***************************************************************************
 * 	c_json_parse_device(struct canopy_device *device,
 *		char* js, int js_len, jsmntok_t *token, int tok_len,
 *		int current)
 *
 * 		parses the JSON vardecl tag from the server to register the variables that are registered
 * 	with the device. (in canopy_device.c)
 */
canopy_error c_json_parse_device(struct canopy_device *device,
        char* js, int js_len, /* the input JSON and total length  */
        jsmntok_t *token, int tok_len, /* token array with length */
        bool *result_code, /* the value of "result : " */
        bool check_obj) { /* expect outer-most object */

    int i;
    int offset = 0;
    char name[128];
    int count;
    int next_token;
    canopy_error err = CANOPY_SUCCESS;
    char buf[CANOPY_NOTE_MAX_LENGTH];

    COS_ASSERT(device != NULL);
    COS_ASSERT(device->remote != NULL);

    /*
     * This is the opening { that must be the first thing
     */
    COS_ASSERT(token[offset].type == JSMN_OBJECT);
    count = token[offset].size;
    offset++;  /* go to the first token after the object */

    /*
     * The parse says that there are 'count' first level tags.
     */
    for (i = 0; i < count; i++) {
        int sizeof_name;
        memset(name, 0, sizeof(name));
        sizeof_name = sizeof(name);

        COS_ASSERT(token[offset].type == JSMN_STRING);
        strncpy(name, &js[token[offset].start], (token[offset].end - token[offset].start));
        /*
         * We don't need to do this because we memset the name to 0s already
         */
        //  name[len + 1] = '\0';

        /*
         * Find the tag, and process what is found
         */
        if (strncmp(name, TAG_STATUS, sizeof_name) == 0) {
            err = c_json_parse_remote_status(device,
                    (char*)js, js_len, /* the input JSON and total length  */
                    token, tok_len, /* token array with length */
                    offset, /* token offset for name status */
                    &next_token);
            offset = next_token;

        } else if (strncmp(name, TAG_VAR_DECLS, sizeof_name) == 0) {
            err = c_json_parse_vardcl(device,
                    (char*)js, js_len, /* the input JSON and total length  */
                    token, tok_len, /* token array with length */
                    offset, /* token offset for name vardecl */
                    &next_token,
                    false); /* expect outer-most object */
            offset = next_token;

        } else if (strncmp(name, TAG_VARS, sizeof_name) == 0) {
            err = c_json_parse_vars(device,
                    (char*)js, js_len, /* the input JSON and total length  */
                    token, tok_len, /* token array with length */
                    offset, /* token offset for name vardecl */
                    &next_token, /* the token after the decls */
                    false); /* expect outer-most object */
            offset = next_token;

        } else if (strncmp(name, TAG_RESULT, sizeof_name) == 0) {
            offset++; /* the thing following the name */
            int ok = strncmp(&js[token[offset].start], "ok", (token[offset].end - token[offset].start));
            int error = strncmp(&js[token[offset].start], "error", (token[offset].end - token[offset].start));
            if (ok == 0 || error == 0) {
                *result_code = (ok == 0);
            } else {
                cos_log(LOG_LEVEL_FATAL, "result isn't 'ok' or 'error'");
                return CANOPY_ERROR_FATAL;
            }
            offset++;

        } else if (strncmp(name, TAG_DEVICE_ID, sizeof_name) == 0) {
            memset(&buf, 0, sizeof(buf));
            offset++; /* the device id as a string */
            COS_ASSERT(token[offset].type == JSMN_STRING);
            COS_ASSERT(token[offset].size == 0);
            strncpy(buf, &js[token[offset].start], (token[offset].end - token[offset].start));
            strncpy(device->device_id, buf, sizeof(device->device_id));

            offset++; /* to the next name tag */

        } else if (strncmp(name, TAG_FRIENDLY_NAME, sizeof_name) == 0) {
            memset(&buf, 0, sizeof(buf));
            offset++; /* the device id as a string */
            COS_ASSERT(token[offset].type == JSMN_STRING);
            COS_ASSERT(token[offset].size == 0);
            strncpy(buf, &js[token[offset].start], (token[offset].end - token[offset].start));
            strncpy(device->friendly_name, buf, sizeof(device->friendly_name));
            device->friendly_name_dirty = false;

            offset++; /* to the next name tag */

        } else if (strncmp(name, TAG_LOCATION_NOTE, sizeof_name) == 0) {
            memset(&buf, 0, sizeof(buf));
            offset++; /* the device id as a string */
            COS_ASSERT(token[offset].type == JSMN_STRING);
            COS_ASSERT(token[offset].size == 0);
            strncpy(buf, &js[token[offset].start], (token[offset].end - token[offset].start));
            strncpy(device->location_note, buf, sizeof(device->location_note));
            device->location_note_dirty = false;

            offset++; /* to the next name tag */

        } else if (strncmp(name, TAG_SECRET_KEY, sizeof_name) == 0) {
            memset(&buf, 0, sizeof(buf));
            offset++; /* the device id as a string */
            COS_ASSERT(token[offset].type == JSMN_STRING);
            COS_ASSERT(token[offset].size == 0);
            strncpy(buf, &js[token[offset].start], (token[offset].end - token[offset].start));
            strncpy(device->secret_key, buf, sizeof(device->secret_key));

            offset++; /* to the next name tag */

        } else {
            int j;

            /*
             * The tag's not implemented, we need to look at the size of the
             * tag string, then check the type to handle other objects.
             *
             * The increment of offset gets us to the token after the name
             * string.
             */
            offset++; /* the thing following the name string */
            if (token[offset].type == JSMN_STRING) {
                COS_ASSERT(token[offset].size == 0);
                offset++; /* to the next name */

            } else if (token[offset].type == JSMN_OBJECT) {

                /*
                 * This isn't right, since OBJECTS an contain other OBJECTS
                 */
                for (j = 0; j < token[offset].size; j++) {


                    offset++;
                }


            } else if (token[offset].type == JSMN_ARRAY) {

                /*
                 * HACK:  This code assumes that the array is empty.  This is
                 * currently true, since the TAG related to this is
                 * "notifs"
                 */
                COS_ASSERT(token[offset].size == 0);
                offset++;

            } else if (token[offset].type == JSMN_PRIMITIVE) {

                COS_ASSERT(token[offset].size == 0);
                offset++;

            }

        }

    } /* for (count) */

    return err;
}
