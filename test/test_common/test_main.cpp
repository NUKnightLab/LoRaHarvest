#include "LoRaHarvest/test_common.h"
#include <unity.h> // This include is needed or the setUp and tearDown will not be called
#include <console.h>

void setUp(void) {
    println("***** SETUP test_common");
}

void tearDown(void) {
    println("TEARDOWN test_common *****");
}

void process() {
    Test_CommonThings::test_all();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    while (!Serial);
    //Serial.begin(115200);
    println("test_common is setup. Running tests ..");
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
