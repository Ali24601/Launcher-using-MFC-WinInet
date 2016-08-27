// MFCUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFCUpdater.h"
//#include "MFCUpdaterDlg.h"
#include "AutoUpdater.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCUpdaterApp

BEGIN_MESSAGE_MAP(CMFCUpdaterApp, CWinApp)
	//{{AFX_MSG_MAP(CMFCUpdaterApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCUpdaterApp construction

CMFCUpdaterApp::CMFCUpdaterApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMFCUpdaterApp object

CMFCUpdaterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMFCUpdaterApp initialization

BOOL CMFCUpdaterApp::InitInstance()
{
	// Standard initialization

#if _MSC_VER <= 1200 // MFC 6.0 or earlier
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	CAutoUpdater updater;
	if (CAutoUpdater::Success == updater.CheckForUpdate(/*"http://www.yjia88.com/upload/"*/))
	{
	}
	else
	{
		//open executable directly
		updater.OpenExec();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
