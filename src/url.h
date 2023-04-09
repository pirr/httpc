#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include "hashmap.h"
#include "buffer.h"

#ifndef URL_H
#define URL_H

typedef struct {
    buffer_t *path;
    hashmap_storage_t *params;
} url_t;

url_t *
parse_url(char *, size_t);

#endif
