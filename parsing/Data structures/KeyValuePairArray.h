#ifndef KEY_VALUE_PAIR_API_H
#define KEY_VALUE_PAIR_API_H

#include <stddef.h>

#define MAX_CAPACITY 10
#define SUCCESS      0
#define FAIL         -1

typedef struct KeyValuePairArray {
    KeyValuePair *data;
    size_t size;
    size_t capacity;
} KeyValuePairArray;

typedef struct KeyValuePair {
    char *key;
    void *value;
} KeyValuePair;

int init_key_value_array(KeyValuePairArray *array, unsigned char capacity);
int init_key_value_pair(KeyValuePair *kv, const char *key, const void *value, size_t value_size);
int add_key_value(KeyValuePairArray *array, const KeyValuePair *kv);
int free_key_value_pair_array(KeyValuePairArray *array);

#endif
