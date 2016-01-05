/**
* @file print.c
* @brief ��ӡ����ģ��
* @version  1.0
* @author Ѧ��
* @date 2007-01-25
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "barcode.h"

#define DISP_CHK_CARD_HOLDER_SIGNATURE \
{\
	PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "��˶�ǩ��");\
	PubUpdateWindow();\
	PubBeep(3);\
}

/**
* ����Ϊ��ģ���ڲ�ʹ�õĺ���
*/
static int SetPrintData_Water(const STWATER *, int, const int);
static int SetAndPrint_Settle(void *);
static int SetAndPrint_Allwater(void *);
static int SetAndPrint_Total(void *);
static int SetAndPrint_Param(void * );
//static int SetAndPrint_Failwater(void *);
static int _printwater(void *);
//static int Print_GetTransType(STWATER *, char *);
//static int PntHex(const char *, const uchar *, const int);
//static int  _printemvparam();
static void DealPrintAmount(char, char *);
static int printYiwei(const char *pszCode);
//static int PntHex(const char *pszTitle, const uchar *psContent, const int nConternLen);


enum PRINT_PARAM_TYPE
{
	PRINT_PARAM_TYPE_MERCHANTINFO=0x01,		/*�̻���Ϣ*/
	PRINT_PARAM_TYPE_TRANSCCTRL=0x02,		/*���׿���*/
	PRINT_PARAM_TYPE_SYSTEMCTRL=0x04,		/*ϵͳ����*/
	PRINT_PARAM_TYPE_COMM=0x08,				/*ͨѶ����*/
	PRINT_PARAM_TYPE_VERSION=0x10,			/*�汾��Ϣ*/
	PRINT_PARAM_TYPE_EMV=0x20,			/*EMV����*/
	PRINT_PARAM_TYPE_OTHER=0x40				/*����*/
};


/* ��ʽ�޸�[1234+���+��������]*/
static const char gszBankName[][20]={//�����������
	{"0100�ʴ��ʴ�����  "},
	{"0102���й�������  "},
	{"0103ũ��ũҵ����  "},
	{"0104�����й�����  "},
	{"0105���н�������  "},
	{"0301���н�ͨ����  "},
	{"0302������������  "},
	{"0303���������  "},
	{"0304���Ļ�������  "},
	{"0305������������  "},
	{"0306�㷢�㷢����  "},
	{"0307������  "},
	{"0308������������  "},
	{"0309��ҵ��ҵ����  "},
	{"0310�ַ��ַ�����  "},
	{"0001����������  "},
	{"0313������ҵ����  "},
	{"0401�Ϻ��Ϻ�����  "},
	{"0403������������  "},
	{"1409ũ��ũ��������"},
	{"0464Ȫ��Ȫ������  "},	/* Ȫ����ҵ*/
	{"0405���̸�������  "},	/* ������ҵ*/
	{"0410ƽ��ƽ������  "},
	{"1410ũ��ũ��������"},
	{"0414�����人����  "},	/* �人����*/
	{"0425������ݸ����  "},
	{"0432�����˲�����  "},	/* �˲�����*/
	{"0461������ɳ����  "},
	{"0464Ȫ��Ȫ������  "},	/* Ȫ����ҵ*/
	{"0489������������  "},
	{"0570���滪���潭  "},
	{"1401��ũ�Ϻ�ũ��  "},
	{"1418��ũ����ũ��  "},
	{"1438��ũ����ũ��  "},
	{"4802������������  "},
	{"0000"},
};					

typedef struct tagPrintPara
{
	const STWATER *pstWater;
	int nReprintFlag;
	int nCurrentPage;
} STPRINTPATA;


typedef struct tagPrintImage
{
	const BmpInfo *pBmpInfo;
}STPRINTIMAGE;



/**
* @brief ��ӡǩ����
*
*    ͨ������_printwater ʵ��
* @param in const STWATER *pstWater ��ˮ�ṹָ��
* @param in int nReprintFlag  ��ӡ���Ʊ�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintWater(const STWATER *pstWater, int nReprintFlag)
{
	int nCurrentPage,nPage=1;
	STPRINTPATA stPara;
	
	if (PubGetPrinter() == _PRINTTYPE_TP)
	{
		GetVarPrintPageCount(&nPage);
	}

	for(nCurrentPage = nPage; nCurrentPage >= 1; nCurrentPage--)
	{
		stPara.pstWater = pstWater;
		stPara.nReprintFlag = nReprintFlag;
		stPara.nCurrentPage = nCurrentPage;

		ASSERT_FAIL(PubPrintCommit( _printwater, (void *)&stPara, 1));
		if (nCurrentPage > 1)
		{
			NDK_ScrPush();
			PubClearAll();
			PubDisplayStrInline(DISPLAY_MODE_CENTER, 4, "�� ˺ Ʊ ��...");
			PubUpdateWindow();
			if(PubGetKeyCode(30) == KEY_ESC)//0.88����ĳ�30��
			{
				//141103���ݽ�������������󣬼�����Ӧȡ������������ӡ
				return APP_SUCC;
			}
			PubClearLine(4, 4);
			NDK_ScrPop();
		}
	}
	return APP_SUCC;
}



/**
* @brief ��ӡǩ����
*
*    ������PrintWater������ʵ��ǩ�����ݵĴ�ӡ
* @param in void *ptrPara �����ṹָ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int _printwater( void *ptrPara )
{
	STPRINTPATA *pstPara;
	pstPara = (STPRINTPATA *)ptrPara;
	char szModuleName[50] = {0};
	// �жϴ�ӡ���������������
	ASSERT_FAIL(SetPrintData_Water(pstPara->pstWater, pstPara->nReprintFlag, pstPara->nCurrentPage));
	if (PubGetPrinter()==_PRINTTYPE_TP)	// ����
	{
		PubSetPrnFontSize(GetVarPrnFontSize());
		strcpy(szModuleName, "PRINT_WATER_WITHOUT_ENGLISH");
	}
	else if (PubGetPrinter()==_PRINTTYPE_IP)// ���
	{
		//�ж����ʹ�õĴ�ӡƱ�ݸ�ʽ
		if (YES == GetVarIsNewTicket())
		{
			strcpy(szModuleName, "PRINT_WATER_IP_UNBLANK");
		}
		else
		{
			strcpy(szModuleName, "PRINT_WATER_IP_BLANK");
		}
	}	
	else
		return APP_FAIL;
	PubPrintModule(szModuleName);

	
	return APP_SUCC;
}


/**
* @brief ���������Ľ������ͻ�ý��׵���Ӣ������(��������ӡ���Ĵ�ӡ��������)
* @param in char cTransType ��������
* @param out char *pszTransName1 �������Ͷ�Ӧ����������
* @param out char *pszTransName2 �������Ͷ�Ӧ��Ӣ������
* @return ��
*/
void GetTransNameForTP(const STWATER *pstWater, char *pszTransName1, char *pszTransName2)
{	

	switch( pstWater->cTransType )
	{
		case TRANS_COUPON_VERIFY:
			strcpy(pszTransName1, "����ж�");
			strcpy(pszTransName2, "Verify By Coupon number");
			break;
		case TRANS_TEL_VERIFY:
			strcpy(pszTransName1, "�ֻ��ųж�");
			strcpy(pszTransName2, "Verify By phonenumber");
			break;
		case TRANS_CARD_VERIFY:
			strcpy(pszTransName1, "���п���ȯ");
			strcpy(pszTransName2, "Verify By card");
			break;
		case TRANS_VOID_COUPON_VERIFY:	
			strcpy(pszTransName1, "������ȯ����");
			strcpy(pszTransName2, "Void Coupon Verify(COU.)");
			break;
		case TRANS_VOID_TEL_VERIFY: 
			strcpy(pszTransName1, "�ֻ�����ȯ����");
			strcpy(pszTransName2, "Void Coupon Verify(PHO.)");
			break;
		case TRANS_VOID_CARD_VERIFY: 
			strcpy(pszTransName1, "���п���ȯ����");
			strcpy(pszTransName2, "Void Coupon Verify(CAR.)");
			break;
		case TRANS_REFUND_VERIFY:
			strcpy(pszTransName1, "�˻�");
			strcpy(pszTransName2, "Refund");
			break;			
		case TRANS_PRECREATE:
			strcpy(pszTransName1, "΢��֧����ɨ");
			strcpy(pszTransName2, "Scan by wechat");
			break;
		case TRANS_VOID_PRECREATE:
			strcpy(pszTransName1, "΢��֧����ɨ����");
			strcpy(pszTransName2, "Void Scan by wechat");
			break;
		case TRANS_CREATEANDPAY:
			strcpy(pszTransName1, "΢��֧����ɨ");
			strcpy(pszTransName2, "Scan by POS(Wechat)");
			break;	
		case TRANS_VOID_CREATEANDPAY:
			strcpy(pszTransName1, "΢��֧����ɨ����");
			strcpy(pszTransName2, "Void Scan by POS");
			break;
		case TRANS_WX_REFUND:
			strcpy(pszTransName1, "΢���˻�");
			strcpy(pszTransName2, "Refund for Wechat");
			break;	
		case TRANS_CREATEANDPAYBAIDU:
			strcpy(pszTransName1, "�ٶ�Ǯ����ɨ");
			strcpy(pszTransName2, "Scan by POS(Baidu)");
			break;
		case TRANS_BAIDU_PRECREATE:
			strcpy(pszTransName1, "�ٶ�Ǯ����ɨ");
			strcpy(pszTransName2, "Scan by Baidu");
			break;
		case TRANS_BAIDU_REFUND:
			strcpy(pszTransName1, "�ٶ�Ǯ���˻�");
			strcpy(pszTransName2, "Refund for Baidu");
			break;	
		case TRANS_VOID_CREATEANDPAYBAIDU:
			strcpy(pszTransName1, "�ٶ�Ǯ����ɨ����");
			strcpy(pszTransName2, "Void for Baidu");
			break;	
		case TRANS_VOID_BAIDU_PRECREATE:
			strcpy(pszTransName1, "�ٶ�Ǯ����ɨ����");
			strcpy(pszTransName2, "Void for Baidu");
			break;	
		case TRANS_JD_PRECREATE:
			strcpy(pszTransName1, "����Ǯ����ɨ");
			strcpy(pszTransName2, "Scan by JD");
			break;
		case TRANS_JD_CREATEANDPAY:
			strcpy(pszTransName1, "����Ǯ����ɨ");
			strcpy(pszTransName2, "Scan by POS(JD)");
			break;
		case TRANS_JD_REFUND:
			strcpy(pszTransName1, "�����˻�");
			strcpy(pszTransName2, "Refund for JD");
			break;	
		case TRANS_VOID_JD_PRECREATE:
			strcpy(pszTransName1, "����Ǯ����ɨ����");
			strcpy(pszTransName2, "Void for JD");
			break;	
		case TRANS_VOID_JD_CREATEANDPAY:
			strcpy(pszTransName1, "����Ǯ����ɨ����");
			strcpy(pszTransName2, "Void for JD");
			break;	
		case TRANS_ALI_CREATEANDPAY:
			strcpy(pszTransName1, "֧������ɨ");
			strcpy(pszTransName2, "Alipay payment");
			break;	
		case TRANS_ALI_PRECREATE:
			strcpy(pszTransName1, "֧������ɨ");
			strcpy(pszTransName2, "Scan by Alipay");
			break;
		case TRANS_ALI_REFUND:
			strcpy(pszTransName1, "֧�����˻�");
			strcpy(pszTransName2, "Refund for Alipay");
			break;	
		case TRANS_VOID_ALI_CREATEANDPAY:
			strcpy(pszTransName1, "֧������ɨ����");
			strcpy(pszTransName2, "Void for Alipay");
			break;	
		case TRANS_VOID_ALI_PRECREATE:
			strcpy(pszTransName1, "֧������ɨ����");
			strcpy(pszTransName2, "Void for Alipay");
			break;	
		case TRANS_COUPON_MEITUAN:
			strcpy(pszTransName1, "���ųж�");
			strcpy(pszTransName2, "Meituan Coupon");
			break;
		case TRANS_VOID_MEITUAN:
			strcpy(pszTransName1, "���ų���");
			strcpy(pszTransName2, "Void for Meituan");
			break;	
		case TRANS_PANJINTONG:
			strcpy(pszTransName1, "�̽�֧ͨ��");
			strcpy(pszTransName2, "Panjin payment");
			break;
		case TRANS_DZ_PRECREATE:
			strcpy(pszTransName1, "���ڵ�����ɨ");
			strcpy(pszTransName2, "Scan by Dianping");
			break;
		case TRANS_DZ_CREATEANDPAY:
			strcpy(pszTransName1, "���ڵ�����ɨ");
			strcpy(pszTransName2, "Scan by POS(Dianping)");
			break;
		case TRANS_DZ_REFUND:
			strcpy(pszTransName1, "���ڵ����˻�");
			strcpy(pszTransName2, "Refund for Dianping");
			break;	
		case TRANS_VOID_DZ_PRECREATE:
			strcpy(pszTransName1, "���ڵ�����ɨ����");
			strcpy(pszTransName2, "Void for Dianping");
			break;	
		case TRANS_VOID_DZ_CREATEANDPAY:
			strcpy(pszTransName1, "���ڵ�����ɨ����");
			strcpy(pszTransName2, "Void for Dianping");
			break;	
		case TRANS_COUPON_DAZHONG:
			strcpy(pszTransName1, "���ڵ����ж�");
			strcpy(pszTransName2, "Dianping Coupon");
			break;
		case TRANS_BESTPAY_CREATEANDPAY:
			strcpy(pszTransName1, "��֧��");
			strcpy(pszTransName2, "Bestpay");
			break;
		case TRANS_BESTPAY_REFUND:
			strcpy(pszTransName1, "��֧���˻�");
			strcpy(pszTransName2, "Refund for Bestpay");
			break;
		case TRANS_VOID_BESTPAY_CREATEANDPAY:
			strcpy(pszTransName1, "��֧������");
			strcpy(pszTransName2, "Void for Bestpay");
			break;
		case TRANS_ALLPAY_CREATEANDPAY:
			sprintf(pszTransName1, "���ܸ�(%s)", pstWater->szTransName);
			strcpy(pszTransName2, "Allpay");
			break;
		case TRANS_ALLPAY_REFUND:
			sprintf(pszTransName1, "���ܸ��˻�(%s)", pstWater->szTransName);
			strcpy(pszTransName2, "Refund for Allpay");
			break;
		default:
			pszTransName1[0] = 0;
			pszTransName2[0] = 0;
			break;
	}
}

void GetTransNameForTP2(char cTransType, char *psTransAttrAndStatus, char *pszTransName1, char *pszTransName2)
{	

	switch( cTransType )
	{

		case TRANS_COUPON_VERIFY:
			strcpy(pszTransName1, "�Ż�ȯ�ж�");
			break;
		case TRANS_TEL_VERIFY:
			strcpy(pszTransName1, "�Ż�ȯ�ж�");
			break;
		case TRANS_CARD_VERIFY:
			strcpy(pszTransName1, "����");
			break;
		default:
			pszTransName1[0] = 0;
			pszTransName2[0] = 0;
			break;
	}
}


/**
* @brief ��ӡȫ����ˮ��ϸ
*
*    ͨ������_printallwater ʵ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintAllWater(void)
{
	int nWaterNum, nCurPrintRecNo=1;
	int nRet;
	
	//�����ܱ���
	GetWaterNum(&nWaterNum);
	if( nWaterNum<=0 )
	{
		PubMsgDlg("��ӡ������ϸ", "�޽���", 0, 5);
		return APP_SUCC;
	}
	
	while(1)
	{
		if(nCurPrintRecNo > nWaterNum)
		{
			break;
		}

		PubPrintFeedPaper(1);
		
		nRet = PubPrintCommit(SetAndPrint_Allwater, (void *)&nCurPrintRecNo, 0);
		if(nRet != APP_SUCC)
		{
			if (APP_FUNCQUIT == nRet)
			{
				PubMsgDlg("��ӡ������ϸ", "�޿ɴ�ӡ����", 1, 3);
			}
			return nRet;
		}
	}
	
	return APP_SUCC;
}


/**
* @brief ��ӡ���㵥
*
*    ͨ������_printsettle ʵ��
* @param in int nReprintFlag  ��ӡ���Ʊ�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintSettle(int nReprintFlag)
{	
	ASSERT_FAIL(PubPrintCommit( SetAndPrint_Settle, (void *)&nReprintFlag, 1));
	return APP_SUCC;
}

/**
* @brief ��ӡ��������
*
*    ͨ������_printsettle ʵ��
* @param in int nReprintFlag  ��ӡ���Ʊ�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintTotal()
{
	ASSERT(PubPrintCommit( SetAndPrint_Total, (void *)NULL, 1));
	return APP_SUCC;
}





int PrintParam(void)
{
	int nRet;
	int nSelect=0xff;
	int nType=0;

	while(1)
	{
		nRet = ProSelectList("1.�̻���Ϣ||2.ϵͳ����||3.ͨѶ����||4.�汾��Ϣ||5.����", "������ӡ", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return nRet;
		}

		switch(nSelect)
		{
		case 1:
			nType=PRINT_PARAM_TYPE_MERCHANTINFO;
			break;
#if 0			
		case 2:
			nType=PRINT_PARAM_TYPE_TRANSCCTRL;	
			break;
#endif			
		case 2:
			nType=PRINT_PARAM_TYPE_SYSTEMCTRL;
			break;
		case 3:
			nType=PRINT_PARAM_TYPE_COMM;
			break;
		case 4:
			nType=PRINT_PARAM_TYPE_VERSION;
			break;
		case 5:
			nType=PRINT_PARAM_TYPE_OTHER;
			break;
		default :                         
			continue;
		}

		ASSERT_FAIL(PubPrintCommit(SetAndPrint_Param, (void *)&nType, 1));
	}
	return APP_SUCC;
}


/**
* @brief �ش�ӡ������ˮ�ͽ��㵥
*    ��main.c ģ���еĺ���MenuFuncSel
* @param ��
* @return ��
*/
int Reprint()
{
	int nWaterNum;
	int nRet;
	STWATER stWater;
	int nSelect;
	while(1)
	{
		if (YES == GetVarReprintSettle())
		{
			ASSERT_QUIT(ProSelectList("1.�ش����һ��||2.�ش�����һ��||3.��ӡ������ϸ||4.��ӡ���׻���||5.�ش�ӡ���㵥", "��ӡ", 0xFF, &nSelect));
		}
		else
		{
			ASSERT_QUIT(ProSelectList("1.�ش����һ��||2.�ش�����һ��||3.��ӡ������ϸ||4.��ӡ���׻���", "��ӡ", 0xFF, &nSelect));		
		}
		switch (nSelect)
		{
		case 1:
			GetWaterNum(&nWaterNum);
			if (nWaterNum <=0 )
			{
				char szDispBuf[100];
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C�޽���");
				PubMsgDlg("�ش����һ��", szDispBuf, 0, 5);
				break;
			}
			FetchLastWater(&stWater);
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintWater(&stWater, REPRINT);
			break;			
		case 2:
			GetWaterNum(&nWaterNum);
			if (nWaterNum <=0 )
			{
				char szDispBuf[100];
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C�޽���");
				PubMsgDlg("�ش�����һ��", szDispBuf, 0, 5);
				break;
			}
			FindByInvoice(1);
			break;
		case 3:   
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintAllWater();
			break;		
		case 4:
			nRet = PubConfirmDlg("��ӡ���׻���", "��ȷ�ϼ���ӡ", 0, 5);
			if (nRet != APP_SUCC)
			{
				break;
			}
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintTotal();
			break;
		case 5:
			if (YES == GetVarReprintSettle())
			{
				nRet = PubConfirmDlg("��ӡ���㵥", "��ȷ�ϼ���ӡ", 0, 5);
				if (nRet != APP_SUCC)
				{
					break;
				}
				PubClearAll();
				DISP_PRINTING_NOW;
				if (APP_SUCC != PrintSettle(REPRINT))
				{
					PubMsgDlg(NULL, "  �޾ɽ�������", 0, 3);
				}
			}
			break;
		default:
			break;
		}
	}
	return APP_SUCC;
}

#if 0
static int Print_GetTransType(STWATER *pstWater, char* pcPrintType)
{
	switch (pstWater->cTransType)
	{
	case TRANS_SALE:
		if(pstWater->cTransAttr == ATTR_qPBOC || pstWater->cTransAttr == ATTR_PBOC_EC)
		{
			*pcPrintType = 'E';
		}
		else
			*pcPrintType = 'S';
		break;
	case TRANS_PHONE_SALE:
		*pcPrintType= 'S';
		break;
	case TRANS_REFUND:
	case TRANS_ALLIANCE_REFUND:
	case TRANS_REFUND_PHONE_SALE:
	case TRANS_EMV_REFUND:
		*pcPrintType = 'R';
		break;
	case TRANS_AUTHSALE:
	case TRANS_PHONE_AUTHSALE:
		*pcPrintType = 'P';
		break;
	case TRANS_AUTHSALEOFF:
	case TRANS_PHONE_AUTHSALEOFF:
		*pcPrintType = 'C';
		break;
	case TRANS_ADJUST:
		*pcPrintType = 'L';
		if (memcmp(pstWater->sFuncCode, "34", 2) == 0)/** �����׷��С�ѣ���С�Ѵ�ӡ*/
		{
			*pcPrintType = 'S';
		}
		break;
	case TRANS_OFFLINE:
		*pcPrintType = 'L';
		break;
	case TRANS_INSTALMENT:
		*pcPrintType = 'T';
		break;
	case TRANS_BONUS_IIS_SALE:
	case TRANS_BONUS_ALLIANCE:
		*pcPrintType = 'B';
		break;
	case TRANS_VOID_AUTHSALE:
		*pcPrintType = 'A';
		break;			
	case TRANS_VOID_SALE:
		*pcPrintType = 'V';
		break;
	case TRANS_COUPON_VERIFY:
	case TRANS_TEL_VERIFY:
	case TRANS_CARD_VERIFY:
		*pcPrintType = 'D';
		break;
	case TRANS_REFUND_VERIFY:
		*pcPrintType = 'R';
		break;
	default:
		*pcPrintType = ' ';
		return APP_FAIL;
	}
	return APP_SUCC;
}
#endif
static void DealPrintAmount(char cTransType, char *pszAmount)
{		
	char szAmt[20] = {0};
	
	if(GetVarIsPrintPrintMinus()==YES)
	{
		switch(cTransType)
		{
		case TRANS_VOID_SALE:				/**< ���ѳ���*/
		case TRANS_VOID_AUTHSALE: 			/**< ��ɳ���*/
		case TRANS_VOID_AUTHSETTLE: 		/**< ���㳷��*/
		case TRANS_VOID_PREAUTH:			/**< ��Ȩ����*/
		case TRANS_VOID_REFUND:				/**< �����˻�*/
		case TRANS_VOID_INSTALMENT: 		/**< ��������*/
		case TRANS_VOID_BONUS_IIS_SALE: 	/**< ���������л�������*/
		case TRANS_VOID_BONUS_ALLIANCE: 	/**< �������˻�������*/
		case TRANS_VOID_PHONE_SALE: 		/**< �����ֻ�оƬ����*/
		case TRANS_VOID_PHONE_PREAUTH:		/**< �����ֻ�оƬԤ��Ȩ*/
		case TRANS_VOID_PHONE_AUTHSALE: 	/**< �����ֻ��������*/
		case TRANS_EMV_REFUND:				/**< EMV�ѻ��˻�*/
		case TRANS_REFUND:  				/**< �˻�*/
		case TRANS_ALLIANCE_REFUND:			/**< ���˻����˻�*/
		case TRANS_REFUND_PHONE_SALE:		/**< �ֻ�оƬ�˻�*/
		case TRANS_VOID_COUPON_VERIFY:		/**<����������ȯ*/
		case TRANS_VOID_TEL_VERIFY:			/**<�����ֻ���ȯ*/
		case TRANS_VOID_CARD_VERIFY: 		/**<�������п���ȯ*/
		case TRANS_REFUND_VERIFY:			/**<��֤�˻�*/
			sprintf(szAmt, "-%s", pszAmount);
			strcpy(pszAmount, szAmt);
			break;
		default:
			break;
		}
	}
}

#if 0
/*���ܺ�GetBankName���ƣ����ڻ�ȡ�������������ơ�
��ͬ���ǵ��Ҳ�����������Ϣ��ʱ�����������δ֪
��������Ϣ����ȡ��ֵ��������������*/
static void GetCardBankName(const char *pszBankCode,char *pszBankName, char cFlag)
{
	int i=0;
	char szBankCode[11+1] = {0};	/**< ���б���*/
	char szLocalCode[11+1] = {0};	/**< ���ش���*/
	char szUnknowBankInfo[20+1]={0};
	
	GetVarBusinessBankId(szBankCode);	 /**<��ȡ���д��룬����4λ*/
	GetVarLocalcode(szLocalCode);			/**<��ȡ���������룬����4λ*/
	while(1)
	{
		if(!memcmp(pszBankCode, gszBankName[i], 4))
		{
			break;
		}
		i++;
		if(!memcmp(gszBankName[i], "0000", 4))
		{
			if (memcmp(pszBankCode, szBankCode, 4) == 0)
			{
				memcpy(pszBankName,"��ҵ����",8);
			}
			else
			{
				GetVarUnknowBankInfo(szUnknowBankInfo);
				if(strlen(szUnknowBankInfo)>0)
				{
					memcpy(pszBankName,szUnknowBankInfo, 11);
				}
				else
				{
					memcpy(pszBankName,pszBankCode, 11);
				}
			}
			return;
		}
	}
	if (memcmp(pszBankCode, szBankCode, 4) == 0)
	{
		memcpy(pszBankName,"��ҵ����",8);
		return;
	}
	
	// ��鷢���д����뱾�ش���ķ��������������μ��������񲹳�淶
	if (cFlag)
	{
		if (memcmp(szLocalCode,"0000",4) == 0)
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		else if ((i<15)&&(memcmp(pszBankCode+4,szLocalCode,2) != 0))
		{
			memcpy(pszBankName,gszBankName[i]+4,4);
			memcpy(pszBankName+4,"  ����",6);
		}
		else
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		return ;
	}

	memcpy(pszBankName, gszBankName[i]+4+4, 10);
	return ;
}
#endif

/**
* @brief �����ṩ�����д��룬ȡ�����ж�Ӧ����������
* @param in char *pszBankCode ���д���
* @param out char *pszBankName ������������
* @param in char cFlag 	����������־
* @return ��
*/
void GetBankName(const char *pszBankCode,char *pszBankName, char cFlag)
{
	int i=0;
	char szBankCode[11+1] = {0};	/**< ���б���*/
	char szLocalCode[11+1] = {0};	/**< ���ش���*/

	GetVarBusinessBankId(szBankCode);	 /**<��ȡ���д��룬����4λ*/
	GetVarLocalcode(szLocalCode);			/**<��ȡ���������룬����4λ*/
	while(1)
	{
		if(!memcmp(pszBankCode, gszBankName[i], 4))
		{
			break;
		}
		i++;
		if(!memcmp(gszBankName[i], "0000", 4))
		{
			if (memcmp(pszBankCode, szBankCode, 4) == 0)
			{
				memcpy(pszBankName,"��ҵ����",8);
			}
			else
			{
				memcpy(pszBankName, pszBankCode, 11);	
			}
			return;
		}
	}
	if (memcmp(pszBankCode, szBankCode, 4) == 0)
	{
		memcpy(pszBankName,"��ҵ����",8);
		return;
	}
	
	// ��鷢���д����뱾�ش���ķ��������������μ��������񲹳�淶
	if (cFlag)
	{
		if (memcmp(szLocalCode,"0000",4) == 0)
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		else if ((i<15)&&(memcmp(pszBankCode+4,szLocalCode,2) != 0))
		{
			memcpy(pszBankName,gszBankName[i]+4,4);
			memcpy(pszBankName+4,"  ����",6);
		}
		else
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		return ;
	}

	memcpy(pszBankName, gszBankName[i]+4+4, 10);
	return ;
}


/**
* @brief �����ӡ����(��Դ)
* @param ��
* @return
* @li APP_SUCC 
* @li APP_FAIL
* @li APP_QUIT
* @author 
* @date
*/
int DealPrintLimit(void)
{
	ST_POWER_INFO stPowerInfo;
	char szVer[64]={0};

	NDK_Getlibver(szVer);
	
	if ((strcmp(szVer, "4.0.0") >= 0) && szVer[0] != 'V')
	{
		memset(&stPowerInfo, 0, sizeof(ST_POWER_INFO));
		NDK_SysGetPowerInfo(&stPowerInfo);
		
		if (stPowerInfo.unPowerType & (1<<0))
			return APP_SUCC;
		if ((stPowerInfo.unPowerType & (1<<1)) && stPowerInfo.unBattryPercent <= 10)
		{
			PubMsgDlg("��ܰ��ʾ", "��������,����!", 3, 10);
			return APP_QUIT;
		}
	}
	
	return APP_SUCC;
}



/**
*	��ͳ���׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntTraditionOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=11;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[11][17]={"ISSALE","ISVOIDSALE","ISREFUND","ISBALANCE","ISPREAUTH",
		"ISVOIDPREAUTH","ISAUTHSALE","ISAUTHSALEOFF",
		"ISVOIDAUTHSALE","ISOFFLINE","ISREJUST"};	

				
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);

	}
}


/**
*	�����ֽ��׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntECashOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=7;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[7][17]={"ISECSALE","ISECFASTSALE",
		"ISECLOAD","NOTBINDECLOAD","ISECLOADCASH",
		"ISECVOIDLOADCASH","ISECOFFREFUND"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue(szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��",6 );	
	}
}


/**
*	����Ǯ�����׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/

void PntWalletOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=4;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[4][17]={"ISEPSALE","ISEPLOAD",
		"ISEPNOTBINDLOAD","ISEPCASHLOAD"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);

	}
}


/**
*	���ڸ���׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/

void PntInstallmentOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[2][17]={"ISINSTALL","ISVOIDINSTALL"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}

/**
*	���ֽ��׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntBonusOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=6;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[6][17]={"BONUS_ALLIANCE","BONUS_IIS_SALE","VOIDBONUS_ALL",
		"VOID_BONUS_IIS","ALLIANCE_BALANCE","ALLIANCE_REFUND"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}


/**
*	�ֻ�оƬ���׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntPhoneChipSaleOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=9;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[9][17]={"ISPHONESALE","ISVOIDPHONESALE","ISREFUNDPHONE",
		"ISPHONEPREAUTH","ISPHONEVOIDPRE","ISPHONEAUTH",
		"ISPHONEAUTHOFF","ISPHONEVOIDAUTH","ISPHONEBALANCE"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}


/**
*	ԤԼ���׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntAppointmentOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[2][17]={"ISAPPOINT","ISVOIDAPPOINT"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}


/**
*	�������׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntOrderOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=8;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[8][17]={"ISORDERSALE","ISORDERVOIDSALE","ISREFUNDORDER",
		"ISORDERPREAUTH","ISORDERVOIDPRE","ISORDERAUTHSALE",
		"ISORDERAUTHOFF","ISORDERVOIDAUTH"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}

/**
* @�������׿������ô�ӡ
* @param TransFlag ֧�ֵĽ��ױ�־��
* @return ��
*/
void PntOtherOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char szName[2][17]={"ISSTRIPECASHLOAD","ISSTRIPELOAD"};
	for(i=0;i<nTransNum;i++)
	{	
		/**<�������Ϊ�մ�������ӡ*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue(szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "֧��" : "��֧��" ,6);
	}
}

#if 0
int SetPrintData_Water(const STWATER *pstWater, int nReprintFlag, const int nCurrentPage)
{
    int	nPage=3;				/**< ��ӡ�ĵ�������*/
	int nMaxLine = 14;
	int nLen = 0;
	uint unLogoXpos = 0;
	char szShopname[40+1] = {0};
	char szPrintName[100+1] = {0};
	char szShopid[15+1] = {0};
	char szTerminalid[8+1] = {0};
	char szTmpStr[80];
	char szStr[30+1];
	char szPosYear[4+1] = {0};			/**< �������*/
	char szTransAttrAndStatus[2];
	char szStripeType[2];
	char szDispAmt[14+1] = {0};
	char szTmpRefNum[12+1] = {0};
	char szHotLine[20+1] = {0};
	char szSoftVer[16+1] = {0};
	char cSTUB = 0;	// 1�̻���2���С�3�ֿ��˴����־

	szTransAttrAndStatus[0] = pstWater->cTransAttr;
	szTransAttrAndStatus[1] = pstWater->cEMV_Status;

	PubGetCurrentDatetime(szTmpStr);
	memcpy(szPosYear, szTmpStr, 4);
	
	PubClearFieldValue();
	GetVarTerminalId(szTerminalid);
	GetVarMerchantId(szShopid);
	GetVarMerchantNameCn(szShopname);
	GetFunctionPntTitle(szPrintName);
	GetFunctionHotLineNo(szHotLine);		/**< ������ߺ���*/
	GetVarDispSoftVer(szSoftVer);
	GetVarPrintPageCount(&nPage);	/**< ��ô�ӡ��������*/

	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	if(GetIsPntTitleMode()==YES)			/**< ��ӡLOGO*/
	{		
		GetPrnLogo(szTmpStr, &unLogoXpos);
		PubSetParamLogo("LOGOTAG", szTmpStr, unLogoXpos);
	}
	else
	{
		Cup_SetPrnTitle(szPrintName, szPrintName);
		PubSetFieldValue("PRINTNAME", szPrintName, strlen(szPrintName));
	}

	/*��ֵҵ��ǩ����*/
	
	PubSetFieldValue("PRINTMITENONAME", "��ֵҵ��ǩ����", strlen("��ֵҵ��ǩ����"));
	
	if ((nReprintFlag == REVERSAL_PRINT) || (nReprintFlag == OFFLINE_PRINT))
	{
		//�Ƿ��ӡ ���ϱ��浥
		PubSetFieldValue("CISPRINTFAIL", "1", 1);
	}

	switch(nCurrentPage)
	{
	case 2:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//���д��
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	case 1:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//  �ֿ��˴��
			PubSetFieldValue("STUB", "3", 1);
			cSTUB = 3;
		}
		break;
	default:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//���д��
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	}

	//�̻�����
	PubAllTrim(szShopname);

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nMaxLine = 20;
	}
	if(strlen(szShopname) <= nMaxLine)
	{
		//���ӡ�Ƶ�����
		PubSetFieldValue("ISUNBLANK", "1", 1);
	}
	PubSetFieldValue("MERCHANTNAME",  szShopname, strlen(szShopname));
	// �̻���
	PubSetFieldValue("SHOPID",  szShopid, strlen(szShopid));
	// POS���
	PubSetFieldValue("TERMINALID", szTerminalid, strlen(szTerminalid));
	//����Ա���
	PubSetFieldValue("OPERID", pstWater->szOper, strlen(pstWater->szOper));


	

	// ����
	memset(szTmpStr, 0, sizeof(szTmpStr));
	PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szTmpStr);
	CtrlCardPan((uchar*)szTmpStr, pstWater->nPanLen, pstWater->cTransType, pstWater->cEMV_Status);
	GetStripeType(pstWater->szInputMode, pstWater->cTransType, pstWater->cTransAttr,szStripeType);
	PubSetFieldValueFormat("CARDNO", "%s /%s", szTmpStr, szStripeType);	

	
	PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));

	// ��������
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));

	GetTransNameForTP( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_CN", "%s", szStr);
	PubSetFieldValueFormat("TRANSTYPE_EN", "%s(%s)", szStr, szTmpStr);

	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	GetTransNameForTP2( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_MITENO_CN", "%s", szStr);
	PubSetFieldValueFormat("TRANSTYPE_MITENO_EN", "%s(%s)", szStr, szTmpStr);

	
	// ��Ч��
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2))
	{	
		PubSetFieldValueFormat( "EXPDATE", "%2.2s%02x/%02x", szPosYear, pstWater->sExpDate[0], pstWater->sExpDate[1] );
	}
	
	//���κ�
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0,  (uchar *)szTmpStr);
	PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));
	//ƾ֤��
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0,  (uchar *)szTmpStr+10);	
	PubSetFieldValue("TRACENO", szTmpStr + 10, strlen(szTmpStr + 10));

	// ��Ȩ��
	if (memcmp(pstWater->szAuthCode, "\x00\x00\x00\x00\x00\x00", 6))
	{
		PubSetFieldValue("AUTHCODE", pstWater->szAuthCode, strlen(pstWater->szAuthCode));
	}
	if (0 == strlen(pstWater->szRefnum))
	{
		memset(szTmpRefNum, 0, sizeof(szTmpRefNum));
		memset(szTmpRefNum, ' ', 12);
	}
	else
	{
		memcpy(szTmpRefNum, pstWater->szRefnum, 12);
	}
	//ϵͳ�ο���
	PubSetFieldValue("REFNUM", szTmpRefNum, 12);

	// �������ڡ�ʱ��
	PubSetFieldValueFormat( "DATETIME", "%4.4s/%02x/%02x %02x:%02x:%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1], pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	PubSetFieldValueFormat( "DATE", "%4.4s/%02x/%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1] );
	PubSetFieldValueFormat( "TIME",  "%02x:%02x:%02x", pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	// ��ӡ���׽��
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
	memset(szDispAmt, 0, sizeof(szDispAmt));
	ProAmtToDispOrPnt(szStr, szDispAmt);
	PubAllTrim(szDispAmt);
	DealPrintAmount(pstWater->cTransType, szDispAmt);
	PubSetFieldValueFormat( "TOTALAMOUNT", "RMB %s", szDispAmt);	

	memset(szDispAmt, 0, sizeof(szDispAmt));
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
	PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

	ProAmtToDispOrPnt(szTmpStr, szDispAmt);
	PubAllTrim(szDispAmt);
	PubSetFieldValueFormat( "REALAMOUNT", "RMB %s", szDispAmt);	

	/*���п����ѽ��*/
	memset(szDispAmt, 0, sizeof(szDispAmt));
	sprintf(szDispAmt, "%012d", atol(szStr)-atol(szTmpStr));
	ProAmtToDispOrPnt(szDispAmt, szDispAmt);
	PubAllTrim(szDispAmt);
	PubSetFieldValueFormat("CARDAMOUNT", "RMB %s", szDispAmt);	

	//  �ش��־
	if (nReprintFlag==REPRINT)
	{
		//�ش�ӡƾ֤
		PubSetFieldValue("ISREPRINT", "1", 1);
	}
	if(nReprintFlag==REVERSAL_PRINT)
	{
		//�������ɹ�,���˹�����
		PubSetFieldValue("ISREVERSALFAIL", "1", 1);
	}
	
	if(nReprintFlag==OFFLINE_PRINT)
	{
		//"���Ͳ��ɹ�,���˹�����
		PubSetFieldValue("ISSENDFAIL", "1", 1);
	}

	/*
		����Ҫ�����ô�ӡ3��ǩ����ʱ��ֻ���̻���ǩ������������ǩ������
		�����ֿ���ǩ���ռ䣬�ֿ�����ǩ�����������ֿ���ǩ���ռ䡣
	*/
	if (cSTUB != 3)
	{
		//�ֿ���ǩ��
		//����ȷ�����Ͻ��ף�ͬ�⽫����뱾���˻�
		PubSetFieldValue("ISCARDSIGN", "1", 1); 
		PubSetFieldValue("ISFEEDLINE", "1", 1); //��ӡ�հ׻���
	}	

	if(strlen(szHotLine) > 0)
	{
		PubSetFieldValueFormat( "HOTLINE",  "%s", szHotLine);
	}
	memset(szTmpStr,0,sizeof(szTmpStr));
	sprintf(szTmpStr, "%s-%-8.8s", PubGetPosTypeStr(), szSoftVer);
	PubSetFieldValueFormat( "POSTYPEANDVER",  "%s", szTmpStr);
	return APP_SUCC;
}

#endif

int SetPrintData_Water(const STWATER *pstWater, int nReprintFlag, const int nCurrentPage)
{
    int	nPage=3;				/**< ��ӡ�ĵ�������*/
	int nMaxLine = 14;
	uint unLogoXpos = 0;
	char szShopname[40+1] = {0};
	char szPrintName[100+1] = {0};
	char szShopid[15+1] = {0};
	char szTerminalid[8+1] = {0};
	char szTmpStr[80];
	char szStr[30+1],szResultAmt[32];
	char szPosYear[4+1] = {0};			/**< �������*/
	char szTransAttrAndStatus[2];
	char szStripeType[2];
	char szDispAmt[14+1] = {0};
	char szTmpRefNum[12+1] = {0};
	char szHotLine[20+1] = {0};
	char szSoftVer[16+1] = {0};
	char cSTUB = 0;	// 1�̻���2���С�3�ֿ��˴����־

	szTransAttrAndStatus[0] = pstWater->cTransAttr;
	szTransAttrAndStatus[1] = pstWater->cEMV_Status;

	PubGetCurrentDatetime(szTmpStr);
	memcpy(szPosYear, szTmpStr, 4);
	
	PubClearFieldValue();
	GetVarTerminalId(szTerminalid);
	GetVarMerchantId(szShopid);
	GetVarMerchantNameCn(szShopname);
	GetFunctionPntTitle(szPrintName);
	GetFunctionHotLineNo(szHotLine);		/**< ������ߺ���*/
	GetVarDispSoftVer(szSoftVer);
	GetVarPrintPageCount(&nPage);	/**< ��ô�ӡ��������*/

	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	if(GetIsPntTitleMode()==YES)			/**< ��ӡLOGO*/
	{		
		GetPrnLogo(szTmpStr, &unLogoXpos);
		PubSetParamLogo("LOGOTAG", szTmpStr, unLogoXpos);
	}
	else
	{
		Cup_SetPrnTitle(szPrintName, szPrintName);
		PubSetFieldValue("PRINTNAME", szPrintName, strlen(szPrintName));
	}

	/*��ֵҵ��ǩ����*/
	
	PubSetFieldValue("PRINTMITENONAME", "��ֵҵ��ǩ����", strlen("��ֵҵ��ǩ����"));
	
	if ((nReprintFlag == REVERSAL_PRINT) || (nReprintFlag == OFFLINE_PRINT))
	{
		//�Ƿ��ӡ ���ϱ��浥
		PubSetFieldValue("CISPRINTFAIL", "1", 1);
	}

	switch(nCurrentPage)
	{
	case 2:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//���д��
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	case 1:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//  �ֿ��˴��
			PubSetFieldValue("STUB", "3", 1);
			cSTUB = 3;
		}
		break;
	default:
		if (nCurrentPage == nPage)
		{
			// �̻����
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//���д��
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	}

	//�̻�����
	PubAllTrim(szShopname);

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nMaxLine = 20;
	}
	if(strlen(szShopname) <= nMaxLine)
	{
		//���ӡ�Ƶ�����
		PubSetFieldValue("ISUNBLANK", "1", 1);
	}
	PubSetFieldValue("MERCHANTNAME",  szShopname, strlen(szShopname));
	// �̻���
	PubSetFieldValue("SHOPID",  szShopid, strlen(szShopid));
	// POS���
	PubSetFieldValue("TERMINALID", szTerminalid, strlen(szTerminalid));
	//����Ա���
	PubSetFieldValue("OPERID", pstWater->szOper, strlen(pstWater->szOper));

	switch(pstWater->cTransType)
	{
		case TRANS_COUPON_VERIFY:		
			PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));
			break;
		case TRANS_TEL_VERIFY:
			PubSetFieldValue("TELNONO", pstWater->sTelNo, strlen(pstWater->sTelNo));
			break;
		case TRANS_CARD_VERIFY:
			memset(szTmpStr, 0, sizeof(szTmpStr));
			PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szTmpStr);
			CtrlCardPan((uchar*)szTmpStr, pstWater->nPanLen, pstWater->cTransType, pstWater->cEMV_Status);
			GetStripeType(pstWater->szInputMode, pstWater->cTransType, pstWater->cTransAttr,szStripeType);
			PubSetFieldValueFormat("CARDNO", "%s /%s", szTmpStr, szStripeType); 
			break;
		case TRANS_COUPON_MEITUAN:		
			PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));
			break;
		case TRANS_VOID_VERIFY:
			break;
		case TRANS_REFUND_VERIFY:
			break;
	}
 	// ��������
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));

	GetTransNameForTP( pstWater, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_CN", "%s", szStr);		
	PubSetFieldValueFormat("TRANSTYPE_EN", "%s(%s)", szStr, szTmpStr);

	//GetTransNameForTP2( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	//PubSetFieldValueFormat("TRANSTYPE_MITENO_CN", "%s", szStr);
 	
	// ��Ч��
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2))
	{	
		PubSetFieldValueFormat( "EXPDATE", "%2.2s%02x/%02x", szPosYear, pstWater->sExpDate[0], pstWater->sExpDate[1] );
	}
	
	//���κ�
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0,  (uchar *)szTmpStr);
	PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));
	//ƾ֤��
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0,  (uchar *)szTmpStr+10);	
	PubSetFieldValue("TRACENO", szTmpStr + 10, strlen(szTmpStr + 10));

	// ��Ȩ��
	if (memcmp(pstWater->szAuthCode, "\x00\x00\x00\x00\x00\x00", 6))
	{
		PubSetFieldValue("AUTHCODE", pstWater->szAuthCode, strlen(pstWater->szAuthCode));
	}
	if (0 == strlen(pstWater->szRefnum))
	{
		memset(szTmpRefNum, 0, sizeof(szTmpRefNum));
		memset(szTmpRefNum, ' ', 12);
	}
	else
	{
		memcpy(szTmpRefNum, pstWater->szRefnum, 12);
	}
	//ϵͳ�ο���
	PubSetFieldValue("REFNUM", szTmpRefNum, 12);

	// �������ڡ�ʱ��
	PubSetFieldValueFormat( "DATETIME", "%02x%02x/%02x/%02x %02x:%02x:%02x", pstWater->sDate[0], pstWater->sDate[1], pstWater->sDate[2], pstWater->sDate[3],pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	PubSetFieldValueFormat( "DATE", "%4.4s/%02x/%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1] );
	PubSetFieldValueFormat( "TIME",  "%02x:%02x:%02x", pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);

	// ��ӡ���׽��
	if (pstWater->cTransType == TRANS_COUPON_DAZHONG)
	{
		memset(szStr, 0, sizeof(szStr));
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);

		PubSetFieldValueFormat( "COUPONCOUNT", "%d��", atoi(szStr));
	}

	if (pstWater->cTransType== TRANS_COUPON_VERIFY/**<��׼EMV���̽���*/
			|| pstWater->cTransType == TRANS_TEL_VERIFY
			|| pstWater->cTransType == TRANS_CARD_VERIFY)
	{
		memset(szDispAmt, 0, sizeof(szDispAmt));
		memset(szStr, 0, sizeof(szStr));
		memset(szTmpStr, 0, sizeof(szTmpStr));
		
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
		PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

		ProAmtToDispOrPnt(szTmpStr, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "REALAMOUNT", "RMB %s", szDispAmt);	

		memset(szDispAmt, 0, sizeof(szDispAmt));
		memset(szStr, 0, sizeof(szStr));
		memset(szTmpStr, 0, sizeof(szTmpStr));
		
		PubHexToAsc((uchar *)pstWater->sPayableAmount, 12, 0, (uchar *)szTmpStr);

		ProAmtToDispOrPnt(szTmpStr, szDispAmt);
		PubAllTrim(szDispAmt);
		
		PubSetFieldValueFormat("CARDAMOUNT", "RMB %s", szDispAmt);	
		/*Ӧ�����*/
		PubSetFieldValueFormat("PAYABLEAMOUNT", "RMB %s", szDispAmt);	

	}
	else
	{
		//�ܽ��
		memset(szStr, 0, sizeof(szStr));
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szStr, szDispAmt);
		PubAllTrim(szDispAmt);
		DealPrintAmount(pstWater->cTransType, szDispAmt);
		PubSetFieldValueFormat( "TOTALAMOUNT", "RMB %s", szDispAmt);

		//ʵ�����
		memset(szTmpStr, 0, sizeof(szTmpStr));
		PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

		if (atoi(szTmpStr)>0)
		{
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt(szTmpStr, szDispAmt);
			PubAllTrim(szDispAmt);
			PubSetFieldValueFormat( "CUSTOMERAMT", "RMB %s", szDispAmt);	

			//�Żݽ��
			memset(szResultAmt, 0, sizeof(szResultAmt));
			PubAscSubAsc((uchar *)szStr, (uchar *)szTmpStr, (uchar *)szResultAmt);

			if (atoi(szResultAmt)>0)
			{
				memset(szDispAmt, 0, sizeof(szDispAmt));
				ProAmtToDispOrPnt(szResultAmt, szDispAmt);
				PubAllTrim(szDispAmt);
				PubSetFieldValueFormat( "DISCOUNTAMT", "RMB %s", szDispAmt);
			}
		}


	}

	//  �ش��־
	if (nReprintFlag==REPRINT)
	{
		//�ش�ӡƾ֤
		PubSetFieldValue("ISREPRINT", "1", 1);
	}
	if(nReprintFlag==REVERSAL_PRINT)
	{
		//�������ɹ�,���˹�����
		PubSetFieldValue("ISREVERSALFAIL", "1", 1);
	}
	
	if(nReprintFlag==OFFLINE_PRINT)
	{
		//"���Ͳ��ɹ�,���˹�����
		PubSetFieldValue("ISSENDFAIL", "1", 1);
	}

	if(strlen(pstWater->szOutTradeNo) > 0)
	{
		PubSetFieldValueFormat("OUTTRADENO", "%s", pstWater->szOutTradeNo);	
		printYiwei(pstWater->szOutTradeNo);
		
	}
	
	if (pstWater->cTransType== TRANS_COUPON_VERIFY/**<��׼EMV���̽���*/
			|| pstWater->cTransType == TRANS_TEL_VERIFY
			|| pstWater->cTransType == TRANS_CARD_VERIFY)
	{
		STCOUPON *pstAddition = (STCOUPON *)pstWater->sAddition;

		#if 0
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetCouponIss(szTmpStr, pstAddition->ucCouponISS);
		GetCouponType(szTmpStr+strlen(szTmpStr), NULL, pstAddition->ucCouponType);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
		memset(szTmpStr, 0, sizeof(szTmpStr));
		PubHexToAsc(pstAddition->usAmount,10, 0,szTmpStr);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		sprintf(szDispAmt, "%012d", atol(szTmpStr));
		ProAmtToDispOrPnt(szDispAmt, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat("COUPONAMOUNT", "RMB %s", szDispAmt);	
		#endif
		
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memcpy(szTmpStr, pstAddition->usCouponName, 20);
		PubAllTrim(szTmpStr);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
		//printYiwei(szTmpStr);
		
	}
	else if(pstWater->cTransType== TRANS_COUPON_MEITUAN)
	{
		STCOUPONMEITUAN *pstAddition = (STCOUPONMEITUAN *)pstWater->sAddition;
		
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memcpy(szTmpStr, pstAddition->usCouponName, 20);
		PubAllTrim(szTmpStr);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
	}
	
	if (strlen(pstWater->szAdvertisement) > 0)
	{
		PubSetFieldValueFormat("ADINFO", "%s", pstWater->szAdvertisement);
	}

	/*
		����Ҫ�����ô�ӡ3��ǩ����ʱ��ֻ���̻���ǩ������������ǩ������
		�����ֿ���ǩ���ռ䣬�ֿ�����ǩ�����������ֿ���ǩ���ռ䡣
	*/
	if (cSTUB != 3)
	{
		//�ֿ���ǩ��
		//����ȷ�����Ͻ��ף�ͬ�⽫����뱾���˻�
		PubSetFieldValue("ISCARDSIGN", "1", 1); 
		PubSetFieldValue("ISFEEDLINE", "1", 1); //��ӡ�հ׻���
	}	

	if(strlen(szHotLine) > 0)
	{
		PubSetFieldValueFormat( "HOTLINE",  "%s", szHotLine);
	}
	memset(szTmpStr,0,sizeof(szTmpStr));
	sprintf(szTmpStr, "%s-%-8.8s", PubGetPosTypeStr(), szSoftVer);
	PubSetFieldValueFormat( "POSTYPEANDVER",  "%s", szTmpStr);
	return APP_SUCC;
}


int SetAndPrint_Settle(void *ptrPara)
{
	struct {
		char OperNo[OPERCODELEN+1];	/**<�������Ա���*/
		char BatchNo[6+1];			/**<�������κ�*/
		char BatchDatetime[14+1];	/**<��������ʱ��*/
		char SettleFlag[2];			/**<�����ʶ��'0'����ƽ,'1'���ʲ�ƽ, SettleFlag[0]�ڿ���SettleFlag[1]�⿨*/
		STSETTLE stSettle[2];			/**<stSettle[0] �洢�ڿ�������Ϣ,stSettle[1] �洢�⿨������Ϣ*/
	} ST_SettleRec;
	STRECFILE stSettleFile;
	char szTmpstr[41];
	int i;
	char szTmpTag[MAX_PRINT_TAG_NAME + 1] = {0};
	int *pnReprintFlag = (int *)ptrPara;
	char *pszModuleName = "PRINT_SETTLE";
	uchar szAmt[13] = {0};
	uchar szAmt1[13] = {0};
	uchar szAmt2[13] = {0};
	char szDispAmt[14+1] = {0};
	uchar szAmtSum[13];
	uchar szAmtSumTemp[14+1];
	ulong _NumSum;

	PubClearFieldValue();

	if (*pnReprintFlag==REPRINT)
	{
		if (YES == GetVarReprintSettle())
		{
			ASSERT_FAIL(PubReadOneRec(SETTLEFILENAME, 1, (char *)(&ST_SettleRec)));
		}
	}
	else
	{
		GetCurrentOper(ST_SettleRec.OperNo, NULL, NULL);	/**< �������Ա��*/
		GetVarBatchNo(ST_SettleRec.BatchNo);				/**< �������κ�*/
		
		/**< ��������*/
		/**< ����ʱ��*/
		PubGetCurrentDatetime(ST_SettleRec.BatchDatetime);
		GetVarSettleDateTime(szTmpstr);
		if (memcmp(szTmpstr, "\x00\x00\x00\x00\x00", 5) == 0)
		{
			;
		}
		else
		{
			PubHexToAsc((uchar *)szTmpstr, 10, 0, (uchar *)(ST_SettleRec.BatchDatetime+4));
		}
		/**< �ڿ������ʶ*/
		ST_SettleRec.SettleFlag[0] = GetVarCnCardFlag();
		/**< �⿨�����ʶ*/		
		ST_SettleRec.SettleFlag[1] = GetVarEnCardFlag();
		GetSettleDataNK(&(ST_SettleRec.stSettle[0])); 			/**< �ڿ���������*/
		GetSettleDataWK(&(ST_SettleRec.stSettle[1]));			/**< �⿨��������*/
		if (YES == GetVarReprintSettle()) /**<֧���ش���㵥��ʱ����Ҫ�������Ϣ*/
		{
			// �������������ļ��������浱ǰ��������
			strcpy( stSettleFile.szFileName, SETTLEFILENAME );
			stSettleFile.cIsIndex = FILE_NOCREATEINDEX;						
			stSettleFile.unMaxOneRecLen = sizeof(ST_SettleRec);
			stSettleFile.unIndex1Start =1;
			stSettleFile.unIndex1Len =  1;
			stSettleFile.unIndex2Start =  1;
			stSettleFile.unIndex2Len = 1;
			ASSERT(PubCreatRecFile(&stSettleFile));
			ASSERT(PubAddRec(SETTLEFILENAME, (char *)(&ST_SettleRec)));
		}
	}
 
	//�̻�����
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarMerchantNameCn(szTmpstr);
	PubSetFieldValue("MERCHANTNAME",  szTmpstr, strlen(szTmpstr));
	// �̻���
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarMerchantId(szTmpstr);
	PubSetFieldValue("SHOPID",  szTmpstr, strlen(szTmpstr));
	
	// POS���
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarTerminalId(szTmpstr);
	PubSetFieldValue("TERMINALID", szTmpstr, strlen(szTmpstr));
	//����Ա���
	PubSetFieldValue("OPERID", ST_SettleRec.OperNo, OPERCODELEN);

	// ���κ�
	PubSetFieldValue("BATCHNO", ST_SettleRec.BatchNo, 6);

	// ����ʱ�������
	PubSetFieldValueFormat( "DATETIME", "%4.4s/%2.2s/%2.2s  %2.2s:%2.2s:%2.2s", 
		ST_SettleRec.BatchDatetime, ST_SettleRec.BatchDatetime+4, ST_SettleRec.BatchDatetime+6, 
		ST_SettleRec.BatchDatetime+8, ST_SettleRec.BatchDatetime+10, ST_SettleRec.BatchDatetime+12);

	//0�ڿ�1�⿨	
	for(i=0; i<1; i++)		/**< i=0 ����ҿ�, i=1 �⿨*/
	{
		memcpy(szAmtSum,"000000000000", 12);
		szAmtSum[12] = 0;
		memcpy(szAmtSumTemp,"000000000000", 12);
		szAmtSumTemp[12] = 0;
		
		_NumSum = 0;
		sprintf(szTmpTag, "ISSETTLEOK%d", i);
		if (ST_SettleRec.SettleFlag[i] == '2')
		{
			//���˲�ƽ
			PubSetFieldValue(szTmpTag, "2", 1);
		}
		else if (ST_SettleRec.SettleFlag[i] == '1')
		{
			//����ƽ
			PubSetFieldValue(szTmpTag, "1", 1);
		}
		else
		{
			//���˴�
			PubSetFieldValue(szTmpTag, "3", 1);
		}

		if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && ST_SettleRec.stSettle[i]._CouponVerifyNum > 0)
		{
			//������֤
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CouponVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "COUPONNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._CouponVerifyNum, szDispAmt);
				
			memcpy(szAmtSum,szAmt,12 );
			_NumSum += ST_SettleRec.stSettle[i]._CouponVerifyNum;

			//����жҳ���
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidCouponVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCOUPONNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCouponVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCouponVerifyNum, szDispAmt);
			}
			
			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCouponVerifyNum;
		}

		if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && ST_SettleRec.stSettle[i]._TelVerifyNum > 0)
		{
			//�ֻ�����֤
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._TelVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "TELNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);			
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._TelVerifyNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum);		
			_NumSum += ST_SettleRec.stSettle[i]._TelVerifyNum;	

			//�ֻ��ųжҳ���
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidTelVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDTELNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidTelVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidTelVerifyNum, szDispAmt);
			}
			
			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidTelVerifyNum;
			
		}

		if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && ST_SettleRec.stSettle[i]._CardVerifyNum > 0)
		{
			//���п���֤
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CardVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "CARDNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._CardVerifyNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._CardVerifyNum;	

			//���п��жҳ���
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VodiCardVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCARDNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCardVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCardVerifyNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCardVerifyNum; 
			
		}
		
		if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && ST_SettleRec.stSettle[i]._RefundVerifyNum > 0)
		{
			//�˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundVerifyNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundVerifyNum;  
		}

		if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
			&& (ST_SettleRec.stSettle[i]._PrecreateNum > 0 || ST_SettleRec.stSettle[i]._CreatepayNum > 0))
		{
			//΢��֧��
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._PrecreateAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "WECHATNUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._PrecreateNum + ST_SettleRec.stSettle[i]._CreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._PrecreateNum;	
			_NumSum += ST_SettleRec.stSettle[i]._CreatepayNum;
		
			//΢��֧������
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidPrecreateAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDWECHATNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidPrecreateNum + ST_SettleRec.stSettle[i]._VoidCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidPrecreateNum + ST_SettleRec.stSettle[i]._VoidCreatepayNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidPrecreateNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCreatepayNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && ST_SettleRec.stSettle[i]._RefundWechatNum > 0)
		{
			//΢���˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundWeChatAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDWECHATNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundWechatNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundWechatNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundWechatNum;
		}


		if((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
			&& (ST_SettleRec.stSettle[i]._BaiduPrecreteNum > 0 || ST_SettleRec.stSettle[i]._BaiduCreatepayNum > 0))
		{
			//�ٶ�֧��
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "CREATEBAIDUNUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BaiduPrecreteNum + ST_SettleRec.stSettle[i]._BaiduCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._BaiduPrecreteNum;	
			_NumSum += ST_SettleRec.stSettle[i]._BaiduCreatepayNum;
		
			//�ٶ�֧������
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDCREATEBAIDUNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum + ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum + ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && ST_SettleRec.stSettle[i]._RefundBaiduNum > 0)
		{
			//�ٶ��˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundBaiduAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDBAIDUNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundBaiduNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundBaiduNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundBaiduNum;
		}

		if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES) && ST_SettleRec.stSettle[i]._JDPrecreateNum > 0)
		{
			//����֧��
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._JDPrecreateAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "JDCREATENUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDPrecreateNum, szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._JDPrecreateNum;	

			//��������
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidJDPrecreateAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDJDCREATENUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidJDPrecreateNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidJDPrecreateNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidJDPrecreateNum; 
		}
		
		
		if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && ST_SettleRec.stSettle[i]._JDRefundNum > 0)
		{
			//�����˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._JDRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "JDREFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDRefundNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._JDRefundNum;
		}


		if((GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
			&& (ST_SettleRec.stSettle[i]._AliPrecreteNum > 0 || ST_SettleRec.stSettle[i]._AliCreatepayNum > 0))
		{
			//֧����֧��
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliCreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "ALICREATEALINUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliPrecreteNum + ST_SettleRec.stSettle[i]._AliCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._AliPrecreteNum;	
			_NumSum += ST_SettleRec.stSettle[i]._AliCreatepayNum;
		
			//֧������
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidAliPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidAliCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDALICREATENUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidAliPrecreteNum + ST_SettleRec.stSettle[i]._VoidAliCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidAliPrecreteNum + ST_SettleRec.stSettle[i]._VoidAliCreatepayNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidAliPrecreteNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidAliCreatepayNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && ST_SettleRec.stSettle[i]._AliRefundNum > 0)
		{
			//֧�����˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "ALIREFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliRefundNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._AliRefundNum;
		}

		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && ST_SettleRec.stSettle[i]._MeituanNum> 0)
		{
			//���ųж�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._MeituanAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "MEITUANNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._MeituanNum, szDispAmt);

			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._MeituanNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && ST_SettleRec.stSettle[i]._PanjintongNum> 0)
		{
			//�̽�ͨ
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._PanjintongAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "PANJINTONGNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._PanjintongNum, szDispAmt);

			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._PanjintongNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && ST_SettleRec.stSettle[i]._BestpayCreatepayNum > 0)
		{
			//��֧��
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "CREATEBESTPAYNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._BestpayCreatepayNum; 

			//��֧������
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCREATEBESTPAYNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum, szDispAmt);
			}

			/*����Ҫ��ȥ�������*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES && ST_SettleRec.stSettle[i]._BestpayRefundNum > 0)
		{
			//��֧���˻�
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BestpayRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDBESTPAYNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayRefundNum, szDispAmt);
			}

			/*����Ҫ��ȥ�˻����*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._BestpayRefundNum;
		}
#if 0		
		//�ܼ�:AmtSubAmt�����⣬������������
		{
			sprintf(szTmpTag, "AMTANDNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));	
			ProAmtToDispOrPnt((char*)szAmtSum, szDispAmt);		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", _NumSum, szDispAmt);
		}
#endif		
	}
	if (*pnReprintFlag==REPRINT)
	{
		//���㵥���ش�ӡƾ֤
		PubSetFieldValue( "ISREPRINT", "1", 1);
	}

	PubPrintModule(pszModuleName);
	return APP_SUCC;
}

/*
����������ظ�Ӧ���޹�

PubSetFieldValue("ISPRINTINPUTPIN", "1", 1);			
//�����ཻ�ײ���ѯ���Ƿ�ˢ��
//���ڸ�����Ƿ�ˢ��
PubSetFieldValue("SALEVOIDSTRIP", stAppPosParam.cIsSaleVoidStrip == '1' ? "��" : "����", 4);			
//Ԥ��Ȩ��ɳ����Ƿ�ˢ��
PubSetFieldValue("AUTHSALEVOIDSTRIP", stAppPosParam.cIsAuthSaleVoidStrip == '1' ? "��" : "����", 4);			
//���ѳ����Ƿ�������
//���ڸ�����Ƿ�������		
PubSetFieldValue("VOIDPIN", stAppPosParam.cIsVoidPin == '1' ? "��" : "����", 4);			
//Ԥ��Ȩ���������Ƿ���������
PubSetFieldValue("PREAUTHVOIDPIN", stAppPosParam.cIsPreauthVoidPin == '1' ? "��" : "����", 4);	
//Ԥ��Ȩ��ɳ����Ƿ�����
PubSetFieldValue("AUTHSALEVOIDPIN", stAppPosParam.cIsAuthSaleVoidPin == '1' ? "��" : "����", 4);			
//��Ȩ�����������
PubSetFieldValue("AUTHSALEPIN", stAppPosParam.cIsAuthSalePin == '1' ?  "��" : "����",4);	

//EMV������ӡ

if((*pnPrintType)&PRINT_PARAM_TYPE_EMV) 
	{
		return _printemvparam();
	}


*/
int SetAndPrint_Param(void *ptrPara)
{
	char szTmpStr[21];
	STAPPPOSPARAM stAppPosParam;
	STAPPCOMMPARAM stAppCommParam;
	STAPPCOMMRESERVE stAppCommReserve;
	
	int *pnPrintType=(int *)ptrPara	;
	char *pszModuleName = NULL;
	char szSoftVer[16+1];	
	

	PubClearAll();
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 3, "���ڴ�ӡ");
	PubUpdateWindow();
		
	PubClearFieldValue();

	// ��ȡ����
	GetAppCommParam(&stAppCommParam);
	GetAppPosParam(&stAppPosParam);
	GetAppCommReserve(&stAppCommReserve);

	if((*pnPrintType)&PRINT_PARAM_TYPE_MERCHANTINFO) /*<�̻���Ϣ*/
	{
		pszModuleName = "PRINT_PARAM_MERCHANTINFO";
		
		PubSetFieldValue("ISPRINTSHOPPARAM", "1", 1);
		//�̻���������
		PubSetFieldValue( "MERCHANTNAME", stAppPosParam.szMerchantNameCn, strlen(stAppPosParam.szMerchantNameCn));
		// �̻����
		PubSetFieldValue( "SHOPID", stAppPosParam.szMerchantId, strlen(stAppPosParam.szMerchantId));
		// POS���
		PubSetFieldValue( "TERMINALID", stAppPosParam.szTerminalId, strlen(stAppPosParam.szTerminalId));
		//Ӧ����
		if(strlen(stAppPosParam.szAppDispname)>0)
		{
			PubSetFieldValue( "APPNAME", stAppPosParam.szAppDispname, strlen(stAppPosParam.szAppDispname));	
		}
		else
		{
			PubSetFieldValue( "APPNAME", APP_NAMEDESC, strlen(APP_NAMEDESC));	
		}
		/*ADTMS�·������Ϣ*/
		if (strlen(stAppPosParam.szAdInfo) > 0)
		{
			PubSetFieldValue("ADSINFO", stAppPosParam.szAdInfo, strlen(stAppPosParam.szAdInfo));
		}
	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_VERSION) /*<�汾��Ϣ*/
	{
		pszModuleName = "PRINT_PARAM_VERSION";
		
		//����汾��
		PubSetFieldValue("ISPRINTVER", "1", 1);		
		memset(szSoftVer, 0, sizeof(szSoftVer));
		memcpy(szSoftVer, "V", 1);
		GetVarDispSoftVer(&szSoftVer[1]);	
		PubSetFieldValue("APPVER", szSoftVer, strlen(szSoftVer));
	}
#if 0	
	if((*pnPrintType)&PRINT_PARAM_TYPE_TRANSCCTRL) /*<���׿���*/
	{
		pszModuleName = "PRINT_PARAM_TRANSCCTRL";
		
		PubSetFieldValue("ISPRINTTRANS", "1", 1);	
		PntTraditionOnoff(stAppPosParam.sTraditionSwitch);
		PntECashOnoff(stAppPosParam.sECashSwitch);
		PntWalletOnoff(stAppPosParam.sWalletSwitch);
		PntInstallmentOnoff(stAppPosParam.sInstallmentSwitch);
		PntBonusOnoff(stAppPosParam.sBonusSwitch);
		PntPhoneChipSaleOnoff(stAppPosParam.sPhoneChipSaleSwitch);
		PntAppointmentOnoff(stAppPosParam.sAppointmentSwitch);
		PntOrderOnoff(stAppPosParam.sOrderSwitch);
		PntOtherOnoff(stAppPosParam.sOtherSwitch);
		//�Զ�ǩ������
		PubSetFieldValue("AUTOLOGOUT", stAppPosParam.cIsAutoLogOut == '1' ? "��" : "����",4);	
		//�����Ƿ��ӡ��ϸ
		PubSetFieldValue("PRNWATERREC", stAppPosParam.cIsPrintWaterRec == '1' ? "��" : "����", 4);	
		//�������ͷ�ʽ
		
		PubSetFieldValue("OFFLINESENDTYPE", stAppPosParam.cIsOfflineSendNow == '1' ? "����ǰ" : "����ǰ", 6);
		//�������ʹ���
		PubSetFieldValueFormat( "OFFLINESENDCOUNT", "%d", stAppCommParam.cOffResendNum >= 0x30 ? stAppCommParam.cOffResendNum - 0x30 : stAppCommParam.cOffResendNum);
		//�Ƿ�������������
		PubSetFieldValue("ADMINPWD", stAppPosParam.cIsAdminPwd == '1' ? "��" : "����",4);	
		//�Ƿ��������俨��
		PubSetFieldValue("CARDINPUT", stAppPosParam.cIsCardInput == '1' ? "��" : "����", 4);	
		//Ĭ��ˢ������
		PubSetFieldValue("DEFAULTTRANSTYPE", stAppPosParam.cDefaultTransType== '1' ? "����" : "Ԥ��Ȩ", 6);	
		//�˻�����޶�
		LeftTrimZero(stAppPosParam.szMaxRefundAmount);
		PubSetFieldValueFormat( "MAXREFUNDAMT", "%lld.%02lld",  AtoLL(stAppPosParam.szMaxRefundAmount)/100,AtoLL(stAppPosParam.szMaxRefundAmount)%100);
	}
#endif	
	if((*pnPrintType)&PRINT_PARAM_TYPE_SYSTEMCTRL) /*<ϵͳ����*/
	{
		pszModuleName = "PRINT_PARAM_SYSTEMCTRL";
		
		PubSetFieldValue("ISPRINTSYSPARAM", "1", 1);	
		//��ǰ����ƾ֤��
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetVarTraceNo(szTmpStr);
		PubSetFieldValue("TRACENO", szTmpStr, strlen(szTmpStr));	
		//��ǰ�������κ�
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetVarBatchNo(szTmpStr);
		PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));	
		//�Ƿ��ӡ�����յ���
		PubSetFieldValue("PNTCHACQUIRER", stAppPosParam.cIsPntChAcquirer == '1' ? "��" : "����", 4);	
		//�Ƿ��ӡ���ķ�����
		PubSetFieldValue("PNTCHCARDSCHEME", stAppPosParam.cIsPntChCardScheme == '1' ? "��" : "����", 4);	
		//�״�ǩ������ʽ
		switch(stAppPosParam.cIsNewTicket)
		{
		case '0':
			PubSetFieldValue("PNTTYPE", "��",2);	
			break;
		case '1':
			PubSetFieldValue("PNTTYPE", "��",2);	
			break;
		case '2':
			PubSetFieldValue("PNTTYPE", "�հ�",4);	
			break;
		default:
			PubSetFieldValue("PNTTYPE", "  ",2);	
			break;
		}
		// ��ӡ��������
		PubSetFieldValueFormat( "PRINTPAGE", "%d", stAppPosParam.cPrintPageCount - '0');		
		//ǩ�����Ƿ��ӡӢ��
		PubSetFieldValue("TICKETWITHEN", stAppPosParam.cIsTicketWithEn == '1' ? "��" : "����", 4);	
		//��������
		PubSetFieldValueFormat( "REVERSALNUM", "%d", stAppCommParam.cReSendNum >= 0x30 ? stAppCommParam.cReSendNum - 0x30 : stAppCommParam.cReSendNum);		
		//����ױ���
		PubSetFieldValueFormat( "MAXTRANSCOUNT", "%d", atoi(stAppPosParam.szMaxTransCount));
		//С�ѱ���
		PubSetFieldValueFormat( "TIPRATE", "%d",  atoi(stAppPosParam.szTipRate));		
		//��ӡ����
		PubSetFieldValue("PNTFONTSIZE", (stAppPosParam.cPntFontSize== '0' )? "С" : (stAppPosParam.cPntFontSize== '1' ?"��":"��"), 2);	
		//��ӡ����
		PubSetFieldValue("PRINTMINUS", stAppPosParam.cIsPrintMinus == '1' ? "��" : "����", 4);	
		//��ӡ���н���
		PubSetFieldValue("PNTALLTRANS", stAppPosParam.cIsPrintAllTrans == '1' ? "��" : "����", 4);	
		//�յ�������
		PubSetFieldValue("UNKNOWBANK", stAppPosParam.szPrintUnknowBankInfo, strlen(stAppPosParam.szPrintUnknowBankInfo));	
	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_OTHER) /*<����*/
	{
		pszModuleName = "PRINT_PARAM_OTHER";

		PubSetFieldValue("ISPRINTKEY", "1", 1);	
		//����Կ������
		PubSetFieldValue("MAINKEYNO", stAppPosParam.szMainKeyNo, strlen(stAppPosParam.szMainKeyNo));
		//��Կ�㷨
		PubSetFieldValue("ENCYPTMODE", stAppPosParam.cEncyptMode == '0' ? "��DES" : "3DES",5);

	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_COMM)
	{
		pszModuleName = "PRINT_PARAM_COMM";
		
		PubSetFieldValue("ISPRINTCOMM", "1", 1);		//ͨѶ��ʽ
		switch(stAppCommParam.cCommType)
		{
		case COMM_RS232:
			PubSetFieldValue( "COMMTYPE", "����", 4);
			break;
		case COMM_DIAL:
			PubSetFieldValue( "COMMTYPE", "����", 4);
			PubSetFieldValue( "TELNO1",  stAppCommParam.szTelNum1, strlen(stAppCommParam.szTelNum1));
			PubSetFieldValue( "TELNO2",  stAppCommParam.szTelNum2, strlen(stAppCommParam.szTelNum2));
			PubSetFieldValue( "TELNO3",  stAppCommParam.szTelNum3, strlen(stAppCommParam.szTelNum3));
			PubSetFieldValue( "MANAGETELNO",  stAppCommParam.szManageTelNum, strlen(stAppCommParam.szManageTelNum));
			break;
		case COMM_GPRS:
		case COMM_CDMA:
		case COMM_ETH:
		case COMM_WIFI:
			if (stAppCommParam.cCommType == COMM_GPRS)
			{
				PubSetFieldValue( "COMMTYPE", "GPRS", 4);
				PubSetFieldValue( "APN1", stAppCommParam.szAPN1, strlen(stAppCommParam.szAPN1));
				PubSetFieldValue( "APN2", stAppCommParam.szAPN2, strlen(stAppCommParam.szAPN2));
			}
			else if (stAppCommParam.cCommType == COMM_ETH)
			{
				PubSetFieldValue( "COMMTYPE", "��̫��", 6);
				PubSetFieldValue( "IPADDR", stAppCommParam.szIpAddr, strlen(stAppCommParam.szIpAddr));
				PubSetFieldValue( "MASK", stAppCommParam.szMask, strlen(stAppCommParam.szMask));
				PubSetFieldValue( "GATE",  stAppCommParam.szGate, strlen(stAppCommParam.szGate));
			}
			else if (stAppCommParam.cCommType == COMM_CDMA)
			{
				PubSetFieldValue( "COMMTYPE", "CDMA", 4);
			
			}
			else if(stAppCommParam.cCommType == COMM_WIFI)
			{
				PubSetFieldValue( "COMMTYPE", "WIFI", 4);
				PubSetFieldValue("WIFISSID", stAppCommReserve.szWifiSsid, strlen(stAppCommReserve.szWifiSsid));
				switch(stAppCommReserve.cWifiMode)
				{
				case WIFI_NET_SEC_NONE:
					PubSetFieldValue( "WIFIMODE", "������", 6);
					break;
				case WIFI_NET_SEC_WEP_OPEN:
					PubSetFieldValue( "WIFIMODE", "OPEN", 4);
					break;
				case WIFI_NET_SEC_WEP_SHARED:
					PubSetFieldValue( "WIFIMODE", "SHARED", 6);
					break;
				case WIFI_NET_SEC_WPA:
					PubSetFieldValue( "WIFIMODE", "WAP", 3);
					break;
				case WIFI_NET_SEC_WPA2:
					PubSetFieldValue( "WIFIMODE", "WAP2", 4);
					break;
				}
			}

			if(stAppCommParam.cIsDns)
			{
				PubSetFieldValue("DNSIP",stAppCommParam.szDNSIp1, strlen(stAppCommParam.szDNSIp1));
				PubSetFieldValue("DOMAIN1",stAppCommParam.szDomain, strlen(stAppCommParam.szDomain));
				PubSetFieldValue("DNSPORT1",stAppCommParam.szPort1, strlen(stAppCommParam.szPort1));
				PubSetFieldValue("DOMAIN2",stAppCommReserve.szDomain2, strlen(stAppCommReserve.szDomain2));
				PubSetFieldValue("DNSPORT2",stAppCommParam.szPort2, strlen(stAppCommParam.szPort2));
			}
			else
			{
				PubSetFieldValue( "IP1", stAppCommParam.szIp1, strlen(stAppCommParam.szIp1));
				PubSetFieldValue( "PORT1", stAppCommParam.szPort1, strlen(stAppCommParam.szPort1));
				PubSetFieldValue( "IP2", stAppCommParam.szIp2, strlen(stAppCommParam.szIp2));
				PubSetFieldValue( "PORT2", stAppCommParam.szPort2, strlen(stAppCommParam.szPort2));
			}

			if ((stAppCommParam.cCommType == COMM_GPRS)||(stAppCommParam.cCommType == COMM_CDMA))
			{
				//�û���
				PubSetFieldValueFormat( "USERNAME", "%3.3s****", stAppCommParam.szUser);
				
				//�û�����
				PubSetFieldValueFormat( "USERPWD", "%3.3s****", stAppCommParam.szPassWd);	

				//�������ĺ���
				PubSetFieldValue( "WIRELESSDIALNUN", stAppCommParam.szWirelessDialNum, strlen(stAppCommParam.szWirelessDialNum));

				//�Ƿ������
				PubSetFieldValue( "COMMMODE", (stAppCommParam.cMode == '1' || stAppCommParam.cMode == 1) ? "��" : "��", 2);
			}
			break;
		default:
			break;
		}
		
		//TPDU
		PubSetFieldValueFormat( "TPDU", "%02x%02x%02x%02x%02x", 
			stAppCommParam.sTpdu[0], stAppCommParam.sTpdu[1], stAppCommParam.sTpdu[2], stAppCommParam.sTpdu[3], stAppCommParam.sTpdu[4]);
		
		//�ز�����
		PubSetFieldValueFormat( "REDIALNUM", "%d", stAppCommParam.cReDialNum >= 0x30 ? stAppCommParam.cReDialNum - 0x30 : stAppCommParam.cReDialNum);

		//���׳�ʱʱ��
		PubSetFieldValueFormat( "TIMEOUT", "%d", stAppCommParam.cTimeOut);

		//Ԥ��������
		PubSetFieldValue( "ISPREDIAL",  stAppCommParam.cPreDialFlag=='1' || stAppCommParam.cPreDialFlag==1 ? "��" : "��", 2);
	}
	
	PubPrintModule(pszModuleName);
	return APP_SUCC ;
}

int SetAndPrint_Allwater(void *ptrPara)
{
	static int nHasRecord;
	int nFileHandle;
	STWATER stWater;
	int nWaterNum;
	int i, nPrintNum = 30;
	int nStartRecNo;
	char szTmpStr[64];
	char szStr[32];
	int *pnCurPrintRecNo;
	char *pszModuleName = "PRINT_ALL_WATER";
	char szTmpAmt[12+1] = {0};

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nPrintNum = 16; 	
	}	
	pnCurPrintRecNo = (int *)ptrPara;
	//�����ܱ���
	GetWaterNum(&nWaterNum);
	nStartRecNo = *pnCurPrintRecNo;
	if (nStartRecNo==1)
	{
		nHasRecord=0;
	}
	
	ASSERT_FAIL(PubOpenFile(FILE_WATER, "r", &nFileHandle));

	for(i=0; i<nPrintNum && (*pnCurPrintRecNo <=nWaterNum); (*pnCurPrintRecNo)++)
	{
		PubClearFieldValue();
		PubReadRec(nFileHandle, *pnCurPrintRecNo, (char *)&stWater);
		
		if (stWater.cStatus == '1' || stWater.cStatus == 0x01)
		{
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}
		if (stWater.cStatus == 0x04)	/**<��������Ĳ���Ҫ�󲻴�ӡ�������Ľ���injz-20121219*/	
		{
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}

		if(stWater.cTransType == TRANS_VOID_SALE ||stWater.cTransType == TRANS_VOID_AUTHSALE ||stWater.cTransType == TRANS_VOID_INSTALMENT
			||stWater.cTransType == TRANS_VOID_COUPON_VERIFY || stWater.cTransType == TRANS_VOID_CARD_VERIFY
			||stWater.cTransType == TRANS_VOID_TEL_VERIFY || stWater.cTransType == TRANS_VOID_PRECREATE 
			|| stWater.cTransType == TRANS_VOID_CREATEANDPAY || stWater.cTransType == TRANS_VOID_CREATEANDPAYBAIDU
			|| stWater.cTransType == TRANS_VOID_JD_PRECREATE || stWater.cTransType == TRANS_VOID_ALI_CREATEANDPAY
			|| stWater.cTransType == TRANS_VOID_MEITUAN || stWater.cTransType == TRANS_VOID_DZ_PRECREATE || stWater.cTransType == TRANS_VOID_DZ_CREATEANDPAY
			|| stWater.cTransType == TRANS_VOID_BESTPAY_CREATEANDPAY
		)

		{	
			/*�������ײ���ӡ*/
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}
		
		// ��ӡ����ͷ
		if (nStartRecNo==1 && i==0 )
		{
			PubSetFieldValue("ISTITLE", "1", 1);
			PubSetFieldValue("ISDESCRIPT", "1", 1);
		}
		// ƾ֤��
		PubSetFieldValueFormat("TRACENO", "%02x%02x%02x", stWater.sTrace[0], stWater.sTrace[1], stWater.sTrace[2]);
        	
		// ����
		memset(szTmpStr, 0, sizeof(szTmpStr));
 		GetTransNameForTP(&stWater, szTmpStr, szStr);
		PubAddSymbolToStr(szTmpStr, 16, ' ', 1);
		PubSetFieldValueFormat("TYPE", "%s",  szTmpStr);
        	
		// ����
		/*
		switch(stWater.cTransType)
		{
			case TRANS_COUPON_VERIFY:		
				PubSetFieldValue("CARDNO", stWater.sCouponID, strlen(stWater.sCouponID));
				break;
			case TRANS_TEL_VERIFY:
				PubSetFieldValue("CARDNO",stWater.sTelNo, strlen(stWater.sTelNo));
				break;
			case TRANS_CARD_VERIFY:
				PubHexToAsc((uchar *)stWater.sPan, stWater.nPanLen, 0, (uchar *)szTmpStr);
				CtrlCardPan((uchar*)szTmpStr, stWater.nPanLen, stWater.cTransType, stWater.cEMV_Status);
				PubSetFieldValueFormat("CARDNO", "%-19.19s", szTmpStr);
				break;
			case TRANS_VOID_VERIFY:
				break;
			case TRANS_REFUND_VERIFY:
				break;
		}
		*/

		
		// ���
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memset(szTmpAmt, 0, sizeof(szTmpAmt));
		if ((stWater.cTransType == TRANS_COUPON_VERIFY) || (stWater.cTransType == TRANS_TEL_VERIFY) ||
			(stWater.cTransType == TRANS_CARD_VERIFY) || (stWater.cTransType == TRANS_VOID_COUPON_VERIFY) || 
			(stWater.cTransType == TRANS_VOID_TEL_VERIFY) || (stWater.cTransType == TRANS_VOID_CARD_VERIFY))
		{
			PubHexToAsc((uchar *)stWater.szRealAmount, 12, 0, (uchar *)szTmpAmt);
		}
		else
		{
			PubHexToAsc((uchar *)stWater.szAmount, 12, 0, (uchar *)szTmpAmt);
		}
		ProAmtToDispOrPnt(szTmpAmt, szTmpStr);
		PubAllTrim(szTmpStr);
		DealPrintAmount(stWater.cTransType, szTmpStr);
		PubAddSymbolToStr(szTmpStr, 13, ' ', 0);
		PubSetFieldValueFormat("AMOUNT", "%s", szTmpStr);	
		
		// ��Ȩ��
		//PubSetFieldValue("AUTHNO", stWater.szAuthCode, strlen(stWater.szAuthCode));
		
		i++;
		PubPrintModule(pszModuleName);
	}
	ASSERT_FAIL(PubCloseFile(&nFileHandle));
	if (i>0)
	{
		nHasRecord = 1;
	}
	else  //�޿ɴ�ӡ��ϸ��¼
	{
		return APP_FUNCQUIT;
	}
		
	// ��ӡ����β
	if (*pnCurPrintRecNo>nWaterNum && nHasRecord==1)
	{
		PubClearFieldValue();
		
		// ��������:S-���� R-�˻� P-��Ȩ���  L-����  A-���� 
		if(GetVarIsPrintAllTrans()==NO)
		{
			PubSetFieldValue( "ISEXPLAIN", "1", 1);
		}
		else
		{
			PubSetFieldValue( "ISEXPLAINALL", "1", 1);
		}
		if (PubGetPrinter()==_PRINTTYPE_TP)
		{
			//- - - - - - - X - - - - - - - X - - - - - - - 
			PubSetFieldValue( "ISEND", "1", 1);
		}
		PubPrintModule(pszModuleName);
	}

	return APP_SUCC;
}

//����
int SetAndPrint_Total(void *ptrPara)
{
	STSETTLE stSettle_NK, stSettle_WK;
	char *pszModuleName = "PRINT_TOTAL";
	char szAmt1[13] = {0};
	char szAmt2[13] = {0};
	char szAmt[13] = {0};
	char szDispAmt[16] = {0};
	
	GetSettleDataNK(&stSettle_NK);
	GetSettleDataWK(&stSettle_WK);

	PubClearFieldValue();

	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && stSettle_NK._CouponVerifyNum)
	{
		//������ȯ
		PubSetFieldValueFormat( "COUPONNUM", "%4ld��", stSettle_NK._CouponVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._CouponVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "COUPONAMT", "  %s" , szDispAmt);

		//���볷��
		PubSetFieldValueFormat( "VOIDCOUPONNUM", "%4ld��", stSettle_NK._VoidCouponVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidCouponVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCOUPONAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && stSettle_NK._TelVerifyNum > 0)
	{
		//�ֻ�����ȯ
		PubSetFieldValueFormat( "TELNUM", "%4ld��", stSettle_NK._TelVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._TelVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "TELAMT", "  %s" , szDispAmt);

		//�ֻ�����֤����
		PubSetFieldValueFormat( "VOIDTELNUM", "%4ld��", stSettle_NK._VoidTelVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidTelVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDTELAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && stSettle_NK._CardVerifyNum > 0)
	{
		//���п���ȯ
		PubSetFieldValueFormat( "CARDNUM", "%4ld��", stSettle_NK._CardVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._CardVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "CARDAMT", "  %s" , szDispAmt);

		//���п���֤����
		PubSetFieldValueFormat( "VOIDCARDNUM", "%4ld��", stSettle_NK._VoidCardVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VodiCardVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCARDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && stSettle_NK._RefundVerifyNum > 0)
	{
		//��֤�˻�
		PubSetFieldValueFormat( "REFUNDNUM", "%4ld��", stSettle_NK._RefundVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDAMT", "  %s" , szDispAmt);
	}

	if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		&& (stSettle_NK._PrecreateNum > 0 || stSettle_NK._CreatepayNum > 0))
	{
		//΢��֧��ɨ��+ ����
		PubSetFieldValueFormat( "WECHATNUM", "%4ld��", stSettle_NK._PrecreateNum+stSettle_NK._CreatepayNum);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._PrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt2, 0, sizeof(szAmt2));
		PubHexToAsc(stSettle_NK._CreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt2);

		memset(szAmt, 0, sizeof(szAmt));
		PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);

		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "WECHATAMT", "  %s" , szDispAmt);
	}
	
	if((GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
		&& (stSettle_NK._VoidPrecreateNum > 0 || stSettle_NK._VoidCreatepayNum > 0))
	{
		//ɨ��֧������ +  ����֧������
		PubSetFieldValueFormat( "VOIDWECHARTNUM", "%4ld��", stSettle_NK._VoidPrecreateNum + stSettle_NK._VoidCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt2, 0, sizeof(szAmt2));
		PubHexToAsc(stSettle_NK._VoidCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt2);

		memset(szAmt, 0, sizeof(szAmt));
		PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
		
		ProAmtToDispOrPnt(szAmt, szAmt1);
		PubAllTrim(szAmt1);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt1);
		}
		else
		{
			strcpy(szDispAmt, szAmt1);
		}
		PubSetFieldValueFormat( "VOIDWECHARTAMT", "  %s" , szDispAmt);
		
	}

	if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && stSettle_NK._RefundWechatNum > 0)
	{
		//΢���˻�
		PubSetFieldValueFormat( "REFUNDWECHATNUM", "%4ld��", stSettle_NK._RefundWechatNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundWeChatAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDWECHATAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES && stSettle_NK._BaiduCreatepayNum > 0)
	{
		//�ٶ�֧��
		PubSetFieldValueFormat( "CREATEBAIDUNUM", "%4ld��", stSettle_NK._BaiduCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BaiduCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEBAIDUAMT", "  %s" , szDispAmt);
		
		//�ٶ�֧������
		PubSetFieldValueFormat( "VOIDCREATEBAIDUNUM", "%4ld��", stSettle_NK._VoidBaiduCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEBAIDUAMT", "  %s" , szDispAmt);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES && stSettle_NK._BaiduPrecreteNum > 0)
	{
		//�ٶȱ�ɨ֧��
		PubSetFieldValueFormat( "PRECREATEBAIDUNUM", "%4ld��", stSettle_NK._BaiduPrecreteNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BaiduPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PRECREATEBAIDUAMT", "  %s" , szDispAmt);
		
		//�ٶȱ�ɨ֧������
		PubSetFieldValueFormat( "VOIDPRECREATEBAIDUNUM", "%4ld��", stSettle_NK._VoidBaiduPrecreteNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDPRECREATEBAIDUAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && stSettle_NK._RefundBaiduNum > 0)
	{
		//�ٶ��˻�
		PubSetFieldValueFormat( "REFUNDBAIDUNUM", "%4ld��", stSettle_NK._RefundBaiduNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundBaiduAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDBAIDUAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES && stSettle_NK._JDPrecreateNum > 0)
	{
		//����֧��
		PubSetFieldValueFormat( "CREATEJDNUM", "%4ld��", stSettle_NK._JDPrecreateNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._JDPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEJDAMT", "  %s" , szDispAmt);
		
		//����֧������
		PubSetFieldValueFormat( "VOIDCREATEJDNUM", "%4ld��", stSettle_NK._VoidJDPrecreateNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidJDPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEJDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && stSettle_NK._JDRefundNum > 0)
	{
		//�����˻�
		PubSetFieldValueFormat( "REFUNDJDNUM", "%4ld��", stSettle_NK._JDRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._JDRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDJDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES && stSettle_NK._AliCreatepayNum > 0)
	{
		//֧����֧��
		PubSetFieldValueFormat( "CREATEALINUM", "%4ld��", stSettle_NK._AliCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEALIAMT", "  %s" , szDispAmt);
		
		//֧����֧������
		PubSetFieldValueFormat( "VOIDCREATEALINUM", "%4ld��", stSettle_NK._VoidAliCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidAliCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEALIAMT", "  %s" , szDispAmt);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && stSettle_NK._AliRefundNum > 0)
	{
		//֧�����˻�
		PubSetFieldValueFormat( "REFUNDALINUM", "%4ld��", stSettle_NK._AliRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDALIAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES && stSettle_NK._AliPrecreteNum > 0)
	{
		//֧������ɨ֧��
		PubSetFieldValueFormat( "PRECREATEALINUM", "%4ld��", stSettle_NK._AliPrecreteNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PRECREATELIAMT", "  %s" , szDispAmt);
		
		//֧������ɨ֧������
		PubSetFieldValueFormat( "VOIDPRECREATEALINUM", "%4ld��", stSettle_NK._VoidAliPrecreteNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidAliPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDPRECREATELIAMT", "  %s" , szDispAmt);
	}

	

	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && stSettle_NK._MeituanNum > 0)
	{
		//���Ŷ�ȯ
		PubSetFieldValueFormat( "MEITUANNUM", "%4ld��", stSettle_NK._MeituanNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._MeituanAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "MEITUANAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && stSettle_NK._PanjintongNum > 0)
	{
		//�̽�֧ͨ��
		PubSetFieldValueFormat( "PANJINTONGNUM", "%4ld��", stSettle_NK._PanjintongNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._PanjintongAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PANJINTONGAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && stSettle_NK._BestpayCreatepayNum > 0)
	{
		//��֧��
		PubSetFieldValueFormat( "CREATEBESTPAYNUM", "%4ld��", stSettle_NK._BestpayCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEBESTPAYAMT", "  %s" , szDispAmt);
		
		//��֧������
		PubSetFieldValueFormat( "VOIDCREATEBESTPAYNUM", "%4ld��", stSettle_NK._BestpayCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEBESTPAYAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && stSettle_NK._BestpayRefundNum > 0)
	{
		//��֧���˻�
		PubSetFieldValueFormat( "REFUNDBESTPAYNUM", "%4ld��", stSettle_NK._BestpayRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDBESTPAYAMT", "  %s" , szDispAmt);
	}

	PubPrintModule(pszModuleName);
	return APP_SUCC;
}

#if 0

static int PntHex(const char *pszTitle, const uchar *psContent, const int nConternLen)
{
	int i;
	
	ASSERT_FAIL(PubPrePrinter(pszTitle));
	for (i = 0; i < nConternLen; i++)
	{
		ASSERT_FAIL(PubPrePrinter("%02X", psContent[i]));
	}
	ASSERT_FAIL(PubPrePrinter("\r"));
	return APP_SUCC;		
}


int  _printemvparam()
{
#if defined(EMV_IC)
	int nRet;
	int i;
	char sAid[20][17];
	char sCapk[50][6];
	emvparam stAID;
	capk stCAPK;
	int nRes;
	uint nNum;

	PubClearAll();
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 3, "���ڴ�ӡ");
	PubUpdateWindow();
	
	nRet = EMV_EnumAID(sAid);
	if (nRet > 0)
	{
		ASSERT_FAIL(PubPrePrinter("AID_Num:%d\r", nRet));
		for (i = 0; i < nRet; i++)
		{	
			stAID._aid_len = sAid[i][0];
			memcpy(stAID._aid, &sAid[i][1], sAid[i][0]);
			nRes = EMV_OperAID(&stAID,AID_GET);
			if (nRes == APP_SUCC)
			{
				ASSERT_FAIL(PubPrePrinter("---------------------\r"));
				PntHex("AID:", stAID._aid, stAID._aid_len);
				PntHex("VER:", stAID._app_ver, 2);
				PntHex("TAC_Default:", stAID._tac_default, 5);
				PntHex("TAC_Decline:", stAID._tac_denial, 5);
				PntHex("TAC_Online :", stAID._tac_online, 5);
				PubC4ToInt(&nNum, (uchar *)stAID._floorlimit);
				ASSERT_FAIL(PubPrePrinter("FloorLimit:%d\r", nNum));
				PubC4ToInt(&nNum, (uchar *)stAID._threshold_value);
				ASSERT_FAIL(PubPrePrinter("ThresholdValue:%d\r", nNum));
				PntHex("MaxTargetPercent:", &stAID._max_target_percent, 1);
				PntHex("TargetPercent:", &stAID._target_percent, 1);
				PntHex("Default DDOL:", stAID._default_ddol, stAID._default_ddol_len);
				PntHex("Default TDOL:", stAID._default_tdol, stAID._default_tdol_len);
				PntHex("CAP:", stAID._cap, 3);
				PntHex("ADDCAP:", stAID._add_cap, 5);
				PntHex("ASI(PartialAID):", &stAID._app_sel_indicator, 1);
				PntHex("ICS:", stAID._ics, 7);
				PntHex("TestType:", &stAID._status, 1);
				PntHex("_limit_exist:", &stAID._limit_exist, 1);
				PntHex("_ec_limit:", stAID._ec_limit, 6);
				PntHex("_cl_limit:", stAID._cl_limit, 6);
				PntHex("_cl_offline_limit:", stAID._cl_offline_limit, 6);
				PntHex("_cvm_limit:", stAID._cvm_limit, 6);
				ASSERT_FAIL(PubPrePrinter("---------------------\r"));
			}

		}
	}
	else
	{
		ASSERT_FAIL(PubPrePrinter("EMV_EnumAID=%d\r", nRet));
	}
	
	nRet = EMV_EnumCAPK(0, 49, sCapk);
	if (nRet > 0)
	{
		ASSERT_FAIL(PubPrePrinter("CAPK_Num:%d\r", nRet));
		for (i = 0; i < nRet; i++)
		{
			memcpy(stCAPK._rid, &sCapk[i][0], 5);
			stCAPK._index = sCapk[i][5];
			nRes = EMV_OperCAPK(&stCAPK, CAPK_GET);
			if (nRes == APP_SUCC)
			{
				ASSERT_FAIL(PubPrePrinter("Index:%02x ", stCAPK._index));
				PntHex("RID:", stCAPK._rid, 5);
				PntHex("   ExpDate:", stCAPK._expired_date, 4);
			}
		}
	}
	else
	{
		ASSERT_FAIL(PubPrePrinter("EMV_EnumCAPK=%d\r", nRet));
	}
#endif

	if (PubGetPrinter()==_PRINTTYPE_TP)
	{
		ASSERT_FAIL(PubPrePrinter("\r\r\r" ));
		ASSERT_FAIL(PubPrePrinter("- - - - - - - X - - - - - - - X - - - - - - - "));
	}
	
	return APP_SUCC;
}
#endif

int Print2DScan(char *pszAscInfo)
{
	DataInfo stDataInfo;
	BmpInfo stBmpInfo;
	memset(&stDataInfo,0,sizeof(DataInfo));
	memset(&stBmpInfo,0,sizeof(BmpInfo));
	

	stDataInfo.nLevel = 3;
	stDataInfo.nMask = -1;
	stDataInfo.nVersion = 6;
	strcpy(stDataInfo.szInputData,pszAscInfo);
	stDataInfo.nLen = strlen(stDataInfo.szInputData);

	if(APP_SUCC == PubEncodeDataAndGenerateBmp(&stDataInfo,&stBmpInfo))
	{
		PubPrintImage(stBmpInfo.xsize, stBmpInfo.ysize, 100, stBmpInfo.bmpbuff);
	}
	else
	{
		PubClearAll();
		PubMsgDlg(NULL, "��������ʧ��", 0, 10);
		return APP_FAIL;
	}
	return APP_SUCC;
}



int _PrintImage(void *ptrPara)
{
	char szStr[16], szShow[64];
	int nLen;
	PrintDataInfo *pstDataInfo = (PrintDataInfo *)ptrPara;

	PubPrintImage(pstDataInfo->pBmpInfo->xsize, pstDataInfo->pBmpInfo->ysize, 105, pstDataInfo->pBmpInfo->bmpbuff);

	memset(szStr, 0, sizeof(szStr));
	ProAmtToDispOrPnt(pstDataInfo->pszAmt, szStr);
	PubAllTrim(szStr);

	sprintf(szShow, "���:%s%s", "RMB", szStr);
	nLen = (32 - strlen(szShow))/2;//�ո���
	nLen += strlen(szShow);
	PubAddSymbolToStr(szShow, nLen, ' ', 0);
	
	ASSERT(PubSetPrintFont(PRN_ZM_FONT_12x24A, PRN_HZ_FONT_24x24A, PRN_MODE_NORMAL, 0));
	ASSERT_FAIL(PubPrePrinter("\r\r%s", szShow));
	ASSERT_FAIL(PubPrePrinter("\r       ��ӭʹ�ö��ܸ�֧��," ));
	ASSERT_FAIL(PubPrePrinter("\r      ɨ�貢֧����ȷ�ϼ�!\r" ));
	
	ASSERT_FAIL(PubPrePrinter("\r\r\r" ));
	return APP_SUCC;
}


/**
* @brief ��ӡǩ����
*
*    ͨ������_printwater ʵ��
* @param in const STWATER *pstWater ��ˮ�ṹָ��
* @param in int nReprintFlag  ��ӡ���Ʊ�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintImage(PrintDataInfo stDataInfo)
{
 	ASSERT_FAIL(PubPrintCommit( _PrintImage, (void *)&stDataInfo, 1)); 
	return APP_SUCC;
}


int printYiwei(const char *pszCode)
{
	char szCode[128];
    STBMPIMAGE stBmpImage;

	memset(szCode, 0, sizeof(szCode));
    memset(&stBmpImage, 0, sizeof(STBMPIMAGE));
#if 1

	strcpy(szCode, pszCode);
	ASSERT_FAIL(MakeCode128(szCode, 1, 80, &stBmpImage, HORIZONTAL));
	PubSetFieldValueImage("COUPONPICPARAM",stBmpImage.sBmpImage,stBmpImage.height * stBmpImage.width);
	PubSetParamImg("COUPONPICSRC", stBmpImage.width, stBmpImage.height, (384 - stBmpImage.width)/2);

//	ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
//    PubPrePrinter("\r\r");

#else

    //setprintgreyscale(5);
    //setprintheatmode();

//	{
//		int i = 0;
		//while (i++ < 10)
//			{
//		ASSERT_FAIL(MakeCode128(pszCode, 20, 80, &stBmpImage, HORIZONTAL));
		//ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
//		PrintImage((BmpInfo *)&stBmpImage);
//			}
	
//	}



//    ASSERT_FAIL(MakeCode128(pszCode, 1, 160, &stBmpImage, HORIZONTAL));

//	PubSetFieldValueImage("COUPONPICPARAM",stBmpImage.sBmpImage,stBmpImage.height * stBmpImage.width);
//	PubSetParamImg("COUPONPICSRC",stBmpImage.width, stBmpImage.height, 10);

//	PrintImage((BmpInfo *)&stBmpImage);

	//PubSetParamLogo("COUPONPIC", szTmpStr, unLogoXpos);
	
    //ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
    
//    PubPrePrinter("\r\r\r");
    //setprintgreyscale(3);
    //setprintheatmode();
    printimage(301, 52, 0, tucBMPym_code128_stand);
    PubPrePrinter("\r\r");
#endif
    return APP_SUCC;
}



