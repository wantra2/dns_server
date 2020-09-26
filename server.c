#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets

#include <netinet/in.h>

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

int prep_tcp(int domain, int port)
{
    struct sockaddr_in addr;

    int sockfd = socket(domain, SOCK_STREAM, 0);

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)))

    {
        printf("setsockopt\n");
        return -1;
    }

    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)))
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
    int port = atoi(argv[1]);
    int sockfd = prep_tcp(AF_INET, port);

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
