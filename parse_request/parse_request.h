//
// Created by denis on 26/09/2020.
//

#ifndef DNS_SERVER_PARSE_REQUEST_H
#define DNS_SERVER_PARSE_REQUEST_H

#include <stdint.h>

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
    uint16_t id;
    uint16_t ra:1;
    uint16_t z:3;
    uint16_t rcode:3;
    uint16_t qr:1;
    uint16_t opcode:4;
    uint16_t aa:1;
    uint16_t tc:1;
    uint16_t rd:1;
    uint16_t qdcount; // questions
    uint16_t ancount; // response
    uint16_t nscount; // authority
    uint16_t arcount; // additional
}dns_header;

typedef struct{
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
}dns_question;

void print_header(dns_header *dns_header);
void print_question(dns_question *dns_question);

dns_header *init_dns_header();
dns_question *init_dns_question();

void parse_query(void *data, dns_header **dnsheader, dns_question **dnsquestion);

#endif //DNS_SERVER_PARSE_REQUEST_H
