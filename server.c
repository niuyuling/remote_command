#include "server.h"
#include "conf.h"

int server_sock, client_sock;
int epollfd;
socklen_t client_addr_len;
struct epoll_event ev, events[MAXEPOLLSIZE];
struct sockaddr_in client_addr;

int set_nonblocking(int fd)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

int create_server_socket(int port)
{
    int server_sock;
    int optval = 1;
    struct sockaddr_in server_addr;
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("bind");
        return -1;
    }
    if (listen(server_sock, 20) < 0) {
        perror("listen");
        return -1;
    }
    return server_sock;
}

int command(int socket, char buffer[], conf * p)
{
    FILE *logfp = fopen(p->COMMAND_OUT_FILE, "w+");
    FILE *popenfd;              // FILE文件操作指针
    char BUFFER[COMMANDOUT_SIZE]; // 命令输出缓存
    memset(BUFFER, 0, sizeof(BUFFER));
    popenfd = _popen(buffer, "r");
    fread(BUFFER, sizeof(char), COMMANDOUT_SIZE, popenfd); // 命令输出写入文件指针
    fseek(logfp, 0, SEEK_END);
    fwrite(BUFFER, sizeof(BUFFER), 1, logfp);
    printf("%s", BUFFER);

    fclose(logfp);
    _pclose(popenfd);
    return 0;
}

void *in(void *para, conf * p)
{
    int client_fd = *(int *)para;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    command(client_fd, buffer, p);

    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = client_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, client_fd, &ev) < 0)
        perror("epoll_ctl");
    return NULL;
}

void *out(void *para, conf * p)
{
    int client_fd = *(int *)para;

    FILE *logfp = fopen(p->COMMAND_OUT_FILE, "r");
    char BUFFER[COMMANDOUT_SIZE]; // 命令输出缓存
    memset(BUFFER, 0, COMMANDOUT_SIZE);
    fread(BUFFER, sizeof(BUFFER), 1, logfp);

    send(client_fd, BUFFER, COMMANDOUT_SIZE, 0); // 回传数据到客户端

    ev.data.fd = client_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, &ev) < 0)
        perror("epoll_ctl");
    close(client_fd);
    fclose(logfp);
    return NULL;
}

int accept_client(conf * p)
{
    char ipstr[128];
    char client_ip[32];         // 客户端IP
    char whitelist_ip[WHITELIST_IP_NUM][32] = {{ 0 }, { 0 }};
    split_string(p->IP_SEGMENT, " ", whitelist_ip);
    /*
    for (int i = 1; i <= WHITELIST_IP_NUM - 1; i++) {
        if (*whitelist_ip[i] != '\0')
            printf("%s\n", whitelist_ip[i]);
    }
    */
    
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
        perror("accept");
        return -1;
    }
    strcpy(client_ip, inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ipstr, sizeof(ipstr))); // 复制客户端IP到client_ip
    printf("%s\n", client_ip);
    if (p->IP_RESTRICTION == 1) {
        if (whitelist(client_ip, whitelist_ip) == 0) {
            printf("非法客户端, 拒绝连接\n");
            close(client_sock);
            return -1;
        }
    }

    set_nonblocking(client_sock);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sock, &ev) < 0)
        perror("epoll_ctl");
    
    return 1;
}

int server_loop(conf * p)
{
    int n;
    while (1) {
        n = epoll_wait(epollfd, events, MAXEPOLLSIZE, -1);
        while (n-- > 0) {
            if (events[n].data.fd == server_sock) {
                if ((accept_client(p)) == -1) {
                    continue;
                }
            }
            if (events[n].data.fd == client_sock) {
                if (events[n].events & EPOLLIN) {
                    in((void *)&(events[n].data.fd), p);
                }
                if (events[n].events & EPOLLOUT) {
                    out((void *)&(events[n].data.fd), p);
                }
            }
        }
    }
    close(server_sock);
    return 0;
}

void help_information() {
    puts("\n"
"remote command server\n"
"Author: aixiao@aixiao.me\n"
"Usage: [-?hc] [-c filename]\n"
"\n"
"   Options:\n"
"       -c          : set configuration file, default: CProxy.conf\n"
"       -? -h       : help information\n");
    exit(0);
}

int _main(int argc, char *argv[], char **env)
{
    char *conf_file = "remote_command.conf";
    conf *configure = (struct CONF *)malloc(sizeof(struct CONF));
    int optval = SO_REUSEADDR;
    struct rlimit rt;
    int opt;
    char optstring[] = ":c:h?";
    while (-1 != (opt = getopt(argc, argv, optstring))) {
        switch (opt) {
        case 'c':
            free_conf_file(configure);
            read_conf_file(optarg, configure);
            break;
        case 'h':
        case '?':
            help_information();
            break;
        default:
            ;
        }
    }

    read_conf_file(conf_file, configure);
    print_conf(configure);
    
    if (daemon(1, 1)) {                         // 守护进程
        perror("daemon");
        return -1;
    }
    
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;   // 设置每个进程允许打开的最大文件数
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit");
    }
    
    signal(SIGPIPE, SIG_IGN);   // 忽略PIPE信号
    server_sock = create_server_socket(configure->PORT);
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // 设置socket属性，端口可以重用
    fcntl(server_sock, F_SETFL, O_NONBLOCK);

    epollfd = epoll_create(MAXEPOLLSIZE);
    client_addr_len = sizeof(struct sockaddr_in);
    ev.events = EPOLLIN;
    ev.data.fd = server_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_sock, &ev) < 0)
        perror("epoll_ctl");

    server_loop(configure);
    free_conf_file(configure);
    
    return 0;
}


int main(int argc, char *argv[], char **env)
{
    _main(argc, argv, env);
    return 0;
}