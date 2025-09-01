#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

int main(int argc, char* argv[])
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect(cfd, (sockaddr*)&addr, sizeof(sockaddr)))
        sys_err("connect err");
    
    char buf[1024] = { 0 };
    for (;;)
    {
        std::cin >> buf;
        write(cfd, buf, sizeof(buf));
        read(cfd, buf, sizeof(buf));
        std::cout << buf << std::endl;
    }
    
    close(cfd);
    return 0;
}