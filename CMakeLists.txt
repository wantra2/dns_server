cmake_minimum_required(VERSION 3.16)
project(dns_server C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Werror -Wextra -pedantic)

add_executable(dns_server main.c server/server.c server/server.h parse_request/parse_request.c parse_request/parse_request.h parse_filezone/zone.h parse_filezone/zone.c parser_options/opt.h parser_options/opt.c)
