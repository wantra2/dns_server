#include "server.h"

int prep_tcp(int port)
{

    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)))

    {
        printf("setsockopt\n");
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
        printf("bind\n");
        return -1;
    }

    return sockfd;
}


