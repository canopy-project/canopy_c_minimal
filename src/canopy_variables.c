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
        canopy_time_t val_time;
    } value;
} canopy_var_value_t;

#define CANOPY_VAR_NAME_MAX_LENGTH 128
typedef struct canopy_var {
    struct canpopy_var *next;    /* linked list of variables, hung off device */
    canopy_device_t *device;
    canopy_var_direction direction;
    canopy_var_datatype type;
    char name[CANOPY_VAR_NAME_MAX_LENGTH];
    struct canopy_var_value val;  /* yes, not a pointer, real storage */
} canopy_var_t;


#endif

canopy_error canopy_device_var_init(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name,
        canopy_var_t *out_var) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_device_get_var_by_name(canopy_device_t *device,
        const char *var_name,
        canopy_var_t *var) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_bool(canopy_var_t *var, bool value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_int8(canopy_var_t *var, int8_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_int16(canopy_var_t *var, int16_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_int32(canopy_var_t *var, int32_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_uint8(canopy_var_t *var, uint8_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_uint16(canopy_var_t *var, uint16_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_uint32(canopy_var_t *var, uint32_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_datetime(canopy_var_t *var, canopy_time_t value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_float32(canopy_var_t *var, float value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_float64(canopy_var_t *var, double value) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_set_string(canopy_var_t *var, const char *value, size_t len) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_bool(canopy_var_t *var,
		bool *value,
		canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_int8(canopy_var_t *var,
        int8_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_int16(canopy_var_t *var,
        int16_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_int32(canopy_var_t *var,
        uint32_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_uint8(canopy_var_t *var,
        uint8_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_uint16(canopy_var_t *var,
        uint16_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_uint32(canopy_var_t *var,
        uint32_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_datetime(canopy_var_t *var,
        canopy_time_t *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_float32(canopy_var_t *var,
        float *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_float64(canopy_var_t *var,
        double *value,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

canopy_error canopy_var_get_string(canopy_var_t *var,
        char *buf,
        size_t len,
        size_t *out_len,
        canopy_time_t *last_time) {
	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

