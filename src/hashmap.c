#include "hashmap.h"

size_t
hash(const char *key, size_t hash_size)
{
    size_t hashval;

    for (hashval = 0; *key != '\0'; key++)
        hashval = *key + 31 * hashval;

    return hashval % hash_size;
}

hashmap_storage_t *
init_hashmap(size_t size)
{
    size_t i;
    hashmap_storage_t *hash_map;
    hash_map = (hashmap_storage_t *) malloc(sizeof(hashmap_storage_t));
    hash_map->size = size;
    hash_map->storage = (hashmap_element_t **) malloc(sizeof(hashmap_element_t *) * size);
    hash_map->used_storage = 0;
    
    for (i = 0; i < size; i++)
        hash_map->storage[i] = NULL;

    return hash_map;
}

int
compare_func(hashmap_element_t *el, const char *key)
{
    if (el == NULL)
        return 0;
    
    return strcmp(el->key, key);
}

hashmap_element_t *
look_hash_el(hashmap_storage_t *hash_storage, const char *key)
{
    hashmap_element_t *el;
    size_t hash_val = hash(key, hash_storage->size);
    for (el = hash_storage->storage[hash_val]; el != NULL; el = el->next)
        if (compare_func(el, key) == 0)
            return el;
    
    return NULL;
}

hashmap_value_t *
create_hash_el_value(void *value, size_t value_size, int (*free_value_func)(void**))
{
    hashmap_value_t *hashmap_value = (hashmap_value_t *) malloc(sizeof(hashmap_value_t));
    hashmap_value->free_value_func = free_value_func;
    hashmap_value->s = value_size;
    hashmap_value->v = value;

    return hashmap_value;
}

hashmap_element_t *
add_hash_el(hashmap_storage_t *hash_storage, const char *key, hashmap_value_t *value)
{
    hashmap_element_t *el, *existed_el;
    size_t hash_val;
    
    if ((el = look_hash_el(hash_storage, key)) == NULL) {
        el = (hashmap_element_t *) malloc(sizeof(hashmap_element_t));
        
        el->key = (char *) malloc(sizeof(key) * strlen(key));
        el->next = NULL;
        memcpy(el->key, key, strlen(key));

        hash_val = hash(key, hash_storage->size);
        if ((existed_el = hash_storage->storage[hash_val]) != NULL) {
            while (existed_el->next != NULL)
                existed_el = existed_el->next;
            existed_el->next = el;
        } else {
            hash_storage->storage[hash_val] = el;
        }
        hash_storage->used_storage += 1;
    } else {
        el->value->free_value_func(&(el->value->v));
        free(el->value);
    }
    
    el->value = value;

    return el;
}

void
delete_hash_el(hashmap_storage_t **hash_storage, const char *key)
{
    hashmap_element_t *el, *tmp;
    size_t hash_val;

    el = look_hash_el(*hash_storage, key);
    if (el != NULL) {
        tmp = el;
        el = el->next;
        free(tmp->key);
        tmp->key = NULL;
        tmp->value->free_value_func(&(tmp->value->v));
        free(tmp->value);
        tmp->value = NULL;
        free(tmp);
        (*hash_storage)->used_storage -= 1;

        hash_val = hash(key, (*hash_storage)->size);
        if ((*hash_storage)->storage[hash_val]->key == NULL) {
            // remove first element
            if (el == NULL) {
                free((*hash_storage)->storage[hash_val]);
                (*hash_storage)->storage[hash_val] = NULL;
            }
            (*hash_storage)->storage[hash_val] = el;
        } 
        else if (el == NULL) {
            // remove last element
            for (el = (*hash_storage)->storage[hash_val]; el->next != tmp && el != NULL; el = el->next)
                {};
            if (el != NULL) {
                el->next = NULL;
            }
        }
        tmp = NULL;
    }
}

int
free_hash_storage(hashmap_storage_t **hash_storage)
{
    if (hash_storage == NULL)
        return 0;

    size_t i;
    hashmap_element_t *existed_el;
    hashmap_element_t *tmp;

    for (i = 0; i < (*hash_storage)->size; i++) {
        if ((*hash_storage)->storage[i] == NULL)
            continue;

        existed_el = (*hash_storage)->storage[i];
        while (existed_el != NULL) {
            tmp = existed_el;
            existed_el = existed_el->next;
            
            if (tmp->value != NULL) {
                tmp->value->free_value_func(&(tmp->value->v));
                free(tmp->value);
            }
            if (tmp->key != NULL)
                free(tmp->key);
            
            free(tmp);
            tmp = NULL;
        }
        (*hash_storage)->storage[i] = NULL;
    }

    free((*hash_storage)->storage);
    (*hash_storage)->storage = NULL;
    free(*hash_storage);
    *hash_storage = NULL;

    return 0;
}
