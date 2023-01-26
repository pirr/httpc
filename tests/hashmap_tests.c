#include <assert.h>
#include <string.h>

#include "../src/hashmap.h"

char *expected[][2] = {
    {"key1", "value1"},
    {"key2", "value2"},
    {"key3", "value3"},
    {"key4", "value4"},
    {"verylonglonglongverylonglonglongkey5", "verylonglonglongverylonglonglongvalue5"},
};

typedef struct test_s {
    char *value;
} test_t;

int
free_test_s(void **test)
{
    if (*test == NULL)
        return 0;
    
    free((*((test_t **) test))->value);
    free(*test);
    test = NULL;

    return 0;
}

int
main()
{
    hashmap_storage_t *hashmap;
    hashmap = init_hashmap(2);

    unsigned expected_size = sizeof(expected) / sizeof(expected[0]);
    unsigned i;
    printf("ADD\n");
    for (i = 0; i < expected_size; i++) {
        test_t *test_value = (test_t *) malloc(sizeof(test_t));
        test_value->value = strdup(expected[i][1]);
        hashmap_value_t *value_p = create_hash_el_value(test_value, sizeof(test_t), free_test_s);
        add_hash_el(hashmap, expected[i][0], value_p);
    }

    assert(hashmap->used_storage == expected_size);

    printf("LOOK\n");
    for (i = 0; i < expected_size; i++) {
        test_t *test_value = (test_t *) look_hash_el(hashmap, expected[i][0])->value->v;
        assert(strcmp(test_value->value, expected[i][1]) == 0);
    }
    assert(look_hash_el(hashmap, "key1000") == NULL);

    printf("REPLACE\n");
    test_t *changed_test_value = (test_t *) malloc(sizeof(test_t));
    changed_test_value->value = strdup("changed value3");
    hashmap_value_t *changed_value = create_hash_el_value(changed_test_value, sizeof(changed_test_value), free_test_s);
    add_hash_el(hashmap, "key3", changed_value);
    test_t *changed_test_v = (test_t *) look_hash_el(hashmap, "key3")->value->v;
    assert(strcmp(changed_test_v->value, changed_test_value->value) == 0);

    printf("DELETE\n");
    delete_hash_el(&hashmap, "key1");
    assert(look_hash_el(hashmap, "key1") == NULL);
    assert(hashmap->used_storage == expected_size - 1);
    delete_hash_el(&hashmap, "key2");
    assert(look_hash_el(hashmap, "key2") == NULL);
    assert(hashmap->used_storage == expected_size - 2);
    delete_hash_el(&hashmap, "verylonglonglongverylonglonglongkey5");
    assert(look_hash_el(hashmap, "verylonglonglongverylonglonglongkey5") == NULL);
    assert(hashmap->used_storage == expected_size - 3);
    delete_hash_el(&hashmap, "key1000");
    assert(hashmap->used_storage == expected_size - 3);

    printf("CHECK AFTER DELETION\n");
    test_t *el4 = (test_t *) look_hash_el(hashmap, "key4")->value->v;
    assert(strcmp(el4->value, expected[3][1]) == 0);

    test_t *el3 = (test_t *) look_hash_el(hashmap, "key3")->value->v;
    assert(strcmp(el3->value, changed_test_value->value) == 0);

    assert(hashmap->storage[0]->next == NULL);
    assert(hashmap->storage[1]->next == NULL);

    printf("FREE\n");
    free_hash_storage(&hashmap);
    assert(hashmap == NULL);
}
