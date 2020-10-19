#ifndef CONF_H
#define CONF_H

#include "libconf.h"
#define WHITELIST_IP_NUM 200

// 配置文件结构
typedef struct CONF {
    char *IP;
    int PORT;
    char *COMMAND_OUT_FILE;
    int IP_RESTRICTION;
    char *IP_SEGMENT;
    int COMMAND_RESTRICTION;
    char *DISABLE_COMMAND;
    int IP_len, whitelist_ip_len, PID_FILE_len, COMMAND_OUT_FILE_len, IP_SEGMENT_LEN, DISABLE_COMMAND_LEN;
} conf;

void read_conf_file(char *file, conf * p);
void free_conf_file(conf * p);
void print_conf(conf *p);
int whitelist(char *client_ip, char (*whitelist_ip)[32]);
void split_string(char string[], char delims[], char (*whitelist_ip)[32]);

#endif