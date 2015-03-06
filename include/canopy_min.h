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

typedef struct CanopyClient_t {

} CanopyClient_t;

typedef struct CanopyAccount_t {

} CanopyAccount_t;

typedef enum {
    CANOPY_AUTH_TYPE_INVALID,
    CANOPY_AUTH_TYPE_BASIC
} CanopyAuthTypeEnum;

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
