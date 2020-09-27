//
// Created by denis on 26/09/2020.
//

#include "parse_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <netinet/in.h>
#include <err.h>

void print_dns_query(dns_query *dns_query)
{
    printf("------HEADER------\n");
    print_header(dns_query->header);
    printf("-----QUESTION-----\n");
    print_question(dns_query->question);
    printf("------------------\n");
    printf("------------------\n");
}

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
    printf("qname = %s \n", dns_question->qname);
    printf("qtype = %x \n", dns_question->qtype);
    printf("qclass = %x \n", dns_question->qclass);
}

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

dns_query *init_dns_query(char *buf)
{
    dns_query *query = malloc(sizeof(dns_query));
    query->header = init_dns_header(buf);
    query->question = init_dns_question(buf);
    return query;
}

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


dns_question *init_dns_question(char *buf)
{

    //Standard size of header
    buf = buf + 12;

    dns_question *question = calloc(1, sizeof(dns_question));
    
    //First compute number and size of each label
    char *tmp = buf;
    int label_number = 0;
    size_t str_size = 0;
    while (*tmp != 0x00)
    {
        if(*((uint8_t*)tmp) < 32) {
            label_number++;
            str_size += *((uint8_t*)tmp);
        }
        tmp ++;
    }
    //Then create and fill a buffer to build the searched name.
    size_t size = str_size + label_number; //-1 pour les intevales mais +1 pour le \0
    char *res = malloc(sizeof(char)*size);
    tmp = buf + 1;
    for (size_t i = 0; i < size-1; i++)
    {
        if(*((uint8_t*)tmp) > 32) {
            res[i] = *((uint8_t*)tmp);
        } else {
            res[i] = '.';
        }
        tmp ++;
    }
    res[size-1] = '\0';

    question->qname = res;

    //Moving forward on data to find Qtype
    tmp = buf + size + 1;
    //Need to use to uint8 and merge them as uint16 is not always aligned
    uint8_t a = *((uint8_t*)tmp);
    tmp ++;
    uint8_t b = *((uint8_t*)tmp);
    uint16_t qtype = ((uint16_t)a << 8) | b;
    tmp++;
    question->qtype = qtype;
    
    //Moving forward on data to find Qclass
    //Need to use to uint8 and merge them as uint16 is not always aligned
    a = *((uint8_t*)tmp);
    tmp ++;
    b = *((uint8_t*)tmp);
    uint16_t qclass = ((uint16_t)a << 8) | b;
    question->qclass = qclass;

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