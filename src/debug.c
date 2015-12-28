/**
* @file debug.c
* @brief 调试模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


/**
* 定义全局变量
*/
static char gszDebugData[100];

/**
* @brief 将调试数据保存到调试全局变量中
* @param [in]  char *pszFile 文件名
* @param [in]  char *pszFunc 函数名
* @param [in]  int nLine     行数
* @return 返回
*/
void SetDebugData(const char *pszFile, const char *pszFunc, const int nLine)
{
	sprintf(gszDebugData, "【%s】[%s][%s][%d]>>>", APP_NAMEDESC, pszFile, pszFunc,nLine);
}

/**
* @brief 调试工具
* @param in 
* @return 返回
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
		PubDebug("【%s】[%s][%s][%d]>>>%d", APP_NAMEDESC, pszFile, pszFunc,nLine,nRes);
	}
	return nRes;
}

/**
* @brief 调试信息格式化函数
* @param in lpszFormat 格式化串 
* @return	无
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
* @brief 调试信息格式化函数
* @param in lpszFormat 格式化串 
* @return	无
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
* @brief 将调试简单的显示在POS上,最好不要用
* @param in lpszFormat 格式化串 
* @return	无
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



