#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "url.h"
#include "response.h"
#include "routers.h"

#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128
#define DEFAULT_HOST "127.0.0.1"

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

int
start_server(uv_loop_t *);
