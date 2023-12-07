#include <stdbool.h>
#include <errno.h>

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
check_field_type(FIELD_TYPE expected_field_type, const char *value, size_t value_len)
{
    if (expected_field_type == INTEGER) {
        size_t i;
        for (i = 0; i < value_len & value[i] != '\0'; i++) {
            if (!isdigit(value[i]))
                return 1;
        }
    }
    else if (expected_field_type == NUMBER) {
        size_t i;
        if (!isdigit(*value))
            return 1;

        bool has_dot = false;

        for (i = 1; i < value_len & value[i] != '\0'; i++) {
            if (value[i] == '.') {
                if (has_dot)
                    return 1;
                else
                    has_dot = true;
            }
            else if (!isdigit(value[i])) {
                return 1;
            }
        }
    }
    else if (expected_field_type == STRING) {
        return 0;
    }

    else {
        return 2;
    }
}

int
validate_vals_by_scheme(scheme_t *scheme, hashmap_storage_t *vals, validation_error_t *error)
{
    char **value_keys;
    char **schema_fields;
    size_t fieds_num;
    scheme_field_t *scheme_field;
    char *val_el;
    int is_valid;

    value_keys = get_hashmap_keys(vals);
    schema_fields = get_scheme_field_names(scheme);

    fieds_num = sizeof(schema_fields) / sizeof(schema_fields[0]);

    size_t i;
    for (i = 0; i < fieds_num; i++) {
        is_valid = 0;
        char *field_name = schema_fields[i];

        val_el = (char *)look_hash_value(vals, field_name);
        scheme_field = get_schema_field(scheme, field_name);

        if (val_el == NULL) {
            if (scheme_field->required) {
                error->field = strdup(field_name);
                error->text = strdup("Field is required");
                return 1;
            }
        }
        else if (scheme_field->type == ARRAY) {
            size_t i;

            size_t arr_size = sizeof(val_el) / sizeof(val_el[0]);
            for (i = 0; i < arr_size; i++) {
                is_valid =
                    check_field_type(scheme_field->items->type, val_el, sizeof(val_el[0]));
                if (is_valid != 0)
                    break;
            }
        }
        else {
            is_valid = check_field_type(scheme_field->type, (const char *)val_el,
                                        strlen((const char *)val_el));
        }

        if (is_valid == 1) {
            error->field = strdup(field_name);
            error->text = strdup("Wrong field type");
        }
        else if (is_valid == 2) {
            error->field = strdup(field_name);
            error->text = strdup("Unknown field type");
        }
    }

    return is_valid;
}
