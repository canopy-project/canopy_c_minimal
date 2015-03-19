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

#include 	<stdlib.h>
#include 	<curl/curl.h>

#include	<canopy_min.h>
#include	<canopy_os.h>


struct private {
	char	*buffer;	/* the buffr being built in */
	int		buffer_len;	/* how big is the raw buffer */
	int		offset;		/* offset where the \0 is relative to the start of the buffer*/
};

// Handler for CURL write callback.  Concatenates received bytes into a
// RedStringList object, for easy conversion to a string when the whole
// response has been received.
static size_t _curl_write_handler(void *ptr, size_t size, size_t nmemb, void *userdata) {
	struct private* http = (struct private*) userdata;
	size_t len = size * nmemb;
	/*
	 * Note..  There's no NULL character at the end of the transfered data.
	 * TODO Check to make sure there's enough buffer space before the copy, so
	 * we can return the actual length we used.
	 */
	memcpy(http->buffer[http->offset], ptr, (http->buffer_len - http->offset));
	http->offset += len;
	http->buffer[http->offset + 1] = '\0';
	return len;
}


canopy_error canopy_http_post(
        struct canopy_remote	*remote,
        const char 				*url,
        const char 				*payload,
		char					*buffer,
		int						buffer_length,
		int						*used_buffer,
		struct canopy_barrier *barrier) {

	CURL *curl = NULL;
	struct private private;
	private.buffer = buffer;
	private.buffer_len = buffer_length;
	private.offset = 0;

	cos_log(LOG_LEVEL_DEBUG, "Sending payload to %s:\n%s\n\n", url, payload);

	curl = curl_easy_init();
	if (!curl) {
		cos_log(LOG_LEVEL_WARN, "Initialzation of curl failed");
		goto cleanup;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &private);

	curl_easy_perform(curl);

	used_buffer = private.offset;
	return CANOPY_SUCCESS;

cleanup:
	return CANOPY_ERROR_UNKNOWN;
}
