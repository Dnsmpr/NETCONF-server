#include "KeyValuePairArray.h"


int init_key_value_array(KeyValuePairArray *array, unsigned char capacity) {
    if (capacity > MAX_CAPACITY)
        capacity = MAX_CAPACITY;

    array->capacity = capacity;
    array->data = NULL;
    array->size = 0;

    return SUCCESS;

}

int init_key_value_pair(KeyValuePair *kv, const char *key, const void *value) {
    

}

int add_key_value(KeyValuePairArray *array, const KeyValuePair *kv) {


}

int free_key_value_pair_array(KeyValuePairArray *array) {


}