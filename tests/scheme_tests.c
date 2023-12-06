#include <glib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/scheme.h"

#define UNUSED(x) (void)(x)

char *test_sceme =
    "{"
    "\"val1\": {"
    "\"type\":\"number\","
    "\"required\":true"
    "},"
    "\"val2\": {"
    "\"type\":\"string\","
    "\"required\":false"
    "},"
    "\"val3\": {"
    "\"type\":\"array\","
    "\"required\":false,"
    "\"items\":{"
    "\"type\": \"string\""
    "}}"
    "}";

typedef struct {
    scheme_t *scheme;
} schemefixture;

void
scheme_setup(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    sf->scheme = parse_scheme(test_sceme);
}

void
scheme_teardown(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    int res = free_scheme(&(sf->scheme));
    g_assert_cmpint(res, ==, 0);
}

void
test_created(schemefixture *sf, gconstpointer ignored)
{
    UNUSED(ignored);
    g_assert_nonnull(sf->scheme);
}

void
test_get(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    scheme_field_t *val1_field = get_schema_field(sf->scheme, "val1");
    g_assert_cmpstr((const char *)val1_field->name, ==, "val1");
    g_assert_cmpint(val1_field->type, ==, NUMBER);
    g_assert_true(val1_field->required);
    g_assert_null(val1_field->items);

    scheme_field_t *val2_field = get_schema_field(sf->scheme, "val2");
    g_assert_cmpstr((const char *)val2_field->name, ==, "val2");
    g_assert_cmpint(val2_field->type, ==, STRING);
    g_assert_true(val2_field->required == FALSE);
    g_assert_null(val2_field->items);

    scheme_field_t *val3_field = get_schema_field(sf->scheme, "val3");
    g_assert_cmpstr((const char *)val3_field->name, ==, "val3");
    g_assert_cmpint(val3_field->type, ==, ARRAY);
    g_assert_true(val3_field->required == FALSE);
    g_assert_nonnull(val3_field->items);

    g_assert_cmpint(val3_field->items->type, ==, STRING);
}

void
test_validation(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(3);
    add_hash_el(values_map, "val1", "1", sizeof(char) + 1, (int (*)(void **))free);
    add_hash_el(values_map, "val2", "abc", sizeof(char) * 4, (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==, 0);
    // int val3[] = {1, 2, 3, 4};
    // add_hash_el(values_map, "val3", val3, sizeof(val3), NULL);
    //
    // g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==, 0);
}

int
main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add("/set1/created test", schemefixture, NULL, scheme_setup, test_created,
               scheme_teardown);
    g_test_add("/set1/get test", schemefixture, NULL, scheme_setup, test_get, scheme_teardown);
    g_test_add("/set1/validation test", schemefixture, NULL, scheme_setup, test_validation,
               scheme_teardown);

    return g_test_run();
}
