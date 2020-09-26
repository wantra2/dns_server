//
// Created by denis on 26/09/2020.
//

#include <stdio.h>
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

