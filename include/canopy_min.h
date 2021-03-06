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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <canopy_os.h>

// This is used to verify that programs compiled using this version of the
// header also link with a compatible version of the library.
#define CANOPY_MIN_HEADER_VERSION "15.02.030"

#define LOCAL_MIN(x,y) ((x) < (y) ? (x) : (y))



/*****************************************************************************/
// ERRORS
/*
 * error definitions
 */
typedef enum {
    // Command succeeded.
    CANOPY_SUCCESS = 0,
    
    // An unknown error occured.
    CANOPY_ERROR_UNKNOWN,

    // Bad credentials
    CANOPY_ERROR_BAD_CREDENTIALS,

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

    // Requested variable is already used
    CANOPY_ERROR_VAR_IN_USE,

    // Requested variable was not found
    CANOPY_ERROR_VAR_NOT_FOUND,

    // Requested variable has never been set
    CANOPY_ERROR_VAR_NOT_SET,

    /* we've run out of memory during dynamic memory allocation */
    CANOPY_ERROR_OUT_OF_MEMORY,

    /* The supplied buffer is too small to hold a result. */
    CANOPY_ERROR_BUFFER_TOO_SMALL,

    /* there's been an error emiting a JSON string */
    CANOPY_ERROR_JSON,

    /* Network error, either HTTP of websocket */
    CANOPY_ERROR_NETWORK,
} canopy_error;

struct canopy_error_strings {
    canopy_error err;
    const char *str;
};
const static struct canopy_error_strings canopy_error_strings_table[] = {
        {CANOPY_SUCCESS, "success"},
        {CANOPY_ERROR_UNKNOWN, "unknown error"},
        {CANOPY_ERROR_BAD_CREDENTIALS, "bad credentials"},
        {CANOPY_ERROR_FATAL, "fatal error"},
        {CANOPY_ERROR_INCOMPATIBLE_LIBRARY_VERSION,
                "incompatible library version"},
        {CANOPY_ERROR_NOT_IMPLEMENTED, "not implemented yet"},
        {CANOPY_ERROR_BAD_PARAM, "bad parameter"},
        {CANOPY_ERROR_WRONG_TYPE, "wrong datatype"},
        {CANOPY_ERROR_AGAIN, "try again"},
        {CANOPY_ERROR_CANCELLED, "operation was cancelled"},
        {CANOPY_ERROR_VAR_IN_USE, "cloud variable already in use"},
        {CANOPY_ERROR_VAR_NOT_FOUND, "cloud variable not found"},
        {CANOPY_ERROR_VAR_NOT_SET, "cloud variable has never been set"},
        {CANOPY_ERROR_OUT_OF_MEMORY, "out of memory"},
        {CANOPY_ERROR_BUFFER_TOO_SMALL, "buffer too small"},
        {CANOPY_ERROR_JSON, "could not emit a JSON string"},
        {CANOPY_ERROR_NETWORK, "network error"},
};

inline static const char *canopy_error_string(canopy_error err) {
    return canopy_error_strings_table[err].str;
}

/*****************************************************************************/
// CONTEXT

/*
 * Represents all of the data known to the library.  The intent is that the
 * context gets initialized once at the beginning of time.
 *
 * NOTE: None of this stuff should be accessed directly by the client.  These
 * details may change in the future.
 */
typedef struct canopy_context {

    /* used with the status reporting stuff */
    int update_period;

    /* List of remotes known to the library.  This may not be needed. */
    struct canopy_remote *remotes;

    /* stuff related to logging */
    bool enabled;
    char* log_file;
    int level;
} canopy_context_t;


/* Initialize a new context.
 * Returns CANOPY_ERROR_INCOMPATIBLE_LIBRARY_VERSION if the version of the
 * library you have linked with is incompatible with the header file you are
 * using.
 *
 *      <update_period> is the rate in seconds at which the library updates the
 *      state of things from the remotes.
 */
extern canopy_error canopy_ctx_init(canopy_context_t *ctx,
        int update_period);

// Shutdown context
// Closes any connections that might still be open.
// Frees any allocated memory
extern canopy_error canopy_ctx_shutdown(canopy_context_t *ctx);

// Set logging options for a context.
//
// <enabled> enables/disables logging.  Defaults to false.
//
// <logfile> specifies the name of the file to log to.  Pass in NULL to
// use stderr or the system's default logging destination.  Defaults to
// NULL.
//
// <level> defines the logging level.  See canopy_os.h for level definitions.
extern canopy_error canopy_ctx_set_logging(canopy_context_t *ctx,
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
extern canopy_error canopy_ctx_get_logging(canopy_context_t *ctx,
        bool *enabled,
        char **logfile,
        size_t *logfile_len,
        int *level);


/*****************************************************************************/
// BARRIERS
//

/*
 * This doesn't belong here based on what it defines, it's here becaused
 * it's used here.
 */
typedef enum canopy_credential {
    CANOPY_INVALID_CREDENTIAL_TYPE = 0,
    CANOPY_DEVICE_CREDENTIALS,                /* device */
    CANOPY_USER_CREDENTIALS                   /* user */
} canopy_credential_type;

/*
 * Defines the data needed by a barrier.
 */
typedef struct canopy_barrier {
    struct canopy_remote *remote;
    canopy_error (*canopy_barrier_cb)(struct canopy_barrier *barrier,
                                      void *userdata);
    void *userdata;
    enum canopy_credential type;
    union {
        struct canopy_device *device;
        struct canopy_user *user;
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
extern canopy_error canopy_barrier_wait_for_complete(canopy_barrier_t *barrier,
        int timeout_ms);

// Cancels the barrier.
// Any threads blocked in canopy_barrier_wait_for_complete will return with
// CANOPY_ERROR_CANCELLED.
// No further callbacks will be triggered for this barrier.
// After calling this it is safe to deallocate the barrier.
extern canopy_error canopy_barrier_cancel(canopy_barrier_t *barrier);

// Establish a callback that will be triggered when the operation has
// completed.  Triggers the callback immediately if the requested operation has
// already finished.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <cb> is the callback to trigger.
//
// <userdata> is user data that gets passed along to the callback.
extern canopy_error canopy_barrier_setup_callback(canopy_barrier_t *barrier,
        canopy_barrier_cb cb,
        void *userdata);

// Check if an asyncrhonous operation has completed.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// Returns CANOPY_SUCCESS if the operation is complete.
// Returns CANOPY_ERROR_AGAIN if the operation has not yet finished.
extern canopy_error canopy_barrier_is_complete(canopy_barrier_t *barrier);

// Get the result of an asynchronous request for a device object.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <device> will store the obtained device object, on success.
//
// If the request is not yet complete, returns CANOPY_ERROR_AGAIN.
// If the request was not for a device object, returns CANOPY_ERROR_WRONG_TYPE.
extern canopy_error canopy_barrier_get_device(canopy_barrier_t *barrier,
        struct canopy_device *device);

// Get the result of an asynchronous request for a user object.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <user> will store the obtained user object, on success.
//
// If the request is not yet complete, returns CANOPY_ERROR_AGAIN.
// If the request was not for a device object, returns CANOPY_ERROR_WRONG_TYPE.
extern canopy_error canopy_barrier_get_user(canopy_barrier_t *barrier,
        struct canopy_user *user);


/*****************************************************************************/
// PERMISSIONS
//
//  Results returned from the server are the "intersection" of objects that
//  meet three criteria:
//      1) Authenticated entity (user->remote credentials or device->remote
//         credentials) has access.
//      2) The device/user specified in the URL has access.
//      3) It meets any filtering criteria.
//
//  In the case of:
//
//      GET /api/device/self/devices
//
//  The criteria (1) and (2) are the same, because the authenticated device is
//  also the device specified in the URL (as "self").
//
//  When criteria (1) and (2) differ, it can be interpreted as "Show me a list
//  of entities, from the perspective of the device/user in the URL".  This is
//  particularly useful for superusers who may have access to everything, but
//  would like to see permission relationships (such as the "toaster" has
//  access to the "fire alarm").
//
//  WWW-Authenticate: Basic "UUID:SECRET"
//
//  
//
//  curl -u "greg:password" https://dev02.canopy.link/api/user/self/devices
//
//  curl -u "TOASTER:SECRET" https://dev02.canopy.link/api/device/self/devices
//


typedef struct canopy_permissions {
    // TBD
} canopy_permissions_t;

/*****************************************************************************/

/*
 * Authentication type, right now only basic is supported
 */
typedef enum {
    CANOPY_DEFUALT_AUTH_TYPE = 0,
    CANOPY_BASIC_AUTH,
} canopy_auth_type;

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
 * This stuff provides the text string used for the canopy_active_status as
 * used in the filters.  The idea is that you would use this lookup table when
 * referring to canopy_active_status.
 */
#define  CANOPY_ACTIVE_STATUS "system.active_status"
struct activity_status {
    canopy_active_status status;
    const char *str;
};
const static struct activity_status activity_status_table[] = {
        {CANOPY_ACTIVE_STATUS_INVALID, "status_invalid"},
        {CANOPY_ACTIVE_STATUS_DONT_CARE, "status_dont_care"},
        {CANOPY_NEWLY_CREATED, "status_newly_created"},
        {CANOPY_INACTIVE, "status_inactive"},
        {CANOPY_ACTIVE, "status_active"},
};
inline static const char *activity_status_string(canopy_active_status status) {
    return activity_status_table[status].str;
}
inline static canopy_active_status activity_status_from_string(const char* str, int len) {
    int i;
    for (i = 0; i < sizeof(activity_status_table); i++) {
        int tl = sizeof(activity_status_table[i].str);
        int small = LOCAL_MIN(len, tl);
        if (strncmp(str, activity_status_table[i].str, small) == 0) {
            return activity_status_table[i].status;
        }
    }
    return CANOPY_ACTIVE_STATUS_INVALID;
}


/*****************************************************************************/

// canopy_ws_connection_status enum identifies whether or not a device is
// currently connected to the server over websockets.
typedef enum {
    // Invalid value, or "we don't care" value when filtering.
    CANOPY_WS_CONNECTION_STATUS_INVALID = 0,
    CANOPY_WS_CONNECTION_STATUS_DONT_CARE = CANOPY_WS_CONNECTION_STATUS_INVALID,

    /* The websocket interface is not being used.  (this is not an error) */
    CANOPY_WS_CONNECTION_STATUS_WS_NOT_USED,

    // Device has never communicated with the server.
    CANOPY_WS_NEVER_CONNECTED,

    // Device has communicated with the server in past, but not recently.
    CANOPY_WS_DISCONNECTED,

    // Device communicated with the server recently.
    CANOPY_WS_CONNECTED,
} canopy_ws_connection_status;

/*
 * This stuff provides the text string used for the canopy_ws_connection_status
 * as used in the filters.  The idea is that you would use this lookup table
 * when referring to canopy_ws_connection_status.
 */
#define  CANOPY_WS_CONNECTION_STATUS "system.ws_connection_status"
struct ws_connection_status {
    canopy_ws_connection_status status;
    const char *str;
};
const static struct ws_connection_status ws_connection_status_table[] = {
        {CANOPY_WS_CONNECTION_STATUS_INVALID, "ws_connection_status_invalid"},
        {CANOPY_WS_CONNECTION_STATUS_DONT_CARE,
                            "ws_connection_status_dont_care"},
        {CANOPY_WS_CONNECTION_STATUS_WS_NOT_USED,
                            "ws_connection_status_ws_not_used"},
        {CANOPY_WS_NEVER_CONNECTED, "ws_connection_status_never_connected"},
        {CANOPY_WS_DISCONNECTED, "ws_connection_status_disconnected"},
        {CANOPY_WS_CONNECTED, "ws_connection_status_connected"},
};
inline static const char *ws_connection_status_string(canopy_ws_connection_status status) {
    return ws_connection_status_table[status].str;
}
inline static canopy_ws_connection_status ws_connection_statum_string(const char* str, int len) {
    int i;
    for (i = 0; i < sizeof(ws_connection_status_table); i++) {
        int tl = sizeof(ws_connection_status_table[i].str);
        int small = LOCAL_MIN(len, tl);
        if (strncmp(str, ws_connection_status_table[i].str, small) == 0) {
            return ws_connection_status_table[i].status;
        }
    }
    return CANOPY_WS_CONNECTION_STATUS_INVALID;
}



/*******************************************************************
 * Parameters to use when talking to a remote.
 */
typedef struct canopy_remote_params {
    canopy_credential_type   credential_type;
    char                     *name;        // user's username or device id
    char                     *password;    // user's password or device secret
    uint16_t                 http_port;    // 0 for default
    uint16_t                 https_port;   // 0 for default
    bool                     use_http;     // true to use HTTP
    bool                     skip_cert_check;// true to ignore SSL cert errors
    canopy_auth_type         auth_type;    // Defaults to BASIC
    char                     *remote;      // hostname or IP  of remote server
    bool                     use_ws;       // hint: use websockets if available
    bool                     persistent;   // hint: keep comm channel open
} canopy_remote_params_t;

/*
 * canopy_remote:
 *         used to hold information about the remote we're using.
 *         The details of how to connect to the remote is passed in as part
 *         of the params.
 */
typedef struct canopy_remote {
    struct canopy_remote            *next;       /* pointer to next remote */
    struct canopy_context           *ctx;        /* back pointer to the context */
    struct canopy_remote_params     *params;     /* params for this remote */

    /* general purpose buffer needs to be supplied by client     */
    char                            *rcv_buffer;
    size_t                          rcv_buffer_size;
    int                             rcv_end;

    canopy_ws_connection_status     ws_status;
    bool                            ws_connected;/* currently connection WS */
    canopy_active_status            active_status;
    cos_time_t                      last_activity;
} canopy_remote_t;



/*****************************************************************************/

/*
 * Query specification stuff.
 *
 * A query consists of a list of filters, a sort order, and a limit specifying
 * how many devices or users to return.
 *
 * Filters consist of a list of filter_terms and term relations.  The first
 * filter in the chain on the query is the first entry on the stack, followed
 * by the second....
 *
 * The filter is sent to the server in the URL query string.
 * 
 * Pre-encoded Example:
 *      (temperature > 40 && temperature < 80.5) || system.ws_connected == false
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

/******************************************************
 *  PARENTHESIS FILTER ELEMENT
 */
typedef struct paren_filter {
    bool open; /* 1=open paren, 0=close paren */
} paren_filter_t;

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
    PAREN,
};

/*
 * A canopy_filter is a linked list of items that contribute to the overall
 * filter operation.
 */
typedef struct canopy_filter {
    struct canopy_filter    *next;
    enum ftype type;
    union {
        filter_term_t term;
        unary_filter_t unary;
        boolean_filter_t boolean;
        paren_filter_t paren;
    } onion;
} canopy_filter_t;

/*
 * This is used during the construction of a filter list.
 */
typedef struct canopy_filter_root {
    canopy_filter_t *head;        /* points to the start of the filter stack */
    canopy_filter_t *tail;        /* tracks the tail of the list */
} canopy_filter_root_t;

extern canopy_error append_term_filter(canopy_filter_root_t *root,
        canopy_filter_t *ft,
        const char *variable_name,
        const char *value, /*  TBD should we use canopy_var_value_t here? */
        canopy_relation_op relation
        );

extern canopy_error append_unary_filter(canopy_filter_root_t *root,
        canopy_filter_t *ft,
        enum unary_type type,
        const char *variable_name    /* only used for HAS */
        );

extern canopy_error append_boolean_filter(canopy_filter_root_t *root,
        canopy_filter_t *ft,
        enum boolean_type type);

extern canopy_error append_open_paren_filter(canopy_filter_root_t *root,
        canopy_filter_t *ft);
extern canopy_error append_close_paren_filter(canopy_filter_root_t *root,
        canopy_filter_t *ft);


/************************************************************
 * Canopy sort definitions
 */
/*
 *  Sort order is sent in the URL query string.  It is a comma-separated list
 *  of variable names.  By default the order is ascending.  Bang (!) in front
 *  of a variable name is used to specify descending order.
 *
 *  ex:
 *      "temperature,!humidity"
 */
enum sort_direction {
    NONE=0,
    ASCENDING=1,
    DESCENDING=2,
    RANDOM=3
};
typedef struct canopy_sort_term {
    char *varname;
    enum sort_direction direction;
} canopy_sort_term_t;

typedef struct canopy_sort {
    int cnt;
    canopy_sort_term_t *terms;
} canopy_sort_t;

/************************************************************
 * Canopy limit definitions
 */
/*
 *  Limit is sent in the URL query string, with start followed by a comma ","
 *  followed by count.
 *
 *  ex:
 *      5,30
 */
typedef struct canopy_limits {
    uint32_t             start;    /* 0 starts at the head of the list */
    uint32_t             count;    /* how many to return */
} canopy_limits_t;


typedef struct canopy_query {
    /* the list used for filtering which devices to report. */
    canopy_filter_root_t *filter_root;

    /* defines the sort order.  If null the result list is unordered. */
    canopy_sort_t *sort;

    /* How many to return */
    canopy_limits_t *limits;
} canopy_query_t;


/*****************************************************************************/

/*
 *     A few words about memory usage.
 *
 *     The various calls into the library provide the storage for the various
 *     data structures.  The exception to this is the way memory is allocated
 *     for device variables, the library allocates the memory for variables.
 *     (there are places where the library
 *     needs to allocate memory for itself, primarily variables that are
 *     created external to the device.)
 *
 */

/*****************************************************************************/
// REMOTE

/*
 * Initializes a new remote object.  Does not necessarily open a TCP or
 * websocket connection to the server, but initializes an object with the
 * parameters to do so when needed.
 *
 *      <ctx> is the context.
 *
 *      <params> must be populated by the caller.  Sets all of the options for
 *      subsequent connections to the server.
 *
 *      <rcv_buffer> is a pointer to storage that gets used as temporary data
 *      primarily as the buffer used for payload and response
 *      communication.  (http)
 *
 *      <rcv_buffer_size> is the size of the temporary buffer.
 *
 *      <remote> is a remote object that is initialized by this call.
 *
 */
extern canopy_error canopy_remote_init(canopy_context_t *ctx,
        canopy_remote_params_t *params,
        char *rcv_buffer,
        size_t rcv_buffer_size,
        canopy_remote_t *remote);

/*
 * Shutdown a remote object.
 * Closes persistent connection to server, if any.
 * Frees any allocated memory.
 */
extern canopy_error canopy_remote_shutdown(canopy_remote_t *remote);

/* Get the remote's clock in milliseconds.  The returned value has no relation
 * to wall clock time, but is monotonically increasing and is reported
 * consistently by the remote to anyone who asks.
 */
extern canopy_error canopy_remote_get_time(canopy_remote_t *remote,
        cos_time_t *time,
        canopy_barrier_t *barrier);

/* Get our version of the remote's clock in milliseconds.  This is based on the
 * time obtained the last time canopy_remote_get_clock_time was called, plus
 * however much time has elapsed since then.
 *
 * Returns CANOPY_ERROR_AGAIN if canopy_remote_get_time() has never been
 * called for <remote>.
 */
extern canopy_error canopy_get_local_time(canopy_remote_t *remote,
        cos_time_t *time);

/*
 *
 * Get a list of devices from the server based on the filters in a device query
 * object.
 *
 * <remote> is the remote object used for connecting to the server.
 *
 * <query> contains the constraints that devices must satisfy to be included in
 * the resulting list.
 *
 * <max_count> is the maximum number of devices to fetch.
 *
 * <devices> is an array of at least <max_count> device objects that will store
 * the results of this operation.  The library will copy the internal memory
 * holding the device into the array provided.  It's up to the client to
 * manage its own memory for these
 *
 * <barrier> will store a new barrier object that can be used to obtain the
 * result when it is ready.  If NULL, this operation will block the current
 * thread.
 *
 */
extern canopy_error canopy_remote_get_devices(canopy_remote_t *remote,
        struct canopy_query *query,
        size_t max_count, 
        struct canopy_device **devices,
        struct canopy_barrier *barrier);

/******************************************************************************
 * Get the device based on the authentication information provided to
 *  canopy_remote_init.
 *
 *  <remote> is the remote object used for connecting to the server.
 *
 *  <device> points to an uninitialized canopy_device_t object that will be
 *  initialized and set to the device object returned from the server.  If
 *  <barrier> is NULL, this will contain the result upon a successful return.
 *  If <barrier> is provided, this will be updated when the result is ready,
 *  and will also be passed along to the barrier object.
 *
 *  <barrier> will store a new barrier object that can be used to obtain the
 *  result when it is ready.  If NULL, this operation will block the current
 *  thread.
 */
extern canopy_error canopy_get_self_device(canopy_remote_t *remote,
        struct canopy_device *device,
        canopy_barrier_t *barrier);

/*
 *
 *  Get the device based on the authentication information provided to
 *  canopy_remote_init.
 *
 *  <remote> is the remote object used for connecting to the server.
 *
 *  <user> will store the user object returned from the server.  If
 *  <barrier> is NULL, this will contain the result upon a successful return.
 *  If <barrier> is provided, this will be updated when the result is ready, and
 *  will also be passed along to the barrier object.
 *
 *  <barrier> will store a new barrier object that can be used to obtain the
 *  result when it is ready.
 */
extern canopy_error canopy_get_self_user(canopy_remote_t *remote,
        struct canopy_user *user,
        canopy_barrier_t *barrier);

/*****************************************************************************/
// DEVICE

#define CANOPY_NOTE_MAX_LENGTH 1024
#define CANOPY_FRIENDLY_NAME_MAX_LENGTH 128
#define CANOPY_DEVICE_ID_MAX_LENGTH 36
#define CANOPY_SECRET_KEY_LENGTH    128

/*
 * canopy_device:
 *         represents a device known locally or to a remote
 *
 */
typedef struct canopy_device {
    struct canopy_device    *next;        /* hung off of User or remote */
    char                    device_id[CANOPY_DEVICE_ID_MAX_LENGTH];
    char                    secret_key[CANOPY_SECRET_KEY_LENGTH];
    char                    friendly_name[CANOPY_FRIENDLY_NAME_MAX_LENGTH];
    bool                    friendly_name_dirty;
    char                    location_note[CANOPY_NOTE_MAX_LENGTH];
    bool                    location_note_dirty;
    bool                    ws_connected;
    canopy_remote_t         *remote;
#ifdef HAVE_MEMORY
    struct canopy_hash_table var_hash;
#else
    struct canopy_var       *vars;        /* list of vars on this device */
#endif
} canopy_device_t;

/*
 * Initializes a new local device object without interacting with the remote.
 * The initialized device starts with no cloud variables and no friendly name.
 *
 *      <device> points to the uninitialized canopy_device_t to initialize.
 *
 *      <remote> configures the remote to use for syncing with the cloud.
 *
 *      <device_id> is the ID of this device.  If NULL, a random type-4 UUID
 *          will be assigned.
 *
 * Typically canopy_get_self_device() should be used instead, which initializes
 * a new device by pulling from the remote.
 */
extern canopy_error canopy_device_init(
        canopy_device_t *device, 
        canopy_remote_t *remote,
        const char *device_id);

/*
 * Get device's friendly name.  This is a local operation that does not
 * interact with the remote.
 *
 * Copies the friendly_name to buffer pointed to by <friendly_name>.  Copies at
 * most <len> bytes.  Returns the error CANOPY_ERROR_BUFFER_TOO_SMALL if <len>
 * is not large enough to hold the result.
 */
extern canopy_error canopy_device_get_friendly_name(
        canopy_device_t *device, 
        char *friendly_name, 
        size_t len);

/*
 * Get device's location note.  This is a local operation that does not
 * interact with the remote.
 *
 * Copies the location_note to buffer pointed to by <email>.  Copies at most
 * <len> bytes.  Returns the error CANOPY_ERROR_BUFFER_TOO_SMALL if <len> is
 * not large enough to hold the result.
 */
extern canopy_error canopy_device_get_location_note(
        canopy_device_t *device, 
        char *location_note, 
        size_t len);

/*
 * Set device's friendly name.  This is a local operation that does not
 * interact with the remote.
 */
extern canopy_error canopy_device_set_friendly_name(
        canopy_device_t *device, 
        const char *friendly_name);

/*
 * Set device's location note.  This is a local operation that does not
 * interact with the remote.
 */
extern canopy_error canopy_device_set_location_note(
        canopy_device_t *device, 
        const char *location_note);

/*
 * Updates a device object's status and properties from the remote server.  Any
 * status or properties with a more recent clock ms value will be updated
 * locally.
 * 
 *  <device> must point to an initialized canopy_device_t object (for example,
 *  obtained from canopy_get_self_device()).
 */
extern canopy_error canopy_device_update_from_remote   (
        canopy_remote_t *remote,
        canopy_device_t *device, 
        canopy_barrier_t *barrier);

/*
 * Updates a device object's status and properties to the remote server.  Any
 * status or properties with a more recent clock ms value will be updated
 * remotely.
 *
 *  <device> must point to an initialized canopy_device_t object (for example,
 *  obtained from canopy_get_self_device()).
 */
extern canopy_error canopy_device_update_to_remote (
        canopy_remote_t *remote,
        canopy_device_t *device, 
        canopy_barrier_t *barrier);

/*
 * Synchronizes a device object with the remote server.
 * (Potential revisit).  Server should do merge.  Does it need to be atomic?
 * 
 * Roughly equivalent to:
 *  canopy_device_update_from_remote(device, remote, NULL);
 *  canopy_device_update_to_remote(device, remote, barrier);
 *
 *  <device> must point to an initialized canopy_device_t object (for example,
 *  obtained from canopy_get_self_device()).
 */
extern canopy_error canopy_device_sync_with_remote (
        canopy_remote_t *remote,
        canopy_device_t *device, 
        canopy_barrier_t *barrier);

/*
 * Get the active status for a device.
 */
extern canopy_error canopy_device_get_active_status (
        canopy_device_t *device, 
        canopy_active_status *active_status,
        canopy_ws_connection_status *ws_status);

/*****************************************************************************/
// USERS

/*
 * TODO:  This needs some thought about who supplies the memory for the device
 * list.
 */

typedef struct canopy_user {
    struct canopy_user *next;
    canopy_remote_t *remote;
    bool is_activated;
    char *name;
} canopy_user_t;

// Goes to remote
extern canopy_error canopy_create_user(canopy_remote_t *remote,
        const char *username,
        const char *password,
        const char *email,
        bool skip_email,
        canopy_user_t *out_user,
        canopy_barrier_t *barrier);

// Goes to remote
extern canopy_error canopy_user_create_devices(canopy_user_t *user,
        uint32_t quantity,
        char **names,
        canopy_device_t **out_devices,
        canopy_barrier_t *barrier);

/*
 * Obtained from local copy of the user.  To fetch latest data from the remote
 * be sure to call canopy_user_update_from_remote() or
 * canopy_user_sync_with_remote().
 */
extern canopy_error canopy_user_get_email(canopy_user_t *user, char **email);
extern canopy_error canopy_user_get_username(canopy_user_t *user, char **username);
extern canopy_error canopy_user_is_validated(canopy_user_t *user, bool *validated);


/*
 * Changes local copy of the user's email.  To update the remote be sure to
 * call canopy_user_update_to_remote() or canopy_user_sync_with_remote().
 */
extern canopy_error canopy_user_set_email(canopy_user_t *user, const char *email);
extern canopy_error canopy_user_set_password(canopy_user_t *user,
        const char *old_password,
        const char *new_password);

/*
 * Updates a user object's properties (excluding devices) from the remote
 * server.  Any properties with a more recent clock ms value will be updated
 * locally.
 */
extern canopy_error canopy_user_update_from_remote(
        canopy_remote_t *remote,
        canopy_user_t *user, 
        canopy_barrier_t *barrier);

/*
 * Updates a user object's properties (excluding devices) to the remote
 * server.  Any properties with a more recent clock ms value will be updated
 * remotely.
 */
extern canopy_error canopy_user_update_to_remote(
        canopy_remote_t *remote,
        canopy_user_t *user, 
        canopy_barrier_t *barrier);

/*
 * Synchronizes a user object with the remote server.
 * (Potential revisit).  Server should do merge.  Does it need to be atomic?
 * 
 * Roughly equivalent to:
 *  canopy_user_update_from_remote(user, remote, NULL);
 *  canopy_user_update_to_remote(user, remote, barrier);
 */
extern canopy_error canopy_user_sync_with_remote(
        canopy_user_t *user, 
        canopy_remote_t *remote,
        canopy_barrier_t *barrier);

/*****************************************************************************/
// QUERIES

/*
 * Get list of devices based on a query.
 *
 *  The list of devices obtained will only include devices that satisfy these criteria:
 *      1) The credentials provided to the remote can access it.
 *      2) The <device> can access it.
 *      3) It satifsies the query.
 *      
 *      WWW-Authentication: BASIC <remote.name>:<remote.password>
 *      GET /api/device/<device.uuid>/devices?sort=<query.sort>&filter=<query.filter>&limit=<query.limit>
 */
extern canopy_error canopy_device_devices(
        canopy_remote_t *remote, 
        canopy_device_t *device, 
        canopy_query_t *query,
        canopy_barrier_t *barrier);

/*
 * Get list of users a device has access to based on a query.
 */
extern canopy_error canopy_device_users(
        canopy_remote_t *remote, 
        canopy_device_t *device, 
        canopy_query_t *query,
        canopy_barrier_t *barrier);

/*
 * Get list of devices a user has access to based on a query.
 */
extern canopy_error canopy_user_devices(
        canopy_remote_t *remote, 
        canopy_user_t *user, 
        canopy_query_t *query,
        canopy_barrier_t *barrier);

/*
 * Get list of users a user has access to based on a query.
 */
extern canopy_error canopy_user_users(
        canopy_remote_t *remote, 
        canopy_user_t *user, 
        canopy_query_t *query,
        canopy_barrier_t *barrier);

/*****************************************************************************/

/* 
 * CLOUD VARIABLES
 */
typedef enum {
    CANOPY_VAR_DIRECTION_INVALID=0,
    CANOPY_VAR_OUT,
    CANOPY_VAR_IN,
    CANOPY_VAR_INOUT,
} canopy_var_direction;

struct canopy_var_direction_strings {
    canopy_var_direction dir;
    const char *str;
};
const static struct canopy_var_direction_strings canopy_var_direction_table[] = {
        {CANOPY_VAR_DIRECTION_INVALID, "INVALID"},
        {CANOPY_VAR_OUT, "out"},
        {CANOPY_VAR_IN, "in"},
        {CANOPY_VAR_INOUT, "inout"},
};
inline static const char *canopy_var_direction_string(canopy_var_direction dir) {
    return canopy_var_direction_table[dir].str;
}
inline static canopy_var_direction direction_from_string(const char* str, int len) {
    int i;
    for (i = 0; i < sizeof(canopy_var_direction_table); i++) {
        int tl = sizeof(canopy_var_direction_table[i].str);
        int small = LOCAL_MIN(len, tl);
        if (strncmp(str, canopy_var_direction_table[i].str, small) == 0) {
            return canopy_var_direction_table[i].dir;
        }
    }
    return CANOPY_VAR_DIRECTION_INVALID;
}


/**************************************************************************
 * Data types
 */
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

struct canopy_var_datatype_strings {
    canopy_var_datatype type;
    const char *str;
};
const static struct canopy_var_datatype_strings canopy_var_datatype_table[] = {
        {CANOPY_VAR_DATATYPE_INVALID, "invalid"},
        {CANOPY_VAR_DATATYPE_VOID, "void"},
        {CANOPY_VAR_DATATYPE_STRING, "string"},
        {CANOPY_VAR_DATATYPE_BOOL, "bool"},
        {CANOPY_VAR_DATATYPE_INT8, "int8"},
        {CANOPY_VAR_DATATYPE_UINT8, "uint8"},
        {CANOPY_VAR_DATATYPE_INT16, "int16"},
        {CANOPY_VAR_DATATYPE_UINT16, "uint16"},
        {CANOPY_VAR_DATATYPE_INT32, "int32"},
        {CANOPY_VAR_DATATYPE_UINT32, "uint32"},
        {CANOPY_VAR_DATATYPE_FLOAT32, "float32"},
        {CANOPY_VAR_DATATYPE_FLOAT64, "float64"},
        {CANOPY_VAR_DATATYPE_DATETIME, "datetime"},
        {CANOPY_VAR_DATATYPE_STRUCT, "struct"},
        {CANOPY_VAR_DATATYPE_ARRAY, "array"},
};
inline static const char *canopy_var_datatype_string(canopy_var_datatype type) {
    return canopy_var_datatype_table[type].str;
}
inline static canopy_var_datatype datatype_from_string(const char* str, int len) {
    int i;
    for (i = 0; i < sizeof(canopy_var_datatype_table); i++) {
        int tl = sizeof(canopy_var_datatype_table[i].str);
        int small = LOCAL_MIN(len, tl);
        if (strncmp(str, canopy_var_datatype_table[i].str, small) == 0) {
            return canopy_var_datatype_table[i].type;
        }
    }
    return CANOPY_VAR_DATATYPE_INVALID;
}


/*
 * Canopy var value
 */
#define CANOPY_VAR_VALUE_MAX_LENGTH 128
struct canopy_var_value {
    canopy_var_datatype type;
    union {
        char val_string[CANOPY_VAR_VALUE_MAX_LENGTH];
        bool val_bool;
        int8_t val_int8;
        int16_t val_int16;
        int32_t val_int32;
        uint8_t val_uint8;
        uint16_t val_uint16;
        uint32_t val_uint32;
        float val_float;
        double val_double;
        cos_time_t val_time;
    } value;
};
typedef struct canopy_var_value canopy_var_value_t;

#define CANOPY_VAR_NAME_MAX_LENGTH 128
struct canopy_var {
    struct canopy_var       *next;    /* linked list of variables, hung off device */
    struct canopy_device    *device;
    canopy_var_direction    direction;
    canopy_var_datatype     type;     /* duplicate of type in the value */
    char name[CANOPY_VAR_NAME_MAX_LENGTH];
    bool                    set;      /* This variable has been set */
    bool                    dirty;    /* Variable needs to be sent to remote */
    cos_time_t              last;     /* when was it changed with remote */
    struct canopy_var_value val;      /* yes, not a pointer, real storage */
};
// typedef struct canopy_var canopy_var_t;

/*****************************************************************************
 *
 * Creates the variable defined and attaches it to the device.
 *
 *     <device>    the device
 *     <direction>    access direction
 *     <type>        variable datatype
 *     <name>        the variables name
 *     <out_var>    this is a pointer to a pointer which will be set to the
 *                 system-allocated variable.
 *
 *     This will return CANOPY_ERROR_VAR_IN_USE if the variable already exists
 *
 */
canopy_error canopy_device_var_declare(canopy_device_t *device,
        canopy_var_direction direction,
        canopy_var_datatype type,
        const char *name,
        struct canopy_var **out_var);

/****************************************************************************
 * Looks up a variable by looking on the device.  If the variable does not
 * exist, this call will return CANOPY_ERROR_VAR_NOT_FOUND.
 *
 * <device>        the device
 * <var_name>    the name of the variable to look up
 * <var>        is a pointer to storage that the variable will be copied into
 */
canopy_error canopy_device_get_var_by_name(canopy_device_t *device, 
        const char *var_name, 
        struct canopy_var *var);

canopy_error canopy_var_set_bool(struct canopy_var *var, bool value);
canopy_error canopy_var_set_int8(struct canopy_var *var, int8_t value);
canopy_error canopy_var_set_int16(struct canopy_var *var, int16_t value);
canopy_error canopy_var_set_int32(struct canopy_var *var, int32_t value);
canopy_error canopy_var_set_uint8(struct canopy_var *var, uint8_t value);
canopy_error canopy_var_set_uint16(struct canopy_var *var, uint16_t value);
canopy_error canopy_var_set_uint32(struct canopy_var *var, uint32_t value);
canopy_error canopy_var_set_datetime(struct canopy_var *var, cos_time_t value);
canopy_error canopy_var_set_float32(struct canopy_var *var, float value);
canopy_error canopy_var_set_float64(struct canopy_var *var, double value);
canopy_error canopy_var_set_string(struct canopy_var *var, const char *value, size_t len);

canopy_error canopy_var_get_bool(struct canopy_var *var,
        bool *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_int8(struct canopy_var *var,
        int8_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_int16(struct canopy_var *var,
        int16_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_int32(struct canopy_var *var,
        uint32_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_uint8(struct canopy_var *var,
        uint8_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_uint16(struct canopy_var *var,
        uint16_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_uint32(struct canopy_var *var,
        uint32_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_datetime(struct canopy_var *var,
        cos_time_t *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_float32(struct canopy_var *var,
        float *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_float64(struct canopy_var *var,
        double *value,
        cos_time_t *last_time);
canopy_error canopy_var_get_string(struct canopy_var *var,
        char *buf, size_t len,    /* buf is null terminated */
        cos_time_t *last_time);

#ifdef __cplusplus
}
#endif

#endif /* CANOPY_MIN_INCLUDED */



