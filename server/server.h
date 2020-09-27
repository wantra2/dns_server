
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

#include "serv_helpers.h"
#include "../parse_zonefile/zone.h"
#include "../make_response/make_response.h"
#define MAX_CONNECTIONS 20
#define UDP_MAX_PAYLOAD 512

int fds_init(fd_set* readfds, int sockfd, int udpfd, int* fd_clients,
             int nb_clients);

int handle_connection(int sockfd, fd_set* readfds, int* fd_clients, int nb_clients);

int loop_clients(fd_set* readfds, int* fd_clients, int nb_clients);

int tcp_rec_wrapper(int fd, char* buf, size_t bufsize);

int udp_rec_wrapper(struct sockaddr* addr, char* buf, struct record_list *records);

int tcp_send_resp(int fd, char* buf, size_t bufsize);

int udp_send_resp(struct sockaddr* addr, char* buf, size_t bufsize);

#endif //DNS_SERVER_SERVER_H
