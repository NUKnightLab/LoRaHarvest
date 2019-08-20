#include <LoRaHarvest.h>
#include <console.h>
#include <math.h>
#include <DataManager.h>

#ifdef ARDUINO
RTCZero rtcz;
#endif

uint8_t nodes[2] = { 2, 3 };
uint8_t routes[255][6] = {
    { 0 },
    { 0 },
    { 1, 2 },
    { 1, 2, 3 },
    { 1, 2, 3, 6, 5, 4 },
    { 1, 3, 6, 5 },
    { 1, 2, 3, 6 }
};

uint8_t _ready_to_post = 0;

void readyToPost(uint8_t node_id)
{
    _ready_to_post = node_id;
}

uint8_t readyToPost()
{
    return _ready_to_post;
}


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

uint8_t _node_id;

void nodeId(uint8_t id)
{
    _node_id = id;
}

uint8_t nodeId()
{
    return _node_id;
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
    #ifdef ARDUINO
    LoRa.sleep();
    delay(timeout);
    LoRa.receive();
    #endif
}

#define VBATPIN 9

float getBatteryLevel()
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

void writeTimestamp()
{
    uint32_t ts = rtcz.getEpoch();
    print("Sending TIMESTAMP: %u", ts);
    LoRa.write(ts >> 24);
    LoRa.write(ts >> 16);
    LoRa.write(ts >> 8);
    LoRa.write(ts);
}


unsigned long getTimestamp()
{
    #ifdef ARDUINO
    return rtcz.getEpoch();
    #else
    return 0;
    #endif
}

void recordBattery()
{
    size_t bufsize = 40;
    char data[bufsize];
    snprintf(data, bufsize, "{\"bat\":%3.2f,\"ts\":%lu}", getBatteryLevel(), getTimestamp());
    recordData(data, strlen(data));
    recordData((uint8_t)nearbyintf(getBatteryLevel() * 10));
    //battery_samples[battery_data_index] = (uint8_t)nearbyintf(batteryLevel() * 10);
    //battery_timestamps[battery_data_index] = timestamp();
    //battery_data_index++;
}

void sendDataPacket(uint8_t packet_id, int seq, uint8_t *reversedRoute, size_t route_size)
{
    #ifdef ARDUINO
    if (packet_id == 0) packet_id = numBatches(MAX_MESSAGE_SIZE);
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
    println("writing FROM: %d", nodeId());
    LoRa.write(nodeId());
    println("writing DEST: %d", reversedRoute[0]);
    LoRa.write(reversedRoute[0]);
    println("writing SEQ: %d", seq);
    LoRa.write(seq);
    println("writing TYPE: %d", PACKET_TYPE_DATA);
    LoRa.write(PACKET_TYPE_DATA);
    writeTimestamp();
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
    //uint8_t batch_size = MAX_MESSAGE_SIZE;
    //uint8_t *batch = getBatch(packet_id - 1, &batch_size);
    //println("PACKET Batch size is %d", batch_size);
    char *batch = getBatch(packet_id - 1);
    println("Sending batch: %s", batch);
    LoRa.print(batch);
    //for (uint8_t i=0; i< batch_size; i++) {
    //    LoRa.write(batch[i]);
    //}
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

void sendStandby(uint8_t seq, uint32_t next_collection)
{
    for (int i=0; i<sizeof(nodes); i++) {
        LoRa.flush();
        LoRa.idle();
        LoRa.beginPacket();
        LoRa.write(routes[nodes[i]][1]);
        LoRa.write(nodeId());
        LoRa.write(nodes[i]);
        LoRa.write(seq);
        LoRa.write(PACKET_TYPE_STANDBY);
        writeTimestamp();
        LoRa.write(routes[nodes[i]], sizeof(routes[nodes[i]]));
        LoRa.write(0); // end route
        LoRa.write(next_collection / 1000);
        LoRa.endPacket();
        LoRa.receive();
    }
    //collectingNodeIndex(-1);
    //collectingPacketId(1);
    //collectingData(false);
    //waitingPacket(false);
}

void sendCollectPacket(uint8_t node_id, uint8_t packet_id, uint8_t seq)
{
    //println("prefetch collection state: collecting: %d, waiting: %d, node idx: %d, packet: %d",
    //    collectingData(), waitingPacket(), collectingNodeIndex(), packet_id);
    //waitingPacket(true);
    //uint8_t node_id = nodes[collectingNodeIndex()];
    println("Sending collect packet NODE: %d; PACKET: %d", node_id, packet_id);
    uint8_t *route = routes[node_id];
    uint8_t route_size = 0;
    while (route_size < sizeof(route) && route[route_size] > 0) route_size++;
    Serial.print("Fetching data from: ");
    Serial.print(node_id);
    Serial.print("; ROUTE: ");
    for (int j=0; j<route_size; j++) {
        Serial.print(route[j]);
        Serial.print(" ");
    }
    Serial.println("");
    LoRa.flush();
    LoRa.idle();
    LoRa.beginPacket();
    LoRa.write(route[1]);
    LoRa.write(nodeId());
    LoRa.write(node_id);
    LoRa.write(seq);
    //LoRa.write(PACKET_TYPE_SENDDATA);
    LoRa.write(1);
    writeTimestamp();
    LoRa.write(route, route_size);
    LoRa.write(0); // end route
    LoRa.write(packet_id); // packet id
    LoRa.endPacket();
    //timeout = millis() + 10000;
    //println("set timeout to: %d", timeout);
    LoRa.receive();
}

void handleDataMessage(uint8_t from_node, uint8_t *message, size_t msg_size)
{
    /**
     * If we asked for packet 0, then the actual packet we were waiting for is whatever
     * the node says it is.
     */
    println("Collecting packet: %d", collectingPacketId());
    if (collectingPacketId() == 0) collectingPacketId(message[0]);
    print("NODE: %d; PACKET: %d; MESSAGE:", from_node, message[0]);
    for (uint8_t i=1; i<msg_size; i++) {
        //print(" %f", (float)message[i] / 10.0);
        print("%c", message[i]);
    }
    recordData((char*)(&message[1]), msg_size-1);
    println("");
    char *batch = getCurrentBatch();
    println("** CURRENT BATCH:");
    for (int i=0; i<strlen(batch); i++) print("%c", batch[i]);
    println("");
    waitingPacket(false);
    println("collection state: collecting: %d, waiting: %d, node idx: %d, packet: %d",
        collectingData(), waitingPacket(), collectingNodeIndex(), collectingPacketId());
}

void collector_handleDataMessage(uint8_t from_node, uint8_t seq, uint8_t *message, size_t msg_size)
{
    uint8_t packet_id = message[0];
    static uint8_t nodes_collected[255] = {0};
    print("NODE: %d; PACKET: %d; MESSAGE:", from_node, packet_id);
    for (uint8_t i=1; i<msg_size; i++) {
        //print(" %f", (float)message[i] / 10.0);
        print("%c", message[i]);
    }
    recordData((char*)(&message[1]), msg_size-1);
    println("");
    char *batch = getCurrentBatch();
    println("Got current batch");
    if (packet_id == 1) { // TODO: be sure we received all available packets
        println("Packet ID is 1");
        readyToPost(from_node);
    } else {
        println("Packet ID is not 1");
        sendCollectPacket(from_node, --packet_id, seq);
    }
}

int collector_handlePacket(int to, int from, int dest, int seq, int packetType, uint32_t timestamp, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology)
{
    static int last_seq = 0;
    if (seq == last_seq) {
        return 1;
        //return MESSAGE_CODE_DUPLICATE_SEQUENCE;
    }
    println("Collector handle data packet to: %d; nodeId: %d", to, nodeId());
    if (to != nodeId() && to != 255) return 2;
    //if (to != config.node_id && to != 255) return MESSAGE_CODE_WRONG_ADDRESS;
    //if (to != nodeId() && to != 255) return MESSAGE_CODE_WRONG_ADDRESS;
    //if (!topologyTest(topology, to, from)) return MESSAGE_CODE_TOPOLOGY_FAIL;
    last_seq = seq;
    uint8_t packet_id;
    if (dest == nodeId()) {
        switch (packetType) {
            //case PACKET_TYPE_SENDDATA:
            //    packet_id = message[0];
            //    /* sync time with upstream requests */
            //    rtcz.setEpoch(timestamp);
            //    sendDataPacket(packet_id, ++last_seq, route, route_size); // TODO: get packet # request from message
            //    return MESSAGE_CODE_SENT_NEXT_DATA_PACKET;
            //case PACKET_TYPE_DATA:
            case 2:
                collector_handleDataMessage(route[0], ++++seq, message, msg_size);
                return 5;
                //return MESSAGE_CODE_RECEIVED_DATA_PACKET;
            //case PACKET_TYPE_STANDBY:
            //    standby(message[0]); // up to 255 seconds. TODO, use 2 bytes for longer timeouts
            //    return MESSAGE_CODE_STANDBY;
            //case PACKET_TYPE_ECHO:
            //    if (!isCollector) {
            //        handleEchoMessage(++last_seq, route, route_size, message, msg_size);
            //    } else {
            //        println("MESSAGE:");
            //        for (uint8_t i=0; i<msg_size; i++) {
            //            print("%d ", message[i]);
            //        }
            //        println("");
            //    }
        }
    } else if (dest == 255) { // broadcast message
        //switch (packetType) {
        //    case PACKET_TYPE_STANDBY:
        //        if (!isCollector) {
        //            println("REC'd BROADCAST STANDBY FOR: %d", message[0]);
        //            routeMessage(255, last_seq, PACKET_TYPE_STANDBY, route, route_size, message, msg_size);
        //            standby(message[0]);
        //        }
        //        return MESSAGE_CODE_STANDBY;
        //}
    } else { // route this message
        //routeMessage(dest, last_seq, packetType, route, route_size, message, msg_size);
        //return MESSAGE_CODE_ROUTED;
    }
    //return MESSAGE_CODE_NONE;
    return 0;
}

void collector_onReceive(int packetSize)
{
    static uint8_t route_buffer[MAX_ROUTE_SIZE];
    static uint8_t msg_buffer[255];
    int to = LoRa.read();
    if (to != nodeId()) return;
    int from = LoRa.read();
    int dest = LoRa.read();
    int seq = LoRa.read();
    int type = LoRa.read();
    uint32_t ts = LoRa.read() << 24 | LoRa.read() << 16 | LoRa.read() << 8 | LoRa.read();
    size_t route_idx_ = 0;
    size_t msg_idx_ = 0;
    print("REC'D: TO: %d; FROM: %d; DEST: %d; SEQ: %d; TYPE: %d; RSSI: %d; ts: %u",
        to, from, dest, seq, type, LoRa.packetRssi(), ts);
    print("; ROUTE:");
    while (LoRa.available()) {
        uint8_t node = LoRa.read();
        if (node == 0) break;
        route_buffer[route_idx_++] = node;
        print(" %d", route_buffer[route_idx_-1]);
    }
    println("");
    println("SNR: %f; FRQERR: %f", LoRa.packetSnr(), LoRa.packetFrequencyError());
    while (LoRa.available()) {
        msg_buffer[msg_idx_++] = LoRa.read();
    }
    collector_handlePacket(to, from, dest, seq, type, ts, route_buffer, route_idx_, msg_buffer, msg_idx_, 0);
}

void handleEchoMessage(uint8_t seq, uint8_t *reversedRoute, uint8_t route_size, uint8_t *message, uint8_t msg_size)
{
    #ifdef ARDUINO
    print("REV rOUTE:");
    for (uint8_t i=0; i<route_size; i++) print(" %d", reversedRoute[i]);
    println("");
    LoRa.idle();
    LoRa.beginPacket();
    println("writing TO: %d", reversedRoute[route_size-2]);
    LoRa.write(reversedRoute[route_size-2]);
    println("writing FROM: %d", nodeId());
    LoRa.write(nodeId());
    println("writing DEST: %d", reversedRoute[0]);
    LoRa.write(reversedRoute[0]);
    println("writing SEQ: %d", seq);
    LoRa.write(seq);
    println("writing TYPE: %d", PACKET_TYPE_ECHO);
    LoRa.write(PACKET_TYPE_ECHO); 
    writeTimestamp();
    println("SIZEOF REVERSED ROUTE: %d", route_size);
    for (size_t i=route_size; i-- > 0;) {
        println("ROUTE: %d", reversedRoute[i]);
        LoRa.write(reversedRoute[i]);
    }
    println("ZERO");
    LoRa.write(0);
    println("RECEIVED REQUEST FOR ECHO");
    LoRa.write(message, msg_size);
    LoRa.endPacket();
    println(".. DONE");
    LoRa.receive();
    println("RECEIVING");
    #endif
}

void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size)
{
    #ifdef ARDUINO
    LoRa.idle();
    LoRa.beginPacket();
    int replyTo = 255;
    for (size_t i=0; i<route_size; i++) {
        if (route[i] == nodeId()) {
            replyTo = route[i+1];
        }
    }
    //if (replyTo == 0) {
    //    println("ERROR: NO ROUTE!");
    //}
    /* 6 bytes + route size + message size */
    LoRa.write(replyTo);                  // to
    LoRa.write(nodeId());                  // from
    LoRa.write(dest);                     // dest
    LoRa.write(seq);                      // seq
    LoRa.write(packetType);               // type
    writeTimestamp();
    LoRa.write(route, route_size);     // route
    LoRa.write(0);                        // end-route
    LoRa.write(message, msg_size); // message
    LoRa.endPacket();
    LoRa.receive();
    #endif
}


int handlePacket(int to, int from, int dest, int seq, int packetType, uint32_t timestamp, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology)
{
    static int last_seq = 0;
    if (seq == last_seq) {
        return MESSAGE_CODE_DUPLICATE_SEQUENCE;
    }
    if (to != nodeId() && to != 255) return MESSAGE_CODE_WRONG_ADDRESS;
    if (!topologyTest(topology, to, from)) return MESSAGE_CODE_TOPOLOGY_FAIL;
    last_seq = seq;
    uint8_t packet_id;
    if (dest == nodeId()) {
        switch (packetType) {
            case PACKET_TYPE_SENDDATA:
                packet_id = message[0];
                /* sync time with upstream requests */
                rtcz.setEpoch(timestamp);
                sendDataPacket(packet_id, ++last_seq, route, route_size); // TODO: get packet # request from message
                return MESSAGE_CODE_SENT_NEXT_DATA_PACKET;
            case PACKET_TYPE_DATA:
                handleDataMessage(route[0], message, msg_size);
                return MESSAGE_CODE_RECEIVED_DATA_PACKET;
            case PACKET_TYPE_STANDBY:
                standby(message[0]); // up to 255 seconds. TODO, use 2 bytes for longer timeouts
                return MESSAGE_CODE_STANDBY;
            case PACKET_TYPE_ECHO:
                if (!isCollector) {
                    handleEchoMessage(++last_seq, route, route_size, message, msg_size);
                } else {
                    println("MESSAGE:");
                    for (uint8_t i=0; i<msg_size; i++) {
                        print("%d ", message[i]);
                    }
                    println("");
                }
        }
    } else if (dest == 255) { // broadcast message
        switch (packetType) {
            case PACKET_TYPE_STANDBY:
                if (!isCollector) {
                    println("REC'd BROADCAST STANDBY FOR: %d", message[0]);
                    routeMessage(255, last_seq, PACKET_TYPE_STANDBY, route, route_size, message, msg_size);
                    standby(message[0]);
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
    static uint8_t route_buffer[MAX_ROUTE_SIZE];
    static uint8_t msg_buffer[255];
    int to = LoRa.read();
    int from = LoRa.read();
    int dest = LoRa.read();
    int seq = LoRa.read();
    int type = LoRa.read();
    uint32_t ts = LoRa.read() << 24 | LoRa.read() << 16 | LoRa.read() << 8 | LoRa.read();
    size_t route_idx_ = 0;
    size_t msg_idx_ = 0;
    print("REC'D: TO: %d; FROM: %d; DEST: %d; SEQ: %d; TYPE: %d; RSSI: %d; ts: %u",
        to, from, dest, seq, type, LoRa.packetRssi(), ts);
    print("; ROUTE:");
    while (LoRa.available()) {
        uint8_t node = LoRa.read();
        if (node == 0) break;
        route_buffer[route_idx_++] = node;
        print(" %d", route_buffer[route_idx_-1]);
    }
    println("");
    println("SNR: %f; FRQERR: %f", LoRa.packetSnr(), LoRa.packetFrequencyError());
    while (LoRa.available()) {
        msg_buffer[msg_idx_++] = LoRa.read();
    }
    handlePacket(to, from, dest, seq, type, ts, route_buffer, route_idx_, msg_buffer, msg_idx_);
}

void setupLoRa(int csPin, int resetPin, int irqPin)
{
    LoRa.setPins(csPin, resetPin, irqPin);
    if (!LoRa.begin(frequency)) {
        println("LoRa init failed.");
        while(true);
    }
    /**
     * Sync word settng is not working in the LoRa library. See this issue:
     * https://github.com/sandeepmistry/arduino-LoRa/issues/16
     */
    //LoRa.setSyncWord(SYNC_WORD);
    LoRa.enableCrc();
    LoRa.onReceive(onReceive);
    LoRa.receive();
}
#endif
