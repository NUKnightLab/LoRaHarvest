#include <unity.h>
#include <EmbeddedThings.h>
#include <stdio.h>
#include "test_main.h"

Thing1 thing1;
Thing2 thing2;

namespace Test_Things {

    void test_add(void) {
        TEST_ASSERT_EQUAL(32, thing1.add(25, 7));
    }

    void test_subtract(void) {
        TEST_ASSERT_EQUAL(18, thing2.subtract(25, 7));
    }

    /**
     * We put the UNITY_BEGIN and UNITY_END calls here in the submodules rather than a single set
     * of calls in the main runner because the location of these determines the filename that
     * is output during testing. Doing this here gives us the more specific output of the filename
     * where the tests actually reside.
     */
    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_add);
        RUN_TEST(test_subtract);
        UNITY_END();
    }
}