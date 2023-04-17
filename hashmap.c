#include "hashmap.h"

unsigned int hash(const char *key, int capacity) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

Hashmap *create_hashmap(int capacity) {
    Hashmap *map = (Hashmap *)malloc(sizeof(Hashmap));
    map->size = 0;
    map->capacity = capacity;
    map->buckets = (Entry **)calloc(capacity, sizeof(Entry *));
    return map;
}

void put(Hashmap *map, const char *key, int value) {
    unsigned int index = hash(key, map->capacity);
    Entry *entry = map->buckets[index];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    entry = (Entry *)malloc(sizeof(Entry));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    map->size++;
}

int get(Hashmap *map, const char *key) {
    unsigned int index = hash(key, map->capacity);
    Entry *entry = map->buckets[index];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return -1;
}

void free_hashmap(Hashmap *map) {
    for (int i = 0; i < map->capacity; i++) {
        Entry *entry = map->buckets[i];
        while (entry != NULL) {
            Entry *next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}
