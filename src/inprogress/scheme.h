#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../hashmap.h"
#include "../buffer.h"

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
init_scheme(char *);

scheme_t *
parse_scheme(scheme_t *scheme, char *scheme_str, size_t scheme_scopes);

int
free_field(void **);

int
free_scheme(void **);

field_t *
parse_field(hashmap_element_t *);
