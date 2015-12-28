/**
* @file managetxn.c
* @brief �����ཻ�״���ģ��
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

/**
* �ڲ���������
*/
/**
* �ӿں���ʵ��
*/
/**
* @brief ����ϵͳ�ṹ����
* @param pstSystem ϵͳ�ṹ����ָ��
* @return ��
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
	memcpy(pstSystem->szDate, szDateTime, 8);/**<ȡPOS��������,��Ҫ���ѻ�ʱ��*/
	memcpy(pstSystem->szTime, szDateTime + 8, 6);/**<ȡPOS����ʱ��,��Ҫ���ѻ�ʱ��*/	
	memset(szSn, 0, sizeof(szSn));
	NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_USN, &unLen, szSn);
	memset(szSoftVer, 0, sizeof(szSoftVer));
	GetVarDispSoftVer(szSoftVer);
	sprintf(pstSystem->szPosInfo, "SI=NEWLAND|SN=%s|TP=%s|CT=%d|RV=%s", szSn, PubGetPosTypeStr(), GetVarCommType(), szSoftVer);
	return ;
}

/**
* @brief ���POS�Ƿ�ǩ��
* @param ��
* @return @li APP_SUCC ��ǩ��
*		@li APP_FAIL δǩ��
*/
int ChkLoginStatus(void)
{
	if (YES != GetVarIsLogin())
	{
		PubMsgDlg(NULL, "POSδǩ��", 3, 3);
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief ������Ա��Ȩ���Ƿ�����ͨ����Ա
* @param ��
* @return @li APP_SUCC ����ͨ����Ա
*		@li APP_FAIL ������ͨ����Ա
*/
int ChkOperRole(void)
{
	int nOperRole;
	
	GetCurrentOper(NULL, NULL, &nOperRole);
	if (NORMALOPER != nOperRole)
	{
		PubMsgDlg("����", "����ִ�иý���", 3, 3);
		return APP_FAIL;
	}
	return APP_SUCC;
}




/**
* @fn Login
* @brief posǩ��
* @param ��
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
	char *pszTitle = "ǩ��";
	int nMainKeyNo = 0;
	char tpke[20];		/*<TPK ����*/
	char tpkcv[10];		/*<TPK ���ֵ*/
	char take[20];		/*<TAK ����*/
	char takcv[10];		/*<TAK ���ֵ*/
	char tdke[20];		/*<TDK ����*/
	char tdkcv[10];		/*<TDK ���ֵ*/
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
	*Ԥ����
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	/**
	*����
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
		PubMsgDlg(pszTitle, "PIN��Կ��װʧ��", 3, 10);
		return APP_FAIL;
	}
	nRet = PubLoadWorkKey(KEY_TYPE_MAC, take, nKeyLen, takcv);
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "MAC��Կ��װʧ��", 3, 10);
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
	PubMsgDlg(pszTitle, "  ǩ �� �� ��", 1, 1);
	ASSERT(SetVarIsLogin(YES));
	return APP_SUCC;
}





/**
* @fn Login
* @brief posǩ��
* @param ��
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
	char *pszTitle = "��Կ����";
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
	*Ԥ����
	*/
	ASSERT(CommPreDial());
	PubDisplayTitle(pszTitle);
	
	/**
	*����
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
	PubMsgDlg(pszTitle, "KEK���سɹ�", 1, 1);
	return APP_SUCC;
}


/**
* @brief posǩ��
* @param ��
* @return @li APP_SUCC
*		@li APP_FAIL
*		@li APP_QUIT
*/
int LogOut(void)
{
	char *pszTitle = "ǩ��";
	char szContent[100] = {0};
	
	IncVarTraceNo();
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|Cǩ�˳ɹ�");
	PubMsgDlg(pszTitle, szContent, 0, 1);
	ASSERT(SetVarIsLogin(NO));
	return APP_SUCC;
}


/**
* @brief ����ϵͳ����(��POS��������),��������ǩ�����������ݡ�״̬���͵�
* @param ��
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
* @brief ���POS����(����ˮ�洢����)
* @param ��
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
		sprintf(szMsg, " POS�洢����ˮ����%d��,���Ƚ���", nNowWaterSum);
		PubMsgDlg("��ܰ��ʾ", szMsg, 3, 30);
		return APP_FAIL;
	}
	return APP_SUCC;
}
/**
* @brief ����POS����(����ˮ�洢����)
* @param ��
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
		nRet = PubConfirmDlg("��ܰ��ʾ", "�洢��,�����", 3, 30);
		if (nRet == APP_SUCC)
		{
			//ASSERT_QUIT(DispTotal());
			if (Settle(0) != APP_SUCC)
			{
				PubMsgDlg("����ʧ��", "�����½���", 3, 10);
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
* @brief ���ڴ�ͳ����,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkTraditionOnOffStatus(char cTransType)
{
	if (YES != GetTraditionSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief ���ڵ����ֽ���,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkECashOnOffStatus(char cTransType,char cAttr)
{
	if (YES != GetECashSwitchOnoff(cTransType,cAttr))	
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief ���ڷ��ڸ����,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkInstallmentOnOffStatus(char cTransType)
{
	if (YES != GetInstallmentSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief ���ڻ��ֽ���,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkBonusOnOffStatus(char cTransType)
{
	if (YES != GetBonusSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??�����ֻ�оƬ����,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkPhoneChipSaleOnOffStatus(char cTransType)
{
	if (YES != GetPhoneChipSaleSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??����ԤԼ����,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkAppointmentOnOffStatus(char cTransType)
{
	if (YES != GetAppointmentSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??���ڶ�������,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkOrderOnOffStatus(char cTransType)
{
	if (YES != GetOrderSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ??������������,���Ŀǰ�������Ƿ�֧�ָý�������
* @param ��
* @return @li APP_SUCC ֧��
*		@li APP_FAIL �ݲ�֧��
*/
int ChkOtherOnOffStatus(char cTransType)
{
	if (YES != GetOtherSwitchOnoff(cTransType))
	{
		PubMsgDlg(NULL, "�ݲ�֧�ָý���", 3, 5);
		return APP_FAIL;
	}
	return APP_SUCC;
}



