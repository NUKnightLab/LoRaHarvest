#include "LoRaHarvest/test_desktop.h"
#include <unity.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void process() {
    Test_DesktopThings::test_all();
}

int main(int argc, char **argv) {
    process();
    return 0;
}