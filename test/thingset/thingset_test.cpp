

#include "thingset.h"
//#include "cbor.h"
#include "unity.h"
//#include <sys/types.h>  // for definition of endianness

#include "test_data.h"

measurement_data_t meas;
calibration_data_t cal;

uint8_t req_buf[TS_REQ_BUFFER_LEN];
uint8_t resp_buf[TS_RESP_BUFFER_LEN];
//ts_buffer_t req, resp;

#include "tests_json.h"
#include "tests_cbor.h"
#include "tests_common.h"

ThingSet ts(dataObjects, sizeof(dataObjects)/sizeof(data_object_t));

int main()
{
    //test_data_init();
    ts.set_pub_channels(pub_channels, sizeof(pub_channels));

    UNITY_BEGIN();

    // data conversion tests
    RUN_TEST(write_json_read_cbor);
    RUN_TEST(write_cbor_read_json);

    // JSON only tests
    RUN_TEST(json_wrong_command);
    RUN_TEST(json_write_wrong_data_structure);
    RUN_TEST(json_write_array);
    RUN_TEST(json_write_readonly);
    RUN_TEST(json_write_unknown);
    RUN_TEST(json_wrong_category);
    RUN_TEST(json_read_array);
    RUN_TEST(json_list_input);
    RUN_TEST(json_list_names_values_input);
    RUN_TEST(json_exec);
    RUN_TEST(json_pub_msg);
    RUN_TEST(json_conf_callback);

    // CBOR only tests
    RUN_TEST(cbor_write_array);
    RUN_TEST(cbor_read_array);
    RUN_TEST(cbor_list_ids_input);
    RUN_TEST(cbor_list_names_input);
    RUN_TEST(cbor_list_names_values_input);
    RUN_TEST(cbor_pub_msg);
    RUN_TEST(cbor_exec);

    UNITY_END();
}

bool dummy_called_flag = 0;

void dummy(void)
{
    dummy_called_flag = 1;
}
