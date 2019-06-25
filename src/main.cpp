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

#ifndef UNIT_TEST

#ifdef ARDUINO

uint8_t nodes[1] = { 2 };
uint8_t routes[255][5] = {
    { 0 },
    { 0 },
    { 2 },
    { 2, 3 },
    { 2, 3, 4 },
    { 2, 3, 5 }
};

#include <Arduino.h>

bool runEvery(unsigned long interval)
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
    static uint8_t seq = 0;
    if (NODE_ID == 1 && runEvery(30000)) {
        for (int i=0; i<sizeof(nodes); i++) {
            uint8_t node_id = nodes[i];
            uint8_t *route = routes[node_id];
            LoRa.idle();
            LoRa.beginPacket();
            LoRa.write(route[0]);
            LoRa.write(NODE_ID);
            LoRa.write(node_id);
            LoRa.write(++++seq);
            LoRa.write(PACKET_TYPE_SENDDATA);
            LoRa.write(route, sizeof(route));
            LoRa.endPacket();
            LoRa.receive();
            delay(3000);
        }
    }
}

#else

int main(int argc, char **argv) {
    return 0;
}

#endif
#endif
