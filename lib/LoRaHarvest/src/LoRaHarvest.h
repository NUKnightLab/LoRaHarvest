#ifndef LORAHARVEST_H
#define LORAHARVEST_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef ARDUINO
#include <LoRa.h>
#include <RTCZero.h>
extern RTCZero rtcz;
#endif


extern uint8_t nodes[2];
extern uint8_t routes[255][6];

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
const int PACKET_TYPE_ECHO = 4; // used for testing

const int MESSAGE_CODE_NONE = 0;
const int MESSAGE_CODE_DUPLICATE_SEQUENCE = 1;
const int MESSAGE_CODE_WRONG_ADDRESS = 2;
const int MESSAGE_CODE_TOPOLOGY_FAIL = 3;
const int MESSAGE_CODE_SENT_NEXT_DATA_PACKET = 4;
const int MESSAGE_CODE_RECEIVED_DATA_PACKET = 5;
const int MESSAGE_CODE_STANDBY = 6;
const int MESSAGE_CODE_ROUTED = 7;

//const int NODE_ID = 1;
const int SYNC_WORD = 0x19; // default 0x12. 0x34 is LoRaWAN
const int MAX_PACKET_SIZE = 255;
const int MAX_ROUTE_SIZE = 10;
//const int MAX_MESSAGE_SIZE = MAX_PACKET_SIZE - 6 - MAX_ROUTE_SIZE;
const int MAX_MESSAGE_SIZE = 100;


void nodeId(uint8_t id);
uint8_t nodeId();

void readyToPost(uint8_t node_id);
uint8_t readyToPost();

void isCollector(bool _is);
bool isCollector();

/* collecting state management */
bool collectingData();
void collectingData(bool val);
bool waitingPacket();
void waitingPacket(bool val);
int collectingNodeIndex();
void collectingNodeIndex(int idx);
uint8_t collectingPacketId();
void collectingPacketId(uint8_t val);

void recordBattery();

void writeTimestamp();
void onReceive(int packetSize);
void setupLoRa(int csPin, int resetPin, int irqPin);
void standby(uint32_t timeout);
void sendDataPacket(uint8_t packet_id, int seq, uint8_t *reversedRoute, size_t route_size);
void sendStandby(uint8_t seq, uint32_t next_collection);
void handleDataMessage(uint8_t from_node, uint8_t *message, size_t msg_size);
void collector_handleDataMessage(uint8_t from_node, uint8_t seq, uint8_t *message, size_t msg_size);
int collector_handlePacket(int to, int from, int dest, int seq, int packetType, uint32_t timestamp, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology);
void collector_onReceive(int packetSize);
void sendCollectPacket(uint8_t node_id, uint8_t packet_id, uint8_t seq);
bool topologyTest(int config, int to, int from);
int handlePacket(int to, int from, int dest, int seq, int packetType, uint32_t timestamp, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology=0);
void handleEchoMessage(uint8_t seq, uint8_t *reversedRoute, uint8_t route_size, uint8_t *message, uint8_t msg_size);
void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size);

#endif


