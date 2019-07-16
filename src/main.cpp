/**
 * Even if you are creating a library project, this main.cpp file is required
 * in order for the project to compile.
 *
 * If you are creating a main project, you might want to replace this file with
 * an appropriate .ino file for compatibility with the Arduino IDE. Note:
 * Arduino IDE compatibility has not been tested with this template. It seems
 * likely that the IDE would have some trouble dealing with some of the
 * organizational complexity here.
 */


/**
 * Everything under lib that you plan to test should be included here so it
 * can be found by unit testing. Alternatively, these could be included as
 * lib_deps in the platformio.ini, however this will result in a "not been
 * found in PlatformIO Registry" warning for the internal libraries.
 */
#include <LoRaHarvest.h>
#include <console.h>

#ifndef UNIT_TEST

#ifdef ARDUINO

uint8_t nodes[1] = { 2 };
uint8_t routes[255][6] = {
    { 0 },
    { 0 },
    { 1, 2 },
    { 1, 2, 3 },
    { 1, 2, 3, 6, 5, 4 },
    { 1, 3, 6, 5 },
    { 1, 2, 3, 6 }
};


#include <Arduino.h>

static unsigned long previousMillis = 0;
static unsigned long interval = 60000;

bool runEvery()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

unsigned long nextCollection()
{
    return previousMillis + interval;
}

bool scheduleDataSample(unsigned long interval)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

void setup() {
    if (NODE_ID == 1) isCollector = true;
    LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
    if (!LoRa.begin(915E6)) {
        Serial.println("LoRa init failed");
        while(true);
    }
    LoRa.enableCrc();
    LoRa.onReceive(onReceive);
    LoRa.receive();
}

void loop() {
    static unsigned long tick_time = 0;
    if (millis() > tick_time) {
        print(".");
        tick_time = millis() + 1000;
    }

    static unsigned long timeout = 0;
    static uint8_t seq = 0;
    if (isCollector && runEvery()) collectingData(true);
    if (!isCollector && scheduleDataSample(5000)) recordBattery();
    if (collectingData()) {
        if (waitingPacket()) {
            if (millis() > timeout) {
                println("TIMEOUT");
                collectingNodeIndex(-1);
                collectingPacketId(1);
                collectingData(false); // TODO: retry data fetch
                waitingPacket(false);
            }
        } else {
            collectingPacketId(collectingPacketId() - 1);
            if (collectingPacketId() == 0) {
                collectingNodeIndex(collectingNodeIndex() + 1);
            }
            if (collectingNodeIndex() >= sizeof(nodes)) {
                for (int i=0; i<sizeof(nodes); i++) {
                    // send shutdown
                    LoRa.idle();
                    LoRa.beginPacket();
                    LoRa.write(routes[nodes[i]][1]);
                    LoRa.write(NODE_ID);
                    LoRa.write(nodes[i]);
                    LoRa.write(++++seq);
                    LoRa.write(PACKET_TYPE_STANDBY);
                    LoRa.write(routes[nodes[i]], sizeof(routes[nodes[i]]));
                    LoRa.write(0); // end route
                    LoRa.write(nextCollection() / 1000);
                    LoRa.endPacket();
                    LoRa.receive();
                }
                collectingNodeIndex(-1);
                collectingPacketId(1);
                collectingData(false);
                waitingPacket(false);
                // TODO: send standby
                return;
            }
            println("prefetch collection state: collecting: %d, waiting: %d, node idx: %d, packet: %d",
                collectingData(), waitingPacket(), collectingNodeIndex(), collectingPacketId());
            waitingPacket(true);
            uint8_t node_id = nodes[collectingNodeIndex()];
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
            LoRa.idle();
            LoRa.beginPacket();
            LoRa.write(route[1]);
            LoRa.write(NODE_ID);
            LoRa.write(node_id);
            LoRa.write(++++seq);
            LoRa.write(PACKET_TYPE_SENDDATA);
            LoRa.write(route, route_size);
            LoRa.write(0); // end route
            LoRa.write(collectingPacketId()); // packet id
            LoRa.endPacket();
            timeout = millis() + 10000;
            println("set timeout to: %d", timeout);
            LoRa.receive();
            //Serial.println("Sending broadcast standby");
            //LoRa.idle();
            //LoRa.beginPacket();
            //LoRa.write(255);
            //LoRa.write(NODE_ID);
            //LoRa.write(255);
            //LoRa.write(++++seq);
            //LoRa.write(PACKET_TYPE_STANDBY);
            //LoRa.write(0);
            //LoRa.write(20); // 20 seconds
            //LoRa.endPacket();
            //LoRa.receive();
        }
    }
}

#else

int main(int argc, char **argv) {
    return 0;
}

#endif
#endif
