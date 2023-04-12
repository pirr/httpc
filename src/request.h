#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /* ssize_t */

#include "headers.h"
#include "buffer.h"
#include "hashmap.h"
#include "url.h"

#ifndef REQUEST_H
#define REQUEST_H
typedef struct request_s request_t;

struct request_s {
    header_t *header;
    url_t *url;
    void *body;
};
#endif

request_t *deserialize_request(char *, void *(*)(char *));
