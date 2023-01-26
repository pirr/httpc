#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/routers.h"


int
main()
{
    router_t *r1, *r2, *r1g, *r2g;
    char *r1_p = strdup("/example");
    char *r2_p = strdup("/example/other");

    r1 = add_router(r1_p, GET, NULL);
    r2 = add_router(r2_p, POST, NULL);

    assert(r1->http_method == GET);
    assert(strcmp(r1->path, r1_p) == 0);

    assert(r2->http_method == POST);
    assert(strcmp(r2->path, r2_p) == 0);

    r1g = get_router_by_path(r1_p);
    r2g = get_router_by_path(r2_p);

    assert(r1g->http_method == GET);
    assert(strcmp(r1g->path, r1_p) == 0);

    assert(r2g->http_method == POST);
    assert(strcmp(r2g->path, r2_p) == 0);

    free_all_routers();
}
