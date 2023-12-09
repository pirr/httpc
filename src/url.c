#include "url.h"

void
set_query_params(hashmap_storage_t *, char *);

url_t *
parse_url(char *str, size_t size)
{
    char *ptr = strchr(str, '/');
    if (ptr != NULL) {
        char *newline_ptr = strchr(str, '\n');
        if (newline_ptr != NULL && ptr >= newline_ptr) {
            return NULL;
        }
        else {
            url_t *parsed_url;
            parsed_url = (url_t *)malloc(sizeof(url_t));
            char s[2];
            s[1] = '\0';
            parsed_url->path = buffer_alloc(128);
            size_t size_tmp = size;
            while (*ptr != EOF && isspace(*ptr) == 0 && *ptr != '?' && size_tmp-- > 0) {
                s[0] = *ptr++;
                buffer_append(parsed_url->path, s, 1);
            }
            if (*ptr == '?') {
                parsed_url->params = init_hashmap(DEFAULT_HASHSIZE);
                set_query_params(parsed_url->params, ++ptr);
            }
            return parsed_url;
        }
    }
    return NULL;
}

int
free_param(void **param)
{
    if (*param != NULL)
        free(*param);

    return 0;
}

void
set_query_params(hashmap_storage_t *params_storage, char *params)
{
    char *param, *param_name, *params_copy, *params_copy_p;

    params_copy = strdup(params);
    params_copy_p = params_copy;

    while ((param = strtok_r(params_copy_p, "&", &params_copy_p)) != NULL) {
        if ((param_name = strtok_r(param, "=", &param)) == NULL)
            break;

        param = strtok_r(param, " ", &param);

        if (strcmp(param_name, "") == 0)
            continue;

        if (param == NULL)
            continue;

        if (strcmp(param, "") == 0)
            continue;

        add_hash_el(params_storage, param_name, param, strlen(param) + 1, free_param);
    }
    free(params_copy);
}
