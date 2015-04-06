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

/*****************************************************************************/

static void append(canopy_filter_root_t *root, canopy_filter_t *ft);


/*****************************************************************************/

/**************************************
 * append_term_filter()
 *
 * 	Appends this term filter to the end of the chain in the root.
 */
canopy_error append_term_filter(canopy_filter_root_t *root,
		canopy_filter_t *ft,
		const char *variable_name,
		const char *value, /*  TBD should we use canopy_var_value_t here? */
		canopy_relation_op relation
		) {
	if (root == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	filter_term_t *term = &ft->onion.term;
	ft->type = TERM;
	term->variable_name = variable_name;
	term->value = value;
	term->relation = relation;

	append(root, ft);
	return CANOPY_SUCCESS;
}

canopy_error append_unary_filter(canopy_filter_root_t *root,
		canopy_filter_t *ft,
		enum unary_type type,
		const char *variable_name	/* only used for HAS */
		) {
	if (root == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	unary_filter_t *unary = &ft->onion.unary;
	unary->type = type;
	unary->variable_name = variable_name;
	ft->type = UNARY;

	append(root, ft);
	return CANOPY_SUCCESS;

}

canopy_error append_boolean_filter(canopy_filter_root_t *root,
		canopy_filter_t *ft,
		enum boolean_type type) {
	if (root == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	boolean_filter_t *boolean = &ft->onion.boolean;
	boolean->type = type;
	ft->type = BOOLEAN;

	append(root, ft);
	return CANOPY_SUCCESS;
}

canopy_error append_open_paren_filter(canopy_filter_root_t *root, canopy_filter_t *ft) {
	if (root == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	paren_filter_t *paren = &ft->onion.paren;
	paren->open = true;
	ft->type = PAREN;

	append(root, ft);
	return CANOPY_SUCCESS;

}

canopy_error append_close_paren_filter(canopy_filter_root_t *root, canopy_filter_t *ft) {
	if (root == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}
	if (ft == NULL) {
		return CANOPY_ERROR_BAD_PARAM;
	}

	paren_filter_t *paren = &ft->onion.paren;
	paren->open = false;
	ft->type = PAREN;

	append(root, ft);
	return CANOPY_SUCCESS;
}


/****************************************************
 * append()
 */
static void append(canopy_filter_root_t *root, canopy_filter_t *ft) {
	ft->next = NULL;
	if (root->head == NULL) {

		/*
		 * The root head is NULL so this is the first filter in the list.  It's
		 * also the last one in the list so tail points to it as well.
		 */
		root->head = ft;
		root->tail = ft;
	} else {

		/*
		 * fixup the tail pointer.  tail points to the last filter in the list
		 * That pointer should have NULL in ->next.  Set ->next to this filter
		 * so that the link is setup.  Then set the tail to this filter.
		 */
		root->tail->next = ft;
		root->tail = ft;
		ft->next = NULL;

		/*
		 * This is here for me to remember how lists work.
		 *
		 * root->ft1
		 * tail->ft1
		 * 		(tail->ft1->next is null.)
		 *
		 * tail->ft1->next = ft2;
		 * tail->ft1 = ft2;
		 * ft2->next = NULL;
		 *
		 * tail->ft2->next = ft3;
		 * tail->ft3;
		 * ft3->next = NULL;
		 *
		 * Following the list
		 *
		 * fts = root;
		 * while (fts != NULL) {
		 * 		fts->filter....
		 * 		fts = fts->next;
		 * }
		 * 	First loop:
		 * 		fts points to ft1
		 * 	next loop
		 * 		fts points to ft2
		 * 	next loop
		 * 		fts points to ft3
		 * 	next loop
		 * 		exits because ft3->next is null.
		 *
		 *
		 */
	}

}


