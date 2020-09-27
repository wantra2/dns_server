#ifndef DNS_SERVER_SERVER_HELPERS_H
#define DNS_SERVER_SERVER_HELPERS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Saves a given fd, used for the main udp fd
int fd_save(int fd);

// Keeps a track on the opened fd and memory to free
void free_all(int sockfd, int udpfd, fd_set* readfds, int* fd_clients);

// Creates and binds a tcp socket
int prep_tcp(int port);

// Creates and binds a udp socket
int prep_udp(int port);

// Returns the max value between a, b and c
int tmax(int a, int b, int c);

void swap(char* a, char* b);

#endif //DNS_SERVER_SERVER_HELPERS_H
