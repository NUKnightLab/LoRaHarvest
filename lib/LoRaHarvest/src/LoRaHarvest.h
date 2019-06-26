#ifndef LORAHARVEST_H
#define LORAHARVEST_H

#include <stdint.h>
#include <stddef.h>

#ifdef ARDUINO
#include <LoRa.h>
#endif

class Thing1 {
public:
    Thing1() {}
    int add (int a, int b);
};

class Thing2 {
public:
    Thing2() {}
    int subtract (int a, int b);
};

const long frequency = 915E6; // Currently only supporting 915 MHz
const int LORA_CS = 8; // chip select
const int LORA_RST = 4; // reset
const int LORA_IRQ = 3; // interrupt

const int TEST_TOPOLOGY_NONE = 0;
const int TEST_TOPOLOGY_1 = 1; // 1 - 2 - 3

const int PACKET_TYPE_SENDDATA = 1;
const int PACKET_TYPE_DATA = 2;
const int PACKET_TYPE_STANDBY = 3;

const int MESSAGE_CODE_NONE = 0;
const int MESSAGE_CODE_DUPLICATE_SEQUENCE = 1;
const int MESSAGE_CODE_WRONG_ADDRESS = 2;
const int MESSAGE_CODE_TOPOLOGY_FAIL = 3;
const int MESSAGE_CODE_SENT_NEXT_DATA_PACKET = 4;
const int MESSAGE_CODE_RECEIVED_DATA_PACKET = 5;
const int MESSAGE_CODE_STANDBY = 6;
const int MESSAGE_CODE_ROUTED = 7;

const int NODE_ID = 1;

/* collecting state management */
bool collectingData();
void collectingData(bool val);
bool waitingPacket();
void waitingPacket(bool val);
int collectingNodeIndex();
void collectingNodeIndex(int idx);
uint8_t collectingPacketId();
void collectingPacketId(uint8_t val);


void onReceive(int packetSize);
void setupLoRa(int csPin, int resetPin, int irqPin);
void standby(uint32_t timeout);
void sendDataPacket(uint8_t packet_id, int seq, uint8_t *reversedRoute);
void handleDataMessage(uint8_t from_node, uint8_t *message, size_t msg_size);
bool topologyTest(int config, int to, int from);
int handlePacket(int to, int from, int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology=0);
void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size);

#endif


