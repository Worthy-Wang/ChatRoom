#include <Unixfunc.h>
#include <iostream>
#define BUFSIZE 4096
using std::cout;
using std::endl;

int main()
{
    int shmid = shmget((key_t)0x1000, BUFSIZE, 0666 | IPC_CREAT);
    ERROR_CHECK(shmid, -1, "shmget");
    char *shm_buf = (char *)shmat(shmid, NULL, 0);
    ERROR_CHECK(shm_buf, (char *)-1, "shmat");
    memset(shm_buf, 0, BUFSIZE);

    while (1)
    {
        if (shm_buf[0] != 0)
            cout << shm_buf << endl;
        memset(shm_buf, 0, BUFSIZE);
    }
    return 0;
}