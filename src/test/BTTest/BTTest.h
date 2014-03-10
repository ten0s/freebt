// BTTest.h : main header file for the BTTEST application
//

#if !defined(AFX_BTTEST_H__C5A1C186_9F7D_4F73_B650_7920C4CB4084__INCLUDED_)
#define AFX_BTTEST_H__C5A1C186_9F7D_4F73_B650_7920C4CB4084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBTTestApp:
// See BTTest.cpp for the implementation of this class
//

class CBTTestApp : public CWinApp
{
public:
	CBTTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBTTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBTTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BTTEST_H__C5A1C186_9F7D_4F73_B650_7920C4CB4084__INCLUDED_)
