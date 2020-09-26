#ifndef DNS_SERVER_SERVER_HELPERS_H
#define DNS_SERVER_SERVER_HELPERS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int prep_tcp(int port);

int prep_udp(int port);

int tmax(int a, int b, int c);


#endif //DNS_SERVER_SERVER_HELPERS_H
