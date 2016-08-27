// MFCUpdater.h : main header file for the MFCUPDATER application
//

#if !defined(AFX_MFCUPDATER_H__DA2B0132_5EA3_4DFE_989C_20A4AFC83B30__INCLUDED_)
#define AFX_MFCUPDATER_H__DA2B0132_5EA3_4DFE_989C_20A4AFC83B30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCUpdaterApp:
// See MFCUpdater.cpp for the implementation of this class
//

class CMFCUpdaterApp : public CWinApp
{
public:
	CMFCUpdaterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCUpdaterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMFCUpdaterApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCUPDATER_H__DA2B0132_5EA3_4DFE_989C_20A4AFC83B30__INCLUDED_)
