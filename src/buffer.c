#include "buffer.h"

buffer_t *
buffer_alloc(int size)
{
    buffer_t *buf;
    if ((buf = (buffer_t *)malloc(sizeof(buffer_t))) == NULL) {
        fprintf(stderr, "Buffer allocation error");
        return NULL;
    }

    if ((buf->content = (char *)malloc((size + 1) * sizeof(char))) == NULL) {
        fprintf(stderr, "Buffer content allocation error");
        return NULL;
    }

    buf->bytes_used = 0;
    buf->total_size = size;

    return buf;
}

int
__buffer_expand(buffer_t *buf, int new_size)
{
    if (buf->total_size - buf->bytes_used >= new_size) {
        return 0;
    }

    if ((buf->content = (char *)realloc(buf->content, new_size)) == NULL) {
        return -1;
    }

    buf->total_size = new_size;
    return 0;
}

int
buffer_append(buffer_t *buf, char *append, int size)
{
    int allocated, additional;
    allocated = buf->total_size - buf->bytes_used;
    if (allocated < size) {
        additional = size - allocated + 1;
        if (__buffer_expand(buf, buf->total_size + additional) != 0) {
            return -1;
        }
    }
    while (*append != '\0') {
        buf->content[buf->bytes_used] = *append++;
        buf->bytes_used += 1;
    }

    buf->content[buf->bytes_used] = '\0';

    return 0;
}

void
bufer_free(buffer_t **buf)
{
    free((*buf)->content);
    free(*buf);
    *buf = NULL;
}

char *
buffer_to_string(buffer_t *buf)
{
    char *content;

    content = strdup(buf->content);

    return content;
}
