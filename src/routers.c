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
    hashmap_value_t *value;

    router = (router_t *) malloc(sizeof(router_t));
    router->path = strdup(path);
    router->router_func = router_func;
    router->http_method = http_method;
    router->deserialize_request_body_func = deserialize_request_body_func;
    
    value = create_hash_el_value(router, sizeof(router_t), free_router);
    add_hash_el(__ROUTERS_MAP, path, value);

    return router;
}

router_t *
get_router_by_path(char *path)
{
    hashmap_element_t *router_el = NULL;
    router_t *router = NULL;
    router_el = look_hash_el(__ROUTERS_MAP, path);

    if (router_el != NULL)
        router = (router_t *) router_el->value->v;

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
