//
// Created by denis on 26/09/2020.
//

#include <stdio.h>
#include <memory.h>
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

static void concat_soa(struct soa *soa, char r_data[256])
{
    char a[32] = {0};
    snprintf(a, 32, "%zu", soa->serial);
    char b[32] = {0};
    snprintf(b, 32, "%zu", soa->refresh);
    char c[32] = {0};
    snprintf(c, 32, "%zu", soa->retry);
    char d[32] = {0};
    snprintf(d, 32, "%zu", soa->expire);
    char e[32] = {0};
    snprintf(e, 32, "%zu", soa->minimum_ttl);
    r_data = strcat(soa->mname, " ");
    r_data = strcat(r_data, soa->rname);
    r_data = strcat(r_data, " ");
    r_data = strcat(r_data, a);
    r_data = strcat(r_data, " ");
    r_data = strcat(r_data, b);
    r_data = strcat(r_data, " ");
    r_data = strcat(r_data, c);
    r_data = strcat(r_data, " ");
    r_data = strcat(r_data, d);
    r_data = strcat(r_data, " ");
    r_data = strcat(r_data, e);
}

dns_packet *make_response(dns_header *header, dns_question *question, struct soa *soa, struct record_list *records)
{
    dns_packet *pkt = calloc(1, sizeof(dns_packet));
    pkt->header = *header;
    pkt->question = *question;
    pkt->data = malloc(1);
    int found = 0;
    char r_data[256];
    while (records)
    {
        if (question->qtype == records->node->type && !strcmp(question->qname, records->node->domain_name))
        {
            found++;
            pkt->data = realloc(pkt->data, found * sizeof(dns_response));
            (((dns_response *)(pkt->data))+(found - 1))->name = question->qname;
            (((dns_response *)(pkt->data))+(found - 1))->type = records->node->type;
            (((dns_response *)(pkt->data))+(found - 1))->class = question->qclass;
            (((dns_response *)(pkt->data))+(found - 1))->ttl = records->node->ttl;
            (((dns_response *)(pkt->data))+(found - 1))->rdlength = strlen(records->node->domain_name);
            (((dns_response *)(pkt->data))+(found - 1))->rdata = records->node->domain_name;
        }
        records = records->next;
    }
    if (found == 0)
    {
        pkt->data = realloc(pkt->data, sizeof(dns_response));
        ((dns_response *)(pkt->data))->name = question->qname;
        ((dns_response *)(pkt->data))->type = SOA;
        ((dns_response *)(pkt->data))->class = question->qclass;
        ((dns_response *)(pkt->data))->ttl = soa->minimum_ttl;
        concat_soa(soa, r_data);
        ((dns_response *)(pkt->data))->rdata = r_data;
        ((dns_response *)(pkt->data))->rdlength = strlen(((dns_response *)(pkt->data))->rdata);
    }
    return pkt;
}