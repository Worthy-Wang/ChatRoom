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

    fd_set rdsets;
    struct timeval tv;
    while (1)
    {
        FD_ZERO(&rdsets);
        FD_SET(0, &rdsets);
        FD_SET(sockfd, &rdsets);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        ret = select(sockfd+1, &rdsets, NULL, NULL, &tv);
        ERROR_CHECK(ret, -1, "select");
        if (ret > 0)
        {
            if (FD_ISSET(0, &rdsets))
            {
                char buf[100] = {0};
                read(0, buf, sizeof(buf));
                write(sockfd, buf, strlen(buf) - 1);
            }
            if (FD_ISSET(sockfd, &rdsets))
            {
                char buf[100] = {0};
                ret = read(sockfd, buf, sizeof(buf));
                if (ret == 0)
                {
                    cout << "bye" << endl;
                    break;
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