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



#include	<canopy_min_internal.h>
#include	<canopy_min.h>
#include	<canopy_os.h>

#ifdef DOCUMENTATION
struct c_json_state {
	char	*buffer;	/* the buffr being built in */
	int		buffer_len;	/* how big is the raw buffer */
	int		offset;		/* offset where the \0 is relative to the start of the buffer*/
	int		indent;
};
#endif

#define C_JSON_MAX_STRING_LEN  64	/* Maximum object or member name length */
#define C_JSON_MAX_VALUE_LEN  128	/* Maximum value name length */
typedef enum {
	C_JOSN_VAL_STR,
	C_JOSN_VAL_INT,
	C_JOSN_VAL_UINT,
	C_JOSN_VAL_FLOAT,
} c_json_data_types;

#if 0
static const char **indent_spaces = {
		{ "    " },
		{ "        " },
		{"            " },
		{ "                " }
};
#endif

/*****************************************************************************/

/*		static definitions go here */

/*****************************************************************************/


/*******************************************************
 * Initialize the buffer to use to build json string
 */
int c_json_buffer_init(struct c_json_state *state, char *buffer, int len) {

	memset(state, 0, sizeof(struct c_json_state));
	state->buffer = buffer;
	state->buffer_len = len;
	state->offset = 0;
	state->indent = 0;
	return C_JSON_OK;
}


static int check_buffer_length(struct c_json_state *state)
{
	/*
	 * Check for buffer overflow condition here, and then copy remaining
	 * data using snprintf. This makes sure there is no mem corruption in
	 * json set operations.
	 */
	if (state->offset >= (state->buffer_len - 1)) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space check_buffer_length()");
		return -1;
	} else
		return 0;
}


/*
 * These procedures emit tokens into the buffer supplied above.
 */
/*
 * Emits
 * 		{
 */
int c_json_emit_open_object(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_open_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	/* snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n%s{\n", indent_spaces[state->indent]); */
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    {\n");
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * Emits
 * 		}
 */
int c_json_emit_close_object(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_close_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	/* snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n%s{\n", indent_spaces[state->indent]); */
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    }\n");
	state->indent--;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * Emits
 * 		[
 */
int c_json_emit_open_array(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_open_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	/* snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n%s[\n", indent_spaces[state->indent]); */
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    [\n");
	state->indent--;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * Emits
 * 		]
 */
int c_json_emit_close_array(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_close_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	/* snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n%s]\n", indent_spaces[state->indent]); */
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    ]\n");
	state->indent--;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * Emits:
 * 		"name" : "value"
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name, char *value) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_name_and_value()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    \"%s\" : \"%s\"  \n", name, value);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * emits:
 * 		"name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_name_and_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    \"%s\" {  \n", name);
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_name_and_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    \"%s\" : [  \n", name);
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/*
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_boolean(struct c_json_state *state, char *name, bool out) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG, "buffer out of space c_json_emit_name_and_value()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    \"%s\" : %s  \n", name, (out ? "true" : "false"));
	state->offset = strlen(state->buffer);
	return C_JSON_OK;

}




