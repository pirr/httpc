#include <assert.h>

#include "../src/url.h"
#include "../src/hashmap.h"


int
main()
{
    url_t *parsed_url1, *parsed_url2;
    hashmap_element_t *el;
    char *expected_keys[3] = {"key1", "key2", "key3"};
    char *expected_values[3] = {"value1", "value2,value3,value4", "value5"};

    char *query_string1 = "/test?key1=value1&key2=value2,value3,value4&key3=value5";
    parsed_url1 = parse_url(query_string1, strlen(query_string1) * sizeof(char));
    assert(strcmp(buffer_to_string(parsed_url1->path), "/test") == 0);

    size_t i;
    for (i = 0; i < sizeof(expected_keys) / sizeof(expected_keys[0]); i++) {
        el = look_hash_el(parsed_url1->params, expected_keys[i]);
        assert(strcmp((char *) el->value->v, expected_values[i]) == 0);
    }

    char *query_string2 = "/test?key1=";
    parsed_url2 = parse_url(query_string2, strlen(query_string1));
    assert(strcmp(buffer_to_string(parsed_url2->path), "/test") == 0);

    el = look_hash_el(parsed_url2->params, "key1");
    assert(el == NULL);

}
