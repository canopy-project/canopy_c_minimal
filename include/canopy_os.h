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
#define COS_ASSERT assert
#else
#define COS_ASSERT(cond) cos_assert(cond, #cond, __FILE__, __LINE__)
void cos_assert(
        int cond,
        const char *condString,
        const char *filename,
        int lineno);
#endif


/*
 * Memory allocation is different between Linux and some RTOs. We do all the
 * allocation and freeing by using these calls.  Don't us malloc(), except in
 * a program in Linux.
 */
void * cos_alloc(size_t size);
void * cos_calloc(int count, size_t size);
void cos_free(void *ptr);

/*
 * Logging is an area where there are differences between embedded systems and
 * Linux.  These #define define the various levels of logging.  Some
 * Implementations will put output into 'stderr' while others might output
 * on a serial port.
 */
#define LOG_LEVEL_FATAL  0x0001
#define LOG_LEVEL_ERROR  0x0002
#define LOG_LEVEL_WARN   0x0004
#define LOG_LEVEL_INFO   0x0008
#define LOG_LEVEL_DEBUG  0x0010
#define LOG_LEVEL_ERROR_OR_HIGHER  (LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)
#define LOG_LEVEL_WARN_OR_HIGHER  (LOG_LEVEL_WARN | LOG_LEVEL_ERROR_OR_HIGHER)
#define LOG_LEVEL_INFO_OR_HIGHER  (LOG_LEVEL_INFO | LOG_LEVEL_WARN_OR_HIGHER)
#define LOG_LEVEL_DEBUG_OR_HIGHER  (LOG_LEVEL_DEBUG | LOG_LEVEL_INFO_OR_HIGHER)
void cos_log(int level, const char *msg, ...);

/*
 * Because memory allocation is different between Linux and embedded systems,
 * we provide our own strdup()
 */
#define COS_MSG_MAX_LENGTH 127
char * cos_strdup(const char *src);

/*
 * Get the canopy time.  cos_time_t is a count of milli-seconds since
 * some point in time.  There's no intention that this represents the wall
 * clock time.  (While this starts outs as seconds since epoch, we turn
 * seconds into milli-seconts by multiplying it with 1000.)
 */
typedef unsigned long cos_time_t;
int cos_get_time(cos_time_t *time);


#ifdef __cplusplus
}
#endif

#endif // CANOPY_OS_INCLUDED


