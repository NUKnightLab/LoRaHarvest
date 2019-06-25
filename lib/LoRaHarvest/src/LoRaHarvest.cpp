#include <LoRaHarvest.h>
#include <console.h>

int Thing1::add(int a, int b)
{
    println("Adding: %d + %d", a, b);
    return a + b;
}

int Thing2::subtract(int a, int b)
{
    println("Subtracting: %d - %d", a, b);
    return a - b;
}

/**
 * Enforce a particular topology. Used in testing for simulation of signal reach under
 * close-proximity (e.g. benchtop) conditions.
 */
bool topologyTest(int config, int to, int from)
{
    switch(config) {
        case TEST_TOPOLOGY_NONE:
            return true;
        case TEST_TOPOLOGY_1:
            if (to == 1 && from != 2) return false;
            if (to == 2 && !( from == 1 || from == 3) ) return false;
            if (to == 3 && from != 2) return false;
            break;
    }
    return true;
}

void standby(uint32_t timeout)
{

}

void sendNextDataPacket(int seq, uint8_t *reversedRoute)
{
    #ifdef ARDUINO
    uint8_t message[3] = { 1, 2, 3 }; // TODO: get the real data
    LoRa.idle();
    LoRa.beginPacket();
    LoRa.write(reversedRoute[sizeof(reversedRoute)-2]);
    LoRa.write(NODE_ID);
    LoRa.write(reversedRoute[0]);
    LoRa.write(seq);
    LoRa.write(PACKET_TYPE_DATA);
    for (size_t i=sizeof(reversedRoute)-1; i>=0; i--) {
        LoRa.write(reversedRoute[i]);
    }
    LoRa.write(0);
    LoRa.write(message, sizeof(message));
    LoRa.endPacket();
    LoRa.receive();
    #endif
}

void handleDataMessage(uint8_t *message)
{

}

void routeMessage(int dest, int seq, int packetType, uint8_t *route, uint8_t *message)
{
    #ifdef ARDUINO
    LoRa.idle();
    LoRa.beginPacket();
    int replyTo = 0;
    for (size_t i=0; i<sizeof(route)-1; i++) {
        if (route[i] == NODE_ID) {
            replyTo = route[i+1];
        }
    }
    if (replyTo == 0) {
        println("ERROR: NO ROUTE!");
    }
    LoRa.write(replyTo);                  // to
    LoRa.write(NODE_ID);                  // from
    LoRa.write(dest);                     // dest
    LoRa.write(seq);                      // seq
    LoRa.write(packetType);               // type
    LoRa.write(route, sizeof(route));     // route
    LoRa.write(0);                        // end-route
    LoRa.write(message, sizeof(message)); // message
    LoRa.endPacket();
    LoRa.receive();
    #endif
}

int handlePacket(int to, int from, int dest, int seq, int packetType, uint8_t *route, uint8_t *message, int topology)
{
    static int last_seq = 0;
    if (seq == last_seq) {
        return MESSAGE_CODE_DUPLICATE_SEQUENCE;
    }
    if (to != NODE_ID && to != 255) return MESSAGE_CODE_WRONG_ADDRESS;
    if (!topologyTest(topology, to, from)) return MESSAGE_CODE_TOPOLOGY_FAIL;
    last_seq = seq;
    if (dest == NODE_ID) {
        switch (packetType) {
            case PACKET_TYPE_SENDDATA:
                sendNextDataPacket(++last_seq, route); // TODO: get packet # request from message
                return MESSAGE_CODE_SENT_NEXT_DATA_PACKET;
            case PACKET_TYPE_DATA:
                handleDataMessage(message);
                return MESSAGE_CODE_RECEIVED_DATA_PACKET;
            case PACKET_TYPE_STANDBY: // in practice, this code will be broadcast, not addressed
                standby(0); // TODO: get the timeout from the message
                return MESSAGE_CODE_STANDBY;
        }
    } else if (dest == 255) { // broadcast message
        switch (packetType) {
            case PACKET_TYPE_STANDBY:
                standby(0); // TODO: get the timeout from the message
                return MESSAGE_CODE_STANDBY;
        }
    } else { // route this message
        routeMessage(dest, last_seq, packetType, route, message);
        return MESSAGE_CODE_ROUTED;
    }
    return MESSAGE_CODE_NONE;
}

#ifdef ARDUINO


void onReceive(int packetSize)
{
    static uint8_t route_buffer[10];
    static uint8_t msg_buffer[255];
    int to = LoRa.read();
    int from = LoRa.read();
    int dest = LoRa.read();
    int seq = LoRa.read();
    int type = LoRa.read();
    int idx_ = 0;
    while (LoRa.available()) {
       route_buffer[idx_++] = LoRa.read();
       if (route_buffer[idx_-1] == 0) break;
    }
    uint8_t route[idx_];
    memcpy(route, route_buffer, idx_*sizeof(uint8_t));
    idx_ = 0;
    while (LoRa.available()) {
        msg_buffer[idx_++] = LoRa.read();
    }
    uint8_t msg[idx_];
    memcpy(msg, msg_buffer, idx_*sizeof(uint8_t));
    handlePacket(to, from, dest, seq, type, route, msg);
}

void setupLoRa(int csPin, int resetPin, int irqPin)
{
    LoRa.setPins(csPin, resetPin, irqPin);
    if (!LoRa.begin(frequency)) {
        println("LoRa init failed.");
        while(true);
    }
    LoRa.enableCrc();
    LoRa.onReceive(onReceive);
}
#endif