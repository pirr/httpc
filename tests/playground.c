#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /* ssize_t */

typedef struct body_s body_s;
typedef struct router_s router_s;

struct router_s {
    void *(*func)(int i);
};

struct body_s {
    int i;
};

void *
make_response(int i)
{
    body_s *body;
    body = (body_s *) malloc(sizeof(body_s));
    body->i = i;
    return body;
}

body_s *
deserialize(int i)
{
    router_s *router;
    body_s *body;
    router = (router_s *) malloc(sizeof(router_s));
    router->func = make_response;

    body = router->func(i);

    return body;
}

int
main()
{
    int i;
    i = 22;

    body_s *body;
    char *p;

    body = deserialize(i);

    printf("Body value: %d\n", body->i);
    char *text = "Hello\n\n";

    p = strstr(text, "\n\n");
    for (i = 0; i < 2; i++) {
        if (*p == '\0' || *p == EOF)
            break;
        p++;
    }
    printf("String: %s", p);

}
