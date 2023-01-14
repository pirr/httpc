#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#include "buffer.h"
#define DEFAULT_HASHSIZE 32

typedef struct hashmap_storage_s hashmap_storage_t;
typedef struct hashmap_element_s hashmap_element_t;
typedef struct hashmap_value_s hashmap_value_t;

struct hashmap_storage_s {
    size_t size;
    size_t used_storage;
    hashmap_element_t **storage;
    int (*compare_func)(hashmap_element_t *, const void *);
};

struct hashmap_element_s {
    void *key;
    void *value;
    hashmap_element_t *next;
};

hashmap_storage_t *init_hashmap(
    size_t size, 
    int (*compare_func)(hashmap_element_t *, const void *)
);

hashmap_element_t *add_hash_el(hashmap_storage_t *, const void *, const void *);
hashmap_element_t *look_hash_el(hashmap_storage_t *, const void *);
size_t hash(const void *, size_t);
void delete_hash_el(hashmap_storage_t **, const void *, int (*free_value_func)(void *));
