#ifndef KEY_VALUE_PAIR_API_H
#define KEY_VALUE_PAIR_API_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CAPACITY 10
#define SUCCESS      0
#define FAIL         -1


typedef struct KeyValuePair {
    char *key;
    void *value;
} KeyValuePair;

typedef struct KeyValuePairArray {
    KeyValuePair *data;
    size_t size;
    size_t capacity;
    char* request_type;
} KeyValuePairArray;



int init_key_value_array(KeyValuePairArray *array, unsigned char capacity);
int init_key_value_pair(KeyValuePair *kv, const char *key, const void *value, size_t value_size);
int set_request_type(const char *request, KeyValuePairArray *array);
int add_key_value(KeyValuePairArray *array, const KeyValuePair *kv);
int free_key_value_pair_array(KeyValuePairArray *array);
int print_all_nodes(KeyValuePairArray *array);
int get_key(KeyValuePairArray *array, char* key);
int get_value(KeyValuePairArray *array, char* key, void **value);

#endif
