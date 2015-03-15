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

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void * canopy_os_alloc(size_t size) {
    return malloc(size);
}

void * canopy_os_calloc(int count, size_t size) {
    return calloc(count, size);
}

void canopy_os_free(void *ptr) {
    free(ptr);
}

int canopy_os_vsnprintf(char *buf, size_t len, const char *msg, va_list ap) {
    return vsnprintf(buf, len, msg, ap);
}

void canopy_os_log(int level, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

char * canopy_os_strdup(const char *src) {
    return strdup(src);
}

