#include "hashmap.h"

size_t
hash(const void *key, size_t hash_size)
{
    unsigned char *key_str = (unsigned char *) key;
    size_t hashval;

    for (hashval = 0; *key_str != '\0'; key_str++)
        hashval = *key_str + 31 * hashval;

    return hashval % hash_size;
}

hashmap_storage_t *
init_hashmap(size_t size, int (*compare_func)(hashmap_element_t *, const void *))
{
    hashmap_storage_t *hash_map;
    hash_map = (hashmap_storage_t *) malloc(sizeof(hashmap_storage_t));
    hash_map->size = size;
    hash_map->storage = (hashmap_element_t **) malloc(sizeof(hashmap_element_t *) * size);
    hash_map->compare_func = compare_func;
    hash_map->used_storage = 0;

    return hash_map;
}

hashmap_element_t *
look_hash_el(hashmap_storage_t *hash_storage, const void *key)
{
    hashmap_element_t *el;
    size_t hash_val = hash(key, hash_storage->size);
    for (el = hash_storage->storage[hash_val]; el != NULL; el = el->next)
        if (hash_storage->compare_func(el, key) == 0)
            return el;
    
    return NULL;
}

hashmap_element_t *
add_hash_el(hashmap_storage_t *hash_storage, const void *key, const void *value)
{
    hashmap_element_t *el;

    if ((el = look_hash_el(hash_storage, key)) == NULL) {
        el = (hashmap_element_t *) malloc(sizeof(hashmap_element_t));
        
        el->key = (void *) malloc(sizeof(key));
        el->value = (void *) malloc(sizeof(value));
        el->next = NULL;
        
        memcpy(el->key, key, sizeof(key));
        hash_storage->storage[hash(el->key, hash_storage->size)] = el;
        hash_storage->used_storage += 1;
    }
    else {
        free(el->value);
    }

    memcpy(el->value, value, sizeof(value));

    return el;
}

void
delete_hash_el(hashmap_storage_t **hash_storage, const void *key, int (*free_value_func)(void *))
{
    hashmap_element_t **el;
    hashmap_element_t *tmp;

    el = (hashmap_element_t **) malloc(sizeof(hashmap_element_t *));
    
    *el = look_hash_el(*hash_storage, key);
    if ((tmp = *el) != NULL) {
        if (tmp->next != NULL)
            *el = tmp->next;
        else
            el = NULL;
        
        free(tmp->key);
        free_value_func(tmp->value);
        free(tmp);

        (*hash_storage)->used_storage -= 1;
    }
}
