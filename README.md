# httpc

httpc is framework for API calls. httpc uses [libuv](http://docs.libuv.org/en/v1.x/) library for server.

the project is currently under development.

## run:
docker-compose -f docker-compose-develop.yaml up --build --remove-orphans && docker-compose rm -fsv
$ curl --header "Content-Type: application/json" -d "{\"value\":\"Hello httpc\"}" http://localhost:7000/test

TODO:
1. Parser for request query parameters (GET).
2. Response as json?
3. Fix Makefile (for MacOS, Linux, Windows?)
