#define CHECK 0

#include <windows.h>

#if CHECK

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#include <winsock2.h>
#include <cstdio>
#include <ctime>
#include <sys/types.h>
#include <stdio.h>

#endif

#define _CRT_SECURE_NO_WARNINGS

extern "C"
{
#if CHECK
extern void mbwrite(const char*);

int gettimeofday(struct timeval * tp, struct timezone * tzp)	/* one day only */
{
    SYSTEMTIME  sys_ti;

    GetSystemTime( &sys_ti );

    tp->tv_sec  = (sys_ti.wHour * 60 + sys_ti.wMinute) * 60 + sys_ti.wSecond;
    tp->tv_usec = (int)sys_ti.wMilliseconds * 1000;
    return sys_ti.wMonth * 31 + sys_ti.wDay;
}
#endif

int millisleep (unsigned int msecs)
{
#if CHECK
    struct timeval stt,fin;

    int stt_day = gettimeofday(&stt, NULL);
#endif
#if 0
    struct fd_set set;
    struct timeval timeout;
    
    /* Initialize the file descriptor set. */
    FD_ZERO (&set);
    FD_SET (filedes, &set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = secs;
    timeout.tv_usec = usecs;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
//  int cc = select (filedes + 1, &set, NULL, NULL, &timeout);
	  int cc = select (NULL, NULL, NULL, NULL, &timeout);
#else
		int cc = 0;
    Sleep(msecs);
#endif

#if CHECK
    int fin_day = gettimeofday(&fin, NULL);
    
    int u = (fin.tv_sec - stt.tv_sec) * 1000000 + (fin.tv_usec - stt.tv_usec);
    if (u < (int)msecs * 1000 && fin_day == stt_day)
    { char msg[80];
//   	sprintf(msg, "Cc %d %d Fin %d Stt %d", cc, u, fin.tv_usec, stt.tv_usec);
    	mbwrite("Didnt SLEEP");
    }
#endif
		return cc;
}

}

