// threads.h : declarations for each GDI thread class
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

#ifndef __THREADS_H__
#define __THREADS_H__

/////////////////////////////////////////////////////////////////////////////
// CGDIThread thread


#define	IBUFSIZE  256		/* must be a power of 2 */

extern unsigned short in_buf[IBUFSIZE];	/* input character buffer */
extern short in_next;		/* pos to retrieve next input character */
extern short in_last;		/* pos to place most recent input character */

void MainPut(int k);

#define in_init() in_next = in_last = 0   /* initialize the input buffer */

#define in_check() (in_next != in_last)   /* is the input buffer non-empty? */

#define in_get() in_buf[in_next++ & (IBUFSIZE - 1)]


class CGDIThread : public CWinThread
{
public:
	DECLARE_DYNAMIC(CGDIThread)
	CGDIThread(CWnd* pWnd, HDC hDC);

// Attributes
public:
	CRect m_rectBorder;
	HDC m_hDC;
	CDC m_dc;
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;
	static HANDLE m_hAnotherDead;
	static HANDLE m_hEventMain;
	static HANDLE m_hEventScrn;

	static CRITICAL_SECTION m_csGDILock;

// Operations
public:
	void KillThread();
	void UpdateBorder();
	virtual void SingleStep() = 0;
	void Paint(CDC* pdc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGDIThread)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGDIThread();
	virtual void Delete();

protected:
	virtual BOOL InitInstance();

	// Generated message map functions
	//{{AFX_MSG(CGDIThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:	// User declarations

     RECT clientarea;

     unsigned long cap_set;

     CString caption;

     bool in_paint;

     int y_row;
     int y_coord;
     int next_y_coord;
     int y_step;		/* y step in pixels */
     int x_step;		/* x step in pixels */

     void OnPaint();

public:

     void  Resize(void);
     void  MoveCursor();
     void  PaintRow(int myrow, const short * src, COLORREF clrb, COLORREF clrf,
						int xstt=0, int xlim=0);
     void  EEol(int myrow, int mycol);
     void  EEop();
     void  Scroll(int sdist);
     void  Update();
                                                  
     int Height;
     int Width;

     short * scrsrc[200];
     int textrow;
     int textcol;
     int noo_row;
     int noo_col;

     bool cursor_off;
};

/////////////////////////////////////////////////////////////////////////////
// CBallThread thread

class CBallThread : public CGDIThread
{
public:
	DECLARE_DYNAMIC(CBallThread)
	CBallThread(CWnd* pWnd, HDC hDC,
		CPoint ptPos, CPoint ptVel, CSize Size, COLORREF color);

// Attributes
public:
	CRect m_rectPosition;
	CPoint m_ptVelocity;
	HBRUSH m_hBrush;
	CBrush m_brush;

// Operations
public:
	virtual BOOL InitInstance();
	virtual void SingleStep();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBallThread)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBallThread() {};

	// Generated message map functions
	//{{AFX_MSG(CBallThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CRectThread thread

class CRectThread : public CGDIThread
{
public:
	DECLARE_DYNAMIC(CRectThread)
	CRectThread(CWnd* pWnd, HDC hDC,
		CPoint ptPos, CPoint ptVel, CSize Size, COLORREF color);

// Attributes
public:
	CRect m_rectPosition;
	CPoint m_ptVelocity;
	HBRUSH m_hBrush;
	CBrush m_brush;

// Operations
public:
	virtual BOOL InitInstance();
	virtual void SingleStep();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRectThread)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRectThread() {};

	// Generated message map functions
	//{{AFX_MSG(CRectThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLineThread thread

class CLineThread : public CGDIThread
{
public:
	DECLARE_DYNAMIC(CLineThread)
	CLineThread(CWnd* pWnd, HDC hDC,
		CPoint ptPos1, CPoint ptVel1, CPoint ptVel2, CSize Size, COLORREF color);

// Attributes
public:
	CPoint m_ptPosition1;
	CPoint m_ptPosition2;
	CPoint m_ptVelocity1;
	CPoint m_ptVelocity2;
	HPEN m_hPen;
	CPen m_pen;

// Operations
public:
	virtual BOOL InitInstance();
	virtual void SingleStep();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineThread)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLineThread() {};

	// Generated message map functions
	//{{AFX_MSG(CLineThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // __THREADS_H__
