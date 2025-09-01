#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <sys/socket.h>
#include <ctype.h>
#include <unistd.h>

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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
        sys_err("socket err!");
    if(0 != bind(sfd, (sockaddr*)&addr, sizeof(addr)))
        sys_err("bind err!");
    
    listen(sfd, 10);

    socklen_t clientAddrLen = sizeof(addr);
    int nsfd = accept(sfd, (sockaddr*)&addr, &clientAddrLen);
    if (nsfd < 0)
        sys_err("accept error!");
    
    char clientIp[20] = { 0 };
    printf("ip:%s port: %d\n", inet_ntop(nsfd, &addr.sin_addr.s_addr, clientIp, sizeof(clientIp)), ntohs(addr.sin_port));

    char buf[1024] = { 0 };
    for (;;)
    {
        int res = read(nsfd, buf, 1024);
        write(STDOUT_FILENO, buf, sizeof(buf));
        
        for(int i = 0; i < res; ++i)
        {
            buf[i] = toupper(buf[i]);
        }
        write(nsfd, buf, res);
    }

    close(sfd);
    close(nsfd);

    return 0;
}