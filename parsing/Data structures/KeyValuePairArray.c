#include "KeyValuePairArray.h"


int init_key_value_array(KeyValuePairArray *array, unsigned char capacity) {
    if (capacity > MAX_CAPACITY)
        capacity = MAX_CAPACITY;

    array->capacity = capacity;
    array->data = NULL;
    array->size = 0;

    return SUCCESS;

}

int init_key_value_pair(KeyValuePair *kv, const char *key, const void *value, size_t value_size) {
    kv->key = strdup(key);
    kv->value = malloc(value_size);

    if(!kv->value)
        return FAIL;

    memcpy(kv->value, value, value_size);

    return SUCCESS;

}

int add_key_value(KeyValuePairArray *array, const KeyValuePair *kv) {
    if(array->size == array->capacity)
        return FAIL;

    array->data[array->size++] = *kv;
    return SUCCESS;

}

int free_key_value_pair_array(KeyValuePairArray *array) {


}