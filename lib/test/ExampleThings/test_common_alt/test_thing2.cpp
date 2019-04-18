#include <unity.h>
#include <ExampleThings.h>
#include <stdio.h>


namespace Test_Thing2 {

    Thing2 thing;

    void test_subtract(void) {
        TEST_ASSERT_EQUAL(18, thing.subtract(25, 7));
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_subtract);
        UNITY_END();
    }
}