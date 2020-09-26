#include "serv_helpers.h"

int prep_tcp(int port)
{

    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "socket tcp failed\n");
        return -1;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)))
    {
        fprintf(stderr, "Setsockopt tcp failed\n");
        return -1;
    }

    struct sockaddr_in6 addr;

    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = in6addr_any;
    addr.sin6_flowinfo = 0;
    addr.sin6_scope_id = 0;
    if (bind(sockfd, (struct sockaddr*)&addr,sizeof(struct sockaddr_in6)))
    {
        fprintf(stderr, "bind tcp failed\n");
        return -1;
    }

    return sockfd;
}

int prep_udp(int port)
{

    int udpfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udpfd == -1)
    {
        fprintf(stderr, "socket udp failed\n");
        return -1;
    }

    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = in6addr_any;
    addr.sin6_flowinfo = 0;
    addr.sin6_scope_id = 0;
    if (bind(udpfd, (struct sockaddr*)&addr,sizeof(struct sockaddr_in6)))
    {
        printf("udp bind error\n");
        return -1;
    }

    return udpfd;
}

int tmax(int a, int b, int c)
{
    if (a > b)
    {
        if (a > c)
            return a;
        if (c > b)
            return c;
    }
    if (c > b)
        return c;
    return b;
}
