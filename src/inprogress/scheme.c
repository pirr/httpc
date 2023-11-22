#include <stdbool.h>

#include "scheme.h"

#define DEFAULT_SIZE 8

int
parse_scheme(scheme_t *scheme, char *scheme_str, size_t scheme_scopes)
{
    hashmap_storage_t *scheme_hash = init_hashmap(NULL);

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