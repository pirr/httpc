#include "server.h"
#define UNUSED(x) (void)(x)

struct sockaddr_in addr;
int respnse_additional_bytes = 32;

void
free_write_req(uv_write_t *req)
{
    write_req_t *wr = (write_req_t *)req;
    free(wr->buf.base);
    free(wr);
}

void
write_response(uv_write_t *req, int status)
{
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free_write_req(req);
}

void
on_close(uv_handle_t *handle)
{
    free(handle);
}

uv_read_cb
read_connection(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
        router_t *router;
        response_t *response;
        buffer_t *response_buff;
        request_t *request;
        url_t *parsed_url;
        char *resp_str;

        write_req_t *req = (write_req_t *) malloc(sizeof(write_req_t));
        printf("Data:\n %s\n", (char *) buf->base);

        if ((parsed_url = parse_url(buf->base, buf->len)) == NULL) {
            uv_close((uv_handle_t *)client, on_close);
            return NULL;
        }

        char *path_str = buffer_to_string(parsed_url->path);
        if ((router = get_router_by_path(path_str)) == NULL) {
            // TODO 404 here
            printf("Path Not found: %s\n", path_str);
            uv_close((uv_handle_t *)client, on_close);
            free(path_str);
            return NULL;
        }

        request = deserialize_request(buf->base, router->deserialize_request_body_func);
        request->url = parsed_url;
        response = router->router_func(request);
        response_buff = response_to_buff_message(response);
        resp_str = (char *) malloc(response_buff->bytes_used + respnse_additional_bytes);
        sprintf(resp_str, "HTTP/1.1 200\n%s", buffer_to_string(response_buff));
        req->buf = uv_buf_init(resp_str, response_buff->bytes_used + respnse_additional_bytes);
        uv_write((uv_write_t *)req, client, &req->buf, 1, write_response);
        free(path_str);
    }
    uv_close((uv_handle_t *)client, on_close);
    return NULL;
}

void
alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    UNUSED(handle);
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void
on_new_connection(uv_stream_t *server, int status)
{
    uv_tcp_t *client;

    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    printf("New connection\n");

    client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

    uv_tcp_init(server->loop, client);
    if (uv_accept(server, (uv_stream_t *)client) == 0) {
        printf("Read connection\n");
        uv_read_start((uv_stream_t *)client, alloc_buffer, (uv_read_cb)read_connection);
    }
    else {
        printf("Close connection\n");
        uv_close((uv_handle_t *)client, on_close);
    }
}

int
start_server(uv_loop_t *loop)
{
    int r;
    uv_tcp_t server;
    char *httpc_host;
    char *httpc_port_s;
    unsigned httpc_port;

    if (loop == NULL)
        loop = uv_default_loop();
    uv_tcp_init(loop, &server);

    if ((httpc_host = getenv("HTTPC_HOST")) == NULL)
        httpc_host = DEFAULT_HOST;

    if ((httpc_port_s = getenv("HTTPC_PORT")) == NULL)
        httpc_port = DEFAULT_PORT;
    else
        httpc_port = atoi(httpc_port_s);

    uv_ip4_addr(httpc_host, httpc_port, &addr);
    uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
    r = uv_listen((uv_stream_t *)&server, DEFAULT_BACKLOG, on_new_connection);

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    fprintf(stdout, "listening on adress %s:%d\n", httpc_host, httpc_port);
    return uv_run(loop, UV_RUN_DEFAULT);
}
