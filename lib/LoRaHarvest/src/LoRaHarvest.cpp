#include <LoRaHarvest.h>
#include <console.h>
#include <math.h>
#include <DataManager.h>

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

bool _collecting = false;
bool _waiting_packet = false;
int _collecting_node_idx = -1;
uint8_t _collecting_packet = 1;

bool collectingData()
{
    return _collecting;
}

void collectingData(bool val)
{
    _collecting = val;
}

bool waitingPacket()
{
    return _waiting_packet;
}

void waitingPacket(bool val)
{
    _waiting_packet = val;
}

int collectingNodeIndex()
{
    return _collecting_node_idx;
}

void collectingNodeIndex(int idx)
{
    _collecting_node_idx = idx;
}

uint8_t collectingPacketId()
{
    return _collecting_packet;
}

void collectingPacketId(uint8_t val)
{
    _collecting_packet = val;
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
    println("Measured BAT: %f", val);
    return val;
}

uint32_t timestamp()
{
    #ifdef ARDUINO
    return millis();
    #endif
    return 0;
}

//uint8_t battery_samples[255];
//uint32_t battery_timestamps[255];
//uint8_t battery_data_head = 0;
//uint8_t battery_data_index = 0;

void recordBattery()
{
    recordData((uint8_t)nearbyintf(batteryLevel() * 10));
    //battery_samples[battery_data_index] = (uint8_t)nearbyintf(batteryLevel() * 10);
    //battery_timestamps[battery_data_index] = timestamp();
    //battery_data_index++;
}

void sendDataPacket(uint8_t packet_id, int seq, uint8_t *reversedRoute, size_t route_size)
{
    #ifdef ARDUINO
    if (packet_id == 0) packet_id = numBatches(10);
    println("SEND DATA PACKET: %d", packet_id);
    print("REV rOUTE:");
    for (uint8_t i=0; i<route_size; i++) print(" %d", reversedRoute[i]);
    println("");
    //uint8_t bat = (uint8_t)nearbyintf(batteryLevel() * 10);
    //uint8_t message[2] = { packet_id, bat }; // TODO: get the real data
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
    //LoRa.write(message, sizeof(message));
    LoRa.write(packet_id);
    uint8_t msg_size = 0;
    //for (;battery_data_head<battery_data_index && msg_size<10; msg_size++) {
    //    LoRa.write(battery_samples[battery_data_head++]);
    //}
    println("RECEIVED REQUEST FOR PACKET: %d", packet_id);
    uint8_t batch_size = 10;
    uint8_t *batch = getBatch(packet_id - 1, &batch_size);
    println("PACKET Batch size is %d", batch_size);
    for (uint8_t i=0; i< batch_size; i++) {
        LoRa.write(batch[i]);
    }
    //if (battery_data_head >= battery_data_index) {
    //    battery_data_head = 0;
    //    battery_data_index = 0;
    //}
    if (packet_id == 1) clearData(); // TODO: tunneled ack from collector before clearing
    LoRa.endPacket();
    println(".. DONE");
    LoRa.receive();
    println("RECEIVING");
    #endif
}

void handleDataMessage(uint8_t from_node, uint8_t *message, size_t msg_size)
{
    /**
     * If we asked for packet 0, then the actual packet we were waiting for is whatever
     * the node says it is.
     */
    println("Collecting packet: %d", collectingPacketId());
    if (collectingPacketId() == 0) collectingPacketId(message[0]);
    print("NODE: %d; PACKET: %d; BATS:", from_node, message[0]);
    for (uint8_t i=1; i<msg_size; i++) {
        print(" %f", (float)message[i] / 10.0);
    }
    println("");
    waitingPacket(false);
    println("collection state: collecting: %d, waiting: %d, node idx: %d, packet: %d",
        collectingData(), waitingPacket(), collectingNodeIndex(), collectingPacketId());
}

void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size)
{
    #ifdef ARDUINO
    LoRa.idle();
    LoRa.beginPacket();
    int replyTo = 255;
    for (size_t i=0; i<route_size; i++) {
        if (route[i] == NODE_ID) {
            replyTo = route[i+1];
        }
    }
    //if (replyTo == 0) {
    //    println("ERROR: NO ROUTE!");
    //}
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
    uint8_t packet_id;
    if (dest == NODE_ID) {
        switch (packetType) {
            case PACKET_TYPE_SENDDATA:
                packet_id = message[0];
                sendDataPacket(packet_id, ++last_seq, route, route_size); // TODO: get packet # request from message
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
                if (NODE_ID != 1) {
                    // TODO: explcitly check if this is a collector
                    println("REC'd BROADCAST STANDBY FOR: %d", message[0]);
                    routeMessage(255, last_seq, PACKET_TYPE_STANDBY, route, route_size, message, msg_size);
                    standby(0); // TODO: get the timeout from the message
                }
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
    static uint8_t route_buffer[10];
    static uint8_t msg_buffer[255];
    int to = LoRa.read();
    int from = LoRa.read();
    int dest = LoRa.read();
    int seq = LoRa.read();
    int type = LoRa.read();
    size_t route_idx_ = 0;
    size_t msg_idx_ = 0;
    print("REC'D: TO: %d; FROM: %d; DEST: %d; SEQ: %d; TYPE: %d; RSSI: %d",
        to, from, dest, seq, type, LoRa.packetRssi());
    print("; ROUTE:");
    while (LoRa.available()) {
        uint8_t node = LoRa.read();
        if (node == 0) break;
        route_buffer[route_idx_++] = node;
        print(" %d", route_buffer[route_idx_-1]);
    }
    println("");
    while (LoRa.available()) {
        msg_buffer[msg_idx_++] = LoRa.read();
    }
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