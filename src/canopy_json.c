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


#include	<canopy_min_internal.h>
#include	<canopy_min.h>
#include	<canopy_os.h>


/*****************************************************************************/

/*		static definitions go here */

/*****************************************************************************/


/*
 * Initialize the buffer to use to build json string
 */
int c_json_buffer_init(struct c_json_state *state, char *buffer, int len) {

	return C_JSON_OK;
}

/*
 * These procedures emit tokens into the buffer supplied above.
 */
/*
 * Emits
 * 		{
 */
int c_json_emit_open_object(struct c_json_state *state) {

	return C_JSON_OK;
}

/*
 * Emits
 * 		}
 */
int c_json_emit_close_object(struct c_json_state *state) {

	return C_JSON_OK;
}

/*
 * Emits
 * 		[
 */
int c_json_emit_open_array(struct c_json_state *state) {

	return C_JSON_OK;
}

/*
 * Emits
 * 		]
 */
int c_json_emit_close_array(struct c_json_state *state) {

	return C_JSON_OK;
}

/*
 * Emits:
 * 		"name" : "value"
 */
int c_json_emit_name_and_value(struct c_json_state *state, char *name, char *value) {

	return C_JSON_OK;
}

/*
 * emits:
 * 		"name" : {
 */
int c_json_emit_name_and_object(struct c_json_state *state, char *name) {

	return C_JSON_OK;
}

/*
 * emits:
 * 		"name" : [
 */
int c_json_emit_name_and_array(struct c_json_state *state, char *name) {

	return C_JSON_OK;
}




