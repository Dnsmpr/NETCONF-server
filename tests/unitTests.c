#include "unity.h"
#include "KeyValuePairArray.h"


void setUp(void) {

}

void tearDown(void) {

}


void test_init_key_value_array(void) {
    KeyValuePairArray array;
    TEST_ASSERT_EQUAL(SUCCESS, init_key_value_array(&array, 5));
    TEST_ASSERT_EQUAL(5, array.capacity);
    TEST_ASSERT_EQUAL(0, array.size);
    TEST_ASSERT_NOT_NULL(array.data);
    free_key_value_pair_array(&array);
}

void test_init_key_value_pair(void) {
    KeyValuePair kv;
    TEST_ASSERT_EQUAL(SUCCESS, init_key_value_pair(&kv, "test_key", "test_value", strlen("test_value") + 1));
    TEST_ASSERT_EQUAL_STRING("test_key", kv.key);
    TEST_ASSERT_EQUAL_STRING("test_value", (char *)kv.value);
    free(kv.key);
    free(kv.value);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_key_value_array);
    RUN_TEST(test_init_key_value_pair);
    // Add more RUN_TEST lines for the rest of the test functions
    return UNITY_END();
}


// Add more test functions for the rest of the functions in your implementation



