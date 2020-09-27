

#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include "make_response.h"

void print_response(dns_response *dns_response)
{
    printf("%s \n", dns_response->name);
    printf("%d \n", dns_response->type);
    printf("%d \n", dns_response->class);
    printf("%d \n", dns_response->ttl);
    printf("%d \n", dns_response->rdlength);
    printf("%s \n", dns_response->rdata);
}

static char *convert(char *qname)
{
    char *buffer = calloc(1, 256 * sizeof(char));       /* MAX_NAME_SIZE = 255 */
    char length[3];

    int offset = 0;
    snprintf(length, 2, "%x", qname[offset]);
    strncpy(buffer + strlen(buffer), qname + offset + 1, atoi(length));
    offset += atoi(length) + 1;
    buffer[offset - 1] = '.';

    while (qname[offset] != '\x00')
    {
        snprintf(length, 3, "%x", qname[offset]);
        strncpy(buffer + strlen(buffer), qname + offset + 1, atoi(length));
        offset += atoi(length) + 1;
        buffer[offset -1] = '.';
    }

    buffer[offset -1] = '\0';
    return buffer;
}

dns_packet *make_response(dns_header *header, dns_question *question, struct record_list *records, size_t *size)
{
    struct record_list *first_rec = records;
    printf("%d\n", question->qtype);
    dns_packet *pkt = calloc(1, sizeof(dns_packet));
    pkt->header = *header;
    pkt->question = *question;

    pkt->header.id = htons(pkt->header.id);
    pkt->header.qdcount = htons(pkt->header.qdcount);
    pkt->header.ancount = htons(pkt->header.ancount);
    pkt->header.nscount = htons(pkt->header.nscount);
    pkt->header.arcount = htons(pkt->header.arcount);
    pkt->header.rcode = NOERROR;
    pkt->header.qr = 1;

    pkt->question.qtype = htons(pkt->question.qtype);
    pkt->question.qclass = htons(pkt->question.qclass);

    pkt->data = malloc(1);

    *size += sizeof(dns_header) + sizeof(dns_question);

    /*if (question->qtype != A && question->qtype != AAAA && question->qtype != CNAME
        && question->qtype != TXT && question->qtype != SOA)
    {
        pkt->header.rcode = NOTIMP;
        return pkt;
    }*/

    int found = 0;
    while (records)
    {
        if (question->qtype == records->node->type && !strcmp(pkt->question.qname, records->node->domain_name))
        {
            found++;
            pkt->data = realloc(pkt->data, found * sizeof(dns_response));
            *size += sizeof(dns_response);
            dns_response *tmp = (dns_response *)(pkt->data);
            tmp += found - 1;
            tmp->name = pkt->question.qname;
            tmp->type = htons(records->node->type);
            tmp->class = htons(pkt->question.qclass);
            tmp->ttl = htonl(records->node->ttl);
            tmp->rdata = records->node->content;
            tmp->rdlength = htons(strlen(records->node->content));
            pkt->header.ancount += 1;
        }
        records = records->next;
    }
    if (found == 0)
    {
        pkt->data = realloc(pkt->data, 1 * sizeof(dns_response));
        *size += sizeof(dns_response);
        ((dns_response *)(pkt->data))->name = pkt->question.qname;
        ((dns_response *)(pkt->data))->type = htons(SOA);
        ((dns_response *)(pkt->data))->class = htons(pkt->question.qclass);
        ((dns_response *)(pkt->data))->ttl = htonl(first_rec->node->ttl);
        ((dns_response *)(pkt->data))->rdata = first_rec->node->content;
        ((dns_response *)(pkt->data))->rdlength = htons(strlen(first_rec->node->content));
        pkt->header.nscount += 1;
    }
    return pkt;
}
