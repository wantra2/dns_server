#include <sys/types.h> // Sockets-ish
#include <sys/socket.h> // Sockets

#include <netdb.h> // addrinfo
#include <netinet/in.h>

#include <stdio.h> // printf

#include <stdlib.h> // malloc
#include <unistd.h> // Read

#include <errno.h>
#include <string.h>

int main(void)
{
    struct sockaddr_in addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)))

    {
        printf("setsockopt\n");
        return -1;
    }

    addr.sin_port = htons(6666);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        printf("bind\n");
        return -1;
    }

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
