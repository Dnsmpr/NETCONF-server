#include "KeyValuePairArray.h"


int init_key_value_array(KeyValuePairArray *array, unsigned char capacity) {
    if (capacity > MAX_CAPACITY)
        capacity = MAX_CAPACITY;

    array->capacity = capacity;
    array->data = malloc(array->capacity*sizeof(KeyValuePair));
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

int set_request_type(const char *request, KeyValuePairArray *array) {
    array->request_type = strdup(request);
    if(!array->request_type)
        return FAIL;

    return SUCCESS;
}

int add_key_value(KeyValuePairArray *array, const KeyValuePair *kv) {
    if(array->size == array->capacity)
        return FAIL;

    array->data[array->size++] = *kv;
    return SUCCESS;

}

int free_key_value_pair_array(KeyValuePairArray *array) {
    for(int i = 0; i < array->size; i++) {
        free(array->data[i].key);
        free(array->data[i].value);

    }
    free(array->request_type);
    free(array->data);

    return SUCCESS;

}

int print_all_nodes(KeyValuePairArray *array) {
    for (int i = 0; i < array->size; i++)
        printf("key: %- *s\tvalue: %- *s\n", 15, array->data[i].key, 15, (const char*)array->data[i].value);

    return SUCCESS;
}