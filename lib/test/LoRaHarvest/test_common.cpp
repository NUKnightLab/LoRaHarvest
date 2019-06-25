#include <unity.h>
#include <LoRaHarvest.h>
#include <stdio.h>
#include "test_common.h"
#include <console.h>

Thing1 thing1;
Thing2 thing2;

/*
TestLoRa::TestLoRa() {}

int TestLoRa::read() {
    return _buffer[_index++];
}
*/


namespace Test_CommonThings {

    static int sequence = 0;

    void test_add(void) {
        TEST_ASSERT_EQUAL(32, thing1.add(25, 7));
    }

    void test_subtract(void) {
        TEST_ASSERT_EQUAL(18, thing2.subtract(25, 7));
    }

    void test_topologyTest(void) {
        TEST_ASSERT_TRUE(topologyTest(0, 1, 2));
        TEST_ASSERT_TRUE(topologyTest(0, 1, 3));
        TEST_ASSERT_TRUE(topologyTest(0, 2, 1));
        TEST_ASSERT_TRUE(topologyTest(0, 2, 3));
        TEST_ASSERT_TRUE(topologyTest(0, 3, 1));
        TEST_ASSERT_TRUE(topologyTest(0, 3, 2));
        TEST_ASSERT_TRUE(topologyTest(1, 1, 2));
        TEST_ASSERT_FALSE(topologyTest(1, 1, 3));
        TEST_ASSERT_TRUE(topologyTest(1, 2, 1));
        TEST_ASSERT_TRUE(topologyTest(1, 2, 3));
        TEST_ASSERT_FALSE(topologyTest(1, 3, 1));
        TEST_ASSERT_TRUE(topologyTest(1, 3, 2));
    }


    void test_handlePacket_STANDBY(void) {
        int to = NODE_ID;
        int from = 0;
        int dest = NODE_ID;
        int type = PACKET_TYPE_STANDBY;
        uint8_t *route;
        uint8_t *message;
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_STANDBY,
            handlePacket(to, from, dest, ++sequence, type, route, 0, message));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_STANDBY,
            handlePacket(255, from, 255, ++sequence, type, route, 0, message));
    }

    /**
     * We put the UNITY_BEGIN and UNITY_END calls here in the submodules rather than a single set
     * of calls in the main runner because the location of these determines the filename that
     * is output during testing. Doing this here gives us the more specific output of the filename
     * where the tests actually reside.
     */
    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_add);
        RUN_TEST(test_subtract);
        RUN_TEST(test_topologyTest);
        RUN_TEST(test_handlePacket_STANDBY);
        UNITY_END();
    }
}