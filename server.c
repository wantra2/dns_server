#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets
#include <netdb.h> // addrinfo

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

int main(void)
{
    struct addrinfo hints, *res = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

    int getaddr_ret = getaddrinfo(NULL, "6666", &hints, &res);
    if (getaddr_ret || res == NULL)
    {
        printf("getaddrinfo\n");
        return -1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
    {
        printf("NOK, %d\n", sockfd);
        return -1;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &optval, sizeof(optval)))

    {
        printf("setsockopt\n");
        return -1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        printf("bind\n");
        return -1;
    }
    freeaddrinfo(res);

    if (listen(sockfd, 3))
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
