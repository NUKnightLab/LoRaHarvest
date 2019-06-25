#include <unity.h>
#include <LoRaHarvest.h>

#ifdef ARDUINO

namespace Test_EmbeddedThings {

    static int sequence = 0;

    void test_test(void) {
        TEST_ASSERT_TRUE(true);
    }

    /**
     * arduino-LoRa does not really expose any public methods we can test against at this point.
     * For now, just run the setup.
     */ 
    void test_LoRaSetup(void) {
        setupLoRa(LORA_CS, LORA_RST, LORA_IRQ);
    }

    void test_handlePacket_SENDDATA(void) {
        int to = NODE_ID;
        int from = 0;
        int dest = NODE_ID;
        int type = PACKET_TYPE_SENDDATA;
        uint8_t *route;
        uint8_t *message;
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(to, from, dest, ++++sequence, type, route, message));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_DUPLICATE_SEQUENCE,
            handlePacket(to, from, dest, ++sequence, type, route, message));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_WRONG_ADDRESS,
            handlePacket(to+1, from, dest, ++sequence, type, route, message));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_SENT_NEXT_DATA_PACKET,
            handlePacket(255, from, dest, ++++sequence, type, route, message));
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_NONE,
            handlePacket(255, from, 255, ++++sequence, type, route, message));
    }

    void test_handlePacket_ROUTE(void) {
        int to = NODE_ID;
        int from = NODE_ID+1;
        int dest = NODE_ID+2;
        int type = PACKET_TYPE_SENDDATA;
        uint8_t route[10] = { from, to, dest };
        uint8_t *message;
        TEST_ASSERT_EQUAL(
            MESSAGE_CODE_ROUTED,
            handlePacket(to, from, dest, ++sequence, type, route, message));
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_test);
        RUN_TEST(test_LoRaSetup);
        // TODO: why is this hanging?
        //RUN_TEST(test_handlePacket_SENDDATA);
        RUN_TEST(test_handlePacket_ROUTE);
        UNITY_END();
    }
}

#endif