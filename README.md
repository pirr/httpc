# httpc

httpc is framework for API calls. httpc uses [libuv](http://docs.libuv.org/en/v1.x/) library for server.

the project is currently under development.

## run:
1. docker build -f Dockerfile-gcc -t gcc_libs .

2. docker-compose -f docker-compose-develop.yaml up --build --remove-orphans && docker-compose rm -fsv

3. $ curl --header "Content-Type: application/json" http://localhost:7000/test_get\?var1\=1\&var2\=2

TODO:

1. ~~Parser for request query parameters (GET).~~

2. Response as json.

3. Fix Makefile (for MacOS, Linux, Windows?)
