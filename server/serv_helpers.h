#ifndef DNS_SERVER_SERVER_HELPERS_H
#define DNS_SERVER_SERVER_HELPERS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int fd_save(int fd);

void free_all(int sockfd, int udpfd, fd_set* readfds, int* fd_clients);

int prep_tcp(int port);

int prep_udp(int port);

int tmax(int a, int b, int c);

void swap(char* a, char* b);

int resp_tcp(int fd, char* buf, size_t bufsize);

int resp_udp();

#endif //DNS_SERVER_SERVER_HELPERS_H
