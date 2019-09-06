#include <LoRaHarvest.h>
#include <console.h>
#include <math.h>
#include <DataManager.h>
#include <ArduinoJson.h>

#ifdef ARDUINO
RTCZero rtcz;
#endif

uint8_t nodes[MAX_NODE_COUNT];
uint8_t routes[255][6] = { { } };
static uint8_t _tx_powers[255];
uint8_t node_count = 0;
static uint32_t _request_time = millis();
uint8_t _last_request_node;

uint8_t txPower(uint8_t node_id)
{
    return _tx_powers[node_id];
}

void txPower(uint8_t node_id, uint8_t db)
{
    if (db > MAX_LORA_TX_POWER) db = MAX_LORA_TX_POWER;
    if (db < MIN_LORA_TX_POWER) db = MIN_LORA_TX_POWER;
    _tx_powers[node_id] = db;
}

uint32_t requestTimer()
{
    return millis() - _request_time;
}

void resetRequestTimer()
{
    _request_time = millis();
}

void parseRoutingTable(char json[])
{
    // TODO: calculate actual needed size
    StaticJsonDocument<1024> routing_table;
    println("Parsing routing table from json ..");
    DeserializationError error = deserializeJson(routing_table, json);
    JsonObject root = routing_table.as<JsonObject>();
    if (error) {
        println("Failed to deserialize JSON: %s", error.c_str());
        while(1);
    } else {
        println(".. done parsing json");
        serializeJsonPretty(routing_table, Serial);
        println("");
        JsonArray _nodes = root["nodes"].as<JsonArray>();
        for (JsonArray::iterator it=_nodes.begin(); it!=_nodes.end(); ++it) {
            JsonObject node = it->as<JsonObject>();
            uint8_t _id = node["node_id"];
            JsonArray _route = node["route"].as<JsonArray>();
            nodes[node_count++] = _id;
            print(" -NODE %d. ROUTE: ", _id);
            uint8_t node_array_index = 0;
            for (JsonArray::iterator route_it=_route.begin(); route_it!=_route.end(); ++route_it) {
                uint8_t _next = route_it->as<uint8_t>();
                print("%d ", _next);
                routes[_id][node_array_index++] = _next;
            }
            routes[_id][node_array_index] = 0;
            println("");
        }
        print("Parsed routes. Nodes: ");
        for (uint8_t i=0; i<node_count; i++) {
            print("%d ", nodes[i]);
            txPower(nodes[i], DEFAULT_LORA_TX_POWER);
        }
        println("");
    }
}

void lastRequestNode(uint8_t node_id)
{
    _last_request_node = node_id;
}

uint8_t lastRequestNode()
{
    return _last_request_node;
}

uint8_t _ready_to_post = 0;


void readyToPost(uint8_t node_id)
{
    _ready_to_post = node_id;
}

uint8_t readyToPost()
{
    return _ready_to_post;
}

bool _is_collector = false;

void isCollector(bool _is)
{
    _is_collector = _is;
}

bool isCollector()
{
    return _is_collector;
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

static uint32_t _system_start_time;

void systemStartTime(uint32_t start_time)
{
    _system_start_time = start_time;
}

uint32_t systemStartTime()
{
    return _system_start_time;
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
    //LoRa.sleep();
    //delay(timeout);
    //LoRa.receive();
    #endif
}

void (*time_sync_fcn)(uint32_t);

int setTimeSyncFcn(timeSyncFcn the_function)
{
    time_sync_fcn = the_function;
    return 0;
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

void writeTimestamp()
{
    uint32_t ts = rtcz.getEpoch();
    println("Sending TIMESTAMP: %u", ts);
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
}

void sendDataPacket(uint8_t packet_id, int seq, uint8_t *reversedRoute, size_t route_size)
{
    #ifdef ARDUINO
    if (packet_id == 0) packet_id = numBatches(MAX_MESSAGE_SIZE);
    LoRa.idle();
    uint8_t to = reversedRoute[route_size-2];
    LoRa.setTxPower(txPower(to));
    LoRa.beginPacket();
    println("writing TO: %d", to);
    LoRa.write(to);
    LoRa.write(nodeId());
    LoRa.write(reversedRoute[0]);
    LoRa.write(seq);
    println("Writing TX: %d", txPower(to));
    LoRa.write(txPower(to));
    println("writing TYPE: %d", PACKET_TYPE_DATA);
    LoRa.write(PACKET_TYPE_DATA);
    writeTimestamp();
    for (size_t i=route_size; i-- > 0;) {
        LoRa.write(reversedRoute[i]);
    }
    LoRa.write(0); // end of route
    LoRa.write(packet_id);
    char *batch = getBatch(packet_id - 1);
    LoRa.print(batch);
    LoRa.endPacket();
    LoRa.setTxPower(DEFAULT_LORA_TX_POWER);
    if (packet_id == 1) clearData();
    LoRa.receive();
    #endif
}

void sendStandby(uint8_t seq, uint32_t next_collection)
{
    for (size_t i=0; i<sizeof(nodes); i++) {
        LoRa.flush();
        LoRa.idle();
        uint8_t to = routes[nodes[i]][1];
        LoRa.setTxPower(txPower(to));
        LoRa.beginPacket();
        LoRa.write(to);
        LoRa.write(nodeId());
        LoRa.write(nodes[i]);
        LoRa.write(seq);
        LoRa.write(txPower(to));
        LoRa.write(PACKET_TYPE_STANDBY);
        writeTimestamp();
        LoRa.write(routes[nodes[i]], sizeof(routes[nodes[i]]));
        LoRa.write(0); // end route
        LoRa.write(next_collection / 1000);
        LoRa.endPacket();
        LoRa.setTxPower(DEFAULT_LORA_TX_POWER);
        LoRa.receive();
    }
}

void sendCollectPacket(uint8_t node_id, uint8_t packet_id, uint8_t seq)
{
    println("Sending collect packet NODE: %d; PACKET: %d", node_id, packet_id);
    uint8_t *route = routes[node_id];
    uint8_t route_size = 0;
    while (route_size < sizeof(route) && route[route_size] > 0) route_size++;
    print("Fetching data from: %d; ROUTE: ", node_id);
    for (int j=0; j<route_size; j++) {
        print("%d ", route[j]);
    }
    uint8_t to = route[1];
    println("\nRoute size: %d; TX: ", route_size, txPower(to));
    LoRa.setTxPower(txPower(to));
    LoRa.flush();
    LoRa.idle();
    LoRa.beginPacket();
    LoRa.write(to);
    LoRa.write(nodeId());
    LoRa.write(node_id);
    LoRa.write(seq);
    LoRa.write(txPower(to));
    LoRa.write(1);
    writeTimestamp();
    LoRa.write(route, route_size);
    LoRa.write(0); // end route
    println("REQUESTING PACKET ID: %d", packet_id);
    LoRa.write(packet_id); // packet id
    LoRa.endPacket();
    LoRa.setTxPower(DEFAULT_LORA_TX_POWER);
    println("RECEIVING ...");
    LoRa.receive();
}

void handleDataMessage(uint8_t from_node, uint8_t seq, uint8_t *message, size_t msg_size)
{
    if (isCollector()) {
        uint8_t packet_id = message[0];
        print("NODE: %d; PACKET: %d; MESSAGE:", from_node, packet_id);
        for (uint8_t i=1; i<msg_size; i++) {
            print("%c", message[i]);
        }
        recordData((char*)(&message[1]), msg_size-1);
        println("\nGot current batch");
        if (packet_id == 1) { // TODO: be sure we received all available packets
            println("Packet ID is 1");
            readyToPost(from_node);
        } else {
            println("Packet ID is not 1");
            sendCollectPacket(from_node, --packet_id, seq);
            lastRequestNode(from_node);
            resetRequestTimer();
        }
    } else {
        println("Collecting packet: %d", collectingPacketId());
        if (collectingPacketId() == 0) collectingPacketId(message[0]);
        print("NODE: %d; PACKET: %d; MESSAGE:", from_node, message[0]);
        for (uint8_t i=1; i<msg_size; i++) {
            print("%c", message[i]);
        }
        recordData((char*)(&message[1]), msg_size-1);
        println("");
        char *batch = getCurrentBatch();
        println("** CURRENT BATCH:");
        for (size_t i=0; i<strlen(batch); i++) print("%c", batch[i]);
        println("");
        waitingPacket(false);
        println("collection state: collecting: %d, waiting: %d, node idx: %d, packet: %d",
            collectingData(), waitingPacket(), collectingNodeIndex(), collectingPacketId());
    }
}

void handleEchoMessage(uint8_t seq, uint8_t *reversedRoute, uint8_t route_size, uint8_t *message, uint8_t msg_size)
{
    #ifdef ARDUINO
    print("REV rOUTE:");
    for (uint8_t i=0; i<route_size; i++) print(" %d", reversedRoute[i]);
    println("");
    LoRa.idle();
    uint8_t to = reversedRoute[route_size-2];
    LoRa.setTxPower(txPower(to));
    LoRa.beginPacket();
    println("writing TO: %d", to);
    LoRa.write(to);
    println("writing FROM: %d", nodeId());
    LoRa.write(nodeId());
    println("writing DEST: %d", reversedRoute[0]);
    LoRa.write(reversedRoute[0]);
    println("writing SEQ: %d", seq);
    LoRa.write(seq);
    println("writing TX: %d", txPower(to));
    LoRa.write(txPower(to));
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
    LoRa.setTxPower(DEFAULT_LORA_TX_POWER);
    println(".. DONE");
    LoRa.receive();
    println("RECEIVING");
    #endif
}

void routeMessage(int dest, int seq, int packetType, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size)
{
    #ifdef ARDUINO
    LoRa.idle();
    int replyTo = 255;
    for (size_t i=0; i<route_size; i++) {
        if (route[i] == nodeId()) {
            replyTo = route[i+1];
        }
    }
    LoRa.setTxPower(txPower(replyTo));
    LoRa.beginPacket();
    LoRa.write(replyTo);                  // to
    LoRa.write(nodeId());                  // from
    LoRa.write(dest);                     // dest
    LoRa.write(seq);                      // seq
    if (replyTo == 255) {
        LoRa.write(0);
    } else {
        LoRa.write(txPower(replyTo));
    }
    LoRa.write(packetType);               // type
    writeTimestamp();
    LoRa.write(route, route_size);     // route
    LoRa.write(0);                        // end-route
    LoRa.write(message, msg_size); // message
    LoRa.endPacket();
    LoRa.setTxPower(DEFAULT_LORA_TX_POWER);
    LoRa.receive();
    #endif
}


int handlePacket(int to, int from, int dest, int seq, int tx, int packetType, uint32_t timestamp, uint8_t *route, size_t route_size, uint8_t *message, size_t msg_size, int topology)
{
    static int last_seq = 0;
    if (seq == last_seq) {
        return MESSAGE_CODE_DUPLICATE_SEQUENCE;
    }
    if (to != nodeId() && to != 255) return MESSAGE_CODE_WRONG_ADDRESS;
    println("REC'D RSSI: %d", LoRa.packetRssi());
    int rssi = LoRa.packetRssi();
    if (tx > 0) {
       if (rssi < -90) txPower(from, tx + 1);
       if (rssi > -80) txPower(from, tx - 1);
    }
    println("Set TX(%d) = %d", from, txPower(from));
    if (!topologyTest(topology, to, from)) return MESSAGE_CODE_TOPOLOGY_FAIL;
    last_seq = seq;
    uint8_t packet_id;
    if (dest == nodeId()) {
        switch (packetType) {
            case PACKET_TYPE_SENDDATA:
                packet_id = message[0];
                /* sync time with upstream requests */
                time_sync_fcn(timestamp);
                sendDataPacket(packet_id, ++last_seq, route, route_size); // TODO: get packet # request from message
                return MESSAGE_CODE_SENT_NEXT_DATA_PACKET;
            case PACKET_TYPE_DATA:
                handleDataMessage(route[0], ++++seq, message, msg_size);
                return MESSAGE_CODE_RECEIVED_DATA_PACKET;
            case PACKET_TYPE_STANDBY:
                standby(message[0]); // up to 255 seconds. TODO, use 2 bytes for longer timeouts
                return MESSAGE_CODE_STANDBY;
            case PACKET_TYPE_ECHO:
                if (!isCollector()) {
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
                if (!isCollector()) {
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
    int tx = LoRa.read();
    int type = LoRa.read();
    uint32_t ts = LoRa.read() << 24 | LoRa.read() << 16 | LoRa.read() << 8 | LoRa.read();
    size_t route_idx_ = 0;
    size_t msg_idx_ = 0;
    while (LoRa.available()) {
        uint8_t node = LoRa.read();
        if (node == 0) break;
        route_buffer[route_idx_++] = node;
    }
    while (LoRa.available()) {
        msg_buffer[msg_idx_++] = LoRa.read();
    }
    handlePacket(to, from, dest, seq, tx, type, ts, route_buffer, route_idx_, msg_buffer, msg_idx_, 0);
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
