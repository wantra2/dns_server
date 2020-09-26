#include "server.h"

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

int fds_init(fd_set* readfds, int sockfd, int udpfd, int* fd_clients,
             int nb_clients)
// Returns the new found fd_max
{
    FD_ZERO(readfds);
    FD_SET(sockfd, readfds);
    FD_SET(udpfd, readfds);
    int fd_max = sockfd;

    for (int i = 0; fd_clients[i] != 0 && i < nb_clients; i++)
    {
        int client_fd = fd_clients[i];
        FD_SET(client_fd, readfds);
        if (client_fd > fd_max)
        {
            fd_max = client_fd;
        }
    }
    return tmax(fd_max, sockfd, udpfd);
}

int handle_connection(int sockfd, fd_set* readfds, int* fd_clients, int nb_clients)
{
    struct sockadrr_storage addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    int accepted;

    if ((accepted = accept(sockfd, (struct sockaddr*)&addr, &addrlen)) == -1)
    {
        fprintf(stderr, "Accept failed\n");
        return -1;
    }

    char host[NI_MAXHOST] = {0};
    char port[NI_MAXSERV] = {0};

    int name_ret = getnameinfo((struct sockaddr *)&addr, addrlen,
                         hoststr, sizeof(host),
                         portstr, sizeof(port),
                         NI_NUMERICHOST | NI_NUMERICSERV);
    if (name_ret != 0)
    {
        fprintf(stderr, "Getnameinfo failed\n");
    }
    printf("New connection from %s#%s\n", host, port);

    if (nb_clients >= MAX_CONNECTIONS)
    {
        fprintf(stderr, "Too many open connections. Dropping this one\n");
        if (shutdown(accepted, SHUT_RDWR) < 0)
        {
            fprintf(stderr, "Shutdown failed\n");
        }
        close(accepted);
        return nb_clients;
    }

    FD_SET(accepted, readfds);
    int i = 0;
    for (; i < nb_clients && fd_clients[i]; i++)
    {
        continue;
    }
    fd_clients[i] = accepted;
    return ++nb_clients;
}

int loop_clients(fd_set* readfds, int* fd_clients, int nb_clients)
{
    for (int i = 0; i < nb_clients; i++)
    {
        if (! FD_ISSET(fd_clients[i], readfds))
        {
            continue;
        }

        char buf[1024];
        ssize_t sz = 0;

        while ((sz = read(fd_clients[i], &buf, 1023)) != 0)
        {
            buf[sz] = 0;
            printf("%s", buf);
            if (buf[sz-1] == '\n')
                break;
        }
        if (!sz) {
            close(fd_clients[i]);
            nb_clients--;
            fd_clients[i] = 0;
        }
    }
    return nb_clients;
}
