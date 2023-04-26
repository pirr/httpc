#include "routers.h"

hashmap_storage_t *__ROUTERS_MAP = NULL;

int
free_router(void **router)
{
    if (*router == NULL)
        return 0;

    free((*((router_t **) router))->path);
    (*((router_t **) router))->path = NULL;
    free(*router);
    *router = NULL;

    return 0;
}

router_t *
add_router(char *path, HTTP_METHOD http_method, void *(*deserialize_request_body_func)(char *), response_t *(*router_func)(request_t *)) 
{
    if (__ROUTERS_MAP == NULL)
        __ROUTERS_MAP = init_hashmap(ROUTERS_COUNT);
    
    router_t *router;

    router = (router_t *) malloc(sizeof(router_t));
    router->path = strdup(path);
    router->router_func = router_func;
    router->http_method = http_method;
    router->deserialize_request_body_func = deserialize_request_body_func;
    
    add_hash_el(__ROUTERS_MAP, path, router, sizeof(router_t), free_router);

    return router;
}

router_t *
get_router_by_path(char *path)
{
    router_t *router = NULL;
    router = look_hash_value(__ROUTERS_MAP, path);

    return router;
}

int
free_all_routers()
{
    if (__ROUTERS_MAP == NULL)
        return 0;
    
    free_hash_storage(&__ROUTERS_MAP);
    __ROUTERS_MAP = NULL;

    return 0;
}
