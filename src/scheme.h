#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>

#include "hashmap.h"
#include "buffer.h"

typedef struct scheme_s scheme_t;
typedef struct scheme_field_s scheme_field_t;

typedef enum { STRING, NUMBER, INTEGER, ARRAY } FIELD_TYPE;

struct scheme_field_s {
    char *name;
    FIELD_TYPE type;
    scheme_field_t *items;
    bool required;
};

struct scheme_s {
    hashmap_storage_t *fields;
};

scheme_t *
parse_scheme(char *);

scheme_field_t *
parse_field(json_t *, char *);

int
free_field(void **);

int
free_scheme(scheme_t **);

scheme_field_t *
get_schema_field(scheme_t *, char *);
