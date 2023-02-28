#include <assert.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "../src/routers.h"

static char *headers_str = 
    "HTTP/1.1 200 OK\n"
    "Content-Type: application/json\n"
    "Date: Thu, 19 Feb 2022 12:27:04 GMT\n"
    "Server: API-C-SERVER\n"
    "Last-Modified: Wed, 18 Jun 2021 16:05:58 GMT\n"
    // "Content-Length: 15\n"
    "Accept-Ranges: bytes\n"
    "Connection: close\n"
    "\n"
;

typedef struct model_s model_t;

struct model_s {
    char *str;
    int val;
};

model_t *
deserizlizer(char *data)
{
    model_t *model;
    model = (model_t *) malloc(sizeof(model_t));

    cJSON *root = cJSON_Parse(data);


    if (root == NULL) {
        printf("Error parsing JSON string: %s\n", cJSON_GetErrorPtr());
        return;
    }

    cJSON *str_json = cJSON_GetObjectItem(root, "str");
    cJSON *val_json = cJSON_GetObjectItem(root, "val");

    char *str = str_json->valuestring;
    int val = val_json->valueint;

    model->str = strdup(str);
    model->val = val;

    return model;
}

response_t *
test_router(request_t *req) 
{
    response_t *resp;
    header_t *header;
    model_t *req_data;
    char *content;
    cJSON *root;

    req_data = (model_t *) req->body;

    header = make_header("Content-Type", "application/json");
    content = "{\"str\": \"Hello user!\"}";

    resp = make_response(header, content);
    
    return resp;
}

size_t 
writeFunction(void *ptr, size_t size, size_t nmemb, void *buf) {
    buffer_append((buffer_t *) buf, (char*) ptr, (int) nmemb);
    return size * nmemb;
}

int
main()
{
    add_router("/test", GET, deserizlizer, test_router);

    // CURL *curl = curl_easy_init();
    // if (curl) {
    //     CURLcode res;
    //     curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:7000/test");
    //     curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    //     curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    //     curl_easy_setopt(curl, CURLOPT_READDATA, "{\"str\": \"hello httpc!\", \"val\": 1}");

    //     buffer_t *buf = buffer_alloc(1024);

    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
    //     curl_easy_setopt(curl, CURLOPT_HEADERDATA, headers_str);

    //     res = curl_easy_perform(curl);
    //     printf("CURL CODE: %d\n", res);
    //     assert(res == 200);
    //     assert(strcmp(buf->content, headers_str) == 0);
    //     curl_easy_cleanup(curl);
    //     curl_global_cleanup();
    //     curl = NULL;
    // }
}