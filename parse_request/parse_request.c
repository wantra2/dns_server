//
// Created by denis on 26/09/2020.
//

#include "parse_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <netinet/in.h>

void print_header(dns_header *dns_header)
{
    printf("%d\n", dns_header->id);
    printf("%d\n", dns_header->qr);
    printf("%d\n", dns_header->opcode);
    printf("%d\n", dns_header->aa);
    printf("%d\n", dns_header->tc);
    printf("%d\n", dns_header->rd);
    printf("%d\n", dns_header->ra);
    printf("%d\n", dns_header->z);
    printf("%d\n", dns_header->rcode);
    printf("%d\n", dns_header->qdcount);
    printf("%d\n", dns_header->ancount);
    printf("%d\n", dns_header->nscount);
    printf("%d\n", dns_header->arcount);
}
void print_question(dns_question *dns_question)
{
    printf("%s \n", dns_question->qname);
    printf("%d \n", dns_question->qtype);
    printf("%d \n", dns_question->qclass);
}


dns_header *init_dns_header(void *data)
{
    dns_header *header = calloc(1, sizeof(dns_header));
    memcpy(header, data, sizeof(dns_header));
    header->id = ntohs(header->id);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);
    header->opcode = OPCODE_VALUE;
    header->aa = AA_VALUE;
    header->rd = RD_VALUE;
    header->ra = RA_VALUE;
    header->z = Z_VALUE;
    return header;
}

static char *sanitize(char *qname)
{
    int len = strlen(qname);
    char *ret = calloc(len, sizeof(char));
    for (int i = 1; i < len; ++i)
    {
        if (qname[i] < 45) //label
            ret[i-1] = '.';
        else
            ret[i-1] = qname[i];
    }
    return ret;
}

dns_question *init_dns_question(void *data)
{
    dns_question *question = calloc(1, sizeof(dns_question));
    question->qname = ((char *)data) + HEADER_LENGTH;
    question->qname = sanitize(question->qname);
    question->qtype = *((uint16_t *)(question->qname + strlen(question->qname) + 1));
    question->qclass = *((uint16_t *)(question->qname + strlen(question->qname) + 1 + 16));
    question->qtype = ntohs(question->qtype);
    question->qclass = ntohs(question->qclass);
    return question;
}

void parse_query(void *data, dns_header *dnsheader, dns_question *dnsquestion)
{
    dns_header *header = init_dns_header(data);
    dnsheader = header;
    if (header->qdcount == 0) {
        printf("NO QUESTION\n");
        return;
    }
    dns_question *question = init_dns_question(data);
    dnsquestion = question;
}




