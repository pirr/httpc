#include "request.h"

request_t *
deserialize_request(char *req, void *(*deserialize_func)(char *)) 
{
  header_t *headers;
  void *body_struct;
  char *body;
  request_t *request;
  int i;

  request = (request_t *) malloc(sizeof(request_t));

  if ((body = (strstr(req, "\r\n\r\n"))) == NULL)
    return NULL;

  for (i = 0; i < 2; i++) {
    if (*body == '\0' || *body == EOF)
      return NULL;
    body++;
  }

  if (deserialize_func != NULL) {
    if ((body_struct = deserialize_func(body)) == NULL)
      // TODO body's errors handling here
      return NULL;
  }

  if ((headers = parse_headers(req)) == NULL)
    // TODO headers' errors handling here
    return NULL;

  request->body = body_struct;
  request->header = headers;

  return request;
}
