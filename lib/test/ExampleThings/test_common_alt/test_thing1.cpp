#include <unity.h>
#include <ExampleThings.h>


namespace Test_Thing1 {

    Thing1 thing;

    void test_add(void) {
        TEST_ASSERT_EQUAL(32, thing.add(25, 7));
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_add);
        UNITY_END();
    }
}