#include "EmbeddedThings/test_common/test_thing1/test_thing1.h"
#include <unity.h> // This include is needed or the setUp and tearDown will not be called
#include <stdio.h>

/**
 * We can really only have a single setUp and tearDown per testing group. It
 * will not help to push these into the submodules as they can only be defined
 * once (per testing group).
 *
 * If you really need different setUp or tearDown functions for a specific
 * group of tests, create a separate test group as exemplified by the
 * test_common_thing groups here.
 *
 * If you don't need distinct testing scaffolding, then this would be
 * organizational overkill and probably should not be used.
 */
void setUp(void) {
    printf("***** SETUP test_common_thing1\n");
}

void tearDown(void) {
    printf("TEARDOWN test_common_thing1 *****\n");
}


void process() {
    Test_Thing1::test_all();
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
