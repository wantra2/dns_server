#include <stdio.h>
#include "parse_request/parse_request.h"
#include "server/server.h"

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
            char buf[UDP_MAX_PAYLOAD];
            ssize_t sz = read(udpfd, &buf, UDP_MAX_PAYLOAD);
            if (sz < 0)
            {
                fprintf(stderr, "UDP read failed\n");
                return -1;
            }
            udp_rec_wrapper(buf, sz);
        }
        if (!FD_ISSET(udpfd, readfds) && !FD_ISSET(sockfd, readfds))
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
