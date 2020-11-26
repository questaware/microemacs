// mtgdi.cpp : Defines the class behaviors for the application.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "mtgdi.h"

#include "mainfrm.h"
#include "mtgdidoc.h"
#include "mtgdivw.h"
#include "threads.h"

#include "build.h"

#include "logmsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CThreadApp, CWinApp)
	//{{AFX_MSG_MAP(CThreadApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()
;

int argc = 0;
char * * argv;

int callmain(LPSTR param_)

{ CString param = param_;
  int slen = param.GetLength();
  char * src_ = (char*)malloc(slen+1+10);
  bool inspace = true;

  strcpy(&src_[0], "emacs ");
  strcat(&src_[0], param_);
  argc = 0;

  char * src;
  for (src = src_; *src != 0; ++src)
  { if (*src > ' ')
    { if (inspace)
        ++argc;
      inspace = false;
    }
    else
    { inspace = true;
      *src = 0;
    }
  }

  argv = (char**)calloc(sizeof(char *), argc + 1);
  argv[argc] = NULL;  
  int aix = 0;
  inspace = true;

  for (src = src_; aix < argc; ++src)
  { 
    if (*src == 0)
      inspace = true;
    else
    { if (inspace)
	argv[aix++] = src;
      inspace = false;
    }
  }

  for (aix = -1; ++aix < argc; )
    loglog1("ARG %s", argv[aix]);

  return 0 /* OK */;
}


//---------------------------------------------------------------------------

CThreadApp::CThreadApp()
{
}

CThreadApp theApp;

BOOL CThreadApp::InitInstance()
{
/*	if (m_lpCmdLine[0] != '\0')*/
	log_init("emacs.log", 300000, 0);
	loglog1("***************Started*************** %x", stdout);

	printf("Going for it\n");

#if 1
	  callmain(m_lpCmdLine);
#else
	{  int len = (wcslen(m_lpCmdLine)+1) * 2;
	    // Determine number of bytes to be allocated for ANSI string. An
	    // ANSI string can have at most 2 bytes per character (for Double
	    // Byte Character Strings.)
    
	    LPSTR cmd = (LPSTR)malloc(len);
						    // Convert to ANSI.
	    if (0 == WideCharToMultiByte(CP_ACP, 0, m_lpCmdLine, len, cmd,
			    len, NULL, NULL))
		Beep(200,200);
	    else
	        callmain(cmd);
	}
#endif

	Enable3dControls();
				    // Initialize static members of CGDIThread
	InitializeCriticalSection(&CGDIThread::m_csGDILock);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate =
		  new CSingleDocTemplate(
				IDR_MAINFRAME,
				RUNTIME_CLASS(CThreadDoc),
				RUNTIME_CLASS(CMainFrame), // main SDI frame window
				RUNTIME_CLASS(CThreadView));
	AddDocTemplate(pDocTemplate);

	// create a new (empty) document
	OnFileNew();

	return TRUE;
}

int CThreadApp::ExitInstance()
{
	DeleteCriticalSection(&CGDIThread::m_csGDILock);
	CloseHandle(CGDIThread::m_hAnotherDead);

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CThreadApp::OnAppAbout()
{
	CAboutDlg().DoModal();
}
