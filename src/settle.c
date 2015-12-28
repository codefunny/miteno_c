/**
* @file settle.c
* @brief 结算管理处理模块
* @version  1.0
* @author 张捷薛震
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "magtxn.h"

#define MAX_AMOUNT "20000000000"/**<POS C 处理的最大金额,2013-5-28,加了个0,lld*/
#define ONE_TRADE_MAX_AMOUNT "9999999999"/**<单笔交易不能超过这个值*/

extern int PackOffline(STSYSTEM *, const STWATER *);
extern int PackAdjust(STSYSTEM *, const STWATER *);
extern int PackAuthSaleOff(STSYSTEM *, STWATER *);


/**
* 定义全局变量
*/
static STSETTLE gstSettle_NK, gstSettle_WK;

/**
* 内部函数声明
*/
static int DispSettleDataItem(char *, SETTLE_NUM , uchar *);
static int ChangeSettleItem(const char *, SETTLE_NUM *, SETTLE_AMT *, const char);
#if 0
static int BatchMagOffline(const int, const int, int *);
static int BatchMagTxn(const int, const int, int *);
static int BatchInform(const int, const int, int *);
static int BatchUpEnd(const int);
static int DoBatchUp(void);
#endif

/**
* @brief pos结算
* @param in cFlag 结算标识0正常结算    非0结算中断后的再次结算
* @return 
* @li APP_SUCC
* @li APP_FAIL
* @li APP_QUIT
*/
int Settle(char cFlag)
{	

	char *pszTitle="结算";
	STSYSTEM stSystem;
	STAMT_NUM_SETTLE stAmtNumSettle;
	STAMT_NUM_INFO_SETTLE stAmtNumInfo;

	int nRet = 0;
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen = 0;
	int nFieldLen = 0;
	char szSettleDateTime[5+1];
	char cProcStep = 0;
	int nTagSum=0;

	memset(&stSystem, 0, sizeof(STSYSTEM));

	/*
	* 检查POS限制，电量是否充足
	*/
	ASSERT_QUIT(DealPrintLimit()); 
	
	if (cFlag)
	{
		/*
		* 取原来结算中断的步骤
		*/
		if (YES == GetVarBatchHaltFlag())
		{
			cProcStep = 2;
		}
		else if (YES == GetVarPrintSettleHalt())
		{
			cProcStep = 3;
		}
		else if (YES == GetVarPrintDetialHalt())
		{
			cProcStep = 4;
		}
		else if (YES == GetVarClrSettleDataFlag())
		{
			cProcStep = 5;
		}
	}
	else
	{
		cProcStep = 0;
		/*
		* 将批上送中断标识的数置为1，从第一笔开始送
		*/
		SetVarBatchMagOfflineHaltFlag(1);
		SetVarFinanceHaltFlag(1);
		SetVarMessageHaltFlag(1);		
		SetVarEmvOfflineUpNum(1);
		SetVarEmvOnlineUpNum(1);
		SetVarEMVOfflineFailHaltFlag(1);
		SetVarEMVOnlineARPCErrHaltFlag(1);
		/*
		* 将批上送的笔数置为0，重新统计
		*/
		SetVarBatchSum(0);
	}	

	/**
	* 联机结算
	*/
	if (cProcStep <= 1)
	{
		/**
		* 检查是否签到
		*/
		ASSERT_QUIT(ChkLoginStatus());

		/**
		* 执行冲正
		*/
		ASSERT_HANGUP_QUIT(DoReversal());
		PubDisplayTitle(pszTitle);		
		/**
		* 再次判断拨号
		*/
		ASSERT_HANGUP_QUIT(CommConnect());
	
		/**
		* 采集数据
		*/
		DealSystem(&stSystem);
		CustodyTlvInit();

		DealSettle(&stAmtNumInfo);
		memset((char *)&stAmtNumSettle, 0, sizeof(stAmtNumSettle));
		PubHexToAsc(stAmtNumInfo.nDebitAmount_N.sSettleAmt, 12, 0,(uchar*)stAmtNumSettle.sDebitAmt_N);
		sprintf(stAmtNumSettle.sDebitNum_N, "%03ld", stAmtNumInfo.nDebitNum_N);		
		PubHexToAsc(stAmtNumInfo.nCreditAmount_N.sSettleAmt, 12, 0, (uchar*)stAmtNumSettle.sCreditAmt_N);
		sprintf(stAmtNumSettle.sCreditNum_N, "%03ld", stAmtNumInfo.nCreditNum_N);

#if 0
		stAmtNumSettle.cSettleCode_N = '0';
		PubHexToAsc(stAmtNumInfo.nDebitAmount_W.sSettleAmt, 12, 0, (uchar*)stAmtNumSettle.sDebitAmt_W);
		sprintf(stAmtNumSettle.sDebitNum_W, "%03ld", stAmtNumInfo.nDebitNum_W);	
		PubHexToAsc(stAmtNumInfo.nCreditAmount_W.sSettleAmt, 12, 0, (uchar*)stAmtNumSettle.sCreditAmt_W);
		sprintf(stAmtNumSettle.sCreditNum_W, "%03ld", stAmtNumInfo.nCreditNum_W);
		stAmtNumSettle.cSettleCode_W= '0';
#endif

		memcpy(stSystem.szTransCode, "200001", 6);
		memset(sPackBuf, 0, sizeof(sPackBuf));
		nPackLen =0;
		ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(5, stSystem.szBatchNo, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(7,  stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValue(8,  stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
		ASSERT_FAIL(CustodyTlvAddValueLen(34, (char *)&stAmtNumSettle,36, sPackBuf, &nPackLen, &nTagSum));
		

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
	
		ASSERT_FAIL(CustodyGetField(34,  (char *)&stAmtNumSettle.cSettleCode_N, &nFieldLen));
		if(stAmtNumSettle.cSettleCode_N == '1' || stAmtNumSettle.cSettleCode_N == '2' || stAmtNumSettle.cSettleCode_N == '3')
		{
			SetVarCnCardFlag(stAmtNumSettle.cSettleCode_N);/*<内卡应答码*/
		}
		else
		{
			PubMsgDlg("结算", "内卡\n对帐应答代码错误", 3, 10);
			CommHangUp();
			return APP_FAIL;
		}
		/**
		* 保存结算日期时间
		*/
		PubAscToHex((uchar *)stSystem.szDate+4, 4, 0, (uchar *)szSettleDateTime);
		PubAscToHex((uchar *)stSystem.szTime, 6, 0, (uchar *)szSettleDateTime + 2);
		SetVarSettleDateTime(szSettleDateTime);
	}

	/**
	* 批上送
	*/
	if (cProcStep <= 2)
	{
		SetVarBatchHaltFlag(YES);
		/*银商要求除了通讯\打包问题，
		批上送应答只显示但是不失败,
		都强制成功*/
		
		//现在没有需要离线上送的交易
		//ASSERT_HANGUP_FAIL(DoBatchUp());
		SetVarBatchHaltFlag(NO);
	}

	CommHangUp();

	/**
	* 打印结算单
	*/
	if (cProcStep <= 3)
	{
		SetVarPrintSettleHalt(YES);
		PubClearAll();
		PubDisplayTitle("正在打印结算单");
		PubDisplayGen(3, "请稍候");
		PubUpdateWindow();
		nRet = PrintSettle(FIRSTPRINT);
		if (nRet == APP_FAIL)
		{
			return APP_FAIL;
		}
 		SetVarPrintSettleHalt(NO);
 	}
	
	/**
	* 打印明细单
	*/
	if (cProcStep <= 4)
	{
		SetVarPrintDetialHalt(YES);
		if (YES == GetVarIsPrintWaterRec())
		{
			int nSelect = 0;
			
			nRet = ProSelectList("0.否||1.是", "是否打印明细", 0xFF, &nSelect);
			if (nRet == APP_SUCC && nSelect == 1)
			{
				PubClear2To4();
				PubDisplayGen(3, "打印明细单");
				PubUpdateWindow();
				nRet = PrintAllWater();
				if (nRet == APP_FAIL)
				{
					return APP_FAIL;
				}
			}
		}

		SetVarPrintDetialHalt(YES);

		#if 0
		if(GetFailSendNum(0xFF) != 0)
		{
			PubClearAll();
			PubDisplayGen(3, "打印上送不成功的");
			PubDisplayGen(4, "交易明细");
			PubUpdateWindow();
	  		nRet = PrintFailWater(0xFF);
			if (nRet != APP_SUCC)
			{
				return APP_FAIL;
			}
		}
		if(GetFailSendNum(0xFE) != 0)
		{
			PubClearAll();
			PubDisplayGen(3, "打印上送被拒的");
			PubDisplayGen(4, "交易明细");
			PubUpdateWindow();
			nRet = PrintFailWater(0xFE);
			if (nRet != APP_SUCC)
			{
				return APP_FAIL;
			}
		}
		#endif
		SetVarPrintDetialHalt(NO);
	}

	/*
	* 清除结算数据流水等
	*/
	if (cProcStep <= 5)
	{
		SetVarClrSettleDataFlag(YES);
		PubClearAll();
		PubDisplayTitle(pszTitle);
		PubDisplayGen(2, "正在处理结算");
		PubUpdateWindow();
		nRet = InitWaterFile();/**<清除流水*/
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{
			InitWaterFile();/**<清除流水*/
		}
		ClearSettle();/**<清除结算数据*/
		IncVarBatchNo();/**<批次号加一*/
		SetVarClrSettleDataFlag(NO);
		SetVarOfflineUnSendNum(0);
	}
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 2, "结 算 成 功");
	PubUpdateWindow();
	return APP_SUCC;
}

/**
* @brief 获得内卡的结算数据
*		供本文件以外的程序模块调用
* @param in pstSettle	存放结算数据的结构
* @return 
* @li APP_SUCC
*/
int GetSettleDataNK(STSETTLE *pstSettle)
{
	memcpy((void *)pstSettle , (void *)&gstSettle_NK, sizeof(STSETTLE));
	return APP_SUCC;
}

/**
* @brief 获得外卡的结算数据
*		供本文件以外的程序模块调用
* @param in pstSettle	存放结算数据的结构
* @return 
* @li APP_SUCC
*/
int GetSettleDataWK(STSETTLE *pstSettle)
{
	memcpy((void *)pstSettle, (void *)&gstSettle_WK, sizeof(STSETTLE));
	return APP_SUCC;
}

/**
* @brief 显示结算数据的某一项
*		供DispSettleData函数调用
* @param in pszTransName	交易类型名称
* @param in nTransNum	交易数量
* @param in nTransAmt		交易金额
* @return 
* @li APP_SUCC
*/
static int DispSettleDataItem(char *pszTransName, SETTLE_NUM nTransNum, uchar *pszAmt)
{
	int nRet;
	char szDispBuf[100] = {0};
	char szDispAmt[13+1] = {0};
	
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, pszTransName);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf+strlen(szDispBuf), "\n笔数:|R%12d", nTransNum);	
	
	memset(szDispAmt, 0, sizeof(szDispAmt));
	ProAmtToDispOrPnt((char *)pszAmt, szDispAmt);
	PubAllTrim(szDispAmt);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf+strlen(szDispBuf),"\n金额:|R%s元", szDispAmt);		
	nRet = PubUpDownMsgDlg("查询交易汇总", szDispBuf, YES, 60, NULL);
	if (nRet==APP_QUIT)
	{
		return APP_QUIT;
	}
	else
	{
		return APP_SUCC;
	}

}

/**
* @brief 计算内卡外卡借记贷记总金额
* @param in STAMT_NUM_INFO_SETTLE *pstInfoSettle 存储借贷记总金额的结构
* @return 
* @li APP_SUCC
*/
int DealSettle(STAMT_NUM_INFO_SETTLE *pstInfoSettle)
{
	uchar szAmt1[13] = {0};
	uchar szAmt2[13] = {0};
	uchar szAmt[13] = {0};

	pstInfoSettle->nDebitNum_N = gstSettle_NK._SaleNum + gstSettle_NK._AuthSaleNum
		+ gstSettle_NK._CouponVerifyNum + gstSettle_NK._TelVerifyNum + gstSettle_NK._CardVerifyNum;
	
	memset(szAmt1, 0, sizeof(szAmt1));
	memset(szAmt2, 0, sizeof(szAmt2));
	memset(szAmt, 0, sizeof(szAmt));
	PubHexToAsc(gstSettle_NK._SaleAmount.sSettleAmt, 12, 0, szAmt1);
	PubHexToAsc(gstSettle_NK._AuthSaleAmount.sSettleAmt, 12, 0, szAmt2);
	AmtAddAmt(szAmt1, szAmt2, szAmt);
	
	strcpy((char*)szAmt1, (char*)szAmt);
	memset(szAmt2, 0, sizeof(szAmt2));
	memset(szAmt, 0, sizeof(szAmt));
	PubHexToAsc(gstSettle_NK._CouponVerifyAmount.sSettleAmt, 12, 0, szAmt2);
	AmtAddAmt(szAmt1, szAmt2, szAmt);

	strcpy((char*)szAmt1, (char*)szAmt);
	memset(szAmt2, 0, sizeof(szAmt2));
	memset(szAmt, 0, sizeof(szAmt));
	PubHexToAsc(gstSettle_NK._TelVerifyAmount.sSettleAmt, 12, 0, szAmt2);
	AmtAddAmt(szAmt1, szAmt2, szAmt);
	
	strcpy((char*)szAmt1, (char*)szAmt);
	memset(szAmt2, 0, sizeof(szAmt2));
	memset(szAmt, 0, sizeof(szAmt));
	PubHexToAsc(gstSettle_NK._CardVerifyAmount.sSettleAmt, 12, 0, szAmt2);
	AmtAddAmt(szAmt1, szAmt2, szAmt);
	PubAscToHex(szAmt, 12, 0, pstInfoSettle->nDebitAmount_N.sSettleAmt);
	
	return APP_SUCC;
}

/**
* @brief 显示内卡外卡借记贷记总金额
* @param in 无
* @return 
* @li APP_SUCC
*/
int DispTotal()
{
	SETTLE_NUM nNum ;
	uchar szAmt1[13] = {0};
	uchar szAmt2[13] = {0};
	uchar szAmtTmp[13] = {0};
	uchar szAmt[13] = {0};
	STSETTLE stSettle;

	GetSettleDataNK(&stSettle); 			/**< 内卡结算数据*/

	nNum = stSettle._CouponVerifyNum - stSettle._VoidCouponVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && nNum > 0)
	{
		/*串码验券*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CouponVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidCouponVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("串码验券总计:", nNum, szAmt));
	}

	nNum = stSettle._TelVerifyNum - stSettle._VoidTelVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && nNum > 0)
	{
		/*手机号验券*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._TelVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidTelVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("手机号验券总计:", nNum, szAmt));
	}

	nNum = stSettle._CardVerifyNum - stSettle._VoidCardVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && nNum > 0)
	{
		/*卡号验券*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CardVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._TelVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		ASSERT_QUIT(DispSettleDataItem("银行卡验券总计:", nNum, szAmt));
	}

	//退货
	nNum = stSettle._RefundVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && nNum > 0) 
	{
		//退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundVerifyAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("退货总计:", nNum, szAmt));
	}

	nNum = stSettle._PrecreateNum - stSettle._VoidPrecreateNum + stSettle._CreatepayNum - stSettle._VoidCreatepayNum;
	if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		&& (nNum > 0))
	{
		/*微信扫码支付*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._PrecreateAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidPrecreateAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//条码支付
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("微信支付总计:", nNum, szAmt1));
	}

	nNum = stSettle._RefundWechatNum;
	if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && nNum > 0)
	{
		//微信退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundWeChatAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("微信退货总计:", nNum, szAmt));
	}

	nNum = stSettle._BaiduCreatepayNum - stSettle._VoidBaiduCreatepayNum + stSettle._BaiduPrecreteNum - stSettle._VoidBaiduPrecreteNum;
	if((GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES) && nNum > 0)
	{
		/*百度被扫*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._BaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//百度主扫
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("百度支付总计:", nNum, szAmt1));
	}

	nNum = stSettle._RefundBaiduNum;
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && nNum > 0)
	{
		//百度退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundBaiduAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("百度退货总计:", nNum, szAmt));
	}

	nNum = stSettle._JDPrecreateNum - stSettle._VoidJDPrecreateNum;
	if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES ) && nNum > 0)
	{
		/*京东支付*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._JDPrecreateAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidJDPrecreateAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("京东支付总计:", nNum, szAmt));
	}

	nNum = stSettle._JDRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && nNum > 0)
	{
		//京东退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._JDRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("京东退货总计:", nNum, szAmt));
	}

	nNum = stSettle._AliCreatepayNum - stSettle._VoidAliCreatepayNum + stSettle._AliPrecreteNum - stSettle._VoidAliPrecreteNum;
	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES && nNum > 0)
	{
		/*支付宝被扫*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._AliPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidAliPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//支付宝主扫
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._AliCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidAliCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("支付宝支付总计:", nNum, szAmt1));
	}
	
	nNum = stSettle._AliRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && nNum > 0)
	{
		//支付宝退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._AliRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("支付宝退货总计:", nNum, szAmt));
	}

	nNum = stSettle._MeituanNum;
	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && nNum > 0)
	{
		//美团兑券
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._MeituanAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("美团兑券总计:", nNum, szAmt));
	}
	nNum = stSettle._PanjintongNum;
	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && nNum > 0)
	{
		//盘锦通
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._PanjintongAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("盘锦通支付总计:", nNum, szAmt));
	}

	nNum = stSettle._BestpayCreatepayNum - stSettle._VoidBestpayCreatepayNum;
	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY) == YES && nNum > 0)
	{
		/*翼支付*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("翼支付支付总计:", nNum, szAmt));
	}

	nNum = stSettle._BestpayRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES && nNum > 0)
	{
		//翼支付退货
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BestpayRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("翼支付退货总计:", nNum, szAmt));
	}

	return APP_SUCC;
}

/**
* @brief 查结算数据,重打印结算汇总数据( 包括本外币)
* @param in 无
* @return 无
*/
int FindSettle()
{
	int nSelect;
	
	while(1)
	{
		ASSERT_QUIT( PubSelectListItem("1.查阅||2.打印", "查汇总", NULL, 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			DispTotal();
			break;
		case 2:
			if(APP_SUCC==PubConfirmDlg("交易总额", "按确认键打印", 0, 0 )) 
			{
				PubClearAll();
				DISP_PRINTING_NOW;
				PrintTotal();
			}
			break;				
		default:
			break;
		}
	}
}

/**
* @brief 清除结算数据
*	清除内卡、外卡交易笔数，流水总笔数，通知类，金融类，断点数
* @param in 无
* @return 无
*/
void ClearSettle(void)
{
	memset((void *)(&gstSettle_NK), 0, sizeof(STSETTLE));
	memset((void *)(&gstSettle_WK), 0, sizeof(STSETTLE));
	
	// 掉电保护
	SetVarSettleData(gstSettle_NK, gstSettle_WK);

	SetVarWaterSum(0);
	SetVarHaveReSendNum(0);
	SetVarPrintSettleHalt(NO);
	SetVarPrintDetialHalt(NO);
	SetVarPrintWaterHaltFlag(NO);
	SetVarCnCardFlag('1');
	SetVarEnCardFlag('1');
	SetVarBatchHaltFlag(NO);
	SetVarClrSettleDataFlag(NO);
	SetVarFinanceHaltFlag(1);
	SetVarMessageHaltFlag(1);
	SetVarEmvOfflineUpNum(1);
	SetVarEmvOnlineUpNum(1);
	SetVarEMVOfflineFailHaltFlag(1);
	SetVarEMVOnlineARPCErrHaltFlag(1);
		
	SetVarBatchSum(0);
	SetVarSettleDateTime("\x00\x00\x00\x00\x00");
}



/**
* @brief 更新结算的数据项目(供ChangeSettle函数调用)
*		函数调用时pnTransNum pnTransAmt   必需为stSettle_NK 或者stSettle_WK结构中的成员
* @param in pszAmount	 需要增加的金额
* @param out pnTransNum	 交易总笔数
* @param out pszAmount	 交易总金额
* @return 
* @li APP_SUCC
* @li APP_FAIL
*/
static int ChangeSettleItem(const char *pszAmount, SETTLE_NUM *pnTransNum, SETTLE_AMT *pstTransAmt, const char cAddFalg)
{
	int nSizeNk, nSizeWk;
	int nOffset, nOffset_Nk, nOffset_Wk;
	uchar szAmt1[12+1] = {0};
	uchar szAmt[12+1] = {0};
	uchar szMaxAmt[12+1] = {0};

	nOffset = (char *)pstTransAmt - (char *)pnTransNum;

	if (nOffset != sizeof(SETTLE_NUM))
	{
		return APP_FAIL;
	}

	nSizeNk = sizeof(STSETTLE);
	nSizeWk = sizeof(STSETTLE);

	nOffset_Nk = (char *)pnTransNum - (char *)(&gstSettle_NK);
	nOffset_Wk = (char *)pnTransNum - (char *)(&gstSettle_WK);

	if (nOffset_Nk >=0 && nOffset_Nk <= nSizeNk - sizeof(SETTLE_AMT))
	{
		nOffset = nOffset_Nk;
	}
	else if (nOffset_Wk >=0 && nOffset_Wk <= nSizeWk - sizeof(SETTLE_AMT))
	{
		nOffset = nSizeNk + nOffset_Wk;
	}
	else
	{
		return APP_FAIL;
	}
	
	(*pnTransNum)++;

	memset(szAmt1, 0, sizeof(szAmt1));
	memset(szAmt, 0, sizeof(szAmt));
	PubHexToAsc(pstTransAmt->sSettleAmt, 12, 0, szAmt1);
	if (YES == cAddFalg)
		AmtAddAmt(szAmt1, (uchar *)pszAmount, szAmt);
	else
		AmtSubAmt(szAmt1, (uchar *)pszAmount, szAmt);
	PubAscToHex(szAmt, 12, 0, pstTransAmt->sSettleAmt);

	SaveSettleDataItem( (char *)pnTransNum, nOffset, sizeof(SETTLE_NUM) );
	SaveSettleDataItem( (char *)pstTransAmt, nOffset +sizeof(SETTLE_NUM) , sizeof(SETTLE_AMT) );

	PubAddSymbolToStr((char *)szAmt, 12, '0', 0);
	strcpy((char *)szMaxAmt, MAX_AMOUNT);
	PubAddSymbolToStr((char *)szMaxAmt, 12, '0', 0);
	if(memcmp(szAmt, szMaxAmt, 12) > 0)/* 系统处理的上限*/
	{
		PubMsgDlg("交易总金额过多!", "请尽快进行结算\n按任意键继续", 3, 10);
	}
	return APP_SUCC;
}
/**
* @brief 记录结算数据
* @param in pszAmount	 需要增加的金额
* @param in cTransType	 交易类型
* @param in pszInterOrg	 国际组织代码
* @return 
* @li APP_SUCC
*/
int ChangeSettle(const char *pszAmount, const char cTransType, const char *pszInterOrg)
{
	STSETTLE *pstSettle;
	if (memcmp(pszInterOrg,"CUP",3)==0)		/**< 内卡*/
	{
		pstSettle = &gstSettle_NK; 
	}
	else
	{
		pstSettle = &gstSettle_WK; 
	}

	switch(cTransType)
	{
	case TRANS_SALE:
		ChangeSettleItem(pszAmount, &(pstSettle->_SaleNum), &(pstSettle->_SaleAmount), YES);
		break;
	case TRANS_VOID_SALE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidSaleNum), &(pstSettle->_VoidSaleAmount), YES);
		break;		
	case TRANS_AUTHSALE:
		ChangeSettleItem(pszAmount, &(pstSettle->_AuthSaleNum), &(pstSettle->_AuthSaleAmount), YES);
		break;		
	case TRANS_VOID_AUTHSALE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidAuthSaleNum), &(pstSettle->_VoidAuthSaleAmount), YES);
        break;
	case TRANS_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_RefundNum), &(pstSettle->_RefundAmount), YES);
        break;        	
	case TRANS_COUPON_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_CouponVerifyNum), &(pstSettle->_CouponVerifyAmount), YES);
		break;
	case TRANS_TEL_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_TelVerifyNum), &(pstSettle->_TelVerifyAmount), YES);
		break;
	case TRANS_CARD_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_CardVerifyNum), &(pstSettle->_CardVerifyAmount), YES);
		break;
	case TRANS_VOID_COUPON_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidCouponVerifyNum), &(pstSettle->_VoidCouponVerifyAmount), YES);
		break;
	case TRANS_VOID_TEL_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidTelVerifyNum), &(pstSettle->_VoidTelVerifyAmount), YES);
		break;
	case TRANS_VOID_CARD_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidCardVerifyNum), &(pstSettle->_VodiCardVerifyAmount), YES);
		break;
	case TRANS_REFUND_VERIFY:
		ChangeSettleItem(pszAmount, &(pstSettle->_RefundVerifyNum), &(pstSettle->_RefundVerifyAmount), YES);
		break;
	//微信	
	case TRANS_WX_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_RefundWechatNum), &(pstSettle->_RefundWeChatAmount), YES);
		break;
	case TRANS_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_PrecreateNum), &(pstSettle->_PrecreateAmount), YES);
		break;
	case TRANS_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_CreatepayNum), &(pstSettle->_CreatepayAmount), YES);
		break;
	case TRANS_VOID_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidPrecreateNum), &(pstSettle->_VoidPrecreateAmount), YES);
		break;
	case TRANS_VOID_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidCreatepayNum), &(pstSettle->_VoidCreatepayAmount), YES);
		break;
	//百度	
	case TRANS_CREATEANDPAYBAIDU:
		ChangeSettleItem(pszAmount, &(pstSettle->_BaiduCreatepayNum), &(pstSettle->_BaiduCreatepayAmount), YES);
		break;
	case TRANS_BAIDU_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_RefundBaiduNum), &(pstSettle->_RefundBaiduAmount), YES);
		break;
	case TRANS_VOID_CREATEANDPAYBAIDU:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidBaiduCreatepayNum), &(pstSettle->_VoidBaiduCreatepayAmount), YES);
		break;
	//增加百度被扫/被扫撤销  wanglez add 20151221
	case TRANS_BAIDU_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_BaiduPrecreteNum), &(pstSettle->_BaiduPrecreteAmount), YES);
		break;
	case TRANS_VOID_BAIDU_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidBaiduPrecreteNum), &(pstSettle->_VoidBaiduPrecreteAmount), YES);
		break;
		
	//京东
	case TRANS_JD_PRECREATE:
	case TRANS_JD_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_JDPrecreateNum), &(pstSettle->_JDPrecreateAmount), YES);
		break;
	case TRANS_JD_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_JDRefundNum), &(pstSettle->_JDRefundAmount), YES);
		break;
	case TRANS_VOID_JD_PRECREATE:
	case TRANS_VOID_JD_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidJDPrecreateNum), &(pstSettle->_VoidJDPrecreateAmount), YES);
		break;
	//支付宝
	case TRANS_ALI_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliCreatepayNum), &(pstSettle->_AliCreatepayAmount), YES);
		break;
	case TRANS_ALI_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliRefundNum), &(pstSettle->_AliRefundAmount), YES);
		break;
	case TRANS_VOID_ALI_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidAliCreatepayNum), &(pstSettle->_VoidAliCreatepayAmount), YES);
		break;
	//增加支付宝被扫/被扫撤销  wanglez add 20151221
	case TRANS_ALI_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliPrecreteNum), &(pstSettle->_AliPrecreteAmount), YES);
		break;
	case TRANS_VOID_ALI_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidAliPrecreteNum), &(pstSettle->_VoidAliPrecreteAmount), YES);
		break;
	//美团
	case TRANS_COUPON_MEITUAN:
		ChangeSettleItem(pszAmount, &(pstSettle->_MeituanNum), &(pstSettle->_MeituanAmount), YES);
		break;
	case TRANS_VOID_MEITUAN:
		ChangeSettleItem(pszAmount, &(pstSettle->_MeituanNum), &(pstSettle->_MeituanAmount), NO);
		break;
	//盘锦通
	case TRANS_PANJINTONG:
		ChangeSettleItem(pszAmount, &(pstSettle->_PanjintongNum), &(pstSettle->_PanjintongAmount), YES);
		break;
	//翼支付
	case TRANS_BESTPAY_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_BestpayCreatepayNum), &(pstSettle->_BestpayCreatepayAmount), YES);
		break;
	case TRANS_BESTPAY_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_BestpayRefundNum), &(pstSettle->_BestpayRefundAmount), YES);
		break;
	case TRANS_VOID_BESTPAY_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidBestpayCreatepayNum), &(pstSettle->_VoidBestpayCreatepayAmount), YES);
		break;
	default:
		break;
	}
	return APP_SUCC;
}

/**
* @brief 改变结算数据针对调整
* @param in pszOldAmt	 调整前原金额
* @param in pszAdjAmt	 调整后的金额(或小费的金额)
* @param in cOldTransType	 被调整的交易的原交易类型
* @param in pszInterOrg	 国际组织代码
* @return 
* @li APP_SUCC
*/
int ChgSettleForAdjust(const char *pszOldAmt, const char *pszAdjAmt, const char cOldTransType, const char *pszInterOrg)
{
	STSETTLE *pstSettle;
//	char szTmpA[13+1];
//	char cFlag = YES;
//	unsigned long	long ullAmt;
	if (memcmp(pszInterOrg,"CUP",3)==0)		/**< 内卡*/
	{
		pstSettle = &gstSettle_NK; 
	}
	else
	{
		pstSettle = &gstSettle_WK; 
	}
#if 0
	switch(cOldTransType)
	{
	case TRANS_SALE:/**<原交易为消费，做小费交易，增加小费的金额*/
		if (NULL == pszAdjAmt)
		{
			return APP_FAIL;
		}
		pstSettle->_SaleNum--;
		ChangeSettleItem(pszAdjAmt, &(pstSettle->_SaleNum), &(pstSettle->_SaleAmount), YES);
		break;
	case TRANS_OFFLINE:/**<原交易为离线结算，做结算调整交易，改变金额*/
		if (NULL == pszAdjAmt || NULL == pszOldAmt)
		{
			return APP_FAIL;
		}		
		memset(szTmpA, 0, sizeof(szTmpA));
		if (AscBigCmpAsc(pszAdjAmt, pszOldAmt) >= 0)
		{
			AmtSubAmt((uchar *)pszAdjAmt,(uchar *)pszOldAmt,(uchar *)szTmpA);
			cFlag = YES;
		}
		else //调整后的金额比原金额小
		{
			AmtSubAmt((uchar *)pszOldAmt,(uchar *)pszAdjAmt,(uchar *)szTmpA);
			cFlag = NO;
		}
		pstSettle->_OfflineNum--;
		ChangeSettleItem(szTmpA, &(pstSettle->_OfflineNum), &(pstSettle->_OfflineAmount), cFlag);
        	break;	
			
	default:
		return APP_FAIL;
		break;
	}
#endif	
	return APP_SUCC;
}
/**
* @brief 处理结算中断
* @param 无
* @return 
* @li APP_SUCC
* @li APP_FAIL
* @li APP_QUIT
*/
int DealSettleTask(void)
{
	if (YES == GetVarBatchHaltFlag() 
		|| YES == GetVarPrintSettleHalt()
		|| YES == GetVarPrintDetialHalt()
		|| YES == GetVarClrSettleDataFlag()
		)
	{
		if(PubConfirmDlg("上次结算未完成", "是否继续执行结算", 0, 30)!=APP_SUCC)
			return APP_FAIL;
		if (APP_SUCC == Settle(1))
		{
			if (YES == GetVarIsAutoLogOut())
			{
				LogOut();
			}

		#if defined(USE_TMS)
			DoTmsTask(TMS_POS_CHKUPDATA);
		#endif
			return APP_QUIT;
		}
		else
		{
			if (PubGetKeyCode(1) == KEY_ESC)
			{
				return APP_QUIT;
			}
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}


int CheckTransAmount(const char *pszAmount, const char cTransType)
{	
	SETTLE_AMT stTotalAmount_NK;
	SETTLE_AMT stTotalAmount_WK;
	uchar szAmt1_NK[12+1] = {0};
	uchar szAmt1_WK[12+1] = {0};
	uchar szAmt1[12+1] = {0};
	uchar szAmt[12+1] = {0};
	uchar szMaxAmt[12+1] = {0};
	
	switch(cTransType)
	{
	case TRANS_SALE:
	case TRANS_INSTALMENT:
		if(AscBigCmpAsc(pszAmount, ONE_TRADE_MAX_AMOUNT) > 0)
		{
			PubMsgDlg(NULL, "单笔交易金额过多!\n按任意键继续", 3, 10);
			return APP_FAIL;
		}
		memcpy(&stTotalAmount_NK, &gstSettle_NK._SaleAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._SaleAmount, sizeof(SETTLE_AMT));
		break;
	case TRANS_VOID_SALE:
	case TRANS_VOID_INSTALMENT:	
		memcpy(&stTotalAmount_NK, &gstSettle_NK._VoidSaleAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._VoidSaleAmount, sizeof(SETTLE_AMT));
		break;		
	case TRANS_AUTHSALE:
		if(AscBigCmpAsc(pszAmount, ONE_TRADE_MAX_AMOUNT) > 0)
		{
			PubMsgDlg(NULL, "单笔交易金额过多!\n按任意键继续", 3, 10);
			return APP_FAIL;
		}
		memcpy(&stTotalAmount_NK, &gstSettle_NK._AuthSaleAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._AuthSaleAmount, sizeof(SETTLE_AMT));
		break;		
	case TRANS_VOID_AUTHSALE:	
		memcpy(&stTotalAmount_NK, &gstSettle_NK._VoidAuthSaleAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._VoidAuthSaleAmount, sizeof(SETTLE_AMT));
		break;
	case TRANS_REFUND:	
		if(AscBigCmpAsc(pszAmount, ONE_TRADE_MAX_AMOUNT) > 0)
		{
			PubMsgDlg(NULL, "单笔交易金额过多!\n按任意键继续", 3, 10);
			return APP_FAIL;
		}
		memcpy(&stTotalAmount_NK, &gstSettle_NK._RefundAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._RefundAmount, sizeof(SETTLE_AMT));
        break;        	
	case TRANS_PREAUTH:  //预授权单笔交易不超过最大金额
		if(AscBigCmpAsc(pszAmount, ONE_TRADE_MAX_AMOUNT) > 0)
		{
			PubMsgDlg(NULL, "单笔交易金额过多!\n按任意键继续", 3, 10);
			return APP_FAIL;
		}
		memset(&stTotalAmount_NK, 0, sizeof(SETTLE_AMT));
		memset(&stTotalAmount_WK, 0, sizeof(SETTLE_AMT));
		break;
	default:
		return APP_SUCC;
		break;
	}

	memset(szAmt1_NK, 0, sizeof(szAmt1_NK));
	memset(szAmt1_WK, 0, sizeof(szAmt1_WK));
	memset(szAmt1, 0, sizeof(szAmt1));
	PubHexToAsc(stTotalAmount_NK.sSettleAmt, 12, 0, szAmt1_NK);
	PubHexToAsc(stTotalAmount_WK.sSettleAmt, 12, 0, szAmt1_WK);
	AmtAddAmt(szAmt1_NK, szAmt1_WK, szAmt1);
	memset(szAmt, 0, sizeof(szAmt));
	AmtAddAmt(szAmt1, (uchar*)pszAmount, szAmt);
	PubAddSymbolToStr((char *)szAmt, 12, '0', 0);
	strcpy((char *)szMaxAmt, MAX_AMOUNT);
	PubAddSymbolToStr((char *)szMaxAmt, 12, '0', 0);
	
	if (memcmp(szAmt, szMaxAmt, 12) > 0) /* 系统处理的上限*/
	{
		PubMsgDlg(NULL, "交易总金额过多!\n按任意键继续", 3, 10);
		return APP_FAIL;
	}
	else
	{
		return APP_SUCC;
	}
}

void InitSettleData(void)
{
	GetVarSettleData(&gstSettle_NK, &gstSettle_WK);
}


#if 0

/**
* @brief pos批上送
* @param 无
* @return 
* @li APP_SUCC
* @li APP_FAIL
* @li APP_QUIT
*/
static int DoBatchUp(void)
{	
	int nRet = 0;
	int nWaterSum;
	int nFileHandle;
	int nBatchNum = 0;
	char nMaxReSend, nSendNum = 0;
	int nIsFailBatchNum = TRUE;/**是否存在失败上送笔数*/

	PubDisplayTitle("批上送");
	GetWaterNum(&nWaterSum);

	if (nWaterSum > 0)
	{
		nRet = PubOpenFile(FILE_WATER, "w", &nFileHandle);
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{
			return APP_FAIL;
		}
	}
	else
	{
		return APP_SUCC;
	}

	/**
	* 取批上送的笔数，如果批上送没有中断的话，这个值将是0，
	*/
	GetVarCommReSendNum(&nMaxReSend);
	for(nSendNum = 0; nSendNum <= nMaxReSend && nIsFailBatchNum; nSendNum++)
	{
		if ('1' != GetVarCnCardFlag() || '1' != GetVarEnCardFlag())/**<判断结算平否的标志，不平需要批上送*/
		{
			/**
			* 上送磁条卡离线类交易
			*/
			nRet = BatchMagOffline(nFileHandle, nWaterSum, &nBatchNum);
			if (nRet != APP_SUCC)
			{
			//	SetVarBatchSum(nBatchNum);
				PubCloseFile(&nFileHandle);			
				return APP_FAIL;
			}
		#if defined(EMV_IC)
			/**
			* 上送EMV成功的脱机消费交易
			*/
			nRet = EmvBatchUpOfflineSucc(nFileHandle, nWaterSum, &nBatchNum);
			if (nRet != APP_SUCC)
			{
				//SetVarBatchSum(nBatchNum);
				PubCloseFile(&nFileHandle);			
				return APP_FAIL;
			}
		#endif
			/**
			* 上送金融交易,磁条卡的请求联机交易明细
			*/
			nRet = BatchMagTxn(nFileHandle, nWaterSum, &nBatchNum);
			if (nRet != APP_SUCC)
			{
				//SetVarBatchSum(nBatchNum);
				PubCloseFile(&nFileHandle);
				return APP_FAIL;
			}
			/**
			* 上送通知类交易
			*/
			nRet = BatchInform(nFileHandle, nWaterSum, &nBatchNum);
			if (nRet != APP_SUCC)
			{
				//SetVarBatchSum(nBatchNum);
				PubCloseFile(&nFileHandle);			
				return APP_FAIL;
			}
		}
#if defined(EMV_IC)
		//所有成功的联机交易上送
		nRet = EmvBatchUpOnlineSucc(nFileHandle, nWaterSum, &nBatchNum);
		if (nRet != APP_SUCC)
	 	{   
	 		//SetVarBatchSum(nBatchNum);
			PubCloseFile(&nFileHandle);
			return APP_FAIL;
	 	}
		//所有失败的脱机交易上送
		nRet = EmvBatchUpOfflineFail(nFileHandle, nWaterSum, &nBatchNum);
		if (nRet != APP_SUCC)
		{
			//SetVarBatchSum(nBatchNum);
			PubCloseFile(&nFileHandle);
			return APP_FAIL;
		}
		//IC卡卡片认证ARPC错但仍然同意该交易的联机交易
		nRet = EmvBatchUpOnlineSuccARPCErr(nFileHandle, nWaterSum, &nBatchNum);
		if (nRet != APP_SUCC)
	 	{   
	 		//SetVarBatchSum(nBatchNum);
			PubCloseFile(&nFileHandle);
			return APP_FAIL;
	 	}
#endif
	}
	ASSERT_FAIL(PubCloseFile(&nFileHandle));	
	GetVarBatchSum(&nBatchNum);
#if defined(EMV_IC)
#else
	if (nBatchNum > 0)
#endif
	{
		nRet = BatchUpEnd(nBatchNum);
		if (nRet != APP_SUCC)
		{
			return APP_FAIL;
		}
	}

	/*
	* 将批上送中断标识的数置为1，从第一笔开始送
	*/
	SetVarBatchMagOfflineHaltFlag(1);
	SetVarFinanceHaltFlag(1);
	SetVarMessageHaltFlag(1);	
	SetVarEmvOfflineUpNum(1);
	SetVarEmvOnlineUpNum(1);
	SetVarEMVOfflineFailHaltFlag(1);
	SetVarEMVOnlineARPCErrHaltFlag(1);
	/*
	* 将批上送的笔数置为0，下次重新统计
	*/
	SetVarBatchSum(0);
	return APP_SUCC;
}


int DealBatchUpFail(STWATER *pstWater, int nHandle, int nRecNo)
{
	int nRet, nBatchNum; 
	char nMaxReSend;

	GetVarCommReSendNum(&nMaxReSend);
	GetVarBatchSum(&nBatchNum);

	if(pstWater->cBatchUpFlag < nMaxReSend)
	{
		pstWater->cBatchUpFlag++;
	}
	else if(pstWater->cBatchUpFlag != 0xFE && pstWater->cBatchUpFlag != 0xFD)
	{
		pstWater->cBatchUpFlag = 0xFF;
		nBatchNum++;
		SetVarBatchSum(nBatchNum);
	}
	else
	{
		nBatchNum++;
		SetVarBatchSum(nBatchNum);
	}
	
	if(pstWater->cBatchUpFlag != 0)
	{	
		nRet = PubUpdateRecByHandle(nHandle, nRecNo, (char *)pstWater);
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{	
			return APP_FAIL; 
		}
	}
	return APP_SUCC;
}

#endif

