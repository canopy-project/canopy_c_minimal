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

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<errno.h>

#include	<stdint.h>
#include	<stdbool.h>
#include	<string.h>

#include	<jsmn/jsmn.h>

#include	<canopy_min_internal.h>
#include	<canopy_min.h>
#include	<canopy_os.h>

static int test_passed = 0;
static int test_failed = 0;

/* Test runner */
static void test(int (*func)(void), const char *name) {
	int r = func();
	if (r == 0) {
		test_passed++;
	} else {
		test_failed++;
		printf("FAILED: %s (at line %d)\n", name, r);
	}
}

static const char* canopy2 = "{"
"    \"result\" : \"ok\", "
"    \"device_id\" : \"d5c827d2-1ba7-4da6-b87b-0864f0969fa8\", "
"    \"friendly_name\" : \"My Toaster 17\", "
"    \"status\" : { "
"        \"ws_connected\" : true, "
"        \"active_status\" : \"active\", "
"        \"last_activity_time\" : 1426803897000000, "
"    }, "
"    \"var_decls\" : { "
"        \"out float32 temperature\" : { }, "
"        \"out float32 humidity\" : { }, "
"        \"in int8 dimmer_brightness\" : { }, "
"        \"in bool reboot_now\" : { },	"
"    },	"
"    \"vars\" : {	"
"        \"temperature\" : {	"
"            \"t\" : 1426803897000000,	"
"            \"v\" : 37.4,	"
"        },	"
"        \"humidity\" : {	"
"            \"t\" : 1426803897000000,	"
"            \"v\" : 92.3,	"
"        },	"
"        \"dimmer_brightness\" : {	"
"            \"t\" : 1426803897000000,	"
"            \"v\" : 0,	"
"        },	"
"        \"reboot_now\" : {	"
"            \"t\" : 1426803897000000,	"
"            \"v\" : false,	"
"        }	"
"    }	"
"}	";


int test_result() {
	int out;
	jsmntok_t *token;
	bool answer;
	int active = 0;
	int tok_len = 128;
	token = (jsmntok_t*)malloc(sizeof(jsmntok_t) * tok_len);

	out = c_json_parse_string((char*)canopy2, strlen(canopy2), token, tok_len, &active);
	printf("parse string returned %d active tokens %d\n", out, active);
	out = c_json_get_result_key((char*)canopy2, strlen(canopy2), token, tok_len, active, &answer);
	printf("get result key returned %d, answer: %d\n", out, answer);
	printf("%s\n\n", canopy2);

	return 0;
}

int test_vardcl_output() {
	int out;
	struct c_json_state state;
	char *buffer = (char*)malloc(1024);
	memset(buffer, 0, 1024);
	out = c_json_buffer_init(&state, buffer, 1024);
	if (out != 0) {
		printf("c_json_buffer_init() returned: %d", out);
		return -1;
	}

	struct canopy_device device;
	canopy_error err = initialize_device(&device, NULL);
	if (err != CANOPY_SUCCESS) {
		printf("initialize_device returned: %d", err);
		return -2;
	}

	struct canopy_var out_var;
	err = canopy_device_var_declare(&device,
			CANOPY_VAR_OUT,
			CANOPY_VAR_DATATYPE_BOOL,
	       "test_var",
			&out_var);
	if (err != CANOPY_SUCCESS) {
		printf("canopy_device_var_declare name: %s returned: %d", "test_var", err);
		return -3;
	}

	err = c_json_emit_vardcl(&device, &state);
	if (err != CANOPY_SUCCESS) {
		printf("c_json_emit_vardcl() returned: %d", err);
		return -4;
	}

	printf("\n%s\n\n", buffer);

	return 0;
}

int main() {
	test(test_result, "general test for the 'result' tag");
	test(test_vardcl_output, "test emit_vardcl");
}



