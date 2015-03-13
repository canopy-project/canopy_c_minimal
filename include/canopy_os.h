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

// canopy_os.h
// OS Abstraction Layer for Canopy

// string.h methods are not provided here because they are present on all
// platforms of interest.  However, you should free memory returned by the
// string.h routines using canopy_os_free.

#ifndef CANOPY_OS_INCLUDED
#define CANOPY_OS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

#ifdef __linux__
#include <assert.h>
#define CANOPY_OS_ASSERT assert
#else
#define CANOPY_OS_ASSERT(cond) canopy_os_assert(cond, #cond, __FILE__, __LINE__)
void canopy_os_assert(
        int cond,
        const char *condString,
        const char *filename,
        int lineno);
#endif

void * canopy_os_alloc(size_t size);
void * canopy_os_calloc(int count, size_t size);
void canopy_os_free(void *ptr);

#define CANOPY_OS_MSG_MAX_LENGTH 127

int canopy_os_vsnprintf(char *buf, size_t len, const char *msg, va_list ap);
void canopy_os_log(const char *msg, ...);

char * canopy_os_strdup(const char *src);

#ifdef __cplusplus
}
#endif

#endif // CANOPY_OS_INCLUDED


