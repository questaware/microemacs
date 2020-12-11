#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#include <cstdio>
#include <ctime>
#include <sys/types.h>
#include <windows.h>
#include <stdio.h>

#define CHECK 1

extern "C" void tcapbeep(void);
extern "C" void mbwrite(char*);

extern "C"
{

int gettimeofday(struct timeval * tp, struct timezone * tzp)	/* one day only */
{
    SYSTEMTIME  sys_ti;

    GetSystemTime( &sys_ti );

    tp->tv_sec  = (sys_ti.wHour * 60 + sys_ti.wMinute) * 60 + sys_ti.wSecond;
    tp->tv_usec = (int)sys_ti.wMilliseconds * 1000;
    return sys_ti.wMonth * 31 + sys_ti.wDay;
}

int input_timeout (int filedes, unsigned int secs, unsigned int usecs)
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
    Sleep(secs * 1000+usecs / 1000);
#endif

#if CHECK
    int fin_day = gettimeofday(&fin, NULL);
    
    int u = (fin.tv_sec - stt.tv_sec) * 1000000 + (fin.tv_usec - stt.tv_usec);
    if (u < (int)secs * 1000000 + (int)usecs && fin_day == stt_day)
    { char msg[80];
    	sprintf(msg, "Cc %d %d Fin %d Stt %d", cc, u, fin.tv_usec, stt.tv_usec);
    	mbwrite(msg);
    }
#endif
		return cc;
}

}

