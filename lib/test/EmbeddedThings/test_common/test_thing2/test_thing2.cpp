#include <unity.h>
#include <EmbeddedThings.h>
#include <stdio.h>

Thing2 thing;

namespace Test_Thing2 {

    void test_subtract(void) {
        TEST_ASSERT_EQUAL(18, thing.subtract(25, 7));
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_subtract);
        UNITY_END();
    }
}