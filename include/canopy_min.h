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
    CANOPY_SUCCESS,
    
    // An unknown error occured.
    CANOPY_ERROR_UNKNOWN,

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
    CANOPY_INVALID_CREDENTIAL_TYPE,
    CANOPY_DEVICE_CREDENTIALS,
    CANOPY_USER_CREDENTIALS
} canopy_credential_type;

typedef enum {
    CANOPY_INVALID_AUTH_TYPE,
    CANOPY_BASIC_AUTH,
} canopy_auth_type;

typedef struct canopy_remote_params {
    canopy_credential_type credential_type;
    char *name; // user's username or device id
    char *password; // user's password or device secret
    uint16_t http_port;
    uint16_t https_port;
    bool use_http;
    canopy_auth_type auth_type;
    char *server; // hostname or IP address of server
    bool use_ws;     // use websockets if available
    bool persistent; // keep communication channel open
} canopy_remote_params_t;

// No user-servicable parts.
typedef struct canopy_connection {
    struct canopy_connection *next;
    struct canopy_client *client;
    struct canopy_remote_params *params;
} canopy_connection_t;

typedef struct canopy_context {
    // Linked-list of connections
    canopy_connection_t *connections;
} canopy_context_t;

typedef struct canopy_remote {
    // TDB
} canopy_remote_t;

typedef struct canopy_user {
    bool is_activated;
    char *name;
} canopy_user_t;

typedef struct canopy_user_query {
    // TBD
} canopy_user_query_t;

typedef struct canopy_permissions {
    // TBD
} canopy_permissions_t;

typedef struct canopy_device {
    // TDB
} canopy_device_t;


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

typedef enum {
    CANOPY_FILTER_ACTIVE_STATUS,
    CANOPY_FILTER_WS_CONNECTION_STATUS,
} filter_term_type;

typedef struct filter_term {
    filter_term_type type;

    union {
    } constraint;
}

typedef struct filter_op {
} or_filter_t;

/*
{
    "filter" : [{
            "active" : true, // implicit AND
            "creation-date" : "yesterday",
        }, // implicit OR
        {
            "connected" : true, // implicit AND
            "creation-date" : "last-year",
        }]
    }
}
*/

typedef struct canopy_device_query {
    // TBD
} canopy_device_query_t;

typedef canopy_error (*canopy_barrier_cb)(struct canopy_barrier *barrier, 
        void *userdata);

typedef struct canopy_barrier {
    canopy_remote_t *remote;
    canopy_barrier_cb cb;
    //canopy_error (*canopy_barrier_cb)(struct canopy_barrier *barrier, void *userdata);
    void *userdata;
    canopy_credential_type type;
    union {
        canopy_device_t *device;
        canopy_user_t *user;
    } result;
} canopy_barrier_t;

/*****************************************************************************/
// Initialize a new context
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

// Options:  -> "connection_opts"
//      username
//      password
//      auth-type
//      hostname
//
// State/status for the connection -> "connection"
//
/*

{
    canopy_connection_t conn;
    conn.http_port = 80;
    conn.username = "fry";
    canopy_connect_to_server(ctx, conn, status);
}
*/

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
// BARRIERS
//
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
canopy_error canopy_barrier_get_device(canopy_barrier_t *barrier, canopy_device_t *device);

// Get the result of an asynchronous request for a user object.
//
// <barrier> is a barrier object representing the asynchronous operation.
//
// <user> will store the obtained user object, on success.
//
// If the request is not yet complete, returns CANOPY_ERROR_AGAIN.
// If the request was not for a device object, returns CANOPY_ERROR_WRONG_TYPE.
canopy_error canopy_barrier_get_user(canopy_barrier_t *barrier, canopy_user_t *user);


/*****************************************************************************/
// USERS

canopy_error canopy_user_get_email(canopy_user_t *user, char **email);
canopy_error canopy_user_get_username(canopy_user_t *user, char **username);
canopy_error canopy_user_is_validated(canopy_user_t *user, bool validated);
canopy_error canopy_user_devices(canopy_user_t *user, canopy_device_query_t *query);

/*****************************************************************************/
// DEVICE QUERY

devices = user.devices().filter({"has_var" : "temperature"}).

canopy_device_query_t dq;
canopy_device_t devices[10];
canopy_user_devices(user, &dq);
canopy_device_query_filter(&dq, CANOPY_FILTER_CONNECTED, true);
canopy_device_query_list(&dq, 10, devices);
#endif


//| devices(filters)                           | -> DeviceQuery
//| emailAddress()                             | -> string
//| isActivated()                              | -> bool
//| quotas()                                   | -> object
//| username()                                 | -> string
//| update(params, function(Error)







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
