#include <Unixfunc.h>
#include <iostream>
using namespace std;

int main()
{
    int ret;
    char temp;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_CHECK(sockfd, -1, "socket");
    struct sockaddr_in sockinfo;
    socklen_t len = sizeof(sockinfo);
    bzero(&sockinfo, sizeof(sockinfo));
    sockinfo.sin_addr.s_addr = inet_addr("172.21.0.7");
    sockinfo.sin_port = htons(2000);
    sockinfo.sin_family = AF_INET;
    sendto(sockfd, "1", 1, 0, (sockaddr *)&sockinfo, len);

    fd_set rdsets;
    struct timeval tv;
    while (1)
    {
        FD_ZERO(&rdsets);
        FD_SET(0, &rdsets);
        FD_SET(sockfd, &rdsets);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        ret = select(sockfd + 1, &rdsets, NULL, NULL, &tv);
        ERROR_CHECK(ret, -1, "select");
        if (ret > 0)
        {
            if (FD_ISSET(0, &rdsets))
            {
                char buf[100] = {0};
                read(0, buf, sizeof(buf));
                sendto(sockfd, buf, strlen(buf) - 1, 0, (sockaddr *)&sockinfo, len);
            }
            if (FD_ISSET(sockfd, &rdsets))
            {
                char buf[100] = {0};
                recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
                cout << buf << endl;
            }
        }
        else //ret == 0
            cout << "timeout!" << endl;
    }

    close(sockfd);
    return 0;
}