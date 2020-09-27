#include "server.h"
#include "../parse_request/parse_request.h"


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
        free_all(-1, -1, NULL, NULL);
        errx(-2, "Accept failed\n");
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

int loop_clients(fd_set* readfds, int* fd_clients, int nb_clients, struct record_list *records)
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
            free_all(-1, -1, NULL, NULL);
            errx(-2, "TCP, reading size failed\n");
            return -1;
        }

        char* buf = malloc(bufsize);

        sz = read(fd_clients[i], buf, bufsize);
        if (sz == -1)
        {
            free_all(-1, -1, NULL, NULL);
            free(buf);
            errx(-2, "TCP, reading query failed\n");
        }
        
        if (!sz) {
            shutdown(fd_clients[i], SHUT_RDWR);
            close(fd_clients[i]);
            nb_clients--;
            fd_clients[i] = 0;
            free(buf);
            continue;
        }
        tcp_rec_wrapper(fd_clients[i], buf, records);
        free(buf);
    }
    return nb_clients;
}

int tcp_rec_wrapper(int fd, char* buf, struct record_list *records)
{
    dns_header *dnsheader = NULL;
    dns_question *dnsquestion = NULL;
    parse_query(buf, &dnsheader, &dnsquestion);
    size_t size = 0;
    dns_packet *packet = make_response(dnsheader, dnsquestion, records, &size);
    tcp_send_resp(fd, packet, size);

    return 0;
}

int udp_rec_wrapper(struct sockaddr* addr, char* buf, struct record_list *records)
{
    //CALL FUNCTION

    // dns_query *query = init_dns_query(buf);
    // print_dns_query(query);
    dns_header *dnsheader = NULL;
    dns_question *dnsquestion = NULL;
    parse_query(buf, &dnsheader, &dnsquestion);
    // free(query);
    size_t size = 0;
    dns_packet *packet = make_response(dnsheader, dnsquestion, records, &size);
    udp_send_resp(addr, packet, size);
    return 0; // To avoid unused variable warning
}

int tcp_send_resp(int fd, dns_packet *packet, size_t bufsize)
{
    char *buf = calloc(bufsize, sizeof(char));;
    buf = strncpy(buf, (char *)&(packet->header), sizeof(dns_header));
    char *tmp = buf + sizeof(dns_header);
    tmp = strncpy(tmp, (char *)&(packet->question), sizeof(dns_question));
    tmp += sizeof(dns_question);
    tmp = strncpy(tmp, packet->data, bufsize - sizeof(dns_header) - sizeof(dns_question));
    if (buf)
        free(buf);
    if (send(fd, buf, bufsize, MSG_DONTWAIT) == -1)
    {
        fprintf(stderr, "TCP send failed\n");
        return -1;
    }
    return 0;
}

int udp_send_resp(struct sockaddr* addr, dns_packet *packet, size_t bufsize)
{
    int tmp_sent = 0;
    int tot_sent = 0;
    int left = bufsize - tot_sent;
    char *buf = calloc(bufsize, sizeof(char));;
    buf = strncpy(buf, (char *)&(packet->header), sizeof(dns_header));
    char *tmp = buf + sizeof(dns_header);
    tmp = strncpy(tmp, (char *)&(packet->question), sizeof(dns_question));
    tmp += sizeof(dns_question);
    tmp = strncpy(tmp, packet->data, bufsize - sizeof(dns_header) - sizeof(dns_question));
    while (left / UDP_MAX_PAYLOAD)
    {
        if ((tmp_sent = sendto(fd_save(-1), buf + tot_sent, UDP_MAX_PAYLOAD, MSG_DONTWAIT, addr,
                               sizeof(struct sockaddr_storage))) == -1)
        {
            free_all(-1, -1, NULL, NULL);
            free(buf);
            errx(-2, "UDP send failed\n");
        }
        tot_sent += tmp_sent;
        left = bufsize - tot_sent;
    }
    if (left != 0)
    {
        if ((tmp_sent = sendto(fd_save(-1), buf + tot_sent, left, MSG_DONTWAIT, addr,
                              sizeof(struct sockaddr_storage))) == -1)
        {
            free_all(-1, -1, NULL, NULL);
            free(buf);
            errx(-2, "UDP send failed\n");
        }
    }
    if (buf)
        free(buf);
    return 0;
}
