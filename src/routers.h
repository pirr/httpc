#include "response.h"
#include "request.h"
#include "hashmap.h"

#define ROUTERS_COUNT 16

#ifndef ROUTERS_H
#define ROUTERS_H
typedef struct router_s router_t;
typedef struct in_model_s in_model_t;

typedef enum {
    OPTIONS,
    HEAD,
    GET,
    POST,
    PATCH,
    PUT,
    DELETE
} HTTP_METHOD;

struct router_s {
    char *path;
    void *(*deserialize_request_body_func)(char *);
    response_t *(*router_func)(request_t *);
    HTTP_METHOD http_method;
};
#endif

router_t *add_router(char *, HTTP_METHOD, void *(*)(char *), response_t *(*)(request_t *));
router_t *get_router_by_path(char *);
int free_all_routers();
