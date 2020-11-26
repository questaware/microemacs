// mtgdivw.cpp : implementation of the CThreadView class
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

#include "mtgdidoc.h"
#include "mtgdivw.h"

#include "estruct.h"
#include "logmsg.h"

#include "edef.h"
#include "etype.h"
#include "epredef.h"
#include "elang.h"
#include "keyboard.h"
using namespace Keyboard;
#include "mycolors.h"
#include "winpipe.h"


extern int argc;
extern char * * argv;



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadView

IMPLEMENT_DYNCREATE(CThreadView, CView)

BEGIN_MESSAGE_MAP(CThreadView, CView)
	//{{AFX_MSG_MAP(CThreadView)
	ON_COMMAND(ID_THREAD_NEWBALL, OnNewball)
	ON_COMMAND(ID_THREAD_KILLTHREADS, OnKillThreads)
	ON_WM_DESTROY()
	ON_COMMAND(ID_THREAD_NEWLINE, OnNewline)
	ON_COMMAND(ID_THREAD_NEWRECTANGLE, OnNewrectangle)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_THREAD_KILLTHREADSSLOW, OnThreadKillThreadsSlow)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define INCWALLPAPER 0

CThreadView * threadv;


/////////////////////////////////////////////////////////////////////////////
// CThreadView construction/destruction

CThreadView::CThreadView()
{
	m_pDC = NULL;
	threadv = this;

	kb_mode = 0;
}

CThreadView::~CThreadView()
{
}

void CThreadView::UpdateTitle(const char * title)
{
	CString strCaption = title;
#if 1
	CWnd * mw = AfxGetMainWnd();
	if (mw != NULL)
	  mw->SetWindowText(_T(strCaption));
#else
	GetParentFrame()->SetWindowText(strCaption);
#endif
	loglog("Did SetWindowTitle");
}

static char consoletitle[120];

void Pascal NEAR setconsoletitle(char * title)

{ if      (title == null or title[0] == 0)
    consoletitle[0] = 0;
  else if (consoletitle[0] == 0)
   strpcpy(&consoletitle[0], title, sizeof(consoletitle));
  else
   return;

  if (threadv != NULL)
     threadv->UpdateTitle(consoletitle);
}


char * Pascal NEAR getconsoletitle()

{ return consoletitle;
}



void Pascal NEAR MySetCoMo()

{
#if 0
  hConsoleIn = GetStdHandle( STD_INPUT_HANDLE );
  if (hConsoleIn == INVALID_HANDLE_VALUE)
  { DWORD ec = GetLastError();
    mlwrite("Pipeerr %d", ec);
    Beep(1200, 2000);
  }

#if 0
  if (0 == SetConsoleMode(hConsoleIn, ENABLE_WINDOW_INPUT/*|ENABLE_PROCESSED_INPUT*/))
  { DWORD ec = GetLastError();
    mlwrite("PipeC %d", ec);
    Beep(1200, 2000);
  }
#endif
  SetConsoleCtrlHandler(MyHandlerRoutine, true);
#endif
}


int TermThreadView(int cc)

{ if (threadv == NULL)
    return 1;
  
  threadv->PostMessage(ID_APP_EXIT/*WM_DESTROY*/,cc,0);
  return OK;
}



int Pascal NEAR tcapbeep(void)

{ Beep(400, 100);
  return OK;
}



int Pascal NEAR tcapclose()

{ return OK;
}



void CThreadView::StartThread(int ThreadID)
{
  static Bool st_subseq = FALSE;

	CWnd * mw = AfxGetMainWnd();
	if (mw != NULL)
		mw->SetWindowPos( NULL, 400, 300, 600, 700, SWP_SHOWWINDOW );


	CGDIThread* pThread;
	CPoint Pos(rand()%100,rand()%100);
	CPoint Vel1((rand()%10)+2,(rand()%10)+2);
	CPoint Vel2((rand()%5)+2,(rand()%5)+2);
	CSize Size(((rand()%60)+20),((rand()%60)+20));
	COLORREF Color;

	//BLOCK: calculate random color
	{
		int r,g,b;

		do
		{
			r = (rand()%3);
			r = (r!=0) ? (r==2) ? 255 : 127 : 0;

			g = (rand()%3);
			g = (g!=0) ? (g==2) ? 255 : 127 : 0;

			b = (rand()%3);
			b = (b!=0) ? (b==2) ? 255 : 127 : 0;
		}
		while ((r==g) && (g==b));
		// No white(255,255,255), gray(127,127,127), or black(0,0,0) allowed

		Color = RGB(r,g,b);
	}

#if INCWALLPAPER
	switch (ThreadID)
#else
	switch (1)
#endif
	{
	case 1:
		pThread = new CBallThread(this, m_pDC->GetSafeHdc(), Pos, Vel1, Size, Color);
		break;
#if INCWALLPAPER
	case 2:
		pThread = new 
		break;
	case 3:
		pThread = new CLineThread(this,
			m_pDC->GetSafeHdc(), Pos, Vel1, Vel2, Size, Color);
		break;
#endif
	}

	loglog1("pThread %x", pThread);

	if (pThread == NULL)
		return;

	ASSERT_VALID(pThread);
	pThread->m_pThreadParams = NULL;

	// Create Thread in a suspended state so we can set the Priority
	// before it starts getting away from us
	if (!pThread->CreateThread(CREATE_SUSPENDED))
	{
		delete pThread;
		return;
	}

	// since everything is successful, add the thread to our list
	// Only support one thread
	m_pThread = pThread;
	m_threadList.AddTail(pThread);

	// If you want to make the sample more sprightly, set the thread priority here
	// a little higher. It has been set at idle priority to keep from bogging down
	// other apps that may also be running.
	VERIFY(pThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	// Now the thread can run wild


        if (!st_subseq)
	{ st_subseq = TRUE;

	  vtinit();
	  edinit();               /* Buffers, windows */
#if CALLED
	  varinit();              /* user variables */
#endif
#if DIACRIT
	  initchars();            /* character set definitions */
#endif
	  pThread->ResumeThread();
	  loglog("EvScrn");
	  WaitForSingleObject(pThread->m_hEventScrn, INFINITE);
	  loglog("GEvScrn");
     	                          /* Process command line and let the */
    	  (void)dcline(argc, argv);	    
	  ttopen();
	  tcapopen();
	  tcapkopen();            /* open the keyboard */
	  tcaprev(FALSE);
/*  this->OnShow = TForm1::Update; */
	  loglog1("Curbp %s", curbp->b_bname);

	/*update(TRUE);*/
	}
      /*(void)launch_worker();*/
}


void CThreadView::OnDraw(CDC* pdc)

{ if (m_pThread != NULL)
    m_pThread->Paint(pdc);
}

void CThreadView::OnPrint(CDC* pdc)

{ if (m_pThread != NULL)
    m_pThread->Paint(pdc);
}

void CThreadView::OnUpdate(CDC* pdc)

{ if (m_pThread != NULL)
    m_pThread->Paint(pdc);
}

/////////////////////////////////////////////////////////////////////////////
// CThreadView diagnostics

#ifdef _DEBUG
void CThreadView::AssertValid() const
{
	CView::AssertValid();
}

void CThreadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CThreadDoc* CThreadView::GetDocument() // non-debug version is inline
{
	return STATIC_DOWNCAST(CThreadDoc, m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CThreadView message handlers

void CThreadView::OnNewball()
{
	StartThread(1);
	UpdateTitle("Ball");
}

void CThreadView::OnNewrectangle()
{
	StartThread(2);
	UpdateTitle("Rect");
}

void CThreadView::OnNewline()
{
	StartThread(3);
	UpdateTitle("Line");
}

// A preferred method for killing many threads.  Tell them to go away all
// at once, then wait for them all to be scheduled and to terminate
// themselves.  This is much, much faster than the more straight forward
// way of asking just one to terminate, then waiting for it to do so.
// It is also a little bit more complex.

void CThreadView::OnKillThreads()
{
	
	eexitflag = TRUE;

	// tell all threads to shutdown
	for (POSITION pos = m_threadList.GetHeadPosition(); pos != NULL; )
	{
		CGDIThread* pThread = m_threadList.GetNext(pos);
		VERIFY(SetEvent(pThread->m_hEventMain));
	}

	// wait for all threads to finish shutdown
	for (int nThreadsLeft = m_threadList.GetCount(); nThreadsLeft != 0; )
	{
		WaitForSingleObject(CGDIThread::m_hAnotherDead, INFINITE);
		Sleep(nThreadsLeft*2);// 200ms for every 100 threads
		nThreadsLeft = 0;
		for (pos = m_threadList.GetHeadPosition(); pos != NULL; )
		{
			CGDIThread* pThread = m_threadList.GetNext(pos);
			if (WaitForSingleObject(pThread->m_hEventDead, 0) == WAIT_TIMEOUT)
				++nThreadsLeft;
		}
		UpdateTitle("Deleting");
	}

	// delete all thread objects
	while (!m_threadList.IsEmpty())
	{
		CGDIThread* pThread = m_threadList.RemoveHead();
		VERIFY(WaitForSingleObject(pThread->m_hThread, INFINITE) == WAIT_OBJECT_0);
		delete pThread;
	}
	UpdateTitle("Deleted");

	// invalidate the window since all threads are now gone
	Invalidate();
}

// KillThreadsSlow is much easier to understand since the process of
// killing a thread is encapsulated in KillThread.  If you get more
// than 50 threads running, however, you'll notice quite a difference
// between this method and the one above.

void CThreadView::OnThreadKillThreadsSlow()
{
	// Iterate through the threads killing each
	// KillThread doesn't return until the Thread is dead
	while (!m_threadList.IsEmpty())
	{
		m_threadList.RemoveHead()->KillThread();
		UpdateTitle("Slow");
	}

	// Invalidate the window so it blanks the window
	Invalidate();
}

void CThreadView::OnDestroy()
{
	OnKillThreads();

	delete m_pDC;
	m_pDC = NULL;

	CView::OnDestroy();
}

int CThreadView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// m_pDC must be initialized here instead of the constructor
	// because the HWND isn't created until Create is called.
	m_pDC = new CClientDC(this);
	UpdateTitle("Creating");

	loglog1("threadv %x", threadv);
	PostMessage(ID_THREAD_NEWBALL);

	return 0;
}



void CThreadView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)

{ static Bool first = 0;
  if (!first)
  { first = TRUE;
    loglog1("State %x", bActivate);
    OnNewball();
  }
}




void CThreadView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// Instead of getting the client rect every time we draw, we only
	// update it when it changes.
	for (POSITION pos = m_threadList.GetHeadPosition(); pos != NULL; )
		m_threadList.GetNext(pos)->UpdateBorder();
}

void CThreadView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

    loglog3("OnKeyDown %x %x %x", nChar, nRepCnt, nFlags);
#if 1
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
#endif
}

void CThreadView::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    if (m_pThread != NULL)
      m_pThread->Paint(&dc);

    loglog("PAINT");
	// TODO: Add your message handler code here
	
	// Do not call CView::OnPaint() for painting messages
}


void CThreadView::MyPaint()

{ 
   if (threadv != NULL)
     threadv->OnPaint();
}

//---------------------------------------------------------------------------

static int Pascal NEAR usehost(char wh, char *line)

{ int cc;
  if (!clexec)
    tcapmove(term.t_nrowm1, 0);           /* Seek to last line.   */
/*  mlwrite(""); */
  tcapclose();
  ttputc('\n');
  /* write(0, "\n", 1);*/
  sgarbf = TRUE;

#if S_WIN32 == 0
  cc = !system(line);
#else

  char buf[258];
  blankpage();
  buf[0] = 0;
  char * cs = getenv("ComSpec");
  if (cs == NULL || strlen(cs) + strlen(line) + 6 > sizeof(buf))
  { mlwrite("Command Too Big");
    return false;
  }
  concat(&buf[0], cs, " /c ", line, null);
  
  CRedirect wpipe;
  cc = wpipe.Create(buf);

  if (cc == OK)
  { 
    while (true)
    { int offset;
	  Cc cc = wpipe.Read(&offset, &buf[0], sizeof(buf)-1);
      if (cc <= 0)
        break;
      scroll_vscr();
	  
      if (buf[offset] != 0)
      { 
        int len = strlen(buf+offset);
        char ch = buf[offset+ len -1];
	if (ch == 10)
	  buf[offset+--len] = 0;
	ch = buf[offset+len-1];
	if (ch == 13)
	  buf[offset+len-1] = 0;
      }
      loglog1("pres %s", &buf[offset]);
      mlwrite("%s", &buf[offset]);
    }

    wpipe.Close();
  }

#endif

#if MOUSE
  ttopen();
#endif
  ttcol = 0;  		   /* otherwise we shall not see the message */
			   /* if we are interactive, pause here */
  if (!clexec)
  { int s;
	scroll_vscr();
    updallwnd(false);
    mlwrite(cc == OK ? TEXT188 : "[Failed]");
/*             "[End]" */
    while ((s = tgetc()) != '\r' && s != ' ')
      ;
    mlerase();
#ifdef _WINDOWS
    sgarbf = true;
    updallwnd(true);
#endif
  }
/*updallwnd(false);
  sgarbf = true;*/

  return cc == OK;
}


static Pascal NEAR gen_spawn(int f, int n, char * prompt)

{          char line [150];
  register int s;

  if (restflag)
    return resterr();
  
  s=mlreply(prompt, line, NLINE);
  if (s != TRUE)
    return s;

  ttputc('\r'); /* wont work!*/

  s = usehost(*prompt, line);

  return s == OK;
#undef line
}

	/*
	 * Create a subjob with a copy of the command intrepreter. When the
	 * command interpreter exits, mark the screen as garbage so that you 
	 * do a full repaint. Bound to "^X C".
	 */
Pascal NEAR spawncli(int f, int n)

{	if (restflag)		/* don't allow this command if restricted */
	  return resterr();

        usehost('!', "");
        return TRUE;
}

	 /* Run a one-liner in a subjob. When the command returns, wait for 
	  * a single character to be typed, then mark the screen as garbage 
	  * so a full repaint is done. Bound to "C-X !".
	  */
Pascal NEAR spawn(int f, int n)

{ return gen_spawn(f, n, "!");
}

	/* Run an external program with arguments. When it returns, wait 
	 * for a single character to be typed, then mark the screen as 
	 * garbage so a full repaint is done. Bound to "C-X $".
	 */
Pascal NEAR execprg(int f, int n)

{ return gen_spawn(f, n, "$");
}

//---------------------------------------------------------------------------


typedef struct
{ CRedirect * wpipe;
  LINE * lp;
} PipeLines_t;


void WriteThread(PipeLines_t * pl)

{ CRedirect * wpipe = pl->wpipe;
  LINE * lp = pl->lp;

  while (((lp = lforw(lp))->l_props & L_IS_HD) == 0)
  { 
      int got = llength(lp); 
      int len = wpipe->Write(&lp->l_text[0], 0, got-1);
      if (len == got)
	len += wpipe->Write("\n", 0, 0);
      if (len != got+1)
      { logwarn("Chopped Writing");
	break;
      }
  }
  char zr[] = "Z\n";
  zr[0] = 'Z' - '@';
  wpipe->Write(zr, 0, 1);
  wpipe->CloseWrite();
}


        char prompt[2];

	/* Pipe a one line command into a window
	 * Bound to ^X @ or ^X #
	 */
static Pascal NEAR pipefilter(char wh)

{
 static const char bname [] = "_command";

        char line[NSTRING+1];       /* command line send to shell */

	if (restflag)		/* don't allow this command if restricted */
	  return resterr();
	prompt[0] = wh;
{	int s;
	char buf[150];
	BUFFER * bp;
	wh -= '@';
	if (wh != 0 and (curbp->b_flag & MDVIEW)) /* disallow this command if */
	  return rdonly();			  /* we are in read only mode */

        if (mlreply(prompt, line, NLINE) != TRUE)
          return FALSE;
			/* get rid of the command output buffer if it exists */
	bp = curbp;
	if (wh == 0)
	{ bp = bfind(bname, TRUE, 0);
          if (bp == null)
	    return FALSE;
	  bp->b_flag &= ~MDVIEW;
        }

	if (wh == 0)
	{ onlywind(FALSE, 1);

	  if (splitwind(FALSE, 1) == FALSE)
	    return FALSE;
        }

	tcapmove(term.t_nrowm1, 0);

	loglog1("PipeCmd %s", line);

        buf[0] = 0;
        char * cs = getenv("ComSpec");
        if (cs == NULL || strlen(cs) + strlen(line) + 6 > sizeof(buf))
        { mlwrite("Command Too Big");
          return FALSE;
        }
        concat(&buf[0], cs, " /c ", line, null);

        CRedirect wpipe;
	s = wpipe.Create(buf, 0, wh != 0);
	if (s == OK)
        { HANDLE TName;
		  PipeLines_t pl_t;
		  pl_t.wpipe = &wpipe;
		  pl_t.lp = curbp->b_baseline;
          
	  if (wh != 0)
	  {	    DWORD ThreadID;
            TName = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteThread,
					&pl_t, CREATE_SUSPENDED, &ThreadID);
  	    if (TName == NULL)
	    {
	      loglog("Could not create thread!");
    	      return FALSE;
	    }
			
	    if ((ResumeThread(TName)) == -1)
	    {
	      loglog("ResumeThread failed!");
	      return FALSE;
	    }
          }
  
	  Char * bufnm = NULL;
	  (void)remallocstr(&bufnm, bp->b_bname, 100);
	  strcat(&bufnm[0], "\n");

	  BUFFER * op = bfind(bufnm, TRUE, 0);
	  free(bufnm);
	  swbuffer(op);

	  while (true)
          { int offset;
	    Cc cc = wpipe.Read(&offset, &buf[0], sizeof(buf)-1);
            if (cc <= 0)
              break;
	  
            int len_ = strlen(buf+offset);
	    int len = len_;
	    if (len > 0)
      	    { 
              char ch = buf[offset + len -1];
	      if (ch == 10)
	        --len;
	      ch = buf[offset+len-1];
	      if (ch == 13)
	        --len;
	      if (len < len_)
	      { buf[offset+len] = '\n';
	        buf[offset+len+1] = 0;
	      }

	      linstr(&buf[offset]);
	      loglog1("Pipe %s", &buf[offset]);
            }
          }

	  LINE * lp = op->b_baseline;

	  init_paren(0);

	  while (((lp = lforw(lp))->l_props & L_IS_HD) == 0)
          { lp->l_props = paren.in_mode & Q_IN_CMT;
	    scan_par_line(lp);
	  }

	  if (wh != 0)
	  { WaitForSingleObject(TName, INFINITE);  // discussed later

	    CloseHandle(TName);
          }

          wpipe.Close();

	  op->b_fname = bp->b_fname;
          bp->b_fname = NULL;
	  strcpy(&op->b_bname[0], bp->b_bname);

			   /* get rid of the existing command buffer */
	  if (zotbuf(bp) != TRUE)
	    return FALSE;
        }

	gotobob(0,0);
	(void)orwindmode(WFMODE, 0);

	if (wh == 0)
	  bp->b_flag |= MDVIEW;
			 	       /* did the output file get generated? */
{/* 	int fid = open(tmpnam, O_RDONLY);
	if (fid < 0)
	  return FALSE;
	close(fid);
*/

	return s == OK;
}}}


	/* Pipe a one line command into a window
	 * Bound to ^X @
	 */
Pascal NEAR pipecmd(int f, int n)

{ return pipefilter('@');
}

	/*
	 * filter a buffer through an external DOS program
	 * Bound to ^X #
	 */
Pascal NEAR filter(int f, int n)

{ return pipefilter('#');
}

//---------------------------------------------------------------------------

int Pascal NEAR transk(int k, int ext)

{ return k;
}



unsigned short in_buf[IBUFSIZE];	/* input character buffer */
Short in_next = 0;		/* pos to retrieve next input character */
Short in_last = 0;		/* pos to place most recent input character */


				/* get an event from the input buffer */

int g_k;


static const char scantokey[] =
{	 '1',		/* 3b */
         '2',
         '3',
         '4',
         '5',
         '6',
         '7',
         '8',
         '9',
         '0',		/* 44 */
         'W',		/* pause */
         'L',		/* Scroll Lock */
         '<',		/* Home */
         'P',		/* Up */
         'Z',		/* Page Up */
         'J',		/* key not know *//* 4a */
         'B',		/* Left */
         'K',		/* key not known */
         'F',		/* Right */
         'L',		/* key not known */
         '>',		/* End */
         'N',		/* Down */
         'V',		/* Page Down */
         'C',		/* Insert */
         'D',		/* Delete */
         'Q',		/* key not known *//* 54 */
         'q',		/* key not known */
         'x',		/* key not known */
         ':',
         ';',		/* 58 */
};


BOOL CThreadView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message != WM_MOUSEMOVE)
          loglog3("PreTM %x %x %x", pMsg->message, pMsg->wParam, pMsg->lParam);
	  
        if      (pMsg->message == WM_KEYUP)
        { int k = pMsg->wParam;
	  if      (k == 'P'-'@')
          { if (kb_mode & SHFT)
	    { /*loglog("SHIFT-OFF");*/
	      kb_mode &= ~SHFT;
	    }          
	    return TRUE;
	  }
	  else if (k == 'Q'-'@' || k == 0x74) /* why lc letter t ? */
          { if (kb_mode & CTRL)
	    { loglog("CTRL-OFF");
	      kb_mode &= ~CTRL;
	    }          
	    return TRUE;
	  }/*
	  else
	    loglog2("PreUp %x %x", pMsg->wParam, pMsg->lParam);*/
        }
	else if (pMsg->message == WM_KEYDOWN or /* 0x100 */
	         pMsg->message == WM_SYSKEYDOWN)  /* 0x104 */
	{ 
	  int k = pMsg->wParam;
	  int hex = (pMsg->lParam>>16) & 0x7f;
	  int ch = MapVirtualKey(hex, 1);

	  if (k == 'P'-'@')
          { if ((kb_mode & SHFT) == 0)
	    { /*loglog("SHIFT-ON");*/
	      kb_mode |= SHFT;
	    }
	    return TRUE;
          }
	  if (k == 'Q'-'@')
          { if ((kb_mode & CTRL) == 0)
	    { loglog("CTRL-ON");
	      kb_mode |= CTRL;
	    }
	    return TRUE;
          }
	  /*loglog4("PreDown %c (%x) %x MVK %x", k, k, pMsg->lParam, ch);*/
          if (/*(pMsg->lParam & 0x1000000) && */ in_range(hex, 0x3b, 0x58)
	      && hex != 0x56
             )
             k = kb_mode | SPEC | scantokey[hex - 0x3b];
	  else		/*abcdef0*/
	  { static char tr1[] =  ";=,-./' :+<_>?@ ";
	    static char tr2[] = "[\\]#`   {|}~ª   ";
	    static char tr3[] = ")!\"œ$%^&*(";
            if      (in_range(k, 0xba, 0xc0))
            { k = tr1[(k-0xba)+(kb_mode & SHFT ? 8 : 0)];
              if (kb_mode & CTRL)
	        k = 0;
            }
	    else if (in_range(k, 0xdb, 0xdf))
            { k = tr2[(k-0xdb)+(kb_mode & SHFT ? 8 : 0)];
              if (kb_mode & CTRL)
		if      (k == '`' || k == 0xaa)
		  k = 0xdd;
	        else if (k == '[' || k == ']')
		  k |= CTRL;
	        else
	          k = 0;
            }
	    else if (in_range(k, '0', '9') && (kb_mode & SHFT))
	      k = tr3[k - '0'];
	    else if (kb_mode == 0 && in_range(k, 'A', 'Z'))
	      k += 'a' - 'A';

	    else if ((kb_mode & CTRL) && in_range(k, '@', 'Z'))
	    { k -= '@';
	      k |= CTRL;
	     
	      if (k == (CTRL | ('X'-'@')))
	      { kb_mode |= CTLX;
	        /*loglog("Prefix C-X");*/
	        return TRUE;
	      }
	    }
	    if (kb_mode & CTLX)
	      k |= CTLX;
	  }
	/*loglog4("PreKeyDown %c scan %x map %x -> %x", k & 0x7f, hex, ch, k);*/

	  if (true)	/* not STRICTLY atomic but who cares? */
	  { MainPut(k);
	    kb_mode &= ~CTLX;
	  }

	  return TRUE;
	}
	else if (pMsg->message == WM_CHAR)
	{ loglog3("PreKey %c %d %x", pMsg->wParam, pMsg->wParam, pMsg->lParam);
	  return TRUE;
	}
	else if (pMsg->message == WM_NCMOUSEMOVE)
	{ loglog2("MouseMove %d %d", pMsg->wParam, pMsg->wParam);
          if (m_pThread != NULL)
	  { CPaintDC dc(this); // device context for painting
            m_pThread->Resize();
	    update(TRUE);
            m_pThread->Paint(&dc);
          }
	}
	else if (pMsg->message == ID_APP_EXIT)
	{ loglog1("Destroy %d", pMsg->wParam);
	  CThreadView::OnDestroy();
	  exit(pMsg->wParam);
	}
	else if (pMsg->message != WM_MOUSEMOVE)
	  loglog3("EVENT %x %x %x", pMsg->message, pMsg->wParam, pMsg->lParam);
    
/* loglog4("PreKeyDown %d   %d %d %d", WM_KEYDOWN, pMsg->message, pMsg->wParam, pMsg->lParam);*/
        /*CDialog::PreTranslateMessage(pMsg);*/
	return FALSE;
}

//---------------------------------------------------------------------------

static int  eaten_char = -1;		 /* Re-eaten char */


void Pascal NEAR reeat(int ch)

{
    eaten_char = ch;			/* save the char for later */
}



Pascal NEAR ttgetc()

{ if (eaten_char != -1)
  { int ch = eaten_char;
    eaten_char = -1;
    return ch;
  }
  
  if (!in_check())
  { 
    while (WaitForSingleObject(CGDIThread::m_hEventMain,1000) != WAIT_OBJECT_0)
      if (eexitflag)
	return -1;
  }

  int k = in_get();
  loglog1("Emacsgets %x", k);
  return k;
}


#if	TYPEAH
	/* typahead:	See if any characters are already in the keyboard buffer
	*/
Pascal NEAR typahead()

{ return FALSE;
}

#endif



BOOL CThreadView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
#if 0
    CRect rct(85, 110, 280, 250);

    m_cedit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP |
             ES_AUTOHSCROLL | WS_BORDER | WS_THICKFRAME, 
			 rct, this, ID_EDIT_BOX);
    m_cedit.SetFocus();
#endif
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}



void Pascal NEAR mbwrite(const char * msg)

{
#if S_WIN32
    HWND mwh = /*GetTopWindow(NULL);*/GetFocus();
    MessageBox(mwh, msg, "Emacs", MB_OK | MB_SYSTEMMODAL);
    SetActiveWindow(mwh);
#endif
}

