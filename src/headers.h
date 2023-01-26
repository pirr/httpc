#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* ssize_t */
#include <string.h>
#include <ctype.h>

#include "buffer.h"

typedef struct header_s header_t;

#ifndef HEADERS_H
#define HEADERS_H
struct header_s {
    Buffer *name;
    Buffer *value;
    header_t *next;
};
#endif

header_t *parse_headers(char *);
header_t *get_header_by_name(header_t *, char *);
int free_headers(header_t **);
