#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Buffer {
    char *content;
    int bytes_used;
    int total_size;
};

typedef struct Buffer Buffer;

Buffer *buffer_alloc(int size);
int buffer_append(Buffer *buf, char *append, int length);
void bufer_free(Buffer **);
