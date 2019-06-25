#include "LoRaHarvest/test_embedded.h"
#include <LoRaHarvest.h>
#include <unity.h>
#include <console.h>

void setUp(void) {
    setupLoRa(LORA_CS, LORA_RST, LORA_IRQ);
}

void tearDown(void) {
    // clean stuff up here
    LoRa.end();
}

void process() {
    Test_EmbeddedThings::test_all();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    while (!Serial);
    //Serial.begin(115200);
    print("test_embedded is setup. Running tests ..");
    process();
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif