// AutoUpdater.h: interface for the CAutoUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_)
#define AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Wininet.h>

#define LOCATION_UPDATE_FILE_CHECK _T("version.txt")
#define EXECUTABLE_FILENAME _T("V-life.exe")
#define CONFIG_FILE _T("config.cic")
#define PRODUCT_NAME _T("赞云·云家")
#define PRODUCT_UNINST_KEY _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\赞云·云家")
#define ROOT_URL _T("http://www.yjia88.com/version/")
class CAutoUpdater  
{
public:
	CAutoUpdater();
	virtual ~CAutoUpdater();

	enum ErrorType { Success, InternetConnectFailure, InternetSessionFailure, 
						VersionFileDownloadFailure, FileDownloadFailure,ConfigDownloadFailure,NoExecutableVersion,
						UpdateNotRequired, UpdateNotComplete,FileOpenFailure,DatabaseDownloadFailure};

	ErrorType CheckForUpdate(/*LPCTSTR UpdateServerURL*/);	
	HINTERNET GetSession(CString &URL);

	bool InternetOkay();
	bool DownloadVersion(HINTERNET hSession, BYTE *pBuf, DWORD bufSize);
	bool DownloadFile(HINTERNET hSession, LPCTSTR localFile);

	CString GetFileVersion(LPCTSTR file);
	int		CompareVersions(CString ver1, CString ver2);
	bool	IsDigits(CString text);
	CString GetExecutable();
	bool	Switch(CString executable, CString update, bool WaitForReboot);
	UINT	OpenExec();
	bool DownloadFileOK(CString filename);//sll 2015.11.24 判断下载的文件是否有效，排除网页无效导致的404问题
	CString executable;
	CString UpdateServerURL;
	CString fileVersion;
	CString m_customer;//sll 2015.11.24 公司名

private:
	HINTERNET hInternet;
};

#endif // !defined(AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_)
