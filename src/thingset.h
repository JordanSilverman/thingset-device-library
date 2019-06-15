/* LibreSolar MPPT charge controller firmware
 * Copyright (c) 2016-2018 Martin Jäger (www.libre.solar)
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

#ifndef __THINGSET_H_
#define __THINGSET_H_

#include "ts_config.h"
#include "jsmn.h"

#include <stdint.h>
#include <stdbool.h>

/* Protocol functions / categories
 */
// function + data object category
#define TS_INFO     0x01       // read-only device information (e.g. manufacturer, device ID)
#define TS_CONF     0x02       // configurable settings
#define TS_INPUT    0x03       // input data (e.g. set-points)
#define TS_OUTPUT   0x04       // output data (e.g. measurement values)
#define TS_REC      0x05       // recorded data (history-dependent)
#define TS_CAL      0x06       // calibration
#define TS_ANY      0x09       // any of above non-exec categories 0x01-0x08
#define TS_EXEC     0x0B       // RPC / function call
// function only
#define TS_NAME     0x0E
#define TS_AUTH     0x10
#define TS_LOG      0x11       // access log data
#define TS_PUB      0x12       // publication request
#define TS_PUBMSG   0x1F       // actual publication message

/* Status codes
 */
#define TS_STATUS_SUCCESS            0
#define TS_STATUS_ERROR             32
#define TS_STATUS_UNKNOWN_FUNCTION  33    // Function ID unknown
#define TS_STATUS_UNKNOWN_DATA_OBJ  34    // Data Object ID unknown
#define TS_STATUS_WRONG_FORMAT      35
#define TS_STATUS_WRONG_TYPE        36    // Data type not supported
#define TS_STATUS_DEVICE_BUSY       37    // Device busy
#define TS_STATUS_UNAUTHORIZED      38
#define TS_STATUS_REQUEST_TOO_LONG  39
#define TS_STATUS_RESPONSE_TOO_LONG 40
#define TS_STATUS_INVALID_VALUE     41     // value out of allowed range
#define TS_STATUS_WRONG_CATEGORY    42

/** Internal C data types (used to cast void* pointers)
 */
enum ts_type {
    TS_T_BOOL,
    TS_T_UINT64,
    TS_T_INT64,
    TS_T_UINT32,
    TS_T_INT32,
    TS_T_UINT16,
    TS_T_INT16,
    TS_T_FLOAT32,
    TS_T_STRING,
    TS_T_DECFRAC       // CBOR decimal fraction
};

/* Internal access rights to data objects
 */
#define TS_ACCESS_READ          (0x1U)
#define TS_ACCESS_WRITE         (0x1U << 1)
#define TS_ACCESS_READ_AUTH     (0x1U << 2)     // read after authentication
#define TS_ACCESS_WRITE_AUTH    (0x1U << 3)     // write after authentication
#define TS_ACCESS_EXEC          (0x1U << 4)     // execute (for RPC only)
#define TS_ACCESS_EXEC_AUTH     (0x1U << 5)     // execute after authentication


/* for CAN only...
 */
#define PUB_MULTIFRAME_EN (0x1U << 7)
#define PUB_TIMESTAMP_EN (0x1U << 6)

/** ThingSet data object struct
 *
 * id = Data object ID
 * access = one of TS_ACCESS_READ, _WRITE, _EXECUTE, ...
 * type = one of TS_TYPE_INT32, _FLOAT, ...
 * detail = exponent (10^exponent = factor to convert to SI unit) for UINT / INT
 *          decimal digits to use for plotting of floats in JSON strings
 *          lenght of string buffer for string type
 */
typedef struct data_object_t {
    const uint16_t id;
    const uint16_t category;
    const uint8_t access;
    const uint8_t type;
    const int16_t detail;
    void *data;
    const char *name;
} data_object_t;

/** Buffer for string-type and binary data
 *
 * Remark: char type data union necessary to use string functions without casts
 */
typedef struct {
    union {
        char *str;          // pointer to ASCII data
        uint8_t *bin;       // pointer to binary data
    } data;
    size_t size;            // size of the array
    size_t pos;             // index of the next free byte

} ts_buffer_t;

/** ThingSet Data Object container including size
 */
typedef struct ts_data_t {
    const data_object_t *objects;
    size_t size;
} ts_data_t;

/** Parser container for JSON data
 */
typedef struct {
    char *str;
    jsmn_parser parser;
    jsmntok_t tokens[TS_NUM_JSON_TOKENS];
    int tok_count;
} ts_parser_t;


/** Container for data object publication channel
 */
typedef struct {
    const char *name;
    const uint16_t *object_ids;  // array of data object IDs
    const size_t num;             // number of objects
    bool enabled;
} ts_pub_channel_t;


class ThingSet
{
public:
    ThingSet(const data_object_t *data, size_t num);
    ThingSet(const data_object_t *data, size_t num_obj, const ts_pub_channel_t *channels, size_t num_ch);
    ~ThingSet();

    /** Process ThingSet request
     *
     * This function also detects if JSON or CBOR format is used
     *
     * @param request Pointer to the request buffer
     * @param req_len Length of the data in the request buffer
     * @param response Pointer to the response buffer, where the result should be stored
     * @param resp_size Size of the response buffer, i.e. maximum allowed length of the response.

     * @returns Status code
     */
    int process(uint8_t *request, size_t req_len, uint8_t *response, size_t resp_size);

    void set_pub_channels(const ts_pub_channel_t *channels, size_t num);

    int set_user_password(char *password);
    int set_manufacturer_password(char *password);

    int pub_msg_json(char *resp, size_t size, unsigned int channel);
    int pub_msg_cbor(uint8_t *resp, size_t size, unsigned int channel);

    // manually supplied list of elements
    int pub_msg_cbor(uint8_t *resp, size_t size, const uint16_t pub_list[], size_t num_elements);

    // function to initialize data objects based on values stored in EEPROM
    // returns ThingSet status code
    int init_cbor(uint8_t *cbor_data, size_t len);

    void set_conf_callback(void (*callback)(void));

    const data_object_t *get_data_object(uint16_t id);
    const data_object_t *get_data_object(char *str, size_t len);

private:

    void set_request(uint8_t *resp, size_t length);

    // returns the length of the response written to response buffer or 0 in case of error
    int get_response(uint8_t *resp, size_t size);


    /** ThingSet data access function (text mode)
    *   - append requested data to resp buffer
    *   - return ThingSet status code
    */
    int data_access_json(char *req, size_t req_len, char *resp, size_t resp_size, int category);

    /**
     * List data objects in text mode (function called with empty argument)
     */
    int list_json(char *resp, size_t size, int category, bool values = false);

    /**
     * List data objects in binary mode (function called with empty argument)
     */
    int list_cbor(uint8_t *resp, size_t size, int category, bool values = false, bool ids_only = true);

    /**
     * Read data object values in text mode (function called with an array as argument)
     */
    int read_json(char *resp, size_t size, int category);

    /**
     * Read data object values in binary mode (function called with an array as argument)
     */
    int read_cbor(uint8_t *resp, size_t size, int category);

    /**
     * Write data object values in text mode (function called with a map as argument)
     */
    int write_json(char *resp, size_t size, int category);

    /**
     * Write data object values in binary mode (function called with a map as argument)
     */
    int write_cbor(uint8_t *resp, size_t size, int category, bool ignore_access);

    /**
     * Execute command in text mode (function called with a single data object name as argument)
     */
    int exec_json(char *resp, size_t size);

    /**
     * Execute command in binary mode (function called with a single data object name/id as argument)
     */
    int exec_cbor(uint8_t *resp, size_t size);

    // authentication
    int auth_json(char *resp, size_t size);
    int auth_cbor(uint8_t *resp, size_t size);

    int json_serialize_value(char *resp, size_t size, const data_object_t* data_obj);
    int json_serialize_name_value(char *resp, size_t size, const data_object_t* data_obj);

    int status_message_json(char *resp, size_t size, int code);

    const data_object_t *data_objects;
    size_t num_objects;

    const ts_pub_channel_t *pub_channels;
    size_t num_channels;

    // request raw data
    uint8_t *req;
    size_t req_len;             // length of request

    // parsed JSON data of request
    jsmntok_t tokens[TS_NUM_JSON_TOKENS];
    char *json_str;
    int tok_count;

    const char *user_pass;
    const char *manufacturer_pass;

    void (*conf_callback)(void);
};

#endif
