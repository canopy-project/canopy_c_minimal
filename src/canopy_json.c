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
 */
int c_json_emit_open_object(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_open_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s{\n", indent_spaces[state->indent]);
	// snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    {\n");
	state->indent++;
	state->offset = strlen(state->buffer);
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
	// snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    }\n");
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * Emits
 * 		[
 */
int c_json_emit_open_array(struct c_json_state *state) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_open_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s[\n", indent_spaces[state->indent]);
	// snprintf(&state->buffer[state->offset], state->buffer_len - state->offset, "\n    [\n");
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
	return C_JSON_OK;
}

/******************************************************************************
 * Emits:
 * 		"name" : "value"
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name,
		char *value) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_value()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s\"%s\" : \"%s\"  \n", indent_spaces[state->indent], name, value);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_object()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s\"%s\" {  \n", indent_spaces[state->indent], name);
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_array()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s\"%s\" : [  \n", indent_spaces[state->indent], name);
	state->indent++;
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_boolean(struct c_json_state *state, char *name,
		bool out) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_boolean()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s\"%s\" : %s  \n", indent_spaces[state->indent], name,
			(out ? "true" : "false"));
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : integer based on size
 */
int c_json_emit_name_and_int(struct c_json_state *state, char *name,
		int32_t out, int size) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_int()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	char *fmt;
	if (size == 1) {
		fmt = "%s\"%s\" : %1d  \n";
	} else if (size == 2) {
		fmt = "%s\"%s\" : %2d  \n";
	} else if (size == 4) {
		fmt = "%s\"%s\" : %4d  \n";
	} else {
		return C_JSON_INVALID_SIZE;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			fmt, indent_spaces[state->indent], name, out);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : unsigned integer based on size
 */
int c_json_emit_name_and_uint(struct c_json_state *state, char *name,
		uint32_t out, int size) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_int()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	char *fmt;
	if (size == 1) {
		fmt = "%s\"%s\" : %1u  \n";
	} else if (size == 2) {
		fmt = "%s\"%s\" : %2u  \n";
	} else if (size == 4) {
		fmt = "%s\"%s\" : %4u  \n";
	} else {
		return C_JSON_INVALID_SIZE;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			fmt, indent_spaces[state->indent], name, out);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : signed 64 bit integer
 */
int c_json_emit_name_and_int64(struct c_json_state *state, char *name,
		int64_t out) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_int()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s : %lld\n", indent_spaces[state->indent], name,
			(unsigned long long) out);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : float
 */
int c_json_emit_name_and_float32(struct c_json_state *state, char *name,
		float out) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_int()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s : %e\n", indent_spaces[state->indent], name, (double) out);
	state->offset = strlen(state->buffer);
	return C_JSON_OK;
}

/******************************************************************************
 * emits:
 * 		"name" : double
 */
int c_json_emit_name_and_float64(struct c_json_state *state, char *name,
		double out) {
	if (check_buffer_length(state) != 0) {
		cos_log(LOG_LEVEL_DEBUG,
				"buffer out of space c_json_emit_name_and_int()");
		return C_JSON_BUFFER_OVERFLOW;
	}
	snprintf(&state->buffer[state->offset], state->buffer_len - state->offset,
			"%s%s : %e\n", indent_spaces[state->indent], name, out);
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

