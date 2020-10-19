# remote_command
    远程执行shell命令服务端和客户端
    服务端非阻塞epoll多路复用IO
    简单支持客户端IP白名单
    支持大部分shell命令，不支持while循环的shell命令
    支持ipv6
    简单支持命令黑名单
    

# 编译
    git clone https://github.com/niuyuling/remote_command
    cd remote_command
    make clean; make
