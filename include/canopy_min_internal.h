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


/******************************************************************************
 * JSON 'tags'
 */
#define		TAG_RESULT				"result"
#define		TAG_USERNAME			"username"
#define		TAG_PASSWORD			"password"
#define		TAG_OLD_USERNAME		"old_username"
#define		TAG_OLD_PASSWORD		"old_password"
#define		TAG_EMAIL				"email"
#define		TAG_SKIP_EMAIL			"skip-email"
#define		TAG_VALIDATED			"validated"
#define		TAG_QUANTITY			"quantity"
#define		TAG_FRIENDLY_NAMES		"friendly_names"	/* note plural */
#define		TAG_COUNT				"count"
#define		TAG_DEVICES				"devices"
#define		TAG_FRIENDLY_NAME		"friendly_name"		/* note not plural */
#define		TAG_DEVICE_ID			"device_id"
#define		TAG_UUID				"uuid"
#define		TAG_DEVICE_SECRET_KEY	"device_secret_key"


/*					status object tags */
#define		TAG_STATUS				"status"
#define		TAG_WS_CONNECTED			"ws_connected"
#define		TAG_ACTIVE_STATUS			"active_status"
#define		TAG_LAST_ACTIVITY_TIME		"last_activity_time"

/*					historical data tags */
#define		TAG_CURRENT_CLOCK_US	"current_clock_us"	/* uint64_t */
#define		TAG_CURRENT_CLOCK_UTC	"current_clock_utc"	/* string */
#define		TAG_SAMPLES				"samples"
#define		TAG_T					"t"
#define		TAG_V					"v"

#define		TAG_LOCATION_NOTE		"location_note"

#define		TAG_VAR_DECLS			"var_decls"
#define		TAG_VARS				"vars"


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
 * 		"name" : "string"
 */
int c_json_emit_name_and_string(struct c_json_state *state, char *name, char *sring);

/***************************************************************************
 * Emits:
 * 		"name" : value
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

/******************************************************************************/
/******************************************************************************/

/* variable related stuff */

/***************************************************************************
 * 	c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_emit_vardcl(struct canopy_device *device, struct c_json_state *state, bool emit_obj);


/***************************************************************************
 * 	c_json_parse_vardcl(struct canopy_device *device,
 *		char* js, int js_len, jsmntok_t *token, int tok_len,
 *		int current)
 *
 * 		parses the JSON vardecl tag from the server to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_parse_vardcl(struct canopy_device *device,
		char* js, int js_len, 			/* the input JSON and total length  */
		jsmntok_t *token, int tok_len,	/* token array with length */
		int name_offset,				/* token offset for name vardecl */
		int *next_token,				/* the token after the decls */
		bool check_obj);				/* expect outer-most object */


/***************************************************************************
 * 	c_json_emit_vars(struct canopy_device *device, struct c_json_state *state)
 *
 * 		creates the JSON  request to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_emit_vars(struct canopy_device *device,
		struct c_json_state *state,
		bool emit_obj);


/***************************************************************************
 * 	c_json_parse_vars()
 *
 * 		parses the JSON vardecl tag from the server to register the variables that are registered
 * 	with the device. (in canopy_variables.c)
 */
canopy_error c_json_parse_vars(struct canopy_device *device,
		char* js, int js_len, 			/* the input JSON and total length  */
		jsmntok_t *token, int tok_len,	/* token array with length */
		int name_offset,				/* token offset for name vardecl */
		int *next_token,				/* the token after the decls */
		bool check_obj);				/* expect outer-most object */


/***************************************************************************
 *  c_json_parse_device)
 *
 *      parses the JSON vardecl tag from the server to register the variables that are registered
 *  with the device. (in canopy_device.c)
 */
canopy_error c_json_parse_device(struct canopy_device *device,
        char* js, int js_len,           /* the input JSON and total length  */
        jsmntok_t *token, int tok_len,  /* token array with length */
        bool *result_code,              /* the value of "result : " */
        bool check_obj);                /* expect outer-most object */

/***************************************************************************
 *  c_json_parse_remote_status()
 *
 *      parses the JSON vardecl tag from the server to register the variables that are registered
 *  with the device. (in canopy_remote.c)
 */
canopy_error c_json_parse_remote_status(struct canopy_device *device,
        char* js, int js_len,           /* the input JSON and total length  */
        jsmntok_t *token, int tok_len,  /* token array with length */
        int name_offset,                /* token offset for name vardecl */
        int *next_token);                /* the token after the decls */


#endif	/* CANOPY_MIN_INTERNAL_INCLUDED */
