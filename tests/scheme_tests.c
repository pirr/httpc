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
    "\"required\":\"true\""
    "},"
    "\"val2\": {"
    "\"type\":\"string\","
    "\"required\":\"false\""
    "},"
    "\"val3\": {"
    "\"type\":\"array\","
    "\"required\":\"false\","
    "\"items\":{"
    "\"type\": \"string\""
    "}"
    "}";

typedef struct {
    scheme_t *scheme;
    scheme_t *expected;
} schemefixture;

void
scheme_setup(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    hashmap_storage_t *fields = init_hashmap(8);
    scheme_field_t val1_field = {
        .name = "val1", .type = NUMBER, .items = NULL, .required = TRUE};
    scheme_field_t val2_field = {
        .name = "val2", .type = STRING, .items = NULL, .required = FALSE};

    scheme_field_t val3_items = {
        .name = NULL, .type = STRING, .items = NULL, .required = NULL};
    scheme_field_t val3_field = {
        .name = "val2", .type = ARRAY, .items = &val3_items, .required = FALSE};

    add_hash_el(fields, "val1", &val1_field, sizeof(scheme_field_t), free_field);
    add_hash_el(fields, "val2", &val2_field, sizeof(scheme_field_t), free_field);
    add_hash_el(fields, "val3", &val3_field, sizeof(scheme_field_t), free_field);

    sf->scheme = malloc(sizeof(scheme_t));
    sf->scheme->fields = fields;
}

void
sheme_teardown(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    int res = free_scheme(&(sf->scheme));
    g_assert_cmpint(res, ==, 0);
}

void
test_new(schemefixture *sf, gconstpointer ignored)
{
    UNUSED(ignored);
    sf->scheme = parse_scheme(test_sceme);
}

void
test_get(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    scheme_field_t *get_schema_field();
}

void
test_equal(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    scheme_field_t *val1_field = get_schema_field(sf->scheme, "val1");

    g_assert_cmpstr((const char *)val1_field->name, ==, "val1");
    g_assert_cmpstr((const char *)val1_field->type, ==, "number");
    g_assert_true(val1_field->required == TRUE);
    g_assert_null(val1_field->items);

    scheme_field_t *val2_field = get_schema_field(sf->scheme, "val2");

    g_assert_cmpstr((const char *)val2_field->name, ==, "val2");
    g_assert_cmpstr((const char *)val2_field->type, ==, "string");
    g_assert_true(val2_field->required == FALSE);
    g_assert_null(val2_field->items);

    scheme_field_t *val3_field = get_schema_field(sf->scheme, "val3");

    g_assert_cmpstr((const char *)val3_field->name, ==, "val3");
    g_assert_cmpstr((const char *)val3_field->type, ==, "array");
    g_assert_true(val3_field->required == FALSE);
    g_assert_null(val3_field->items);

    g_assert_null(val3_field->items->name);
    g_assert_null(val3_field->items->items);
    g_assert_true(val3_field->items->required == TRUE);
    g_assert_cmpstr((const char *)val3_field->items->type, ==, "string");
}

int
main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add("/set1/new test", schemefixture, NULL, scheme_setup, test_new, sheme_teardown);
}
