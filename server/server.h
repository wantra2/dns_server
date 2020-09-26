
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

#include <sys/select.h>

#include <netdb.h>

#define MAX_CONNECTIONS 20
#define UDP_MAX_PAYLOAD 512

int prep_tcp(int port);

int prep_udp(int port);

int tmax(int a, int b, int c);

int fds_init(fd_set* readfds, int sockfd, int udpfd, int* fd_clients,
             int nb_clients);

int handle_connection(int sockfd, fd_set* readfds, int* fd_clients, int nb_clients);

int loop_clients(fd_set* readfds, int* fd_clients, int nb_clients);

#endif //DNS_SERVER_SERVER_H
