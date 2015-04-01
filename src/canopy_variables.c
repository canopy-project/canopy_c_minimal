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
#include	<stdio.h>

#include	<canopy_min.h>
#include	<canopy_min_internal.h>
#include	<canopy_os.h>
#include	<jsmn/jsmn.h>

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
static struct canopy_var * create_variable(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name);


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

/******************************************************
/***************************************************************************
 * Allocates a variable, initializes it and hangs it on the device...
 *
 * 	<device>	device
 * 	<direction>	direction
 * 	<type>		datatype
 * 	<name>		variable name
 *
 * 	Returns NULL if the variable can't be created.
 *
 */
static struct canopy_var * create_variable(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name) {

	struct canopy_var *var;

	if (device == NULL) {
		cos_log(LOG_LEVEL_FATAL, "device is null in call to create_variable()");
		return NULL;
	}
	if (name == NULL) {
		cos_log(LOG_LEVEL_FATAL, "name is null in call to create_variable()");
		return NULL;
	}

	var = (struct canopy_var*)cos_alloc(sizeof(struct canopy_var));
	if (var == NULL) {
		cos_log(LOG_LEVEL_FATAL, "could not allocate memory create_variable()");
		return NULL;
	}

	/*
	 * Setup the variable
	 */
	memset(var, 0, sizeof(struct canopy_var));
	strncpy(var->name, name, CANOPY_VAR_NAME_MAX_LENGTH - 2);
	var->name[CANOPY_VAR_NAME_MAX_LENGTH - 1] = '\0';
	var->direction = direction;
	var->type = type;
	var->set = false;
	var->val.type = type;

	return var;
}


/*****************************************************
 * canopy_device_var_init()
 */
canopy_error canopy_device_var_declare(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name,
        struct canopy_var *out_var) {

	struct canopy_var *var;

	struct canopy_var *tmp = find_name(device, name);
	if (tmp != NULL) {
		cos_log(LOG_LEVEL_DEBUG, "variable already in use in call to canopy_device_var_declare()");
		return CANOPY_ERROR_VAR_IN_USE;
	}

	var = create_variable(device, direction, type, name);
	if (var == NULL) {
		cos_log(LOG_LEVEL_DEBUG, "create_variable() failed");
		return CANOPY_ERROR_OUT_OF_MEMORY;
	}

	/*
	 * We stick the new variable at the front of the list.  Device->vars can be
	 * NULL, but it get's updated correctly when it is.
	 */
#ifdef HAVE_MEMORY
    /*
     * TODO, write code to store this into the hash table on the device.....
     */
#else

	var->next = device->vars;
	device->vars = var;
#endif

	memcpy(out_var, var, sizeof(struct canopy_var));
	return CANOPY_SUCCESS;
}

/*****************************************************
 * find_name()
 */
static struct canopy_var* find_name(canopy_device_t *device, const char* name) {
#ifdef HAVE_MEMORY
    /*
     * TODO, write code to travers the hash table on the device.....
     */
#else
	struct canopy_var *var = device->vars;
	while (var != NULL) {
		if (strncmp(var->name, name, CANOPY_VAR_NAME_MAX_LENGTH - 1) == 0) {
			return var;
		}
		var = (struct canopy_var *)var->next; /* I have no idea why this is needed */
	}
	return NULL;
#endif
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
 * 	c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state,
 * 	bool emit_obj)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the remote. (in canopy_variables.c) "emit_obj" sends the opening
 * 	and closing object keys.
 */
canopy_error c_json_emit_vardcl(struct canopy_device *device,
		struct c_json_state *state, bool emit_obj) {
	int err = CANOPY_SUCCESS;
	struct canopy_var *var;
	if (emit_obj) {
		err = c_json_emit_open_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit opening object err: %d\n",
					err);
			return CANOPY_ERROR_JSON;
		}
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
			cos_log(LOG_LEVEL_DEBUG, "unable to emit variable: %s err: %d\n",
					name, err);
			return CANOPY_ERROR_JSON;
		}
		err = c_json_emit_close_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG,
					"unable to emit variable %s closing object err: %d\n", name,
					err);
			return CANOPY_ERROR_JSON;
		}
		var = var->next;
	}

	err = c_json_emit_close_object(state);
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG,
				"unable to emit var_decls closing object err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	if (emit_obj) {
		err = c_json_emit_close_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit opening object err: %d\n",
					err);
			return CANOPY_ERROR_JSON;
		}
	}
	return err;
}

/***************************************************************************
 * 	c_json_parse_vardcl(struct canopy_device *device,
 *		char* js, int js_len,
 *		jsmntok_t *token, int tok_len,
 *		int current,
 *		bool check_obj)
 *
 * 		parses the JSON vardecl tag from the server to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_parse_vardcl(struct canopy_device *device,
		char* js, int js_len, 			/* the input JSON and total length  */
		jsmntok_t *token, int tok_len,	/* token array with length */
		int offset,						/* token offset for name vardecl */
		int *next_token,				/* the token after the decls */
		bool check_obj) {				/* expect outer-most object */
	int err = CANOPY_SUCCESS;
	int i;

	COS_ASSERT(device != NULL);
	COS_ASSERT(device->remote != NULL);

	/*
	 * The JSON returned parses to the following structure.
	 * 	{type = JSMN_STRING, start = 6, end = 15, size = 1}, 	var_decls
	 *	{type = JSMN_OBJECT, start = 19, end = 68, size = 2}, 		{
     *	{type = JSMN_STRING, start = 31, end = 48, size = 0}, 			 out bool test_var
     *	{type = JSMN_OBJECT, start = 50, end = 62, size = 0}, 			{
	 *
	 * or
	 *
	 * 	{type = JSMN_STRING, start = 6, end = 15, size = 1}, 	var_decls
	 * 	{type = JSMN_OBJECT, start = 19, end = 114, size = 4}, 		{
	 * 	{type = JSMN_STRING, start = 31, end = 48, size = 0}, 			"out bool test_var"
	 * 	{type = JSMN_OBJECT, start = 50, end = 62, size = 0}, 			{
	 * 	{type = JSMN_STRING, start = 72, end = 94, size = 0},			"in datetime  test_time"
	 * 	{type = JSMN_OBJECT, start = 96, end = 108, size = 0}, 			{
	 *
	 * or
	 *
	 * 	{type = JSMN_STRING, start = 6, end = 15, size = 1}, 	var_decls
	 * 	{type = JSMN_OBJECT, start = 19, end = 159, size = 6}, 		{
	 * 	{type = JSMN_STRING, start = 31, end = 48, size = 0}, 			"out bool test_var"
	 * 	{type = JSMN_OBJECT, start = 50, end = 62, size = 0}, 			{
	 * 	{type = JSMN_STRING, start = 72, end = 94, size = 0}, 			"in datetime  test_time"
	 * 	{type = JSMN_OBJECT, start = 96, end = 108, size = 0}, 			{
	 * 	{type = JSMN_STRING, start = 118, end = 139, size = 0}, 		"inout uint32  test_32"
	 * 	{type = JSMN_OBJECT, start = 141, end = 153, size = 0}, 		{
	 *
	 */

	COS_ASSERT(token[offset].type == JSMN_STRING);
	COS_ASSERT(strncmp((const char*) &js[token[offset].start], "var_decls", (token[offset].end - token[offset].start)) == 0);
	COS_ASSERT(token[offset].size == 1);
	offset++;

	/*
	 * We should be at the object after the decls.  The size of this object indicates
	 * the number of entries in the list times 2.
	 * 		The first thing should be a string that has the specification in it.  Its size should be 0.
	 * 		The next token is the object, which also should have a size of 0.
	 *
	 * repeat as necessary.........
	 */
	COS_ASSERT(token[offset].type == JSMN_OBJECT);
	int n_decls = token[offset].size;
	COS_ASSERT((n_decls % 2) == 0);
	offset++;
	for (i = 0; i < (n_decls / 2); i++) {
		char dir[32];
		char type[32];
		char name[128];
		memset(&dir, 0, sizeof(dir));
		memset(&type, 0, sizeof(type));
		memset(&name, 0, sizeof(name));

		/*
		 * the token at offset should be the string we need to parse,
		 */
		COS_ASSERT(token[offset].type == JSMN_STRING);
		COS_ASSERT(token[offset].size == 0);
		strncpy(buffer, &js[token[offset].start], (token[offset].end - token[offset].start));
		buffer[(token[offset].start - token[offset].end)] = '\0';

		sscanf(buffer, "%s %s %s", (char*)&dir, (char*)&type, (char*)&name);
		canopy_var_direction v_dir = direction_from_string((const char*) dir,  sizeof(dir));
		canopy_var_datatype v_type = datatype_from_string((const char*) type, sizeof(type));
		if (v_dir == CANOPY_VAR_DIRECTION_INVALID) {
			cos_log(LOG_LEVEL_DEBUG, "v_dir in string is invalid: %s\n", dir);
			return CANOPY_ERROR_JSON;
		}
		if (v_type == CANOPY_VAR_DATATYPE_INVALID) {
			cos_log(LOG_LEVEL_DEBUG, "v_dir in string is invalid: %s\n", type);
			return CANOPY_ERROR_JSON;
		}

		/*
		 * We've got the name, so we need to look to see if this variable has
		 * already been defined.  If it has, we're done, but log any
		 * Difference in direction or type.
		 */
		struct canopy_var* var = find_name(device, (const char*) name);
		if (var != NULL) {

			/*
			 * We found the variable, check to see if something's different
			 */
			if (var->direction != v_dir) {
				cos_log(LOG_LEVEL_DEBUG, "v_dir %d doesn't match: %d\n", v_dir, var->direction);
			}
			if (var->type != v_type) {
				cos_log(LOG_LEVEL_DEBUG, "v_type %d doesn't match: %d\n", v_type, var->type);
			}
		} else {

			/*
			 * We need to create a new variable, and hang it on the device.
			 */
			var = create_variable(device, v_dir, v_type, name);
		#ifdef HAVE_MEMORY
		    /*
		     * TODO, write code to store this into the hash table on the device.....
		     */
		#else

			var->next = device->vars;
			device->vars = var;
		#endif
		}

		/*
		 * Go on to the next token
		 */
		offset++;

		/*
		 * Now there should be an empty object...
		 */
		COS_ASSERT(token[offset].type == JSMN_OBJECT);
		COS_ASSERT(token[offset].size == 0);
		offset++;
	} /* decl loop */

	*next_token = offset;

	return err;
}

/***************************************************************************
 * 	c_json_emit_vars(struct canopy_device *device, struct c_json_state *state)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_emit_vars(struct canopy_device *device,
		struct c_json_state *state,
		bool emit_obj) {

	int err = CANOPY_SUCCESS;
	struct canopy_var *var;
	if (emit_obj) {
		err = c_json_emit_open_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit opening object err: %d\n",
					err);
			return CANOPY_ERROR_JSON;
		}
	}

	err = c_json_emit_name_and_object(state, "vars");
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG, "unable to emit vars err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	var = device->vars;
	while (var != NULL) {
		char * name = var->name;
		canopy_var_datatype type = var->type;

		switch (type) {
			break;

		case CANOPY_VAR_DATATYPE_STRING:
			return CANOPY_ERROR_NOT_IMPLEMENTED;

		case CANOPY_VAR_DATATYPE_BOOL:
			snprintf(buffer, sizeof(buffer), "%s", (var->val.value.val_bool ? "true" : "false"));
			break;

		case CANOPY_VAR_DATATYPE_INT8:
			snprintf(buffer, sizeof(buffer), "%d", var->val.value.val_int8);
			break;

		case CANOPY_VAR_DATATYPE_INT16:
			snprintf(buffer, sizeof(buffer), "%d", var->val.value.val_int16);
			break;

		case CANOPY_VAR_DATATYPE_INT32:
			snprintf(buffer, sizeof(buffer), "%d", var->val.value.val_int32);
			break;

		case CANOPY_VAR_DATATYPE_UINT8:
			snprintf(buffer, sizeof(buffer), "%u", var->val.value.val_uint8);
			break;

		case CANOPY_VAR_DATATYPE_UINT16:
			snprintf(buffer, sizeof(buffer), "%u", var->val.value.val_uint16);
			break;

		case CANOPY_VAR_DATATYPE_UINT32:
			snprintf(buffer, sizeof(buffer), "%u", var->val.value.val_uint32);
			break;

		case CANOPY_VAR_DATATYPE_FLOAT32:
			snprintf(buffer, sizeof(buffer), "%e", var->val.value.val_float);
			break;

		case CANOPY_VAR_DATATYPE_FLOAT64:
			snprintf(buffer, sizeof(buffer), "%e", var->val.value.val_double);
			break;

		case CANOPY_VAR_DATATYPE_DATETIME:
			snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)var->val.value.val_time);
			break;

		case CANOPY_VAR_DATATYPE_VOID:
		case CANOPY_VAR_DATATYPE_STRUCT:
		case CANOPY_VAR_DATATYPE_ARRAY:
		case CANOPY_VAR_DATATYPE_INVALID:
		default:
			cos_log(LOG_LEVEL_FATAL, "invalid type code %d\n", type);
			return CANOPY_ERROR_FATAL;
		} /* switch(type) */

		err = c_json_emit_name_and_value(state, name, buffer);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit variable: %s err: %d\n",
					name, err);
			return CANOPY_ERROR_JSON;
		}
		var = var->next;
	}

	err = c_json_emit_close_object(state);
	if (err != C_JSON_OK) {
		cos_log(LOG_LEVEL_DEBUG,
				"unable to emit vars closing object err: %d\n", err);
		return CANOPY_ERROR_JSON;
	}

	if (emit_obj) {
		err = c_json_emit_close_object(state);
		if (err != C_JSON_OK) {
			cos_log(LOG_LEVEL_DEBUG, "unable to emit closing object err: %d\n",
					err);
			return CANOPY_ERROR_JSON;
		}
	}
	return err;
}


/***************************************************************************
 * 	c_json_parse_vars(struct canopy_device *device,
 *		char* js, int js_len, jsmntok_t *token, int tok_len,
 *		int current)
 *
 * 		parses the JSON vardecl tag from the server to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_parse_vars(struct canopy_device *device,
		char* js, int js_len, 			/* the input JSON and total length  */
		jsmntok_t *token, int tok_len,	/* token array with length */
		int offset,				/* token offset for name vardecl */
		int *next_token,				/* the token after the decls */
		bool check_obj) {				/* expect outer-most object */

	int err = CANOPY_SUCCESS;
	int i;
	char name[128];
	memset(&name, 0, sizeof(name));
	char primative[128];
	memset(&primative, 0, sizeof(primative));

	COS_ASSERT(device != NULL);
	COS_ASSERT(device->remote != NULL);

	/*
	 * 		JSON.  This is how
	 *
	 *  "{ "
	 *	"    \"vars\" : {	"
	 *	"        \"temperature\" : 43.0,	"
	 *	"        \"humidity\" : 32.41,	"
	 *	"        \"dimmer_level\" : 4,	"
	 *	"        \"happy\" : true	"
	 *	"    }	"
	 *	"}	";
	 *
	 * 	gets parsed to
	 *
	 * 			outer object
	 * 	{type = JSMN_OBJECT, start = 0, end = 132, size = 1},
	 * 			"vars" : {
	 *	{type = JSMN_STRING, start = 6, end = 10, size = 1},
	 * 	{type = JSMN_OBJECT, start = 14, end = 130, size = 4},
	 *
	 *			"tempurature" : 43.0
	 * 	{type = JSMN_STRING, start = 25, end = 36, size = 1},
	 * 	{type = JSMN_PRIMITIVE, start = 40, end = 44, size = 0},
	 *
	 * 			"humidity" : 32.41
	 * 	{type = JSMN_STRING, start = 55, end = 63, size = 1},
	 * 	{type = JSMN_PRIMITIVE, start = 67, end = 72, size = 0},
	 *
	 * 			"dimmer_level" : 4
	 * 	{type = JSMN_STRING, start = 83, end = 95, size = 1},
	 * 	{type = JSMN_PRIMITIVE, start = 99, end = 100, size = 0},
	 *
	 * 			"happy" : true
	 * 	{type = JSMN_STRING, start = 111, end = 116, size = 1},
	 * 	{type = JSMN_PRIMITIVE, start = 120, end = 124, size = 0}
	 *
	 */

	COS_ASSERT(token[offset].type == JSMN_STRING);
	COS_ASSERT(strncmp((const char*) &js[token[offset].start], "vars", (token[offset].end - token[offset].start)) == 0);
	COS_ASSERT(token[offset].size == 1);
	offset++;

	/*
	 * We should be at the object after the vars.  The size of this object indicates
	 * the number of entries in the list times 2.
	 * 		The first thing should be a string that has the name in it.  Its size should be 1.
	 * 		The next token is the value, which should have a size of 0.
	 *
	 * repeat as necessary.........
	 */
	COS_ASSERT(token[offset].type == JSMN_OBJECT);
	int n_vars = token[offset].size;
	offset++;
	for (i = 0; i < n_vars; i++) {
		memset(&name, 0, sizeof(name));
		memset(&primative, 0, sizeof(primative));


		/*
		 * the token at offset should be the string we need to parse,
		 */
		COS_ASSERT(token[offset].type == JSMN_STRING);
		COS_ASSERT(token[offset].size == 1);
		strncpy(name, &js[token[offset].start], (token[offset].end - token[offset].start));
		name[(token[offset].start - token[offset].end)] = '\0';
		offset++;

		COS_ASSERT(token[offset].type == JSMN_PRIMITIVE);
		COS_ASSERT(token[offset].size == 0);
		strncpy(primative, &js[token[offset].start], (token[offset].end - token[offset].start));
		primative[(token[offset].start - token[offset].end)] = '\0';

		/*
		 * We've got the name, and the primative, now we need to look to see if this variable has
		 * already been defined.  If it has, we need to update the varaible.
		 */
		struct canopy_var* var = find_name(device, (const char*) name);
		if (var != NULL) {
			int v;
			double dv;
			unsigned long long ull;

			/*
			 * We found the variable, get the value based on the var we find,
			 */
			canopy_var_datatype type = var->type;
			switch (type) {
				break;

			case CANOPY_VAR_DATATYPE_STRING:
				return CANOPY_ERROR_NOT_IMPLEMENTED;

			case CANOPY_VAR_DATATYPE_BOOL:
				if (strncmp(primative, "true", sizeof(primative)) == 0) {
					var->val.value.val_bool = true;
				} else if (strncmp(primative, "false", sizeof(primative)) == 0) {
					var->val.value.val_bool = false;
				} else {
					/* something bad happened */
					return CANOPY_ERROR_FATAL;
				}
				break;

			case CANOPY_VAR_DATATYPE_INT8:
				v = atoi(primative);
				var->val.value.val_int8 = (int8_t)v;
				break;

			case CANOPY_VAR_DATATYPE_INT16:
				v = atoi(primative);
				var->val.value.val_int16 = (int16_t)v;
				break;

			case CANOPY_VAR_DATATYPE_INT32:
				v = atoi(primative);
				var->val.value.val_int32 = (int32_t)v;
				break;

			case CANOPY_VAR_DATATYPE_UINT8:
				v = atoi(primative);
				var->val.value.val_uint8 = (uint8_t)v;
				break;

			case CANOPY_VAR_DATATYPE_UINT16:
				v = atoi(primative);
				var->val.value.val_uint16 = (uint16_t)v;
				break;

			case CANOPY_VAR_DATATYPE_UINT32:
				v = atoi(primative);
				var->val.value.val_uint32 = (uint32_t)v;
				break;

			case CANOPY_VAR_DATATYPE_FLOAT32:
				dv = atof(primative);
				var->val.value.val_float = (float)dv;
				break;

			case CANOPY_VAR_DATATYPE_FLOAT64:
				dv = atof(primative);
				var->val.value.val_double = dv;
				break;

			case CANOPY_VAR_DATATYPE_DATETIME:
				ull = atoll(primative);
				var->val.value.val_time = (cos_time_t)ull;
				break;

			case CANOPY_VAR_DATATYPE_VOID:
			case CANOPY_VAR_DATATYPE_STRUCT:
			case CANOPY_VAR_DATATYPE_ARRAY:
			case CANOPY_VAR_DATATYPE_INVALID:
			default:
				cos_log(LOG_LEVEL_FATAL, "invalid type code %d\n", type);
				return CANOPY_ERROR_FATAL;
			} /* switch(type) */


		} else {

			/*
			 * We didn't find it, so this is an error.
			 */
			return CANOPY_ERROR_VAR_NOT_FOUND;
		}

		/*
		 * Go on to the next token
		 */
		offset++;
	} /* var loop */

	*next_token = offset;

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

