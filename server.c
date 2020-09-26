#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets

#include <netinet/in.h>

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

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

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return -1;
    }

    char *endptr;
    int port = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE || errno == EINVAL || port == 0)
    {
        fprintf(stderr, "Incorrect port given : %s\n", argv[1]);
        return -1;
    }
    int sockfd = prep_tcp(port);

    if (listen(sockfd, 1))
    {
        printf("listen\n");
        return -1;
    }

    int accepted = accept(sockfd, NULL, NULL);
    if (accepted == -1)
    {
        return -1;
    }
    char buf[1024];

    ssize_t test = 0;
    while( (test = read(accepted, &buf, 1023)) == 0)
    {
        continue;
    }
    buf[test] = 0;
    printf("%s", buf);

    close(sockfd);
    close(accepted);

    return 0;
}
