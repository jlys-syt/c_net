#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <sys/socket.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

struct s_info
{
    struct sockaddr_in clientAddr;
    int connfd;
};

void* do_work(void* arg)
{
    struct s_info* ts = (struct s_info*) arg;
    char buf[1024] = { 0 };
    for (;;)
    {
        int ret = read(ts->connfd, buf, sizeof(buf));
        if ( ret == 0)
        {
            printf("the client %d closed...\n", ts->connfd);
            break;
        }

        for(int i = 0; i < ret; ++i)
        {
            buf[i] = toupper(buf[i]);
        }

        write(STDOUT_FILENO, buf, ret);
        write(ts->connfd, buf, ret);
    }
    close(ts->connfd);
    return (void *) 0;
}

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

    struct s_info ts[156];
    pthread_t tid;
    int i = 0;

    for (;;)
    {
        socklen_t clientAddrLen = sizeof(addr);
        int nsfd = accept(sfd, (sockaddr*)&addr, &clientAddrLen);
        if (nsfd < 0)
            sys_err("accept error!");

        ts[i].clientAddr = addr;
        ts[i].connfd = nsfd;
        pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
        pthread_detach(tid);
        ++i;
        char clientIp[1024] = { 0 };
        printf("ip:%s port: %d\n",
            inet_ntop(nsfd, &addr.sin_addr.s_addr, clientIp, sizeof(clientIp)),
             ntohs(addr.sin_port));
    }

    close(sfd);

    return 0;
}