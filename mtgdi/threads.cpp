// threads.cpp : contains all the different GDI thread classes
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
#include <windows.h>

#include "mtgdi.h"
#include "threads.h"
#include "MYCOLORS.h"


#include "estruct.h"
#include "edef.h"
#include "etype.h"
#include "epredef.h"
#include "elang.h"
#include "logmsg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

Bool shutthreads = FALSE;

// critical section to protect while drawing to the DC
CRITICAL_SECTION CGDIThread::m_csGDILock;

// m_hAnotherDead is used to signal that one or more threads have ended
//  (it is an auto-reset event; and starts out not signaled)
HANDLE CGDIThread::m_hEventMain   = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE CGDIThread::m_hEventScrn   = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE CGDIThread::m_hAnotherDead = CreateEvent(NULL, FALSE, FALSE, NULL);

/////////////////////////////////////////////////////////////////////////////
// CGDIThread

IMPLEMENT_DYNAMIC(CGDIThread, CWinThread)

BEGIN_MESSAGE_MAP(CGDIThread, CWinThread)
	//{{AFX_MSG_MAP(CGDIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
;

CGDIThread * thrwind;

COLORREF	cfcolor = clBlack;		/* current forground color */
COLORREF	cbcolor = clLtGray;		/* current background color */


CGDIThread::CGDIThread(CWnd* pWnd, HDC hDC)
{
	m_bAutoDelete = FALSE;
	m_pMainWnd = pWnd;
	m_pMainWnd->GetClientRect(&m_rectBorder);
	m_hDC = hDC;

	// kill event starts out in the signaled state
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
        
        cursor_off = FALSE;
}



void CGDIThread::Resize()
{	
	(void)GetClientRect(m_pMainWnd->m_hWnd, &clientarea);
	Height = clientarea.bottom - clientarea.top;
	Width = clientarea.right - clientarea.left;
	CSize fsz = m_dc.GetOutputTextExtent(CString(" "));
        y_step = 13; /* Canvas->TextHeight("A");*/
	x_step =  fsz.cx;
        noo_row = this->Height / y_step;
	if (noo_row > term.t_mrowm1)
	    noo_row = term.t_mrowm1;
	noo_col = this->Width / x_step;
	EEop();
	newdims(noo_col, noo_row);
}



void MainPut(int k)

{ Short dif = in_last - in_next;
  if (dif == IBUFSIZE-1)
    tcapbeep();
  else
  { 				/* event to enter into the input buffer */
    in_buf[in_last & (IBUFSIZE - 1)] = k;
    in_last += 1;

    if (dif == 0)
      VERIFY(SetEvent(CGDIThread::m_hEventMain));
  }
}


void MainGet()

{ 
  if (!in_check())
    while (WaitForSingleObject(CGDIThread::m_hEventMain,1000) != WAIT_OBJECT_0)
      if (eexitflag)
        break;
}

// The reason we don't just get a CDC from our owner and simply use it is because
// MFC GDI objects can't be passed between threads.  So we must instead pass a
// handle and then reconvert them back to MFC objects.  The reason  for this is
// because MFC maintains a list of all GDI objects on a per thread basis.  So if
// you pass a GDI object to another thread, it won't be in the correct list
// and MFC will assert.

BOOL CGDIThread::InitInstance()
{
	// thread setup
	m_dc.Attach(m_hDC);

	in_paint = false;
	cbcolor = RGB(180, 180, 180);	/* current background color */

	textrow = 0;
	textcol = 0;
	y_row = 0;
	y_coord = 0;

#if 1
	int ft_height = 13;
	CFont bont;
        /*font.CreatePointFont(8, "Ariel", &dc);*/
    	bont.CreateFont(-ft_height, 0, 0, 0, 0/*FW_BOLD*/, FALSE, FALSE, 0, 
			 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			  DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Courier");
        CFont * pFOld = m_dc.SelectObject(&bont);
/*	SetFont(&bont, false);*/
#endif
	Resize();

	loglog3("H %d %d %d", Height, noo_row, noo_col);
	loglog2("XY step %d %d", x_step, y_step);

	for (int ix = upper_index(scrsrc)+1; --ix >= 0; )
	{ scrsrc[ix] = 0;
	}

	if (thrwind != NULL)
	  logwarn("Two CGDIThreads");

	thrwind = this;
	loglog1("Thrwind %x", thrwind);
#if 1
	VERIFY(SetEvent(m_hEventScrn));
	eexitflag = false;

	while (!eexitflag)
	{ int c;
	  lastflag = 0;		/* Fake last flags.*/
	   			/* execute the "command" macro...normally null*/
	  execkey(&cmdhook, FALSE, 1);	/* used to push/pop lastflag */
				
	  update(FALSE);		/* Fix up the screen	*/
				   
	  loglog("WEvScr");

	  MainGet();

	  loglog("GO");
	  c = getkey();		/* get the next command from the keyboard */
	  loglog1("KEYLOOP %x", c);
	  eexitval = editloop(c);
	}


        TermThreadView(eexitval);

	if (!eexitflag)
					// loop but check for kill notification
	  while (WaitForSingleObject(m_hEventMain, 0) == WAIT_TIMEOUT)
		SingleStep();

					// thread cleanup
	m_dc.Detach();
#endif
	// avoid entering standard message loop by returning FALSE
	return FALSE;
}




void CGDIThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
	VERIFY(SetEvent(m_hAnotherDead));
}

CGDIThread::~CGDIThread()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

void CGDIThread::KillThread()
{
	// Note: this function is called in the context of other threads,
	//  not the thread itself.

	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	// now delete CWinThread object since no longer necessary
	delete this;
}

void CGDIThread::UpdateBorder()
{
	// Since another thread can access this variable, we block them when we write
	// it. In this case, it is overkill since the other thread would just animate
	// in the wrong direction while reading intermediate values and finally adjust
	// itself when the new value is valid.  It is a good idea to avoid such
	// anomalies, however.

	EnterCriticalSection(&CGDIThread::m_csGDILock);
	{
		m_pMainWnd->GetClientRect(&m_rectBorder);
	}
	LeaveCriticalSection(&CGDIThread::m_csGDILock);
}



#define BG_CURSOR RGB(247,247,255)
#define BG_PAPER  RGB(223,223,223)


static short sctop = 0;		/* zero based */
static short scbot = 25;  	/* zero based */ /* set by tcapopen */


const COLORREF ctrans[] =		/* ansi to Win32 color translation table */
	{clBlack, clBlue, clGreen, clYellow, clRed, clPurple, clAqua, BG_PAPER,
	 /*8, 12, 10, 14, 9, 13, 11, 15*/};

/* editor variable: pd_cmt_colour;		   comment colour in ibm */


void Pascal NEAR tcapsetsize(int wid, int dpth)

{ 
/*COORD size;
  size.X = wid;
  size.Y = dpth;
  
  SetConsoleScreenBufferSize( hConsoleOut, size);*/
}



void Pascal NEAR tcapopen()

{
  if (thrwind != NULL)
  { scbot = thrwind->noo_row - 1;

    newdims(thrwind->noo_col, scbot+1);
  }
/*vtinit();*/
}


int Pascal NEAR tcapcres(char * res) /* change screen resolution */

{
  return TRUE;
}


#if	COLOR

void Pascal NEAR tcapbfcol(unsigned int color)/* set the current output color */

{ /*cbcolor = ctrans[(color>>8) & 7];
  cfcolor = ctrans[(color & 0xf) & 7];*/
}
#endif


void Pascal NEAR tcapscreg(int row1, int row2)

{ sctop = row1;
  scbot = row2;
}


COLORREF Pascal NEAR hlcolour(COLORREF chr)

{ COLORREF res = 0;
  COLORREF resa = 0;
  int dist = 0;

  for (int shft = 24; (shft -= 8) >= 0; )
  { int rgb = chr >> shft;
    if (rgb <= 240)
    { dist += 1;
      rgb += 16;
    }
    res |= (rgb << shft);
    resa |= (256 - (rgb <<shft));
  }
  if (dist == 0)
    res = resa;
  return res;
}


static int oldcurx;
static int oldcury;
static COLORREF  oldattrb = clLtGray;
static COLORREF  oldattrf = clBlack;


void Pascal NEAR tcapmove(int row, int col)

{ ttrow = row;
  ttcol = col;
 
  if (thrwind != NULL && !thrwind->cursor_off)
  { thrwind->textrow = row;
    thrwind->textcol = col;

#if 1
    short * scrline = get_vscr_line(oldcury);
    WORD color = window_bgfg(curwp);
	         
    thrwind->PaintRow(oldcury, &scrline[oldcurx],oldattrb, oldattrf, oldcurx);
    oldattrf = ctrans[(color) & 7];
    oldattrb = ctrans[(color >> 4) & 7];
    if (row == term.t_mrowm1)
      oldattrb = clLtGray;

    oldcury = ttrow;
    oldcurx = ttcol;
    scrline = get_vscr_line(oldcury);
    thrwind->PaintRow(oldcury, &scrline[oldcurx], 
			hlcolour(oldattrb), oldattrf, oldcurx);
#endif
  }
}



void Pascal NEAR ttscupdn(int sdist)    /* direction the window moves */

{ if (thrwind != NULL)
  { thrwind->Scroll(sdist);
  }
}



void CGDIThread::Scroll(int sdist)

{ int from = 0;

#if 0
  char keyString[30];
  sprintf(&keyString[0], "SCROLL %d %d", sdist, Row);
  Application->MessageBox(keyString , "Message Box", MB_OK);
#endif
/*(void)GetClientRect(m_pMainWnd->m_hWnd, &clientarea);

  int h = clientarea.bottom - clientarea.top;
  int w = clientarea.right - clientarea.left;
  loglog2("h %d w %d", h, w);*/

/*ScrollWindow(m_pMainWnd->m_hWnd, 0, sdist * y_step, NULL, &clientarea);*/
}


int Pascal NEAR millisleep(unsigned int n)

{ return OK;
}




int Pascal NEAR tcapbeeol(int row, int col) /* erase to the end of the line */

{ if (thrwind == NULL)
    return -1;

  thrwind->EEol(row, col);
  return OK;
}



void Pascal NEAR tcapeeol()	/* erase to the end of the line */

{ tcapbeeol(ttrow, ttcol);
}

//---------------------------------------------------------------------------


void Pascal NEAR tcapeeop()

{ if (thrwind != NULL)
  { for (int myrow = ttrow -1; ++myrow < thrwind->noo_row; )
     tcapbeeol(myrow, 0);
  }
}



void CGDIThread::EEop()

{ CRgn rgn;
  int y = clientarea.top;
  rgn.CreateRectRgn(/*x*/clientarea.left,
		    /*y*/y,
		    /*x*/clientarea.right,  clientarea.bottom);
  CBrush br(cbcolor);

  loglog("Eop");

  (void)m_dc.FillRgn( &rgn, &br );
}




void CGDIThread::EEol(int myrow, int mycol)

{ if (myrow >= noo_row || mycol >= noo_col)
    return;

  CRgn rgn;
  int y = clientarea.top + myrow * y_step;
  rgn.CreateRectRgn(/*x*/clientarea.left + mycol * x_step, 
		    /*y*/y,
		    /*x*/clientarea.right,  y + y_step);
  CBrush br(cbcolor);

  loglog2("Eol %d %d", myrow, mycol);

  (void)m_dc.FillRgn( &rgn, &br );
}


Bool Pascal NEAR cursor_on_off(Bool on)

{ Bool res = true;
  if (thrwind != NULL && false)
  { res = thrwind->cursor_off;
	thrwind->cursor_off = !on;
  }

  return res;
}


/* modeline put character
 */
int Pascal NEAR ttputc(char c)

{ short buf[2];
  buf[0] = c;
  buf[1] = 0;
  if (thrwind != NULL)
  { 
    thrwind->PaintRow(thrwind->noo_row-1, static_cast<const short*>(buf),
			BG_PAPER/*RGB(0,200,255)*/, clBlack, ttcol);
  }
  return OK;
}


						/* write a line out */
int Pascal NEAR scwrite(int row, short * outstr, int color)	
	/* int    row;	* row of screen to place outstr on */
	/* short *outstr;	* string to write out (must be term.t_ncol long) */
{		    /* build the attribute byte and setup the screen pointer */
  if (thrwind == NULL)
    return -1;

/*loglog1("SCWRITE %d", row);*/

  COLORREF cb = ctrans[(color >> 4) & 7];
  thrwind->PaintRow(row, outstr, cb, ctrans[(color) & 7], 0, term.t_ncol-1);
  return OK;
}


#if 1

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGDIThread::Paint(CDC* pdc) 
{
  for (int mr = -1; ++mr < noo_row; )
  { char colors = get_vscr_colors(mr);
    COLORREF cb = ctrans[(colors >> 4) & 7];
    COLORREF cf = ctrans[(colors) & 7];

    if (mr == noo_row - 1)
      loglog1("NOOR %d", noo_row);

    PaintRow(mr, get_vscr_line(mr), cb, cf, 0, term.t_ncol-1);
  }
}

#endif



void CGDIThread::PaintRow(int myrow, const short * src, 
			    COLORREF clrb, COLORREF clrf, int xstt, int xlim)
{
  static int maxrow = 0;
  m_dc.SetTextColor(clrf);
/* RGB(256, 0, 0)*/;
  m_dc.SetBkColor(clrb);
  
 /*LPRECT rectClient;*/
 /*GetClientRect(&rectClient);*/
 /*CPoint pt(20, 20);*/
 /*CSize sz(100, 1600);*/
 /*CBrush br(RGB(128, 128, 64));*/
 /*m_dc.DrawState(pt, sz, str, 0, TRUE, 5, &br);*/
/*
  m_dc.SelectObject(&bont);
  m_dc.TextOutA(20, 20+26, str, 5);
  m_dc.TextOutA(20, 20+39, "Second Line", 11);
  m_dc.SelectObject(pPenOld);
  m_dc.SelectObject(oldft);
*/
  char spbuf[300];
  int six = 0;

  int x = clientarea.left + xstt * x_step;
  int y = clientarea.top + myrow * y_step;
  int h = 0;
  int pause = 0;
  COLORREF pushclr = clrf;
  COLORREF nxtclr = clrf;
/*
  if (myrow == noo_row-1)
    loglog2("PML %d %d %d", myrow, textrow, textcol);
*/
/*loglog2("Row Col %d %d", textrow, textcol);*/

  int lim = xlim == 0 ? xstt+1 : xlim < noo_col ? xlim+1 : noo_col+1;

  for (int mycol = xstt-1; ++mycol <= lim; ++src)

  { if (textrow == myrow && mycol == textcol && !cursor_off)
      pause = 4;
    if (mycol == lim)
    { pause |= 2;
    }
    else
    { if (*src & 0x7f00)
      { pushclr = nxtclr;		/* push */
        nxtclr = ctrans[((*src & 0x7f00) >> 8) & 7];
        spbuf[six] = 0;
        CString str = spbuf;
	if (six > 0)
        { if (myrow >= maxrow)
            loglog3("Bef cmt %d %d %s", myrow, mycol, spbuf);
          m_dc.TextOut(x, y, str);
          x += six * x_step;
	  six = 0;
        }
        m_dc.SetTextColor(nxtclr);
        /*loglog1("Push %x", nxtclr);*/
      }
      spbuf[six++] = *src & 0xff;
      if (spbuf[six-1] == '\t')
        loglog("TABPROB");
    /*if (spbuf[six-1] == 0)
        loglog("ZEROPROB");*/
      if (*src & 0x8000)	/* pop */
      { nxtclr = pushclr;
        pause = 1;
      }
    }
    if (pause)
    { 
    /*loglog3("TO %d %d %s", x, y, spbuf *str.GetBuffer(100)*);*/
      /*if (str != "")*/
      if (pause == 4)
        --six;
      if (six > 0)
      { spbuf[six] = 0;
        CString str = spbuf;
      /*if (myrow >= maxrow)
          loglog3("MSTR %d %d %s", myrow, mycol, spbuf);*/
      
        m_dc.TextOut(x, y, str);
        x += six * x_step;
      }

      six = 0;
      if (pause == 4)
      { spbuf[0] = 0;	/* prevent garbage in CMDLINE diagnostic */
        m_dc.SetBkColor(BG_CURSOR);
        m_dc.TextOut(x, y, (char)(*src & 0xff));
        m_dc.SetBkColor(clrb);
        if (myrow >= maxrow)
          loglog2("Cur %d %x", myrow, (char)(*src & 0xff));
        x += x_step;
      }
      pause = 0;
      m_dc.SetTextColor(nxtclr);
    }
  }

  if (myrow >= maxrow)
  { maxrow = myrow;
  /*loglog3("CMDLINE %d %d %s.", x, myrow, spbuf);*/
  }

  GdiFlush();
  m_dc.SetBkColor(cbcolor);
/* m_dc.SelectObject(oldft);*/

  in_paint = false;
}




// ******************************************************************

// Clipboard Functions


HANDLE  m_hClipData;
HANDLE  m_hData;
Int     m_DataSize;
Int     m_StrLen;
char *  m_lpData;

Char * ClipAlloc(int size)

{ 
#if 1
  return NULL;
#else
  HANDLE hData = GlobalAlloc(GMEM_DDESHARE, size);
  if (!hData)
    return NULL;

{ char * lpData = (char*)GlobalLock(hData);
  if (lpData != NULL)
  { lpData[0] = 0;
    if (m_lpData != NULL)
    { strcpy(lpData, m_lpData);
      GlobalUnlock(m_hData);
    }
    m_hData = hData;
    m_lpData = lpData;
  }
  return lpData;
}
#endif
}



Char * ClipRef()

{
#if 1
  return NULL;
#else
  if (m_lpData != NULL)
    return m_lpData;
  
  return m_hData == NULL ? NULL : (m_lpData = (char*)GlobalLock(m_hData));
#endif
}




Cc ClipSet(char * src)

{ Bool extend;
  Int len = strlen(src);
  HWND mwh = GetTopWindow(NULL);
  if (mwh == NULL)
    return -1;

#if 0
  GlobalUnlock(m_hData);
  m_lpData = NULL;
  
  if (!OpenClipboard(/*m_pMainWnd->*/mwh))
    return -1;
    
  EmptyClipboard();
  if (m_hData)
    SetClipboardData(CF_TEXT, m_hData);
  CloseClipboard();
#else

  extend = /*len <= m_DataSize*/ false;
  if (!extend)
  { m_StrLen = len;
    m_DataSize = len + KBLOCK*20;
  
    m_hData = GlobalAlloc(GMEM_DDESHARE, m_DataSize+10);
    if (!m_hData)  
      return -1;
  }
  
  m_lpData = (char *)GlobalLock(m_hData);
  if (m_lpData == NULL)
    return -1;
  
  strcpy(&m_lpData[0], src);
  GlobalUnlock(m_hData);

  if (OpenClipboard(mwh))
  { if (!extend)
	          /* Clear the current contents of the clipboard, and set
  		   * the data handle to the new string.
	  	   */
    {  EmptyClipboard();
       SetClipboardData(CF_TEXT, m_hData);
    }
    CloseClipboard();
  }
  /*m_hData = NULL;*/
  
#endif  
  return OK;
}


Cc ClipDelete()

{ 
#if 1
  return OK;
#else
  HWND mwh = GetTopWindow(NULL);
  if (mwh == NULL)
    return -1;

  if (!OpenClipboard(/*m_pMainWnd->*/mwh))
    return -1;

  EmptyClipboard();
  CloseClipboard();
/*
  if (m_hData)
    GlobalFree(m_hData);
*/
  m_hData = 0;
  return OK;
#endif
}



char * ClipPasteStart()

{ HWND mwh = GetTopWindow(NULL);
  if (mwh != NULL &&
      OpenClipboard(mwh))
  { m_hClipData = GetClipboardData(CF_TEXT);
    if (m_hClipData)
      return (char *)GlobalLock(m_hClipData);
  }

  loglog("PasteFailed");
  return null;
}



void ClipPasteEnd()

{ 
  if (m_hClipData)
  { GlobalUnlock(m_hClipData);
    m_hClipData = NULL;
  }
  CloseClipboard();
}




/////////////////////////////////////////////////////////////////////////////
// CBallThread

IMPLEMENT_DYNAMIC(CBallThread, CGDIThread)

BEGIN_MESSAGE_MAP(CBallThread, CGDIThread)
	//{{AFX_MSG_MAP(CBallThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CBallThread::CBallThread(CWnd* pWnd, HDC hDC, CPoint ptPos, CPoint ptVel,
	CSize Size, COLORREF color)
	: CGDIThread(pWnd,hDC)
{
	m_rectPosition.SetRect(ptPos.x,ptPos.y,ptPos.x+Size.cx,ptPos.y+Size.cx);
	m_ptVelocity = ptVel;

	CBrush brush(color);
	m_hBrush = (HBRUSH)brush.Detach();
}

// The reason we don't just store the CBrush from our owner and simply use it is
// because MFC GDI objects can't be passed between threads.  So we must instead
// pass a handle and then reconvert them back to MFC objects.  The reason for
// this is because MFC maintains a list of all GDI objects on a per thread basis.
// So if you pass a GDI object to another thread, it won't be in the correct
// list and MFC will assert.

BOOL CBallThread::InitInstance()
{
	m_brush.Attach(m_hBrush);

	return CGDIThread::InitInstance();
}

void CBallThread::SingleStep()
{
	m_rectPosition.OffsetRect(m_ptVelocity);

	if (m_rectPosition.top<m_rectBorder.top)
		m_ptVelocity.y = (m_ptVelocity.y>0) ? m_ptVelocity.y : -m_ptVelocity.y;

	if (m_rectPosition.bottom>m_rectBorder.bottom)
		m_ptVelocity.y = (m_ptVelocity.y>0) ? -m_ptVelocity.y : m_ptVelocity.y;

	if (m_rectPosition.left<m_rectBorder.left)
		m_ptVelocity.x = (m_ptVelocity.x>0) ? m_ptVelocity.x : -m_ptVelocity.x;

	if (m_rectPosition.right>m_rectBorder.right)
		m_ptVelocity.x = (m_ptVelocity.x>0) ? -m_ptVelocity.x : m_ptVelocity.x;

	// Since all threads share the same HDC it is necessary
	// to block all other threads while we render in the HDC
	EnterCriticalSection(&CGDIThread::m_csGDILock);
	{
		CBrush* oldbrush;

		oldbrush = m_dc.SelectObject(&m_brush);
		m_dc.Ellipse(m_rectPosition);
		m_dc.SelectObject(oldbrush);

		// Win32 optimizes GDI calls by collecting them in a batch
		// and then thunking the whole batch at once on a per
		// thread basis.  Since we share an HDC with multiple
		// threads, we must flush the batch before yielding to
		// other threads that will adjust the HDC.  To see what
		// I mean, comment out the GdiFlush() function call and
		// watch the results.
		GdiFlush();
	}
	LeaveCriticalSection(&CGDIThread::m_csGDILock);
}

/////////////////////////////////////////////////////////////////////////////
//
// CRectThread
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CRectThread, CGDIThread)

BEGIN_MESSAGE_MAP(CRectThread, CCmdTarget)
	//{{AFX_MSG_MAP(CRectThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CRectThread::CRectThread(CWnd* pWnd, HDC hDC, CPoint ptPos, CPoint ptVel,
	CSize Size, COLORREF color) : CGDIThread(pWnd, hDC)
{
	m_rectPosition.SetRect(ptPos.x ,ptPos.y, ptPos.x+Size.cx, ptPos.y+Size.cy);
	m_ptVelocity = ptVel;

	CBrush brush(color);
	m_hBrush = (HBRUSH)brush.Detach();
}

BOOL CRectThread::InitInstance()
{
	m_brush.Attach(m_hBrush);

	return CGDIThread::InitInstance();
}

void CRectThread::SingleStep()
{
	m_rectPosition.OffsetRect(m_ptVelocity);

	if (m_rectPosition.top<m_rectBorder.top)
		m_ptVelocity.y = (m_ptVelocity.y>0) ? m_ptVelocity.y : -m_ptVelocity.y;

	if (m_rectPosition.bottom>m_rectBorder.bottom)
		m_ptVelocity.y = (m_ptVelocity.y>0) ? -m_ptVelocity.y : m_ptVelocity.y;

	if (m_rectPosition.left<m_rectBorder.left)
		m_ptVelocity.x = (m_ptVelocity.x>0) ? m_ptVelocity.x : -m_ptVelocity.x;

	if (m_rectPosition.right>m_rectBorder.right)
		m_ptVelocity.x = (m_ptVelocity.x>0) ? -m_ptVelocity.x : m_ptVelocity.x;

	// Since all threads share the same HDC it is necessary
	// to block all other threads while we render in the HDC
	EnterCriticalSection(&CGDIThread::m_csGDILock);
	{
		CBrush* oldbrush;

		oldbrush = m_dc.SelectObject(&m_brush);
		m_dc.Rectangle(m_rectPosition);
		m_dc.SelectObject(oldbrush);

		// Win32 optimizes GDI calls by collecting them in a batch
		// and then thunking the whole batch at once on a per
		// thread basis.  Since we share an HDC with multiple
		// threads, we must flush the batch before yielding to
		// other threads that will adjust the HDC.  To see what
		// I mean, comment out the GdiFlush() function call and
		// watch the results.
		GdiFlush();
	}
	LeaveCriticalSection(&CGDIThread::m_csGDILock);
}

/////////////////////////////////////////////////////////////////////////////
// CLineThread

IMPLEMENT_DYNAMIC(CLineThread, CGDIThread)

BEGIN_MESSAGE_MAP(CLineThread, CCmdTarget)
	//{{AFX_MSG_MAP(CLineThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CLineThread::CLineThread(CWnd* pWnd, HDC hDC, CPoint ptPos1, CPoint ptVel1,
	CPoint ptVel2, CSize Size, COLORREF color) : CGDIThread(pWnd, hDC)
{
	m_ptPosition1 = ptPos1;
	m_ptPosition2 = ptPos1 + Size;
	m_ptVelocity1 = ptVel1;
	m_ptVelocity2 = ptVel2;

	CPen pen(PS_SOLID, 1, color);
	m_hPen = (HPEN)pen.Detach();
}

BOOL CLineThread::InitInstance()
{
	m_pen.Attach(m_hPen);

	return CGDIThread::InitInstance();
}

void CLineThread::SingleStep()
{
	m_ptPosition1.Offset(m_ptVelocity1);
	m_ptPosition2.Offset(m_ptVelocity2);

	if (m_ptPosition1.y<m_rectBorder.top)
		m_ptVelocity1.y = (m_ptVelocity1.y>0) ? m_ptVelocity1.y : -m_ptVelocity1.y;
	else if (m_ptPosition1.y>m_rectBorder.bottom)
		m_ptVelocity1.y = (m_ptVelocity1.y>0) ? -m_ptVelocity1.y : m_ptVelocity1.y;

	if (m_ptPosition2.y<m_rectBorder.top)
		m_ptVelocity2.y = (m_ptVelocity2.y>0) ? m_ptVelocity2.y : -m_ptVelocity2.y;
	else if (m_ptPosition2.y>m_rectBorder.bottom)
		m_ptVelocity2.y = (m_ptVelocity2.y>0) ? -m_ptVelocity2.y : m_ptVelocity2.y;

	if (m_ptPosition1.x<m_rectBorder.left)
		m_ptVelocity1.x = (m_ptVelocity1.x>0) ? m_ptVelocity1.x : -m_ptVelocity1.x;
	else if (m_ptPosition1.x>m_rectBorder.right)
		m_ptVelocity1.x = (m_ptVelocity1.x>0) ? -m_ptVelocity1.x : m_ptVelocity1.x;

	if (m_ptPosition2.x<m_rectBorder.left)
		m_ptVelocity2.x = (m_ptVelocity2.x>0) ? m_ptVelocity2.x : -m_ptVelocity2.x;
	else if (m_ptPosition2.x>m_rectBorder.right)
		m_ptVelocity2.x = (m_ptVelocity2.x>0) ? -m_ptVelocity2.x : m_ptVelocity2.x;

	// Since all threads share the same HDC it is necessary
	// to block all other threads while we render in the HDC
	EnterCriticalSection(&CGDIThread::m_csGDILock);
	{
		CPen* oldpen;

		oldpen = m_dc.SelectObject(&m_pen);
		m_dc.MoveTo(m_ptPosition1);
		m_dc.LineTo(m_ptPosition2);
		m_dc.SelectObject(oldpen);
		// Win32 optimizes GDI calls by collecting them in a batch
		// and then thunking the whole batch at once on a per
		// thread basis.  Since we share an HDC with multiple
		// threads, we must flush the batch before yielding to
		// other threads that will adjust the HDC.  To see what
		// I mean, comment out the GdiFlush() function call and
		// watch the results.
		GdiFlush();
	}
	LeaveCriticalSection(&CGDIThread::m_csGDILock);
}

BOOL CGDIThread::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
    loglog2("Threads OnCmdMsg %x %x", nID, nCode);

	return CWinThread::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
