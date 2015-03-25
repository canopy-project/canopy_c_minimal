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

/******************************************************************************/

/*
 *  var_declare(OUT, FLOAT32, "temperature", &var);
 *
 *  =>
 *
 *      "var_decls" : {
 *          "out float32 temperature" : {},
 *          "out float32 humidity" : {}
 *      }
 */

/*
 * Decisions regarding timestamps.
 *
 * - Client never sends times to the server.
 * - When the client makes a api/device/<ID> request, the response includes all
 *   cloud variables and the time they last changed.  The response also
 *   includes the server time that the response was generated.
 * - The client stores this internally as "last_poll_time" (device object? remote?).
 * 
 *
 * On update the client just sends the changed variables.  The server returns
 * all of them with their timestamps (we may later optimize this).  The library
 * looks at the last timestamp for the variable and decides if it needs to be
 * updated locally or not.
 *
 * Timestamps are encoded over the wire as 64-bit integer representing
 * microseconds since the Unix Epoch.
 */


static char buffer[1024];

static struct canopy_var* find_name(canopy_device_t *device, const char* name);

#ifdef DOCUMENT
typedef enum {
    CANOPY_VAR_DIRECTION_INVALID=0,
    CANOPY_VAR_OUT,
    CANOPY_VAR_IN,
    CANOPY_VAR_INOUT,
} canopy_var_direction;

typedef enum {
    CANOPY_VAR_DATATYPE_INVALID=0,
    CANOPY_VAR_DATATYPE_VOID,
    CANOPY_VAR_DATATYPE_STRING,
    CANOPY_VAR_DATATYPE_BOOL,
    CANOPY_VAR_DATATYPE_INT8,
    CANOPY_VAR_DATATYPE_UINT8,
    CANOPY_VAR_DATATYPE_INT16,
    CANOPY_VAR_DATATYPE_UINT16,
    CANOPY_VAR_DATATYPE_INT32,
    CANOPY_VAR_DATATYPE_UINT32,
    CANOPY_VAR_DATATYPE_FLOAT32,
    CANOPY_VAR_DATATYPE_FLOAT64,
    CANOPY_VAR_DATATYPE_DATETIME,
    CANOPY_VAR_DATATYPE_STRUCT,
    CANOPY_VAR_DATATYPE_ARRAY,
} canopy_var_datatype;

#define CANOPY_VAR_VALUE_MAX_LENGTH 128
typedef struct canopy_var_value {
    canopy_var_datatype type;
    union {
        struct {
            char *buf;
            size_t len;
        } val_string;
        bool val_bool;
        int8_t val_int8;
        int16_t val_int16;
        int32_t val_int32;
        uint8_t val_uint8;
        uint16_t val_uint16;
        uint32_t val_uint32;
        float val_float;
        double val_double;
        cos_time_t val_time;
    } value;
} canopy_var_value_t;

#define CANOPY_VAR_NAME_MAX_LENGTH 128
typedef struct canopy_var {
    struct canopy_var 		*next;    /* linked list of variables, hung off device */
    canopy_device_t 		*device;
    canopy_var_direction 	direction;
    canopy_var_datatype 	type;	/* duplicate of type in the value */
    char name[CANOPY_VAR_NAME_MAX_LENGTH];
	bool 					set;	/* This variable has been set */
    struct canopy_var_value	val;  	/* yes, not a pointer, real storage */
} struct canopy_var;


#endif

/*****************************************************
 * canopy_device_var_init()
 */
canopy_error canopy_device_var_declare(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name,
        struct canopy_var *out_var) {
	if (device == NULL) {
		cos_log(LOG_LEVEL_FATAL, "device is null in call to canopy_device_var_declare()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (out_var == NULL) {
		cos_log(LOG_LEVEL_FATAL, "out_var is null in call to canopy_device_var_declare()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (name == NULL) {
		cos_log(LOG_LEVEL_FATAL, "name is null in call to canopy_device_var_declare()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	struct canopy_var *tmp = find_name(device, name);
	if (tmp != NULL) {
		cos_log(LOG_LEVEL_DEBUG, "variable already in use in call to canopy_device_var_declare()");
		return CANOPY_ERROR_VAR_IN_USE;
	}

	/*
	 * Setup the out_var before we hang it on the device...
	 */
	memset(out_var, 0, sizeof(struct canopy_var));
	strncpy(out_var->name, name, CANOPY_VAR_NAME_MAX_LENGTH - 2);
	out_var->name[CANOPY_VAR_NAME_MAX_LENGTH - 1] = '\0';
	out_var->direction = direction;
	out_var->type = type;
	out_var->set = false;
	out_var->val.type = type;

	/*
	 * TODO Figure out why the cast is needed.  I've turned the warning off
	 * but I'd shough would like to know why...
	 *
	 * We stick the new variable at the front of the list.  Device->vars can be
	 * NULL, but it get's updated correctly when it is.
	 */
	out_var->next = device->vars;
	device->vars = out_var;
	return CANOPY_SUCCESS;
}

/*****************************************************
 * find_name()
 */
static struct canopy_var* find_name(canopy_device_t *device, const char* name) {
	struct canopy_var *var = device->vars;
	while (var != NULL) {
		if (strncmp(var->name, name, CANOPY_VAR_NAME_MAX_LENGTH - 1) == 0) {
			return var;
		}
		var = (struct canopy_var *)var->next; /* I have no idea why this is needed */
	}
	return NULL;
}

/*****************************************************
 * canopy_device_get_var_by_name()
 */
canopy_error canopy_device_get_var_by_name(canopy_device_t *device,
        const char *var_name,
        struct canopy_var *var) {
	if (device == NULL) {
		cos_log(LOG_LEVEL_FATAL, "device is null in call to canopy_device_get_var_by_name()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (var_name == NULL) {
		cos_log(LOG_LEVEL_FATAL, "var_name is null in call to canopy_device_get_var_by_name()");
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (var == NULL) {
		cos_log(LOG_LEVEL_FATAL, "var is null in call to canopy_device_get_var_by_name()");
		return CANOPY_ERROR_BAD_PARAM;
	}

	struct canopy_var *dev_var;
	dev_var = find_name(device, var_name);
	if (dev_var != NULL) {
		/*
		 * Find and fill in variable data.
		 * TODO think about who owns the memory once it's in the library
		 */
		memcpy(var, dev_var, sizeof(struct canopy_var));
		var->next = NULL;
		cos_log(LOG_LEVEL_DEBUG,
				"variable %s 0x%p found in call to canopy_device_get_var_by_name()",
				var_name, (void*)dev_var);
	} else {
		cos_log(LOG_LEVEL_DEBUG, "variable not found in call to canopy_device_get_var_by_name()");
		return CANOPY_ERROR_VAR_NOT_FOUND;
	}
	return CANOPY_SUCCESS;
}

/***************************************************************************
 * 	c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the remote. (in canopy_variables.c)
 */
canopy_error c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state) {
	int err = CANOPY_SUCCESS;
	struct canopy_var *var;
	err = c_json_emit_open_object(state);
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG, "unable to emit opening object err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	err = c_json_emit_name_and_object(state, "var_decls");
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG, "unable to emit var_decls err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	var = device->vars;
	while (var != NULL) {
		char * name = var->name;
	    snprintf(buffer, sizeof(buffer), "%s %s %s",
	    		canopy_var_direction_string(var->direction),
	    		canopy_var_datatype_string(var->type), name);
		err = c_json_emit_name_and_object(state, buffer);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit variable: %s err: %d\n", name, err);
			return CANOPY_ERROR_JSON;
		}
		err = c_json_emit_close_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit variable %s closing object err: %d\n", name, err);
			return CANOPY_ERROR_JSON;
		}
		var = var->next;
	}


	err = c_json_emit_close_object(state);
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG, "unable to emit var_decls closing object err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	err = c_json_emit_close_object(state);
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG, "unable to emit opening object err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}
	return err;
}


/*****************************************************************************/
/*****************************************************************************/

canopy_error canopy_var_set_bool(struct canopy_var *var, bool value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_BOOL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_bool()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_BOOL;
	var_val->value.val_bool = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_int8(struct canopy_var *var, int8_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_INT8) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_int8()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_INT8;
	var_val->value.val_int8 = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_int16(struct canopy_var *var, int16_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_INT16) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_int16()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_INT16;
	var_val->value.val_int16 = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_int32(struct canopy_var *var, int32_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_INT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_int32()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_INT32;
	var_val->value.val_int32 = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_uint8(struct canopy_var *var, uint8_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_UINT8) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_uint8()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_BOOL;
	var_val->value.val_uint8 = value;
	var->set = true;
	cos_get_time(&var->last);
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_uint16(struct canopy_var *var, uint16_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_UINT16) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_uint16()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_UINT16;
	var_val->value.val_uint16 = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_uint32(struct canopy_var *var, uint32_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_UINT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_uint32()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_UINT32;
	var_val->value.val_bool = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_datetime(struct canopy_var *var, cos_time_t value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_DATETIME) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_datetime()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_DATETIME;
	var_val->value.val_time = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_float32(struct canopy_var *var, float value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_FLOAT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_float32()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_FLOAT32;
	var_val->value.val_float = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_float64(struct canopy_var *var, double value) {
	struct canopy_var_value *var_val = &var->val;
	if (var->type != CANOPY_VAR_DATATYPE_FLOAT64) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (cos_get_time(&var->last) != 0) {
		cos_log(LOG_LEVEL_FATAL, "Could not get time in canopy_var_set_float64()");
		return CANOPY_ERROR_FATAL;
	}
	var_val->type = CANOPY_VAR_DATATYPE_FLOAT64;
	var_val->value.val_double = value;
	var->set = true;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_set_string(struct canopy_var *var, const char *value, size_t len) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

/*****************************************************************************/


canopy_error canopy_var_get_bool(struct canopy_var *var,
		bool *value,
		cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_BOOL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_bool;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_int8(struct canopy_var *var,
        int8_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_INT8) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_int8;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_int16(struct canopy_var *var,
        int16_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_INT16) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_int16;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_int32(struct canopy_var *var,
        uint32_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_INT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_int32;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_uint8(struct canopy_var *var,
        uint8_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_UINT8) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_uint8;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_uint16(struct canopy_var *var,
        uint16_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_UINT16) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_uint16;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_uint32(struct canopy_var *var,
        uint32_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_UINT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_uint32;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_datetime(struct canopy_var *var,
        cos_time_t *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_DATETIME) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_time;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_float32(struct canopy_var *var,
        float *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_FLOAT32) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_float;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_float64(struct canopy_var *var,
        double *value,
        cos_time_t *last_time) {

	if (!var->set) {
		return CANOPY_ERROR_VAR_NOT_SET;
	}
	if (var->type != CANOPY_VAR_DATATYPE_FLOAT64) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	*value = var->val.value.val_double;
	*last_time = var->last;
	return CANOPY_SUCCESS;
}

canopy_error canopy_var_get_string(struct canopy_var *var,
        char *buf,
        size_t len,
        size_t *out_len,
        cos_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

