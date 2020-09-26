//
// Created by denis on 26/09/2020.
//

#ifndef DNS_SERVER_SERVER_H
#define DNS_SERVER_SERVER_H

#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets

#include <netinet/in.h>

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

int prep_tcp(int port);

#endif //DNS_SERVER_SERVER_H
