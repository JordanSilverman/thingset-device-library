/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2017 Martin Jäger / Libre Solar
 */

#ifndef TEST_DATA_H
#define TEST_DATA_H

#include <stdint.h>
#include <stdbool.h>
#include "thingset.h"

// info
char manufacturer[] = "Libre Solar";
static uint32_t timestamp = 12345678;

// conf
static float bat_charging_voltage = 14.4;
static float load_disconnect_voltage = 10.8;

// input
static bool enable_switch = false;

// output
static float battery_voltage = 14.1;
static float battery_current = 5.13;
static int16_t ambient_temp = 22;

// rec
static float bat_energy_hour = 32.2;
static float bat_energy_day = 123;
static int16_t ambient_temp_max_day = 28;

// pub
bool pub_serial_enable = false;
uint16_t pub_serial_interval = 1000;
node_id_t pub_serial_ids[20] = { 0x1A, 0x71, 0x72, 0x73 };
ArrayInfo pub_serial_array = {
    pub_serial_ids, sizeof(pub_serial_ids)/sizeof(node_id_t), TS_AUTODETECT_ARRLEN, TS_T_NODE_ID
};

bool pub_can_enable = true;
uint16_t pub_can_interval = 100;
node_id_t pub_can_ids[20] = { 0x71, 0x72, 0x73 };
ArrayInfo pub_can_array = {
    pub_can_ids, sizeof(pub_can_ids)/sizeof(node_id_t), TS_AUTODETECT_ARRLEN, TS_T_NODE_ID
};

// exec
void reset_function(void);
void auth_function(void);
char auth_password[11];


char strbuf[300];

float f32;

static uint64_t ui64;
static int64_t i64;

static uint32_t ui32;
int32_t i32;

static uint16_t ui16;
static int16_t i16;

bool b;

static uint32_t secret_user = 1;
static uint32_t secret_maker = 2;

int32_t A[100] = {4, 2, 8, 4};
ArrayInfo int32_array = {A, sizeof(A)/sizeof(int32_t), 4, TS_T_INT32};

float B[100] = {2.27, 3.44};
ArrayInfo float32_array = {B, sizeof(B)/sizeof(float), 2, TS_T_FLOAT32};

void dummy(void);
void conf_callback(void);

static const DataNode data_nodes[] = {

    // DEVICE INFORMATION /////////////////////////////////////////////////////
    // using IDs >= 0x18

    TS_NODE_PATH(TS_INFO, "info", 0, NULL),

    TS_NODE_STRING(0x19, "Manufacturer", manufacturer, 0, TS_INFO, TS_ANY_R),
    TS_NODE_UINT32(0x1A, "Timestamp_s", &timestamp, TS_INFO, TS_ANY_RW),
    TS_NODE_STRING(0x1B, "DeviceID", strbuf, sizeof(strbuf), TS_INFO, TS_ANY_R | TS_MKR_W),

    // CONFIGURATION //////////////////////////////////////////////////////////
    // using IDs >= 0x30 except for high priority data objects

    TS_NODE_PATH(TS_CONF, "conf", 0, &conf_callback),

    TS_NODE_FLOAT(0x31, "BatCharging_V", &bat_charging_voltage, 2, TS_CONF, TS_ANY_RW),
    TS_NODE_FLOAT(0x32, "LoadDisconnect_V", &load_disconnect_voltage, 2, TS_CONF, TS_ANY_RW),

    // INPUT DATA /////////////////////////////////////////////////////////////
    // using IDs >= 0x60

    TS_NODE_PATH(TS_INPUT, "input", 0, NULL),

    TS_NODE_BOOL(0x61, "EnableCharging", &enable_switch, TS_INPUT, TS_ANY_RW),

    // OUTPUT DATA ////////////////////////////////////////////////////////////
    // using IDs >= 0x70 except for high priority data objects

    TS_NODE_PATH(TS_OUTPUT, "output", 0, NULL),

    TS_NODE_FLOAT(0x71, "Bat_V", &battery_voltage, 2, TS_OUTPUT, TS_ANY_R),
    TS_NODE_FLOAT(0x72, "Bat_A", &battery_current, 2, TS_OUTPUT, TS_ANY_R),
    TS_NODE_INT16(0x73, "Ambient_degC", &ambient_temp, TS_OUTPUT, TS_ANY_R),

    // RECORDED DATA ///////////////////////////////////////////////////////
    // using IDs >= 0xA0

    TS_NODE_PATH(TS_REC, "rec", 0, NULL),

    TS_NODE_FLOAT(0xA1, "BatHour_kWh", &battery_voltage, 2, TS_REC, TS_ANY_R),
    TS_NODE_FLOAT(0xA2, "BatDay_kWh", &battery_current, 2, TS_REC, TS_ANY_R),
    TS_NODE_INT16(0xA3, "AmbientMaxDay_degC", &ambient_temp, TS_REC, TS_ANY_R),

    // CALIBRATION DATA ///////////////////////////////////////////////////////
    // using IDs >= 0xD0

    TS_NODE_PATH(TS_CAL, "cal", 0, NULL),

    // FUNCTION CALLS (EXEC) //////////////////////////////////////////////////
    // using IDs >= 0xE0

    TS_NODE_PATH(TS_EXEC, "exec", 0, NULL),

    TS_NODE_EXEC(0xE1, "reset", &reset_function, TS_EXEC, TS_ANY_RW),

    TS_NODE_EXEC(0xE2, "auth", &auth_function, 0, TS_ANY_RW),
    TS_NODE_STRING(0xE3, "Password", auth_password, sizeof(auth_password), 0xE2, TS_ANY_RW),

    // PUBLICATION DATA ///////////////////////////////////////////////////////
    // using IDs >= 0xF0

    TS_NODE_PATH(TS_PUB, "pub", 0, NULL),

    TS_NODE_PATH(0xF1, "serial", TS_PUB, NULL),
    TS_NODE_BOOL(0xF2, "Enable", &pub_serial_enable, 0xF1, TS_ANY_RW),
    TS_NODE_UINT16(0xF3, "Interval_ms", &pub_serial_interval, 0xF1, TS_ANY_RW),
    TS_NODE_ARRAY(0xF4, "IDs", &pub_serial_array, 0, 0xF1, TS_ANY_RW),

    TS_NODE_PATH(0xF5, "can", TS_PUB, NULL),
    TS_NODE_BOOL(0xF6, "Enable", &pub_can_enable, 0xF5, TS_ANY_RW),
    TS_NODE_UINT16(0xF7, "Interval_ms", &pub_can_interval, 0xF5, TS_ANY_RW),
    TS_NODE_ARRAY(0xF8, "IDs", &pub_can_array, 0, 0xF5, TS_ANY_RW),

    // LOGGING DATA ///////////////////////////////////////////////////////
    // using IDs >= 0x100

    TS_NODE_PATH(0x100, "log", 0, NULL),

    TS_NODE_PATH(0x110, "hourly", 0x100, NULL),
    /*
        "hourly": {
            "0h": {"Timestamp_s":23094834,"BatHour_kWh":123},
            "-1h": {"Timestamp_s":23094834,"BatHour_kWh":123}
        }
    */

    TS_NODE_PATH(0x130, "daily", 0x100, NULL),
    /*
        "daily": {
            "0d": {"Timestamp_s":23094834,"BatDay_kWh":123,"AmbientMaxDay_degC":26},
            "-1d": {"Timestamp_s":34209348,"BatDay_kWh":151,"AmbientMaxDay_degC":28}
        },
    */

    // UNIT TEST DATA ///////////////////////////////////////////////////////
    // using IDs >= 0x1000

    TS_NODE_PATH(0x1000, "test", 0, NULL),

    TS_NODE_INT32(0x4001, "i32_readonly", &i32, 0x1000, TS_ANY_R),

    TS_NODE_EXEC(0x5001, "dummy", &dummy, TS_EXEC, TS_ANY_RW),

    TS_NODE_UINT64(0x6001, "ui64", &ui64, TS_CONF, TS_ANY_RW),
    TS_NODE_INT64(0x6002, "i64", &i64, TS_CONF, TS_ANY_RW),
    TS_NODE_UINT32(0x6003, "ui32", &ui32, TS_CONF, TS_ANY_RW),
    TS_NODE_INT32(0x6004, "i32", &i32, TS_CONF, TS_ANY_RW),
    TS_NODE_UINT16(0x6005, "ui16", &ui16, TS_CONF, TS_ANY_RW),
    TS_NODE_INT16(0x6006, "i16", &i16, TS_CONF, TS_ANY_RW),
    TS_NODE_FLOAT(0x6007, "f32", &f32, 2, TS_CONF, TS_ANY_RW),
    TS_NODE_BOOL(0x6008, "bool", &b, TS_CONF, TS_ANY_RW),

    TS_NODE_STRING(0x6009, "strbuf", strbuf, sizeof(strbuf), TS_CONF, TS_ANY_RW),

    TS_NODE_FLOAT(0x600A, "f32_rounded", &f32, 0, TS_CONF, TS_ANY_RW),

    TS_NODE_UINT32(0x7001, "secret_expert", &ui32, TS_CONF, TS_ANY_R | TS_EXP_W | TS_MKR_W),
    TS_NODE_UINT32(0x7002, "secret_maker", &ui32, TS_CONF, TS_ANY_R | TS_MKR_W),
    TS_NODE_ARRAY(0x7003, "arrayi32", &int32_array, 0, TS_CONF, TS_ANY_RW),
    // data_node->detail will specify the number of decimal places for float
    TS_NODE_ARRAY(0x7004, "arrayfloat", &float32_array, 2, TS_CONF, TS_ANY_RW),
};

#endif
