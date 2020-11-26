#ifndef REDIRECT_H_INCLUDED__
#define REDIRECT_H_INCLUDED__

typedef short Cc;
typedef short Bool;
typedef unsigned long DWORD;

Cc   Wp_Create(char * szCommand, int pipesz /*= 0*/, Bool /*conn_stdin= 0*/);
int  Wp_Read(int * offset, char * PipeData, int bufsz);
int  Wp_Write(char * PipeData, int stt, int lim);
Cc   Wp_CloseWrite();
Cc   Wp_Close();
void Wp_Stop();

void  Wp_AppendText(char * Text);
void  Wp_PeekAndPump();
void  Wp_SetSleepInterval(DWORD dwMilliseconds);

extern Bool		m_bStopped;
extern DWORD		m_dwSleepMilliseconds;
extern char *		m_szCommand;

extern HANDLE		    DownReadHandle;
extern HANDLE		    DownWriteHandle;
extern HANDLE		    UpReadHandle;
extern HANDLE		    UpWriteHandle;
extern PROCESS_INFORMATION ProcessInfo;
extern char                 m_szCurrentDirectory[132];

extern Bool Wp_stopping;
extern int next_offset;
extern int next_end;

extern char schar;


#endif	// REDIRECT_H_INCLUDED__
