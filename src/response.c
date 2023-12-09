#include "response.h"

// static char *reply_headers =
// // "HTTP/1.1 200 OK\n"
// // "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
// "Server: Apache/2.2.3\n"
// // "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
// "Content-Type: application/json\n"
// // "Content-Length: 15\n"
// "Accept-Ranges: bytes\n"
// "Connection: close\n";

int
free_response(response_t **resp_pp)
{
    if (*resp_pp == NULL)
        return 0;

    if ((*resp_pp)->header)
        free_headers(&((*resp_pp)->header));

    if ((*resp_pp)->content)
        bufer_free(&((*resp_pp)->content));

    free(*resp_pp);

    return 0;
}

response_t *
make_response(header_t *header, const char *content)
{
    response_t *resp;
    header_t *content_length_header;
    int content_len, tmp_content_len, size_content_len;
    char *content_len_str;

    resp = (response_t *)malloc(sizeof(response_t));

    content_len = strlen(content) + 3;

    resp->content = buffer_alloc(content_len);
    buffer_append(resp->content, (char *)content, content_len);

    for (size_content_len = 1, tmp_content_len = content_len; tmp_content_len > 0;
         tmp_content_len /= 10, size_content_len++) {
    }
    content_len_str = (char *)malloc(sizeof(char) * size_content_len);
    sprintf(content_len_str, "%d", content_len);

    content_length_header = make_header("Content-Length", content_len_str);
    merge_headers(header, content_length_header);

    resp->header = header;
    resp->size = content_len;

    return resp;
}

buffer_t *
response_to_buff_message(response_t *response)
{
    buffer_t *header_buff, *message_buff;
    header_buff = header_to_buffer(response->header);
    message_buff = buffer_alloc(response->content->bytes_used + header_buff->bytes_used + 128);
    buffer_append(message_buff, header_buff->content, header_buff->bytes_used);
    buffer_append(message_buff, "\r\n\n", 3);
    buffer_append(message_buff, response->content->content, response->content->bytes_used);

    return message_buff;
}
