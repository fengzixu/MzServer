#include<winsock2.h>
#include<mstcpip.h>
#include <vector>
#include <string>
using std::vector;
using std::string;
#define SOCKET_CONNECT WM_USER + 1//���������Ϊ����һ��ֵ ����������ϵͳ��Ϣʱ������
#define CMD_HEARTBEAT WM_USER + 2   //������
#define CMD_FILEMANAGE WM_USER + 3 //�ļ�����������
#define SOKCET_FILEMANAGE WM_USER + 4 //�ļ�����
#define CMD_FILEDRIVER WM_USER + 5 //��ȡ������Ϣ��ָ��
#define CMD_SUCCEED WM_USER + 6 //��Ӧ�ɹ�
#define CMD_FILEDIRECTORY WM_USER + 7//��ȡ����Ŀ¼��Ϣ������`
#define CMD_DIRFLODERERR  WM_USER + 8 //�ļ��в��ɶ�
#define CMD_SCREEN_REQUEST WM_USER + 888  //��Ļ��������
#define CMD_SCREENDLG_SHOW  WM_USER+ 889//��ʾ��Ļ����
#define CMD_GETFIRST_SCREEN WM_USER + 890 //������Ļ��һ֡
#define CMD_SCREEN_TO_SHOW WM_USER + 891 //��ʾ��õ���Ļ��Ϣ
#define CMD_CHANGE_SCREEN_BITS WM_USER + 892 //�ı�λͼ����λ��


// ��Ļģ�����
#define CMD_CTRLALTDEL   WM_USER + 893 // Ctrl + Alt + del
#define CMD_KEYDOWN      WM_USER + 894 //WM_KEYDOWN 
#define CMD_KEYUP        WM_USER + 895 //WM_KEYUP
#define CMD_MOUSEMOVE    WM_USER + 896//WM_MOUSEMOVE
#define CMD_LBUTTONDOWN  WM_USER + 897//WM_LBUTTONDOWN
#define CMD_LBUTTONUP    WM_USER + 898//WM_LBUTTONUP
#define CMD_LBUTTONDBLCLK WM_USER + 899//WM_LBUTTONDBLCLK
#define CMD_RBUTTONDOWN   WM_USER + 900//WM_RBUTTONDOWN   
#define CMD_RBUTTONUP     WM_USER + 901//WM_RBUTTONUP
#define CMD_RBUTTONDBLCLK WM_USER + 902//WM_RBUTTONDBLCLK
#define CMD_SCREEN_PRESS WM_USER + 903

// cmd  ���� 
#define CMD_CMD_SHELL_REQUEST WM_USER + 910
#define CMD_SHELLDLG_SHOW WM_USER + 911
#define CMD_PROCESS_SHOW WM_USER + 913
#define CMD_REFRESH WM_USER + 915
#define CMD_ENDTASK WM_USER + 916

struct ProcsInfo{
	string strFileName;
	DWORD dwProcessID;
	string strFilePath;
};

typedef struct tagSysInfo//��õ��Ե�ϵͳ��Ϣ
{
	char cComputer[64];        
	char cMemorySize[32];
	char cOs[64];
	char cVersion[32];
    char bVideo;
	int iVipID;
}SysInfo,*LPSysInfo;

typedef struct tagMsgHead//�������Ϣͷ
{
	DWORD dwCmd;
	DWORD dwSize;
	DWORD dwExtend1;
	DWORD dwExtend2;
}MsgHead,*LPMsgHead;



//Ӳ�̽ṹ��
typedef struct tagDriver
{
	char driver[4];     //�̷�����
	char display[64];   //����
	int  drivertype;    //������
}DriverInfo, *LPDriverInfo;
//�ļ���Ϣ�ṹ��
typedef struct tagFileInfo
{
    int  iType;         //1-Ŀ¼��2-�ļ���3-��ЧĿ¼
	char cFileName[64]; //�ļ���
	char cAttrib[32];   //�ļ�����
	char cTime[32];     //ʱ��
	char cSize[32];     //�ļ���С	
}FileInfo,*LPFileInfo;

BOOL RecvData(SOCKET s,char *data,int len);
BOOL RecvMsg(SOCKET s,char  *pBuf,LPMsgHead lpMsgHead);

BOOL SendData(SOCKET S,char*  data ,int len);
BOOL SendMsg(SOCKET s,char const *pBuf,LPMsgHead lpMsgHead);
BOOL GetSystemInfo( SysInfo& info);