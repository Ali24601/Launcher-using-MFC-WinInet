// AutoUpdater.cpp: implementation of the CAutoUpdater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpdater.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TRANSFER_SIZE 4096

CAutoUpdater::CAutoUpdater()
{
	// Initialize WinInet
	hInternet = InternetOpen("AutoUpdateAgent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	

	m_customer="ZY";//Ĭ�����ƹ�˾��

	//����ע����ȡ
	// Get executable path
	//CString CurPath = GetExecutable();//��ȡ��·���Ŀ�ִ���ļ���
	//executable = CurPath.Left(1+CurPath.ReverseFind(_T('\\')))+CString(EXECUTABLE_FILENAME);//��ȡ��ִ���ļ���

	HKEY hKEY;//�����йص�hKEY,�ڲ�ѯ����ʱҪ�ر�
	//����·��data_Set��ص�hKEY
	LPCTSTR data_Set= (LPCTSTR)PRODUCT_UNINST_KEY;
	//����ע���hKEY�򱣴�˺������򿪵ļ��ľ��
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY))
	{
		int er;
		//��ȡ�Ӱ汾��Ϣ
		char dwValue[256];
		memset(dwValue,0,256*sizeof(char));
		DWORD dwSzType = REG_SZ;
		DWORD dwSize = sizeof(dwValue);
		if (::RegQueryValueEx(hKEY,_T("SubVersion"), 0, &dwSzType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox(_T("�����޷���ѯ���Ӱ汾��Ϣ"));
		}
		else
		{
			UpdateServerURL=ROOT_URL+(CString)dwValue+"/";
		}
		//����������رմ򿪵�hKEY
		//::RegCloseKey(hKEY);

		//��ȡ��ִ���ļ�·��
		memset(dwValue,0,256*sizeof(char));
		dwSzType = REG_SZ;
		dwSize = sizeof(dwValue);
		if ((er=::RegQueryValueEx(hKEY,_T("DisplayIcon"), 0, &dwSzType, (LPBYTE)&dwValue, &dwSize)) != ERROR_SUCCESS)
		{
			AfxMessageBox(_T("�����޷���ѯ����װ·��"));
		}
		else
		{
			executable=(CString)dwValue;
		}
		//����������رմ򿪵�hKEY
		//::RegCloseKey(hKEY);

		//��ȡ�汾��
		memset(dwValue,0,256*sizeof(char));
		dwSzType = REG_SZ;
		dwSize = sizeof(dwValue);
		if (::RegQueryValueEx(hKEY,_T("DisplayVersion"), 0, &dwSzType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox(_T("�����޷���ѯ���Ѱ�װ�汾"));
		}
		else
		{
			fileVersion=(CString)dwValue;
		}
		//����������رմ򿪵�hKEY
		::RegCloseKey(hKEY);

		//sll 2015.11.24 Get original company name
		CString originalconfigfile=executable.Left(1+executable.ReverseFind(_T('\\')))+"images\\Customized\\"+CONFIG_FILE;
		char * configfile=originalconfigfile.GetBuffer(0);

		//{
		//	WIN32_FIND_DATA FindFileData;
		//	HANDLE hFind;

		//	printf ("Target file is %s. ", configfile);

		//	hFind = FindFirstFile(configfile, &FindFileData);

		//	if (hFind == INVALID_HANDLE_VALUE) {
		//		printf ("Invalid File Handle. Get Last Error reports %d ", GetLastError ());
		//	} 
		//	else 
		//	{
		//		printf ("The first file found is %s ", FindFileData.cFileName);
		//		FindClose(hFind);
		//	}
		//}
		//sll 2015.10.27 �������ļ���ȡ��Ϣ
		FILE *pFile=fopen(configfile,"r") ;//ֻ��
		if (pFile) //�򿪳ɹ�
		{
			//��ȡ�ļ�����
			fseek(pFile, 0L, SEEK_SET);
			long beginPos = ftell(pFile);
			fseek(pFile, 0L, SEEK_END);
			long endPos = ftell(pFile);
			if(beginPos == endPos)//�ļ�Ϊ��
			{
				fclose(pFile);
			}
			else
			{
				fseek(pFile, 0L, SEEK_SET);
				char buf[100];// �����ȡ�Ľڵ㣨��һ�У�		
				while(fgets(buf,100,pFile))
				{
					int length=(int)strlen(buf);
					if ('\n'==buf[length-1])
					{
						buf[length-1]='\0';//������tempbuff�е���Ϣת��Ϊ�ַ���
					}
					CString templine(buf);
					templine.Remove(_T(' '));//������пո�
					if (templine.Left(8)=="Customer")//��ȡURL
					{
						m_customer=templine.Right(templine.GetLength()-8);
						m_customer.MakeUpper();
					}
					memset(buf,0,100);//���buf����
				}
				fclose(pFile);
			}
		}
	}
}

CAutoUpdater::~CAutoUpdater()
{
	if (hInternet) {
		InternetCloseHandle(hInternet);
	}
}

// Check if an update is required
//
CAutoUpdater::ErrorType CAutoUpdater::CheckForUpdate(/*LPCTSTR UpdateServerURL*/)
{	
	if (!InternetOkay())//��������ʧ��
	{
		return InternetConnectFailure;
	}

	TCHAR path[MAX_PATH];
	GetTempPath(MAX_PATH, path);//��ȡTEMP·��
	CString exeName=(CString)PRODUCT_NAME+_T(".exe");
	CString directory = path;//��ȡTEMP·��
	CString URL;
	HINTERNET hSession;
	bool bTransferSuccess = false;

	// First we must check the remote configuration file to see if an update is necessary
	URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	hSession = GetSession(URL);
	if (!hSession)
	{
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bTransferSuccess = DownloadVersion(hSession, pBuf, TRANSFER_SIZE);//�ӷ�����version.txt��ȡ�汾��Ϣ
	InternetCloseHandle(hSession);
	if (!bTransferSuccess)
	{
		return VersionFileDownloadFailure;
	}

	//����ע����ȡ�汾��
	/*CString fileVersion = GetFileVersion(executable);\*/
	if (fileVersion.IsEmpty())
	{
		return NoExecutableVersion;
	}

	CString updateVersion = (char *) pBuf;
	if (CompareVersions(updateVersion, fileVersion) != 1)//�Ƚϰ汾��
	{	
		return UpdateNotRequired;
	}


	
	// Download the updated file
	//URL = UpdateServerURL + exeName;//���������ļ���ַ
	URL = UpdateServerURL + "Update.exe";
	hSession = GetSession(URL);
	if (!hSession)
	{
		return InternetSessionFailure;
	}

	CString msg;
	msg.Format(_T("An update of %s is now available. Proceed with the update?"), exeName);
	if (IDNO == MessageBox(GetActiveWindow(), msg, _T("Update is available"), MB_YESNO|MB_ICONQUESTION))
	{
		return UpdateNotComplete;	
	}

	// Proceed with the update
	//CString updateFileLocation = directory+exeName;
	CString updateFileLocation = directory+"Update.exe";
	bTransferSuccess = DownloadFile(hSession, updateFileLocation);
	InternetCloseHandle(hSession);
	if(bTransferSuccess)
	{
		bTransferSuccess=DownloadFileOK(updateFileLocation);
	}
	if (!bTransferSuccess)
	{
		return FileDownloadFailure;
	}	

	//sll 2015.11.24 ʹ���������ļ���ַ
	//download config file and move it
	CString configfile;
	if ("ZY"==m_customer||"INNER"==m_customer)
	{
		configfile=UpdateServerURL+CONFIG_FILE;
	}
	else
	{
		configfile=UpdateServerURL+m_customer+'/'+CONFIG_FILE;
	}
	hSession = GetSession(configfile);
	if (!hSession)
	{
		return InternetSessionFailure;
	}
	CString updateconfigLocation = directory+CONFIG_FILE;
	bTransferSuccess=DownloadFile(hSession, updateconfigLocation);
	InternetCloseHandle(hSession);
	if(bTransferSuccess)
	{
		bTransferSuccess=DownloadFileOK(updateconfigLocation);
	}
	if (!bTransferSuccess)
	{
		return ConfigDownloadFailure;
	}
	//move it
	CString newconfigfile=executable.Left(1+executable.ReverseFind(_T('\\')))+"images\\Customized\\"+CONFIG_FILE;
	int er=MoveFileEx(updateconfigLocation,newconfigfile,MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED);

	er=WinExec(updateFileLocation, SW_SHOWNORMAL);//��������ĸ����ļ�
	if (er<32)
	{
		//��exeʧ��
		return FileOpenFailure;
	}	
	//if (!Switch(executable, updateFileLocation, false))
	//{
	//	return UpdateNotComplete;
	//}
	
	return Success;
}

// Ensure the internet is ok to use
//
bool CAutoUpdater::InternetOkay()
{
	if (hInternet == NULL) {
		return false;
	}

	// Important step - ensure we have an internet connection. We don't want to force a dial-up.
	DWORD dwType;
	if (!InternetGetConnectedState(&dwType, 0))
	{
		return false;
	}

	return true;
}

// Get a session pointer to the remote file
//
HINTERNET CAutoUpdater::GetSession(CString &URL)
{
	// Canonicalization of the URL converts unsafe characters into escape character equivalents
	TCHAR canonicalURL[1024];
	DWORD nSize = 1024;
	InternetCanonicalizeUrl(URL, canonicalURL, &nSize, ICU_BROWSER_MODE);		
	
	DWORD options = INTERNET_FLAG_NEED_FILE|INTERNET_FLAG_HYPERLINK|INTERNET_FLAG_RESYNCHRONIZE|INTERNET_FLAG_RELOAD;
	HINTERNET hSession = InternetOpenUrl(hInternet, canonicalURL, NULL, NULL, options, 0);
	URL = canonicalURL;

	return hSession;
}

// Download a file into a memory buffer
//
bool CAutoUpdater::DownloadVersion(HINTERNET hSession, BYTE *pBuf, DWORD bufSize)
{	
	DWORD	dwReadSizeOut;
	InternetReadFile(hSession, pBuf, bufSize, &dwReadSizeOut);
	if (dwReadSizeOut <= 0)
	{
		return false;
	}

	
	return true;
}

// Download a file to a specified location
//
bool CAutoUpdater::DownloadFile(HINTERNET hSession, LPCTSTR localFile)
{	
	HANDLE	hFile;
	BYTE	pBuf[TRANSFER_SIZE];
	DWORD	dwReadSizeOut, dwTotalReadSize = 0;

	hFile = CreateFile(localFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	do {
		DWORD dwWriteSize, dwNumWritten;
		BOOL bRead = InternetReadFile(hSession, pBuf, TRANSFER_SIZE, &dwReadSizeOut);
		dwWriteSize = dwReadSizeOut;

		if (bRead && dwReadSizeOut > 0) {
			dwTotalReadSize += dwReadSizeOut;
			WriteFile(hFile, pBuf, dwWriteSize, &dwNumWritten, NULL); 
			// File write error
			if (dwWriteSize != dwNumWritten) {
				CloseHandle(hFile);					
				return false;
			}
		}
		else {
			if (!bRead)
			{
				// Error
				CloseHandle(hFile);	
				return false;
			}			
			break;
		}
	} while(1);

	CloseHandle(hFile);
	return true;
}

// Get the version of a file
//
CString CAutoUpdater::GetFileVersion(LPCTSTR file)
{
	CString version;
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD	dwTemp, dwSize, dwHandle = 0;
	BYTE	*pData = NULL;
	UINT	uLen;

	try {
		dwSize = GetFileVersionInfoSize((LPTSTR) file, &dwTemp);
		if (dwSize == 0) throw 1;

		pData = new BYTE[dwSize];
		if (pData == NULL) throw 1;

		if (!GetFileVersionInfo((LPTSTR) file, dwHandle, dwSize, pData))
			throw 1;

		if (!VerQueryValue(pData, _T("\\"), (void **) &pVerInfo, &uLen)) 
			throw 1;

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;

		int ver[4];
		ver[0] = HIWORD(verMS);
		ver[1] = LOWORD(verMS);
		ver[2] = HIWORD(verLS);
		ver[3] = LOWORD(verLS);

		// Are lo-words used?
		if (ver[2] != 0 || ver[3] != 0)
		{
			version.Format(_T("%d.%d.%d.%d"), ver[0], ver[1], ver[2], ver[3]);
		}
		else if (ver[0] != 0 || ver[1] != 0)
		{
			version.Format(_T("%d.%d"), ver[0], ver[1]);
		}

		delete pData;
		return version;
	}
	catch(...) {
		return _T("");
	}	
}

// Compare two versions 
//
int CAutoUpdater::CompareVersions(CString ver1, CString ver2)
{
	int  wVer1[4], wVer2[4];
	int	 i;
	TCHAR *pVer1 = ver1.GetBuffer(256);
	TCHAR *pVer2 = ver2.GetBuffer(256);

	for (i=0; i<4; i++)
	{
		wVer1[i] = 0;
		wVer2[i] = 0;
	}

	// Get version 1 to DWORDs
	TCHAR *pToken = strtok(pVer1, _T("."));
	if (pToken == NULL)
	{
		return -21;
	}

	i=3;
	while(pToken != NULL)
	{
		if (i<0 || !IsDigits(pToken)) 
		{			
			return -21;	// Error in structure, too many parameters
		}		
		wVer1[i] = atoi(pToken);
		pToken = strtok(NULL, _T("."));
		i--;
	}
	ver1.ReleaseBuffer();

	// Get version 2 to DWORDs
	pToken = strtok(pVer2, _T("."));
	if (pToken == NULL)
	{
		return -22;
	}

	i=3;
	while(pToken != NULL)
	{
		if (i<0 || !IsDigits(pToken)) 
		{
			return -22;	// Error in structure, too many parameters
		}		
		wVer2[i] = atoi(pToken);
		pToken = strtok(NULL, _T("."));
		i--;
	}
	ver2.ReleaseBuffer();

	// Compare the versions
	for (i=3; i>=0; i--)
	{
		if (wVer1[i] > wVer2[i])
		{
			return 1;		// ver1 > ver 2
		}
		else if (wVer1[i] < wVer2[i])
		{
			return -1;
		}
	}

	return 0;	// ver 1 == ver 2
}

// Ensure a string contains only digit characters
//
bool CAutoUpdater::IsDigits(CString text)
{
	for (int i=0; i<text.GetLength(); i++)
	{
		TCHAR c = text.GetAt(i);
		if (c >= _T('0') && c <= _T('9'))
		{
		}
		else
		{
			return false;
		}
	}

	return true;
}

CString CAutoUpdater::GetExecutable()
{
	HMODULE hModule = ::GetModuleHandle(NULL);
    ASSERT(hModule != 0);
    
    TCHAR path[MAX_PATH];
    VERIFY(::GetModuleFileName(hModule, path, MAX_PATH));
    return path;
}

bool CAutoUpdater::Switch(CString executable, CString update, bool WaitForReboot)
{
	int type = (WaitForReboot) ? MOVEFILE_DELAY_UNTIL_REBOOT : MOVEFILE_COPY_ALLOWED;

	const TCHAR *backup = _T("OldExecutable.bak");
	CString directory = executable.Left(executable.ReverseFind(_T('\\')));	
	CString backupFile = directory + _T('\\') + CString(backup);

	DeleteFile(backupFile);
	if (!MoveFileEx(executable, backupFile, type)) //�Ȱ�ԭ�е��ļ�����ΪOldExecutable.bak
	{
		return false;
	}


	bool bMoveOK = (MoveFileEx(update, executable, type) == TRUE);//�ٰѸ����ص���ʱ�ļ����͵�ָ��Ŀ¼������滻�ļ��Ĺ���
	int i = GetLastError();

	return bMoveOK;	
}

UINT CAutoUpdater::OpenExec()
{
	return(WinExec(executable, SW_SHOWNORMAL));
}

bool CAutoUpdater::DownloadFileOK(CString filename)
{
	char * configfile=filename.GetBuffer(0);
	//sll 2015.10.27 �������ļ���ȡ��Ϣ
	FILE *pFile=fopen(configfile,"r") ;//ֻ��
	if (pFile) //�򿪳ɹ�
	{
		//��ȡ�ļ�����
		fseek(pFile, 0L, SEEK_SET);
		long beginPos = ftell(pFile);
		fseek(pFile, 0L, SEEK_END);
		long endPos = ftell(pFile);
		if(beginPos == endPos)//�ļ�Ϊ��
		{
			fclose(pFile);
			return false;
		}
		else if(endPos-beginPos>2000)//�ļ�̫�󲻼��
		{
			fclose(pFile);
			return true;
		}
		else
		{
			fseek(pFile, 0L, SEEK_SET);
			char buf[100];// �����ȡ�Ľڵ㣨��һ�У�		
			fgets(buf,100,pFile);
			int length=(int)strlen(buf);
			if ('\n'==buf[length-1])
			{
				buf[length-1]='\0';//������tempbuff�е���Ϣת��Ϊ�ַ���
			}
			CString templine(buf);
			int pos=templine.Find("Error report");
			if(pos>=0)
			{
				fclose(pFile);
				return false;
			}
			memset(buf,0,100);//���buf����
			fclose(pFile);
			return true;
		}
	}
	return false;
}