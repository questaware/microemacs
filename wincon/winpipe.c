#if 0
//------------------------------------------------------------------------------
// Redirect.cpp : implementation file
//
// Creates a child process that runs a user-specified command and redirects its
// standard output and standard error to a CEdit control.
//
// Written by Matt Brunk (brunk@gorge.net)
// Copyright (C) 1999 Matt Brunk
// All rights reserved.
//
// This code may be used in compiled form in any way. This file may be
// distributed by any means providing it is not sold for profit without
// the written consent of the author, and providing that this notice and the
// author's name is included in the distribution. If the compiled form of the
// source code in this file is used in a commercial application, an e-mail to
// the author would be appreciated.
//
// Thanks to Dima Shamroni (dima@abirnet.co.il) for providing the essential
// code for this class.
//
// Thanks to Chris Maunder (chrismaunder@codeguru.com) for the PeekAndPump()
// function (from his CProgressWnd class).
//
// Initial Release Feb 8, 1999
//------------------------------------------------------------------------------


typedef unsigned long DWORD;

#include <windef.h>
//#include <winuser.h>
#include <string.h>
#include <direct.h>
/*#include <afxwin.h>*/

#include "winpipe.h"

#include "estruct.h"


#include "../src/logmsg.h"

#if 1

Bool		m_bStopped = false;
DWORD		m_dwSleepMilliseconds = 100;
char *		m_szCommand;

HANDLE		    DownReadHandle;
HANDLE		    DownWriteHandle;
HANDLE		    UpReadHandle;
HANDLE		    UpWriteHandle;
PROCESS_INFORMATION ProcessInfo;
char                m_szCurrentDirectory[132];

Bool Wp_stopping;
int next_offset;
int next_end;

char schar;



Cc Wp_Create(char * szCommand, int pipesz, Bool connect_stdin)
{
  SECURITY_ATTRIBUTES	SecurityAttributes;
  STARTUPINFO		StartupInfo;

  m_szCommand = szCommand;
  _getcwd(&m_szCurrentDirectory[0], sizeof(m_szCurrentDirectory));

  //--------------------------------------------------------------------------
  //	Zero the structures.
  //--------------------------------------------------------------------------
  ZeroMemory( &StartupInfo,	sizeof( StartupInfo ));
  ZeroMemory( &ProcessInfo,	sizeof( ProcessInfo ));

  DownReadHandle = 0;
  DownWriteHandle = 0;

  //--------------------------------------------------------------------------
  //	Create a pipe for the child's STDOUT.
  //--------------------------------------------------------------------------
  SecurityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttributes.bInheritHandle       = TRUE;
  SecurityAttributes.lpSecurityDescriptor = NULL;

{ BOOL Success = CreatePipe
  		(
  		&UpReadHandle,	// address of variable for read handle
  		&UpWriteHandle,	// address of variable for write handle
  		&SecurityAttributes,	// pointer to security attributes
  		pipesz);		// number of bytes reserved for pipe (use default size)
  if ( !Success )
  {
     loglog("Error creating pipe");
     return -1;
  }	

  if (connect_stdin)
  {
    SecurityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
    SecurityAttributes.bInheritHandle       = TRUE;
    SecurityAttributes.lpSecurityDescriptor = NULL;

    Success = CreatePipe(
  		&DownReadHandle,	// address of variable for read handle
  		&DownWriteHandle,	// address of variable for write handle
  		&SecurityAttributes,	// pointer to security attributes
  		0);			// number of bytes reserved for pipe (use default size)
     
    if ( !Success )
    {
      loglog("Error creating Down");
      return -1;
    }	
  }

  //---------------------------------------------------------------------
  //	Set up members of STARTUPINFO structure.
  //---------------------------------------------------------------------
  StartupInfo.cb           = sizeof(STARTUPINFO);
  StartupInfo.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  StartupInfo.wShowWindow  = SW_HIDE; /* SW_SHOW;*/
  StartupInfo.hStdInput    = DownReadHandle;
  StartupInfo.hStdOutput   = UpWriteHandle;
  StartupInfo.hStdError    = UpWriteHandle;

  loglog1("Cmd %s", m_szCommand);

  //---------------------------------------------------------------------
  //	Create the child process.
  //---------------------------------------------------------------------
  Success = CreateProcess
  		( 
  		NULL,			// pointer to name of executable module
  		(LPTSTR)(m_szCommand),	// command line 
  		NULL,			// pointer to process security attributes 
  		NULL,			// pointer to thread security attributes (use primary thread security attributes)
  		TRUE,			// inherit handles
  		0,			// creation flags
  		NULL,			// pointer to new environment block (use parent's)
  		m_szCurrentDirectory,	// pointer to current directory name
  		&StartupInfo,		// pointer to STARTUPINFO
  		&ProcessInfo);		// pointer to PROCESS_INFORMATION

  if ( !Success )
  {
     loglog("Error creating process");
     return -1;
  }

  Wp_stopping = false;

  next_offset = -1;
  next_end = 1;

  return OK;
}}



int Wp_Read(int * offset, char * PipeData, int bufsz)

{ int ix;

  if (next_offset == -1)
    next_offset = next_end+3;

  *offset = 0;
  PipeData[next_offset] = schar;

  while (true)
  { int len;
    for (ix = next_offset-1; ++ix <= next_end &&
  			   PipeData[ix] != '\n'/* &&
  			   PipeData[ix] != '\r' */; )
      ;
  
    len = ix - next_offset;
  
    if (ix <= next_end || Wp_stopping && ix > next_offset)
    { *offset = next_offset;
      ++len;
      next_offset = ++ix;
      schar = PipeData[ix];
      PipeData[ix] = 0;
      return len;
    }
  
    memmove(&PipeData[0], &PipeData[next_offset], bufsz - next_offset);
    next_end -= next_offset;
    if (next_end < 0)
      next_end = -1;
    next_offset = 0;
  
  { int space = bufsz - next_end - 1;
  
    DWORD  BytesLeftThisMessage = 0;
    DWORD  TotalBytesAvailable = 0;
    DWORD  NumBytesRead = 0;
    BOOL rc;
    if (!Wp_stopping)
    { rc = PeekNamedPipe(UpReadHandle,	// handle to pipe to copy from 
    			 &PipeData[next_end+1],// pointer to data buffer 
    			 1,		// size, in bytes, of data buffer 
    			 &NumBytesRead,	// pointer to number of bytes read 
    			 &TotalBytesAvailable, // pointer to total number of bytes available
    			 &BytesLeftThisMessage);
      if ( !rc )
      { loglog("PeekNamedPipe failed");
        break;
      }
    }
  
 /* loglog1("NumBytesRead %d", NumBytesRead); */

    if ( NumBytesRead )
    { DWORD ii;

      rc = ReadFile(UpReadHandle, // handle to pipe to copy from 
					  		    &PipeData[next_end+1],// address of buffer that receives data
  		    					space, 		// number of bytes to read
					  		    &NumBytesRead,  // address of number of bytes read
					  		    NULL); // address of structure for data for overlapped I/O
      if ( !rc )
      { loglog("ReadFile fialed");
        break;
      }

      PipeData[next_end+1+NumBytesRead] = '\0';

  /*  loglog1("Pread %s", &PipeData[next_end+1]); */
 
      for ( ii = 0; ii < NumBytesRead; ii++ )
      {
        if ( PipeData[ii] == '\b' )
          PipeData[ii] = ' ';
      }  		
    		//-------------------------------------------------------------
    		//	Append the output to the CEdit control.
    		//-------------------------------------------------------------
      Wp_PeekAndPump();
  
      next_end += NumBytesRead;
      continue;
    }
    else if (!Wp_stopping && next_end > 0)
      Wp_stopping = true;
    else
    { while (true)
      {		//------------------------------------------------------------
  		//	If the child process has completed, break out.
  		//------------------------------------------------------------
        if ( WaitForSingleObject(ProcessInfo.hProcess, 0) == WAIT_OBJECT_0 )	//lint !e1924 (warning about C-style cast)
          break;

        Wp_PeekAndPump();

        if ( m_bStopped )
        {
		  	  rc = TerminateProcess(ProcessInfo.hProcess, 0);

				  if ( rc )
				  {
				    strcpy(&PipeData[0], "\r\nCancelled.\r\n\r\nProcess terminated successfully.\r\n");
				  }
				  else
				  { loglog("Error terminating process.");
				  }

				  return -1;
        }

        Sleep(m_dwSleepMilliseconds);
      }
      return -1;
    }
  }}
}



int Wp_Write(char * PipeData, int stt, int lim)

{ 
  DWORD todo = lim - stt + 1;
  DWORD len = 0;

  if (todo <= 0)
    return todo;

  loglog2("Write Pipe %d %s", todo, &PipeData[stt]);

{ int rc = WriteFile(DownWriteHandle, // handle to pipe to copy from 
		     &PipeData[stt], 
  		     todo, 
		     &len, NULL);
  loglog2("Wrote Pipe %d %d", rc, len);
  return rc ? len : -1;
}}


Cc Wp_CloseWrite()

{ 
  if (DownWriteHandle != 0)
  { Cc rc = CloseHandle(DownReadHandle);
    if ( !rc )
    {
      loglog("Error closing down read handle.");
    }

    rc = CloseHandle(DownWriteHandle);
    if ( !rc )
    {
      loglog("Error closing down write handle.");
    }

    DownWriteHandle = 0;
  }

  return OK;
}



Cc Wp_Close()

{
  //--------------------------------------------------------------------------
  //	Close handles.
  //--------------------------------------------------------------------------
  BOOL rc = CloseHandle(ProcessInfo.hThread);
  if ( !rc )
  {
    loglog("Error closing thread handle.");
  }

  rc = CloseHandle(ProcessInfo.hProcess);
  if ( !rc )
  {
    loglog("Error closing process handle.");
  }

  rc = CloseHandle(UpReadHandle);
  if ( !rc )
  {
    loglog("Error closing pipe read handle.");
  }

  rc = CloseHandle(UpWriteHandle);
  if ( !rc )
  {
    loglog("Error closing pipe write handle.");
  }

  return Wp_CloseWrite();
}



void Wp_PeekAndPump()
{
    MSG Msg;
    while (PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) 
    {
        (void)AfxGetApp()->PumpMessage(); //lint !e1924 (warning about C-style cast)
    }
}



void Wp_Stop()
{
  m_bStopped = true;
}


/*

void AppendText(char * Text)
{
  int Length = m_pEdit->GetWindowTextLength();

  m_pEdit->SetSel(Length, Length);
  m_pEdit->ReplaceSel(Text);
  m_pEdit->LineScroll( m_pEdit->GetLineCount() );
}

*/

void Wp_SetSleepInterval(DWORD dwMilliseconds)
{
  m_dwSleepMilliseconds = dwMilliseconds;
}

#endif

#endif
