// Shannon.h : main header file for the SHANNON application
//

#if !defined(AFX_SHANNON_H__F09B1A79_35FE_4089_B220_82C76EAA1AE3__INCLUDED_)
#define AFX_SHANNON_H__F09B1A79_35FE_4089_B220_82C76EAA1AE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CShannonApp:
// See Shannon.cpp for the implementation of this class
//

class CShannonApp : public CWinApp
{
public:
	CShannonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShannonApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CShannonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHANNON_H__F09B1A79_35FE_4089_B220_82C76EAA1AE3__INCLUDED_)
