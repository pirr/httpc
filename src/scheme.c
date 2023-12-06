#include <stdbool.h>

#include "scheme.h"

#define DEFAULT_SCHEME_FIELDS_SIZE 8

scheme_t *
parse_scheme(char *scheme_str)
{
    json_error_t error;
    json_t *root = json_loads(scheme_str, 0, &error);

    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return NULL;
    }

    scheme_t *scheme = malloc(sizeof(scheme_t));
    scheme->fields = init_hashmap(DEFAULT_SCHEME_FIELDS_SIZE);

    const char *field_name;
    json_t *field_value;

    json_object_foreach(root, field_name, field_value)
    {
        scheme_field_t *field = parse_field(field_value, field_name);
        add_hash_el(scheme->fields, field_name, (void *)field, sizeof(scheme_field_t),
                    free_field);
    }

    json_decref(root);

    return scheme;
}

scheme_field_t *
parse_field(json_t *field_json, const char *field_name)
{
    scheme_field_t *field = malloc(sizeof(scheme_field_t));
    if (field_name != NULL)
        field->name = strdup(field_name);
    else
        field->name = NULL;

    field->items = NULL;

    const char *type_str = json_string_value(json_object_get(field_json, "type"));
    if (strcmp(type_str, "number") == 0) {
        field->type = NUMBER;
    }
    else if (strcmp(type_str, "string") == 0) {
        field->type = STRING;
    }
    else if (strcmp(type_str, "integer") == 0) {
        field->type = INTEGER;
    }
    else if (strcmp(type_str, "array") == 0) {
        field->type = ARRAY;
        json_t *items_json = json_object_get(field_json, "items");
        field->items = parse_field(items_json, NULL);
    }
    field->required = json_is_true(json_object_get(field_json, "required"));
    return field;
}

int
free_scheme(scheme_t **scheme)
{
    if (*scheme == NULL)
        return 0;

    if ((*((scheme_t **)scheme))->fields != NULL)
        free_hash_storage(&(*((scheme_t **)scheme))->fields);

    free(*scheme);

    *scheme = NULL;

    return 0;
}

int
free_field(void **field)
{
    if (*field == NULL)
        return 0;

    if ((*((scheme_field_t **)field))->name != NULL) {
        free((*((scheme_field_t **)field))->name);
        (*((scheme_field_t **)field))->name = NULL;
    }

    if ((*((scheme_field_t **)field))->items != NULL) {
        void *scheme_field = (void *)(*((scheme_field_t **)field))->items;
        free_field(scheme_field);
        (*((scheme_field_t **)field))->items;
    }

    (*((scheme_field_t **)field))->required = NULL;

    free(*field);

    *field = NULL;

    return 0;
}

scheme_field_t *
get_schema_field(scheme_t *scheme, char *field_name)
{
    if (scheme == NULL)
        return NULL;

    if (scheme->fields == NULL)
        return NULL;

    scheme_field_t *field_el = look_hash_value(scheme->fields, field_name);

    return field_el;
}

char **
get_scheme_field_names(scheme_t *scheme)
{
    char **schema_fields;
    schema_fields = get_hashmap_keys(scheme->fields);

    return schema_fields;
}

int
validate_vals_by_scheme(scheme_t *scheme, hashmap_storage_t *vals, validation_error_t *error)
{
    char **value_keys;
    char **schema_fields;
    size_t fieds_num;
    scheme_field_t *scheme_field;
    hashmap_element_t *val_el;
    int valid_val;

    value_keys = get_hashmap_keys(vals);
    schema_fields = get_scheme_field_names(scheme);

    fieds_num = sizeof(schema_fields) / sizeof(schema_fields[0]);

    size_t i;
    for (i = 0; i < fieds_num; i++) {
        valid_val = 0;
        char *field_name = schema_fields[i];

        val_el = look_hash_value(value_keys, field_name);
        scheme_field = get_schema_field(scheme, field_name);

        if (val_el == NULL | val_el->value == NULL) {
            if (scheme_field->required) {
                error->field = strdup(field_name);
                error->text = strdup("Field is required");
                return 1;
            }
        }
        else if (strcmp((const char *)scheme_field->type, "array") == 0) {
            valid_val =
                check_field_type((const char *)scheme_field->items->type, val_el->value);
            /* code */
        }
        else {
            valid_val = check_field_type((const char *)scheme_field->type, val_el->value);
        }

        if (valid_val != 0) {
            error->field = strdup(field_name);
            error->text = strdup("Wrong field type");
        }
    }
}
