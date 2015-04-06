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

// Linux implementation of OS Abstraction Layer for Canopy

#include <canopy_os.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void * cos_alloc(size_t size) {
    return malloc(size);
}

void * cos_calloc(int count, size_t size) {
    return calloc(count, size);
}

void cos_free(void *ptr) {
    free(ptr);
}

int cos_vsnprintf(char *buf, size_t len, const char *msg, va_list ap) {
    return vsnprintf(buf, len, msg, ap);
}

void cos_log(int level, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

char * cos_strdup(const char *src) {
    return strdup(src);
}


int cos_get_time(cos_time_t *time) {
	int err = 0;
	/*
	 *  struct timeval {
     *          time_t      tv_sec;     /* seconds
     *          suseconds_t tv_usec;    /* microseconds
     *      };
	 *
	 */
	struct timeval tv;
    err = gettimeofday(&tv, NULL);
    if (err != 0) {
    	return -1;
    }
    /*
     * chop off the high order bits of the seconds, so we can add the minutes
     * and msec.  minutes will probably overflow, but it's not an error
     * the way we use cos_time_t.
     */
    unsigned long minutes = tv.tv_sec * 1000;
    unsigned long msec = tv.tv_usec / 1000;
    unsigned long ret = minutes + msec;
    *time = ret;
	return 0;
}
