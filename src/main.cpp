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

uint8_t nodes[2] = { 2, 3 };
uint8_t routes[255][5] = {
    { 0 },
    { 0 },
    { 1, 2 },
    { 1, 2, 3 },
    { 1, 2, 3, 4 },
    { 1, 2, 3, 5 }
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
            LoRa.write(0);
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
