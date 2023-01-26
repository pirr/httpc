#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#define DEFAULT_HASHSIZE 32

typedef struct hashmap_storage_s hashmap_storage_t;
typedef struct hashmap_element_s hashmap_element_t;
typedef struct hashmap_value_s hashmap_value_t;

struct hashmap_storage_s {
    size_t size;
    size_t used_storage;
    hashmap_element_t **storage;
};

struct hashmap_value_s {
    void *v;
    size_t s;
    int (*free_value_func)(void **);
};

struct hashmap_element_s {
    char *key;
    hashmap_value_t *value;
    hashmap_element_t *next;
};

hashmap_storage_t *init_hashmap(size_t size);
hashmap_value_t *create_hash_el_value(void *, size_t size, int (*)(void **));
hashmap_element_t *add_hash_el(hashmap_storage_t *, const char *, hashmap_value_t *);
hashmap_element_t *look_hash_el(hashmap_storage_t *, const char *);
void delete_hash_el(hashmap_storage_t **, const char *);
int free_hash_storage(hashmap_storage_t **);
