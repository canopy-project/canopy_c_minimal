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

#include	<canopy_communication.h>
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

/*
 * Internal test of the canopy_http_peform routine with GET
 */
#define USE_HTTP false
#define REMOTE_NAME "dev02.canopy.link"
#define USERNAME "e43eb410-48da-421a-b07d-1cd751412fd5"
#define PASSWORD "wYI0G+HQN9fj76fpyxwiHKJDQags0dpM"
static int test_raw_api_info() {
    canopy_error err;
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    int end;
    err = canopy_http_perform(
            CANOPY_HTTP_GET,
            USE_HTTP,
            true,
            USERNAME,
            PASSWORD,
            buf,
            sizeof(buf),
            &end,
            NULL,
            REMOTE_NAME,
            "/api/info",
            NULL,
            NULL);

    if (err != CANOPY_SUCCESS) {
        printf("Error: %s\n", canopy_error_string(err));
        return err;
    }

    printf("Payload recieved: %s\n", buf);
    return 0;
}

static int test_post_api_device_self() {
    canopy_error err;
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    int end;
    int status_code;
    char *payload = "{\"location_note\" : \"mars\"}";
    err = canopy_http_perform(
            CANOPY_HTTP_POST,
            USE_HTTP,
            true,
            USERNAME,
            PASSWORD,
            buf,
            sizeof(buf),
            &end,
            &status_code,
            REMOTE_NAME,
            "/api/device/self",
            payload,
            NULL);

    if (err != CANOPY_SUCCESS) {
        printf("Error: %s\n", canopy_error_string(err));
        return err;
    }

    printf("Payload recieved: %d %s\n", status_code, buf);
    return 0;
}

/*******************************************************************************
 * 	main() start of program.
 */
int main() {
	test(test_raw_api_info, "test basic HTTP GET communication with remote");
	test(test_post_api_device_self, "test basic HTTP POST communication with remote");
}

