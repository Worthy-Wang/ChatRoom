#include <Unixfunc.h>
#include <iostream>
#define BUFSIZE 4096
using std::cout;
using std::endl;

/*
实现双人聊天室
通过两个命名管道 1.pipe 2.pipe
以及使用IO复用模型 select
*/

int main(int argc, char *argv[])
{
    ARGS_CHECK(argc, 3);
    int fdr = open(argv[1], O_RDONLY);
    int fdw = open(argv[2], O_WRONLY);
    ERROR_CHECK(fdw, -1, "open 1");
    ERROR_CHECK(fdr, -1, "open 2");
    int shmid = shmget((key_t)0x2000, BUFSIZE, 0666 | IPC_CREAT);
    ERROR_CHECK(shmid, -1, "shmget");
    char *shm_buf = (char *)shmat(shmid, NULL, 0);
    ERROR_CHECK(shm_buf, (char *)-1, "shmat");
    memset(shm_buf, 0, BUFSIZE);

    char buf[BUFSIZE] = {0};
    fd_set rdset;
    struct timeval timeout;
    int ret;
    system("clear");
    cout << "fdw:" << fdw << " fdr:" << fdr << endl;
    while (1)
    {
        FD_ZERO(&rdset);
        FD_SET(0, &rdset);
        FD_SET(fdr, &rdset);
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        ret = select(fdr + 1, &rdset, NULL, NULL, &timeout);
        ERROR_CHECK(ret, -1, "select");
        if (ret > 0)
        {
            if (FD_ISSET(0, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                read(0, buf, sizeof(buf));
                system("clear");
                write(fdw, buf, strlen(buf) - 1);
            }
            if (FD_ISSET(fdr, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                memset(shm_buf, 0, BUFSIZE);
                ret = read(fdr, buf, sizeof(buf));
                if (ret == 0)
                {
                    strcpy(shm_buf, "ByeBye");
                    break;
                }
                strcpy(shm_buf, buf);
            }
        }
        else
        {
            memset(shm_buf, 0, BUFSIZE);
            strcpy(shm_buf, "timeout!");
        }
    }
    shmctl(shmid, IPC_RMID, NULL);
    close(fdw);
    close(fdr);
    return 0;
}
