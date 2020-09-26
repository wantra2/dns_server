#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <regex.h>
#include "zone.h"

size_t line_nb = 1;
int zonefile_error = 0;

enum record_type get_record_type(char *str, size_t len)
{
    char *tmp = calloc(len + 1, sizeof(char));
    tmp = strncpy(tmp, str, len);
    enum record_type type = ERROR;
    if (!strcmp(tmp, "A"))
        type = A;
    if (!strcmp(tmp, "AAAA"))
        type = AAAA;
    if (!strcmp(tmp, "CNAME"))
        type = CNAME;
    if (!strcmp(tmp, "TXT"))
        type = TXT;
    if (!strcmp(tmp, "SOA"))
        type = SOA;
    if (type == ERROR)
    {
        warnx("line %ld: invalid record type", line_nb);
        zonefile_error = 1;
    }
    free(tmp);
    return type;
}

struct record_list *add_record(struct record_list *l, struct record *r)
{
    struct record_list *new_node = calloc(1, sizeof(struct record_list));
    new_node->node = r;
    if (!l)
    {
        l = new_node;
    }
    else
    {
        struct record_list *tmp = l;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_node;
    }
    return l;
}

void free_list(struct record_list *list)
{
    if (list)
    {
        struct record_list *tmp = list;
        while (list->next)
        {
            if (list->node)
            {
                free(list->node->domain_name);
                free(list->node->content);
                if (list->node->soa)
                {
                    free(list->node->soa->mname);
                    free(list->node->soa->rname);
                    free(list->node->soa);
                }
                free(list->node);
            }
            list = list->next;
            free(tmp);
            tmp = list;
        }
        if (list->node)
        {
            free(list->node->domain_name);
            free(list->node->content);
            if (list->node->soa)
            {
                free(list->node->soa->mname);
                free(list->node->soa->rname);
                free(list->node->soa);
            }
            free(list->node);
        }
        free(list);
    }
}

int check_ipv4(char *str)
{
    regex_t preg;
    const char *str_regex = "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
        "(\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3}";
    int err = regcomp(&preg, str_regex, REG_NOSUB | REG_EXTENDED);
    if (!err)
    {
        int match = regexec(&preg, str, 0, NULL, 0);
        regfree(&preg);
        if (match)
        {
            warnx("line %ld: invalid IPv4 address.", line_nb);
            zonefile_error = 1;
            return 0;
        }
    }
    return 1;
}

int check_ipv6(char *str)
{
    regex_t preg;
    const char *str_regex = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:)"
        "{1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]"
        "{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|"
        "[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4})"
        "{1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0"
        "{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}"
        "(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:"
        "((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|"
        "1{0,1}[0-9]){0,1}[0-9]))";
    int err = regcomp(&preg, str_regex, REG_NOSUB | REG_EXTENDED);
    if (!err)
    {
        int match = regexec(&preg, str, 0, NULL, 0);
        regfree(&preg);
        if (match)
        {
            warnx("line %ld: invalid IPv6 address.", line_nb);
            zonefile_error = 1;
            return 0;
        }
    }
    return 1;
}

char *check_domain_name(char *str, size_t len)
{
    char *domain_name = calloc(len, sizeof(char));
    domain_name = strncpy(domain_name, str, len);
    regex_t preg;
    const char *str_regex = "^(([a-zA-Z0-9]|[a-zA-Z0-9\\-][a-zA-Z0-9]*"
        "[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])\\.?$";
    int err = regcomp(&preg, str_regex, REG_NOSUB | REG_EXTENDED);
    if (!err)
    {
        int match = regexec(&preg, domain_name, 0, NULL, 0);
        regfree(&preg);
        if (match)
        {
            warnx("line %ld: %s is not a valid domain name.", line_nb, domain_name);
            zonefile_error = 1;
        }
    }
    return domain_name;
}

size_t check_ttl(char *str, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            warnx("line %ld: invalid TTL value.", line_nb);
            zonefile_error = 1;
            return 0;
        }
    }
    char *ttl_s = calloc(len, sizeof(char));
    ttl_s = strncpy(ttl_s, str, len);
    size_t ttl = atoi(ttl_s);
    free(ttl_s);
    return ttl;
}

size_t check_value(char *str, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            warnx("line %ld: invalid number.", line_nb);
            zonefile_error = 1;
            return 0;
        }
    }
    char *value_s = calloc(len, sizeof(char));
    value_s = strncpy(value_s, str, len);
    size_t value = atoi(value_s);
    free(value_s);
    return value;
}


char *check_content(char *str, size_t len, enum record_type type)
{
    char *content = calloc(len + 1, sizeof(char));
    content = strncpy(content, str, len);
    if (type == A)
        check_ipv4(content);
    else if (type == AAAA)
        check_ipv6(content);
    else if (type == CNAME)
    {
        char *tmp = check_domain_name(content, len - 1);
        free(tmp);
    }
    return content;
}

struct soa *create_soa(char *str)
{
    size_t position = 0;
    char *end;
    struct soa *soa = calloc(1, sizeof(struct soa));
    for (int i = 0; i < 2; i++)
    {
        end = strchr(str + position, ' ');
        if (!end)
        {
            warnx("line %ld: missing information.", line_nb);
            zonefile_error = 1;
            return NULL;
        }
        char *name = check_domain_name(str + position, end - (str + position));
        if (i == 0)
            soa->mname = name;
        else
            soa->rname = name;
        if (!(end + 1))
        {
            warnx("line %ld: missing information.", line_nb);
            zonefile_error = 1;
            return NULL;
        }
        position += (end - (str + position) + 1);
    }
    for (int i = 0; i < 5; i++)
    {
        if (i != 4)
            end = strchr(str + position, ' ');
        else
            end = strchr(str + position, '\0') - 1;
        if (i != 4 && !end)
        {
            warnx("line %ld: missing information.", line_nb);
            zonefile_error = 1;
            return NULL;
        }
        size_t size = check_ttl(str + position, end - (str + position));
        if (i == 0)
            soa->serial = size;
        else if (i == 1)
            soa->refresh = size;
        else if (i == 2)
            soa->retry = size;
        else if (i == 3)
            soa->expire = size;
        else
            soa->minimum_ttl = size;
        if (i != 3 && !(end + 1))
        {
            warnx("line %ld: missing information.", line_nb);
            zonefile_error = 1;
            return NULL;
        }
        position += (end - (str + position) + 1);
    }
    return soa;
}

struct record *create_record(char *line)
{
    char *str_start = line;
    char *str_end = strchr(str_start, ';');
    if (!str_end)
    {
        warnx("line %ld: missing semicolon.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    char *domain_name = check_domain_name(str_start, str_end - str_start);
    if (!(str_end + 1))
    {
        warnx("line %ld: missing information.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    str_start = str_end + 1;
    str_end = strchr(str_start, ';');
    if (!str_end)
    {
        warnx("line %ld: missing semicolon.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    enum record_type type = get_record_type(str_start, str_end - str_start);
    if (!(str_end + 1))
    {
        warnx("line %ld: missing information.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    str_start = str_end + 1;
    str_end = strchr(str_start, ';');
    if (!str_end)
    {
        warnx("line %ld: missing semicolon.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    size_t ttl = check_ttl(str_start, str_end - str_start);
    if (!(str_end + 1))
    {
        warnx("line %ld: missing the content.", line_nb);
        zonefile_error = 1;
        return NULL;
    }
    str_start = str_end + 1;
    char *content = check_content(str_start, strlen(str_start), type);
    struct record *record = calloc(1, sizeof(struct record));
    record->domain_name = domain_name;
    record->type = type;
    record->ttl = ttl;
    record->content = content;
    if (type == SOA)
        record->soa = create_soa(content);
    return record;
}

struct record_list *create_record_list(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        warnx("cannot find the file named %s", filename);
        exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    struct record_list *list = NULL;
    while ((read = getline(&line, &len, file)) != -1)
    {
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';
        struct record *r = create_record(line);
        list = add_record(list, r);
        line_nb++;
    }
    free(line);
    fclose(file);
    return list;
}


