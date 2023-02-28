#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "headers.h"
#include "buffer.h"

typedef struct response_s response_t;

#ifndef RESPONSE_H
#define RESPONSE_H
typedef enum {
    OK = 200,
    BAD_REQUEST = 422,
    SERVER_ERROR = 500
} STATUS_CODE;

struct response_s {
    header_t *header;
    buffer_t *content;
    size_t size;
    STATUS_CODE status;
};
#endif

response_t *make_response(header_t *, const char *);
buffer_t *response_to_json(response_t *);
