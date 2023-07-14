#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

typedef struct Hashmap {
    int size;
    int capacity;
    Entry **buckets;
} Hashmap;

Hashmap *create_hashmap(int capacity);
void put(Hashmap *map, const char *key, int value);
int get(Hashmap *map, const char *key);
void free_hashmap(Hashmap *map);

#endif // HASHMAP_H
