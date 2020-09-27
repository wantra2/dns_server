#include <stdio.h>
#include "parse_request/parse_request.h"
#include "server/server.h"
#include "parse_zonefile/zone.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Need a port/zone file\n");
        return -1;
    }

    struct record_list *records = create_record_list(argv[2]);
    if (zonefile_error != 0)
    {
        printf("Erreur dans le fichier de zone, le serveur ne peut demarrer\n");
        free_list(records);
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
    udpfd = fd_save(udpfd);

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
            char* buf = malloc(UDP_MAX_PAYLOAD);
            if (buf == NULL)
            {
                fprintf(stderr, "malloc failed\n");
                return -1;
            }

            struct sockaddr_storage addr;

            socklen_t addrlen = sizeof(struct sockaddr_storage);

            int i = 0;
            ssize_t sz = 0;
            int tmp_add = 0;
            while ((tmp_add = recvfrom(udpfd, buf + i * UDP_MAX_PAYLOAD,
                                      UDP_MAX_PAYLOAD, MSG_DONTWAIT,
                                       (struct sockaddr*)&addr, &addrlen))
                   == UDP_MAX_PAYLOAD)
            {
                if (tmp_add < 0)
                {
                    perror(NULL);
                    fprintf(stderr, "UDP read failed\n");
                    return -1;
                }
                sz += tmp_add;
                i++;
                if ((buf = realloc(buf, sz + UDP_MAX_PAYLOAD)) == NULL)
                {
                fprintf(stderr, "malloc failed\n");
                return -1;
                }
            }
            sz += tmp_add;
            udp_rec_wrapper((struct sockaddr*)&addr, buf, records);
            free(buf);
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
    free_list(records);
    return 0;
}
