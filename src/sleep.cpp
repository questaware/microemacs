#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#include <cstdio>
#include <ctime>
#include <sys/types.h>

extern "C"
{

int input_timeout (int filedes, unsigned int secs, unsigned int msecs)
{
    struct fd_set set;
    struct timeval timeout;

    /* Initialize the file descriptor set. */
    FD_ZERO (&set);
    FD_SET (filedes, &set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = secs;
    timeout.tv_usec = msecs;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
    return select (filedes + 1, &set, NULL, NULL, &timeout);
}

}

