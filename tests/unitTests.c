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

void test_set_request_type(void) {
    KeyValuePairArray array;
    init_key_value_array(&array, 5);
    TEST_ASSERT_EQUAL(SUCCESS, set_request_type("GET", &array));
    TEST_ASSERT_EQUAL_STRING("GET", array.request_type);
    free_key_value_pair_array(&array);
}

void test_add_key_value(void) {
    KeyValuePairArray array;
    KeyValuePair kv1, kv2, kv3;

    init_key_value_array(&array, 2);
    init_key_value_pair(&kv1, (char*) "key1", (void*) "value1", strlen("value1") + 1);
    TEST_ASSERT_EQUAL(SUCCESS, add_key_value(&array, &kv1));
    TEST_ASSERT_EQUAL(1, array.size);

    init_key_value_pair(&kv2, (char*) "key2", (void*) "value2", strlen("value2") + 1);
    TEST_ASSERT_EQUAL(SUCCESS, add_key_value(&array, &kv2));
    TEST_ASSERT_EQUAL(2, array.size);

    init_key_value_pair(&kv3, (char*) "key3", (void*) "value3", strlen("value3") + 1);
    TEST_ASSERT_EQUAL(FAIL, add_key_value(&array, &kv3));
    TEST_ASSERT_EQUAL(2, array.size);

    free(kv1.key);
    free(kv1.value);
    free(kv2.key);
    free(kv2.value);
    free(kv3.key);
    free(kv3.value);
}

void test_get_key(void) {
    KeyValuePairArray array;
    KeyValuePair kv1, kv2;

    init_key_value_array(&array, 2);
    init_key_value_pair(&kv1, (char*) "key1", (void*) "value1", strlen("value1") + 1);
    add_key_value(&array, &kv1);
    init_key_value_pair(&kv2, (char*) "key2", (void*) "value2", strlen("value2") + 1);
    add_key_value(&array, &kv2);

    TEST_ASSERT_EQUAL(SUCCESS, get_key(&array, "key1"));
    TEST_ASSERT_EQUAL(SUCCESS, get_key(&array, "key2"));
    TEST_ASSERT_EQUAL(FAIL, get_key(&array, "key3"));

    free(kv1.key);
    free(kv1.value);
    free(kv2.key);
    free(kv2.value);
}

void test_get_value(void) {
    KeyValuePairArray array;
    KeyValuePair kv1, kv2;
    void *value;

    init_key_value_array(&array, 2);
    init_key_value_pair(&kv1, (char*) "key1", (void*) "value1", strlen("value1") + 1);
    add_key_value(&array, &kv1);
    init_key_value_pair(&kv2, (char*) "key2", (void*) "value2", strlen("value2") + 1);
    add_key_value(&array, &kv2);

    TEST_ASSERT_EQUAL(SUCCESS, get_value(&array, "key1", &value));
    TEST_ASSERT_EQUAL_STRING("value1", (char*) value);

    TEST_ASSERT_EQUAL(SUCCESS, get_value(&array, "key2", &value));
    TEST_ASSERT_EQUAL_STRING("value2", (char*) value);

    TEST_ASSERT_EQUAL(FAIL, get_value(&array, "key3", &value));

    free(kv1.key);
    free(kv1.value);
    free(kv2.key);
    free(kv2.value);
}

void test_print_all_nodes(void) {
    KeyValuePairArray array;
    KeyValuePair kv1, kv2;

    init_key_value_array(&array, 2);
    set_request_type("GET", &array);
    init_key_value_pair(&kv1, (char*) "key1", (void*) "value1", strlen("value1") + 1);
    add_key_value(&array, &kv1);
    init_key_value_pair(&kv2, (char*) "key2", (void*) "value2", strlen("value2") + 1);
    add_key_value(&array, &kv2);

    TEST_ASSERT_EQUAL(SUCCESS, print_all_nodes(&array));

    free_key_value_pair_array(&array);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_key_value_array);
    RUN_TEST(test_init_key_value_pair);
    RUN_TEST(test_set_request_type);
    RUN_TEST(test_add_key_value);
    RUN_TEST(test_get_key);
    RUN_TEST(test_get_value);
    RUN_TEST(test_print_all_nodes);
    return UNITY_END();
}
