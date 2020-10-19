#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#define MAXEPOLLSIZE 1024
#define COMMANDOUT_SIZE 102400
#define BUFFER_SIZE 1024

void *pthread_handle_message(void *para);
FILE *_popen(const char *cmdstring, const char *type);
int _pclose(FILE * fp);

#endif
