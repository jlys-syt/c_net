#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

void handlerchld(int signum)
{
    while(waitpid(0,nullptr, WNOHANG) > 0);
    return;
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

    for(;;)
    {
        socklen_t clientAddrLen = sizeof(addr);
        int nsfd = accept(sfd, (sockaddr*)&addr, &clientAddrLen);
        if (nsfd < 0)
            sys_err("accept error!");
        char buf[1024] = { 0 };
        
        int pid = fork();
        if (pid == 0)
        {
            close(sfd);
            for(;;)
            {
                int res = read(nsfd, buf, sizeof(buf));
                if(res == 0)
                {
                    close(nsfd);
                    exit(1);
                }
                write(STDOUT_FILENO, buf, res);
                for(int i = 0; i < res; ++i)
                {
                    buf[i] = toupper(buf[i]);
                }
                write(nsfd, buf, res);
            }
        }
        else if (pid > 0)
        {
            close(nsfd);
            signal(SIGCHLD, handlerchld);
            struct sigaction act;
            act.sa_handler = handlerchld;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            if(0 != sigaction(SIGCHLD, &act, NULL))
                sys_err("signal register SIGCHLD err");
            continue;
        }
        else if( pid < 0)
        {
            sys_err("pid < 0, fork err");
        }

        close(nsfd);
        close(sfd);
    }

    return 0;
}