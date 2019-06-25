#include <unity.h>
#include <LoRaHarvest.h>
#include <console.h>

#ifdef ARDUINO

namespace Test_EmbeddedThings {

    static int sequence = 0;

    void test_test(void) {
        TEST_ASSERT_TRUE(true);
    }

    void test_handlePacket_SENDDATA(void) {
        int to = NODE_ID;
        int from = 0;
        int dest = NODE_ID;
        int type = PACKET_TYPE_SENDDATA;
        // routes here are built as would come from a remote node
        uint8_t route[2] = { (uint8_t)(NODE_ID+1), (uint8_t)NODE_ID };
        println("first check size of route: %d", sizeof(route));
        uint8_t *message;
        println("NEXT DATA PACKET");
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(to, from, dest, ++++sequence, type, route, 2, message));
        println("DUPLICATE SEQUENCE");
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_DUPLICATE_SEQUENCE,
            handlePacket(to, from, dest, ++sequence, type, route, 2, message));
        println("WRONG ADDRESS");
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_WRONG_ADDRESS,
            handlePacket(to+1, from, dest, ++sequence, type, route, 2, message));
        println("NEXT DATA PACKET");
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(255, from, dest, ++++sequence, type, route, 2, message));
        println("CODE NONE");
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_NONE,
            handlePacket(255, from, 255, ++++sequence, type, route, 2, message));
        uint8_t route_multi[3] = { (uint8_t)(NODE_ID+2), (uint8_t)(NODE_ID+1), (uint8_t)(NODE_ID) };
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(255, from, dest, ++++sequence, type, route_multi, 3, message));
    }

    void test_handlePacket_ROUTE(void) {
        int to = NODE_ID;
        int from = NODE_ID+1;
        int dest = NODE_ID+2;
        int type = PACKET_TYPE_SENDDATA;
        uint8_t route[3] = { from, to, dest };
        uint8_t *message;
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_ROUTED,
            handlePacket(to, from, dest, ++++sequence, type, route, 3, message));
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_test);
        RUN_TEST(test_handlePacket_SENDDATA);
        RUN_TEST(test_handlePacket_ROUTE);
        UNITY_END();
    }
}

#endif