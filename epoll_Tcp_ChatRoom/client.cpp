#include <Unixfunc.h>
#include <iostream>
using namespace std;

int main()
{
    int ret;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sockfd, -1, "socket");
    struct sockaddr_in sockinfo;
    bzero(&sockinfo, sizeof(sockinfo));
    sockinfo.sin_addr.s_addr = inet_addr("172.21.0.7");
    sockinfo.sin_port = htons(2000);
    sockinfo.sin_family = AF_INET;
    ret = connect(sockfd, (sockaddr *)&sockinfo, sizeof(sockinfo));
    ERROR_CHECK(ret, -1, "connect");
    cout << "connect succuss" << endl;

    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    while (1)
    {
        struct epoll_event evs[2];
        int nfds = epoll_wait(epfd, evs, 2, 3000);
        ERROR_CHECK(nfds, -1, "epoll_wait");
        if (nfds > 0)
        {
            for (int i = 0; i < nfds; i++)
            {
                if (evs[i].data.fd == STDIN_FILENO)
                {
                    char buf[100] = {0};
                    read(0, buf, sizeof(buf));
                    write(sockfd, buf, strlen(buf) - 1);
                }
                if (evs[i].data.fd == sockfd)
                {
                    char buf[100] = {0};
                    ret = read(sockfd, buf, sizeof(buf));
                    if (ret == 0)
                    {
                        cout << "bye" << endl;
                        goto chatOver;
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