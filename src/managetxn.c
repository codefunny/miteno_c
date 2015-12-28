/**
* @file managetxn.c
* @brief 管理类交易处理模块
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

/**
* 内部函数声明
*/
/**
* 接口函数实现
*/
/**
* @brief 处理系统结构数据
* @param pstSystem 系统结构数据指针
* @return 无
*/
void DealSystem(STSYSTEM *pstSystem)
{
	char szDateTime[14+1];
	char szSn[50+1];
	unsigned int unLen=0;
	char szSoftVer[16+1];
	
	GetVarTraceNo(pstSystem->szTrace);
	GetCurrentOper(pstSystem->szOperNow, NULL, NULL);
	
	GetVarTerminalId(pstSystem->szPosID);
	GetVarMerchantId(pstSystem->szShopID);
	
	GetVarBatchNo(pstSystem->szBatchNo);
	memset(szDateTime, 0, sizeof(szDateTime));

	
	PubGetCurrentDatetime(szDateTime);
	memcpy(pstSystem->szDate, szDateTime, 8);/**<取POS本地日期,主要是脱机时用*/
	memcpy(pstSystem->szTime, szDateTime + 8, 6);/**<取POS本地时间,主要是脱机时用*/	
	memset(szSn, 0, sizeof(szSn));
	NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_USN, &unLen, szSn);
	memset(szSoftVer, 0, sizeof(szSoftVer));
	GetVarDispSoftVer(szSoftVer);
	sprintf(pstSystem->szPosInfo, "SI=NEWLAND|SN=%s|TP=%s|CT=%d|RV=%s", szSn, PubGetPosTypeStr(), GetVarCommType(), szSoftVer);
	return ;
}

/**
* @brief 检测POS是否签到
* @param 无
* @return @li APP_SUCC 已签到
*		@li APP_FAIL 未签到
*/
int ChkLoginStatus(void)
{
	if (YES != GetVarIsLogin())
	{
		PubMsgDlg(NULL, "POS未签到", 3, 3);
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief 检测操作员的权限是否是普通操作员
* @param 无
* @return @li APP_SUCC 是普通操作员
*		@li APP_FAIL 不是普通操作员
*/
int ChkOperRole(void)
{
	int nOperRole;
	
	GetCurrentOper(NULL, NULL, &nOperRole);
	if (NORMALOPER != nOperRole)
	{
		PubMsgDlg("主管", "不能执行该交易", 3, 3);
		return APP_FAIL;
	}
	return APP_SUCC;
}




/**
* @fn Login
* @brief pos签到
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int Login(void)
{
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STSYSTEM stSystem;
	int nTagSum=0;
	char *pszTitle = "签到";
	int nMainKeyNo = 0;
	char tpke[20];		/*<TPK 密文*/
	char tpkcv[10];		/*<TPK 检查值*/
	char take[20];		/*<TAK 密文*/
	char takcv[10];		/*<TAK 检查值*/
	char tdke[20];		/*<TDK 密文*/
	char tdkcv[10];		/*<TDK 检查值*/
	char tpkcv_buf[10];  
	int nLen=0;
	int nRet;
	char szTmp[100+1];
	int nKeyLen;

	memset(&stSystem, 0, sizeof(STSYSTEM));
	memset(sPackBuf, 0, sizeof(sPackBuf));
	
	nTagSum = 0;

	PubDisplayTitle(pszTitle);
	
	DealSystem(&stSystem);
	
	memcpy(stSystem.szTransCode, "100001", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(97, stSystem.szPosInfo, sPackBuf, &nPackLen, &nTagSum));

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUp();
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}

	memset(tpke,'\0',sizeof(tpke)); 
	memset(tpkcv_buf,'\0',sizeof(tpkcv_buf)); 
	memset(take,'\0',sizeof(take)); 
	memset(takcv,'\0',sizeof(takcv)); 
	memset(tdke,'\0',sizeof(tdke)); 
	memset(tdkcv,'\0',sizeof(tdkcv));


	memset(szTmp, 0, sizeof(szTmp));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(92, szTmp, &nLen));
	memcpy(tpke, szTmp, 16);
	memcpy(tpkcv,szTmp+16,4);

	memset(szTmp, 0, sizeof(szTmp));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(93, szTmp, &nLen));
	memcpy(take, szTmp, 16);
	memcpy(takcv,szTmp+16,4);

	GetVarMainKeyNo(&nMainKeyNo);
	PubSetCurrentMainKeyIndex(nMainKeyNo);
	nKeyLen = 16;
	nRet = PubLoadWorkKey(KEY_TYPE_PIN, tpke, nKeyLen, tpkcv);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "PIN密钥下装失败", 3, 10);
		return APP_FAIL;
	}
	nRet = PubLoadWorkKey(KEY_TYPE_MAC, take, nKeyLen, takcv);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "MAC密钥下装失败", 3, 10);
		return APP_FAIL;
	}


	memset(szTmp, 0, sizeof(szTmp));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(94, szTmp, &nLen));
	SetVarMerchantNameCn(szTmp);

	memset(szTmp, 0, sizeof(szTmp));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(96, szTmp, &nLen));
//PubMsgDlg("debug",szTmp,0,0);
//memset(szTmp, '1', 20);
//strcpy(szTmp, "1111011111111111111");
	SetTransTieTieSwitchValue(szTmp);
	
	PubSetPosDateTime(stSystem.szDate, "YYYYMMDD", stSystem.szTime);	
	ASSERT(SetVarBatchNo(stSystem.szBatchNo));
	ASSERT(SetVarIsLogin(YES));
	SetVarLastLoginDateTime(stSystem.szDate);
	PubClearAll();
	PubMsgDlg(pszTitle, "  签 到 成 功", 1, 1);
	ASSERT(SetVarIsLogin(YES));
	return APP_SUCC;
}





/**
* @fn Login
* @brief pos签到
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int DownloadKek(void)
{
	char sPackBuf[MAX_PACK_SIZE]; 
	int nPackLen=0;
	STSYSTEM stSystem;
	int nTagSum=0;
	char *pszTitle = "密钥下载";
	char szMainKey[50+1];
	int nMainKeyNo = 0;
	int nKeyLen;
	int nLen;


	memset(&stSystem, 0, sizeof(STSYSTEM));
	memset(sPackBuf, 0, sizeof(sPackBuf));
	
	nTagSum = 0;

	PubDisplayTitle(pszTitle);
	
	DealSystem(&stSystem);
	
	memcpy(stSystem.szTransCode, "200003", 6);
	ASSERT_FAIL(CustodyTlvAddValue(1, stSystem.szTransCode, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(2, stSystem.szDate, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(3, stSystem.szTime, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(4, stSystem.szTrace, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(7, stSystem.szPosID, sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyTlvAddValue(8, stSystem.szShopID, sPackBuf, &nPackLen, &nTagSum));
	//PrintDebugData(sPackBuf, nPackLen);

	/**
	*预拨号
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	
	/**
	*拨号
	*/
	ASSERT_HANGUP_QUIT(CommConnect());
	ASSERT_HANGUP_FAIL(CustodyPack(sPackBuf, &nPackLen, nTagSum));
	IncVarTraceNo();
	ASSERT_HANGUP_FAIL(CommSend(sPackBuf, nPackLen));
	ASSERT_HANGUP_FAIL(CommRecv(sPackBuf, &nPackLen));
	CommHangUp();
	ASSERT_FAIL(CustodyUpPack(sPackBuf, &nPackLen, &nTagSum));
	ASSERT_FAIL(CustodyChkRespon(&stSystem, sPackBuf, nPackLen));
	if (memcmp(stSystem.szResponse, "00", 2) != 0)
	{
		CustoyDispResp(stSystem.szResponse, stSystem.szRespInfo);
		return APP_QUIT;
	}
	PubDisplayTitle(pszTitle);
	PubSetPosDateTime(stSystem.szDate, "YYYYMMDD", stSystem.szTime);	
	memset(szMainKey, 0, sizeof(szMainKey));
	nLen = 0;
	ASSERT_FAIL(CustodyGetField(91, szMainKey, &nLen));
	GetVarMainKeyNo(&nMainKeyNo);
	PubSetCurrentMainKeyIndex(nMainKeyNo);
	if (DESMODE_DES == GetVarEncyptMode())
	{
		nKeyLen = 8;
	}
	else
	{
		nKeyLen = 16;
	}	
	PubLoadMainKey(nMainKeyNo,szMainKey, nKeyLen);
	SetVarDownKek(NO);
	PubClearAll();
	PubMsgDlg(pszTitle, "KEK下载成功", 1, 1);
	return APP_SUCC;
}


/**
* @brief pos签到
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int LogOut(void)
{
	char *pszTitle = "签退";
	char szContent[100] = {0};
	
	IncVarTraceNo();
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C签退成功");
	PubMsgDlg(pszTitle, szContent, 0, 1);
	ASSERT(SetVarIsLogin(NO));
	return APP_SUCC;
}


/**
* @brief 处理系统请求(由POS中心引发),包含重新签到、参数传递、状态上送等
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int DealSysReq(void)
{
	char sTmp[6+1];
	char cStatus = NO;
	int i, nReSend;
	memset(sTmp, 0, sizeof(sTmp));
	PubGetVar(&cStatus, fIsReLogin_off, fIsReLogin_len);
	if(cStatus == YES)
	{
		GetVarCommReSendNum((char *)&nReSend);
		for (i = 0; i <= nReSend; i++)
		{
			if (APP_SUCC == Login())
				break;
		}
	}

	return APP_SUCC;
}

/**
* @brief 检查POS限制(如流水存储满等)
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int ChkPosLimit(void)
{
	int nNowWaterSum = 0;
	int nPosMaxWaterSum;
	char szMsg[48+1];

	GetWaterNum(&nNowWaterSum);
	GetVarMaxTransCount(&nPosMaxWaterSum);

	if (nNowWaterSum >= nPosMaxWaterSum)
	{
		sprintf(szMsg, " POS存储的流水已满%d笔,请先结算", nNowWaterSum);
		PubMsgDlg("温馨提示", szMsg, 3, 30);
		return APP_FAIL;
	}
	return APP_SUCC;
}
/**
* @brief 处理POS限制(如流水存储满等)
* @param 无
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int DealPosLimit(void)
{
	int nNowWaterSum = 0;
	int nPosMaxWaterSum;
	int nRet;
	ulong nSpace;

	GetWaterNum(&nNowWaterSum);
	GetVarMaxTransCount(&nPosMaxWaterSum);
	NDK_FsGetDiskSpace(1, &nSpace);

	nRet = DealSettleTask();
	 if (APP_SUCC != nRet)
	{
		return nRet;
	}

	if (nNowWaterSum >= nPosMaxWaterSum || nSpace <= (10*1024))
	{
		nRet = PubConfirmDlg("温馨提示", "存储满,请结算", 3, 30);
		if (nRet == APP_SUCC)
		{
			//ASSERT_QUIT(DispTotal());
			if (Settle(0) != APP_SUCC)
			{
				PubMsgDlg("结算失败", "请重新结算", 3, 10);
				return APP_FAIL;
			}
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
			return APP_FAIL;
		}
	}
	ASSERT_FAIL(Ums_DealPrintLimit());
	return APP_SUCC;
}
/**
* @brief 对于传统交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkTraditionOnOffStatus(char cTransType)
{
	if (YES != GetTraditionSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief 对于电子现金交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkECashOnOffStatus(char cTransType,char cAttr)
{
	if (YES != GetECashSwitchOnoff(cTransType,cAttr))	
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief 对于分期付款交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkInstallmentOnOffStatus(char cTransType)
{
	if (YES != GetInstallmentSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief 对于积分交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkBonusOnOffStatus(char cTransType)
{
	if (YES != GetBonusSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??对于手机芯片交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkPhoneChipSaleOnOffStatus(char cTransType)
{
	if (YES != GetPhoneChipSaleSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??对于预约交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkAppointmentOnOffStatus(char cTransType)
{
	if (YES != GetAppointmentSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??对于订购交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkOrderOnOffStatus(char cTransType)
{
	if (YES != GetOrderSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??对于其他交易,检测目前的设置是否支持该交易类型
* @param 无
* @return @li APP_SUCC 支持
*		@li APP_FAIL 暂不支持
*/
int ChkOtherOnOffStatus(char cTransType)
{
	if (YES != GetOtherSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "暂不支持该交易", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



