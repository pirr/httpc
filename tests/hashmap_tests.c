#include <assert.h>
#include <string.h>

#include "../src/hashmap.h"

char *expected[][2] = {
    {"key1", "value1"},
    {"key2", "value2"},
    {"key3", "value3"},
    {"key4", "value4"},
};

typedef struct test_s {
    char *value;
} test_t;

int
compare_func(hashmap_element_t *el, const void *key)
{
    if (el == NULL)
        return 0;
    
    char *key_str = (char *) key;
    return strcmp((char *) el->key, key_str);
}

int
free_test_s(void *test)
{
    if (test == NULL)
        return 0;
    
    free(((test_t *) test)->value);
    free(test);

    return 0;
}

int
main()
{
    hashmap_storage_t *hashmap;
    hashmap = init_hashmap(DEFAULT_HASHSIZE, compare_func);

    unsigned expected_size = sizeof(expected) / sizeof(expected[0]);
    unsigned i;
    for (i = 0; i < expected_size; i++) {
        test_t *test_value = (test_t *) malloc(sizeof(test_t));
        test_value->value = strdup(expected[i][1]);
        add_hash_el(hashmap, expected[i][0], test_value);
    }

    assert(hashmap->used_storage == expected_size);

    for (i = 0; i < expected_size; i++) {
        test_t *test_value = (test_t *) look_hash_el(hashmap, expected[i][0])->value;
        assert(strcmp(test_value->value, expected[i][1]) == 0);
    }

    delete_hash_el(&hashmap, "key1", free_test_s);
    assert(look_hash_el(hashmap, "key1") == NULL);
    assert(hashmap->used_storage == expected_size - 1);

    delete_hash_el(&hashmap, "key2", free_test_s);
    assert(look_hash_el(hashmap, "key2") == NULL);
    assert(hashmap->used_storage == expected_size - 2);
}
