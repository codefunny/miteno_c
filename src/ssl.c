/**
* @file ssl.c
* @brief ssl模块
* @version  1.0
* @author 林礼达
* @date 2014-09-25
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


static int GetHttpField(const char *pszHttpBuff, char *pszHttpField, int *pnPos)
{
	int i, nLen;
	char szStr[1024] = {0};

	if(NULL == pszHttpBuff || NULL == pszHttpField || NULL == pnPos)
	{
		return APP_FAIL;
	}
	nLen = strlen(pszHttpBuff);
	for(i = *pnPos; i < nLen - 1; i++)
	{
		if(pszHttpBuff[i] == '\r' && pszHttpBuff[i + 1] == '\n')
		{
			strcpy(pszHttpField, szStr);
			*pnPos = i+2;
			return APP_SUCC;
		}
		szStr[i - (*pnPos)] = pszHttpBuff[i];
	}
	return APP_FAIL;
}

//格式如下:
//POST /unp/webtrans/WPOS HTTP /1.1
//HOST: 145.4.206.244:5000
//User-Agent: Donjin Http 0.1
//Cache-Control: no-cache
//Content-Type:x-ISO-TPDU/x-auth
//Accept: */*
//Content-Length: 93
//
//00 5B 60 00 03 00 00 60 31 00 31 13 12 08 00 
//00 20 00 00 00 c0 00 16 00 00 01 31 30 30 30 
//30 31 35 39 38 38 30 32 31 30 30 31 30 32 31 
//30 31 36 30 00 11 00 00 00 01 00 30 00 29 53 
//65 71 75 65 6e 63 65 20 4e 6f 31 36 33 31 35 
//30 53 58 58 2d 34 43 33 30 34 31 31 39 00 03 
//30 31 20


static int HttpAddHead(char *pszBuff, int *pnLen)
{
	char szBuff[MAX_PACK_SIZE + 1] = {0};
	STAPPCOMMPARAM stAppCommParam;
	char szHost[30] = {0};
	int nIndex = -1, nLen;

	if(NULL == pszBuff || NULL == pnLen)
	{
		return APP_FAIL;
	}
	memset(&stAppCommParam, 0, sizeof(STAPPCOMMPARAM));
	GetAppCommParam(&stAppCommParam);
	PubGetConnectIndex(&nIndex);
	if(0 == nIndex)
		sprintf(szHost, "%s:%s", stAppCommParam.szIp1, stAppCommParam.szPort1);
	else
		sprintf(szHost, "%s:%s", stAppCommParam.szIp2, stAppCommParam.szPort2);
	
	sprintf(szBuff, "%s\r\nHOST:%s\r\nUser-Agent:%s\r\nCache-Control:%s\r\nContent-Type:%s\r\nAccept:%s\r\nContent-Length:%d\r\n\r\n", 
		"POST /unp/webtrans/WPOS HTTP/1.1", szHost, "Donjin Http 0.1", "no-cache", "x-ISO-TPDU/x-auth", "*/*", *pnLen);
	nLen = strlen(szBuff);
	memcpy(szBuff + nLen, pszBuff, *pnLen);
	memcpy(pszBuff, szBuff, nLen + *pnLen);
	*pnLen= *pnLen + nLen;
	return APP_SUCC;
}


//格式如下:
//HTTP/1.1 200 OK
//ALLOW: POST, PUT
//Content-Type:x-ISO-TPDU/x-auth
//Date: Wed, 19 Feb 2014 15:18:03 GMT
//Content-Length: 123
//Server: Access-Guard-1000-Software/1.0
//Connect: close
//
//00 79 60 00 00 00 03 60 31 00 31 13 12 08 10 
//00 38 00 01 0a c0 00 14 00 00 01 14 32 58 03 
//19 08 86 02 10 01 30 30 30 30 30 30 30 30 30 
//30 30 31 30 30 31 30 30 30 30 31 35 39 38 38 
//30 32 31 30 30 31 30 32 31 30 31 36 30 00 11 
//00 00 00 01 00 30 00 40 7c e2 1b 8f 80 ce 69 
//37 a8 1d f0 31 ea e0 aa 2d a1 ba c1 b3 46 ca 
//a9 5b 65 f7 03 f2 00 00 00 00 00 00 00 00 d8 
//69 04 e5

static int HttpDelHead(char *pszBuff, int *pnLen)
{
	char szStr[500] = {0};
	char szLenStr[10] = {0};	
	int nPos = 0, nLen = 0;
	char szTmbBuf[MAX_RECV_SIZE + 1] = {0};

	if(NULL == pszBuff || NULL == pnLen)
	{
		return APP_FAIL;
	}

	memcpy(szTmbBuf, pszBuff, *pnLen);
	
	//http头各个字段暂时还没用到，这边获取只是为了偏移nPos到真正的8583报文，以便解析
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);//8583报文长度字段
	sscanf(szStr, "%*[^:]:%s", szLenStr);//解析Content-Length: 123中的123
	nLen = atoi(szLenStr);
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);
	GetHttpField(szTmbBuf, szStr, &nPos);//空行

	if (*pnLen - nPos < nLen) /**最后可能有换行*/
	{
		return APP_FAIL;
	}

	memcpy(pszBuff, szTmbBuf + nPos, nLen);
	pszBuff[nLen] = '\0';
	*pnLen = nLen;
	
	return APP_SUCC;
}

//尾部加\r\n\r\n
static int HttpAddEnd(char *pszBuff, int *pnLen)
{
	memcpy(pszBuff+*pnLen, "\r\n\r\n", 4);
	*pnLen += 4;
	return APP_SUCC;
}


void ExportSslCa(void)
{
	if(NDK_FsExist("../mainctrl/cacert.pem") == NDK_OK)
	{
		system("cp -f ../mainctrl/cacert.pem cacert.pem");
	}

}


int SslCommSend(char *psSendBuf, int nBufLen)
{
	char sTmpSendBuf[MAX_SEND_SIZE];	/**<发送缓冲区*/
	int nLen = nBufLen;
	
	memset(sTmpSendBuf, 0, sizeof(sTmpSendBuf));
	
	memcpy(sTmpSendBuf + COMM_TCPIP_HEAD_LEN, psSendBuf, nLen);
	
	PubIntToC2((uchar *)sTmpSendBuf, (uint)*&nLen);
	nLen += COMM_TCPIP_HEAD_LEN;
	
	HttpAddHead(sTmpSendBuf, &nLen);
	HttpAddEnd(sTmpSendBuf, &nLen);
	
	return PubCommWrite(sTmpSendBuf, nLen);

}

int SslCommRecv(char *psRecvBuf, int *pnBufLen)
{
	int nLenTrue = 0;
	int nRet = 0;
	int nLen = 0;
	char szTmpReciveBuf[MAX_RECV_SIZE + 1] = {0};
	
	nRet = PubCommRead(szTmpReciveBuf, MAX_PACK_SIZE, &nLen);
	if(nRet != APP_SUCC)
	{
		PubDispErr("通讯失败");
		return APP_FAIL;
	}

	if (APP_SUCC != HttpDelHead(szTmpReciveBuf, &nLen))
	{
		PubMsgDlg("通讯失败","接收失败(-1)", 3, 5);
		return APP_FAIL;
	}
	PubC2ToInt((uint *)&nLenTrue, (uchar *)szTmpReciveBuf);
	if (nLenTrue != nLen - 2)
	{
		PubMsgDlg("通讯失败","接收数据错误(-1)", 3, 5);
		return APP_FAIL;
	}
		
	memcpy(psRecvBuf, szTmpReciveBuf + 2, nLenTrue);
	*pnBufLen = nLenTrue;
	return APP_SUCC;

}

/**
* @fn IsSupportSslLib
* @brief 是否支持SSL库
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int IsSupportSslLib()
{
	char szVer[100] = {0};

	PubGetCommVerion(szVer);
	if(strcmp(szVer,"ALCOMM0114022801")>=0)
	{
		return APP_SUCC;
	}
	PubMsgDlg("通讯", "库版本过低, 不支持公网", 3, 10);
	PubDebug("COMMVER:%s Too Old to Support SSL\n",szVer);
	return APP_FAIL;
}

/**
* @brief 设置Ssl模块
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int DealSslMode(void)
{
	STSSLMODE stSslMode;

	memset(&stSslMode, 0, sizeof(STSSLMODE));
	stSslMode.nAuthOpt = SSL_AUTH_CLIENT;
	stSslMode.nType = HANDSHAKE_TLSv1;
	stSslMode.nFileFormat = SSL_FILE_PEM;

	strcpy(stSslMode.szServerCert, "cacert.pem");
	
	return PubSslSetMode(&stSslMode);

}

/**
* @brief 处理Ssl模块
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/

int SetSslMode(void)
{
	if(IsUseSslFunction() == APP_SUCC)
	{
		if (APP_SUCC != IsSupportSslLib())
		{
			return APP_FAIL;
		}
		
		return DealSslMode();
	}
	else
	{
		return APP_SUCC;
	}
}



