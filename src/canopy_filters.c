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

#include	<stdint.h>
#include	<stdbool.h>

#include	<canopy_min.h>

canopy_error append_term_filter(canopy_filter_root_t *root, filter_term_t *ft,
		const char *variable_name,
		const char *value, /*  TBD should we use canopy_var_value_t here? */
		canopy_relation_op relation
		) {
	if (root == NULL) {
		return CANOPY_ERROR_FATAL;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_FATAL;
	}

	ft->is_builtin = false;
	ft->variable_name = variable_name;
	ft->value = value;
	ft->relation = relation;

	if (root->head == NULL) {
		root->head = ft;
		root->tail = ft;
	} else {
		root->tail->next = ft;
		ft->next = NULL;
	}
	return CANOPY_SUCCESS;
}

canopy_error append_unary_filter(canopy_filter_root_t *root, unary_filter_t *ut,
		enum unary_type type,
		const char *variable_name	/* only used for HAS */
		) {
	if (root == NULL) {
		return CANOPY_ERROR_FATAL;
	}
	if (ut == NULL) {
		return CANOPY_ERROR_FATAL;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;

}

canopy_error append_boolean_filter(canopy_filter_root_t *root, boolean_filter_t *bt,
		enum boolean_type type) {
	if (root == NULL) {
		return CANOPY_ERROR_FATAL;
	}
	if (bt == NULL) {
		return CANOPY_ERROR_FATAL;
	}

	return CANOPY_ERROR_NOT_IMPLEMENTED;

}

