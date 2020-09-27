#include "opt.h"

static size_t check_port(char *str, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            warnx("Options: invalid Port.");
            return 0;
        }
    }
    return 1;
}

static int check_ipv4(char *str)
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
            warnx("Options: invalid IPv4 address.");
            return 0;
        }
    }
    return 1;
}

static int check_ipv6(char *str)
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
            warnx("Options: invalid IPv6 address.");
            return 0;
        }
    }
    return 1;
}

struct opt parse_opt(int argc, char **argv)
{
    char *optstring = "";
    int c;

    static struct option long_opt[] = {
        {"file", 1, 0, 'f'},
        {"ipv4", 1, 0, '4'},
        {"ipv6", 1, 0, '6'},
        {"uport", 1, 0, 'u'},
        {"tport", 1, 0, 't'},
        {0, 0, 0, 0}
    };
    struct opt options;
    options.file = NULL;
    options.ipv4_addr = "127.0.0.1";
    options.ipv6_addr = "::1";
    options.uport = 53;
    options.tport = 53;

    while (1)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, optstring, long_opt, &option_index);
        if (c == -1 || c == 0)
            break;
        if (c == 'f')
            options.file = optarg;
        else if (c == '4')
        {
            if (check_ipv4(optarg))
                options.ipv4_addr = optarg;
            else
                exit(1);
        }
        else if (c == '6')
        {
            if (check_ipv6(optarg))
                options.ipv6_addr = optarg;
            else
                exit(1);
        }
        else if (c == 'u')
        {
            if (check_port(optarg, strlen(optarg)))
                options.uport = atoi(optarg);
            else
                exit(1);
        }
        else if (c == 't')
        {
            if (check_port(optarg,strlen(optarg)))
                options.tport = atoi(optarg);
            else
                exit(1);
        }
    }
    if (options.file == NULL)
    {
        warnx("Options: File option necessary");
        exit(1);
    }
    return options;
}

int main(int argc, char *argv[])
{
    parse_opt(argc, argv);
    return 0;
}
