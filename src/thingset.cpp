/* ThingSet protocol client library
 * Copyright (c) 2017-2019 Martin Jäger (www.libre.solar)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ts_config.h"
#include "thingset.h"
#include "jsmn.h"
#include "cbor.h"

#include <string.h>
#include <stdio.h>

#define DEBUG 0

ThingSet::ThingSet(const data_object_t *data, size_t num)
{
    data_objects = data;
    num_objects = num;
}

ThingSet::ThingSet(const data_object_t *data, size_t num_obj, const ts_pub_channel_t *channels, size_t num_ch)
{
    data_objects = data;
    num_objects = num_obj;
    pub_channels = channels;
    num_channels = num_ch;
}

ThingSet::~ThingSet()
{

}

void ThingSet::set_pub_channels(const ts_pub_channel_t *channels, size_t num)
{
    pub_channels = channels;
    num_channels = num;
}

int ThingSet::process(uint8_t *request, size_t request_len, uint8_t *response, size_t response_size)
{
    // check if proper request was set before asking for a response
    if (request == NULL || request_len < 1)
        return 0;

    // assign private variables
    req = request;
    req_len = request_len;
    resp = response;
    resp_size = response_size;

    if (req[0] <= TS_EXEC) {          // CBOR list/read/write request
        if (req_len == 2 && (req[1] == CBOR_NULL || req[1] == CBOR_ARRAY || req[1] == CBOR_MAP)) {
            //printf("list_cbor\n");
            return list_cbor(req[0], req[1] == CBOR_MAP, req[1] == CBOR_NULL);
        }
        else if ((req[1] & CBOR_TYPE_MASK) == CBOR_MAP) {
            //printf("write_cbor\n");
            int len = write_cbor(req[0], false);
            if ((response[0] - 0x80) == TS_STATUS_SUCCESS &&
                req[0] == TS_CONF && conf_callback != NULL) {
                conf_callback();
            }
            return len;
        }
        else {  // array or single data object
            if (req[0] == TS_EXEC) {
                return exec_cbor();
            }
            else {
                //printf("read_cbor\n");
                return read_cbor(req[0]);
            }
        }
    }
    else if (req[0] == '!') {      // JSON request

        if (req_len >= 5 && strncmp((char *)req, "!info", 5) == 0) {
            return access_json(TS_INFO, 5);
        }
        else if (req_len >= 5 && strncmp((char *)req, "!conf", 5) == 0) {
            return access_json(TS_CONF, 5);
        }
        else if (req_len >= 6 && strncmp((char *)req, "!input", 6) == 0) {
            return access_json(TS_INPUT, 6);
        }
        else if (req_len >= 7 && strncmp((char *)req, "!output", 7) == 0) {
            return access_json(TS_OUTPUT, 7);
        }
        else if (req_len >= 4 && strncmp((char *)req, "!rec", 4) == 0) {
            return access_json(TS_REC, 4);
        }
        else if (req_len >= 4 && strncmp((char *)req, "!cal", 4) == 0) {
            return access_json(TS_CAL, 4);
        }
        else if (req_len >= 5 && strncmp((char *)req, "!exec", 5) == 0) {
            return access_json(TS_EXEC, 5);
        }
        /*
        else if (req_len >= 2 && strncmp((char *)req, "! ", 2) == 0) {
            function = TS_ANY;
            len_function = 2;
        }
        else if (req_len >= 4 && strncmp((char *)req, "!pub", 4) == 0) {
            function = TS_PUB;
            len_function = 4;
        }*/
        else if (req_len >= 5 && strncmp((char *)req, "!auth", 5) == 0) {
            return auth_json();
        }
        else {
            return status_message_json(TS_STATUS_UNKNOWN_FUNCTION);
        }
    }
    else {
        // not a thingset command --> ignore and set response to empty string
        response[0] = 0;
        return 0;
    }
}

void ThingSet::set_conf_callback(void (*callback)(void))
{
    conf_callback = callback;
}

const data_object_t* ThingSet::get_data_object(char *str, size_t len)
{
    //printf("get_data_object(%.*s)\n", len, str);
    for (unsigned int i = 0; i < num_objects; i++) {
        //printf("i=%d num_obj=%d name=%s\n", i, num_objects, data_objects[i].name);
        if (strncmp(data_objects[i].name, str, len) == 0
            && strlen(data_objects[i].name) == len) {  // otherwise e.g. foo and fooBar would be recognized as equal
            return &(data_objects[i]);
        }
    }
    return NULL;
}

const data_object_t* ThingSet::get_data_object(uint16_t id)
{
    for (unsigned int i = 0; i < num_objects; i++) {
        if (data_objects[i].id == id) {
            return &(data_objects[i]);
        }
    }
    return NULL;
}

void ThingSet::set_user_password(const char *password)
{
    user_pass = password;
}

void ThingSet::set_root_password(const char *password)
{
    root_pass = password;
}
