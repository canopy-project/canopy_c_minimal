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


#include	<stdint.h>
#include	<stdbool.h>
#include	<string.h>

#include	<canopy_min.h>
#include	<canopy_min_internal.h>


/*****************************************************************************/

	/* statics go here */

/*****************************************************************************/

canopy_error canopy_cleanup_remote(canopy_remote_t *remote) {
	if (remote == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;
}

