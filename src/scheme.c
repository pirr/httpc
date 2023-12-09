#include <stdbool.h>
#include <errno.h>

#include "scheme.h"

#define DEFAULT_SCHEME_FIELDS_SIZE 8

const char *VALIDATION_ERR_TEXT[MAX + 1] = {"OK", "Wrong field type", "Unknown field type",
                                            "Unknown field", "Field is required"};

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
                return WRONG_FIELD_TYPE;
        }
        return 0;
    }
    else if (expected_field_type == NUMBER) {
        size_t i;
        if (!isdigit(*value))
            return WRONG_FIELD_TYPE;

        bool has_dot = false;

        for (i = 1; i < value_len & value[i] != '\0'; i++) {
            if (value[i] == '.') {
                if (has_dot)
                    return WRONG_FIELD_TYPE;
                else
                    has_dot = true;
            }
            else if (!isdigit(value[i])) {
                return WRONG_FIELD_TYPE;
            }
        }
        return OK;
    }
    else if (expected_field_type == STRING) {
        return OK;
    }

    else {
        return UNKNOWN_FIELD_TYPE;
    }
}

int
validate_vals_by_scheme(scheme_t *scheme, hashmap_storage_t *vals, validation_error_t *error)
{
    char **value_keys;
    char **schema_fields;
    scheme_field_t *scheme_field;
    hashmap_element_t *scheme_el;
    hashmap_element_t *val_el;
    int is_valid;
    char *field_name;

    value_keys = get_hashmap_keys(vals);
    schema_fields = get_scheme_field_names(scheme);

    size_t i;
    char *value_key;
    if (error->text != NULL)
        free(error->text);

    error->field = NULL;
    error->text = NULL;

    is_valid = 0;

    for (i = 0; i < vals->used_storage; i++) {
        field_name = value_keys[i];
        scheme_el = look_hash_el(scheme->fields, field_name);
        if (scheme_el == NULL) {
            is_valid = UNKNOWN_FIELD;
            break;
        }
    }

    for (i = 0; i < scheme->fields->used_storage & is_valid == 0; i++) {
        field_name = schema_fields[i];

        val_el = look_hash_el(vals, field_name);
        scheme_field = get_schema_field(scheme, field_name);

        if (val_el == NULL) {
            if (scheme_field->required) {
                is_valid = FIELD_IS_REQUIRED;
                break;
            }
        }
        else if (scheme_field->type == ARRAY) {
            size_t i;
            char **arr = (char **)(val_el->value->v);
            size_t arr_size = (val_el->value->s) / sizeof(arr);

            for (i = 0; i < arr_size; i++) {
                is_valid = check_field_type(scheme_field->items->type, arr[i], sizeof(arr));
                if (is_valid != 0)
                    break;
            }
        }
        else {
            is_valid = check_field_type(scheme_field->type, (const char *)val_el->value->v,
                                        val_el->value->s);
        }
    }

    if (is_valid != 0) {
        error->field = strdup(field_name);
        error->text = strdup(VALIDATION_ERR_TEXT[is_valid]);
    }

    return is_valid;
}
