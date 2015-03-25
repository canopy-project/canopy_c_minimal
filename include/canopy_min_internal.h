// Copyright 2015 SimpleThings, Inc.
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

#ifndef CANOPY_MIN_INTERNAL_INCLUDED
#define CANOPY_MIN_INTERNAL_INCLUDED

#include	<stdbool.h>
#include	<stdint.h>

#include	<jsmn/jsmn.h>

#include	<canopy_min.h>



/***************************************************************************
 * Procedures related to canopy_remote's
 */
canopy_error canopy_cleanup_remote(struct canopy_remote *remote);


/******************************************************************************/
/*	device stuff
 *
 */
canopy_error initialize_device(struct canopy_device *device, struct canopy_remote *remote);

/******************************************************************************/
/******************************************************************************
 * 	JSON stuff.
 */
struct c_json_state {
	char	*buffer;	/* the buffr being built in */
	int		buffer_len;	/* how big is the raw buffer */
	int		offset;		/* where the \0 is */
	int		indent;
};

#if 0
#define C_JSON_MAX_STRING_LEN  64	/* Maximum object or member name length */
#define C_JSON_MAX_VALUE_LEN  128	/* Maximum value name length */
typedef enum {
	C_JOSN_VAL_STR,
	C_JOSN_VAL_INT,
	C_JOSN_VAL_UINT,
	C_JOSN_VAL_FLOAT,
} c_json_data_types;
#endif


#define	C_JSON_OK				0x0000
#define	C_JSON_BUFFER_OVERFLOW	0x0001
#define	C_JSON_INVALID_SIZE		0x0002
#define	C_JSON_PARSE_ERROR		0x0004


/***************************************************************************
 * Initialize the buffer to use to build json string
 */
extern int c_json_buffer_init(struct c_json_state *state, char *buffer, int len);

/***************************************************************************
 * These procedures emit tokens into the buffer supplied above.
 */
/***************************************************************************
 * Emits
 * 		{
 */
int c_json_emit_open_object(struct c_json_state *state);

/***************************************************************************
 * Emits
 * 		}
 */
int c_json_emit_close_object(struct c_json_state *state);

/***************************************************************************
 * Emits
 * 		[
 */
int c_json_emit_open_array(struct c_json_state *state);

/***************************************************************************
 * Emits
 * 		]
 */
int c_json_emit_close_array(struct c_json_state *state);

/***************************************************************************
 * Emits:
 * 		"name" : "value"
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name, char *value);

/***************************************************************************
 * emits:
 * 		"name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name);

/***************************************************************************
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name);

/***************************************************************************
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_boolean(struct c_json_state *state, char *name, bool out);

/***************************************************************************
 * emits:
 * 		"name" : integer based on size
 */
int c_json_emit_name_and_int(struct c_json_state *state, char *name, int32_t out, int size);

/***************************************************************************
 * emits:
 * 		"name" : unsigned integer based on size
 */
int c_json_emit_name_and_uint(struct c_json_state *state, char *name, uint32_t out, int size);

/***************************************************************************
 * emits:
 * 		"name" : signed 64 bit integer
 */
int c_json_emit_name_and_int64(struct c_json_state *state, char *name, int64_t out);

/***************************************************************************
 * emits:
 * 		"name" : float
 */
int c_json_emit_name_and_float32(struct c_json_state *state, char *name, float out);

/***************************************************************************
 * emits:
 * 		"name" : double
 */
int c_json_emit_name_and_float64(struct c_json_state *state, char *name, double out);


/***************************************************************************
 * 	c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the remote. (in canopy_variables.c)
 */
canopy_error c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state);

/***************************************************************************/
/***************************************************************************/
/*
 * errors from jsmn
 */
/* Not enough tokens were provided */
#define	JSMN_ERROR_NOMEM 	 -1,
/* Invalid character inside JSON string */
#define JSMN_ERROR_INVAL 	 -2,
/* The string is not a full JSON packet, more bytes expected */
#define JSMN_ERROR_PART 	 -3

#define CHECK_TOKEN_STRING(js, t, constant) \
	(strncmp(&js[(t).start], constant, (t).end - (t).start) == 0)

// int equal = strncmp((const char*)&js[token[1].start], "result", (token[1].end - token[1].start));

/***************************************************************************
 * Parse the json string into tokens.
 */
int c_json_parse_string(char* js, int js_len, jsmntok_t *token, int tok_len, int *active);

/***************************************************************************
 * Returns the value of the result string.
 * 	Each JSON returned has the tag "result" : STR as the second token.  (The
 * 	first token is the open object '{'  The expected response is either 'ok' or
 * 	'error', which is updated in result.  If the resonse isn't expected the
 * 	call returns C_JSON_PARSE_ERROR.
 */
int c_json_get_result_key(char* js, int js_len, jsmntok_t *token, int tok_len, int active, bool *result);

#endif	/* CANOPY_MIN_INTERNAL_INCLUDED */
