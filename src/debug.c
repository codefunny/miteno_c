/**
* @file debug.c
* @brief ����ģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


/**
* ����ȫ�ֱ���
*/
static char gszDebugData[100];

/**
* @brief ���������ݱ��浽����ȫ�ֱ�����
* @param [in]  char *pszFile �ļ���
* @param [in]  char *pszFunc ������
* @param [in]  int nLine     ����
* @return ����
*/
void SetDebugData(const char *pszFile, const char *pszFunc, const int nLine)
{
	sprintf(gszDebugData, "��%s��[%s][%s][%d]>>>", APP_NAMEDESC, pszFile, pszFunc,nLine);
}

/**
* @brief ���Թ���
* @param in 
* @return ����
*/
int Debug(const char *pszFile, const char *pszFunc, const int nLine, const int nRes)
{
	STAPPCOMMPARAM stCommPara;

	GetAppCommParam(&stCommPara);
	if(stCommPara.cCommType == COMM_RS232)
	{
		return nRes;
	}
	if (nRes != APP_SUCC)
	{
		PubDebug("��%s��[%s][%s][%d]>>>%d", APP_NAMEDESC, pszFile, pszFunc,nLine,nRes);
	}
	return nRes;
}

/**
* @brief ������Ϣ��ʽ������
* @param in lpszFormat ��ʽ���� 
* @return	��
*/
void Trace(char* lpszFormat, ...)
{
	va_list args;
	int nBuf;
	char buf[2048];
	STAPPCOMMPARAM stCommPara;

	GetAppCommParam(&stCommPara);
	if(stCommPara.cCommType == COMM_RS232)
	{
		return;
	}
	
	va_start(args, lpszFormat);
	nBuf=vsprintf(buf, lpszFormat, args);
	PubDebug("%s%s",gszDebugData, buf);	
	va_end(args);
}


/**
* @brief ������Ϣ��ʽ������
* @param in lpszFormat ��ʽ���� 
* @return	��
*/
void TraceHex(char* pszHexBuf, int nLen, char* lpszFormat, ...)
{
	va_list args;
	int nBuf;
	char buf[2048];
	STAPPCOMMPARAM stCommPara;

	GetAppCommParam(&stCommPara);
	if(stCommPara.cCommType == COMM_RS232)
	{
		return;
	}
	
	va_start(args, lpszFormat);
	sprintf(buf, "%s", gszDebugData);
	nBuf=vsprintf(buf+strlen(buf), lpszFormat, args);
	PubDebugData(buf, pszHexBuf, nLen);
	va_end(args);
}

/**
* @brief �����Լ򵥵���ʾ��POS��,��ò�Ҫ��
* @param in lpszFormat ��ʽ���� 
* @return	��
*/
void Lindebug(char* lpszFormat, ...)
{
	va_list args;
	char buf[2048] = {0};
	va_start(args, lpszFormat);
	vsprintf(buf, lpszFormat, args);
	PubClearAll();
	NDK_ScrPrintf(buf);
	PubUpdateWindow();
	PubGetKeyCode(0);
	va_end(args);
}

typedef struct 
{	
	unsigned char * psData;
	int nDataLen;
} STDEBUGPRINTDATA;


static int _Debug_PrintData(STDEBUGPRINTDATA * pstDebugPrintData)
{
	int i;


	PubPrePrinter("Length=%d\r", pstDebugPrintData->nDataLen);
	for (i = 0; i < pstDebugPrintData->nDataLen; i++)
	{
		PubPrePrinter("%02x", *(pstDebugPrintData->psData+i));
	}
	PubPrePrinter("\r");
	return APP_SUCC;
}


int PrintDebugData(unsigned char *psData, int nLen)
{
	int nRet = 0;
	STDEBUGPRINTDATA stDebugPrintData;

	stDebugPrintData.psData = psData;
	stDebugPrintData.nDataLen = nLen;

	nRet = PubPrintCommit( _Debug_PrintData, (void *)&stDebugPrintData, 1);
	ASSERT(nRet);
	return nRet;
}



