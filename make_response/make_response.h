//
// Created by denis on 26/09/2020.
//

#ifndef DNS_SERVER_MAKE_RESPONSE_H
#define DNS_SERVER_MAKE_RESPONSE_H

#include <stdint.h>

typedef struct{
    char *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t  rdlength;
    char *rdata;
}dns_response;

#endif //DNS_SERVER_MAKE_RESPONSE_H
