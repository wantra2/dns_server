#include "server.h"

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

int handle_connection(int sockfd, fd_set* readfds, int* fd_clients,
                      int nb_clients)
{
    struct sockaddr_in6 addr;

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
                         host, sizeof(host),
                         port, sizeof(port),
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

        ssize_t sz = 0;
        char len[2] = {0};
        sz = read(fd_clients[i], &len, 2);
        swap(len, len+1);
        short bufsize = (short)*len;
        if (sz == -1)
        {
            fprintf(stderr, "TCP, reading size failed\n");
            return -1;
        }

        char* buf = malloc(bufsize);

        sz = read(fd_clients[i], buf, bufsize);
        if (sz == -1)
        {
            fprintf(stderr, "TCP, reading query failed\n");
            free(buf);
            return -1;
        }

        tcp_rec_wrapper(fd_clients[i], buf, sz);
        free(buf);

        if (!sz) {
            close(fd_clients[i]);
            nb_clients--;
            fd_clients[i] = 0;
        }
    }
    return nb_clients;
}

int tcp_rec_wrapper(int fd, char* buf, size_t bufsize)
{
    //CALL_FUNCTION

    buf[bufsize] = 0; // Temporary
    printf("%s\n", buf);
    return 0 * fd * *buf * bufsize; // To avoid unused variable warning
}

int udp_rec_wrapper(char* buf, size_t bufsize)
{
    //CALL FUNCTION

    buf[bufsize] = 0; // Temporary
    printf("%s", buf);
    return 0 * *buf * bufsize; // To avoid unused variable warning

}
