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




