#pragma once
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>
#include <regex.h>
#include <string.h>


struct opt {
    char *file;
    char *ipv4_addr;
    char *ipv6_addr;
    int uport;
    int tport;
};

struct opt parse_option(int argc, char **argv);
