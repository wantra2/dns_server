cmake_minimum_required(VERSION 3.16)
project(dns_server C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Wextra -pedantic)

add_executable(dns_server main.c parse_request/parse_request.c parse_request/parse_request.h server/server.c server/server.h parse_zonefile/zone.c parse_zonefile/zone.h)

