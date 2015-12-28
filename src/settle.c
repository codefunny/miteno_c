/**
* @file settle.c
* @brief ���������ģ��
* @version  1.0
* @author �Ž�Ѧ��
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "magtxn.h"

#define MAX_AMOUNT "20000000000"/**<POS C ����������,2013-5-28,���˸�0,lld*/
#define ONE_TRADE_MAX_AMOUNT "9999999999"/**<���ʽ��ײ��ܳ������ֵ*/

extern int PackOffline(STSYSTEM *, const STWATER *);
extern int PackAdjust(STSYSTEM *, const STWATER *);
extern int PackAuthSaleOff(STSYSTEM *, STWATER *);


/**
* ����ȫ�ֱ���
*/
static STSETTLE gstSettle_NK, gstSettle_WK;

/**
* �ڲ���������
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
* @brief pos����
* @param in cFlag �����ʶ0��������    ��0�����жϺ���ٴν���
* @return 
* @li APP_SUCC
* @li APP_FAIL
* @li APP_QUIT
*/
int Settle(char cFlag)
{	

	char *pszTitle="����";
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
	* ���POS���ƣ������Ƿ����
	*/
	ASSERT_QUIT(DealPrintLimit()); 
	
	if (cFlag)
	{
		/*
		* ȡԭ�������жϵĲ���
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
		* ���������жϱ�ʶ������Ϊ1���ӵ�һ�ʿ�ʼ��
		*/
		SetVarBatchMagOfflineHaltFlag(1);
		SetVarFinanceHaltFlag(1);
		SetVarMessageHaltFlag(1);		
		SetVarEmvOfflineUpNum(1);
		SetVarEmvOnlineUpNum(1);
		SetVarEMVOfflineFailHaltFlag(1);
		SetVarEMVOnlineARPCErrHaltFlag(1);
		/*
		* �������͵ı�����Ϊ0������ͳ��
		*/
		SetVarBatchSum(0);
	}	

	/**
	* ��������
	*/
	if (cProcStep <= 1)
	{
		/**
		* ����Ƿ�ǩ��
		*/
		ASSERT_QUIT(ChkLoginStatus());

		/**
		* ִ�г���
		*/
		ASSERT_HANGUP_QUIT(DoReversal());
		PubDisplayTitle(pszTitle);		
		/**
		* �ٴ��жϲ���
		*/
		ASSERT_HANGUP_QUIT(CommConnect());
	
		/**
		* �ɼ�����
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
		*����
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
			SetVarCnCardFlag(stAmtNumSettle.cSettleCode_N);/*<�ڿ�Ӧ����*/
		}
		else
		{
			PubMsgDlg("����", "�ڿ�\n����Ӧ��������", 3, 10);
			CommHangUp();
			return APP_FAIL;
		}
		/**
		* �����������ʱ��
		*/
		PubAscToHex((uchar *)stSystem.szDate+4, 4, 0, (uchar *)szSettleDateTime);
		PubAscToHex((uchar *)stSystem.szTime, 6, 0, (uchar *)szSettleDateTime + 2);
		SetVarSettleDateTime(szSettleDateTime);
	}

	/**
	* ������
	*/
	if (cProcStep <= 2)
	{
		SetVarBatchHaltFlag(YES);
		/*����Ҫ�����ͨѶ\������⣬
		������Ӧ��ֻ��ʾ���ǲ�ʧ��,
		��ǿ�Ƴɹ�*/
		
		//����û����Ҫ�������͵Ľ���
		//ASSERT_HANGUP_FAIL(DoBatchUp());
		SetVarBatchHaltFlag(NO);
	}

	CommHangUp();

	/**
	* ��ӡ���㵥
	*/
	if (cProcStep <= 3)
	{
		SetVarPrintSettleHalt(YES);
		PubClearAll();
		PubDisplayTitle("���ڴ�ӡ���㵥");
		PubDisplayGen(3, "���Ժ�");
		PubUpdateWindow();
		nRet = PrintSettle(FIRSTPRINT);
		if (nRet == APP_FAIL)
		{
			return APP_FAIL;
		}
 		SetVarPrintSettleHalt(NO);
 	}
	
	/**
	* ��ӡ��ϸ��
	*/
	if (cProcStep <= 4)
	{
		SetVarPrintDetialHalt(YES);
		if (YES == GetVarIsPrintWaterRec())
		{
			int nSelect = 0;
			
			nRet = ProSelectList("0.��||1.��", "�Ƿ��ӡ��ϸ", 0xFF, &nSelect);
			if (nRet == APP_SUCC && nSelect == 1)
			{
				PubClear2To4();
				PubDisplayGen(3, "��ӡ��ϸ��");
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
			PubDisplayGen(3, "��ӡ���Ͳ��ɹ���");
			PubDisplayGen(4, "������ϸ");
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
			PubDisplayGen(3, "��ӡ���ͱ��ܵ�");
			PubDisplayGen(4, "������ϸ");
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
	* �������������ˮ��
	*/
	if (cProcStep <= 5)
	{
		SetVarClrSettleDataFlag(YES);
		PubClearAll();
		PubDisplayTitle(pszTitle);
		PubDisplayGen(2, "���ڴ������");
		PubUpdateWindow();
		nRet = InitWaterFile();/**<�����ˮ*/
		ASSERT(nRet);
		if (nRet != APP_SUCC)
		{
			InitWaterFile();/**<�����ˮ*/
		}
		ClearSettle();/**<�����������*/
		IncVarBatchNo();/**<���κż�һ*/
		SetVarClrSettleDataFlag(NO);
		SetVarOfflineUnSendNum(0);
	}
	PubClearAll();
	PubDisplayTitle(pszTitle);
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 2, "�� �� �� ��");
	PubUpdateWindow();
	return APP_SUCC;
}

/**
* @brief ����ڿ��Ľ�������
*		�����ļ�����ĳ���ģ�����
* @param in pstSettle	��Ž������ݵĽṹ
* @return 
* @li APP_SUCC
*/
int GetSettleDataNK(STSETTLE *pstSettle)
{
	memcpy((void *)pstSettle , (void *)&gstSettle_NK, sizeof(STSETTLE));
	return APP_SUCC;
}

/**
* @brief ����⿨�Ľ�������
*		�����ļ�����ĳ���ģ�����
* @param in pstSettle	��Ž������ݵĽṹ
* @return 
* @li APP_SUCC
*/
int GetSettleDataWK(STSETTLE *pstSettle)
{
	memcpy((void *)pstSettle, (void *)&gstSettle_WK, sizeof(STSETTLE));
	return APP_SUCC;
}

/**
* @brief ��ʾ�������ݵ�ĳһ��
*		��DispSettleData��������
* @param in pszTransName	������������
* @param in nTransNum	��������
* @param in nTransAmt		���׽��
* @return 
* @li APP_SUCC
*/
static int DispSettleDataItem(char *pszTransName, SETTLE_NUM nTransNum, uchar *pszAmt)
{
	int nRet;
	char szDispBuf[100] = {0};
	char szDispAmt[13+1] = {0};
	
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, pszTransName);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf+strlen(szDispBuf), "\n����:|R%12d", nTransNum);	
	
	memset(szDispAmt, 0, sizeof(szDispAmt));
	ProAmtToDispOrPnt((char *)pszAmt, szDispAmt);
	PubAllTrim(szDispAmt);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf+strlen(szDispBuf),"\n���:|R%sԪ", szDispAmt);		
	nRet = PubUpDownMsgDlg("��ѯ���׻���", szDispBuf, YES, 60, NULL);
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
* @brief �����ڿ��⿨��Ǵ����ܽ��
* @param in STAMT_NUM_INFO_SETTLE *pstInfoSettle �洢������ܽ��Ľṹ
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
* @brief ��ʾ�ڿ��⿨��Ǵ����ܽ��
* @param in ��
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

	GetSettleDataNK(&stSettle); 			/**< �ڿ���������*/

	nNum = stSettle._CouponVerifyNum - stSettle._VoidCouponVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && nNum > 0)
	{
		/*������ȯ*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CouponVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidCouponVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("������ȯ�ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._TelVerifyNum - stSettle._VoidTelVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && nNum > 0)
	{
		/*�ֻ�����ȯ*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._TelVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidTelVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("�ֻ�����ȯ�ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._CardVerifyNum - stSettle._VoidCardVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && nNum > 0)
	{
		/*������ȯ*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CardVerifyAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._TelVerifyAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		ASSERT_QUIT(DispSettleDataItem("���п���ȯ�ܼ�:", nNum, szAmt));
	}

	//�˻�
	nNum = stSettle._RefundVerifyNum;
	if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && nNum > 0) 
	{
		//�˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundVerifyAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("�˻��ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._PrecreateNum - stSettle._VoidPrecreateNum + stSettle._CreatepayNum - stSettle._VoidCreatepayNum;
	if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		&& (nNum > 0))
	{
		/*΢��ɨ��֧��*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._PrecreateAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidPrecreateAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//����֧��
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._CreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("΢��֧���ܼ�:", nNum, szAmt1));
	}

	nNum = stSettle._RefundWechatNum;
	if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && nNum > 0)
	{
		//΢���˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundWeChatAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("΢���˻��ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._BaiduCreatepayNum - stSettle._VoidBaiduCreatepayNum + stSettle._BaiduPrecreteNum - stSettle._VoidBaiduPrecreteNum;
	if((GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES) && nNum > 0)
	{
		/*�ٶȱ�ɨ*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._BaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//�ٶ���ɨ
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("�ٶ�֧���ܼ�:", nNum, szAmt1));
	}

	nNum = stSettle._RefundBaiduNum;
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && nNum > 0)
	{
		//�ٶ��˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._RefundBaiduAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("�ٶ��˻��ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._JDPrecreateNum - stSettle._VoidJDPrecreateNum;
	if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES ) && nNum > 0)
	{
		/*����֧��*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._JDPrecreateAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidJDPrecreateAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("����֧���ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._JDRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && nNum > 0)
	{
		//�����˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._JDRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("�����˻��ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._AliCreatepayNum - stSettle._VoidAliCreatepayNum + stSettle._AliPrecreteNum - stSettle._VoidAliPrecreteNum;
	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES && nNum > 0)
	{
		/*֧������ɨ*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmtTmp, 0, sizeof(szAmtTmp));
		PubHexToAsc(stSettle._AliPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidAliPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmtTmp);

		//֧������ɨ
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._AliCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidAliCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubAscAddAsc(szAmtTmp,szAmt,szAmt1);
		
		ASSERT_QUIT(DispSettleDataItem("֧����֧���ܼ�:", nNum, szAmt1));
	}
	
	nNum = stSettle._AliRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && nNum > 0)
	{
		//֧�����˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._AliRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("֧�����˻��ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._MeituanNum;
	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && nNum > 0)
	{
		//���Ŷ�ȯ
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._MeituanAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("���Ŷ�ȯ�ܼ�:", nNum, szAmt));
	}
	nNum = stSettle._PanjintongNum;
	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && nNum > 0)
	{
		//�̽�ͨ
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._PanjintongAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("�̽�֧ͨ���ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._BestpayCreatepayNum - stSettle._VoidBestpayCreatepayNum;
	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY) == YES && nNum > 0)
	{
		/*��֧��*/
		memset(szAmt1, 0, sizeof(szAmt1));
		memset(szAmt2, 0, sizeof(szAmt2));
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt1);
		PubHexToAsc(stSettle._VoidBestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt2);
		PubAscSubAsc(szAmt1,szAmt2,szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("��֧��֧���ܼ�:", nNum, szAmt));
	}

	nNum = stSettle._BestpayRefundNum;
	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES && nNum > 0)
	{
		//��֧���˻�
		memset(szAmt, 0, sizeof(szAmt));
		PubHexToAsc(stSettle._BestpayRefundAmount.sSettleAmt, 12, 0, szAmt);
		
		ASSERT_QUIT(DispSettleDataItem("��֧���˻��ܼ�:", nNum, szAmt));
	}

	return APP_SUCC;
}

/**
* @brief ���������,�ش�ӡ�����������( ���������)
* @param in ��
* @return ��
*/
int FindSettle()
{
	int nSelect;
	
	while(1)
	{
		ASSERT_QUIT( PubSelectListItem("1.����||2.��ӡ", "�����", NULL, 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			DispTotal();
			break;
		case 2:
			if(APP_SUCC==PubConfirmDlg("�����ܶ�", "��ȷ�ϼ���ӡ", 0, 0 )) 
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
* @brief �����������
*	����ڿ����⿨���ױ�������ˮ�ܱ�����֪ͨ�࣬�����࣬�ϵ���
* @param in ��
* @return ��
*/
void ClearSettle(void)
{
	memset((void *)(&gstSettle_NK), 0, sizeof(STSETTLE));
	memset((void *)(&gstSettle_WK), 0, sizeof(STSETTLE));
	
	// ���籣��
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
* @brief ���½����������Ŀ(��ChangeSettle��������)
*		��������ʱpnTransNum pnTransAmt   ����ΪstSettle_NK ����stSettle_WK�ṹ�еĳ�Ա
* @param in pszAmount	 ��Ҫ���ӵĽ��
* @param out pnTransNum	 �����ܱ���
* @param out pszAmount	 �����ܽ��
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
	if(memcmp(szAmt, szMaxAmt, 12) > 0)/* ϵͳ���������*/
	{
		PubMsgDlg("�����ܽ�����!", "�뾡����н���\n�����������", 3, 10);
	}
	return APP_SUCC;
}
/**
* @brief ��¼��������
* @param in pszAmount	 ��Ҫ���ӵĽ��
* @param in cTransType	 ��������
* @param in pszInterOrg	 ������֯����
* @return 
* @li APP_SUCC
*/
int ChangeSettle(const char *pszAmount, const char cTransType, const char *pszInterOrg)
{
	STSETTLE *pstSettle;
	if (memcmp(pszInterOrg,"CUP",3)==0)		/**< �ڿ�*/
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
	//΢��	
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
	//�ٶ�	
	case TRANS_CREATEANDPAYBAIDU:
		ChangeSettleItem(pszAmount, &(pstSettle->_BaiduCreatepayNum), &(pstSettle->_BaiduCreatepayAmount), YES);
		break;
	case TRANS_BAIDU_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_RefundBaiduNum), &(pstSettle->_RefundBaiduAmount), YES);
		break;
	case TRANS_VOID_CREATEANDPAYBAIDU:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidBaiduCreatepayNum), &(pstSettle->_VoidBaiduCreatepayAmount), YES);
		break;
	//���Ӱٶȱ�ɨ/��ɨ����  wanglez add 20151221
	case TRANS_BAIDU_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_BaiduPrecreteNum), &(pstSettle->_BaiduPrecreteAmount), YES);
		break;
	case TRANS_VOID_BAIDU_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidBaiduPrecreteNum), &(pstSettle->_VoidBaiduPrecreteAmount), YES);
		break;
		
	//����
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
	//֧����
	case TRANS_ALI_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliCreatepayNum), &(pstSettle->_AliCreatepayAmount), YES);
		break;
	case TRANS_ALI_REFUND:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliRefundNum), &(pstSettle->_AliRefundAmount), YES);
		break;
	case TRANS_VOID_ALI_CREATEANDPAY:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidAliCreatepayNum), &(pstSettle->_VoidAliCreatepayAmount), YES);
		break;
	//����֧������ɨ/��ɨ����  wanglez add 20151221
	case TRANS_ALI_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_AliPrecreteNum), &(pstSettle->_AliPrecreteAmount), YES);
		break;
	case TRANS_VOID_ALI_PRECREATE:
		ChangeSettleItem(pszAmount, &(pstSettle->_VoidAliPrecreteNum), &(pstSettle->_VoidAliPrecreteAmount), YES);
		break;
	//����
	case TRANS_COUPON_MEITUAN:
		ChangeSettleItem(pszAmount, &(pstSettle->_MeituanNum), &(pstSettle->_MeituanAmount), YES);
		break;
	case TRANS_VOID_MEITUAN:
		ChangeSettleItem(pszAmount, &(pstSettle->_MeituanNum), &(pstSettle->_MeituanAmount), NO);
		break;
	//�̽�ͨ
	case TRANS_PANJINTONG:
		ChangeSettleItem(pszAmount, &(pstSettle->_PanjintongNum), &(pstSettle->_PanjintongAmount), YES);
		break;
	//��֧��
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
* @brief �ı����������Ե���
* @param in pszOldAmt	 ����ǰԭ���
* @param in pszAdjAmt	 ������Ľ��(��С�ѵĽ��)
* @param in cOldTransType	 �������Ľ��׵�ԭ��������
* @param in pszInterOrg	 ������֯����
* @return 
* @li APP_SUCC
*/
int ChgSettleForAdjust(const char *pszOldAmt, const char *pszAdjAmt, const char cOldTransType, const char *pszInterOrg)
{
	STSETTLE *pstSettle;
//	char szTmpA[13+1];
//	char cFlag = YES;
//	unsigned long	long ullAmt;
	if (memcmp(pszInterOrg,"CUP",3)==0)		/**< �ڿ�*/
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
	case TRANS_SALE:/**<ԭ����Ϊ���ѣ���С�ѽ��ף�����С�ѵĽ��*/
		if (NULL == pszAdjAmt)
		{
			return APP_FAIL;
		}
		pstSettle->_SaleNum--;
		ChangeSettleItem(pszAdjAmt, &(pstSettle->_SaleNum), &(pstSettle->_SaleAmount), YES);
		break;
	case TRANS_OFFLINE:/**<ԭ����Ϊ���߽��㣬������������ף��ı���*/
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
		else //������Ľ���ԭ���С
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
* @brief ��������ж�
* @param ��
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
		if(PubConfirmDlg("�ϴν���δ���", "�Ƿ����ִ�н���", 0, 30)!=APP_SUCC)
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
			PubMsgDlg(NULL, "���ʽ��׽�����!\n�����������", 3, 10);
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
			PubMsgDlg(NULL, "���ʽ��׽�����!\n�����������", 3, 10);
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
			PubMsgDlg(NULL, "���ʽ��׽�����!\n�����������", 3, 10);
			return APP_FAIL;
		}
		memcpy(&stTotalAmount_NK, &gstSettle_NK._RefundAmount, sizeof(SETTLE_AMT));
		memcpy(&stTotalAmount_WK, &gstSettle_WK._RefundAmount, sizeof(SETTLE_AMT));
        break;        	
	case TRANS_PREAUTH:  //Ԥ��Ȩ���ʽ��ײ����������
		if(AscBigCmpAsc(pszAmount, ONE_TRADE_MAX_AMOUNT) > 0)
		{
			PubMsgDlg(NULL, "���ʽ��׽�����!\n�����������", 3, 10);
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
	
	if (memcmp(szAmt, szMaxAmt, 12) > 0) /* ϵͳ���������*/
	{
		PubMsgDlg(NULL, "�����ܽ�����!\n�����������", 3, 10);
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
* @brief pos������
* @param ��
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
	int nIsFailBatchNum = TRUE;/**�Ƿ����ʧ�����ͱ���*/

	PubDisplayTitle("������");
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
	* ȡ�����͵ı��������������û���жϵĻ������ֵ����0��
	*/
	GetVarCommReSendNum(&nMaxReSend);
	for(nSendNum = 0; nSendNum <= nMaxReSend && nIsFailBatchNum; nSendNum++)
	{
		if ('1' != GetVarCnCardFlag() || '1' != GetVarEnCardFlag())/**<�жϽ���ƽ��ı�־����ƽ��Ҫ������*/
		{
			/**
			* ���ʹ����������ཻ��
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
			* ����EMV�ɹ����ѻ����ѽ���
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
			* ���ͽ��ڽ���,����������������������ϸ
			*/
			nRet = BatchMagTxn(nFileHandle, nWaterSum, &nBatchNum);
			if (nRet != APP_SUCC)
			{
				//SetVarBatchSum(nBatchNum);
				PubCloseFile(&nFileHandle);
				return APP_FAIL;
			}
			/**
			* ����֪ͨ�ཻ��
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
		//���гɹ���������������
		nRet = EmvBatchUpOnlineSucc(nFileHandle, nWaterSum, &nBatchNum);
		if (nRet != APP_SUCC)
	 	{   
	 		//SetVarBatchSum(nBatchNum);
			PubCloseFile(&nFileHandle);
			return APP_FAIL;
	 	}
		//����ʧ�ܵ��ѻ���������
		nRet = EmvBatchUpOfflineFail(nFileHandle, nWaterSum, &nBatchNum);
		if (nRet != APP_SUCC)
		{
			//SetVarBatchSum(nBatchNum);
			PubCloseFile(&nFileHandle);
			return APP_FAIL;
		}
		//IC����Ƭ��֤ARPC����Ȼͬ��ý��׵���������
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
	* ���������жϱ�ʶ������Ϊ1���ӵ�һ�ʿ�ʼ��
	*/
	SetVarBatchMagOfflineHaltFlag(1);
	SetVarFinanceHaltFlag(1);
	SetVarMessageHaltFlag(1);	
	SetVarEmvOfflineUpNum(1);
	SetVarEmvOnlineUpNum(1);
	SetVarEMVOfflineFailHaltFlag(1);
	SetVarEMVOnlineARPCErrHaltFlag(1);
	/*
	* �������͵ı�����Ϊ0���´�����ͳ��
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

