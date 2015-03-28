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

#define TOASTER_UUID "e43eb410-48da-421a-b07d-1cd751412fd5"
#define TOASTER_SECRET_KEY "wYI0G+HQN9fj76fpyxwiHKJDQags0dpM"
#define REMOTE_ADDR "dev02.canopy.link"


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

/******************************************************************************
 * string which is returned by a call to one of the device calls
 */
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


/*****************************************************************************
 * 		test_result
 */
int test_result() {
	int out;
	int tok_len = 128;
	jsmntok_t tokens[128];
	const char *js;
	int i;
	int ti = 1;

	js = canopy2;
	memset(&tokens, 0, sizeof(jsmntok_t) * tok_len);
	bool answer;
	int active = 0;

	out = c_json_parse_string((char*)canopy2, strlen(canopy2), tokens, tok_len, &active);
	printf("parse string returned %d active tokens %d\n", out, active);
	out = c_json_get_result_key((char*)canopy2, strlen(canopy2), tokens, tok_len, active, &answer);
	printf("get result key returned %d, answer: %d\n", out, answer);
	printf("%s\n\n", canopy2);

	return 0;
}

/****************************************************************************
 * 	test_vardcl_output()
 */
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

	err = c_json_emit_vardcl(&device, &state, true);
	if (err != CANOPY_SUCCESS) {
		printf("c_json_emit_vardcl() returned: %d", err);
		return -4;
	}

	printf("\n%s\n\n", buffer);

	return 0;
}


/*
 * Input string for var_decls piece of JSON
 */
static const char* var_decls1 = "{"
		"    \"var_decls\" : {  "
		"        \"out bool test_var\" {  "
		"        }	"
		"    }	"
		"}";

static const char* var_decls2 = "{"
		"    \"var_decls\" : {  "
		"        \"out bool test_var\" {  "
		"        }	"
		"        \"in datetime  test_time\" {  "
		"        }	"
		"    }	"
		"}";

static const char* var_decls3 = "{"
		"    \"var_decls\" : {  "
		"        \"out bool test_var\" {  "
		"        }	"
		"        \"in datetime  test_time\" {  "
		"        }	"
		"        \"inout uint32  test_32\" {  "
		"        }	"
		"    }	"
		"}";


/*****************************************************************************
 * 		test_vardecl_1_input
 */
int test_vardecl_1_input() {
	int out;
	int tok_len = 128;
	jsmntok_t tokens[128];
	const char *js;
	int i;
	int ti = 1;

	canopy_context_t ctx;
    canopy_remote_params_t params;
    canopy_remote_t remote;
    canopy_device_t device;

    // Initialize canopy ctx
    canopy_error err = canopy_ctx_init(&ctx, 0);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing ctx: %s\n", canopy_error_string(err));
        exit(-1);
    }

    // Initialize remote
    memset(&params, 0, sizeof(params));
    params.credential_type = CANOPY_DEVICE_CREDENTIALS;
    params.name = TOASTER_UUID;
    params.password = TOASTER_SECRET_KEY;
    params.auth_type = CANOPY_BASIC_AUTH;
    params.remote = REMOTE_ADDR;
    params.use_ws = false;
    params.persistent = false;
    err = canopy_remote_init(&ctx, &params, &remote);
    if (err != CANOPY_SUCCESS) {
        cos_log(LOG_LEVEL_ERROR, "Error initializing remote: %s\n", canopy_error_string(err));
         return -3;
    }

	js = var_decls1;
	memset(&tokens, 0, sizeof(jsmntok_t) * tok_len);
	bool answer;
	int active = 0;
	err = initialize_device(&device, &remote);
	if (err != CANOPY_SUCCESS) {
		printf("initialize_device returned: %d", err);
		return -2;
	}

	out = c_json_parse_string((char*)js, strlen(js), tokens, tok_len, &active);
	printf("parse string returned %d active tokens %d\n", out, active);

	out = c_json_parse_vardcl(&device,
			(char*)js, strlen(js), 			/* the input JSON and total length  */
			tokens, tok_len,	/* token array with length */
			1,						/* token offset for name vardecl */
			false);				/* expect outer-most object */

	printf("get result key returned %d, answer: %d\n", out, answer);
	printf("%s\n\n", var_decls1);

	return 0;
}


/*****************************************************************************
 * 		test_vardecl_2_input
 */
int test_vardecl_2_input() {
	int out;
	int tok_len = 128;
	jsmntok_t tokens[128];
	const char *js;
	int i;
	int ti = 1;

	js = var_decls2;
	memset(&tokens, 0, sizeof(jsmntok_t) * tok_len);
	bool answer;
	int active = 0;

	out = c_json_parse_string((char*)js, strlen(js), tokens, tok_len, &active);
	printf("parse string returned %d active tokens %d\n", out, active);

	out = c_json_get_result_key((char*)js, strlen(js), tokens, tok_len, active, &answer);
	printf("get result key returned %d, answer: %d\n", out, answer);
	printf("%s\n\n", var_decls2);

	return 0;
}


/*****************************************************************************
 * 		test_vardecl_3_input
 */
int test_vardecl_3_input() {
	int out;
	int tok_len = 128;
	jsmntok_t tokens[128];
	const char *js;
	int i;
	int ti = 1;

	js = var_decls3;
	memset(&tokens, 0, sizeof(jsmntok_t) * tok_len);
	bool answer;
	int active = 0;

	out = c_json_parse_string((char*)js, strlen(js), tokens, tok_len, &active);
	printf("parse string returned %d active tokens %d\n", out, active);

	out = c_json_get_result_key((char*)js, strlen(js), tokens, tok_len, active, &answer);
	printf("get result key returned %d, answer: %d\n", out, answer);
	printf("%s\n\n", var_decls3);

	return 0;
}


/*******************************************************************************
 * 	main() start of program.
 */
int main() {
	test(test_result, "general test for the 'result' tag");
	test(test_vardcl_output, "test emit_vardcl");
	test(test_vardecl_1_input, "tests parsing of one var_dcls");
	test(test_vardecl_2_input, "tests parsing of two var_dcls");
	test(test_vardecl_3_input, "tests parsing of three var_dcls");
}



