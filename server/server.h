

#ifndef DNS_SERVER_SERVER_H
#define DNS_SERVER_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

#include <err.h>
#include <errno.h>
#include <string.h>

#include <sys/select.h>

#include <netdb.h>

#include "serv_helpers.h"
#include "../parse_zonefile/zone.h"
#include "../make_response/make_response.h"

#define MAX_CONNECTIONS 20 // The maximum amount of concurrent TCP connections
#define UDP_MAX_PAYLOAD 512 // As defined in RFC 1035


// Initializes the set of fds, with sockfd and udpfd already in it
int fds_init(fd_set* readfds, int sockfd, int udpfd, int* fd_clients,
             int nb_clients);

// Handler for activity on sockfd, meaning a new tcp connection
int handle_connection(int sockfd, fd_set* readfds, int* fd_clients, int nb_clients);

// Main loop through the tcp fds calling the read functions on those who
// were updated
int loop_clients(fd_set* readfds, int* fd_clients, int nb_clients, struct record_list *records);

// Calls the dns logic from a dns query filled buffer trought tcp
int tcp_rec_wrapper(int fd, char* buf, struct record_list *records);

// Calls the dns logic from a dns query filled buffer trought udp
int udp_rec_wrapper(struct sockaddr* addr, char* buf, struct record_list *records);

// Sends the response contained in buf to fd via tcp
int tcp_send_resp(int fd, dns_packet *packet, size_t bufsize);

// Sends the response contained in buf to the address and port in addr via udp
int udp_send_resp(struct sockaddr* addr, dns_packet *packet, size_t bufsize);

#endif //DNS_SERVER_SERVER_H
