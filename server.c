#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets

#include <netinet/in.h>

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

#include <sys/select.h>

#define MAX_CONNECTIONS 20

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

int prep_udp(int port)
{

    int udpfd = socket(AF_INET6, SOCK_DGRAM, 0);
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
    int accepted;
    if ((accepted = accept(sockfd, NULL, NULL)) == -1)
    {
        fprintf(stderr, "Accept failed\n");
        return -1;
    }
    if (nb_clients >= MAX_CONNECTIONS)
    {
        fprintf(stderr, "Too many open connections. Dropping this one..\n");
        if (shutdown(accepted, SHUT_RDWR) < 0)
        {
            fprintf(stderr, "Shutdown failed\n");
        }
        close(accepted);
        return nb_clients;
    }

    printf("New Connection\n");
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


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Need a port\n");
        return -1;
    }

    char *endptr;
    int port = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE || errno == EINVAL || port == 0)
    {
        fprintf(stderr, "Incorrect port given : %s\n", argv[1]);
        return -1;
    }

    int sockfd = prep_tcp(port); // Prepares and binds the TCP socket for IPv4/6

    int udpfd = prep_udp(port); // Prepares and binds the UDP socket for IPv4/6

    if (listen(sockfd, MAX_CONNECTIONS)!=0)
    {
        fprintf(stderr, "Listen failed\n");
        return -1;
    }


    int* fd_clients = calloc(MAX_CONNECTIONS, sizeof(int));
    fd_set* readfds = malloc(sizeof(fd_set));
    int fd_max = sockfd;
    int nb_clients = 0;

    while (1)
    {

        fd_max = fds_init(readfds, sockfd, udpfd, fd_clients, nb_clients);

        if (select(fd_max + 1, readfds, NULL, NULL, NULL) == -1)
        {
            fprintf(stderr, "Select failed\n");
            return -1;
        }

        if (FD_ISSET(sockfd, readfds))
        {
            nb_clients = handle_connection(sockfd, readfds, fd_clients,
                                           nb_clients);
            if (nb_clients == -1)
            {
                return -1;
            }
        }

        if (FD_ISSET(udpfd, readfds))
        {
            char buf[1024];
            ssize_t sz = 0;

            while ((sz = read(udpfd, &buf, 1023)) != 0)
            {
                buf[sz] = 0;
                printf("%s", buf);
                if (buf[sz-1] == '\n')
                    break;
            }
        }
        else
        {
            nb_clients = loop_clients(readfds, fd_clients, nb_clients);
        }
    }

    free(fd_clients);
    free(readfds);
    close(udpfd);
    close(sockfd);

    return 0;
}
