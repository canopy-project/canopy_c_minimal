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

#include	<canopy_min.h>



/*
 * Procedures related to canopy_remote's
 */
canopy_error canopy_cleanup_remote(struct canopy_remote *remote);


/******************************************************************************
 * 	JSON stuff.
 */
struct c_json_state {
	char	*buffer;	/* the buffr being built in */
	int		buffer_len;	/* how big is the raw buffer */
	int		end;		/* where the \0 is */
	int		indent;
};

#define C_JSON_MAX_STRING_LEN  64	/* Maximum object or member name length */
#define C_JSON_MAX_VALUE_LEN  128	/* Maximum value name length */
typedef enum {
	C_JOSN_VAL_STR,
	C_JOSN_VAL_INT,
	C_JOSN_VAL_UINT,
	C_JOSN_VAL_FLOAT,
} c_json_data_types;

#define	C_JSON_OK				0x0000
#define	C_JSON_BUFFER_OVERFLOW	0x0001

/*
 * Initialize the buffer to use to build json string
 */
int c_json_buffer_init(struct c_json_state *state, char *buffer, int len);

/*
 * These procedures emit tokens into the buffer supplied above.
 */
/*
 * Emits
 * 		{
 */
int c_json_emit_open_object(struct c_json_state *state);

/*
 * Emits
 * 		}
 */
int c_json_emit_close_object(struct c_json_state *state);

/*
 * Emits
 * 		[
 */
int c_json_emit_open_array(struct c_json_state *state);

/*
 * Emits
 * 		]
 */
int c_json_emit_close_array(struct c_json_state *state);

/*
 * Emits:
 * 		"name" : "value"
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name, char *value);

/*
 * emits:
 * 		"name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name);

/*
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name);




#endif	/* CANOPY_MIN_INTERNAL_INCLUDED */
