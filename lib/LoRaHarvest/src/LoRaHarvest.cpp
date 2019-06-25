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

#define VBATPIN 9

float batteryLevel()
{
    float val = 0.0;
    #ifdef ARDUINO
    val = analogRead(VBATPIN);
    val *= 2;    // we divided by 2, so multiply back
    val *= 3.3;  // Multiply by 3.3V, our reference voltage
    val /= 1024; // convert to voltage
    #endif
    return val;
}

void sendNextDataPacket(int seq, uint8_t *reversedRoute, size_t route_size)
{
    #ifdef ARDUINO
    println("SEND NEXT DATA PACKET");
    print("REV rOUTE:");
    for (uint8_t i=0; i<route_size; i++) print(" %d", reversedRoute[i]);
    println("");
    uint8_t bat = (uint8_t)nearbyintf(batteryLevel() * 10);
    uint8_t message[1] = { bat }; // TODO: get the real data
    LoRa.idle();
    LoRa.beginPacket();
    println("writing TO: %d", reversedRoute[route_size-2]);
    LoRa.write(reversedRoute[route_size-2]);
    println("writing FROM: %d", NODE_ID);
    LoRa.write(NODE_ID);
    println("writing DEST: %d", reversedRoute[0]);
    LoRa.write(reversedRoute[0]);
    println("writing SEQ: %d", seq);
    LoRa.write(seq);
    println("writing TYPE: %d", PACKET_TYPE_DATA);
    LoRa.write(PACKET_TYPE_DATA);
    println("SIZEOF REVERSED ROUTE: %d", route_size);
    for (size_t i=route_size; i-- > 0;) {
        println("ROUTE: %d", reversedRoute[i]);
        LoRa.write(reversedRoute[i]);
    }
    println("ZERO");
    LoRa.write(0);
    println("Message");
    LoRa.write(message, sizeof(message));
    LoRa.endPacket();
    println(".. DONE");
    LoRa.receive();
    println("RECEIVING");
    #endif
}

void handleDataMessage(uint8_t from_node, uint8_t *message, size_t msg_size)
{
    println("NODE: %d; BAT: %f", from_node, message[0] / 10);
}

void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size)
{
    #ifdef ARDUINO
    LoRa.idle();
    LoRa.beginPacket();
    int replyTo = 0;
    for (size_t i=0; i<route_size; i++) {
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
    LoRa.write(route, route_size);     // route
    LoRa.write(0);                        // end-route
    LoRa.write(message, msg_size); // message
    LoRa.endPacket();
    LoRa.receive();
    #endif
}

int handlePacket(int to, int from, int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology)
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
                sendNextDataPacket(++last_seq, route, route_size); // TODO: get packet # request from message
                return MESSAGE_CODE_SENT_NEXT_DATA_PACKET;
            case PACKET_TYPE_DATA:
                handleDataMessage(route[0], message, msg_size);
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
        routeMessage(dest, last_seq, packetType, route, route_size, message, msg_size);
        return MESSAGE_CODE_ROUTED;
    }
    return MESSAGE_CODE_NONE;
}

#ifdef ARDUINO


void onReceive(int packetSize)
{
    println("RECEIVED A PACKET");

    static uint8_t route_buffer[10];
    static uint8_t msg_buffer[255];
    int to = LoRa.read();
    int from = LoRa.read();
    int dest = LoRa.read();
    int seq = LoRa.read();
    int type = LoRa.read();
    size_t route_idx_ = 0;
    size_t msg_idx_ = 0;
    println("TO: %d; FROM: %d; DEST: %d; SEQ: %d; TYPE: %d", to, from, dest, seq, type);
    print("ROUTE:");
    while (LoRa.available()) {
        uint8_t node = LoRa.read();
        if (node == 0) break;
        route_buffer[route_idx_++] = node;
        print(" %d", route_buffer[route_idx_-1]);
    }
    println("");
    //uint8_t route[idx_];
    //memcpy(route, route_buffer, idx_*sizeof(uint8_t));
    while (LoRa.available()) {
        msg_buffer[msg_idx_++] = LoRa.read();
    }
    println("HANDLE PACKET");
    //uint8_t msg[idx_];
    //memcpy(msg, msg_buffer, idx_*sizeof(uint8_t));
    handlePacket(to, from, dest, seq, type, route_buffer, route_idx_, msg_buffer, msg_idx_);
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