#include <unity.h>
#include <DataManager.h>
#include "test_datamanager_common.h"


namespace Test_DataManagerCommon {

    void test_recordData() {
        clearData();
        recordData(1);
        TEST_ASSERT_EQUAL(1, dataIndex());
        char data[] = "{\"test\":1.0}";
        recordData(data, strlen(data));
        //char *bad_data = "123";
        //recordData(bad_data, sizeof(bad_data));
        char *recorded_data = getBatch(0);
        TEST_ASSERT_EQUAL(strlen(recorded_data), strlen(data));
        TEST_ASSERT_EQUAL_STRING_LEN(data, recorded_data, strlen(data));
        TEST_ASSERT_EQUAL_STRING_LEN("{\"test\":1.0}", recorded_data, 12);
    }

    void test_numBatches() {
        uint8_t batch_size = 10;
        uint8_t *batch;
        clearData();
        /*
        TEST_ASSERT_EQUAL(0, numBatches(batch_size));
        batch = getBatch(0, &batch_size);
        TEST_ASSERT_EQUAL(0, batch_size);
        batch_size = 10;
        for (uint8_t i=0; i<10; i++) recordData(i);
        TEST_ASSERT_EQUAL(1, numBatches(batch_size));
        recordData(0);
        TEST_ASSERT_EQUAL(2, numBatches(batch_size));
        batch = getBatch(0, &batch_size);
        TEST_ASSERT_EQUAL(10, batch_size);
        batch = getBatch(1, &batch_size);
        TEST_ASSERT_EQUAL(1, batch_size);
        batch_size = 10;
        for (uint8_t i=1; i<10; i++) recordData(i);
        TEST_ASSERT_EQUAL(2, numBatches(batch_size));
        batch = getBatch(1, &batch_size);
        TEST_ASSERT_EQUAL(10, batch_size);
        for (uint8_t i=0; i<10; i++) {
            TEST_ASSERT_EQUAL(i, batch[i]);
        }
        batch = getBatch(0, &batch_size);
        for (uint8_t i=0; i<10; i++) {
            TEST_ASSERT_EQUAL(i, batch[i]);
        }
        */
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_recordData);
        RUN_TEST(test_numBatches);
        UNITY_END();
    }
}