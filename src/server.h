#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128
#define DEFAULT_HOST "0.0.0.0"

typedef struct {
    char *host;
    unsigned port;
} ServerConfig;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

uv_loop_t *loop;
struct sockaddr_in addr;

ServerConfig *init_config(char *, unsigned);
void start_server(ServerConfig *);

