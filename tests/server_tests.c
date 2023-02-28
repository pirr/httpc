#include <assert.h>
#include <string.h>
#include <curl/curl.h>
// #include <cjson/cJSON.h>

#include "../src/server.h"
#include "../src/routers.h"

typedef struct model_s model_t;

struct model_s {
    char *str;
    int val;
};

void *
deserializer(char *data)
{
    model_t *model;
    // cJSON *root, *str_json, *val_json;
    model = (model_t *) malloc(sizeof(model_t));
    printf("Request data: %s\n", data);
    
    // root = cJSON_Parse(data);

    // if (root == NULL) {
    //     printf("Error parsing JSON string: %s\n", cJSON_GetErrorPtr());
    //     return NULL;
    // }

    // str_json = cJSON_GetObjectItemCaseSensitive(root, "str");
    // val_json = cJSON_GetObjectItemCaseSensitive(root, "val");

    // char *str = str_json->valuestring;
    // int val = val_json->valueint;

    model->str = strdup("Hello httpc!");
    model->val = 1;

    return model;
}

response_t *
test_router(request_t *req) 
{
    response_t *resp;
    header_t *header;
    model_t *req_data;
    char content[1024];

    printf("Test router\n");

    req_data = (model_t *) req->body;

    header = make_header("Content-Type", "application/json");
    sprintf(content, "{\"str\": \"Hello user!\", \"req_val\": %d}", req_data->val);
    printf("Test router content: %s\n",  content);
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
    add_router("/test", GET, deserializer, test_router);
    uv_loop_t *loop;
    loop = uv_default_loop();
    start_server(loop);

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
