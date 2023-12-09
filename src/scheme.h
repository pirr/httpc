#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>
#include <ctype.h>

#include "hashmap.h"
#include "buffer.h"

typedef struct scheme_s scheme_t;
typedef struct scheme_field_s scheme_field_t;
typedef struct validation_error_s validation_error_t;

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

struct validation_error_s {
    char *field;
    char *text;
};

enum VALIDATION_ERR {
    VALID_OK,
    WRONG_FIELD_TYPE,
    UNKNOWN_FIELD_TYPE,
    UNKNOWN_FIELD,
    FIELD_IS_REQUIRED,
    MAX = FIELD_IS_REQUIRED
};

scheme_t *
parse_scheme(char *);

scheme_field_t *
parse_field(json_t *, const char *);

int
free_field(void **);

int
free_scheme(scheme_t **);

scheme_field_t *
get_schema_field(scheme_t *, char *);

int
validate_vals_by_scheme(scheme_t *, hashmap_storage_t *, validation_error_t *);
