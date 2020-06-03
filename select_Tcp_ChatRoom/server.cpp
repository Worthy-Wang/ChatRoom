#include <Unixfunc.h>
#include <iostream>
using namespace std;

/*
Tcp通信：
双人聊天室，实现功能：安全退出，断开重连，互发消息

*/

int main()
{
    int ret;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sockfd, -1, "socket");
    int on = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    ERROR_CHECK(ret, -1, "setsockopt");
    struct sockaddr_in sockinfo;
    bzero(&sockinfo, sizeof(sockinfo));
    sockinfo.sin_addr.s_addr = inet_addr("172.21.0.7");
    sockinfo.sin_port = htons(2000);
    sockinfo.sin_family = AF_INET;
    ret = bind(sockfd, (sockaddr *)&sockinfo, sizeof(sockinfo));
    ERROR_CHECK(ret, -1, "bind");
    ret = listen(sockfd, 10);
    ERROR_CHECK(ret, -1, "listen");

    int newfd;
    int maxfd = 10;
    fd_set rdsets;
    fd_set needMonitorsets;
    FD_ZERO(&needMonitorsets);
    FD_SET(0, &needMonitorsets);
    FD_SET(sockfd, &needMonitorsets);
    struct timeval tv;
    while (1)
    {
        rdsets = needMonitorsets;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        ret = select(maxfd, &rdsets, NULL, NULL, &tv);
        ERROR_CHECK(ret, -1, "select");
        if (ret > 0)
        {
            if (FD_ISSET(sockfd, &rdsets))
            {
                struct sockaddr_in client;
                bzero(&client, sizeof(client));
                socklen_t len = sizeof(client);
                newfd = accept(sockfd, (sockaddr *)&client, &len);
                cout << "client " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << " has connected" << endl;
                FD_SET(newfd, &needMonitorsets);
            }
            if (FD_ISSET(0, &rdsets))
            {
                char buf[100] = {0};
                read(0, buf, sizeof(buf));
                write(newfd, buf, strlen(buf) - 1);
            }
            if (FD_ISSET(newfd, &rdsets))
            {
                char buf[100] = {0};
                ret = read(newfd, buf, sizeof(buf));
                if (ret == 0)
                {
                    cout << "bye" << endl;
                    FD_CLR(newfd, &needMonitorsets);
                    close(newfd);
                    continue;
                }
                cout << buf << endl;
            }
        }
        else //ret == 0
            cout << "timeout!" << endl;
    }

    close(sockfd);
    return 0;
}