//
// Created by denis on 26/09/2020.
//

#ifndef DNS_SERVER_MAKE_RESPONSE_H
#define DNS_SERVER_MAKE_RESPONSE_H

#include <stdint.h>
#include "../parse_request/parse_request.h"

typedef struct{
    char *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t  rdlength;
    char *rdata;
}dns_response;

typedef struct{
    dns_header header;
    dns_question question;
    char *data;
}dns_packet;

#endif //DNS_SERVER_MAKE_RESPONSE_H
