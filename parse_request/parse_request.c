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

void print_response(dns_response *dns_response)
{
    printf("%s \n", dns_response->name);
    printf("%d \n", dns_response->type);
    printf("%d \n", dns_response->class);
    printf("%d \n", dns_response->ttl);
    printf("%d \n", dns_response->rdlength);
    printf("%s \n", dns_response->rdata);
}

dns_header *init_dns_header()
{
    dns_header *header = calloc(1, sizeof(dns_header));
    if (header == NULL)
        return NULL;
    header->opcode = OPCODE_VALUE;
    header->aa = AA_VALUE;
    header->rd = RD_VALUE;
    header->ra = RA_VALUE;
    header->z = Z_VALUE;
    return header;
}
dns_question *init_dns_question()
{
    dns_question *question = calloc(1, sizeof(dns_question));
    return question;
}
dns_response *init_dns_response()
{
    dns_response *response = calloc(1, sizeof(dns_response));
    return response;
}

dns_pkt *init_dns_pkt(size_t size)
{
    dns_pkt *pkt = calloc(1, sizeof(dns_pkt));
    if (pkt == NULL)
        return NULL;
    pkt->data = calloc(size, sizeof(char));
    return pkt;
}

void parse_header(dns_pkt *pkt, void *data)
{
    dns_header *header = init_dns_header();
    memcpy(header, data, HEADER_LENGTH);
    header->id = ntohs(header->id);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);
    pkt->header = *header;
}

dns_question *parse_question(dns_pkt *pkt)
{
    dns_question *question = init_dns_question();
    question = (dns_question *)pkt->data;
    return question;
}


dns_pkt *parse_query(void *data, dns_question *question, size_t pkt_size)
{
    dns_pkt *pkt = init_dns_pkt(pkt_size);

    parse_header(pkt, data);
    memcpy(pkt->data, ((char *)data) + HEADER_LENGTH, pkt_size - HEADER_LENGTH);
    if (pkt->header.qdcount == 1)
        question = parse_question(pkt);
    print_question(question);
    return pkt;
}




