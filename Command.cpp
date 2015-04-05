#include"stdafx.h"
#include"Command.h"
 

BOOL RecvData(SOCKET s,char *data,int len)
{
        if(len<=0)
             return TRUE;                 
	char *pData=data;///��¼���ݴ洢��ַ
	int iLeftRecv=len;//����δ����ʣ�೤��
	int iHasRecv=0;//�Ѿ����յ����ݳ���
	int iRet=0;//ÿ�ν��յ����ݳ���
	if(len<=0)  return TRUE;

	while(1)
	{
		iRet=recv(s,pData,iLeftRecv,0);
		if(iRet==0||iRet==SOCKET_ERROR) return FALSE;

		iHasRecv+=iRet;
		iLeftRecv-=iRet;
		pData+=iRet;//�������ݴ洢��ַҲҪ�ı�
		if(iHasRecv>=len) break;//�ܷ��ͳ��ȴ��ڵ����ܳ��� ���˳�
	}
	return TRUE;
}

BOOL RecvMsg(SOCKET s,char *pBuf,LPMsgHead lpMsgHead)// ����ָ�� �Ƚ���ָ���ͷ�� ֮�������ϸ��Ϣ
{
	if(!RecvData(s,(char *)lpMsgHead,sizeof(MsgHead)))//�Ƚ�����Ϣ��ͷ�� 
	{
		return FALSE;
	}
	
	if(lpMsgHead->dwSize<=0)  return TRUE;

	if(!RecvData(s,pBuf,lpMsgHead->dwSize))//�ٽ�������
		return FALSE;
	return TRUE;
}

BOOL SendData(SOCKET s,char*  data ,int len)
{
	char *pData=data;///��¼���ݴ洢��ַ
	int iHasSend=0;//�Ѿ����͵����ݳ���
	int iLeftSend=len;//����δ����ʣ�೤��
	int iRet=0;//ÿ�η��͵����ݳ���

	if(len<=0)  return TRUE;

	while(1)
	{
		iRet=send(s,pData,iLeftSend,0);
		if(iRet==0||iRet==SOCKET_ERROR) return FALSE;

		iHasSend+=iRet;
		pData+=iRet;//�������ݴ洢��ַҲҪ�ı�
		iLeftSend-=iRet;

		if(iHasSend>=len) break;//�ܷ��ͳ��ȴ��ڵ����ܳ��� ���˳�
	}
	return TRUE;
}

BOOL SendMsg(SOCKET s,char const *pBuf,LPMsgHead lpMsgHead)// ����ָ�� �ȷ���ָ���ͷ�� ֮������ϸ��Ϣ
{
	if(!SendData(s,(char *)lpMsgHead,sizeof(MsgHead)))//�ȷ�����Ϣ��ͷ�� 
	{
		 
		return FALSE;
	} 
	if(lpMsgHead->dwSize<=0) { return TRUE;}

	if(!SendData(s,(char*)pBuf,lpMsgHead->dwSize))//�ٷ�������
	{
		 
		return FALSE;
	}
	return TRUE;
}

BOOL GetSystemInfo( SysInfo& info)//�õ�����ϵͳ����Ϣ
{
	memset(&info,0,sizeof(SysInfo));
	DWORD iSize=64;
	GetComputerName(info.cComputer,&iSize);//�õ���������

	char szSystem[32];
	OSVERSIONINFOEX osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx((OSVERSIONINFO*)&osvi ))//�õ���ϵͳ�汾��Ϣ�Ž�osvi  
	{
		return FALSE;
	}
	
	switch (osvi.dwPlatformId)//����ƽ̨
	{
      case VER_PLATFORM_WIN32_NT:
		    if (osvi.dwMajorVersion == 6)//ϵͳ�汾
				strcpy(szSystem, "Windows Vista");
		    if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			    strcpy(szSystem, "Windows 2003");
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			    strcpy(szSystem, "Windows XP");
            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			    strcpy(szSystem, "Windows 2000");
            if ( osvi.dwMajorVersion <= 4 )
			    strcpy(szSystem, "Windows NT");
         break;
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
                strcpy(szSystem, "Windows 95");

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
                strcpy(szSystem, "Windows 98");
         break;
    }

    wsprintf(info.cOs,"%s SP%d (Build %d)",szSystem,osvi.wServicePackMajor,osvi.dwBuildNumber);//����Ϣд�����Ƕ���Ľṹ��ĳ�Ա��
	////get memory size////////////////
	MEMORYSTATUS mem;
	mem.dwLength=sizeof(mem);
	GlobalMemoryStatus(&mem);
	wsprintf(info.cMemorySize,"%dMB",mem.dwTotalPhys/1024/1024+1);//��1  ��Ϊ��ȡ��  ��ֹС��1G ����ʾ0G
	///////server version//////////////////

	return TRUE;
}

