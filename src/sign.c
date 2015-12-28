
#if defined(SUPPORT_ELECSIGN)

#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


#define BOARD_MAX_SIZE			10240
#define BOARD_PORT				PORT_NUM_COM2
//////////////////////////////////////////////////////////////////////////////////////////////
/*以下定义各个命令码Command ID*/

/*握手请求*/
#define HAND_SHAKE_REQ			0xA0
/*握手响应*/
#define HAND_SHAKE_RESP		0xB0

/*前一笔签字上传状态结果通知*/
#define LAST_STATUS_REQ		0xA1
/*前一笔签字上传状态结果响应*/
#define LAST_STATUS_RESP		0xB1

/*签字输入请求*/
#define SIGN_REQ				0xA2
/*签字完成成功响应*/
#define SIGN_SUCC_RESP			0xB2
/*签字完成异常响应*/
#define SIGN_FAIL_RESP			0xC2

/*签字结束请求*/
#define SIGN_END_REQ			0xA3
/*签字结束响应*/
#define SIGN_END_RESP			0xB3

/*上传失败签字批量传输请求*/
#define BATCH_FAIL_SIGN_REQ	0xA4
/*上传失败签字批量传输响应*/
#define BATCH_FAIL_SIGN_RESP	0xB4

/*上传失败签字批量传输结束请求*/
#define BATCH_FAIL_SIGN_END_REQ	0xA5
/*上传失败签字批量传输结束响应*/
#define BATCH_FAIL_SIGN_END_RESP	0xB5

/*批量传输接收成功*/
#define BATCH_SIGN_SUCC	0xA8

/*批量传输接收失败*/
#define BATCH_SIGN_FAIL	0xA9


//////////////////////////////////////////////////////////////////////////////////////////////////////
//static int nTimeOut = 60;


int Board_Pack(char cCommand, char* pszData, int* pnDataLen)
{
	char szPack[BOARD_MAX_SIZE];
	szPack[0] = STX;
	/*长度暂定这个方式*/
	PubIntToC2((unsigned char *)&szPack[1], (unsigned int)*pnDataLen + 2);
	szPack[3] = cCommand;
	memcpy(&szPack[4], pszData, *pnDataLen);
	szPack[4+*pnDataLen] = ETX;
	PubCalcLRC(&szPack[1], *pnDataLen + 4, &szPack[5+*pnDataLen]);
	*pnDataLen+=6;
	memcpy(pszData, szPack, *pnDataLen);
	return APP_SUCC;
}


int Board_UnPack(char *psBuf, unsigned int *punLen)
{
	int nLen = 0;
	char cLRC = 0;
	char sTemp[BOARD_MAX_SIZE];
	
	if (NULL == psBuf || NULL == punLen)
	{
		return APP_FAIL;
	}
		
	if (psBuf[0] != STX)
	{
		return APP_FAIL;
	}
	if (*punLen < 6)
	{
		return APP_FAIL;
	}
	PubC2ToInt((unsigned int*)&nLen, (unsigned char*)psBuf + 1);
	//if (PubC2ToInt((unsigned int*)&nLen, (unsigned char*)psBuf + 1) != APP_SUCC)
	{
		//return APP_FAIL;
	}	
	if (*punLen != nLen +4)
	{
		return APP_FAIL;
	}
	if (PubCalcLRC(psBuf + 1, nLen + 2, &cLRC) != APP_SUCC)
	{	
		return APP_FAIL;
	}
//clrscr();printf("LRC[%02x]\nclac Lrc[%02x]",*(psBuf + *punLen -1) , cLRC);getkeycode(0);
	if (*(psBuf + *punLen -1) != cLRC)
	{
		return APP_FAIL;
	}
	memcpy(sTemp, psBuf + 3, nLen-1);
	memcpy(psBuf, sTemp, nLen-1);
	*punLen = nLen-1;
	return APP_SUCC;
}

#if 0
int Board_Txn(const char* pszDataReq, int nDataReqLen, char cCommandReq, char* pszDataResp, int* pnDataRespLen, char cCommandResp)
{
	char szData[BOARD_MAX_SIZE];
	memset(szData, 0, sizeof(szData));
	memcpy(szData, pszDataReq, nDataReqLen);
	Board_Pack(cCommandReq, szData, nDataReqLen);
	//发送
	//接收
	memset(szData, 0, sizeof(szData));

	Board_UnPack(szData, *pnDataRespLen);
	if((szData[0] != cCommandResp && cCommandReq != 0xA2) || (cCommandReq == 0xA2) && szData[0] != 0xB2 && szData[0] != 0xC2)
	{
		return APP_FAIL;
	}
	*pnDataRespLen--;
	memcpy(pszDataResp, &szData[1], *pnDataRespLen);
	return APP_SUCC;
}
#else
#define MAX_RECV 4096
int Board_Txn(char* pszData, int* pnDataLen, char cCommandReq, char cCommandResp)
{
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;
	int nRet = 0;
	int nLen = 0;
//	static char cFlag = 0;
	STCOMMPARAM stPubCommParam;


	memset(szData, 0, sizeof(szData));
	memcpy(szData, pszData, *pnDataLen);
	Board_Pack(cCommandReq, szData, pnDataLen);
#if 0
	if (!cFlag)
	{
		nRet = initaux(BOARD_PORT, BPS115200, DB8|STOP1|NP);
		if (nRet != 0)
		{
			PubMsgDlg("初始化串口", "初始化串口失败", 3, 10);
			return APP_FAIL;
		}
		cFlag = 1;
	}
#endif

	memset(&stPubCommParam, 0, sizeof(stPubCommParam));
	stPubCommParam.cCommType = COMMTYPE_PORT;
	stPubCommParam.ConnInfo.stPortParam.nAux = BOARD_PORT;
	strcpy(stPubCommParam.ConnInfo.stPortParam.szAttr, "115200");
	stPubCommParam.cPreDialFlag = 0;
	stPubCommParam.nTimeOut = 10;
	stPubCommParam.cIsSupportQuitRecv = 1; // 1接收的时候可退出

	nRet = PubCommInit(&stPubCommParam);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}
	SetControlCommInit();/**<要求需要初始化通讯模块*/
#if 0
	clrportbuf(BOARD_PORT, 0);
	clrportbuf(BOARD_PORT, 1);
	nRet = portwrite(BOARD_PORT, *pnDataLen, szData);
	if (nRet < 0)
	{
		return APP_FAIL;
	}
#endif

	PubCommConnect();

	if(PubCommWrite(szData, *pnDataLen) == APP_FAIL)
	{
		PubMsgDlg(NULL, "串口数据发送失败", 3, 60);
		PubCommHangUp();
		return APP_FAIL;
	}

	
	memset(szData, 0, sizeof(szData));
	while(1)
	{
		szData[0] = 0;
		nRet = PubCommRead((char *)szData, 1, &nLen);
		if (APP_QUIT == nRet)
		{
			PubCommHangUp();
			return APP_TIMEOUT;
		}
		if (nRet == APP_FAIL)
		{
			PubCommHangUp();
			return APP_QUIT;
		}
		if (nLen != 1)
		{
			break;
		}
		if (szData[0] == STX)
		{
			break;
		}
	}
	if (szData[0] == STX)
	{
		nRet = PubCommRead((char *)szData+1, 2, &nLen);
		if (APP_SUCC != nRet)
		{
			PubCommHangUp();
			return APP_QUIT;
		}
		if (nLen != 2)
		{
			PubMsgDlg("接收", "接收失败1", 3, 10);
			PubCommHangUp();
			return APP_FAIL;
		}
		nDataLen = 2;
		PubC2ToInt((unsigned int *)&nDataLen, (unsigned char *)szData + 1);
		{
			int nOff=0;
			int nTmpLen = 0;
			nTmpLen = nDataLen + 1;
			while(1)
			{
				if(nLen <= MAX_RECV)
				{
					nRet = PubCommRead((char *)szData+3+nOff,nTmpLen, &nLen);
					if (APP_SUCC != nRet)
					{
						PubCommHangUp();
						return APP_QUIT;
					}
					if (nLen != nTmpLen)
					{
						PubCommHangUp();
						return APP_FAIL;
					}
					break;
				}
				else
				{
					nRet = PubCommRead((char *)szData+3+nOff,MAX_RECV, &nLen);
					if (APP_SUCC != nRet)
					{
						PubCommHangUp();
						return APP_QUIT;
					}
					if (nLen != MAX_RECV)
					{
						PubCommHangUp();
						return APP_FAIL;
					}
					nLen -= MAX_RECV;
					nOff += MAX_RECV;
				}
			}
		}
/*
		nRet = portread(BOARD_PORT, nDataLen+1, (char *)szData+3, nTimeOut);
		if (nRet == QUIT)
		{
			return APP_QUIT;
		}
		if (nRet != nDataLen+1)
		{
			return APP_FAIL;
		}
*/
	}
	else
	{

		PubMsgDlg("接收", "接收失败3", 3, 10);
		PubCommHangUp();
		return APP_FAIL;
	}
	*pnDataLen = nDataLen+4;

{
//	char szTemp[BOARD_MAX_SIZE*2];
//	PubHexToAsc((unsigned char*)szData, (*pnDataLen)*2, 0, (unsigned char*)szTemp);
	}

	Board_UnPack(szData, (unsigned int *)pnDataLen);
{
//	char szTemp[BOARD_MAX_SIZE*2];
//	PubHexToAsc((unsigned char*)szData, (*pnDataLen)*2, 0, (unsigned char*)szTemp);
	}

	PubCommHangUp();
	if((cCommandReq == 0xA2) && szData[0] == 0xC2)
	{
		
		return APP_QUIT;
	}
	else if(szData[0] != cCommandResp)
	{
		return APP_FAIL;
	}
	(*pnDataLen)--;

	memcpy(pszData, &szData[1], *pnDataLen);
	return APP_SUCC;
}
#endif


/*握手*/
int Hand_Shake(void)
{
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;
	
	memset(szData, 0, sizeof(szData));
	ASSERT_FAIL(Board_Txn(szData, &nDataLen,HAND_SHAKE_REQ,  HAND_SHAKE_RESP));
	if(szData[0] == 1)/*响应状态     1：准备好；0：未准备好*/
		return APP_SUCC;
	return APP_FAIL;
}

/*前一笔签字上传状态结果*/
int Last_Status_Inform(const char* sNum, char *pcStatus)
{
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;

	memset(szData, 0, sizeof(szData));
//	PubAscToHex((unsigned char*)szTraceNo, 6, 0, (unsigned char*)szData);
	memcpy(szData,sNum,3);
	nDataLen += 3;
	szData[nDataLen] = *pcStatus;	
	nDataLen ++;
	ASSERT_FAIL(Board_Txn(szData, &nDataLen, LAST_STATUS_REQ,LAST_STATUS_RESP));
	*pcStatus = szData[0];
	return APP_SUCC;
}

/*签字输入*/
int Sign_Input(const char* szFeatureCode, char* pszNum, char* pszFileContent)
{
	int nRet;
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;

	memset(szData, 0, sizeof(szData));
	memcpy(szData, szFeatureCode, 8);
	nDataLen = 8;

	nRet = Board_Txn(szData, &nDataLen, SIGN_REQ,SIGN_SUCC_RESP);	
	if(nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	memcpy(pszNum, szData, 3);
	memcpy(pszFileContent, szData+3, nDataLen-3);
	return nDataLen-3;
}

/*签字结束*/
int Sign_End()
{
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;
	memset(szData, 0, sizeof(szData));
	Board_Txn(szData, &nDataLen, SIGN_END_REQ, SIGN_END_RESP);
	return APP_SUCC;
}

/*上传失败签字批量传输*/
int Batch_Fail_Sign(char *pcSigninfNum, char *psNum, char *psData)
{
	int nRet;
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;

	memset(szData, 0, sizeof(szData));

	nRet = Board_Txn(szData, &nDataLen, BATCH_FAIL_SIGN_REQ,BATCH_FAIL_SIGN_RESP);	
	if(nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	*pcSigninfNum = szData[0];
	memcpy(psNum, szData, 3);
	memcpy(psData, szData+4, nDataLen-4);
	return nDataLen-4;
}
/*上传失败签字批量传输结束*/
int Batch_Fail_Sign_End()
{
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;
	memset(szData, 0, sizeof(szData));
	Board_Txn(szData, &nDataLen, BATCH_FAIL_SIGN_END_REQ, BATCH_FAIL_SIGN_END_RESP);
	return APP_SUCC;
}

/*批量传输接收成功*/
int Batch_Sign_Succ(char *pcSigninfNum, char *psNum, char *psData)
{
	int nRet;
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;

	memset(szData, 0, sizeof(szData));

	nRet = Board_Txn(szData, &nDataLen, BATCH_SIGN_SUCC,BATCH_FAIL_SIGN_RESP);	
	if(nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	*pcSigninfNum = szData[0];
	memcpy(psNum, szData, 3);
	memcpy(psData, szData+4, nDataLen-4);
	return nDataLen-4;
}

/*批量传输接收失败*/
int Batch_Sign_Fail(char *pcSigninfNum, char *psNum, char *psData)
{
	int nRet;
	char szData[BOARD_MAX_SIZE];
	int nDataLen = 0;

	memset(szData, 0, sizeof(szData));

	nRet = Board_Txn(szData, &nDataLen, BATCH_SIGN_FAIL,BATCH_FAIL_SIGN_RESP);	
	if(nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	*pcSigninfNum = szData[0];
	memcpy(psNum, szData, 3);
	memcpy(psData, szData+4, nDataLen-4);
	return nDataLen-4;
}

int SaveSignToBmp(char *pszTrace, char *pszFileContent, int nFileContentLen)
{
	char szPath[64];
	int nRet, nHandle;
	
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s%s.bmp", FILE_PATH, pszTrace);
	

	if(NDK_FsExist(szPath))
		NDK_FsDel(szPath);

	nHandle = NDK_FsOpen(szPath, "w");

	if(nHandle < 0)
	{
		return APP_FAIL;
	}
	
	nRet = NDK_FsWrite(nHandle, pszFileContent, nFileContentLen);
	NDK_FsClose(nHandle);

	if(nRet < nFileContentLen)
	{	
		return APP_FAIL;
	}
	
	return APP_SUCC;
}
#define OPTIONAL_SIGN 1
int DoSign(const char* pszTrace, const char* pszDate, const char* pszRefNum)
{	
	int nRet, nHandle, nFileContentLen = 0;
	char szFeatureCode[8+1];
	char szPath[64];
	char szNum[3+1], szLastNum[3+1], szFileContent[BOARD_MAX_SIZE];
	char cStatus;
	STWATER stWater;
	int nRecNo=0;		/**<当前记录号*/
	
	nRet = Hand_Shake();
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(NULL, "没有正常的签字板,请手工签字", 3, 3);
		return APP_FAIL;
	}

#if OPTIONAL_SIGN
	memset(szLastNum, 0, sizeof(szLastNum));
	GetVarLastElecSignSendNum(szLastNum);
	
	if(YES == GetVarElecSignSendResult())
	{
		cStatus = 0x01;
	}
	else
	{
		cStatus = 0x00;
	}
	
	nRet = Last_Status_Inform(szLastNum,&cStatus);
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(NULL, "未获取到上一笔状态响应报文", 3, 3);
		cStatus = 0x04;
//			return APP_FAIL;
	}
#endif
	
	
	memset(szFeatureCode, 0, sizeof(szFeatureCode));
	GetFeatureCode(pszDate, pszRefNum, szFeatureCode);

	memset(szNum, 0, sizeof(szNum));
	memset(szFileContent, 0, sizeof(szFileContent));

	nRet = Sign_Input(szFeatureCode, szNum, szFileContent);
	if(nRet < 0)
	{
		Sign_End();
		PubMsgDlg(NULL, "签字异常, 请手工签字", 3, 3);
		return APP_FAIL;
	}
	sprintf(szPath, "%s%s.bmp", FILE_PATH, pszTrace);
	

	if(NDK_FsExist(szPath))
		NDK_FsDel(szPath);

	nHandle = NDK_FsOpen(szPath, "w");

	if(nHandle < 0)
	{
		Sign_End();
		return APP_FAIL;
	}

	nFileContentLen = nRet;
	nRet = NDK_FsWrite(nHandle, szFileContent, nFileContentLen);
	NDK_FsClose(nHandle);

	if(nRet < nFileContentLen)
	{	
		Sign_End();
		return APP_FAIL;
	}
	Sign_End();
/*	
	nFileContentLen = nRet;
	nRet = SaveSignToBmp(pszTrace, szFileContent,nFileContentLen);
	Sign_End();
	if(nRet != APP_SUCC)
	{
		return APP_FAIL;
	}
//*/
	memset(&stWater, 0, sizeof(STWATER));
	FindWaterWithInvno(pszTrace, &stWater);
//	memcpy(stWater.sNum,szNum,3);
//	TlvAdd(TAG_SIGN_NUM, 3, szNum, (char *)stWater.sAddition2, &stWater.nAdditionLen2);		

	UpdateWater(&stWater);

#if OPTIONAL_SIGN
	{
		char cSigninfNum = 0;
//		char szNum[3+1];
//		char sData[BOARD_MAX_SIZE];
		char cFailCount = 0;
		

		if(cStatus == 0x03)
		{
			char szTrace[6+1] = {0};
			
			cSigninfNum = 0;
			memset(szNum, 0, sizeof(szNum));	
			/*待修改*/
			memset(szFileContent, 0, sizeof(szFileContent));
		
			PubDisplayGen(2, "正在批量传输");
			PubDisplayGen(3, "失败电子签名单据");
			PubUpdateWindow();
			nRet = Batch_Fail_Sign(&cSigninfNum, szNum, szFileContent);
			
		
			while(1)
			{
				if(nRet >= 0)
				{
					/*处理sData值(上送失败的电子签名)*/
					memset(&stWater, 0, sizeof(STWATER));
					memset(&szTrace, 0, sizeof(szTrace));
					PubFindRec (FILE_WATER, NULL, szNum,1, (char *)&stWater, &nRecNo);
					PubHexToAsc((unsigned char *)stWater.sTrace, 6, 0, (unsigned char *)szTrace);
					nFileContentLen = nRet;

					/*此处nRet 结果也作为与签字板的接收成功或失败处理*/
					//nRet = SaveSignToBmp(pszTrace, szFileContent, nFileContentLen);
					sprintf(szPath, "%s%s.bmp", FILE_PATH, szTrace);
	

					if(NDK_FsExist(szPath))
						NDK_FsDel(szPath);

					nHandle = NDK_FsOpen(szPath, "w");

					if(nHandle < 0)
					{
						PubMsgDlg("签名", "文件打开失败", 3, 5);
						nRet = APP_FAIL;
					}
					else
					{
						nFileContentLen = nRet;
						nRet = NDK_FsWrite(nHandle, szFileContent, nFileContentLen);
						NDK_FsClose(nHandle);

						if(nRet < nFileContentLen)
						{	
							PubMsgDlg("签名", "写文件失败", 3, 5);
							nRet = APP_FAIL;
						}
						else
						{
							CompressElecFailSign(szTrace);
						}						
					}

				}

				if(nRet < 0)
				{
					/*重发一次，否则退出*/
					if(cFailCount > 1)
					{
						break;
					}
					cSigninfNum = 0;
					memset(szNum, 0, sizeof(szNum));			
					memset(szFileContent, 0, sizeof(szFileContent));
					cFailCount ++;
					
					nRet = Batch_Sign_Fail(&cSigninfNum, szNum, szFileContent);
				}
				else
				{
					
					
					cFailCount = 0;
					if(cSigninfNum <= 1)
					{
						break;
					}
					
					cSigninfNum = 0;
					memset(szNum, 0, sizeof(szNum));			
					memset(szFileContent, 0, sizeof(szFileContent));
					nRet = Batch_Sign_Succ(&cSigninfNum, szNum, szFileContent);
				}
			}

			if(cSigninfNum == 1)
			{
				nRet = Batch_Fail_Sign_End();
			}
			
		}
		else if(cStatus == 0x04)/*打单处理*/
		{
			memset(&stWater, 0, sizeof(STWATER));
			PubFindRec (FILE_WATER, NULL, szLastNum,1, (char *)&stWater, &nRecNo);
			PubDisplayGen(2, "正 在 重 打 印");
			PubDisplayGen(3, "上笔电子签名单据");
			PubUpdateWindow();
			PrintWater(&stWater, REPRINT);
		}
	}
#endif	

	if (GetVarExElecConfirm() == '1' || GetVarExElecConfirm() == 1)
	{
		nRet = PubConfirmDlg(NULL, "请确认签字是否正确", 1, 0);
		if (nRet != APP_SUCC)
		{
			NDK_FsDel(szPath);		/*取消签名则删除保存的电子签名*/
			return APP_FAIL;
		}
	}
	
	return APP_SUCC;
}

#endif

