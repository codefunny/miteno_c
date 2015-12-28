/**
* @file ickeymana.c
* @brief IC卡导密钥---广东银联使用
* @version  1.0
* @author 广东银联
* @date 2014-10-17
*/

#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


/*广东银联IC卡解主密钥*/

static int VerifyTerminalPin(char *szPin)
{
	uchar verify_apdu[256];
	uchar pin_bcd[10] ;
	uchar sRecebuf[256];
	int nRecvlen;
	int nLen,nRet;

	nLen = strlen(szPin);
	memset(pin_bcd, '\xff', sizeof(pin_bcd));
	PubAscToHex((uchar *)szPin, nLen, 0, pin_bcd) ;
	memset(verify_apdu,0,sizeof(verify_apdu));
	memcpy(verify_apdu, "\x00\x20\x00\x81\x00\x00", 6);
	verify_apdu[4] = '\x08' ;
	memcpy(verify_apdu+5,pin_bcd,8);

	nRet = NDK_Iccrw(ICTYPE_IC, 13, verify_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int SelectTerminalBinary(void)
{
	uchar select_apdu[256];
	uchar szRealData[3+1];
	uchar sRecebuf[256];
	int nRecvlen;
	int nRet;

	memset(szRealData, 0, sizeof(szRealData));
	memcpy(szRealData, "\xef\x09", 2);
	memset(select_apdu,0,sizeof(select_apdu));
	memcpy(select_apdu, "\x00\xa4\x00\x0C\x00\x00", 6) ;	
	select_apdu[4] = '\x02' ;		
	memcpy(select_apdu+5,szRealData,2);	
       
	nRet = NDK_Iccrw(ICTYPE_IC, 7, select_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int ReadTerminalBinary(uchar *szMainkeyNum)
{
	uchar read_apdu[256];
	uchar sRecebuf[256];
	int nRecvlen;
	int nRet;

	memset(read_apdu,0,sizeof(read_apdu));
	memcpy (read_apdu, "\x00\xb0\x00\x00\x00\x00", 6) ;	
	read_apdu[4] =0x03;	
	
	nRet = NDK_Iccrw(ICTYPE_IC, 5, read_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		memcpy(szMainkeyNum,sRecebuf,nRecvlen-2);
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int ReadTerminalMainkey(int index, uchar *szBuffer)
{
	uchar read_apdu[256] ;
	uchar sRecebuf[256];
	int nRecvlen;
	int nLen,nRet;
	
	nLen=3+index*24;
	memset(read_apdu,0,sizeof(read_apdu));
	memcpy (read_apdu, "\x00\xb0\x00\x00\x00\x00", 6) ;	
	read_apdu[2] = nLen/256;			
	read_apdu[3] = nLen%256;	
	read_apdu[4] = 0x18 ;	
	nLen=24;
	
	nRet = NDK_Iccrw(ICTYPE_IC, 5, read_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		memcpy(szBuffer,sRecebuf,nRecvlen-2);
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int SelectTerminalPerson(void)
{
	uchar select_apdu[256] ;
	uchar szRealData[3+1];
	uchar sRecebuf[256];
	int nRecvlen;
	int nRet;
	
	memset(szRealData, 0, sizeof(szRealData));
	memcpy(szRealData, "\xef\x08", 2);
	
	memcpy (select_apdu, "\x00\xa4\x00\x0c\x00\x00", 6) ;	
	select_apdu[4] = '\x02' ;
	memcpy(select_apdu+5,szRealData,2);	

	nRet = NDK_Iccrw(ICTYPE_IC, 7, select_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int ReadTerminalExpire(uchar *szExpire)
{
	uchar read_apdu[256];
	uchar sRecebuf[256];
	int nRecvlen;
	int nRet;

	memset(read_apdu,0,sizeof(read_apdu));
	memcpy(read_apdu, "\x00\xb0\x00\x00\x00\x00", 6) ;	
	read_apdu[4] = 0x08 ;
	
	nRet = NDK_Iccrw(ICTYPE_IC, 5, read_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		memcpy(szExpire,sRecebuf,nRecvlen-2);
		return APP_SUCC;
	}
	return APP_FAIL;
}

static int ReadTerminalDecypt(uchar *szBuffer, uchar *szOut)
{
	uchar decypt_apdu[256];
	uchar sRecebuf[256];
	int nRecvlen;	
	int nRet;

	memset(decypt_apdu,0,sizeof(decypt_apdu));
	memcpy(decypt_apdu, "\x80\xf8\x01\x02\x00\x00", 6) ;	
	decypt_apdu[4] = 0x10;
	memcpy(decypt_apdu+5,szBuffer,16);
	decypt_apdu[21] = 0x10;

	nRet = NDK_Iccrw(ICTYPE_IC, 22, decypt_apdu, &nRecvlen, sRecebuf);
	if(nRet != NDK_OK)
	{
		return APP_FAIL;
	}
	if(sRecebuf[nRecvlen-2] == 0x90&&sRecebuf[nRecvlen-1] == 0x00)
	{
		memcpy(szOut,sRecebuf,nRecvlen-2);
		return APP_SUCC;
	}
	return APP_FAIL;
}


int MainKeyDeciphering(uchar *szEnmainkey, uchar *szOutBuf)
{
	int nICFlag;
	int nKeyCode;
	int nAtrLen;
	uchar szAtrBuf[64];
	char szICPin[12+1];
	int nRet,nLen;
	char szExpire[20];
	char * pszTitle="密钥管理";

	/*********************维护传输卡处理********************/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayStr(1, 3, 1, "请插入维护人员卡");
	PubUpdateWindow();
	while(1)
	{
		nRet = NDK_IccDetect(&nICFlag);
		if(nRet != APP_SUCC)
		{
			PubMsgDlg(pszTitle, "获取卡片失败", 3, 10);
			return APP_FAIL;
		}
		if(nICFlag&0x00000001)
		{
			nRet = NDK_IccPowerUp(ICTYPE_IC, szAtrBuf, &nAtrLen);
			if(nRet != APP_SUCC)
			{
				PubMsgDlg(pszTitle, "卡片上电失败", 3, 10);
				return APP_FAIL;
			}
			break;
		}
		nRet = NDK_KbGetCode(1, &nKeyCode);
		if(nRet != APP_SUCC ||nKeyCode == KEY_ESC)
		{
			NDK_IccPowerDown(ICTYPE_IC);
			return APP_FAIL;
		}

	}
	memset(szICPin, 0, sizeof(szICPin));
	nRet = PubInputDlg("密钥管理", "请输入IC卡密码:", szICPin,&nLen, 
		6, 6, 60, INPUT_MODE_PASSWD);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	nRet = VerifyTerminalPin(szICPin);
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "IC卡密码验证失败", 3, 10);
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	nRet = SelectTerminalPerson();
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	memset(szExpire, 0, sizeof(szExpire));
	nRet=ReadTerminalExpire((uchar *)szExpire);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	if(strlen(szExpire) == 0)
	{
		PubMsgDlg(pszTitle, "IC卡上电失败", 3, 10);
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	memset(szOutBuf, 0, sizeof(szOutBuf));
	nRet = ReadTerminalDecypt(szEnmainkey,szOutBuf);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	NDK_IccPowerDown(ICTYPE_IC);
	return APP_SUCC;
}

static int SetMainKeyByHandIC(void)
{
	int nKeyIndex;
	int nRet;
	int nLen;
	char szKeyIndex[2+1];
	char szTmpStr[21];
	char szAscMainKey[32+1], szBcdMainKey[16+1];
	char szCheckValue[8+1];
	char szResult[64];
	char szSrc[8+1];
	char szDes[8+1];
	char *pszTitle = "密钥管理";
	YESORNO cIsPinPad = NO;

	cIsPinPad = GetVarIsPinpad();
	if (YES == cIsPinPad && YES == GetControlChkPinpad())
	{
		if (APP_SUCC != ChkPinpad())
		{
			return APP_FAIL;
		}
		SetControlChkPinpad(NO);
	}
	memset(szKeyIndex, 0, sizeof(szKeyIndex));
	ASSERT_QUIT(PubInputDlg("输入主密钥索引:", "(0 ~ 9)", szKeyIndex, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	nKeyIndex = atoi(szKeyIndex);
	sprintf(szTmpStr, "请输入主密钥%d:", nKeyIndex);
	nLen = (GetVarEncyptMode() == DESMODE_3DES) ? 32 : 16;

	memset(szAscMainKey, 0, sizeof(szAscMainKey));	
	ASSERT_QUIT(PubInputDlg(pszTitle, szTmpStr, szAscMainKey, &nLen, nLen, nLen, 0, INPUT_MODE_STRING));
	PubAscToHex((const uchar *)szAscMainKey, nLen, 0,(uchar *)szBcdMainKey);

	memset(szTmpStr, 0, sizeof(szTmpStr));
	ASSERT_QUIT(PubInputDlg(pszTitle, "请输入CHECK值:", szTmpStr, &nLen, 8, 8, 0, INPUT_MODE_STRING));
	memset(szCheckValue, 0, sizeof(szCheckValue));
	PubAscToHex((uchar *)szTmpStr, 8, 0, (uchar *)szCheckValue);

	nRet = MainKeyDeciphering((uchar *)szBcdMainKey, (uchar *)szResult);
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "密钥处理失败", 1, 1);
		return APP_FAIL;
	}
	
	memset(szSrc,0,sizeof(szSrc));
	memset(szDes,0,sizeof(szDes));
	if(GetVarEncyptMode() == DESMODE_3DES)
	{
		memset(szBcdMainKey,0,sizeof(szBcdMainKey));
		memcpy(szBcdMainKey,szResult,16);
		PubSoftDes3(szBcdMainKey, szSrc, szDes);
		nLen = 16;
	}
	else
	{
		memset(szBcdMainKey,0,sizeof(szBcdMainKey));
		memcpy(szBcdMainKey,szResult,8);
		PubSoftDes(szBcdMainKey,szSrc,szDes);
		nLen = 8;
	}
	if(memcmp(szDes, szCheckValue, 4) != 0)
	{
		PubMsgDlg(pszTitle, "密钥校验码错!", 1, 1);
		return APP_FAIL;
	}
	PubMsgDlg(pszTitle, "密钥校验码正确!", 0, 1);
	
	nRet = PubLoadMainKey(nKeyIndex,szBcdMainKey, nLen);
	if (nRet != APP_SUCC)
	{
		PubDispErr("安装密钥失败");
		return APP_FAIL;
	}
	SetVarMainKeyNo(nKeyIndex);	
	PubMsgDlg(pszTitle, "主密钥输入完毕!", 1, 1);
	return APP_SUCC;
}
static int SetMainKeyByICIC(void)
{
	int nKeyIndex;
	int nRet;
	int nLen;
	char szKeyIndex[2+1];
	char szBcdMainKey[16+1];
	char *pszTitle = "密钥管理";
	int nICFlag,nKeyCode;
	char szICPin[12+1];
	char szMainkeyNum[8+1];
	int nMainkeyNum;
	char szMainkeySer[12+1];
	char szOutBuf[64+1];
	char szCheckValue[8+1];
	char szMainkey[16+1];
	char szSrc[8+1];
	char szDes[8+1];
	char szAtrBuf[64+1];
	int nAtrLen;
	YESORNO cIsPinPad = NO;
	int nIndex=0;

	cIsPinPad = GetVarIsPinpad();
	if (YES == cIsPinPad && YES == GetControlChkPinpad())
	{
		if (APP_SUCC != ChkPinpad())
		{
			return APP_FAIL;
		}
		SetControlChkPinpad(NO);
	}
	memset(szKeyIndex, 0, sizeof(szKeyIndex));
	ASSERT_QUIT(PubInputDlg("输入主密钥索引:", "(0 ~ 9)", szKeyIndex, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	nKeyIndex = atoi(szKeyIndex);
	
	/*********************存储密钥卡处理********************/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayStr(1, 3, 1, "请插入密钥存储卡");
	PubUpdateWindow();
	NDK_IccGetType(ICTYPE_IC);
	while(1)
	{
		nRet = NDK_IccDetect(&nICFlag);
		if(nRet != NDK_OK)
		{
			PubMsgDlg(pszTitle, "获取卡片失败", 3, 3);
			return APP_FAIL;
		}
		if(nICFlag&0x00000001)
		{
			nRet = NDK_IccPowerUp(ICTYPE_IC, (uchar *)szAtrBuf, &nAtrLen);
			if(nRet != NDK_OK)
			{
				PubMsgDlg(pszTitle, "卡片上电失败", 3, 3);
				return APP_FAIL;
			}
			break;
		}
		nRet = NDK_KbGetCode(1, &nKeyCode);
		if(nRet != APP_SUCC||nKeyCode == KEY_ESC)
		{
			NDK_IccPowerDown(ICTYPE_IC);
			return APP_FAIL;
		}

	}
	memset(szICPin, 0, sizeof(szICPin));
	nRet = PubInputDlg(pszTitle, "输存储卡密码:", szICPin,&nLen, 
		6, 6, 60, INPUT_MODE_PASSWD);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	nRet=VerifyTerminalPin(szICPin);
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "密码验证失败", 3, 10);
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	nRet=SelectTerminalBinary();
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "选择密钥分区失败", 3, 10);
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	memset(szMainkeyNum, 0, sizeof(szMainkeyNum));
	nRet=ReadTerminalBinary((uchar *)szMainkeyNum);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	
	memset(szMainkeySer, 0, sizeof(szMainkeySer));
	nRet=PubInputDlg(pszTitle, "请输密钥序号:", szMainkeySer, \
		&nLen, 0, 11, 60, INPUT_MODE_STRING);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	nMainkeyNum = atoi(szMainkeyNum);
	for(nIndex=0; nIndex<nMainkeyNum; nIndex++)	
	{
		char szMainkeyNoIc[12+1];
		int nMidCode;
		memset(szOutBuf, 0, sizeof(szOutBuf));
		nRet=ReadTerminalMainkey(nIndex,(uchar *)szOutBuf);
		if(nRet != APP_SUCC)
		{
			NDK_IccPowerDown(ICTYPE_IC);
			return APP_FAIL;
		}
		nMidCode=szOutBuf[1]*256+szOutBuf[2];
		sprintf(szMainkeyNoIc, "%3.3d%5.5d%3.3d",szOutBuf[0],nMidCode,szOutBuf[3]);
		if(memcmp(szMainkeyNoIc, szMainkeySer, 11) == 0)
		{
			break;
		}
	}
	
	if(nIndex == nMainkeyNum)
	{
		PubMsgDlg(pszTitle, "未找到相应序号!", 1, 1);
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	PubClearAll();
	PubDisplayInv(1, pszTitle);
	nRet = PubConfirmDlg(pszTitle, "请移走存储卡,\n插入维护人员卡", 1, 0);
	if(nRet != APP_SUCC)
	{
		NDK_IccPowerDown(ICTYPE_IC);
		return APP_FAIL;
	}
	NDK_IccPowerDown(ICTYPE_IC);
	memset(szMainkey, 0, sizeof(szMainkey));
	memset(szCheckValue, 0, sizeof(szCheckValue));
	memcpy(szMainkey, szOutBuf+4, 16);
	memcpy(szCheckValue, szOutBuf+20, 4);
	//fprintf(stderr,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",szMainkey[0],szMainkey[1],szMainkey[2],szMainkey[3],szMainkey[4],szMainkey[5],szMainkey[6],szMainkey[7],szMainkey[8],szMainkey[9],szMainkey[10],szMainkey[11],szMainkey[12],szMainkey[13],szMainkey[14],szMainkey[15]);
	memset(szOutBuf,0,sizeof(szOutBuf));
	nRet = MainKeyDeciphering((uchar *)szMainkey, (uchar *)szOutBuf);
	if(nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "密钥处理失败", 1, 1);
		return APP_FAIL;
	}
	
	memset(szSrc,0,sizeof(szSrc));
	memset(szDes,0,sizeof(szDes));
	if(GetVarEncyptMode() == DESMODE_3DES)
	{
		memset(szBcdMainKey,0,sizeof(szBcdMainKey));
		memcpy(szBcdMainKey,szOutBuf,16);
		PubSoftDes3(szBcdMainKey, szSrc, szDes);
		nLen=16;
	}
	else
	{
		memset(szBcdMainKey,0,sizeof(szBcdMainKey));
		memcpy(szBcdMainKey,szOutBuf,8);
		PubSoftDes(szBcdMainKey,szSrc,szDes);
		nLen=8;
	}
	if(memcmp(szDes, szCheckValue, 4) != 0)
	{
		PubMsgDlg(pszTitle, "密钥校验码错!", 1, 1);
		return APP_FAIL;
	}
	PubMsgDlg(pszTitle, "密钥校验码正确!", 1, 1);
	nRet = PubLoadMainKey(nKeyIndex,szBcdMainKey, nLen);
	if (nRet != APP_SUCC)
	{
		PubDispErr("安装密钥失败");
		return APP_FAIL;
	}
	SetVarMainKeyNo(nKeyIndex);	
	PubMsgDlg(pszTitle, "主密钥输入完毕!", 1, 1);
	return APP_SUCC;
	
}

int SetMainKeyByIC(void)
{
	int	nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList( "1.IC卡导密钥密文||2.手输密钥密文", "IC卡导入密钥",  0xFF, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return nRet;
		}
		switch(nSelect)
		{
		case 1:
			if( YES == GetVarIsPinpad())
			{
				PubMsgDlg("IC卡导密钥密文",  "请确认密码键盘\n已经正确连接", 0, 0);
			}
			SetMainKeyByICIC();
			break;
		case 2:
			if( YES == GetVarIsPinpad())
			{
				PubMsgDlg("手输密钥密文",  "请确认密码键盘\n已经正确连接", 0, 0);
			}
			SetMainKeyByHandIC();
			break;
		}
	}
}

