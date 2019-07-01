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
        uint8_t to = NODE_ID;
        int from = 0;
        int dest = NODE_ID;
        int type = PACKET_TYPE_SENDDATA;
        // routes here are built as would come from a remote node. ie, the current node id will be last
        uint8_t route[2] = { NODE_ID+1, NODE_ID };
        uint8_t *message;
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(to, from, dest, ++++sequence, type, route, 2, message, 0));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_DUPLICATE_SEQUENCE,
            handlePacket(to, from, dest, ++sequence, type, route, 2, message, 0));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_WRONG_ADDRESS,
            handlePacket(to+1, from, dest, ++sequence, type, route, 2, message, 0));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(255, from, dest, ++++sequence, type, route, 2, message, 0));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_NONE,
            handlePacket(255, from, 255, ++++sequence, type, route, 2, message, 0));
        uint8_t route_multi[3] = { NODE_ID+2, NODE_ID+1, NODE_ID };
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(255, from, dest, ++++sequence, type, route_multi, 3, message, 0));
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
            handlePacket(to, from, dest, ++++sequence, type, route, 3, message, 0));
    }

    void test_ECHO(void) {
        int to = 2;
        int from = 1;
        int dest = 2;
        int type = PACKET_TYPE_ECHO;
        uint8_t route[2] = { 1, 2};
        uint8_t msg_size = 255;
        uint8_t message[msg_size];
        for (uint8_t i=0; i<msg_size; i++) {
            message[i] = i;
        }
        routeMessage(dest, ++++sequence, type, route, sizeof(route), message, sizeof(message));
        for (int i=0; i<30; i++){
            delay(100);
        }
    }

    void test_all() {
        UNITY_BEGIN();
        //RUN_TEST(test_test);
        //RUN_TEST(test_handlePacket_SENDDATA);
        //RUN_TEST(test_handlePacket_ROUTE);
        RUN_TEST(test_ECHO);
        UNITY_END();
    }
}

#endif