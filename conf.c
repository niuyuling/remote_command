#include "conf.h"

void read_conf_file(char *file, conf * p) {
    // IP
    p->IP_len = strlen(read_conf(file, "global", "IP")) + 1;
    p->IP = (char *)malloc(p->IP_len);
    if (p->IP == NULL) {
        free(p->IP);
    }
    memset(p->IP, 0, p->IP_len);
    memcpy(p->IP, read_conf(file, "global", "IP"), p->IP_len);
    
    // PORT
    p->PORT = atoi(read_conf(file, "global", "PORT"));
    
    // COMMAND_OUT_FILE
    p->COMMAND_OUT_FILE_len = strlen(read_conf(file, "global", "COMMAND_OUT_FILE")) + 1;
    p->COMMAND_OUT_FILE = (char *)malloc(p->COMMAND_OUT_FILE_len);
    if (p->COMMAND_OUT_FILE == NULL) {
        free(p->COMMAND_OUT_FILE);
    }
    memset(p->COMMAND_OUT_FILE, 0, p->COMMAND_OUT_FILE_len);
    memcpy(p->COMMAND_OUT_FILE, read_conf(file, "global", "COMMAND_OUT_FILE"), p->COMMAND_OUT_FILE_len);
    
    
    // IP_RESTRICTION
    p->IP_RESTRICTION = atoi(read_conf(file, "global", "IP_RESTRICTION"));
    
    // IP_SEGMENT
    p->IP_SEGMENT_LEN = strlen(read_conf(file, "global", "IP_SEGMENT")) + 1;
    p->IP_SEGMENT = (char *)malloc(p->IP_SEGMENT_LEN);
    if (p->IP_SEGMENT == NULL) {
        free(p->IP_SEGMENT);
    }
    memset(p->IP_SEGMENT, 0, p->IP_SEGMENT_LEN);
    memcpy(p->IP_SEGMENT, read_conf(file, "global", "IP_SEGMENT"), p->IP_SEGMENT_LEN);
    
    // COMMAND_RESTRICTION
    p->COMMAND_RESTRICTION = atoi(read_conf(file, "global", "COMMAND_RESTRICTION"));
    
    // DISABLE_COMMAND
    p->DISABLE_COMMAND_LEN = strlen(read_conf(file, "global", "DISABLE_COMMAND")) + 1;
    p->DISABLE_COMMAND = (char *)malloc(p->DISABLE_COMMAND_LEN);
    if (p->DISABLE_COMMAND == NULL) {
        free(p->DISABLE_COMMAND);
    }
    memset(p->DISABLE_COMMAND, 0, p->DISABLE_COMMAND_LEN);
    memcpy(p->DISABLE_COMMAND, read_conf(file, "global", "DISABLE_COMMAND"), p->DISABLE_COMMAND_LEN);
}

void free_conf_file(conf * p) {
    free(p->IP);
    free(p->COMMAND_OUT_FILE);
    free(p->IP_SEGMENT);
}

void print_conf(conf *p) {
    printf("%s\n", p->IP);
    printf("%d\n", p->PORT);
    printf("%s\n", p->COMMAND_OUT_FILE);
    printf("%d\n", p->IP_RESTRICTION);
    printf("%s\n", p->IP_SEGMENT);
    printf("%d\n", p->COMMAND_RESTRICTION);
    printf("%s\n", p->DISABLE_COMMAND);
}

void split_string(char string[], char delims[], char (*whitelist_ip)[32])
{
    int i = 0;
    char *result = NULL;
    result = strtok(string, delims);
    while (result != NULL) {
        i++;
        strcpy(whitelist_ip[i], result);
        result = strtok(NULL, delims);
    }
}

// IP段白名单
int whitelist(char *String, char (*whitelist_ip)[32])
{
    int i;
    for (i = 1; i < WHITELIST_IP_NUM - 1; i++) {
        if (strcmp(whitelist_ip[i], "\0") == 0) { //  如果字符串为空就跳出循环
            break;
        }
        if ((strncmp(String, whitelist_ip[i], strlen(whitelist_ip[i]))) == 0) { // 对比String长度,相等返回1不相等返回0
            return 1;
        }
    }

    return 0;
}