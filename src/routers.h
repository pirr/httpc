#include "response.h"
#include "request.h"
#include "hashmap.h"

#define ROUTERS_COUNT 16

typedef struct router_s router_t;

typedef enum {
    OPTIONS,
    HEAD,
    GET,
    POST,
    PATCH,
    PUT,
    DELETE
} HTTP_METHOD;

struct router_s{
    char *path;
    response_t *(*router_func)(request_t *);
    HTTP_METHOD http_method;
};

router_t *add_router(char *, HTTP_METHOD, response_t *(*)(request_t *));
router_t *get_router_by_path(char *);
int free_all_routers();
