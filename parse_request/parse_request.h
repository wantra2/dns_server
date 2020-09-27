//
// Created by denis on 26/09/2020.
//

#ifndef DNS_SERVER_PARSE_REQUEST_H
#define DNS_SERVER_PARSE_REQUEST_H

#include <stdint.h>
#include <sys/types.h>

#define OPCODE_VALUE 0
#define AA_VALUE 1
#define RD_VALUE 0
#define RA_VALUE 0
#define Z_VALUE 0

#define TCP_BEFORE_FIELD_SIZE 16

#define HEADER_LENGTH 12
#define QUESTION_LENGTH 16
#define AUTHORITY_LENGTH 16
#define ADDITIONAL_LENGTH 16

typedef struct{
    u_int16_t id;
    u_int16_t qr:1;
    u_int16_t opcode:4;
    u_int16_t aa:1;
    u_int16_t tc:1;
    u_int16_t rd:1;
    u_int16_t ra:1;
    u_int16_t z:1;
    u_int16_t ad:1;
    u_int16_t cd:1;
    u_int16_t rcode:3;
    u_int16_t qdcount; // questions
    u_int16_t ancount; // response
    u_int16_t nscount; // authority
    u_int16_t arcount; // additional
}dns_header;

typedef struct{
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
}dns_question;

typedef struct{
    dns_header* header;
    dns_question* question;
}dns_query;


void print_header(dns_header *dns_header);
void print_question(dns_question *dns_question);
void print_dns_query(dns_query *dns_query);

dns_query *init_dns_query(char *buf);
dns_header *init_dns_header(char *data);
dns_question *init_dns_question(char *buf);

void parse_query(void *data, dns_header **dnsheader, dns_question **dnsquestion);

#endif //DNS_SERVER_PARSE_REQUEST_H
