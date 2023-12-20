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
    "}},"
    "\"val4\": {"
    "\"type\": \"object\","
    "\"required\": false,"
    "\"items\": {"
    "\"sub_val1\": {"
    "\"type\": \"number\","
    "\"required\": true"
    "},"
    "\"sub_val2\": {"
    "\"type\": \"string\","
    "\"required\": false"
    "}"
    "}"
    "}"
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
test_get_hashmap_keys(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    char *expected[] = {"val1", "val2", "val3", "val4"};
    char **keys = get_hashmap_keys(sf->scheme->fields);
    for (int i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        int ok = false;
        for (int j = 0; j < sf->scheme->fields->used_storage; j++) {
            if (strcmp(expected[i], keys[j]) == 0) {
                ok = true;
                break;
            }
        }
        g_assert_true(ok);
    }
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
    g_assert_null(val1_field->sub_scheme);

    scheme_field_t *val2_field = get_schema_field(sf->scheme, "val2");
    g_assert_cmpstr((const char *)val2_field->name, ==, "val2");
    g_assert_cmpint(val2_field->type, ==, STRING);
    g_assert_true(val2_field->required == FALSE);
    g_assert_null(val1_field->items);
    g_assert_null(val1_field->sub_scheme);

    scheme_field_t *val3_field = get_schema_field(sf->scheme, "val3");
    g_assert_cmpstr((const char *)val3_field->name, ==, "val3");
    g_assert_cmpint(val3_field->type, ==, ARRAY);
    g_assert_true(val3_field->required == FALSE);
    g_assert_nonnull(val3_field->items);
    g_assert_null(val3_field->sub_scheme);

    g_assert_cmpint(val3_field->items->type, ==, STRING);

    scheme_field_t *val4_field = get_schema_field(sf->scheme, "val4");
    g_assert_cmpstr((const char *)val4_field->name, ==, "val4");
    g_assert_cmpint(val4_field->type, ==, OBJECT);
    g_assert_true(val4_field->required == FALSE);
    g_assert_null(val4_field->items);
    g_assert_nonnull(val4_field->sub_scheme);

    scheme_field_t *val1_field_in_val4 = get_schema_field(val4_field->sub_scheme, "sub_val1");
    g_assert_cmpstr((const char *)val1_field_in_val4->name, ==, "sub_val1");
    g_assert_cmpint(val1_field_in_val4->type, ==, NUMBER);
    g_assert_true(val1_field_in_val4->required);

    scheme_field_t *val2_field_in_val4 = get_schema_field(val4_field->sub_scheme, "sub_val2");
    g_assert_cmpstr((const char *)val2_field_in_val4->name, ==, "sub_val2");
    g_assert_cmpint(val2_field_in_val4->type, ==, STRING);
    g_assert_true(val2_field_in_val4->required == FALSE);
}

void
test_validation_ok_required(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(3);
    add_hash_el(values_map, "val1", "1", sizeof(char) * 2, (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==, 0);
    g_assert_null(error.field);
    g_assert_null(error.text);
}

void
test_validation_ok_not_req_set(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(3);
    add_hash_el(values_map, "val1", "1", sizeof(char) * 2, (int (*)(void **))free);
    add_hash_el(values_map, "val2", "abc", sizeof(char) * 4, (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==, 0);
    g_assert_null(error.field);
    g_assert_null(error.text);
}

void
test_validation_ok_array(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(3);
    add_hash_el(values_map, "val1", "1", sizeof(char) * 2, (int (*)(void **))free);

    char *arr[] = {"aa1", "b2", "c3", "dd4"};
    add_hash_el(values_map, "val3", arr, sizeof(arr), (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==, 0);
    g_assert_null(error.field);
    g_assert_null(error.text);
}

void
test_validation_unknown_field(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(1);
    add_hash_el(values_map, "val_unk", "1", sizeof(char) * 2, (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==,
                    UNKNOWN_FIELD);
    g_assert_cmpstr(error.field, ==, "val_unk");
    g_assert_cmpstr(error.text, ==, "Unknown field");
}

void
test_validation_wrong_field_type(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(1);
    add_hash_el(values_map, "val1", "one", sizeof(char) * 4, (int (*)(void **))free);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==,
                    WRONG_FIELD_TYPE);
    g_assert_cmpstr(error.field, ==, "val1");
    g_assert_cmpstr(error.text, ==, "Wrong field type");
}

void
test_validation_unknown_field_type(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *values_map = init_hashmap(1);
    add_hash_el(values_map, "val1", "1", sizeof(char) * 2, (int (*)(void **))free);
    char *field_name = strdup("val1");
    scheme_field_t wrong_field = {
        .items = NULL, .sub_scheme = NULL, .name = field_name, .required = true, .type = 99};
    add_hash_el(sf->scheme->fields, "val1", (void *)&wrong_field, sizeof(scheme_field_t),
                free_field);
    validation_error_t error;
    g_assert_cmpint(validate_vals_by_scheme(sf->scheme, values_map, &error), ==,
                    UNKNOWN_FIELD_TYPE);
    g_assert_cmpstr(error.field, ==, "val1");
    g_assert_cmpstr(error.text, ==, "Unknown field type");
}

int
main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add("/set1/created test", schemefixture, NULL, scheme_setup, test_created,
               scheme_teardown);
    g_test_add("/set1/keys test_get_keys", schemefixture, NULL, scheme_setup,
               test_get_hashmap_keys, scheme_teardown);
    g_test_add("/set1/get test", schemefixture, NULL, scheme_setup, test_get, scheme_teardown);
    g_test_add("/set1/validation test_req_ok", schemefixture, NULL, scheme_setup,
               test_validation_ok_required, scheme_teardown);
    g_test_add("/set1/validation test_not_req_ok", schemefixture, NULL, scheme_setup,
               test_validation_ok_not_req_set, scheme_teardown);
    g_test_add("/set1/validation test_arr", schemefixture, NULL, scheme_setup,
               test_validation_ok_array, scheme_teardown);
    g_test_add("/set1/validation unknown_field", schemefixture, NULL, scheme_setup,
               test_validation_unknown_field, scheme_teardown);
    g_test_add("/set1/validation wrong_field_type", schemefixture, NULL, scheme_setup,
               test_validation_wrong_field_type, scheme_teardown);
    g_test_add("/set1/validation unknown_field_type", schemefixture, NULL, scheme_setup,
               test_validation_unknown_field_type, scheme_teardown);

    return g_test_run();
}
