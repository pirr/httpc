#include <glib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/hashmap.h"

char *expected[][2] = {
    {"key1", "value1"},
    {"key2", "value2"},
    {"key3", "value3"},
    {"key4", "value4"},
    {"verylonglonglongverylonglonglongkey5", "verylonglonglongverylonglonglongvalue5"},
};

typedef struct {
    hashmap_storage_t *hashmap;
} mapfixture;

typedef struct {
    char *value;
} test_t;

int
free_test_t(void **t)
{
    test_t **tt = (test_t **)t;
    free((*tt)->value);
    free(*tt);
    return 0;
}

void
hashmap_setup(mapfixture *hmf, gconstpointer test_data)
{
    hmf->hashmap = init_hashmap(2);
    unsigned expected_size = sizeof(expected) / sizeof(expected[0]);
    unsigned i;

    for (i = 0; i < expected_size; i++) {
        test_t *test_value = (test_t *)malloc(sizeof(test_t));
        test_value->value = strdup(expected[i][1]);
        add_hash_el(hmf->hashmap, expected[i][0], test_value, sizeof(test_t), free_test_t);
    }
}

void
hashmap_teardown(mapfixture *hmf, gconstpointer test_data)
{
    free_hash_storage(&(hmf->hashmap));
}

void
check_keys(hashmap_storage_t const *hm)
{
    unsigned i, expected_size;
    expected_size = sizeof(expected) / sizeof(expected[0]);
    for (i = 0; i < expected_size; i++) {
        test_t *test_value =
            (test_t *)look_hash_el((hashmap_storage_t *)hm, expected[i][0])->value->v;
        g_assert_cmpstr(test_value->value, ==, expected[i][1]);
    }
}

void
update_value(hashmap_storage_t *hm)
{
    char *new_value_str = "hello I have been changed.";
    test_t *changed_value = (test_t *)malloc(sizeof(test_t));
    changed_value->value = strdup(new_value_str);
    add_hash_el(hm, expected[0][0], changed_value, sizeof(test_t), free_test_t);
    test_t *test_value = (test_t *)look_hash_value(hm, expected[0][0]);
    g_assert_cmpstr(test_value->value, ==, new_value_str);
}

void
delete_key(hashmap_storage_t *hm)
{
    unsigned i, expected_size;
    expected_size = sizeof(expected) / sizeof(expected[0]);
    for (i = 0; i < expected_size; i++) {
        delete_hash_el(&hm, expected[i][0]);
    }
    for (i = 0; i < expected_size; i++) {
        g_assert_true(look_hash_value(hm, expected[i][0]) == NULL);
    }
}

void
test_new(mapfixture *hmf, gconstpointer ignored)
{
    check_keys(hmf->hashmap);
}

void
test_update(mapfixture *hmf, gconstpointer ignored)
{
    update_value(hmf->hashmap);
}

void
test_delete(mapfixture *hmf, gconstpointer ignored)
{
    delete_key(hmf->hashmap);
}

int
main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add("/set1/new test", mapfixture, NULL, hashmap_setup, test_new, hashmap_teardown);
    g_test_add("/set1/update value test", mapfixture, NULL, hashmap_setup, test_update,
               hashmap_teardown);
    g_test_add("/set1/delete key test", mapfixture, NULL, hashmap_setup, test_delete,
               hashmap_teardown);
    return g_test_run();
}
