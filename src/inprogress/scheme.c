#include <stdbool.h>

#include "scheme.h"

#define DEFAULT_SIZE 8

int
parse_scheme(scheme_t *scheme, char *scheme_str, size_t scheme_scopes)
{
    hashmap_storage_t *scheme_hash = init_hashmap(0);

    buffer_t *scheme_key;
    buffer_t *scheme_value;

    char *scheme_str_o = scheme_str;
    bool *is_value = false;
    bool *found = false;

    for (; *scheme_str_o != '\0'; ++scheme_str_o) {
        if (*scheme_str_o == '{') {
            scheme_scopes += 1;
            scheme_hash = parse_scheme(scheme_str_o, scheme_scopes);
        }

        else if (*scheme_str_o == ' ') {
            continue;
        }

        else if (*scheme_str_o == '}') {
            scheme_scopes -= 1;
            return scheme_hash;
        }

        else if (*scheme_str_o == ':') {
            is_value = true;
            continue;
        }

        else if (*scheme_str_o == ',') {
            add_hash_el(scheme_hash, buffer_to_string(scheme_key),
                        buffer_to_string(scheme_value), scheme_value->bytes_used, free);
            continue;
        }

        if (is_value) {
            if (scheme_value == NULL)
                scheme_value = buffer_alloc(DEFAULT_SIZE);
            buffer_append(scheme_value, *scheme_str_o, 1);
        }

        if (scheme_key == NULL)
            scheme_key = buffer_alloc(DEFAULT_SIZE);

        buffer_append(scheme_key, *scheme_str_o, 1);
    }

    return scheme_hash;
}

int
free_scheme(void **scheme)
{
    if (*scheme == NULL)
        return 0;

    if ((*((scheme_t **)scheme))->fields != NULL)
        free_hash_storage(&(*((scheme_t **)scheme))->fields);

    free(scheme);

    scheme = NULL;

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
        free_field(&((*((scheme_field_t **)field))->items));
        (*((scheme_field_t **)field))->items;
    }

    (*((scheme_field_t **)field))->required = NULL;
    (*((scheme_field_t **)field))->type = NULL;

    free(*field);

    *field = NULL;

    return 0;
}
