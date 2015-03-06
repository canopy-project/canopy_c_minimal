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

#ifndef CANOPY_MIN_INCLUDED
#define CANOPY_MIN_INCLUDED

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
    bool use_ws; // use websockets if available
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

typedef struct canopy_device_query {
    // TDB
} canopy_device_query_t;

/*****************************************************************************/
// Initialize a new context
void canopy_ctx_init(canopy_context_t *ctx);

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


//canopy_error canopy_connection_shutdown(canopy_context_t *ctx, 
//        canopy_connection_params_t *params,
//        canopy_connection_t *conn);

canopy_error canopy_get_authenticated_device(canopy_connection_t *conn, 
        canopy_device_t *device);

canopy_error canopy_get_authenticated_user(canopy_connection_t *conn, 
        canopy_user_t *user);








#if 0
CanopyResultEnum canopy_init_client(CanopyClient_t *client);
CanopyResultEnum canopy_shutdown_client(CanopyClient_t *client);

CanopyResultEnum canopy_client_set_auth_username(CanopyClient_t *client, const char *username);
CanopyResultEnum canopy_client_set_auth_password(CanopyClient_t *client, const char *password);
CanopyResultEnum canopy_client_set_auth_device_id(CanopyClient_t *client, const char *device_id);
CanopyResultEnum canopy_client_set_auth_device_secret(CanopyClient_t *client, const char *device_secret);
CanopyResultEnum canopy_client_set_auth_type(CanopyClient_t *client, CanopyAuthTypeEnum auth_type);
CanopyResultEnum canopy_client_set_http_port(CanopyClient_t *client, uint16_t port);
CanopyResultEnum canopy_client_set_https_port(CanopyClient_t *client, uint32_t port);
CanopyResultEnum canopy_client_set_use_https(CanopyClient_t *client, bool use_https);

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
| devices(filters)                           | -> DeviceQuery
| emailAddress()                             | -> string
| isActivated()                              | -> bool
| quotas()                                   | -> object
| username()                                 | -> string
| update(params, function(Error)



//
//  canopy_device_update()
//      // among other things, triggers callbacks for changed values


#endif
