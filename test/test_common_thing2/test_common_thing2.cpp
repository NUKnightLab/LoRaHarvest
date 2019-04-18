#include "ExampleThings/test_common_alt/test_thing2.h"
#include <unity.h>
#include <console.h>

void setUp(void) {
    println("***** SETUP test_common_alt Testing: Thing2");
}

void tearDown(void) {
    println("TEARDOWN test_common_alt for Thing2 *****");
}


void process() {
    Test_Thing2::test_all();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
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
