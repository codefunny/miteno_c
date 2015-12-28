/**
* @file water.c
* @brief ��ˮ����ģ��
* @version  1.0
* @author Ѧ���Ž�
* @date 2007-01-25
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


/**
* ����Ϊȫ�ֱ���
*/
static int gnCurrentRecNo=0;		/**<��ǰ��¼��*/

/**
* ����Ϊ�ӿ��ڲ�ʹ�õĺ���
*/

/**
* @brief ��ʼ����ˮ��¼�ļ�
*
*	��ˮ�ļ��������򴴽����ļ�������Ѵ��������´���ԭ�е���ˮ��¼��ɾ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitWaterFile(void)
{
	STRECFILE stWaterFile;
	STWATER stWaterRec;

	strcpy( stWaterFile.szFileName, FILE_WATER);
	stWaterFile.cIsIndex = FILE_CREATEINDEX;						/**< ���������ļ�*/
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
* @brief ������ˮ��¼
* @param in const STWATER *pstWater ��ˮ�ṹָ��
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
* @brief �޸ĵ�ǰ����ˮ��¼
* @param in const STWATER *pstWater ��ˮ�ṹָ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int UpdateWater(const STWATER *pstWater)
{
	return PubUpdateRec (FILE_WATER, gnCurrentRecNo,  (const char *)pstWater);
}

/**
* @brief �����ṩ��Ʊ�ݺŲ�����Ӧ����ˮ��¼
* @param in const char *szInvno ��Ҫ���ҵ���ˮ��Ʊ�ݺ�
* @param in int *pnRecNo           ���ز��ҵ�����ˮ��¼��
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
* @brief ����Ʊ�ݺŲ�����ˮ����ʾ����Ļ��
* 
*  ��POS��Ļ����ʾ����Ʊ�ݺ�,
*  �������ṩ��Ʊ�ݺŲ�����Ӧ����ˮ��¼,
*  ����Ļ����ʾ�ñʽ�����ˮ
* @param in int PrintFlag  ֵΪ1ʱ����ӡ�˵����´�ӡĳ��ˮʹ��	
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
		strcpy(szDispTitle, "�ش�����һ��");
	}
	else
	{
		strcpy(szDispTitle, "��ƾ֤�Ų�ѯ");
	}

	nLen = 6;
	memset(szInvno, 0, sizeof(szInvno));
	ASSERT_QUIT(PubInputDlg(szDispTitle, "������λ��ˮ��:", szInvno, &nLen, 1, 6, 0, INPUT_MODE_NUMBER));	
	PubAddSymbolToStr(szInvno, 6, '0', 0);
	nRet = FindWaterWithInvno( szInvno, &stWater);
	if (nRet==APP_FAIL)
	{
		char szDispBuf[100];
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C�޽���");
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
			nRet = PubConfirmDlg(szDispTitle, "�Ƿ��ӡ��Ʊ��?", 0, 50);
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
* @brief ȡ��ǰ��ˮ��¼������¼ֵ����STWATER *pstWater��
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int FetchCurrentWater(STWATER *pstWater)
{
	return PubReadOneRec(FILE_WATER,gnCurrentRecNo, (char *)pstWater);	
}

/**
* @brief ȡ��ˮ��¼�׼�¼
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
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
* @brief ȡ��һ����ˮ��¼
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
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
* @brief ȡ��һ����ˮ��¼
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
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
* @brief ȡ���һ����ˮ��¼
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
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
	* ��ʾ��������
	*/
	switch(pstWater->cTransType)
	{
	case TRANS_COUPON_VERIFY:
		sprintf(szContent, "������ȯ:");
		break;
	case TRANS_TEL_VERIFY:
		sprintf(szContent, "�ֻ�����ȯ:");
		break;
	case TRANS_CARD_VERIFY:
		sprintf(szContent, "������ȯ:");
		break;
	case TRANS_VOID_COUPON_VERIFY:
		sprintf(szContent, "������ȯ����:");
		break;
	case TRANS_VOID_TEL_VERIFY:
		sprintf(szContent, "�ֻ�����ȯ����:");
		break;
	case TRANS_VOID_CARD_VERIFY:
		sprintf(szContent, "������ȯ����:");
		break;
	default:
		sprintf(szContent, "δ֪:%d",pstWater->cTransType);
		break;
	}

	/*--------��ʾԭƱ�ݺ�------------*/
	switch(pstWater->cStatus)
	{
	case 3:		/**< �˻�*/
		break;
	case 1:		/**< ����*/
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(�ѳ�)", szContent);
		break;
	case 2:
		break;
	default:		/**< ����*/
		switch(pstWater->cTransType)
		{
		case TRANS_VOID_SALE:
		case TRANS_VOID_INSTALMENT:
		case TRANS_VOID_AUTHSALE:
		case TRANS_ADJUST:	/**<�ŵ������ж�*/
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
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R%6.6s(����)", szContent,szDispBuf);
	}
	else if(pstWater->cSendFlag >= 0xFD && (pstWater->cStatus == 2 || pstWater->cStatus == 4))
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(�ѵ�����)", szContent);
	}
	else if(pstWater->cSendFlag >= 0xFD)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(����)", szContent);
	}
	else if(pstWater->cStatus == 2 || pstWater->cStatus == 4)
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%s|R(�ѵ�)", szContent);
	}

	//�жϿ����Ƿ����
	if(pstWater->nPanLen > 0)
	{
		//����
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
	
	//ƾ֤��
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nVOUCH NO:|R%s", szDispBuf);

	//���		
	memset(szTemp, 0, sizeof(szTemp));
	memset(szDispBuf, 0, sizeof(szDispBuf));
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szTemp);	
	ProAmtToDispOrPnt(szTemp, szDispBuf);
	PubAllTrim(szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nAMOUNT:|R%s", szDispBuf);

	//ϵͳ�ο���
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
* @brief ����Ļ����ʾ�������׵���ˮ
* @param in STWATER *pstWater��ż�¼���ݵĽṹָ��
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


	//�жϿ����Ƿ����
	if(pstWater->nPanLen > 0)
	{
		//����
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
	
	//ƾ֤��
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)szDispBuf);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nVOUCH NO:|R%s", szDispBuf);

	//���		
	memset(szAmount, 0, sizeof(szAmount));
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szAmount);	
	ProAmtToDispOrPnt(szAmount, szDispAmt);
	PubAllTrim(szDispAmt);
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent+strlen(szContent), "\nAMOUNT:|R%s", szDispAmt);

	//ϵͳ�ο���
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
* @brief ����Ļ�� �����ʾ��ˮ������
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
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C�޽���");
		PubMsgDlg("�����¼", szDispBuf, 0, 5);
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
* @brief ��ˮ��¼�ṹת����ϵͳ���ݽṹ
* @param in STWATER *pstWater��ż�¼���ݵĽṹָ��
* @param out  STSYSTEM *pstSystem ת�����ϵͳ���ݽṹ
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int WaterToSys(const STWATER *pstWater, STSYSTEM *pstSystem)
{
	pstSystem->cTransType = pstWater->cTransType;							/**<��������*/
	pstSystem->cTransAttr = pstWater->cTransAttr;								/**<��������*/
	pstSystem->cEMV_Status = pstWater->cEMV_Status;							/**<EMV���׵�ִ��״̬*/
	PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)pstSystem->szPan);		/**<2 ���ʺ� */
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)pstSystem->szAmount);				/**<4 ���      */
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0, (uchar *)pstSystem->szTrace);					/**<POS��ˮ��*/
	PubHexToAsc((uchar *)pstWater->sTime, 6, 0, (uchar *)pstSystem->szTime);					/**<12�򣻽���ʱ��hhmmss */
	PubHexToAsc((uchar *)pstWater->sDate, 8, 0, (uchar *)pstSystem->szDate);					/**<13�򣻽�������yyyymmdd��yymmdd��mmdd */
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2) == 0)
	{
		;
	}
	else
	{
		PubHexToAsc((uchar *)pstWater->sExpDate, 4, 0, (uchar *)pstSystem->szExpDate);				/**<14 ����Ч��(ͬԭ������)*/
	}
	strcpy(pstSystem->szInputMode, pstWater->szInputMode);						/**<22 ����ģʽ*/
	strcpy(pstSystem->szRefnum, pstWater->szRefnum);							/**<37 ϵͳ�ο���*/
	PubHexToAsc((uchar *)pstWater->sTrack2, pstWater->nTrack2Len, 0, (uchar *)pstSystem->szTrack2);/**<35 ���ŵ�����(BCD)*/
	PubHexToAsc((uchar *)pstWater->sTrack3, pstWater->nTrack3Len, 0, (uchar *)pstSystem->szTrack3);/**<36 ���ŵ�����(BCD)*/
	strcpy(pstSystem->szResponse, pstWater->szResponse);						/**<39 ��Ӧ��*/
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0, (uchar *)pstSystem->szBatchNo);	/**<���κ�*/
	PubHexToAsc((uchar *)pstWater->sOldBatch, 6, 0, (uchar *)pstSystem->szOldBatchNo);	/**<ԭ�������κ�*/
	PubHexToAsc((uchar *)pstWater->sOldTrace, 6, 0, (uchar *)pstSystem->szOldTraceNo);	/**<ԭ��ˮ��   */
	strcpy(pstSystem->szOldRefnum, pstWater->szOldRefnum);					/**<ԭ���ײο���*/
	strcpy(pstSystem->szOperNow, pstWater->szOper);							/**<����Ա��                  */
	memcpy(pstSystem->szTelNo, pstWater->sTelNo, 11);
	memcpy(pstSystem->szCouponID, pstWater->sCouponID, 20);
	/**<int nAdditionLen;��char sAddition[100];�ɾ��彻�׸�ֵ��pstSystem*/
	
	return APP_SUCC;
}

/**
* @brief ϵͳ���ݽṹת������ˮ��¼�ṹ
* @param in  STSYSTEM *pstSystem ת�����ϵͳ���ݽṹ
* @param out STWATER *pstWater��ż�¼���ݵĽṹָ��
* @return
* @li APP_SUCC		
* @li APP_FAIL
*/
int SysToWater(const STSYSTEM *pstSystem, STWATER *pstWater )
{
	pstWater->cTransType = pstSystem->cTransType;							/**<��������*/
	pstWater->cTransAttr = pstSystem->cTransAttr;								/**<��������*/
	pstWater->cEMV_Status = pstSystem->cEMV_Status;							/**<EMV���׵�ִ��״̬*/
	memcpy(pstWater->sTransCode, pstSystem->szTransCode, 6);
	pstWater->nPanLen = strlen(pstSystem->szPan);								/**<2 ���ʺų���*/
	PubAscToHex((uchar *)pstSystem->szPan, pstWater->nPanLen, 0, (uchar *)pstWater->sPan);	/**<2 ���ʺ�(BCD)*/
	PubAscToHex((uchar *)pstSystem->szAmount, 12, 0, (uchar *)pstWater->szAmount);				/**<4 ���(BCD)*/
	PubAscToHex((uchar *)pstSystem->szTrace, 6, 0, (uchar *)pstWater->sTrace);					/**<11 POS��ˮ��(BCD)    */
	PubAscToHex((uchar *)pstSystem->szTime, 6, 0, (uchar *)pstWater->sTime);					/**<12 �򣻽���ʱ��hhmmss(BCD)  */
	PubAscToHex((uchar *)pstSystem->szDate, 8, 0, (uchar *)pstWater->sDate);					/**<13 �򣻽�������yyyymmdd��yymmdd��mmdd  (BCD)  */
	PubAscToHex((uchar *)pstSystem->szExpDate, 4, 0, (uchar *)pstWater->sExpDate);				/**<14 ����Ч��(ͬԭ������)*/
	strcpy(pstWater->szInputMode, pstSystem->szInputMode );					/**<22 ����ģʽ*/
	pstWater->nTrack2Len = strlen(pstSystem->szTrack2);		/**<35 ���ŵ����ݳ���*/
	PubAscToHex((uchar *)pstSystem->szTrack2, pstWater->nTrack2Len, 0, (uchar *)pstWater->sTrack2);/**<35 ���ŵ�����(BCD)*/
	pstWater->nTrack3Len = strlen(pstSystem->szTrack3);		/**<36 ���ŵ����ݳ���*/
	PubAscToHex((uchar *)pstSystem->szTrack3, pstWater->nTrack3Len, 0, (uchar *)pstWater->sTrack3);/**<36 ���ŵ�����(BCD)*/
	strcpy(pstWater->szRefnum, pstSystem->szRefnum);							/**<37 ϵͳ�ο���*/
	strcpy(pstWater->szResponse, pstSystem->szResponse);						/**<39 ��Ӧ��*/
	PubAscToHex((uchar *)pstSystem->szBatchNo, 6, 0, (uchar *)pstWater->szBatchNum);	/**<���κ�*/
	PubAscToHex((uchar *)pstSystem->szOldBatchNo, 6, 0, (uchar *)pstWater->sOldBatch );	/**<ԭ�������κ�*/
	PubAscToHex((uchar *)pstSystem->szOldTraceNo, 6, 0, (uchar *)pstWater->sOldTrace);	/**<ԭ��ˮ��(BCD)    */
	strcpy(pstWater->szOldRefnum, pstSystem->szOldRefnum);					/**<ԭ���ײο���*/
	pstWater->cOldTransType = pstSystem->cOldTransType;
	pstWater->cStatus = 0;											/**<����0, ��ȡ��1,�ѵ���2,���˻�3;  */
	strcpy(pstWater->szOper, pstSystem->szOperNow);							/**<����Ա��                  */
	strcpy(pstWater->sTelNo, pstSystem->szTelNo);	/**< �ֻ�����*/
	strcpy(pstWater->sCouponID, pstSystem->szCouponID);			/**<����*/
	PubAscToHex((uchar *)pstSystem->szRealAmount, 12, 0, (uchar *)pstWater->szRealAmount);				/**<���ý��(BCD)*/
	strcpy(pstWater->szOutTradeNo, pstSystem->szOutTradeNo);		/**<�̻�������*/
	PubAscToHex((uchar *)pstSystem->szPayableAmount, 12, 0, (uchar *)pstWater->sPayableAmount);				/**<Ӧ�����(BCD)*/
	pstWater->cSendFlag = 0;					/**<���ͱ�־0��δ���ͣ�0xFD��������,0xFF-����ʧ��*/
	pstWater->cBatchUpFlag= 0;					/**<���ͱ�־0��δ���ͣ�0xFD��������,0xFF-����ʧ��*/
	strcpy(pstWater->szAdvertisement, pstSystem->szAdvertisement);//�����Ϣ

	/**<int nAdditionLen;��char sAddition[100];�ɾ��彻�׸�ֵ��pstWater*/

	return APP_SUCC;
}


/**
* @brief �����ˮ��¼���ܼ�¼��
* @param out int *pnWaterNum��Ż�õļ�¼��
* @return ��
*/
void GetWaterNum( int *pnWaterNum)
{
	PubGetRecSum(FILE_WATER, pnWaterNum);
}

/**
* @brief �����ˮ�׼�¼�Ĳ���Ա����
* @param out psOper����Ա����
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
* @brief �ж�ָ���Ĳ���Ա�Ƿ��в�����ˮ����
* @param in psOper����Ա����
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
