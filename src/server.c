#include "server.h"
#define UNUSED(x) (void)(x)

void
__free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void
__write(uv_write_t *req, int status)
{
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    __free_write_req(req);
}

void
__on_close(uv_handle_t *handle) {
    free(handle);
}

void
__read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
        write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);
        printf("Data: %s\n", (char *) req->buf.base);
        uv_write((uv_write_t*) req, client, &req->buf, 1, __write);
        return;
    }
}

void
__alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    UNUSED(handle);
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void
__on_new_connection(uv_stream_t *server, int status) 
{
    uv_tcp_t *client;

    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    printf("New connection\n");

    client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        printf("Read connection\n");
        uv_read_start((uv_stream_t*) client, __alloc_buffer, __read);
    }
    else {
        printf("Close connection\n");
        uv_close((uv_handle_t*) client, __on_close);
    }
}

int 
main()
{
    int r;
    uv_tcp_t server;
    char *httpc_host;
    char *httpc_port_s;
    unsigned httpc_port;

    loop = uv_default_loop();
    uv_tcp_init(loop, &server);

    if ((httpc_host = getenv("HTTPC_HOST")) == NULL)
        httpc_host = DEFAULT_HOST;

    if ((httpc_port_s = getenv("HTTPC_PORT")) == NULL)
        httpc_port = DEFAULT_PORT;
    else
        httpc_port = atoi(httpc_port_s);

    uv_ip4_addr(httpc_host, httpc_port, &addr);
    uv_tcp_bind(&server, (const struct sockaddr*) &addr, 0);
    r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, __on_new_connection);

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    fprintf(stdout, "listening on adress %s:%d\n", httpc_host, httpc_port);
    return uv_run(loop, UV_RUN_DEFAULT);
}
