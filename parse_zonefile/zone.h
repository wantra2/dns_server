#pragma once

#include <stdlib.h>

enum record_type
{
    A = 1,
    AAAA = 28,
    CNAME = 5,
    TXT = 16,
    SOA = 6,
    ERROR = 5
};

struct soa
{
    char *mname;
    char *rname;
    size_t serial;
    size_t refresh;
    size_t retry;
    size_t expire;
    size_t minimum_ttl;
};

struct record
{
    char *domain_name;
    enum record_type type;
    size_t ttl;
    char *content;
    struct soa *soa;
};

struct record_list
{
    struct record *node;
    struct record_list *next;
};

enum record_type get_record_type(char *str, size_t len);
struct record *create_record(char *line);
struct soa *create_soa(char *str);
size_t check_value(char *str, size_t len);
struct record_list *add_record(struct record_list *l, struct record *r);
void free_list(struct record_list *list);
int check_ipv4(char *str);
int check_ipv6(char *str);
char *check_domain_name(char *str, size_t len);
size_t check_ttl(char *str, size_t len);
char *check_content(char *str, size_t len, enum record_type type);
struct record_list *create_record_list(char *filename);
extern size_t line_nb;
extern int zonefile_error;
