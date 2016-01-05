/**
* @file magtxn.c
* @brief 交易处理模块（磁条卡交易部分）
* @version  1.0
* @author 张捷、薛震
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "custody.h"

#define _DEFAULT_PAN_		"0000000000000000"


/**
* @brief 系统数据结构转化成冲正记录结构
* @param in  STSYSTEM *pstSystem 转换后的系统数据结构
* @param out STREVERSAL *pstReversal存放冲正信息的结构指针
* @return 无
*/
void SystemToReveral(const STSYSTEM *pstSystem, STREVERSAL *pstReversal)
{
	pstReversal->cTransType = pstSystem->cTransType;
	pstReversal->cTransAttr = pstSystem->cTransAttr;
	pstReversal->cEMV_Status = pstSystem->cEMV_Status;
	memcpy(pstReversal->szTransCode, pstSystem->szTransCode, 6);
	memcpy(pstReversal->szDate, pstSystem->szDate, 8);
	memcpy(pstReversal->szTime, pstSystem->szTime, 6);
	memcpy(pstReversal->szTrace, pstSystem->szTrace, 6);
	memcpy(pstReversal->szBatchNo, pstSystem->szBatchNo, 6);
	memcpy(pstReversal->szInvoiceNo, pstSystem->szInvoiceNo, 6);
	memcpy(pstReversal->szPan, pstSystem->szPan, 19);
	memcpy(pstReversal->szExpDate, pstSystem->szExpDate, 4);
	memcpy(pstReversal->szTelNo, pstSystem->szTelNo, 11);	
	memcpy(pstReversal->szCouponID, pstSystem->szCouponID, 20);
	memcpy(pstReversal->szInputMode, pstSystem->szInputMode, 3);
	memcpy(pstReversal->szAmount, pstSystem->szAmount, 12);
	memcpy(pstReversal->szOperNow, pstSystem->szOperNow, 2);
	strcpy(pstReversal->szOutTradeNo, pstSystem->szOutTradeNo);

	if (pstSystem->nAddFieldLen > 0 && pstSystem->nAddFieldLen <= sizeof(pstReversal->szFieldAdd1))
	{
		memcpy(pstReversal->szFieldAdd1, pstSystem->psAddField, pstSystem->nAddFieldLen);
		pstReversal->nFieldAdd1Len = pstSystem->nAddFieldLen;
	}
	else
	{
		pstReversal->nFieldAdd1Len = 0;
	}
	return ;
}



/**
* @brief 冲正记录结构转化成系统数据结构
* @param in STREVERSAL *pstReversal 冲正信息的结构指针
* @param out STSYSTEM *pstSystem 系统数据结构
* @return 无
*/

void ReveralToSystem(const STREVERSAL *pstReversal, STSYSTEM *pstSystem)
{
	pstSystem->cTransType = pstReversal->cTransType;
	pstSystem->cTransAttr = pstReversal->cTransAttr;
	pstSystem->cEMV_Status = pstReversal->cEMV_Status ;
	memcpy(pstSystem->szOldTransCode, pstReversal->szTransCode, 6);
	memcpy(pstSystem->szOldDate, pstReversal->szDate, 8);
	memcpy(pstSystem->szOldTime, pstReversal->szTime, 6);
	memcpy(pstSystem->szOldTraceNo, pstReversal->szTrace, 6);
	memcpy(pstSystem->szOldBatchNo, pstReversal->szBatchNo, 6);
	memcpy(pstSystem->szOldVoiceNo, pstReversal->szInvoiceNo, 6);
	memcpy(pstSystem->szPan, pstReversal->szPan, 19);
	memcpy(pstSystem->szExpDate, pstReversal->szExpDate, 4);
	memcpy(pstSystem->szTelNo, pstReversal->szTelNo, 11);
	memcpy(pstSystem->szCouponID, pstReversal->szCouponID, 20);
	memcpy(pstSystem->szInputMode, pstReversal->szInputMode, 3);
	memcpy(pstSystem->szAmount, pstReversal->szAmount, 12);
	memcpy(pstSystem->szOperNow, pstReversal->szOperNow, 2);
	strcpy(pstSystem->szOutTradeNo, pstReversal->szOutTradeNo);

	if (NULL != pstSystem->psAddField)
	{
		memcpy(pstSystem->psAddField, pstReversal->szFieldAdd1, pstReversal->nFieldAdd1Len);
		pstSystem->nAddFieldLen = pstReversal->nFieldAdd1Len ;
	}
	else
	{
		pstSystem->nAddFieldLen = 0;
	}

	return ;
}


/**
* @brief 自动冲正交易
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int DoReversal(void)
{
	int nRet = 0;
	int nPackLen = 0;
	int nFieldLen = 0;
	int nReSend = 0;
	int nConnectFailNum = 0 ;
	char sPackBuf[MAX_PACK_SIZE];
	char cMaxReSend;
	char sAddField1[256] = {0};
	char szDispBuf[30] = {0};
	STSYSTEM stSystem;
	STREVERSAL stReversal;
	int nTagSum;
	char *pszTitle = "冲正";


	if (YES != GetVarIsReversal())
	{
		SetVarHaveReversalNum(0);
		return APP_SUCC;
	}
	memset(&stSystem, 0, sizeof(STSYSTEM));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	GetVarCommReSendNum((char *)&cMaxReSend);

	GetVarHaveReversalNum(&nReSend);/**<获取已冲正次数*/

	/**<这个地方要先取冲正的数据，可能冲正次数已经到了要去打故障单了，这是数据要取到*/
	GetVarReversalData((char *)&stReversal, &nFieldLen);

	memset(&stSystem, 0, sizeof(STSYSTEM));
	DealSystem(&stSystem);
	stSystem.psAddField = sAddField1;/**<指定附加域的指针*/
	ReveralToSystem(&stReversal, &stSystem);

	nConnectFailNum = 0;

	while(nReSend <= cMaxReSend)
	{
		GetVarReversalData((char *)&stReversal, &nFieldLen);
		memset(&stSystem, 0, sizeof(STSYSTEM));
		DealSystem(&stSystem);
		stSystem.psAddField = sAddField1;/**<指定附加域的指针*/
		ReveralToSystem(&stReversal, &stSystem);
		PubDisplayTitle(pszTitle);
		nRet = CommConnect();
		if (nRet != APP_SUCC)
		{
		    nConnectFailNum++;
			if(nConnectFailNum >= cMaxReSend)
			{
				return APP_FAIL;
			}
			else
			{
				continue;
			}
		}
		nConnectFailNum = 0;
		PubClearAll();
		PubDisplayTitle(pszTitle);
		
		DealSystem(&stSystem);
		CustodyTlvInit();
		memcpy(stSystem.szTransCode, "400012", 6);
		memset(sPackBuf, 0, sizeof(sPackBuf));
		nPackLen =0;
		nTagSum=0;
		
		ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	    ASSERT_FAIL(CustodyTlvAddValue(14, stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(20, stSystem.szOldTraceNo, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(24,	stSystem.szOldTransCode, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(35,	stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
		/**
		*拨号
		*/
		ASSERT_FAIL(CommConnect());
		ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
				
		IncVarTraceNo();
		SetVarHaveReversalNum(nReSend++);
		ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
		ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
		CommHangUpSocket();
		ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
		nRet = CustodyChkMac(stSystem.szMac);
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
			return APP_FAIL;
		}	

		if ((memcmp(stSystem.szResponse, "00", 2) == 0)
	        || (memcmp(stSystem.szResponse, "12", 2) == 0) \
	        || (memcmp(stSystem.szResponse, "25", 2) == 0)
	   		)
		{
			;
		}
		else
		{
			CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		}
		SetVarIsReversal(NO);
		SetVarHaveReversalNum(0);
		break;
	}

	if (nReSend > cMaxReSend)
	{
		PubGetStrFormat(0, szDispBuf, "|C冲正失败");
		PubMsgDlg(NULL, szDispBuf, 0, 3);
		if (YES == GetVarIsPrintErrReport())
		{
			STWATER stWater;

			memset(&stWater, 0, sizeof(STWATER));
			stSystem.cTransAttr = 0;
			SysToWater(&stSystem, &stWater);
			PrintWater(&stWater, REVERSAL_PRINT);
		}
		SetVarIsReversal(NO);
		SetVarHaveReversalNum(0);
	}
	else
	{
		PubClearAll();
		PubDisplayGen(3, "冲 正 成 功");
		PubUpdateWindow();
		PubGetKeyCode(1);
	}
	return APP_SUCC;
}


int AutoDoReversal()
{
	if (YES != GetVarIsReversal())
	{
		return APP_SUCC;
	}
	DoReversal();
	CommHangUp();
	return APP_SUCC;
}


static int ProSelectCoupon(STCOUPON *pstCoupon, char *pszCouponInfo)
{
	int nCouponNum=0;
	char szTmp[64];
	int i=0;
	int nOffset=0;
	STCOUPON stCouponList[10];
	char szConten[1024];
	int nRet;
	int nSelect=0;

	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, pszCouponInfo, 2);
	memset((char *)&stCouponList, 0, sizeof(stCouponList));
	nOffset = 7;
	nCouponNum = atoi(szTmp);

	//单个直接返回无需选择
	if (nCouponNum == 1) 
	{
		memcpy((char *)pstCoupon, pszCouponInfo+nOffset, 48);
		
		return APP_SUCC;
	}
	else if (nCouponNum >= 10)
	{
		nCouponNum = 9;
	}

	memset(szConten, 0, sizeof(szConten));
	for(i=0; i<nCouponNum; i++)
	{
		if(strlen(szConten) > 0)
		{
			strcat(szConten, "||");
		}
		memcpy((char *)&stCouponList[i], pszCouponInfo+nOffset, 48);
		nOffset += 48;

#if 0

		memset(szTmp, 0, sizeof(szTmp));

		sprintf(szTmp, "%d", i+1);

		switch(stCouponList[i].ucCouponISS)
		{

			case 0x01:
				strcat(szTmp, "百度");
				break;
			case 0x03:
				strcat(szTmp, "移动");
				break;
			case 0x00:
			default:
				strcat(szTmp, "梅泰诺");
				break;
		}
		memset(szAmount, 0, sizeof(szAmount));
		PubHexToAsc(stCouponList[i].usAmount,10, 0,szAmount);
		sprintf(szTmp+strlen(szTmp), "%d元", atoi(szAmount)/100);
	
		switch(stCouponList[i].ucCouponType)
		{
			case 0x00:
				strcat(szTmp, "团购券");
				break;
			case 0x01:
				strcat(szTmp, "代金券");
				break;
			case 0x02:
			default:
				strcat(szTmp, "代金券");
				break;
		}
		strcat(szConten, szTmp);
#endif
	
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "%d", i+1);
		strcat(szConten, szTmp);
		
		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp,  stCouponList[i].usCouponName, 20);
		PubAllTrim(szTmp);		
		strcat(szConten, szTmp);
	}
	while(1)
	{				
		nRet = PubSelectListItemExtend(szConten, "卡券选择", NULL, 0xFF, &nSelect, 60, NULL);
		if (nRet==APP_QUIT)
		{
			return nRet;
		}
		if (nRet == APP_TIMEOUT)
		{
			return nRet;
		}
			
		if((nSelect <= nCouponNum) && nSelect > 0)
		{
			memcpy((char *)pstCoupon, (char *)&stCouponList[nSelect-1], sizeof(STCOUPON));
			return APP_SUCC;
		}
	}		
}

//分流程不需要挂断，由调用挂断
static int ProCouponQueryCard(char *pszTitle, STSYSTEM *pstSystem)
{
	int nRet = 0;
	int nInput;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;

	PubClearAll();
	PubDisplayTitle(pszTitle);

	/**
	* 处理刷卡等操作
	*/
	nInput = INPUT_NO;
	ASSERT_QUIT(ProInputCard(&nInput, pstSystem));

	/**
	* 预拨号
	*/
	CommPreDial();

	PubDisplayTitle(pszTitle);
	//产品要求不要显示，这一步客户可能不会再细看
	//ASSERT_FAIL(DispPan(pstSystem->szPan, DISPPAN_TIMEOUT));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(pstSystem->szAmount, 0, 13);
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", pstSystem->szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));
		/**
		* 检查金额的有效性
		*/
		if (CheckTransAmount(pstSystem->szAmount, pstSystem->cTransType ) == APP_SUCC )
		{
			break;
		}
	}
	
	/**
	* 执行冲正等
	*/
	ASSERT_FAIL(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);


	DealSystem(pstSystem);	
	CustodyTlvInit();
	memcpy(pstSystem->szTransCode, "100006", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1,pstSystem->szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, pstSystem->szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, pstSystem->szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, pstSystem->szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, pstSystem->szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, pstSystem->szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, pstSystem->szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(10, pstSystem->szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, pstSystem->szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_FAIL(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(pstSystem, sPackBuf, nPackLen));
	if (memcmp(pstSystem->szResponse, "00", 2) != 0)
	{
		CustoyDispResp(pstSystem->szResponse, pstSystem->szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(pstSystem->szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	return APP_SUCC;
	
}


int MagCardVeriCoupon(void)
{
	char *pszTitle = "银行卡验券";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPON stCoupon;
	STREVERSAL stReversal;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_CARD_VERIFY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	//Cup_OpenLedBackGround();
	ASSERT_HANGUP_FAIL(ProCouponQueryCard("银行卡券查询", &stSystem));
	
	ASSERT_HANGUP_FAIL(ProSelectCoupon(&stCoupon, stSystem.szTag32));
	memcpy(stSystem.szCouponID, stCoupon.usCouponID, 20);

	/**
	* 输入密码
	*/
	if(stCoupon.ucPinMode == 0x01)
	{
		PubClearAll();
		PubDisplayTitle(pszTitle);
		ASSERT_HANGUP_QUIT(GetPin(stSystem.szPan, stSystem.szAmount, stSystem.szPin));		
		if (memcmp(stSystem.szPin, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)
		{
			stSystem.szInputMode[2] = '2';
		}
		else
		{
			stSystem.szInputMode[2] = '1';
		}
	}

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100007", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(10, stSystem.szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(14,  stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	if(stSystem.szInputMode[2] == '1')
	{
		ASSERT_HANGUP_FAIL(CustodyTlvAddValueLen(16,  stSystem.szPin,8, sPackBuf, &nPackLen, &nTagSum));
	}
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));

	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);

	SysToWater(&stSystem, &stWater);
	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}



//分流程不需要挂断，由调用挂断
static int ProCouponQueryID(char *pszTitle, STSYSTEM *pstSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;

	PubClearAll();
	PubDisplayTitle(pszTitle);

	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入串码", sizeof(pstSystem->szCouponID)-1, pstSystem->szCouponID, NO));

	/**
	* 预拨号
	*/
	CommPreDial();

	PubDisplayTitle(pszTitle);
	//产品要求不要显示，这一步客户可能不会再细看
	ASSERT_QUIT(DispCouponID(pstSystem->szCouponID, DISPPAN_TIMEOUT));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(pstSystem->szAmount, 0, 13);
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", pstSystem->szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));
		/**
		* 检查金额的有效性
		*/
		if (CheckTransAmount(pstSystem->szAmount, pstSystem->cTransType ) == APP_SUCC )
		{
			break;
		}
	}
	/**
	* 执行冲正等
	*/
	ASSERT_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);


	DealSystem(pstSystem);	
	CustodyTlvInit();
	memcpy(pstSystem->szTransCode, "100002", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1,pstSystem->szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, pstSystem->szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, pstSystem->szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, pstSystem->szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, pstSystem->szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, pstSystem->szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, pstSystem->szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(14, pstSystem->szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, pstSystem->szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));	
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(pstSystem, sPackBuf, nPackLen));
	if (memcmp(pstSystem->szResponse, "00", 2) != 0)
	{
		CustoyDispResp(pstSystem->szResponse, pstSystem->szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(pstSystem->szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	return APP_SUCC;
	
}


int MagIDVeriCoupon(void)
{
	char *pszTitle = "串码验券";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPON stCoupon;
	STREVERSAL stReversal;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_COUPON_VERIFY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	//Cup_OpenLedBackGround();

	ASSERT_HANGUP_FAIL(ProCouponQueryID("券查询", &stSystem));
	
	ASSERT_HANGUP_FAIL(ProSelectCoupon(&stCoupon, stSystem.szTag32));
	memcpy(stSystem.szCouponID, stCoupon.usCouponID, 20);

	/**
	* 输入密码
	*/
	if(stCoupon.ucPinMode == 0x01)
	{
		PubClearAll();
		PubDisplayTitle(pszTitle);
		ASSERT_HANGUP_QUIT(GetPin(_DEFAULT_PAN_, stSystem.szAmount, stSystem.szPin));
		if (memcmp(stSystem.szPin, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)
		{
			stSystem.szInputMode[2] = '2';
		}
		else
		{
			stSystem.szInputMode[2] = '1';
		}
	}
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100005", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(10, stSystem.szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(14,  stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(16,  stSystem.szPin, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	
	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);

	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");

	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	
	return APP_SUCC;	
}



static int ProCouponQueryTelno(char *pszTitle, STSYSTEM *pstSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen=0;

	PubClearAll();
	PubDisplayTitle(pszTitle);


	ASSERT_QUIT(PubInputDlg(pszTitle, "请输入手机号:", pstSystem->szTelNo, &nLen, 11, 11, 0, INPUT_MODE_NUMBER));


	/**
	* 预拨号
	*/
	CommPreDial();

	PubDisplayTitle(pszTitle);
	//产品要求不要显示，这一步客户可能不会再细看
	//ASSERT_QUIT(DispTelno(pstSystem->szTelNo, DISPPAN_TIMEOUT));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(pstSystem->szAmount, 0, 13);
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", pstSystem->szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));
		/**
		* 检查金额的有效性
		*/
		if (CheckTransAmount(pstSystem->szAmount, pstSystem->cTransType ) == APP_SUCC )
		{
			break;
		}
	}
	/**
	* 执行冲正等
	*/
	ASSERT_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);


	DealSystem(pstSystem);	
	CustodyTlvInit();
	memcpy(pstSystem->szTransCode, "100003", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1,pstSystem->szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, pstSystem->szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, pstSystem->szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, pstSystem->szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, pstSystem->szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, pstSystem->szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, pstSystem->szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(13, pstSystem->szTelNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, pstSystem->szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(pstSystem, sPackBuf, nPackLen));
	if (memcmp(pstSystem->szResponse, "00", 2) != 0)
	{
		CustoyDispResp(pstSystem->szResponse, pstSystem->szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(pstSystem->szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	return APP_SUCC;
	
}




int MagTelnoVeriCoupon(void)
{
	char *pszTitle = "手机号验券";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPON stCoupon;
	STREVERSAL stReversal;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_TEL_VERIFY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	//Cup_OpenLedBackGround();

	ASSERT_HANGUP_FAIL(ProCouponQueryTelno("券查询", &stSystem));
	ASSERT_HANGUP_FAIL(ProSelectCoupon(&stCoupon, stSystem.szTag32));
	memcpy(stSystem.szCouponID, stCoupon.usCouponID, 20);

	/**
	* 输入密码
	*/
	if(stCoupon.ucPinMode == 0x01)
	{
		PubClearAll();
		PubDisplayTitle(pszTitle);
		ASSERT_HANGUP_QUIT(GetPin(_DEFAULT_PAN_, stSystem.szAmount, stSystem.szPin));
		if (memcmp(stSystem.szPin, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)
		{
			stSystem.szInputMode[2] = '2';
		}
		else
		{
			stSystem.szInputMode[2] = '1';
		}
	}
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100004", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(10, stSystem.szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(13,  stSystem.szTelNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(14,  stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(16,  stSystem.szPin, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));

	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);
	
	SysToWater(&stSystem, &stWater);

	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}



int ProQrCodeQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100009", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}


int MagScanQrCodePay(void)
{
	char *pszTitle = "微信支付被扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_PRECREATE;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100008", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	CommHangUpSocket();
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(36, stSystem.szCodeUrl, &nLen));
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	//条码生成,打印
	ASSERT_HANGUP_FAIL(DealQRCode(stSystem.szCodeUrl, stSystem.szAmount, stSystem.cTransType));
	
	//Print2DScan(stSystem.szCodeUrl);

	//订单查询
	ASSERT_HANGUP_QUIT(ProQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagBarcodePay(void)
{
	char *pszTitle = "微信支付主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	//ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码:", 20,stSystem.szCodeUrl));
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));
	//ASSERT_HANGUP_QUIT(ScanBarCode2(pszTitle, 60, stSystem.szCodeUrl));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100012", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
#if 0
	PubClear2To4();
	PubDisplayGen(2, "等待支付...");
	PubDisplayGen(3, "支付完成后按确认键,");
	PubDisplayGen(4, "发起订单查询.");

	CommSetSuspend(0);	//禁止休眠
	nRet = ProConfirm();
	CommSetSuspend(1);	//允许休眠
	if(nRet == APP_SUCC)
	{
		ASSERT_HANGUP_QUIT(ProQrCodeQuery("订单查询", &stSystem));
		CommHangUp();
	}
#else
	ASSERT_HANGUP_QUIT(ProQrCodeQuery("订单查询", &stSystem, NO));
	CommHangUp();
#endif
	
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}




int MagBarCodeRefund(void)
{
	char *pszTitle = "微信退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_WX_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100013", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}


/**
* @brief 消费撤销
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int VoidSale(void)
{
	int nRet = 0, nLen;
	char szInvno[6+1] = {0};
	char *pszTitle = "撤销";
	char szContent[100] = {0};
	STSYSTEM stSystem;
	STWATER stWater;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	int nTagSum=0;
	STWATER stWaterOld;
	STREVERSAL stReversal;

	memset(&stSystem, 0, sizeof(STSYSTEM));
	memset(&stWater, 0, sizeof(STWATER));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = (char)TRANS_VOID_VERIFY;
	stSystem.cTransAttr = (char)ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));

	/**
	* 处理保存的流水信息
	*/
	ASSERT_QUIT(PubInputDlg(pszTitle, "请输入原凭证号:", szInvno, &nLen, 1, 6, 0, INPUT_MODE_NUMBER));
	PubAddSymbolToStr(szInvno, 6, '0', 0);
	nRet = FindWaterWithInvno(szInvno, &stWater);
	if (nRet == APP_FAIL)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C原交易不存在");
		PubMsgDlg(pszTitle, szContent, 0, 5);
		return APP_FAIL;
	}
	else
	{
		if ((stWater.cTransType == TRANS_SALE) && (stWater.cStatus != 0))
		{
			if (2 == stWater.cStatus || 4 == stWater.cStatus)
			{
				PubMsgDlg(pszTitle, "该交易已调整,不允许撤销", 0, 5);
			}
			else
			{
				PubMsgDlg(pszTitle, "该交易已经被撤销", 0, 5);
			}

			return APP_FAIL;
		}

		switch(stWater.cTransType)
		{	
		case TRANS_COUPON_VERIFY:
			stSystem.cTransType = (char)TRANS_VOID_COUPON_VERIFY;
			pszTitle = "串码验券撤销";
			break;
		case TRANS_TEL_VERIFY:
			stSystem.cTransType = (char)TRANS_VOID_TEL_VERIFY;
			pszTitle = "手机号验券撤销";
			break;
		case TRANS_CARD_VERIFY:
			stSystem.cTransType = (char)TRANS_VOID_CARD_VERIFY;
			pszTitle = "银行卡验券撤销";
			break;
		case TRANS_PRECREATE:
			stSystem.cTransType = (char)TRANS_VOID_PRECREATE;
			pszTitle = "微信支付被扫撤销";
			break;
		case TRANS_CREATEANDPAY:
			stSystem.cTransType = (char)TRANS_VOID_CREATEANDPAY;
			pszTitle = "微信支付主扫撤销";
			break;
		case TRANS_BAIDU_PRECREATE:
			stSystem.cTransType = (char)TRANS_VOID_BAIDU_PRECREATE;
			pszTitle = "百度钱包被扫撤销";
			break;
		case TRANS_CREATEANDPAYBAIDU:
			stSystem.cTransType = (char)TRANS_VOID_CREATEANDPAYBAIDU;
			pszTitle = "百度钱包主扫撤销";
			break;
		case TRANS_JD_PRECREATE:
			stSystem.cTransType = (char)TRANS_VOID_JD_PRECREATE;
			pszTitle = "京东钱包被扫撤销";
			break;
		case TRANS_JD_CREATEANDPAY:
			stSystem.cTransType = (char)TRANS_VOID_JD_CREATEANDPAY;
			pszTitle = "京东钱包主扫撤销";
			break;
		case TRANS_ALI_PRECREATE:
			stSystem.cTransType = (char)TRANS_VOID_ALI_PRECREATE;
			pszTitle = "支付宝被扫撤销";
			break;
		case TRANS_ALI_CREATEANDPAY:
			stSystem.cTransType = (char)TRANS_VOID_ALI_CREATEANDPAY;
			pszTitle = "支付宝主扫撤销";
			break;
		case TRANS_COUPON_MEITUAN:
			stSystem.cTransType = (char)TRANS_VOID_MEITUAN;
			pszTitle = "美团撤销";
			break;
		case TRANS_DZ_PRECREATE:
			stSystem.cTransType = (char)TRANS_VOID_DZ_PRECREATE;
			pszTitle = "大众点评被扫撤销";
			break;
		case TRANS_DZ_CREATEANDPAY:
			stSystem.cTransType = (char)TRANS_VOID_DZ_CREATEANDPAY;
			pszTitle = "大众点评主扫撤销";
			break;
		case TRANS_BESTPAY_CREATEANDPAY:
			stSystem.cTransType = (char)TRANS_VOID_BESTPAY_CREATEANDPAY;
			pszTitle = "翼支付撤销";
			break;
		default:
			PubMsgDlg(pszTitle, "原交易不符条件", 3, 5);
			return APP_FAIL;
			break;
		}
		
		ASSERT_FAIL(DispVoidWater(pszTitle, &stWater));
	}

	PubClearAll();
	PubDisplayTitle(pszTitle);
	CommPreDial();
	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);


	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100010", 6);

	strcpy(stSystem.szTelNo, stWater.sTelNo);
	strcpy(stSystem.szCouponID, stWater.sCouponID);	
	strcpy(stSystem.szOutTradeNo, stWater.szOutTradeNo);
	PubHexToAsc((uchar *)stWater.szBatchNum, 6, 0, (uchar *)stSystem.szOldBatchNo);
	PubHexToAsc((uchar *)stWater.sTrace, 6, 0, (uchar *)stSystem.szOldTraceNo);
	PubHexToAsc((uchar *)stWater.szAmount,12, 0, (uchar *)stSystem.szAmount);
	PubHexToAsc((uchar *)stWater.szRealAmount,12, 0, (uchar *)stSystem.szRealAmount);	
	memcpy(stSystem.szOldRefnum, stWater.szRefnum, 12);
	memcpy(stSystem.szOldTransCode, stWater.sTransCode, 6);
	stSystem.cOldTransType = stWater.cTransType;
	
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(10, stSystem.szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(13, stSystem.szTelNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(14, stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(20, stSystem.szOldTraceNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(22, stSystem.szOldRefnum, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(24, stSystem.szOldTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));

	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/**
	* 修改原流水
	*/
	memset(&stWaterOld, 0, sizeof(STWATER));
	FindWaterWithInvno(szInvno, &stWaterOld);
	stWaterOld.cStatus = 0x01;
	UpdateWater(&stWaterOld);
	/**
	* 追加本笔流水
	*/
	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));

	if ((stSystem.cTransType == TRANS_COUPON_VERIFY) || (stSystem.cTransType == TRANS_TEL_VERIFY) ||
				(stSystem.cTransType == TRANS_CARD_VERIFY) || (stSystem.cTransType == TRANS_VOID_COUPON_VERIFY) || 
				(stSystem.cTransType == TRANS_VOID_TEL_VERIFY) || (stSystem.cTransType == TRANS_VOID_CARD_VERIFY))
	{
		ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");
	}
	else
	{
		ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	}
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	
	return APP_SUCC;	
}


/**
* @brief 磁条卡退货
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int MagRefund()
{
	char *pszTitle = "退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPON stCoupon;
	char szContent[100] = {0};
	int nFieldLen = 0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));

	stSystem.cTransType = TRANS_REFUND_VERIFY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原交易参考号
	*/
	while(1)
	{
		nFieldLen = 0;
		ASSERT_QUIT(PubInputDlg(pszTitle, "请输入原参考号:", stSystem.szOldRefnum, &nFieldLen, 1, 12, INPUT_STRING_TIMEOUT, INPUT_MODE_STRING));/**<允许英文*/
		if (12 == nFieldLen)
			break;
		else
		{
			memset(szContent, 0, sizeof(szContent));
			PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C位数不足\n|C请重新输入");
			PubMsgDlg(pszTitle, szContent, 0, 3);
		}
	}

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 输入原交易日期
	*/
	ASSERT_HANGUP_QUIT(PubInputDate(pszTitle, "请输入原交易日期\n(年年年年月月日日):", stSystem.szOldDate, INPUT_DATE_MODE_YYYYMMDD, INPUT_DATE_TIMEOUT));
	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;
		char szDispAmt[13+1] = {0};

		memset(szDispAmt, 0, sizeof(szDispAmt));
		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));

		ASSERT_HANGUP_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_MODE_AMOUNT, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount( stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}

		/**<需要再确定一次*/

		PubClear2To4();
		PubDisplayStrInline(0, 2, "请确认金额:");
		ProAmtToDispOrPnt(stSystem.szAmount, szDispAmt);
		PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 3, "|R%s", szDispAmt);
		PubDisplayStrInline(DISPLAY_MODE_CENTER, 4, "按取消键修改");
		if (APP_SUCC != ProConfirm())
		{
			continue;
		}
		break;
	}
	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100011", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(22, stSystem.szOldRefnum, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(23,  stSystem.szOldDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	stSystem.psAddField = (char *)&stCoupon;
	stSystem.nAddFieldLen = sizeof(stCoupon);	
	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;
}

int GetCouponIss(char *pszIssName, const char cIss)
{
/*
	券显示采用2个汉字，简称：
	00-显示"梅泰诺"；01-显示"百度"；02-显示"移动"
*/
	switch(cIss)
	{
		case 0x00:
			strcpy(pszIssName, "梅泰诺");
			break;
		case 0x01:
			strcpy(pszIssName, "百度");
			break;
		case 0x02:
			strcpy(pszIssName, "移动");
			break;
		default:
			strcpy(pszIssName, "梅泰诺");
			break;
	}
	return APP_SUCC;
}


int GetCouponType(char *pszTypeName,char *pszTypeNameShort, const char cType)
{
	char szTmp1[100];
	char szTmp2[100];

/*
	0x00：团购券
	0x01：代金券（赠领）
	0x02：代金券（购买）
*/
	memset(szTmp1, 0, sizeof(szTmp1));
	memset(szTmp2, 0, sizeof(szTmp2));

	switch(cType)
	{
		case 0x00:
			strcpy(szTmp1, "团购券");
			strcpy(szTmp2, "团购券");
			break;
		case 0x01:
			strcpy(szTmp1, "代金券(赠领)");
			strcpy(szTmp2, "代金券");
			break;
		case 0x02:
			strcpy(szTmp1, "代金券(购买)");
			strcpy(szTmp2, "代金券");
			break;
		default:
			strcpy(szTmp1, "团购券");
			strcpy(szTmp2, "团购券");
			break;
	}
	if(pszTypeName != NULL)
	{
		strcpy(pszTypeName, szTmp1);
	}
	if(pszTypeNameShort != NULL)
	{
		strcpy(pszTypeName, szTmp2);
	}

	return APP_SUCC;
}


int ProQrCodeQueryBaidu(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100022", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}


int MagBarcodeBaidu(void)
{
	char *pszTitle = "百度钱包主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_CREATEANDPAYBAIDU;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100021", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_QUIT(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));


	ASSERT_HANGUP_QUIT(ProQrCodeQueryBaidu("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagScanQrCodePayBaidu(void)
{
	char *pszTitle = "百度钱包被扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_BAIDU_PRECREATE;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100023", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	CommHangUpSocket();
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(36, stSystem.szCodeUrl, &nLen));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	//条码生成,打印
	ASSERT_HANGUP_FAIL(DealQRCode(stSystem.szCodeUrl, stSystem.szAmount, stSystem.cTransType));
	
	//Print2DScan(stSystem.szCodeUrl);

	//订单查询
	ASSERT_HANGUP_QUIT(ProQrCodeQueryBaidu("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagBarCodeRefundBaidu(void)
{
	char *pszTitle = "百度钱包退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_BAIDU_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100024", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}




int ProJDQrCodeQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100032", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}



int MagJDBarcode(void)
{
	char *pszTitle = "京东钱包主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_JD_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100034", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_QUIT(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));

	ASSERT_HANGUP_QUIT(ProJDQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagJDScanQrCodePay(void)
{
	char *pszTitle = "京东钱包被扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_JD_PRECREATE;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100031", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	CommHangUpSocket();
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(36, stSystem.szCodeUrl, &nLen));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	//条码生成,打印
	ASSERT_HANGUP_FAIL(DealQRCode(stSystem.szCodeUrl, stSystem.szAmount, stSystem.cTransType));
	
	//Print2DScan(stSystem.szCodeUrl);

	//订单查询
	ASSERT_HANGUP_QUIT(ProJDQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}




int MagBarJDCodeRefund(void)
{
	char *pszTitle = "京东退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_JD_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100033", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}


//支付宝


int ProAliQrCodeQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100042", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}



int MagAliBarcode(void)
{
	char *pszTitle = "支付宝主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_ALI_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100041", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));


	ASSERT_HANGUP_QUIT(ProAliQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}



int MagAlipayScanQrCodePay(void)
{
	char *pszTitle = "支付宝被扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_ALI_PRECREATE;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100044", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	CommHangUpSocket();
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(36, stSystem.szCodeUrl, &nLen));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	//条码生成,打印
	ASSERT_HANGUP_FAIL(DealQRCode(stSystem.szCodeUrl, stSystem.szAmount, stSystem.cTransType));
	
	//Print2DScan(stSystem.szCodeUrl);

	//订单查询
	ASSERT_HANGUP_QUIT(ProAliQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagBarAliCodeRefund(void)
{
	char *pszTitle = "支付宝退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_ALI_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100043", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}




//分流程不需要挂断，由调用挂断
static int ProMeituanCouponQuery(char *pszTitle, STSYSTEM *pstSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen=0;

	PubClearAll();
	PubDisplayTitle(pszTitle);

	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入串码", sizeof(pstSystem->szCouponID)-1, pstSystem->szCouponID, NO));

	/**
	* 预拨号
	*/
	CommPreDial();

	PubDisplayTitle(pszTitle);
	//产品要求不要显示，这一步客户可能不会再细看
	//ASSERT_QUIT(DispCouponID(pstSystem->szCouponID, DISPPAN_TIMEOUT));

	/**
	* 执行冲正等
	*/
	ASSERT_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(pstSystem);	
	CustodyTlvInit();
	memcpy(pstSystem->szTransCode, "100051", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1,pstSystem->szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, pstSystem->szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, pstSystem->szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, pstSystem->szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, pstSystem->szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, pstSystem->szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, pstSystem->szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(14, pstSystem->szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));	
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(pstSystem, sPackBuf, nPackLen));
	if (memcmp(pstSystem->szResponse, "00", 2) != 0)
	{
		CustoyDispResp(pstSystem->szResponse, pstSystem->szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(pstSystem->szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szAmount, &nLen));
	
	return APP_SUCC;
	
}


int MagMeituanVeriCoupon(void)
{
	char *pszTitle = "美团验券";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPONMEITUAN stCoupon;
	STREVERSAL stReversal;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_COUPON_MEITUAN;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	ASSERT_HANGUP_FAIL(ProMeituanCouponQuery("券查询", &stSystem));

	strcpy((char *)stCoupon.usCouponID, stSystem.szCouponID);
	PubAscToHex((uchar *)stSystem.szAmount, 12, 0, stCoupon.usAmount);
	memcpy(stCoupon.usCouponName, stSystem.szTag32, 20);

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100052", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	//ASSERT_FAIL(CustodyTlvAddValue(10, stSystem.szPan, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(14,  stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	
	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);

	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");

	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	
	return APP_SUCC;	
}


int MagPanjintong(void)
{
	char *pszTitle = "盘锦通支付";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_PANJINTONG;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCouponID)-1, stSystem.szCouponID, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100061", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(14, stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

#if 0//盘锦通不需要冲正
	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);
#endif

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));


	//ASSERT_HANGUP_QUIT(ProAliQrCodeQuery("订单查询", &stSystem));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
#if 0
	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
#endif	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}



//分流程不需要挂断，由调用挂断
static int ProDazhongCouponQuery(char *pszTitle, STSYSTEM *pstSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen=0;

	PubClearAll();
	PubDisplayTitle(pszTitle);

	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入串码", sizeof(pstSystem->szCouponID)-1, pstSystem->szCouponID, NO));

	/**
	* 预拨号
	*/
	CommPreDial();

	PubDisplayTitle(pszTitle);
	//产品要求不要显示，这一步客户可能不会再细看
	//ASSERT_QUIT(DispCouponID(pstSystem->szCouponID, DISPPAN_TIMEOUT));

	/**
	* 执行冲正等
	*/
	ASSERT_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(pstSystem);	
	CustodyTlvInit();
	memcpy(pstSystem->szTransCode, "100071", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1,pstSystem->szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, pstSystem->szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, pstSystem->szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, pstSystem->szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, pstSystem->szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, pstSystem->szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, pstSystem->szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(14, pstSystem->szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));	
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(pstSystem, sPackBuf, nPackLen));
	if (memcmp(pstSystem->szResponse, "00", 2) != 0)
	{
		CustoyDispResp(pstSystem->szResponse, pstSystem->szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(pstSystem->szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szAmount, &nLen));
	
	return APP_SUCC;
	
}


int MagDazhongVeriCoupon(void)
{
	char *pszTitle = "大众点评验券";
	STSYSTEM stSystem;
	int nRet;
	int nLen;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPONMEITUAN stCoupon;
	STREVERSAL stReversal;
	char szTmp[64];

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_COUPON_DAZHONG;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	ASSERT_HANGUP_FAIL(ProDazhongCouponQuery("大众券查询", &stSystem));

	strcpy((char *)stCoupon.usCouponID, stSystem.szCouponID);
	PubAscToHex((uchar *)stSystem.szAmount, 12, 0, stCoupon.usAmount);
	memcpy(stCoupon.usCouponName, stSystem.szTag32, 20);

	nLen = atoi(stSystem.szAmount);
	if (nLen <= 0)
	{
		PubMsgDlg(pszTitle,"无可用券",3,10);
		CommHangUp();
		return APP_FAIL;
	}
	else if (nLen > 1)
	{
		sprintf(szTmp, "请输兑券数<=%d张", nLen);
		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_HANGUP_FAIL(PubInputDlg(pszTitle, szTmp, stSystem.szAmount, &nRet, 1, 10, 60, INPUT_MODE_NUMBER));
		PubAddSymbolToStr(stSystem.szAmount, 12, '0', 0);
	}	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100072", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(14,  stSystem.szCouponID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
	//SystemToReveral(&stSystem, &stReversal);
	//SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	//SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	
	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);

	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");

	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	
	return APP_SUCC;	
}



int ProDazhongQrCodeQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100075", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szAmount, &nLen));
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}



int MagDazhongBarcode(void)
{
	char *pszTitle = "大众点评主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_DZ_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100074", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));

	ASSERT_HANGUP_QUIT(ProDazhongQrCodeQuery("大众点评订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagDazhongScanQrCodePay(void)
{
	char *pszTitle = "大众点评被扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_DZ_PRECREATE;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100073", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	CommHangUpSocket();
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(36, stSystem.szCodeUrl, &nLen));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	//条码生成,打印
	ASSERT_HANGUP_FAIL(DealQRCode(stSystem.szCodeUrl, stSystem.szAmount, stSystem.cTransType));
	
	//Print2DScan(stSystem.szCodeUrl);

	//订单查询
	ASSERT_HANGUP_QUIT(ProDazhongQrCodeQuery("大众点评订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}




int MagBarDazhongCodeRefund(void)
{
	char *pszTitle = "大众点评退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_DZ_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100076", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}



int MagDazhongConfirm(void)
{
	char *pszTitle = "大众点评订单确认";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STCOUPONMEITUAN stCoupon;
	STREVERSAL stReversal;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset((char *)&stCoupon, 0, sizeof(stCoupon));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_COUPONFRM_DAZHONG;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入订单码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100077", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(36,  stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	
	
	/**
	* 保存冲正数据
	*/
//	SystemToReveral(&stSystem, &stReversal);
//	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
//	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));

	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	
	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		SetVarIsReversal(NO);
		memset(&stReversal, 0, sizeof(STREVERSAL));
		SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	memcpy(stWater.sAddition, (char *)&stCoupon, sizeof(stCoupon));
	stWater.nAdditionLen = sizeof(stCoupon);

	SysToWater(&stSystem, &stWater);
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szRealAmount, stSystem.cTransType, "CUP");

	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	
	return APP_SUCC;
}




int ProBestpayQrCodeQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100082", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}



int MagBestpayBarcode(void)
{
	char *pszTitle = "翼支付主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_BESTPAY_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100081", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));


	ASSERT_HANGUP_QUIT(ProBestpayQrCodeQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}


int MagBarBestpayCodeRefund(void)
{
	char *pszTitle = "翼支付退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_BESTPAY_REFUND;
	stSystem.cTransAttr = ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100083", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}




//都能付

int ProAllpayQuery(char *pszTitle, STSYSTEM *pstSystem, char isNeedSystem)
{
	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE] = {0};
	int nTagSum=0;
	int nPackLen=0;
	int nLen;
	STSYSTEM stSystem;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memcpy((char *)&stSystem, pstSystem, sizeof(STSYSTEM));
RETRY:	

	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);	
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100092", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nTagSum = 0;
	nPackLen = 0;
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));
	
	/**
	*拨号
	*/
	ASSERT_QUIT(CommConnect());
	ASSERT_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUpSocket();
	
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	/*判断响应码*/
	if(memcmp(stSystem.szResponse, "Z1", 2) == 0)
	{
		PubClear2To4();
		PubDisplayGen(2, "等待支付...");
		PubDisplayGen(3, "支付完成后按确认键,");
		PubDisplayGen(4, "发起订单查询.");
		if(ProConfirm() == APP_SUCC)
		{
			goto RETRY;
		}
		else
		{
			return APP_FAIL;
		}
	}
	else if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse,stSystem.szRespInfo);
		return APP_QUIT;
	}
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, pstSystem->szRealAmount, &nLen));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(29, stSystem.szRealAmount, &nLen));
	
	if (isNeedSystem == YES)
	{
		memset((char *)pstSystem, 0, sizeof(stSystem));
		memcpy(pstSystem, (char *)&stSystem, sizeof(STSYSTEM));
	}

	return APP_SUCC;
}


int MagAllpayBarcode(void)
{
	char *pszTitle = "都能付主扫";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;
	STREVERSAL stReversal;
	int nLen =0;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));
	memset(&stReversal, 0, sizeof(STREVERSAL));

	stSystem.cTransType = TRANS_ALLPAY_CREATEANDPAY;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));


	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();
	/**
	* 处理扫描等操作
	*/
	PubClearAll();
	PubDisplayTitle(pszTitle);
	
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入支付码", sizeof(stSystem.szCodeUrl)-1, stSystem.szCodeUrl, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100091", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyTlvAddValue(36, stSystem.szCodeUrl, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_QUIT(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	/**
	* 保存冲正数据
	*/
	SystemToReveral(&stSystem, &stReversal);
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	SetVarIsReversal(YES);

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	CommHangUpSocket();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CommHangUp();
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		CommHangUp();
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	
	
	nLen = 0;
	ASSERT_HANGUP_FAIL(CustodyGetField(35, stSystem.szOutTradeNo, &nLen));


	ASSERT_HANGUP_QUIT(ProAllpayQuery("订单查询", &stSystem, NO));

	CommHangUp();
	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	

	SetVarIsReversal(NO);
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));
	return APP_SUCC;	
}




int MagAllPayRefund(void)
{
	char *pszTitle = "都能付退货";
	STSYSTEM stSystem;
	int nRet;
	int nTagSum=0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STWATER stWater;

	memset((char *)&stSystem, 0, sizeof(stSystem));
	memset((char *)&stWater, 0, sizeof(stWater));

	stSystem.cTransType = TRANS_ALLPAY_REFUND;
	stSystem.cTransAttr =	ATTR_MAG;

	ASSERT_QUIT(TradeInit(pszTitle, TDI_CHECKOPER, stSystem.cTransType, 0, YES));


	/**
	* 输入原商户订单号
	*/
	ASSERT_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, YES));

	/**
	* 输入金额
	*/
	while(1)
	{
		int nAmtLen = 12;

		memset(stSystem.szAmount, 0, sizeof(stSystem.szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", stSystem.szAmount, &nAmtLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));

		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(stSystem.szAmount, stSystem.cTransType) != APP_SUCC )
		{
			continue;
		}
		break;
	}
	

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);

	DealSystem(&stSystem);
	CustodyTlvInit();
	memcpy(stSystem.szTransCode, "100094", 6);
	memset(sPackBuf, 0, sizeof(sPackBuf));
	nPackLen =0;
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(29, stSystem.szAmount, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyTlvAddValue(35, stSystem.szOutTradeNo, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyAddMac(sPackBuf, &nPackLen, &nTagSum));

	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));

	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));	
	ASSERT_HANGUP_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_HANGUP_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));

	CommHangUp();
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	nRet = CustodyChkMac(stSystem.szMac);
	ASSERT(nRet);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "\n主机报文的MAC错", 3, 10);
		return APP_FAIL;
	}	

	SysToWater(&stSystem, &stWater);	
	nRet = AppendWater(&stWater);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}	
	ChangeSettle(stSystem.szAmount, stSystem.cTransType, "CUP");
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayGen(5, "正 在 打 印");
	PubUpdateWindow();
	ASSERT_FAIL(PrintWater(&stWater, FIRSTPRINT));

	return APP_SUCC;	
}


int MagBalance(char cTransType)
{
	char *pszTitle = NULL;
	STSYSTEM stSystem;
	int nRet;

	memset((char *)&stSystem, 0, sizeof(stSystem));

	stSystem.cTransType = cTransType;
	stSystem.cTransAttr = ATTR_MAG;

	switch(cTransType)
	{
	case TRANS_BALANCE_ALIPAY:
		pszTitle = "支付宝交易查询";
		if (GetVarIsHaveScanner() == YES)
		{
			stSystem.cTransType = TRANS_ALI_CREATEANDPAY;
		}
		else
		{
			stSystem.cTransType = TRANS_ALI_PRECREATE;
		}
		break;
	case TRANS_BALANCE_BAIDU:
		pszTitle = "百度交易查询";
		if (GetVarIsHaveScanner() == YES)
		{
			stSystem.cTransType = TRANS_CREATEANDPAYBAIDU;
		}
		else
		{
			stSystem.cTransType = TRANS_BAIDU_PRECREATE;
		}
		break;
	case TRANS_BALANCE_JD:
		pszTitle = "京东交易查询";
		if (GetVarIsHaveScanner() == YES)
		{
			stSystem.cTransType = TRANS_JD_CREATEANDPAY;
		}
		else
		{
			stSystem.cTransType = TRANS_JD_PRECREATE;
		}
		break;
	case TRANS_BALANCE_WECHAT:
		pszTitle = "微信交易查询";
		if (GetVarIsHaveScanner() == YES)
		{
			stSystem.cTransType = TRANS_CREATEANDPAY;
		}
		else
		{
			stSystem.cTransType = TRANS_PRECREATE;
		}
		break;
	case TRANS_BALANCE_MEITUAN:
		pszTitle = "美团交易查询";
		stSystem.cTransType = TRANS_COUPON_MEITUAN;
		break;
	case TRANS_BALANCE_DZ:
		pszTitle = "大众点评订单查询";
		if (GetVarIsHaveScanner() == YES)
		{
			stSystem.cTransType = TRANS_DZ_CREATEANDPAY;
		}
		else
		{
			stSystem.cTransType = TRANS_DZ_PRECREATE;
		}
		break;
	case TRANS_BALANCE_ALLPAY:
		pszTitle = "都能付订单查询";
		stSystem.cTransType = TRANS_ALLPAY_CREATEANDPAY;
		break;
	case TRANS_BALANCE_BESTPAY:
		pszTitle = "翼支付订单查询";
		stSystem.cTransType = TRANS_BESTPAY_CREATEANDPAY;
		break;
	default:
		break;
	}

	ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, stSystem.cTransType, 0, YES));

	/**
	* 预拨号
	*/
	CommPreDial();

	/**
	* 输入原商户订单号
	*/
	ASSERT_HANGUP_QUIT(ScanBarCode(pszTitle, "请输入原商户订单号", sizeof(stSystem.szOutTradeNo)-1, stSystem.szOutTradeNo, NO));

	/**
	* 执行冲正等
	*/
	ASSERT_HANGUP_QUIT(DoReversal());
	PubClearAll();
	PubDisplayTitle(pszTitle);


	nRet = APP_FAIL;
	switch(cTransType)
	{
	case TRANS_BALANCE_ALIPAY:
		nRet = ProAliQrCodeQuery(pszTitle,&stSystem, YES);
		break;
	case TRANS_BALANCE_BAIDU:
		nRet = ProQrCodeQueryBaidu(pszTitle,&stSystem, YES);
		break;
	case TRANS_BALANCE_JD:
		nRet = ProJDQrCodeQuery(pszTitle,&stSystem, YES);
		break;
	case TRANS_BALANCE_WECHAT:
		nRet = ProQrCodeQuery(pszTitle,&stSystem, YES);
		break;
	case TRANS_BALANCE_MEITUAN:
		nRet = ProMeituanCouponQuery(pszTitle,&stSystem);
		break;
	case TRANS_BALANCE_DZ:
		nRet = ProDazhongQrCodeQuery(pszTitle,&stSystem, YES);
		break;
	case TRANS_BALANCE_ALLPAY:
		nRet = ProAllpayQuery(pszTitle,&stSystem, YES);
		break;
	case TRANS_BESTPAY_CREATEANDPAY:
		nRet = ProBestpayQrCodeQuery(pszTitle,&stSystem, YES);
		break;
	default:
		break;
	}

	CommHangUp();
	
	if (nRet == APP_SUCC)
	{
		STWATER stWater;

		memset(&stWater, 0, sizeof(stWater));
		SysToWater(&stSystem, &stWater);
		PubClearAll();
		PubDisplayTitle(pszTitle);
		PubDisplayGen(5, "正 在 打 印");
		PubUpdateWindow();
		ASSERT_FAIL(PrintWater(&stWater, REPRINT));
	}

	return APP_SUCC;	
}


