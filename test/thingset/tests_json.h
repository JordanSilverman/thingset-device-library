/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2017 Martin Jäger / Libre Solar
 */

#include "thingset.h"
#include "test_data.h"
#include "unity.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t req_buf[];
extern uint8_t resp_buf[];
extern ThingSet ts;

void json_wrong_command()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!abcd \"f32\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A4 Not Found.", resp_buf);
}

void json_patch_wrong_data_structure()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf [\"f32\":54.3");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A0 Bad Request.", resp_buf);

    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf{\"f32\":54.3}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A4 Not Found.", resp_buf);
}

void json_patch_array()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {    \"f32\" : 52.8,\"i32\":50.6}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);
    TEST_ASSERT_EQUAL_FLOAT(52.8, f32);
    TEST_ASSERT_EQUAL(50, i32);
}

void json_patch_readonly()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!output {\"i32_output\" : 52}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A1 Unauthorized.", resp_buf);
}

void json_patch_wrong_path()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!info {\"i32\" : 52}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A4 Not Found.", resp_buf);
}

void json_patch_unknown_node()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"i3\" : 52}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A4 Not Found.", resp_buf);
}

void json_fetch_array()
{
    //                                                      float        bool         int
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf [\"f32\",\"bool\",\"i32\"]");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. [52.80,false,50]", resp_buf);
}

void json_fetch_rounded()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf \"f32_rounded\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. 53", resp_buf);
}

void json_list_input()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!input/");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. [\"loadEnTarget\",\"usbEnTarget\"]", resp_buf);
}

void json_list_names_values_input()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!input {}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. {\"loadEnTarget\":false,\"usbEnTarget\":false}", resp_buf);
}

void json_fetch_int32_array()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf [\"arrayi32\"]");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. [[4,2,8,4]]", resp_buf);
}

void json_fetch_float_array()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf [\"arrayfloat\"]");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. [[2.27,3.44]]", resp_buf);
}

void json_pub_msg()
{
    int resp_len = ts.pub_msg_json((char *)resp_buf, TS_RESP_BUFFER_LEN, 0);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING("# {\"ui32\":4294967295,\"i32\":50,\"ui16\":65535,\"i16\":-32768,\"f32\":52.80,\"bool\":false,\"strbuf\":\"Hello World!\"}", resp_buf);
}

extern bool dummy_called_flag;

void json_exec()
{
    dummy_called_flag = 0;

    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!exec \"dummy\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":83 Valid.", resp_buf);
    TEST_ASSERT_EQUAL(1, dummy_called_flag);
}

void json_conf_callback()
{
    dummy_called_flag = 0;
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"i32\":52}");

    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);
    TEST_ASSERT_EQUAL(0, dummy_called_flag);

    ts.set_conf_callback(dummy);

    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);
    TEST_ASSERT_EQUAL(1, dummy_called_flag);
}

void json_auth_user()
{
    ts.set_user_password("user123");

    // authorize as user
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth \"user123\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":83 Valid.", resp_buf);

    // write user data
    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_user\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);

    // attempt to write admin data
    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_maker\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A1 Unauthorized.", resp_buf);
}

void json_auth_root()
{
    ts.set_maker_password("maker456");

    // authorize as root
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth \"maker456\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":83 Valid.", resp_buf);

    // write user data
    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_user\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);

    // write admin data
    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_maker\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);
}

void json_auth_failure()
{
    ts.set_user_password("pass_user");

    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth \"abc\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A9 Conflict.", resp_buf);

    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_user\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A1 Unauthorized.", resp_buf);
}

void json_auth_long_password()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth \"012345678901234567890123456789\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A9 Conflict.", resp_buf);

}

void json_auth_reset()
{
    ts.set_user_password("pass_user");

    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth \"pass_user\"");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":83 Valid.", resp_buf);

    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!auth");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":83 Valid.", resp_buf);

    req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!conf {\"secret_user\" : 10}");
    resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":A1 Unauthorized.", resp_buf);
}

void json_pub_list()
{
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!pub");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":85 Content. [\"Serial_1s\"]", resp_buf);
}

void json_pub_enable()
{
    pub_channels[0].enabled = false;
    size_t req_len = snprintf((char *)req_buf, TS_REQ_BUFFER_LEN, "!pub {\"Serial_1s\":true}");
    int resp_len = ts.process(req_buf, req_len, resp_buf, TS_RESP_BUFFER_LEN);
    TEST_ASSERT_EQUAL(strlen((char *)resp_buf), resp_len);
    TEST_ASSERT_EQUAL_STRING(":84 Changed.", resp_buf);
    TEST_ASSERT_EQUAL(pub_channels[0].enabled, true);
}

void json_get_endpoint_node()
{
    const DataNode *node;

    node = ts.get_endpoint_node("conf", strlen("conf"));
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL(node->id, TS_CONF);

    node = ts.get_endpoint_node("conf/", strlen("conf/"));
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL(node->id, TS_CONF);
}
