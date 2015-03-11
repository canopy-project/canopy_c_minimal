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

#ifndef CANOPY_MIN_INCLUDED
#define CANOPY_MIN_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// This is used to verify that programs compiled using this version of the
// header also link with the same version number of the library.
#define CANOPY_MIN_HEADER_VERSION "15.02.030"

typedef enum {
    // Command succeeded.
    CANOPY_SUCCESS = 0,
    
    // An unknown error occured.
    CANOPY_ERROR_UNKNOWN,

	/* something internal is bad */
	CANOPY_ERROR_FATAL,

    // You are using a version of the library binary and header file that are
    // incompatible.
    CANOPY_ERROR_INCOMPATIBLE_LIBRARY_VERSION,

    // The requested operation has not been implemented on the current
    // platform.
    CANOPY_ERROR_NOT_IMPLEMENTED,

    // A bad parameter value was provided.
    CANOPY_ERROR_BAD_PARAM,

    // An operation was requested on the wrong data type.
    CANOPY_ERROR_WRONG_TYPE,

    // The requested data is not available yet.  Try again.
    CANOPY_ERROR_AGAIN,

    // The requested operation was cancelled.
    CANOPY_ERROR_CANCELLED,
} canopy_error;

typedef enum {
    CANOPY_INVALID_CREDENTIAL_TYPE = 0,
    CANOPY_DEVICE_CREDENTIALS,
    CANOPY_USER_CREDENTIALS
} canopy_credential_type;

typedef enum {
    CANOPY_INVALID_AUTH_TYPE = 0,
    CANOPY_BASIC_AUTH,
} canopy_auth_type;

typedef struct canopy_remote_params {
    canopy_credential_type 	credential_type;
    char 					*name; 			// user's username or device id
    char 					*password; 		// user's password or device secret
    uint16_t 				http_port;
    uint16_t 				https_port;
    canopy_auth_type		auth_type;
    char 					*remote; 		// hostname or IP address of remote server
    bool 					use_ws;     	// hint: use websockets if available
    bool 					persistent; 	// hint: keep communication channel open
    bool 					use_http;		// not sure what this does yet....
} canopy_remote_params_t;

typedef struct canopy_context {
	int						update_period;	// used with the status reporting stuff
    // Linked-list of connections
    struct canopy_remote 	*remotes;		// List of remotes known to the library.  This may not be needed.
} canopy_context_t;

/*
 * canopy_remote:
 * 		used to hold information about the remote we're using.  The details of how to connect
 * 		to the remote is passed in as part of the params.
 */
typedef struct canopy_remote {
    struct canopy_remote 		*next;		/* pointer to new remote known to the library */
    struct canopy_context 		*ctx;		/* back pointer to the context */
    struct canopy_remote_params *params;	/* params for this remote */

    bool 						ws_connected;	/* we currently have a webscocket connection */
} canopy_remote_t;

typedef struct canopy_user {
	struct canopy_user *next;
    bool is_activated;
    char *name;
} canopy_user_t;

typedef struct canopy_user_query {
    // TBD
} canopy_user_query_t;

typedef struct canopy_permissions {
    // TBD
} canopy_permissions_t;

/*
 * canopy_device:
 * 		represents a device known locally or to a remote
 *
 */
typedef struct canopy_device {
	struct canopy_device 		*next;		/* hung off of User or remote */
    char 						*uuid;		/* the uuid of the device */
    bool 						ws_connected;
    canopy_remote_t 			*remote;
    struct canopy_var 			*vars;		/* list of vars on this device */
} canopy_device_t;


/*****************************************************************************/

// canopy_active_status enum identifies whether or not a device has
// communicated with the server recently.  By "recently" we mean in the last 60
// seconds or so.
typedef enum {
    // Invalid value, or "we don't care" value when filtering.
    CANOPY_ACTIVE_STATUS_INVALID = 0,
    CANOPY_ACTIVE_STATUS_DONT_CARE = CANOPY_ACTIVE_STATUS_INVALID,

    // Device has never communicated with the server.
    CANOPY_NEWLY_CREATED,

    // Device has communicated with the server in past, but not recently.
    CANOPY_INACTIVE,

    // Device communicated with the server recently.
    CANOPY_ACTIVE,
} canopy_active_status;


/*
 * This stuff provides the text string used for the canopy_active_status as used in
 * the filters.  The idea is that you would use this lookup table when referring to
 * canopy_active_status.
 */
#define  CANOPY_ACTIVE_STATUS "active_status"
struct activity_status {
	canopy_active_status status;
	const char *str;
};
struct activity_status activity_status_table[] = {
		{CANOPY_ACTIVE_STATUS_INVALID, "status_invalid"},
		{CANOPY_ACTIVE_STATUS_DONT_CARE, "status_dont_care"},
		{CANOPY_NEWLY_CREATED, "status_newly_created"},
		{CANOPY_INACTIVE, "status_inactive"},
		{CANOPY_ACTIVE, "status_active"},
};

/*****************************************************************************/

// canopy_ws_connection_status enum identifies whether or not a device is
// currently connected to the server over websockets.
typedef enum {
    // Invalid value, or "we don't care" value when filtering.
    CANOPY_WS_CONNECTION_STATUS_INVALID = 0,
    CANOPY_WS_CONNECTION_STATUS_DONT_CARE = CANOPY_WS_CONNECTION_STATUS_INVALID,

    // Device has never communicated with the server.
    CANOPY_WS_NEVER_CONNECTED,

    // Device has communicated with the server in past, but not recently.
    CANOPY_WS_DISCONNECTED,

    // Device communicated with the server recently.
    CANOPY_WS_CONNECTED,
} canopy_ws_connection_status;

/*
 * This stuff provides the text string used for the canopy_ws_connection_status as used in
 * the filters.  The idea is that you would use this lookup table when referring to
 * canopy_ws_connection_status.
 */
#define  CANOPY_ACTIVE_STATUS "active_status"
struct ws_connection_status {
	canopy_ws_connection_status status;
	const char *str;
};
struct ws_connection_status ws_connection_status_table[] = {
		{CANOPY_WS_CONNECTION_STATUS_INVALID, "ws_connection_status_invalid"},
		{CANOPY_WS_CONNECTION_STATUS_DONT_CARE, "ws_connection_status_dont_care"},
		{CANOPY_WS_NEVER_CONNECTED, "ws_connection_status_never_connected"},
		{CANOPY_WS_DISCONNECTED, "ws_connection_status_disconnected"},
		{CANOPY_WS_CONNECTED, "ws_connection_status_connected"},
};


/*****************************************************************************/

/*
 * Query specification stuff.
 *
 * A query consists of a list of filters, a sort order, and a limit specifying how many devices or users
 * to return.
 *
 * Filters consist of a list of filter_terms and term relations.  The first filter in the chain on the
 * query is the first entry on the stack, followed by the second....  They are processed like an HP calculator, reverse
 * polish notation.
 *
 * Filter terms
 * 		variable	relation	value
 * 		"TERM temperature < 50"
 *
 * Unary boolean operators
 * 		'UNARY variable HAS'		-- Evaluates TRUE if the variable exists on this device
 * 		'UNARY term NOT'			-- The term is not true
 *
 * Boolean operators
 * 		term term 'BOOL AND'		-- Evaluates TRUE if both terms are true
 * 		term term 'BOOL OR'		-- Evaluates TRUE if one of them terms is true
 *
 * Example:
 * 	The device has a variable called temperature, and it's < 50 or > 80
 * 		'"UNARY temperature HAS" "TERM temperature < 50" "TERM temperature > 80" "BOOL OR" "BOOL AND"'
 *
 *
 */

/******************************************************
 *  TERM FILTERS
 */
/*
 * These define the relation portion of the filter_term_t.
 */
typedef enum {
    CANOPY_RELATION_OP_INVALID=0,
    CANOPY_EQ,
    CANOPY_NEQ,
    CANOPY_GT,
    CANOPY_GTE,
    CANOPY_LT,
    CANOPY_LTE,
} canopy_relation_op;

/*
 * Specifies the variable and value to filter by.
 */
typedef struct filter_term {
    bool is_builtin;
    const char *variable_name;
    const char *value; // TBD should we use canopy_var_value_t here?
    canopy_relation_op relation;
} filter_term_t;


/******************************************************
 *  UNARY FILTERS
 */
/*
 * These define the type of unary operator used in the unary_filter_t
 */
enum unary_type {
	HAS,
	NOT,
};
typedef struct unary_filter {
	enum unary_type type;
	const char *variable_name;
} unary_filter_t;


/******************************************************
 *  BOOLEAN FILTERS
 */
/*
 * These define the type of unary operator used in the unary_filter_t
 */
enum boolean_type {
	AND,
	OR,
};
typedef struct boolean_filter {
	enum boolean_type type;
} boolean_filter_t;


/*******************************************************
 * FILTER CONSTRUCTION
 */
/*
 * TAG to define which type of filter element this canopy_filter_t
 */
enum ftype {
	TERM,
	UNARY,
	BOOLEAN,
};

/*
 * A canopy_filter is a linked list of items that contribute to the overall
 * filter operation.
 */
typedef struct canopy_filter {
	struct canopy_filter	*next;
	enum ftype type;
	union {
		filter_term_t		term;
		unary_filter_t		unary;
		boolean_filter_t	boolean;
	} onion;
} canopy_filter_t;

/*
 * This is used during the construction of a filter list.
 */
typedef struct canopy_filter_root {
	canopy_filter_t *head;		/* First one on the list.  This points to the start of the filter stack */
	canopy_filter_t *tail;		/* tracks the tail of the list for adding new filters */
} canopy_filter_root_t;

extern canopy_error append_term_filter(canopy_filter_root_t *root, filter_term_t *ft,
		const char *variable_name,
		const char *value, /*  TBD should we use canopy_var_value_t here? */
		canopy_relation_op relation
		);

extern canopy_error append_unary_filter(canopy_filter_root_t *root, unary_filter_t *ut,
		enum unary_type type,
		const char *variable_name	/* only used for HAS */
		);

extern canopy_error append_boolean_filter(canopy_filter_root_t *root, boolean_filter_t *bt,
		enum boolean_type type);



/************************************************************
 * Canopy sort definitions
 */
typedef struct canopy_sort {

} canopy_sort_t;

typedef struct canopy_limits {
	uint32_t 			start;	/* which entry to start with, 0 starts at the head of the list */
	uint32_t			count;	/* how many to return */
} canopy_limits_t;


typedef struct canopy_device_query {
	canopy_filter_root_t 	*filter_root;  	/* the list used for filtering which devices to report. */
	canopy_sort_t 		*sort;		/* defines the sort order.  If null the result list is unordered. */
	canopy_limits_t		*limits;	/* How many to return */
} canopy_device_query_t;


/*****************************************************************************/
// BARRIERS
//
typedef struct canopy_barrier {
    canopy_remote_t *remote;
    //canopy_barrier_cb cb;
    canopy_error (*canopy_barrier_cb)(struct canopy_barrier *barrier, void *userdata);
    void *userdata;
    canopy_credential_type type;
    union {
        canopy_device_t *device;
        canopy_user_t *user;
    } result;
} canopy_barrier_t;

typedef canopy_error (*canopy_barrier_cb)(struct canopy_barrier *barrier,
        void *userdata);

// WARNING! WARNING!
// Stack-scoped barriers can be used, but you must call either
// canopy_barrier_wait_for_complete() or canopy_barrier_cancel() before
// returning from the scope with the barrier.

// Block the current thread until the operation has completed, or timeout
// occurs.  Returns immediately if the requested operation has already
// finished.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <timeout_ms> is number of milliseconds to wait before CANOPY_ERROR_TIMEOUT
// is returned.
//
canopy_error canopy_barrier_wait_for_complete(canopy_barrier_t *barrier, int timeout_ms);

// Cancels the barrier.
// Any threads blocked in canopy_barrier_wait_for_complete will return with
// CANOPY_ERROR_CANCELLED.
// No further callbacks will be triggered for this barrier.
// After calling this it is safe to deallocate the barrier.
canopy_error canopy_barrier_cancel(canopy_barrier_t *barrier);

// Establish a callback that will be triggered when the operation has
// completed.  Triggers the callback immediately if the requested operation has
// already finished.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <cb> is the callback to trigger.
//
// <userdata> is user data that gets passed along to the callback.
canopy_error canopy_barrier_setup_callback(canopy_barrier_t *barrier, canopy_barrier_cb cb, void *userdata);

// Check if an asyncrhonous operation has completed.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// Returns CANOPY_SUCCESS if the operation is complete.
// Returns CANOPY_ERROR_AGAIN if the operation has not yet finished.
canopy_error canopy_barrier_is_complete(canopy_barrier_t *barrier);

// Get the result of an asynchronous request for a device object.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <device> will store the obtained device object, on success.
//
// If the request is not yet complete, returns CANOPY_ERROR_AGAIN.
// If the request was not for a device object, returns CANOPY_ERROR_WRONG_TYPE.
canopy_error canopy_barrier_get_device(canopy_barrier_t *barrier, struct canopy_device *device);

// Get the result of an asynchronous request for a user object.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <user> will store the obtained user object, on success.
//
// If the request is not yet complete, returns CANOPY_ERROR_AGAIN.
// If the request was not for a device object, returns CANOPY_ERROR_WRONG_TYPE.
canopy_error canopy_barrier_get_user(canopy_barrier_t *barrier, struct canopy_user *user);




/*****************************************************************************/
// CONTEXT

// Initialize a new context.
// 
// Returns CANOPY_ERROR_INCOMPATIBLE_LIBRARY_VERSION if the version of the
// library you have linked with is incompatible with the header file you are
// using.
canopy_error canopy_ctx_init(canopy_context_t *ctx);

// Shutdown context
// Closes any connections that might still be open.
// Frees any allocated memory
canopy_error canopy_ctx_shutdown(canopy_context_t *ctx);

// Set logging options for a context.
//
// <enabled> enables/disables logging.  Defaults to false.
//
// <logfile> specifies the name of the file to log to.  Pass in NULL to
// use stderr or the system's default logging destination.  Defaults to
// NULL.
//
// <level> defines the logging level
#define LOG_LEVEL_FATAL  0x0001
#define LOG_LEVEL_ERROR  0x0002
#define LOG_LEVEL_WARN   0x0004
#define LOG_LEVEL_INFO   0x0008
#define LOG_LEVEL_DEBUG  0x0010
#define LOG_LEVEL_ERROR_OR_HIGHER  (LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)
#define LOG_LEVEL_WARN_OR_HIGHER  (LOG_LEVEL_WARN | LOG_LEVEL_ERROR_OR_HIGHER)
#define LOG_LEVEL_INFO_OR_HIGHER  (LOG_LEVEL_INFO | LOG_LEVEL_WARN_OR_HIGHER)
#define LOG_LEVEL_DEBUG_OR_HIGHER  (LOG_LEVEL_DEBUG | LOG_LEVEL_INFO_OR_HIGHER)
canopy_error canopy_ctx_set_logging(canopy_context_t *ctx,
        bool enabled,
        const char *logfile,
        int level);

// Get the logging options for a context.
//
// <enabled> is a pointer to a bool, or NULL to not read this option.
//
// <logfile> is a pointer to a buffer at least <logfile_len> bytes long, or
// NULL to not read this option.  This call will set the string to an empty
// string if the default log destination is in use.
//
// <level> is a pointer to an int, or NULL to not read this option.
canopy_error canopy_ctx_get_logging(canopy_context_t *ctx,
        bool *enabled,
        char **logfile,
        size_t logfile_len,
        int *level);


/*****************************************************************************/
// REMOTE

// Initializes a new remote object.  Does not necessarily open a TCP or
// websocket connection to the server, but initializes an object with the
// parameters to do so when needed.
//
// <ctx> is the context.
//
// <params> must be populated by the caller.  Sets all of the options for
// subsequent connections to the server.
//
// <remote> is a remote object that is initialized by this call.
canopy_error canopy_remote_init(canopy_context_t *ctx, 
        canopy_remote_params_t *params,
        canopy_remote_t *remote);

// Shutdown a remote object.
// Closes persistent connection to server, if any.
// Frees any allocated memory.
canopy_error canopy_remote_shutdown(canopy_remote_t *remote);

// Get the remote's clock in milliseconds.  The returned value has no relation
// to wall clock time, but is monotonically increasing and is reported
// consistently by the remote to anyone who asks.
canopy_error canopy_remote_get_clock_ms(canopy_remote_t *remote, 
        unsigned long *out_timestamp,
        canopy_barrier_t *barrier);

// Get our version of the remote's clock in milliseconds.  This is based on the
// time obtained the last time canopy_remote_get_clock_ms was called, plus
// however much time has elapsed since then.
//
// Returns CANOPY_ERROR_AGAIN if canopy_remote_get_clock_ms() has never been
// called for <remote>.
canopy_error canopy_get_local_ms(canopy_remote_t *remote, 
        unsigned long *out_timestamp);

// Get a list of devices from the server based on the filters in a device query
// object.
//
// <remote> is the remote object used for connecting to the server.
// 
// <query> contains the constraints that devices must satisfy to be included in
// the resulting list.
//
// <max_count> is the maximum number of devices to fetch.
//
// <devices> is an array of at least <max_count> device objects that will store
// the results of this operation.
//
// <barrier> will store a new barrier object that can be used to obtain the
// result when it is ready.  If NULL, this operation will block the current
// thread.
canopy_error canopy_remote_get_devices(canopy_remote_t *remote, 
        canopy_device_query_t *query, 
        size_t max_count, 
        canopy_device_t **devices, 
        canopy_barrier_t *barrier);

// Get the device based on the authentication information provided to
// canopy_remote_init.
//
// <remote> is the remote object used for connecting to the server.
//
// <device> will store the device object returned from the server.  If
// <barrier> is NULL, this will contain the result upon a successful return.
// If <barrier> is provided, this will be updated when the result is ready, and
// will also be passed along to the barrier object.
//
// <barrier> will store a new barrier object that can be used to obtain the
// result when it is ready.  If NULL, this operation will block the current
// thread.
canopy_error canopy_get_my_device(canopy_remote_t *remote, 
        canopy_device_t *device, 
        canopy_barrier_t *barrier);

// Get the device based on the authentication information provided to
// canopy_remote_init.
//
// <remote> is the remote object used for connecting to the server.
//
// <user> will store the user object returned from the server.  If
// <barrier> is NULL, this will contain the result upon a successful return.
// If <barrier> is provided, this will be updated when the result is ready, and
// will also be passed along to the barrier object.
//
// <barrier> will store a new barrier object that can be used to obtain the
// result when it is ready.
canopy_error canopy_get_my_user(canopy_remote_t *remote, 
        canopy_user_t *user, 
        canopy_barrier_t *barrier);

/*****************************************************************************/
// DEVICE

// Updates a device object's status and properties from the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// locally.
canopy_error canopy_device_update_from_remote(
        canopy_device_t *device, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

// Updates a device object's status and properties to the remote server.  Any
// status or properties with a more recent clock ms value will be updated
// remotely.
canopy_error canopy_device_update_to_remote(
        canopy_device_t *device, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

// Synchronizes a device object with the remote server.
// (Potential revisit).  Server should do merge.  Does it need to be atomic?
// 
// Roughly equivalent to:
//  canopy_device_update_from_remote(device, remote, NULL);
//  canopy_device_update_to_remote(device, remote, barrier);
//
canopy_error canopy_device_sync_to_remote(
        canopy_device_t *device, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

// Get the active status for a device.
canopy_error canopy_device_get_active_status(
        canopy_device_t *device, 
        canopy_active_status *out_status);



/*****************************************************************************/
// USERS

// Goes to remote
canopy_error canopy_create_user(canopy_remote_t *remote,
        const char *username,
        const char *password,
        const char *email,
        canopy_user_t *out_user,
        canopy_barrier_t *barrier);

// Goes to remote
canopy_error canopy_user_create_devices(canopy_user_t *user,
        uint32_t quantity,
        char **names,
        canopy_device_t **out_devices,
        canopy_barrier_t *barrier);

canopy_error canopy_user_get_email(canopy_user_t *user, char **email);
canopy_error canopy_user_get_username(canopy_user_t *user, char **username);
canopy_error canopy_user_is_validated(canopy_user_t *user, bool *validated);
canopy_error canopy_user_devices(canopy_user_t *user, canopy_device_query_t *query);

canopy_error canopy_user_set_email(canopy_user_t *user, const char *email);
canopy_error canopy_user_set_password(canopy_user_t *user, 
        const char *old_password,
        const char *new_password);

// Updates a user object's properties (excluding devices) from the remote
// server.  Any properties with a more recent clock ms value will be updated
// locally.
canopy_error canopy_user_update_from_remote(
        canopy_user_t *user, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

// Updates a user object's properties (excluding devices) to the remote
// server.  Any properties with a more recent clock ms value will be updated
// remotely.
canopy_error canopy_user_update_to_remote(
        canopy_user_t *user, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

// Synchronizes a user object with the remote server.
// (Potential revisit).  Server should do merge.  Does it need to be atomic?
// 
// Roughly equivalent to:
//  canopy_user_update_from_remote(user, remote, NULL);
//  canopy_user_update_to_remote(user, remote, barrier);
canopy_error canopy_user_sync_to_remote(
        canopy_user_t *user, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);



/*****************************************************************************/

// CLOUD VARIABLES
typedef enum {
    CANOPY_VAR_DIRECTION_INVALID=0,
    CANOPY_VAR_OUT,
    CANOPY_VAR_IN,
    CANOPY_VAR_INOUT,
} canopy_var_direction;

typedef enum {
    CANOPY_VAR_DATATYPE_INVALID=0,
    CANOPY_VAR_DATATYPE_VOID,
    CANOPY_VAR_DATATYPE_STRING,
    CANOPY_VAR_DATATYPE_BOOL,
    CANOPY_VAR_DATATYPE_INT8,
    CANOPY_VAR_DATATYPE_UINT8,
    CANOPY_VAR_DATATYPE_INT16,
    CANOPY_VAR_DATATYPE_UINT16,
    CANOPY_VAR_DATATYPE_INT32,
    CANOPY_VAR_DATATYPE_UINT32,
    CANOPY_VAR_DATATYPE_FLOAT32,
    CANOPY_VAR_DATATYPE_FLOAT64,
    CANOPY_VAR_DATATYPE_DATETIME,
    CANOPY_VAR_DATATYPE_STRUCT,
    CANOPY_VAR_DATATYPE_ARRAY,
} canopy_var_datatype;

typedef unsigned long canopy_time_t;

#define CANOPY_VAR_VALUE_MAX_LENGTH 128
typedef struct canopy_var_value {
    canopy_var_datatype type;
    union {
        struct {
            char *buf;
            size_t len;
        } val_string;
        bool val_bool;
        int8_t val_int8;
        int16_t val_int16;
        int32_t val_int32;
        uint8_t val_uint8;
        uint16_t val_uint16;
        uint32_t val_uint32;
        float val_float;
        double val_double;
        canopy_time_t val_time;
    } value;
} canopy_var_value_t;

#define CANOPY_VAR_NAME_MAX_LENGTH 128
typedef struct canopy_var {
	struct canpopy_var *next;	/* linked list of variables, hung off device */
    canopy_device_t *device;
    canopy_var_direction direction;
    canopy_var_datatype type;
    char name[CANOPY_VAR_NAME_MAX_LENGTH];
    struct canopy_var_value val;  /* yes, not a pointer, real storage */
} canopy_var_t;

canopy_error canopy_device_var_init(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name,
        canopy_var_t *out_var);

canopy_error canopy_var_set_bool(canopy_var_t *var, bool value);
canopy_error canopy_var_set_int8(canopy_var_t *var, int8_t value);
canopy_error canopy_var_set_int16(canopy_var_t *var, int16_t value);
canopy_error canopy_var_set_int32(canopy_var_t *var, int32_t value);
canopy_error canopy_var_set_uint8(canopy_var_t *var, uint8_t value);
canopy_error canopy_var_set_uint16(canopy_var_t *var, uint16_t value);
canopy_error canopy_var_set_uint32(canopy_var_t *var, uint32_t value);
canopy_error canopy_var_set_datetime(canopy_var_t *var, canopy_time_t value);
canopy_error canopy_var_set_float32(canopy_var_t *var, float value);
canopy_error canopy_var_set_float64(canopy_var_t *var, double value);
canopy_error canopy_var_set_string(canopy_var_t *var, const char *value, size_t len);

canopy_error canopy_var_get_bool(canopy_var_t *var, 
        bool *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_int8(canopy_var_t *var, 
        int8_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_int16(canopy_var_t *var, 
        int16_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_int32(canopy_var_t *var, 
        uint32_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_uint8(canopy_var_t *var, 
        uint8_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_uint16(canopy_var_t *var, 
        uint16_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_uint32(canopy_var_t *var, 
        uint32_t *out_value, 
        unsigned long *out_ms);
canopy_error canopy_var_get_datetime(canopy_var_t *var, 
        canopy_time_t *out_value,
        unsigned long *out_ms);
canopy_error canopy_var_get_float32(canopy_var_t *var, 
        float out_value,
        unsigned long *out_ms);
canopy_error canopy_var_get_float64(canopy_var_t *var, 
        double out_value,
        unsigned long *out_ms);
canopy_error canopy_var_get_string(canopy_var_t *var, 
        char *dest, 
        size_t len,
        size_t *out_len,
        unsigned long *out_ms);

/*
 *  myVar = device.varInit("out", "float32", "temperature");
 *
 *  device.updateToServer().onDone(
 *      function(error, device) {alert("all done");}
 *  );
 *
 *  
 */

/*
 *  Variable initialization will be ignored if the variable already exists on
 *  the server. 
 *
 *  You can't change the datatype after initialization.
 *
 *  POST api/devices/UUID
 *  {
 *      "var_init" : {
 *          "out int8 coffee_mode" : {},
 *          "out float32 temperature" : {},
 *      }
 *  }
 *
 *  POST api/devices/UUID
 *  {
 *      "var_init" : {
 *          "out int8 coffee_mode" : {},
 *          "out float32 temperature" : {},
 *      }
 *      "vars" : {
 *          "coffee_mode" : 4,
 *          "temperature" : 100.34,
 *          "statusmsg" : "hello",
 *      }
 *  }
 *
 *  GET api/devices/UUID
 *  {
 *      "status" : {
 *          "last_seen" : "2015-03-09 10:44:32.245 UTC",
 *          "ws_connected" : true,
 *      }
 *      "sddl" : {
 *          "out int8 coffee_mode" : {}
 *      }
 *      "vars" : {
 *          "coffee_mode" : {
 *              "t" : "2015-03-09 10:44:32.245 UTC",
 *              "v" : 4
 *          },
 *          "statusmsg" : {
 *              "t" : "2015-03-09 10:44:32.245 UTC",
 *              "v" : "hello"
 *          }
 *      }
 *  }
 */

/*
 * canopy_var_t gpsvar;
 * canopy_device_var_init(dev, OUT, CANOPY_VAR_DATATYPE_STRUCT, "gps", &gpsvar)
 *
 * canopy_var_struct_init_member(
 *      &gpsvar, 
 *      CANOPY_VAR_DATATYPE_FLOAT32, 
 *      "latitude");
 *
 * canopy_var_struct_init_member(
 *      &gpsvar, 
 *      CANOPY_VAR_DATATYPE_FLOAT32, 
 *      "longitude");
 *
 *
 * canopy_var_set_int8(var
 * 
 */

#if 0
CanopyResultEnum canopy_init_client(CanopyClient_t *client);
CanopyResultEnum canopy_shutdown_client(CanopyClient_t *client);

CanopyResultEnum canopy_client_set_use_https(CanopyClient_t *client, bool use_https);

{
    CanopyClient_t client;

    canopy_init_client(&client, &opts);
}
#endif

/*
 *  PERMISSIONS
 *      devices_i_can_access()
 *      users_i_can_access()
 *      admin_features_i_can_access()
 *
 *  ACCOUNT (User Account)
 *      devices_i_own()
 *      permissions()
 *  
 *  DEVICE
 *      permissions()
 *      owner()
 *      set_var()
 *      get_var()
 */

// Every physical device (such as a Toaster) is represented on the server as
// both a Device resource and an Account resource.
//
// HUMAN (indepedant being)
//      - Account
//
// TOASTER (physical thing)
//      - Account
//      - Device
//
// The Account resource is used for authentication and authorization.
//
//| devices(filters)                           | -> DeviceQuery
//| emailAddress()                             | -> string
//| isActivated()                              | -> bool
//| quotas()                                   | -> object
//| username()                                 | -> string
//| update(params, function(Error)



//
//  canopy_device_update()
//      // among other things, triggers callbacks for changed values


#endif
