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

#include	<jsmn/jsmn.h>

#include	<canopy_min_internal.h>
#include	<canopy_min.h>
#include	<canopy_os.h>

#ifdef DOCUMENTATION
struct c_json_state {
	char *buffer; /* the buffr being built in */
	int buffer_len; /* how big is the raw buffer */
	int offset; /* offset where the \0 is relative to the start of the buffer*/
	int indent;
    bool separator_needed[MAX_JSON_STACK_DEPTH];
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

static const char *indent_spaces[] = {
		"    ",
		"        ",
		"            ",
		"                ", };

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
	state->stack_depth = 0;
	return C_JSON_OK;
}

/******************************************************************************
 *	Check to see if there's space left in the buffer.
 */
static int check_buffer_length(struct c_json_state *state) {
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
/******************************************************************************
 * Emits
 * 		{
 * or (if state->prepend_separator[state->stack_depth] is true):
 * 		, {
 */
int c_json_emit_open_object(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_open_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s{\n", indent_spaces[state->indent],
            (state->prepend_separator[state->stack_depth] ? ", " : ""));
	state->indent++;
	state->offset = strlen(state->buffer);

	state->stack_depth++;
	ASSERTION_CHECK(state->stack_depth < MAX_JSON_STACK_DEPTH, CANOPY_ERROR_USAGE);
    state->prepend_separator[state->stack_depth] = false;
	return C_JSON_OK;
}

/******************************************************************************
 * Emits
 * 		}
 */
int c_json_emit_close_object(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_close_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	state->indent--;
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s}\n", indent_spaces[state->indent]);
	state->offset = strlen(state->buffer);

    ASSERTION_CHECK(state->stack_depth > 0, CANOPY_ERROR_USAGE);
    state->stack_depth--;
    state->prepend_separator[state->stack_depth] = true;
	return C_JSON_OK;
}

/******************************************************************************
 * Emits
 * 		[
 * 	(TBD how should commas be generated?)
 */
int c_json_emit_open_array(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_open_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s[\n", indent_spaces[state->indent]);
	state->indent++;
	state->offset = strlen(state->buffer);

	return C_JSON_OK;
}

/******************************************************************************
 * Emits
 * 		]
 */
int c_json_emit_close_array(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_close_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	state->indent--;
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s]\n", indent_spaces[state->indent]);
	// snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    ]\n");
	state->offset = strlen(state->buffer);
    state->prepend_separator[state->stack_depth] = true;
	return C_JSON_OK;
}

/******************************************************************************
 * Emits:
 * 		"name" : value
 * or (if state->prepend_separator[state->stack_depth] is true):
 * 		, "name" : value
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name,
		char *value) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_value()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s\"%s\" : %s  \n", indent_spaces[state->indent], 
            (state->prepend_separator[state->stack_depth] ? ", " : ""),
            name, value);
	state->offset = strlen(state->buffer);
    state->prepend_separator[state->stack_depth] = true;
	return C_JSON_OK;
}

/******************************************************************************
 * Emits:
 * 		"name" : {
 * or (if state->prepend_separator[state->stack_depth] is true):
 * 		, "name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s\"%s\" : {  \n", indent_spaces[state->indent],
            (state->prepend_separator[state->stack_depth] ? ", " : ""),
            name);
	state->indent++;
	state->offset = strlen(state->buffer);

	state->stack_depth++;
    ASSERTION_CHECK(state->stack_depth < MAX_JSON_STACK_DEPTH, CANOPY_ERROR_USAGE);
    state->prepend_separator[state->stack_depth] = false;
	return C_JSON_OK;
}

/******************************************************************************
 * Emits:
 * 		"name" : [
 * or (if state->prepend_separator[state->stack_depth] is true):
 * 		, "name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s\"%s\" : [  \n", indent_spaces[state->indent], 
            (state->prepend_separator[state->stack_depth] ? ", " : ""),
            name);
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}


/******************************************************************************
 * Parse the string using jsmn
 */
int c_json_parse_string(char* js, int js_len, jsmntok_t *token, int tok_len,
		int *active) {
	int r;
	jsmn_parser p;

	jsmn_init(&p);
	r = jsmn_parse(&p, js, js_len, token, tok_len);
	if (r < 0) {
		return r;
	}
	*active = r;
	return C_JSON_OK;
}

/***************************************************************************
 * Returns the value of the result string.
 */
int c_json_get_result_key(char* js, int js_len, jsmntok_t *token, int tok_len,
		int active, bool *result) {
	char* err = "no error";
	if (tok_len < 2 || tok_len < active) {
		err = "token length error";
		goto error;
	}
	if (token[0].type != JSMN_OBJECT) {
		err = "initial JSON is not an object";
		goto error;
	}
	if (token[1].type != JSMN_STRING) {
		err = "first token is not a string";
	}
	if (token[1].size != 1) {
		err = "first token isn't size 1";
		goto error;
	}
	/*
	 * The token has the offset of the first byte of the token into the passed
	 * in JSON string.  It also has the end offset in the string.  This value
	 * is the first byte outside the token, so you treat it like a length.
	 *
	 * OK, there's a big hack here to avoid allocating any additional memory
	 * for the string.
     *
     * TODO: Accept fields in arbitrary order
	 */
	// int equal = CHECK_TOKEN_STRING(js, token[1], "result");
	int equal = strncmp((const char*) &js[token[1].start], "result",
			(token[1].end - token[1].start));
	if (equal != 0) {
		err = "first token isn't 'result'";
		goto error;
	}

	if (token[2].type != JSMN_STRING) {
		err = "second token is not a string";
		goto error;
	}

	int ok = strncmp(&js[token[2].start], "ok",
			(token[2].end - token[2].start));
	int error = strncmp(&js[token[2].start], "error",
			(token[2].end - token[2].start));
	if (ok == 0 || error == 0) {
		*result = (ok == 0);
	} else {
		err = "result isn't 'ok' or 'error'";
		goto error;
	}
	return C_JSON_OK;
	error: cos_log(LOG_LEVEL_DEBUG, err);
	return C_JSON_PARSE_ERROR;
}

