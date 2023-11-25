#include <glib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/inprogress/scheme.h"

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
    free_scheme(&(sf->expected));
    free_scheme(&(sf->scheme));
}

void
test_new(schemefixture *sf, gconstpointer ignored)
{
    UNUSED(ignored);
    parse_scheme();
}

void
test_get(schemefixture *sf, gconstpointer test_data)
{
    UNUSED(test_data);
    get_schema_field();
}

int
main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add("/set1/new test", schemefixture, NULL, scheme_setup, test_new, sheme_teardown);
}
