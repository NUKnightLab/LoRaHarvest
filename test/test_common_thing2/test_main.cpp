#include "EmbeddedThings/test_common/test_thing2/test_thing2.h"
#include <unity.h>
#include <stdio.h>

void setUp(void) {
    printf("***** test_thing2 SETUP\n");
}

void tearDown(void) {
    printf("test_thing2 TEARDOWN *****\n");
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
