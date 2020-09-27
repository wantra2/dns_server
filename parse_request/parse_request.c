//
// Created by denis on 26/09/2020.
//

#include "parse_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <netinet/in.h>
#include <err.h>

void print_header(dns_header *dns_header)
{
    printf("id = %x\n", dns_header->id);
    printf("qr = %x\n", dns_header->qr);
    printf("opcode = %x\n", dns_header->opcode);
    printf("aa = %x\n", dns_header->aa);
    printf("tc = %x\n", dns_header->tc);
    printf("rd = %x\n", dns_header->rd);
    printf("ra = %x\n", dns_header->ra);
    printf("z = %x\n", dns_header->z);
    printf("ad = %x\n", dns_header->ad);
    printf("cd = %x\n", dns_header->cd);
    printf("rcode = %x\n", dns_header->rcode);
    printf("qdcount = %x\n", dns_header->qdcount);
    printf("ancount = %x\n", dns_header->ancount);
    printf("nscount = %x\n", dns_header->nscount);
    printf("arcount = %x\n", dns_header->arcount);
}

void print_question(dns_question *dns_question)
{
    printf("%s \n", dns_question->qname);
    printf("%d \n", dns_question->qtype);
    printf("%d \n", dns_question->qclass);
}

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

dns_header *init_dns_header(char *data)
{
    dns_header *header = malloc(sizeof header);

    char *tmp_data = data;
    char *tmp_header = (char*)header;
    
    u_int16_t *id = (u_int16_t*)tmp_data;
    memcpy(tmp_header, (void*)id, 2);
    header->id = ntohs(header->id);
    tmp_data += 2;
    tmp_header += 2;

    u_int16_t *tmp_byte_ptr = (uint16_t*) tmp_data;
    uint16_t tmp_byte = *tmp_byte_ptr;
    tmp_byte = ntohs(tmp_byte);
    header->qr = MID(tmp_byte,15, 16);
    header->opcode = MID(tmp_byte,11, 15);
    header->aa = MID(tmp_byte,10, 11);
    header->tc = MID(tmp_byte,9, 10);
    header->rd = MID(tmp_byte,8, 9);
    header->ra = MID(tmp_byte,7, 8);
    header->z = MID(tmp_byte,6, 7);
    header->ad = MID(tmp_byte,5, 6);
    header->cd = MID(tmp_byte,4, 5);
    header->rcode = MID(tmp_byte,0, 4);
    tmp_data += 2;
    tmp_header += 2;

    u_int16_t *qdcount = (u_int16_t*)tmp_data;
    memcpy(tmp_header, (void*)qdcount, 2);
    header->qdcount = ntohs(header->qdcount);
    tmp_data += 2;
    tmp_header += 2;

    u_int16_t *ancount = (u_int16_t*)tmp_data;
    memcpy(tmp_header, (void*)ancount, 2);
    header->ancount = ntohs(header->ancount);
    tmp_data += 2;
    tmp_header += 2;

    u_int16_t *nscount = (u_int16_t*)tmp_data;
    memcpy(tmp_header, (void*)nscount, 2);
    header->nscount = ntohs(header->nscount);
    tmp_data += 2;
    tmp_header += 2;

    u_int16_t *arcount = (u_int16_t*)tmp_data;
    memcpy(tmp_header, (void*)arcount, 2);
    header->arcount = ntohs(header->arcount);

    return header;
}


dns_question *init_dns_question(void *data)
{
    dns_question *question = calloc(1, sizeof(dns_question));
    question->qname = ((char *)data) + HEADER_LENGTH;
    question->qtype = *((uint16_t *)(question->qname + strlen(question->qname) + 1));
    question->qclass = *((uint16_t *)(question->qname + strlen(question->qname) + 1 + 16));
    question->qtype = ntohs(question->qtype);
    question->qclass = ntohs(question->qclass);
    return question;
}

void parse_query(void *data, dns_header **dnsheader, dns_question **dnsquestion)
{
    *dnsheader = init_dns_header(data);
    if (dnsheader == NULL)
        return;
    if ((*dnsheader)->qdcount == 0) {
        warnx("NO QUESTION\n");
        exit(1);
    }
    *dnsquestion = init_dns_question(data);
    if (dnsquestion == NULL)
        return;
}




