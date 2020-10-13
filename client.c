#include "client.h"

void help_information() {
    puts("\n"
"remote command client\n"
"Author: aixiao@aixiao.me\n"
"Usage: [-?hlpb]\n"
"\n"
"   Options:\n"
"       -l          : Ip Address\n"
"       -p          : Port\n"
"       -b          : send buffer\n"
"                       \"command : Shell command\"\n"
"       -? -h       : help information\n");
    exit(0);
}

char *get_ip(char *dname)
{
    extern int h_errno;
    struct hostent *h;
    struct in_addr in;
    struct sockaddr_in addr_in;
    h = gethostbyname(dname);
    if (h == NULL) {
        printf("%s\n", hstrerror(h_errno));
    } else {
        memcpy(&addr_in.sin_addr.s_addr, h->h_addr, 4);
        in.s_addr = addr_in.sin_addr.s_addr;
        return inet_ntoa(in);
    }
    return NULL;
}

int create_connect(char *ip, char *port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    if (sock == -1) {
        printf("Socket was not created.\n");
        exit(1);
    }
    struct sockaddr_in serv_addr; // 向服务器(特定的IP和端口)发起请求
    memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充
    serv_addr.sin_family = AF_INET; // 使用IPv4地址
    //serv_addr.sin_addr.s_addr = inet_addr(IP);   // 具体的IP地址
    inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(atoi(port)); // 端口
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Failed to connect to socket.\n");
        exit(3);
    }
    return sock;
}

int main(int argc, char *argv[])
{
    char *IP = NULL;
    char *PORT = NULL;
    char *buffer = (char *)malloc(BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
    int ch;
    int sock;
    opterr = 0;
    while ((ch = getopt(argc, argv, "l:p:b:h?")) != -1) {
        switch (ch) {
        case 'l':
            IP = strdup(get_ip(optarg));
            break;
        case 'p':
            PORT = strdup(optarg);
            break;
        case 'b':{
                strcpy(buffer, optarg);
                break;
            }
        case 'h':
        case '?':
            help_information();
            exit(0);
            break;
        default:
            if (ch == '?') {
                help_information();
                exit(0);
            };
        }
    }
    
    if (argc == 1) {
        help_information();
    }
    
    printf("%s\n", IP);
    printf("%s\n", PORT);
    printf("%s\n", buffer);
    
    sock = create_connect(IP, PORT);

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        printf("Failed to send data to socket.\n");
    }

    memset(buffer, 0, strlen(buffer));

    errno = 0;
    recv(sock, buffer, BUFFER_SIZE, 0); // 接收服务端发来的buffer
    if (errno != 0) {
        printf("errno值: %d\n", errno);
        printf("错误信息: %s\n", strerror(errno));
    }
    printf("%s", buffer);

    close(sock);                //关闭套接字
    free(buffer);
    return 0;
}

