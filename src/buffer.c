#include "buffer.h"


Buffer *
buffer_alloc(int size)
{
    Buffer *buf;
    if ((buf = (Buffer *) malloc(sizeof(Buffer))) == NULL) {
        fprintf(stderr, "Buffer allocation error");
        return NULL;
    }

    if ((buf->content = (char *) malloc((size + 1) * sizeof(char))) == NULL) {
        fprintf(stderr, "Buffer content allocation error");
        return NULL;
    }

    buf->bytes_used = 0;
    buf->total_size = size;

    return buf;
}

int
__buffer_expand(Buffer *buf, int new_size)
{
    if (buf->total_size - buf->bytes_used >= new_size) {
        return 0;
    }

    if ((buf->content = (char *) realloc(buf->content, new_size)) == NULL) {
        return -1;
    }

    buf->total_size = new_size;
    return 0;
}

int
buffer_append(Buffer *buf, char *append, int size)
{
    int allocated, additional;
    allocated = buf->total_size - buf->bytes_used;
    if (allocated < size) {
        additional = size - allocated;
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
bufer_free(Buffer **buf)
{
    free((*buf)->content);
    free(*buf);
    *buf = NULL;
}
