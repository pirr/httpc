#include <assert.h>
#include "../src/headers.h"

static char *test_headers_str = 
    "HTTP/1.1 200 OK\n"
    "Content-Type: application/json\n"
    "Date: Thu, 19 Feb 2022 12:27:04 GMT\n"
    "Server: API-C-SERVER\n"
    "Last-Modified: Wed, 18 Jun 2021 16:05:58 GMT\n"
    "Content-Length: 15\n"
    "Accept-Ranges: bytes\n"
    "Connection: close\n"
    "\n"
    "body"
    ;

static char *expected_headers[][2] = {
    {"Content-Type", "application/json"},
    {"Date", "Thu, 19 Feb 2022 12:27:04 GMT"},
    {"Server", "API-C-SERVER"},
    {"Last-Modified", "Wed, 18 Jun 2021 16:05:58 GMT"},
    {"Content-Length", "15"},
    {"Accept-Ranges", "bytes"},
    {"Connection", "close"},
};

int
main()
{
    header_t *result_headers;
    int count, i;
    count = sizeof(expected_headers) / sizeof(expected_headers[0]);
    result_headers = parse_headers(test_headers_str);
    for (i = 0; result_headers != NULL || i < count; i++, result_headers = result_headers->next) {
        printf("result name = %s, expected name = %s\n", result_headers->name->content, expected_headers[i][0]);
        assert(strcmp(result_headers->name->content, expected_headers[i][0]) == 0);
        printf("result value = %s, expected value = %s\n", result_headers->value->content, expected_headers[i][1]);
        assert(strcmp(result_headers->value->content, expected_headers[i][1]) == 0);
    }
    printf("free headers\n");
    free_headers(&result_headers);
    assert(result_headers == NULL);
}
