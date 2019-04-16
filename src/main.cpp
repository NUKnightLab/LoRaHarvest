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
 * Everything under lib should be included here so it can be found by unit
 * testing. Alternatively, these could be included as lib_deps in the
 * platformio.ini, however this will result in a "not been found in PlatformIO
 * Registry" warning for the internal libraries.
 */
#include <EmbeddedThings.h>

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
}

void loop() {
}

#else

int main(int argc, char **argv) {
    return 0;
}

#endif
