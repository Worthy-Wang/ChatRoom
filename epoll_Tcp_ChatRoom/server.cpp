#include <Unixfunc.h>
#include <iostream>
using namespace std;

/*
Tcp通信：
双人聊天室，I/O多路复用epoll模型，安全退出，断开重连，互发消息

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
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    socklen_t len = sizeof(client);

    int newfd;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    while (1)
    {
        struct epoll_event evs[3];
        int nfds = epoll_wait(epfd, evs, 3, 3000);
        ERROR_CHECK(nfds, -1, "epoll_wait");
        if (nfds > 0)
        {
            for (int i = 0; i < nfds; i++)
            {
                if (evs[i].data.fd == sockfd)
                {
                    newfd = accept(sockfd, (sockaddr *)&client, &len);
                    cout << "client " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << " has connected" << endl;
                    ev.data.fd = newfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &ev);
                }
                if (evs[i].data.fd == STDIN_FILENO)
                {
                    char buf[100] = {0};
                    read(0, buf, sizeof(buf));
                    write(newfd, buf, strlen(buf) - 1);
                }
                if (evs[i].data.fd == newfd)
                {
                    char buf[100] = {0};
                    ret = read(newfd, buf, sizeof(buf));
                    if (ret == 0)
                    {
                        cout << "bye" << endl;
                        ev.data.fd = newfd;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
                        close(newfd);
                        continue;
                    }
                    cout << buf << endl;
                }
            }
        }
        else //nfds == 0
            cout << "timeout!" << endl;
    }
chatOver:
    close(sockfd);
    close(epfd);
    return 0;
}