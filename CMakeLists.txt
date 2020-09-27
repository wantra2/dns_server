cmake_minimum_required(VERSION 3.16)
project(dns_server C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_INSTALL_PREFIX /usr/sbin)
add_compile_options(-Wall -Wextra -pedantic -g)

add_executable(dns_server main.c
  parse_request/parse_request.c parse_request/parse_request.h
  server/server.c server/server.h
  server/serv_helpers.c server/serv_helpers.h
  make_response/make_response.c make_response/make_response.h
  parse_zonefile/zone.c parse_zonefile/zone.h)

install(TARGETS dns_server DESTINATION ${CMAKE_INSTALL_PREFIX})
