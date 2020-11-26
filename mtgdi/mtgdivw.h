// mtgdivw.h : interface of the CThreadView class
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

#include "threads.h"

class CThreadView : public CView
{
protected: // create from serialization only
	CThreadView();
	DECLARE_DYNCREATE(CThreadView)

 virtual
	BOOL PreTranslateMessage(MSG* pMsg);

// Attributes
public:
	CClientDC *m_pDC;
	CTypedPtrList<CObList,CGDIThread*>     m_threadList;
	CThreadDoc* GetDocument();
	CGDIThread* m_pThread;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThreadView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnPrint(CDC* pDC);  // overridden to draw this view
	virtual void OnUpdate(CDC* pDC);  // overridden to draw this view
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
 static	void MyPaint(void);
	void StartThread(int ThreadID);
	void UpdateTitle(const char * title);
	virtual ~CThreadView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

// Generated message map functions
protected:
	//{{AFX_MSG(CThreadView)
	afx_msg void OnNewball();
	afx_msg void OnKillThreads();
	afx_msg void OnDestroy();
	afx_msg void OnNewline();
	afx_msg void OnNewrectangle();
	afx_msg void On10newlines();
	afx_msg void On10newrectangles();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void On25newballs();
	afx_msg void On25newlines();
	afx_msg void On25newrectangles();
	afx_msg void OnThreadKillThreadsSlow();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int kb_mode;

};

extern CThreadView * threadv;


#ifndef _DEBUG  // debug version in mtgdivw.cpp
inline CThreadDoc* CThreadView::GetDocument()
   { return (CThreadDoc*)m_pDocument; }
#endif

int callmain(LPSTR param_);

/////////////////////////////////////////////////////////////////////////////
