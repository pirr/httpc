#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* ssize_t */
#include <string.h>
#include <ctype.h>

#include "buffer.h"

typedef struct header Header;

struct header {
    Buffer *name;
    Buffer *value;
    Header *next;
};

Header *parse_headers(char *);
Header *get_header_by_name(Header *, char *);
void free_headers(Header *);
