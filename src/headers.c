#include "headers.h"

const char *headers_delimiters = "\n";
const char *header_delimiter = ":";

char *
ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *
rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char 
*trim(char *s)
{
    return rtrim(ltrim(s)); 
}

int
free_headers(header_t **root)
{
    if (*root == NULL)
        return 0;
    
    header_t *current, *tmp;
    current = *root;
    while (current->next != NULL) {
        tmp = current;
        current = current->next;
        bufer_free(&(tmp->name));
        bufer_free(&(tmp->value));
        free(tmp);
        tmp = NULL;
    }
    root = NULL;
    return 0;
}

header_t *
parse_headers(char *headers_string)
{
    header_t *root_header, *current_header;
    int header_name_len, header_val_len;
    char *st_headers_p, *headers_p, *header_str, *header_name, *header_value;

    root_header = (header_t *) malloc(sizeof(header_t));
    root_header->next = NULL;
    current_header = root_header;

    st_headers_p = strdup(headers_string);
    headers_p = st_headers_p;
    
    while((header_str = strtok_r(headers_p, headers_delimiters, &headers_p)) != NULL) {

        if (headers_p == NULL)
            break;
        
        if ((header_name = strtok_r(header_str, header_delimiter, &header_str)) == NULL)
            break;

        if (strcmp(header_name, "") == 0)
            continue;

        if (header_str == NULL)
            continue;

        if (strcmp(header_str, "") == 0)
            continue;

        current_header->next = (header_t *) malloc(sizeof(header_t));
        current_header->next->next = NULL;
        
        header_name = trim(header_name);
        header_name_len = strlen(header_name);
        header_value = trim(header_str);
        header_val_len = strlen(header_value);

        current_header->next->name = buffer_alloc(header_name_len);
        current_header->next->value = buffer_alloc(header_val_len);
        buffer_append(current_header->next->name, header_name, header_name_len);
        buffer_append(current_header->next->value, header_value, header_val_len);

        current_header = current_header->next;
    }

    current_header = root_header;
    root_header = root_header->next;
    
    free(st_headers_p);

    return root_header;
}

header_t *
make_header(char *name, char *value)
{
    int name_len, value_len; 
    header_t *header;
    header = (header_t *) malloc(sizeof(header_t));
    name_len = strlen(name);
    value_len = strlen(value);
    header->name = buffer_alloc(name_len);
    buffer_append(header->name, name, name_len);
    header->value = buffer_alloc(value_len);
    buffer_append(header->value, value, value_len);
    header->next = NULL;

    return header;
}

header_t *
merge_headers(header_t *target, header_t *source)
{
    if (target == NULL || source == NULL)
        return NULL;
    header_t *curr;
    curr = target;
    while (curr->next != NULL)
        curr = curr->next;

    curr->next = source;

    return target;
}

buffer_t *
header_to_buffer(header_t *header)
{
    buffer_t *header_buffer;
    header_t *curr;

    header_buffer = buffer_alloc(1024);

    curr = header;

    while(curr != NULL) {
        buffer_append(header_buffer, curr->name->content, curr->name->bytes_used);
        buffer_append(header_buffer, ": ", 2);
        buffer_append(header_buffer, curr->value->content, curr->value->bytes_used);
        buffer_append(header_buffer, "\n", 1);
        curr = curr->next;
    }

    buffer_append(header_buffer, "\n", 1);

    return header_buffer;
}
