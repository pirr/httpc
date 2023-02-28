#include "server.h"
#define UNUSED(x) (void)(x)

struct sockaddr_in addr;

void
free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
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
on_close(uv_handle_t *handle) {
    free(handle);
}

buffer_t *
get_path_buff(char *header_str, size_t size)
{
    buffer_t *path_buff;
    char s[2];
    s[1] = '\0';

    char* ptr = strchr(header_str, '/');
    if (ptr != NULL) {
        char* newline_ptr = strchr(header_str, '\n');
        if (ptr >= newline_ptr) {
            return NULL;
        }
        else {
            path_buff = buffer_alloc(size / 5);
            while(*ptr != EOF && isspace(*ptr) == 0) {
                s[0] = *ptr++;
                buffer_append(path_buff, s, 1);
            }
        }
    }
    else {
        return NULL;
    }

    return path_buff;
}

uv_read_cb
read_connection(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
        router_t *router;
        response_t *response;
        request_t *request;
        buffer_t *path_buff;
        
        write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        printf("Data:\n %s\n", (char *) buf->base);
        if ( (path_buff = get_path_buff(buf->base, buf->len)) == NULL ) {
            uv_close((uv_handle_t*) client, on_close);
            return NULL;
        }
        
        char *path_str = buffer_to_string(path_buff);
        if ( (router = get_router_by_path(path_str)) == NULL ) {
            // TODO 404 here
            printf("Path Not found: %s\n", path_str);
            uv_close((uv_handle_t*) client, on_close);
            free(path_str);
            return NULL;
        }

        printf("Path found: %s\n", buffer_to_string(path_buff));
       
        free(path_str);
        
        request = deserialize_request(buf->base, router->deserialize_request_body_func);
        printf("Request deserialized\n");

        response = router->router_func(request);
        printf("Response prepared\n");
        
        // printf("Header to buff\n");
        // header_buff = header_to_buffer(header);
        // printf("Header: %s\n", header_buff->content);
        printf("Response: %s\n", response->content->content);
        req->buf = uv_buf_init(response->content->content, response->content->bytes_used);
        uv_write((uv_write_t*) req, client, &req->buf, 1, write_response);
    }
    uv_close((uv_handle_t*) client, on_close);
    return NULL;
}

void
alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    UNUSED(handle);
    buf->base = (char*) malloc(suggested_size);
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

    client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    
    uv_tcp_init(server->loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        printf("Read connection\n");
        uv_read_start((uv_stream_t*) client, alloc_buffer, (uv_read_cb) read_connection);
    }
    else {
        printf("Close connection\n");
        uv_close((uv_handle_t*) client, on_close);
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
    uv_tcp_bind(&server, (const struct sockaddr*) &addr, 0);
    r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    fprintf(stdout, "listening on adress %s:%d\n", httpc_host, httpc_port);
    return uv_run(loop, UV_RUN_DEFAULT);
}
