#include <unity.h>

namespace Test_DesktopThings {

    void test_test(void) {
        TEST_ASSERT_TRUE(true);
    }

    void test_all() {
        UNITY_BEGIN();
        RUN_TEST(test_test);
        UNITY_END();
    }
}