/**
* @file water.c
* @brief 流水处理模块
* @version  1.0
* @author 薛震张捷
* @date 2007-01-25
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


/**
* 以下为全局变量
*/
static int gnCurrentRecNo=0;		/**<当前记录号*/

/**
* 以下为接口内部使用的函数
*/

/**
* @brief 初始化流水记录文件
*
*	流水文件不存在则创建新文件，如果已存在则重新创建原有的流水记录被删除。
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitWaterFile(void)
{
	STRECFILE stWaterFile;
	STWATER stWaterRec;

	strcpy( stWaterFile.szFileName, FILE_WATER);
	stWaterFile.cIsIndex = FILE_CREATEINDEX;						/**< 创建索引文件*/
	stWaterFile.unMaxOneRecLen = sizeof(STWATER);
	stWaterFile.unIndex1Start =(uint)((char *)(stWaterRec.sTrace) - (char *)&stWaterRec)+1;
	stWaterFile.unIndex1Len =  sizeof(stWaterRec.sTrace);
	stWaterFile.unIndex2Start =  1;
	stWaterFile.unIndex2Len = 1;
	gnCurrentRecNo = 0;
	ASSERT_FAIL(PubCreatRecFile(&stWaterFile));
#if defined (SUPPORT_ELECSIGN)
	DelElecSignFile();
#endif	
	return APP_SUCC;
}

/**
* @brief 增加流水记录
* @param in const STWATER *pstWater 流水结构指针
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int AppendWater(const STWATER *pstWater)
{	
	PubGetRecSum(FILE_WATER, &gnCurrentRecNo );
	ASSERT_FAIL(PubAddRec(FILE_WATER, (const char *)pstWater ));
	gnCurrentRecNo++;
	return APP_SUCC;
}

/**
* @brief 修改当前的流水记录
* @param in const STWATER *pstWater 流水结构指针
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int UpdateWater(const STWATER *pstWater)
{
	return PubUpdateRec (FILE_WATER, gnCurrentRecNo,  (const char *)pstWater);
}

/**
* @brief 根据提供的票据号查找相应的流水记录
* @param in const char *szInvno 需要查找的流水的票据号
* @param in int *pnRecNo           返回查找到的流水记录号
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FindWaterWithInvno( const char *pszInvno, STWATER *pstWater)
{
	char szInvno[6+1];

	PubAscToHex((uchar *)pszInvno, 6, 0, (uchar *)szInvno);
	return PubFindRec (FILE_WATER, szInvno, NULL,1, (char *)pstWater, &gnCurrentRecNo);
}

 /**
* @brief 根据票据号查找流水并显示在屏幕上
* 
*  在POS屏幕上提示输入票据号,
*  并根据提供的票据号查找相应的流水记录,
*  在屏幕上显示该笔交易流水
* @param in int PrintFlag  值为1时供打印菜单重新打印某流水使用	
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FindByInvoice(int PrintFlag)
{
	int nRet, nLen;
	STWATER stWater;
	char szInvno[6+1];
	char szDispTitle[32];

	if (PrintFlag)
	{
		strcpy(szDispTitle, "重打任意一笔");
	}
	else
	{
		strcpy(szDispTitle, "按凭证号查询");
	}

	nLen = 6;
	memset(szInvno, 0, sizeof(szInvno));
	ASSERT_QUIT(PubInputDlg(szDispTitle, "请输六位流水号:", szInvno, &nLen, 1, 6, 0, INPUT_MODE_NUMBER));	
	PubAddSymbolToStr(szInvno, 6, '0', 0);
	nRet = FindWaterWithInvno( szInvno, &stWater);
	if (nRet==APP_FAIL)
	{
		char szDispBuf[100];
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C无交易");
		PubMsgDlg(szDispTitle, szDispBuf, 0, 5);
		return APP_FAIL;		
	}
	else
	{

		nRet = DispWater(&stWater);
		if(nRet != KEY_ENTER)
			return APP_QUIT;
		if (PrintFlag)
		{	
			nRet = PubConfirmDlg(szDispTitle, "是否打印该票据?", 0, 50);
			if (nRet == APP_SUCC)
			{
				PubClearAll();
				DISP_PRINTING_NOW;
				PrintWater(&stWater, REPRINT);
			}
		}
	}
	return APP_SUCC;
}

/**
* @brief 取当前流水记录，将记录值放入STWATER *pstWater中
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchCurrentWater(STWATER *pstWater)
{
	return PubReadOneRec(FILE_WATER,gnCurrentRecNo, (char *)pstWater);	
}

/**
* @brief 取流水记录首记录
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchFirstWater(STWATER *pstWater)
{
	gnCurrentRecNo = 1;
	return FetchCurrentWater(pstWater);
}

/**
* @brief 取下一条流水记录
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchNextWater(STWATER *pstWater)
{
	int nRecNum;

	PubGetRecSum(FILE_WATER, &nRecNum );
	gnCurrentRecNo++;
	if( gnCurrentRecNo > nRecNum )
	{
		gnCurrentRecNo = nRecNum;
	}
	return FetchCurrentWater(pstWater);
}

/**
* @brief 取上一条流水记录
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchPreviousWater(STWATER *pstWater)
{
	gnCurrentRecNo--;
	if( gnCurrentRecNo ==  0 )
	{
		gnCurrentRecNo = 1;
	}
	return FetchCurrentWater(pstWater);
}

/**
* @brief 取最后一条流水记录
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchLastWater(STWATER *pstWater)
{
	PubGetRecSum(FILE_WATER, &gnCurrentRecNo );
	return FetchCurrentWater(pstWater);
}


int DispWater(const STWATER *pstWater)
{
	char szDispBuf[30], szContent[512], szTemp[30];
	int nKey, nRet = 0;

	/**
	* 显示交易类型
	*/
	switch(pstWater->cTransType)
	{
	case TRANS_COUPON_VERIFY:
		sprintf(szContent, "串码验券:");
		break;
	case TRANS_TEL_VERIFY:
		sprintf(szContent, "手机号验券:");
		break;
	case TRANS_CARD_VERIFY:
		sprintf(szContent, "卡号验券:");
		break;
	case TRANS_VOID_COUPON_VERIFY:
		sprintf(szContent, "串码验券撤销:");
		break;
	case TRANS_VOID_TEL_VERIFY:
		sprintf(szContent, "手机号验券撤销:");
		break;
	case TRANS_VOID_CARD_VERIFY:
		sprintf(szContent, "卡号验券撤销:");
		break;
	default:
		sprintf(szContent, "未知:%d",pstWater->cTransType);
		break;
	}

	/*--------显示原票据号------------*/
	switch(pstWater->cStatus)
	{
	case 3:		/**< 退货*/
		break;
	case 1:		/**< 撤消*/
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(已撤)", szContent);
		break;
	case 2:
		break;
	default:		/**< 正常*/
		switch(pstWater->cTransType)
		{
		case TRANS_VOID_SALE:
		case TRANS_VOID_INSTALMENT:
		case TRANS_VOID_AUTHSALE:
		case TRANS_ADJUST:	/**<放到下面判断*/
			if(pstWater->cSendFlag < 0xFD)
			{	
				PubHexToAsc((uchar *)pstWater->sOldTrace, 6, 0, (uchar *)szDispBuf);
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R%6.6s", szContent,szDispBuf);
	    	}
			break;
	    	default:	
			break;
		}
		break;
	}
	if(pstWater->cTransType == TRANS_ADJUST && pstWater->cSendFlag >= 0xFD)
	{
		PubHexToAsc((uchar *)pstWater->sOldTrace, 6, 0, (uchar *)szDispBuf);
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R%6.6s(已送)", szContent,szDispBuf);
	}
	else if(pstWater->cSendFlag >= 0xFD && (pstWater->cStatus == 2 || pstWater->cStatus == 4))
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(已调已送)", szContent);
	}
	else if(pstWater->cSendFlag >= 0xFD)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(已送)", szContent);
	}
	else if(pstWater->cStatus == 2 || pstWater->cStatus == 4)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(已调)", szContent);
	}

	//判断卡号是否存在
	if(pstWater->nPanLen > 0)
	{
		//卡号
		memset(szDispBuf, 0, sizeof(szDispBuf));
		PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szDispBuf);
		CtrlCardPan((uchar*)szDispBuf, (uint)pstWater->nPanLen, (uint)pstWater->cTransType, pstWater->cEMV_Status);
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", szDispBuf);
	}
	else if(strlen(pstWater->sTelNo) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->sTelNo);
	}
	else if(strlen(pstWater->sCouponID) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->sCouponID);
	}
	else if(strlen(pstWater->szOutTradeNo) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->szOutTradeNo);
	}
	
	//凭证号
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nVOUCH NO:|R%s", szDispBuf);

	//金额		
	memset(szTemp, 0, sizeof(szTemp));
	memset(szDispBuf, 0, sizeof(szDispBuf));
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szTemp);	
	ProAmtToDispOrPnt(szTemp, szDispBuf);
	PubAllTrim(szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nAMOUNT:|R%s", szDispBuf);

	//系统参考号
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent),"\nREF:|R%s", pstWater->szRefnum);
	
	nRet = PubUpDownMsgDlg(NULL, szContent, NO, 60, &nKey);	
	
	if(nRet == APP_QUIT)
		return KEY_ESC;
	if(nRet == APP_FUNCQUIT && nKey == KEY_ENTER)
	{
		nRet = PubUpDownMsgDlg(NULL, szContent, YES, 60, &nKey);
		nKey = KEY_ENTER;
	}
	return nKey;
}

/**
* @brief 在屏幕上显示撤销交易的流水
* @param in STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int DispVoidWater(const char * pszTitle, const STWATER *pstWater)
{
	char szDispBuf[30], szAmount[13];
	char szContent[500];
	char szDispAmt[13+1] = {0};
	int nKey =0;


	//判断卡号是否存在
	if(pstWater->nPanLen > 0)
	{
		//卡号
		memset(szDispBuf, 0, sizeof(szDispBuf));
		PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szDispBuf);
		CtrlCardPan((uchar*)szDispBuf, (uint)pstWater->nPanLen, (uint)pstWater->cTransType, pstWater->cEMV_Status);
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", szDispBuf);
	}
	else if(strlen(pstWater->sTelNo) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->sTelNo);
	}
	else if(strlen(pstWater->sCouponID) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->sCouponID);
	}
	else if(strlen(pstWater->szOutTradeNo) > 0)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s", pstWater->szOutTradeNo);
	}
	
	//凭证号
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nVOUCH NO:|R%s", szDispBuf);

	//金额		
	memset(szAmount, 0, sizeof(szAmount));
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szAmount);	
	ProAmtToDispOrPnt(szAmount, szDispAmt);
	PubAllTrim(szDispAmt);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nAMOUNT:|R%s", szDispAmt);

	//系统参考号
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent),"\nREF:|R%s", pstWater->szRefnum);
	PubUpDownMsgDlg(pszTitle, szContent, NO, 30, &nKey);	
	if(KEY_ENTER == nKey)
	{
		return APP_SUCC;
	}
	else
	{
	  	return APP_FAIL;
	}
 
}



/**
* @brief 在屏幕上 逐笔显示流水的内容
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int WaterOneByOne(void)
{
	int nRet, nWaterRecNum;
	STWATER stWater;

	GetWaterNum(&nWaterRecNum);
	if (nWaterRecNum==0) 
	{
		char szDispBuf[100];
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C无交易");
		PubMsgDlg("浏览纪录", szDispBuf, 0, 5);
		return APP_SUCC;
	}
	
	ASSERT(FetchLastWater(&stWater));
	while(1)
	{
		nRet = DispWater(&stWater);
		switch(nRet)
		{
		case KEY_LEFT:
		case KEY_UP_720:
		case KEY_LEFT_720:
		case KEY_ENTER:
			ASSERT(FetchPreviousWater(&stWater));
			break;
		case KEY_RIGHT:
		case KEY_DOWN_720:
		case KEY_RIGHT_720:
			ASSERT(FetchNextWater(&stWater));
			break;
		case 0:
		case KEY_ESC:
			return APP_QUIT;
		default:
			break;
		}
	}
	return APP_SUCC;
}

/**
* @brief 流水记录结构转化成系统数据结构
* @param in STWATER *pstWater存放记录内容的结构指针
* @param out  STSYSTEM *pstSystem 转换后的系统数据结构
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int WaterToSys(const STWATER *pstWater, STSYSTEM *pstSystem)
{
	pstSystem->cTransType = pstWater->cTransType;							/**<交易类型*/
	pstSystem->cTransAttr = pstWater->cTransAttr;								/**<交易属性*/
	pstSystem->cEMV_Status = pstWater->cEMV_Status;							/**<EMV交易的执行状态*/
	PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)pstSystem->szPan);		/**<2 主帐号 */
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)pstSystem->szAmount);				/**<4 金额      */
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)pstSystem->szTrace);					/**<POS流水号*/
	PubHexToAsc((uchar *)pstWater->sTime, 6, 0, (uchar *)pstSystem->szTime);					/**<12域；交易时间hhmmss */
	PubHexToAsc((uchar *)pstWater->sDate, 8, 0, (uchar *)pstSystem->szDate);					/**<13域；交易日期yyyymmdd、yymmdd、mmdd */
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2) == 0)
	{
		;
	}
	else
	{
		PubHexToAsc((uchar *)pstWater->sExpDate, 4, 0, (uchar *)pstSystem->szExpDate);				/**<14 卡有效期(同原请求交易)*/
	}
	strcpy(pstSystem->szInputMode, pstWater->szInputMode);						/**<22 输入模式*/
	strcpy(pstSystem->szRefnum, pstWater->szRefnum);							/**<37 系统参考号*/
	PubHexToAsc((uchar *)pstWater->sTrack2, pstWater->nTrack2Len, 0, (uchar *)pstSystem->szTrack2);/**<35 二磁道数据(BCD)*/
	PubHexToAsc((uchar *)pstWater->sTrack3, pstWater->nTrack3Len, 0, (uchar *)pstSystem->szTrack3);/**<36 三磁道数据(BCD)*/
	strcpy(pstSystem->szResponse, pstWater->szResponse);						/**<39 响应码*/
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0, (uchar *)pstSystem->szBatchNo);	/**<批次号*/
	PubHexToAsc((uchar *)pstWater->sOldBatch, 6, 0, (uchar *)pstSystem->szOldBatchNo);	/**<原交易批次号*/
	PubHexToAsc((uchar *)pstWater->sOldTrace, 6, 0, (uchar *)pstSystem->szOldTraceNo);	/**<原流水号   */
	strcpy(pstSystem->szOldRefnum, pstWater->szOldRefnum);					/**<原交易参考号*/
	strcpy(pstSystem->szOperNow, pstWater->szOper);							/**<操作员号                  */
	memcpy(pstSystem->szTelNo, pstWater->sTelNo, 11);
	memcpy(pstSystem->szCouponID, pstWater->sCouponID, 20);
	/**<int nAdditionLen;和char sAddition[100];由具体交易赋值给pstSystem*/
	
	return APP_SUCC;
}

/**
* @brief 系统数据结构转化成流水记录结构
* @param in  STSYSTEM *pstSystem 转换后的系统数据结构
* @param out STWATER *pstWater存放记录内容的结构指针
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int SysToWater(const STSYSTEM *pstSystem, STWATER *pstWater )
{
	pstWater->cTransType = pstSystem->cTransType;							/**<交易类型*/
	pstWater->cTransAttr = pstSystem->cTransAttr;								/**<交易属性*/
	pstWater->cEMV_Status = pstSystem->cEMV_Status;							/**<EMV交易的执行状态*/
	memcpy(pstWater->sTransCode, pstSystem->szTransCode, 6);
	pstWater->nPanLen = strlen(pstSystem->szPan);								/**<2 主帐号长度*/
	PubAscToHex((uchar *)pstSystem->szPan, pstWater->nPanLen, 0, (uchar *)pstWater->sPan);	/**<2 主帐号(BCD)*/
	PubAscToHex((uchar *)pstSystem->szAmount, 12, 0, (uchar *)pstWater->szAmount);				/**<4 金额(BCD)*/
	PubAscToHex((uchar *)pstSystem->szTrace, 6, 0, (uchar *)pstWater->sTrace);					/**<11 POS流水号(BCD)    */
	PubAscToHex((uchar *)pstSystem->szTime, 6, 0, (uchar *)pstWater->sTime);					/**<12 域；交易时间hhmmss(BCD)  */
	PubAscToHex((uchar *)pstSystem->szDate, 8, 0, (uchar *)pstWater->sDate);					/**<13 域；交易日期yyyymmdd、yymmdd、mmdd  (BCD)  */
	PubAscToHex((uchar *)pstSystem->szExpDate, 4, 0, (uchar *)pstWater->sExpDate);				/**<14 卡有效期(同原请求交易)*/
	strcpy(pstWater->szInputMode, pstSystem->szInputMode );					/**<22 输入模式*/
	pstWater->nTrack2Len = strlen(pstSystem->szTrack2);		/**<35 二磁道数据长度*/
	PubAscToHex((uchar *)pstSystem->szTrack2, pstWater->nTrack2Len, 0, (uchar *)pstWater->sTrack2);/**<35 二磁道数据(BCD)*/
	pstWater->nTrack3Len = strlen(pstSystem->szTrack3);		/**<36 三磁道数据长度*/
	PubAscToHex((uchar *)pstSystem->szTrack3, pstWater->nTrack3Len, 0, (uchar *)pstWater->sTrack3);/**<36 三磁道数据(BCD)*/
	strcpy(pstWater->szRefnum, pstSystem->szRefnum);							/**<37 系统参考号*/
	strcpy(pstWater->szResponse, pstSystem->szResponse);						/**<39 响应码*/
	PubAscToHex((uchar *)pstSystem->szBatchNo, 6, 0, (uchar *)pstWater->szBatchNum);	/**<批次号*/
	PubAscToHex((uchar *)pstSystem->szOldBatchNo, 6, 0, (uchar *)pstWater->sOldBatch );	/**<原交易批次号*/
	PubAscToHex((uchar *)pstSystem->szOldTraceNo, 6, 0, (uchar *)pstWater->sOldTrace);	/**<原流水号(BCD)    */
	strcpy(pstWater->szOldRefnum, pstSystem->szOldRefnum);					/**<原交易参考号*/
	pstWater->cOldTransType = pstSystem->cOldTransType;
	pstWater->cStatus = 0;											/**<正常0, 已取消1,已调整2,已退货3;  */
	strcpy(pstWater->szOper, pstSystem->szOperNow);							/**<操作员号                  */
	strcpy(pstWater->sTelNo, pstSystem->szTelNo);	/**< 手机号码*/
	strcpy(pstWater->sCouponID, pstSystem->szCouponID);			/**<串码*/
	PubAscToHex((uchar *)pstSystem->szRealAmount, 12, 0, (uchar *)pstWater->szRealAmount);				/**<抵用金额(BCD)*/
	strcpy(pstWater->szOutTradeNo, pstSystem->szOutTradeNo);		/**<商户订单号*/
	PubAscToHex((uchar *)pstSystem->szPayableAmount, 12, 0, (uchar *)pstWater->sPayableAmount);				/**<应付金额(BCD)*/
	pstWater->cSendFlag = 0;					/**<上送标志0－未上送，0xFD－已上送,0xFF-上送失败*/
	pstWater->cBatchUpFlag= 0;					/**<上送标志0－未上送，0xFD－已上送,0xFF-上送失败*/
	strcpy(pstWater->szAdvertisement, pstSystem->szAdvertisement);//广告信息

	/**<int nAdditionLen;和char sAddition[100];由具体交易赋值给pstWater*/

	return APP_SUCC;
}


/**
* @brief 获得流水记录的总记录数
* @param out int *pnWaterNum存放获得的记录数
* @return 无
*/
void GetWaterNum( int *pnWaterNum)
{
	PubGetRecSum(FILE_WATER, pnWaterNum);
}

/**
* @brief 获得流水首记录的操作员号码
* @param out psOper操作员号码
* @return 
* @li APP_SUCC		
* @li APP_FAIL
*/
int GetWaterOper(char *psOper)
{
	STWATER stWater;

	memset(&stWater, 0, sizeof(STWATER));
	if (APP_SUCC == FetchFirstWater(&stWater))
	{
		memcpy(psOper, stWater.szOper, 2);
		return APP_SUCC;
	}
	else
	{
		return APP_FAIL;
	}
}

/**
* @brief 判断指定的操作员是否有操作流水存在
* @param in psOper操作员号码
* @return 
* @li YES		
* @li NO
*/
int IsOperHasWater( const char *psOper)
{
	STWATER stWater;
	int nRet;
	int nFileHandle;
	int nRecNo;
	
	memset(&stWater, 0, sizeof(STWATER));
	ASSERT_FAIL(PubOpenFile(FILE_WATER, "r", &nFileHandle));
	for(nRecNo=1;;nRecNo++)
	{
		nRet = PubReadRec(nFileHandle, nRecNo, (char *)&stWater);
		if (nRet != APP_SUCC)
		{
			ASSERT(PubCloseFile(&nFileHandle));
			return APP_FAIL;
		}
		if ( memcmp(stWater.szOper, psOper, 2 ) == 0 )
		{
			ASSERT(PubCloseFile(&nFileHandle));
			return APP_SUCC;
		}		
	}
}

int GetUnSendNum(int *pnUnSendNum)
{
	STWATER stWater;
	int nRet = 0;
	int nWaterSum;
	int nLoop;
	int nFileHandle;
	char nMaxReSend;
	int nUnSendSum = 0;


	GetWaterNum(&nWaterSum);
	if (nWaterSum > 0)
	{
		nRet = PubOpenFile(FILE_WATER, "r", &nFileHandle);
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{
			return APP_FAIL;
		}
	}
	else
	{
		return APP_FAIL;
	}
	GetVarCommReSendNum(&nMaxReSend);
	for (nLoop = 1; nLoop <= nWaterSum; nLoop++)
	{
		memset(&stWater, 0, sizeof(STWATER));
		if((nRet = PubReadRec(nFileHandle, nLoop, (char *)&stWater))!= APP_SUCC)
		{
			PubCloseFile(&nFileHandle);
			return nRet;
		}
		switch (stWater.cTransType)
		{
		case TRANS_ADJUST:
		case TRANS_OFFLINE:
			if (stWater.cSendFlag > nMaxReSend)
			{
				continue;
			}
			else
				nUnSendSum++;
			break;
		default:
			continue;
		}
	}
	*pnUnSendNum = nUnSendSum;
	PubCloseFile(&nFileHandle);
	return APP_SUCC;
}
int GetFailSendNum(char cFlag)
{
	STWATER stWater;
	int nWaterSum;
	int i;
	int nFileHandle;
	int nFailSendSum = 0;
	GetWaterNum(&nWaterSum);
	ASSERT_FAIL(PubOpenFile(FILE_WATER, "r", &nFileHandle));
	for(i=1; i<= nWaterSum;i++)
	{
		PubReadRec(nFileHandle, i, (char *)&stWater);
		switch(stWater.cTransType)
		{
		case TRANS_ADJUST:
		case TRANS_OFFLINE:
			if(stWater.cSendFlag== cFlag)
				break;
			else
				continue;
		default:
			continue;
		}
		nFailSendSum++;
	}
	PubCloseFile(&nFileHandle);
	return nFailSendSum;
}
