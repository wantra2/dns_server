#include <stdio.h>
#include "parse_request/parse_request.h"
#include "server/server.h"

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

    char *payload = buf + 2;

    dns_header header;
    dns_question question;
    parse_query(payload, &header, &question);
    close(sockfd);
    close(accepted);

    return 0;
}
