/**
* @file param.c
* @brief ��������ģ��
* @version  1.0
* @author ������Ѧ���Ž�
* @date 2007-02-05
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
//2015��9��17���޸�APPCHECKVALUE  ������
#define APPCHECKVALUE		APP_NAME"021"	/**<�汾˵��Ϊ7���ֽ��й�������4λ��д+1�ܹ�˾�汾+01�汾��*/
#define FILE_APPPOSPARAM	APP_NAME"PARA"
#define FILE_EXTAPPPOSPARAM	APP_NAME"EXTPARA"
#define APPTYPE				"60"			/**<Ӧ������*/
#define APPVER				"31"			/**<Ӧ�ð汾��,2010���¹淶�汾*/
#define SECURITYPWD 		"888888"	/**<�ն˰�ȫ���룬������8���ֽ�*/
#define HIDEOPERPWD			"20100322"
#define FILE_PARAM_INI 		APP_NAME"PARA.ini"
#define FILE_PARAM_EXTERN   APP_NAME"PARAEXT"
#define FILE_EMVCODE_INI	"../share/EMVERRCODE.INI"

/**
* @struct STREVERSALPARAM ��������
*/
typedef struct
{
	char cIsReversal;		/**<������־��'0'-�ѳ�����'1'-δ����*/
	int nHaveReversalNum;	/**<�ѳ�������*/
	int nReversalLen;		/**<�������ݳ���*/
	char sReversalData[fReversalData_len+1];				/**<��������*/			
} STREVERSALPARAM;

/**
* @struct STSETTLEPARAM �������
*/
typedef struct
{
	int	nWaterSum;						/**<��ˮ����*/
	int nHaveReSendNum;					/**<���߽��������ʹ���*/
	char cPrintSettleHalt;				/**<��ӡ���㵥�ж�*/
	char cPrintDetialHalt;				/**<��ӡ��ϸ�ж�*/
	char cPrintWaterHaltFlag;			/**<��ӡǩ�����ж�*/
	char cCnCardFlag;					/**<�ڿ���ƽ��ʶ*/
	char cEnCardFlag;					/**<�⿨��ƽ��ʶ*/
	char cBatchHaltFlag;				/**<�������жϱ�ʶ*/
	char cClrSettleDataFlag;			/**<������������жϱ�ʶ*/
	int	nFinanceHaltFlag;				/**<���������ͱ�ʶ*/
	int	nMessageHaltFlag;				/**<֪ͨ�����ͱ�ʶ*/
	int	nBatchMagOfflinHaltFlag;		/**<���������������ͱ�ʶ*/
	int	nIcPurchaseHaltFlag;			/**<����Ǯ���������ͱ�ʶ*/
	int	nIcLoadHaltFlag;				/**<Ȧ�������ͱ�ʶ*/
	int	nBatchSum;						/**<�������ܱ���*/
	char sSettleDateTime[5];			/**<����ʱ��*/
	char sNumAmount[fSettleData_len+1];	/**<��������*/
} STSETTLEPARAM;


/**
* @struct STBANKPARAM ������Ϣ
*/
typedef struct
{
	char cPosMerType;					/**<POS����*/
	char sRetCommHead[fRetCommHead_len+1];/**<����Ҫ��*/
	char cIsLogin;						/**<ǩ����ʶ*/
	char cIsLock;						/**<������ʶ*/
	char szTraceNo[fTraceNo_len+1];		/**<��ˮ��*/
	char szBatchNo[fBatchNo_len+1];		/**<���κ�*/
	char szInvoiceNo[fBatchNo_len+1];	/**<Ʊ�ݺ�*/
	ulong ulDialSuccSum;				/**<��ͨ����*/
	ulong ulDialSum;					/**<���Ŵ���*/
	long lTimeCounter;					/**<ʱ�������*/
	char sLastLoginDateTime[5];			/**<�ϻز���Ա��¼ʱ��*/
	char szLastLoginOper[OPERCODELEN+1];/**<�ϻص�¼����Ա����*/
	char cLastLoginOperRole;			/**<�ϻص�¼����Ա����*/
	int nMCTimes;						/**<���ؽ���ʱ��ʱ������*/
	int nMCRandom;						/**<���ؽ���ʱ�������*/
	char cIsDownKek;					/**<�Ƿ���Ҫ����KEK*/
} STBANKPARAM;

/**
* @struct STEMVPARAM EMV������Ϣ
*/
typedef struct
{
	int	nTransSerial;				/**<EMV���׼�����*/
	int	nOfflineUpNum;				/**<EMV�ѻ��������ͱ���*/
	int	nOnlineUpNum;				/**<EMV�����������ͱ���*/
	int	nOfflineFailUpNum;			/**<EMV�ѻ�ʧ�ܽ������ͱ���*/
	int	nOnlineARPCErrUpNum;		/**<EMV�����ɹ���ARPC���������ͱ���*/
	char cIsDownCAPKFlag;			/**<�Ƿ���Ҫ���ز���(EMV�����ع�Կ)*/
	char cIsDownAIDFlag;			/**<�Ƿ���Ҫ���ز���(EMV��ic������)*/
	char cIsScriptInform;			/**<�ű�����֪ͨ��ʶ��'0'-��'1'-��*/
	char sScriptData[256];			/**<�ű���������*/
} STEMVPARAM;


/**
* @struct STEXTAPPPARAM ��չӦ�ò���
*/
typedef struct
{
	 char szBankCode[3][8+1];
	 char szBankName[3][20+1];
	 char szShowName[8+1];
	 char szPrnLogoName[50];
	 uint unPrnLogoXpos;
	char szReserve[600];				/**<������*/
}STEXTAPPPARAM;

 


/**
* ����ȫ�ֱ���
*/
static STAPPPOSPARAM gstAppPosParam;	/**<POS�����ļ�*/
static STSETTLEPARAM gstSettleParam;	/**<��������*/
static STREVERSALPARAM gstReversalParam;/**<��������*/
static STBANKPARAM gstBankParam;		/**<�յ��в���*/
static STEMVPARAM gstEmvParam;			/**<EMV����*/
static STEXTAPPPARAM gstExtAppParam;       /**<Ӧ����չ����*/


	
static char gcIsQpboc = NO;
static char gcFirstRunFlag = NO;     /*�״�װ����ʶ,���ڴ���ǩ��+�²���+�¹�Կ�������в��Ҷ�**/
/**
* �����ڲ�����
*/


/**
* �ӿں���ʵ��
*/

/**
* @brief У����POS�Ƿ��ǵ�һ�����У��״�������Ҫ��ʼ����
*		���ʼ�����������޸İ汾�ţ�����ʱPOS�Զ���ʼ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IsFirstRun()
{
	int	i;
	char cCh=0;
	char sBuf[fPosCheck_len+1]= {0};
	char szTmp[fPosCheck_len+1]= {0};

	memset(szTmp,0,sizeof(szTmp));
	strcpy(szTmp,APPCHECKVALUE);

	memset(sBuf,0,sizeof(sBuf));
	PubGetVar(sBuf,fPosCheck_off,fPosCheck_len);

	for(i=0; i<7; i++)
	{
		cCh^=sBuf[i];
	}

	if((memcmp(szTmp,sBuf,7)==0) &&(cCh==sBuf[7]))
	{
		return APP_SUCC;
	}
	else
	{
		return APP_FAIL;
	}
}

/**
* @brief ����POS�״�������Ϣ��ʹ�ð汾�ſ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetAppCheckValue()
{
	int	i;
	char cCh=0,sBuf[fPosCheck_len+1]= {0};

	memset(sBuf,0,sizeof(sBuf));
	strcpy(sBuf,APPCHECKVALUE);

	for(i=0; i<7; i++)
	{
		cCh^=sBuf[i];
	}
	sBuf[i]=cCh;
	PubSaveVar(sBuf,fPosCheck_off,fPosCheck_len);
	return APP_SUCC;
}

/**
* @brief ��ʼ����Ĭ��ֵ�������һ�����б�����ã����ֹ���ʼ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitPosDefaultParam()
{
	STRECFILE stPosParamfile;
	char szBuf[fNVR_Addr_End-fNVR_Addr_Start+1];
	char cStatus = NO;
#if defined(EMV_IC)	
	char cIsPack9F36 = TRUE;
#endif
#ifdef USE_TMS	
	char cTmpNum = 0;
#endif
	/**<���FRAM����������*/
	memset(szBuf,0,sizeof(szBuf));
	PubSaveVar(szBuf,fNVR_Addr_Start,fNVR_Addr_End-fNVR_Addr_Start);

	/**<POS����*/
	memset(&stPosParamfile, 0, sizeof(STRECFILE));
	strcpy( stPosParamfile.szFileName, FILE_APPPOSPARAM);
	stPosParamfile.cIsIndex = FILE_NOCREATEINDEX;						/**< ���������ļ�*/
	stPosParamfile.unMaxOneRecLen = sizeof(STAPPPOSPARAM);
	stPosParamfile.unIndex1Start =0;
	stPosParamfile.unIndex1Len =0;
	stPosParamfile.unIndex2Start =0;
	stPosParamfile.unIndex2Len =0;
	ASSERT(PubCreatRecFile(&stPosParamfile));

	memset(&gstAppPosParam,0,sizeof(gstAppPosParam));
	PubAddRec(FILE_APPPOSPARAM,(char *)&gstAppPosParam);
	strcpy(gstAppPosParam.szTerminalId,"12345676");
	strcpy(gstAppPosParam.szMerchantId,"123456789012345");
	strcpy(gstAppPosParam.szAppDispname,APP_NAMEDESC);
	strcpy(gstAppPosParam.szMerchantNameCn,"�й�����");
	strcpy(gstAppPosParam.szMerchantNameEn,"China Unionpay");
//#if defined(USE_TMS)	������TMS���Ƿ�TMS �����ն˺ź��̻��� д�����أ���֤�����ܴ�ӡ����
	MC_SetPubParam(APP_POSID, gstAppPosParam.szTerminalId, 8);
	MC_SetPubParam(APP_MERCHANTID, gstAppPosParam.szMerchantId, 15);
	MC_SetPubParam(APP_DISPNAME, gstAppPosParam.szAppDispname, 0);
//#endif	
#if defined(EMV_IC)
	EmvSetTerminalId((uchar *)gstAppPosParam.szTerminalId);
	EmvSetMerchantId((uchar *)gstAppPosParam.szMerchantId);
	EmvSetMerchantName((unsigned char *)gstAppPosParam.szMerchantNameCn);
#endif	
	strcpy(gstAppPosParam.szAppType,APPTYPE);
	memcpy(gstAppPosParam.szAppVer,APP_VERSION,2);
	gstAppPosParam.cPosState=POS_STATE_NORMAL;
	strcpy(gstAppPosParam.szBusinessBankId,"0000");
	strcpy(gstAppPosParam.szLocalCode,"0000");
#if 0
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_PINPAD, NULL))
	{
		gstAppPosParam.cIsPinPad=YES;
	}
	else
	{
		gstAppPosParam.cIsPinPad=NO;
	}
#else	
	gstAppPosParam.cIsPinPad=NO;
#endif	
	gstAppPosParam.cPinPadAuxNo=PORT_NUM_COM2;
	strcpy(gstAppPosParam.szPinPadTimeOut,"60");	/**<Ĭ�ϳ�ʱ120��*/
	strcpy(gstAppPosParam.szSecurityPwd,SECURITYPWD);
	strcpy(gstAppPosParam.szMainKeyNo,"00");
	gstAppPosParam.cEncyptMode=DESMODE_3DES;/**<Ĭ��˫��������3des*/
	gstAppPosParam.cPinEncyptMode=PINTYPE_WITHPAN;
	gstAppPosParam.cIsAutoLogOut=YES;
	gstAppPosParam.cIsPrintWaterRec=YES;
	gstAppPosParam.cIsPrintFailWaterRec=YES;
	gstAppPosParam.cIsNewTicket='0';
	gstAppPosParam.cIsTicketWithEn='0';
	gstAppPosParam.cIsPreauthShieldPan = NO;
	gstAppPosParam.cSafeKeepYear='2';
	gstAppPosParam.cIsPrintErrReport=NO;
	gstAppPosParam.cPrintPageCount='1';
	strcpy(gstAppPosParam.szMaxTransCount,"300");
	strcpy(gstAppPosParam.szMaxRefundAmount,"000000100000");		/**<1000Ԫ���Է�Ϊ��λ*/
	memcpy(gstAppPosParam.sTraditionSwitch,"\xFF\xFF\xFF\xFF",4);/*��ͳ��*/
	memcpy(gstAppPosParam.sECashSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sWalletSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sInstallmentSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sBonusSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sPhoneChipSaleSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sAppointmentSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sOrderSwitch,"\xFF\xFF\xFF\xFF",4);
	memcpy(gstAppPosParam.sOtherSwitch,"\xFF\xFF\xFF\xFF",4);
#if defined(EMV_IC)
	gstAppPosParam.cIsIcFlag=YES;
#else
	gstAppPosParam.cIsIcFlag=NO;
#endif
	gstAppPosParam.cIsIcConfirmFlag=NO;
	gstAppPosParam.cIsCardInput=YES;
#if !defined(USE_TMS)
	gstAppPosParam.cIsSaleVoidStrip=YES;
#else
	gstAppPosParam.cIsSaleVoidStrip=NO;
#endif
	gstAppPosParam.cIsAuthSaleVoidStrip=NO;
	gstAppPosParam.cIsVoidPin=NO;
	gstAppPosParam.cIsAuthSalePin=NO;
	gstAppPosParam.cDefaultTransType='1';
	gstAppPosParam.cAuthSaleMode='0';
	gstAppPosParam.cIsTipFlag = YES;
	memcpy(gstAppPosParam.szTipRate, "15", 2);
	gstAppPosParam.cIsDispEMV_TVRTSI = NO;
	memcpy(gstAppPosParam.sUID, "1234567812345678", 16);
	gstAppPosParam.cIsShieldPan = YES;
#if defined(EMV_IC)
	gstAppPosParam.cIsExRF = YES;
	gstAppPosParam.cIsUsePP60RF= NO;
	/*20120229�޸�Ĭ�϶�֧�ַǽ�*/
	gstAppPosParam.cIsSupportRF = YES;
	if(APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_RADIO_FREQUENCY, NULL))
	{
		gstAppPosParam.cIsExRF = NO;/**<����*/
	}
#endif
	strcpy(gstAppPosParam.szHideMenuPwd,HIDEOPERPWD);
	gstAppPosParam.cIsPntChAcquirer = YES;
	gstAppPosParam.cIsPntChCardScheme = YES;
	gstAppPosParam.cPntTitleMode = YES;
	strcpy(gstAppPosParam.szPntTitleCn,"����");
	//strcpy(gstAppPosParam.szHotLine,"400-622-8888");
	gstAppPosParam.cIsAdminPwd = YES;
#ifdef USE_TMS
	gstAppPosParam.cIsModify= YES;
#else
	gstAppPosParam.cIsModify= YES;
#endif
	gstAppPosParam.cPntFontSize ='1';
	gstAppPosParam.cIsPrintMinus=YES;
	gstAppPosParam.cIsPrintAllTrans=YES;
	strcpy(gstAppPosParam.szAppDispname,APP_NAMEDESC);
	strcpy(gstAppPosParam.szMaxOffSendNum, "10");

	gstAppPosParam.nSettleNum = 0;
	gstAppPosParam.cIsReprintSettle='1';

	gstAppPosParam.cTmsReSendNum = 3;
#if defined(SUPPORT_ELECSIGN)
	strcpy(gstAppPosParam.szElecTimeOut, "150");
	gstAppPosParam.cElecSendTime = 3;
	gstAppPosParam.cElecSignSendResult=YES;
	gstAppPosParam.cIsElecSubcontractFlag = NO;
	strcpy(gstAppPosParam.sElecSubcontractSize, "900");
	memcpy(gstAppPosParam.sLastElecSignSendNum,"\x00\x00\x00",3);
	gstAppPosParam.cIsSupportElecSign = NO;
#endif

	strcpy(gstAppPosParam.szRfSearchDelayTimeMs, "150");
	gstAppPosParam.cIsRfSearchCardDelay = NO;
	gstAppPosParam.cIsOutsideScaner = YES;
	gstAppPosParam.cIsHaveScanner = YES;
	PubSetReadRfDelay(0);

	ASSERT(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	/**<��������*/
	memset(&gstSettleParam,0,sizeof(gstSettleParam));
	ClearSettle();

	/**<��������*/
	memset(&gstReversalParam,0,sizeof(gstReversalParam));
	SetVarIsReversal(NO);
	SetVarHaveReversalNum(0);
	SetVarReversalData(gstReversalParam.sReversalData,sizeof(gstReversalParam.sReversalData));

	/**<���в���*/
	memset(&gstBankParam,0,sizeof(gstBankParam));
	SetVarPosMerType(0);
	SetVarCommHead("\x60\x22\x00\x00\x00\x00");
	SetVarIsLogin(NO);
	SetVarIsLock(NO);
	SetVarTraceNo("000001");
	SetVarBatchNo("000001");
	SetVarDialSuccSum(0);
	SetVarDialSum(0);
	SetVarTimeCounter(time(NULL));
	SetVarLastLoginDateTime("\x00\x00\x00\x00\x00");
	SetVarLastOperInfo("01", NORMALOPER);
	SetVarMCEnterTimeAndRandom(0xFFFFFFFF, 0xFFFFFFFF);
	SetVarOfflineUnSendNum(0);
	SetVarDownKek(YES);
#if defined (SUPPORT_ELECSIGN)
	SetVarElecSignUnSendNum(0);
#endif	
	/**<EMV����*/
	memset(&gstEmvParam,0,sizeof(gstEmvParam));
	SetVarEmvTransSerial(1);
	SetVarEmvOfflineUpNum(1);
	SetVarEmvOnlineUpNum(1);
	SetVarEmvOfflineUpNum(1);
	SetVarEmvOnlineUpNum(1);
	SetVarEMVOfflineFailHaltFlag(1);
	SetVarEMVOnlineARPCErrHaltFlag(1);
	SetVarEmvIsDownCAPKFlag(YES);
	SetVarEmvIsDownAIDFlag(YES);
	SetVarEmvIsScriptInform(NO);
	SetVarEmvScriptData(gstEmvParam.sScriptData,sizeof(gstEmvParam.sScriptData));

	/**<�����ŵ����ܱ�ʶ*/
	SetVarIsEncryptTrack(YES);

	/**<Ĭ�����ò�֧��С�����Ȩ*/
	SetVarIsSmallGeneAuth(NO);

	/**<Ӧ����չ����*/
	memset(&stPosParamfile, 0, sizeof(STRECFILE));
	strcpy( stPosParamfile.szFileName, FILE_EXTAPPPOSPARAM);
	stPosParamfile.cIsIndex = FILE_NOCREATEINDEX;
	stPosParamfile.unMaxOneRecLen = sizeof(STEXTAPPPARAM);
	stPosParamfile.unIndex1Start =0;
	stPosParamfile.unIndex1Len =0;
	stPosParamfile.unIndex2Start =0;
	stPosParamfile.unIndex2Len =0;
	ASSERT(PubCreatRecFile(&stPosParamfile));

	memset(&gstExtAppParam, 0, sizeof(gstExtAppParam));
	strcpy(gstExtAppParam.szShowName, "���ܸ�");
	strcpy(gstExtAppParam.szPrnLogoName,"TT370x80.bmp");
	gstExtAppParam.unPrnLogoXpos = 6;

	ASSERT(PubAddRec(FILE_EXTAPPPOSPARAM, (char *)&gstExtAppParam));

	
	cStatus = NO;
	PubSaveVar(&cStatus, fIsPosParamDown_off, fIsPosParamDown_len);
	PubSaveVar(&cStatus, fIsPosStatusSend_off, fIsPosStatusSend_len);
	PubSaveVar(&cStatus, fIsReLogin_off, fIsReLogin_len);
#if defined(EMV_IC)
	cStatus = YES;
	PubSaveVar(&cIsPack9F36, fIsReversalPack9F36_off, fIsReversalPack9F36_len);
#endif
	cStatus = NO;
#if defined(USE_TMS)
	PubSaveVar(&cStatus, fIsTmsDown_off, fIsTmsDown_len);
	cTmpNum = 0;
	PubSaveVar(&cTmpNum, fTmsFailNum_off, fTmsFailNum_len);
	PubSaveVar(&cTmpNum, fTmsEscNum_off, fTmsEscNum_len);
#endif
	PubSaveVar(&cStatus, fIsBlackListDown_off, fIsBlackListDown_len);
	PubSaveVar(UPDATE_CTRL, fUpdateCtrl_off, fUpdateCtrl_len);

	SetVarHaveScriptAdviseNum(0);
	if(YES == GetVarIsIcFlag())
	{
		SetVarFirstRunFlag(YES);
	}
	/**<����Ӧ����ֵ֤*/
	SetAppCheckValue();
	return APP_SUCC;
}
/**
* @brief �Ӳ����ļ��������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetParamFromIni()
{
	const char *szIniFile = FILE_PARAM_INI;
	int nIniHandle;
	int nValueLen, nValue,nCommValue;
	char szBuf[100], szTmp[100];
//	int nNum_Tradition = 11, nNum_ECash = 7,nNum_Wallet = 4,nNum_Installment = 2,nNum_Bonus = 6;
//	int nNum_PhoneChipSale = 9, nNum_Appointment = 2,nNum_Order = 8,nNum_Other = 2;
//	int i=0;

	STAPPCOMMPARAM stAppCommParam;
	STAPPCOMMRESERVE stAppCommReserve;

	memset(&stAppCommParam, 0, sizeof(STAPPCOMMPARAM));
	memset(&stAppCommReserve, 0, sizeof(STAPPCOMMRESERVE));
	GetAppCommParam(&stAppCommParam);
	GetAppCommReserve(&stAppCommReserve);

	ASSERT_QUIT(PubOpenFile (szIniFile, "r", &nIniHandle));

	PubClearAll();
	PubDisplayStrInline(0, 3, "���ڽ��в�������");
	PubUpdateWindow();

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTID",  sizeof(gstAppPosParam.szMerchantId) - 1, gstAppPosParam.szMerchantId, &nValueLen), "ȡ�̻���ʧ��");
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "POSID",  sizeof(gstAppPosParam.szTerminalId) - 1, gstAppPosParam.szTerminalId, &nValueLen), "ȡ�ն˺�ʧ��");
	memset(szBuf,0,sizeof(szBuf));
	if (PubGetINIItemStr (nIniHandle, "BASE", "APPDISPNAME", 14,szBuf, &nValue) == APP_SUCC)
	{
		if(strlen(szBuf)>0)
		{
			strcpy(gstAppPosParam.szAppDispname,szBuf);
		}
	}
#if defined(EMV_IC)
	EmvSetTerminalId((uchar *)gstAppPosParam.szTerminalId);
	EmvSetMerchantId((uchar *)gstAppPosParam.szMerchantId);
#endif	
//#if defined(USE_TMS)
	MC_SetPubParam(APP_POSID, gstAppPosParam.szTerminalId, 8);
	MC_SetPubParam(APP_MERCHANTID, gstAppPosParam.szMerchantId, 15);
	MC_SetPubParam(APP_DISPNAME, gstAppPosParam.szAppDispname, 0);
//#endif

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "TRACENO",  sizeof(gstBankParam.szTraceNo) - 1, gstBankParam.szTraceNo, &nValueLen), "ȡ��ˮ��ʧ��");
	PubSaveVar(gstBankParam.szTraceNo,fTraceNo_off,fTraceNo_len);

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "BATCHNO",  sizeof(gstBankParam.szBatchNo) - 1, gstBankParam.szBatchNo, &nValueLen), "ȡ���κ�ʧ��");
	PubSaveVar(gstBankParam.szBatchNo,fBatchNo_off,fBatchNo_len);

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "PRTCOUNT",  &nValue), "ȡ��ӡҳ��ʧ��");
	gstAppPosParam.cPrintPageCount = nValue + '0';
#if 0
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTNAME",  sizeof(gstAppPosParam.szMerchantNameCn) - 1, gstAppPosParam.szMerchantNameCn, &nValueLen), "ȡ�̻�����ʧ��");
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTNAMEEN", sizeof(gstAppPosParam.szMerchantNameEn) - 1, gstAppPosParam.szMerchantNameEn, &nValueLen), "ȡ�̻���Ӣ������ʧ��");

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "REFUNDAMOUNT",  &nValue), "ȡ�˻������ʧ��");
	sprintf(gstAppPosParam.szMaxRefundAmount, "%012d", nValue);

	/*�ۼ��Զ����ͱ���-�¸���2011-09-01*/
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "LOCALCODE",  sizeof(gstAppPosParam.szLocalCode) - 1, gstAppPosParam.szLocalCode, &nValueLen), "ȡ��������ʧ��");

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCODE",  sizeof(gstAppPosParam.szBusinessBankId) - 1, gstAppPosParam.szBusinessBankId, &nValueLen), "ȡ���д���ʧ��");
	//�Ƿ�֧�ִŵ�����
	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "ENCPYTRACK", &nValue), "ȡ�Ƿ�ʹ�ôŵ�����ʧ��");
	if (nValue)
	{
		gstAppPosParam.cIsEncryptTrack = YES;
	}
	else
	{
		gstAppPosParam.cIsEncryptTrack = NO;
	}
#endif

	/*ǩ����̧ͷ*/
	if(APP_SUCC == PubGetINIItemInt(nIniHandle, "BASE", "PRINTTITLEMODE", &nValue))
	{
		if(nValue)
		{
			gstAppPosParam.cPntTitleMode = YES;
		}
		else
		{
			gstAppPosParam.cPntTitleMode = NO;
		}
	}

	memset(szBuf,0,sizeof(szBuf));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "BASE", "PRINTTITLECN", sizeof(gstAppPosParam.szPntTitleCn)-1, szBuf, &nValueLen))
	{
		memset(gstAppPosParam.szPntTitleCn,0,sizeof(gstAppPosParam.szPntTitleCn));
		memcpy(gstAppPosParam.szPntTitleCn,szBuf,nValueLen);
	}

	/*���ߺ���*/
	memset(szBuf,0,sizeof(szBuf));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "BASE", "HOTLINE", sizeof(gstAppPosParam.szHotLine)-1, szBuf, &nValueLen))
	{
		memset(gstAppPosParam.szHotLine,0,sizeof(gstAppPosParam.szHotLine));
		memcpy(gstAppPosParam.szHotLine,szBuf,nValueLen);
	}

	/*��������Ա����*/
	memset(szBuf,0,sizeof(szBuf));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "BASE", "SUPERPWD", 8, szBuf, &nValueLen))
	{
		if(nValueLen == 8)
		{
			ASSERT_PARA_FAIL(SetAdminPwd(szBuf),"���ó�������Ա����ʧ��");
		}
		else
		{
			PubMsgDlg("��������", "��������Ա���볤�ȴ�", 3, 3);
		}
	}

	ASSERT_PARA_FAIL(PubGetINIItemInt(nIniHandle, "BASE", "REVERSALNUM",   &nValue), "ȡ��������ʧ��");
	stAppCommParam.cReSendNum= nValue;

	if (PubGetINIItemInt (nIniHandle, "BASE", "ISSCANGUN",  &nValue) == APP_SUCC)
	{
		gstAppPosParam.cIsHaveScanner= nValue + '0';
	}
	if (PubGetINIItemInt (nIniHandle, "BASE", "SCANGUNOUT",  &nValue) == APP_SUCC)
	{
		gstAppPosParam.cIsOutsideScaner = nValue + '0';
	}
	if (PubGetINIItemInt (nIniHandle, "BASE", "PRINTSIZE",  &nValue) == APP_SUCC)
	{
		gstAppPosParam.cPntFontSize= nValue + '0';
	}

	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_PINPAD, NULL))
	{
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "PIN", "PINPAD", &nValue), " ȡ������̱�־ʧ��");
		gstAppPosParam.cIsPinPad = (nValue ==1 ? YES : NO);
	}
	else
		gstAppPosParam.cIsPinPad=NO;

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "COMM", "TIMEOUT",  &nValue), "ȡ���׳�ʱʱ��ʧ��");
	stAppCommParam.cTimeOut = nValue;
	if (PubGetINIItemInt (nIniHandle, "COMM", "PREDIAL",  &nValue) == APP_SUCC)
	{
		stAppCommParam.cPreDialFlag = nValue + '0';
	}
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "TPDU",  10, szBuf, &nValueLen), "ȡ����TPDUʧ��");
	PubAscToHex((uchar *)szBuf, 10, 0, (uchar *)stAppCommParam.sTpdu);

	if (PubGetINIItemInt (nIniHandle, "COMM", "ISSSL",  &nValue) == APP_SUCC)
	{
		stAppCommParam.cIsSSL = nValue;
	}

	if (PubGetINIItemInt (nIniHandle, "COMM", "ISSENDSSL",  &nValue) == APP_SUCC)
	{
		stAppCommParam.cIsSendSSLAuth = nValue;
	}

	if (PubGetINIItemInt (nIniHandle, "COMM", "ISDNS",  &nValue) == APP_SUCC)
	{
		stAppCommParam.cIsDns = nValue;
	}

	if (PubGetINIItemInt (nIniHandle, "COMM", "ISDHCP",  &nValue) == APP_SUCC)
	{
		stAppCommReserve.cIsDHCP = nValue;
	}

	if(stAppCommParam.cIsDns != 0)
	{
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNS",  sizeof(stAppCommParam.szDNSIp1) - 1, stAppCommParam.szDNSIp1, &nValueLen), "ȡ����������ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNNAME1",  sizeof(stAppCommParam.szDomain) - 1, stAppCommParam.szDomain, &nValueLen), "ȡ����1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNNAME2",  sizeof(stAppCommReserve.szDomain2) - 1, stAppCommReserve.szDomain2, &nValueLen), "ȡ����2ʧ��");
	}

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "COMM", "COMMTYPE",  &nCommValue), "ȡͨѶ����ʧ��");
	//�����󲻹�ʲôͨѶ���ͣ���Ҫ�Ѳ���ȡ����
	if(nCommValue != 0)
	{
		//�ǲ�������ҲҪ�Ѳ��Ų���ȡ����
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "DIAL", "DIALNUM",  &nValue), " ȡ���Ŵ���ʧ��");
		stAppCommParam.cReDialNum = nValue;

		nValueLen = 0;
		memset(stAppCommParam.szPreDial, 0, sizeof(stAppCommParam.szPreDial));
		PubGetINIItemStr (nIniHandle, "DIAL", "PREPHONE",  sizeof(stAppCommParam.szPreDial) - 1, stAppCommParam.szPreDial, &nValueLen);
		MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, (uint)nValueLen);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE1",  sizeof(stAppCommParam.szTelNum1) - 1, stAppCommParam.szTelNum1, &nValueLen), "ȡ�绰����1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE2",  sizeof(stAppCommParam.szTelNum2) - 1, stAppCommParam.szTelNum2, &nValueLen), "ȡ�绰����2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE3",  sizeof(stAppCommParam.szTelNum3) - 1, stAppCommParam.szTelNum3, &nValueLen), "ȡ�绰����3ʧ��");
	}
	if(nCommValue != 1)
	{
		//����������ҲҪ����̫�����в���ȡ����
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "ȡ����IP��ַʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "ȡ������������ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "ȡ���ص�ַʧ��");
	}
	if(nCommValue != 2 && nCommValue != 3)
	{
		//����������ҲҪȡGPRS�Ĳ���
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "ȡGPRS�û���ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "ȡGPRS�û�����ʧ��");

		//������������
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"GPRS", "MODE", &nValue), "ȡ������������ʧ��");
		stAppCommParam.cMode = nValue;
	}
	if(nCommValue != 3)
	{
		//��GPRSҲҪ��GPRS���в���ȡ����
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "APN", sizeof(stAppCommParam.szAPN1)-1, stAppCommParam.szAPN1, &nValue), "ȡGPRS APNʧ��");
		strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
	}
	if(nCommValue != 1 && nCommValue != 2 && nCommValue != 3)
	{
		//��IP�˿�����ȡGPRS�Ĳ���
		//������ַ1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "ȡ������IP��ַ1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT1",  &nValue), "ȡ����˿ں�1ʧ��");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//������ַ2������������ַ��
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "ȡ������IP��ַ2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT2",  &nValue), "ȡ����˿ں�2ʧ��");
		sprintf(stAppCommParam.szPort2, "%d", nValue);
	}

	switch(nCommValue)
	{
	case 0:
		stAppCommParam.cCommType = COMM_DIAL;
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "DIAL", "DIALNUM",  &nValue), " ȡ���Ŵ���ʧ��");
		stAppCommParam.cReDialNum = nValue;

		nValueLen = 0;
		memset(stAppCommParam.szPreDial, 0, sizeof(stAppCommParam.szPreDial));
		PubGetINIItemStr (nIniHandle, "DIAL", "PREPHONE",  sizeof(stAppCommParam.szPreDial) - 1, stAppCommParam.szPreDial, &nValueLen);
		MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, (uint)nValueLen);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE1",  sizeof(stAppCommParam.szTelNum1) - 1, stAppCommParam.szTelNum1, &nValueLen), "ȡ�绰����1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE2",  sizeof(stAppCommParam.szTelNum2) - 1, stAppCommParam.szTelNum2, &nValueLen), "ȡ�绰����2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE3",  sizeof(stAppCommParam.szTelNum3) - 1, stAppCommParam.szTelNum3, &nValueLen), "ȡ�绰����3ʧ��");
		break;
	case 1:
		stAppCommParam.cCommType =COMM_ETH;

		//������ַ1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "ȡ������IP��ַ1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "LINE", "PORT1",  &nValue), "ȡ����˿ں�1ʧ��");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//������ַ2������������ַ��
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "ȡ������IP��ַ2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "LINE", "PORT2",  &nValue), "ȡ����˿ں�2ʧ��");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "ȡ����IP��ַʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "ȡ������������ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "ȡ���ص�ַʧ��");
		break;
	case 2://CDMA
		stAppCommParam.cCommType =COMM_CDMA;
		//������ַ1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "ȡ������IP��ַ1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "CDMA", "PORT1",  &nValue), "ȡ����˿ں�1ʧ��");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//������ַ2������������ַ��
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "ȡ������IP��ַ2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "CDMA", "PORT2",  &nValue), "ȡ����˿ں�2ʧ��");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "ȡCDMA�û���ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "ȡCDMA����ʧ��");

		//������������
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"CDMA", "MODE", &nValue), "ȡ������������ʧ��");
		stAppCommParam.cMode = nValue;
		break;
	case 3://GPRS
		stAppCommParam.cCommType =COMM_GPRS;
		//������ַ1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "ȡ������IP��ַ1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT1",  &nValue), "ȡ����˿ں�1ʧ��");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//������ַ2������������ַ��
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "ȡ������IP��ַ2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT2",  &nValue), "ȡ����˿ں�2ʧ��");
		sprintf(stAppCommParam.szPort2, "%d", nValue);
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "APN", sizeof(stAppCommParam.szAPN1)-1, stAppCommParam.szAPN1, &nValue), "ȡGPRS APNʧ��");
		strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "ȡGPRS�û���ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "ȡGPRS�û�����ʧ��");

		//������������
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"GPRS", "MODE", &nValue), "ȡ������������ʧ��");
		stAppCommParam.cMode = nValue;
		break;
	case 4:
		stAppCommParam.cCommType =COMM_RS232;
		break;
	case 5:
		stAppCommParam.cCommType= COMM_WIFI;
		//������ַ1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "ȡ������IP��ַ1ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "PORT1",  &nValue), "ȡ����˿ں�1ʧ��");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//������ַ2������������ַ��
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "ȡ������IP��ַ2ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "PORT2",  &nValue), "ȡ����˿ں�2ʧ��");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "ȡ����IP��ַʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "ȡ������������ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "ȡ���ص�ַʧ��");

		//ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "KEYTYPE",  &nValue), "ȡWIFI�����ʽʧ��");
		//stAppCommReserve.cWifiKeyType = nValue;

		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "MODE",  &nValue), "ȡWIFI����ģʽʧ��");
		switch(nValue)
		{
		case 1:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WEP_OPEN;
			break;
		case 2:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WEP_SHARED;
			break;
		case 3:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA;
			break;
		case 4:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA2;
			break;
		case 5:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA;
			break;
		default:
			stAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA2;
			break;
		}
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SSID", sizeof(stAppCommReserve.szWifiSsid)-1, stAppCommReserve.szWifiSsid, &nValue), "ȡWIFI�ȵ���ʧ��");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "PWD", sizeof(stAppCommReserve.szWifiKey)-1, stAppCommReserve.szWifiKey, &nValue), "ȡWIFI����ʧ��");
		break;
	default:
		stAppCommParam.cCommType =COMM_DIAL;
		break;
	}

	//���¹��ܽ���������Աʹ�ã��������ǵ�������Կ����ʽ������*.INI�ļ�����������������Կ
	{
		char szMasterKey[16+1] = {0};
		memset(gstAppPosParam.szMainKeyNo, 0, sizeof(gstAppPosParam.szMainKeyNo));
		PubGetINIItemStr(nIniHandle, "BASE", "KEYINDEX", sizeof(gstAppPosParam.szMainKeyNo) - 1, gstAppPosParam.szMainKeyNo, &nValueLen);
		nValue = atoi(gstAppPosParam.szMainKeyNo);

		if (APP_SUCC == PubGetINIItemStr (nIniHandle, "BASE", "MASTERKEY", 32, szBuf, &nValueLen))
		{
		/**<�����������Կ��ʱ�����ڶ�һ��һ���ˣ����ܶ�Ҫ��Ҫ����Կ*/
			if (nValueLen == 16 || nValueLen == 32)
			{
				if (nValueLen == 16)
				{
					gstAppPosParam.cEncyptMode = DESMODE_DES;
					PubAscToHex((unsigned char *)szBuf, 16, 0, (unsigned char *)szMasterKey);
				}
				else if (nValueLen == 32)
				{
					gstAppPosParam.cEncyptMode = DESMODE_3DES;
					PubAscToHex((unsigned char *)szBuf, 32, 0, (unsigned char *)szMasterKey);
				}
				if (ChkPinpad() == APP_SUCC)
				{
					PubLoadMainKey(nValue, szMasterKey, nValueLen>>1);	
				}
			}
			else	/**<��Կ����*/
			{
				PubMsgDlg("����", "��Կ���ȴ���", 3, 5);
			}
		}
	}	

	/**<Ӧ����չ����*/
	memset(szTmp, 0, sizeof(szTmp));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "EXT", "SHOWNAME",  8,  szTmp, &nValueLen))
	{
		memset(gstExtAppParam.szShowName, 0, sizeof(gstExtAppParam.szShowName));
	    strcpy(gstExtAppParam.szShowName, szTmp);
	}
#if 0	
	for(i = 0; i < 3; i++)
	{
		sprintf(szTmp, "BANKCODE%d", i+1);		
		PubGetINIItemStr(nIniHandle, "EXT", szTmp, 4, gstExtAppParam.szBankCode[i],&nValueLen);
		sprintf(szTmp, "BANKNAME%d", i+1);
		PubGetINIItemStr(nIniHandle, "EXT", szTmp, 20, gstExtAppParam.szBankName[i],&nValueLen);
	}
#endif	
	memset(szTmp, 0, sizeof(szTmp));
	if (APP_SUCC == PubGetINIItemStr(nIniHandle, "EXT", "PRNIMGNAME",  50, szTmp, &nValueLen))
	{
		memset(gstExtAppParam.szPrnLogoName, 0, sizeof(gstExtAppParam.szPrnLogoName));
		strcpy(gstExtAppParam.szPrnLogoName, szTmp);
	}

	memset(szTmp, 0, sizeof(szTmp));
	if (APP_SUCC == PubGetINIItemStr(nIniHandle, "EXT", "PRNXPOS",  3, szTmp, &nValueLen))
	{
		gstExtAppParam.unPrnLogoXpos = (uint)atoi(szTmp);
	}
	ASSERT_FAIL(PubUpdateRec(FILE_EXTAPPPOSPARAM,1,(char *)&gstExtAppParam));

	
	PubCloseFile(&nIniHandle);

	sprintf(szBuf, "�ն˺�:%8.8s\n�̻���:\n%15.15s", gstAppPosParam.szTerminalId, gstAppPosParam.szMerchantId);
	PubMsgDlg("�ն˲���", szBuf, 0, 3);

	//sprintf(szBuf, "�̻�����:\n%s", gstAppPosParam.szMerchantNameCn);

	PubDelFile(szIniFile);

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	ASSERT_FAIL(SetAppCommParam(&stAppCommParam));
	ASSERT_FAIL(SetAppCommReserve(&stAppCommReserve));
#if defined(EMV_IC)
	EmvSetMerchantId((uchar *)gstAppPosParam.szMerchantId);
	EmvSetMerchantName((uchar *)gstAppPosParam.szMerchantNameCn);
	EmvSetTerminalId((uchar *)gstAppPosParam.szTerminalId);
#endif
	return APP_SUCC;
}

/**
* @brief ����POS����,����ʱ����һ�μ���,���Ӳ���ʱ�����ڴ���Ӧ���ӣ�
*		��InitPosDefaultParam���ʹ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ExportPosParam()
{
	char szFramBuf[fNVR_Addr_End-fNVR_Addr_Start+1];


	/**<���FRAM����������*/
	memset(szFramBuf,0,sizeof(szFramBuf));
	PubGetVar(szFramBuf,fNVR_Addr_Start,fNVR_Addr_End-fNVR_Addr_Start);
	
	/**<POS����*/
	memset(&gstAppPosParam,0,sizeof(gstAppPosParam));
	ASSERT_FAIL(PubReadOneRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));


	/**<��������*/
	memset(&gstSettleParam,0,sizeof(gstSettleParam));
	InitSettleData();
	memcpy((char *)&(gstSettleParam.nWaterSum), szFramBuf + fWaterSum_off, fWaterSum_len);
	memcpy((char *)&(gstSettleParam.nHaveReSendNum), szFramBuf + fHaveReSendNum_off, fHaveReSendNum_len);
	memcpy((char *)&(gstSettleParam.cPrintSettleHalt), szFramBuf + fSettlePrintHaltFlag_off, fSettlePrintHaltFlag_len);
	memcpy((char *)&(gstSettleParam.cPrintDetialHalt), szFramBuf + fPrintDetialHaltFlag_off, fPrintDetialHaltFlag_len);
	memcpy((char *)&(gstSettleParam.cPrintWaterHaltFlag), szFramBuf + fPrintWaterHaltFlag_off, fPrintWaterHaltFlag_len);
	memcpy((char *)&(gstSettleParam.cCnCardFlag), szFramBuf + fCnCardFlag_off, fCnCardFlag_len);
	memcpy((char *)&(gstSettleParam.cEnCardFlag), szFramBuf + fEnCardFlag_off, fEnCardFlag_len);
	memcpy((char *)&(gstSettleParam.cBatchHaltFlag), szFramBuf + fBatchHaltFlag_off, fBatchHaltFlag_len);
	memcpy((char *)&(gstSettleParam.cClrSettleDataFlag), szFramBuf + fClrSettleDataFlag_off, fClrSettleDataFlag_len);
	memcpy((char *)&(gstSettleParam.nFinanceHaltFlag), szFramBuf + fFinanceHaltNum_off, fFinanceHaltNum_len);
	memcpy((char *)&(gstSettleParam.nMessageHaltFlag), szFramBuf + fMessageHaltNum_off, fMessageHaltNum_len);
	memcpy((char *)&(gstSettleParam.nBatchMagOfflinHaltFlag), szFramBuf + fBatchMagOfflineHaltNum_off, fBatchMagOfflineHaltNum_len);
	memcpy((char *)&(gstSettleParam.nIcPurchaseHaltFlag), szFramBuf + fIcPurchaseHaltNum_off, fIcPurchaseHaltNum_len);
	memcpy((char *)&(gstSettleParam.nIcLoadHaltFlag), szFramBuf + fIcLoadHaltNum_off, fIcLoadHaltNum_len);
	memcpy((char *)&(gstSettleParam.nBatchSum), szFramBuf + fBatchUpSum_off, fBatchUpSum_len);
	memcpy((char *)&(gstSettleParam.sSettleDateTime), szFramBuf + fSettleDateTime_off, fSettleDateTime_len);
	memcpy((char *)&(gstSettleParam.sNumAmount), szFramBuf + fSettleData_off, fSettleData_len);

	/**<��������*/
	memset(&gstReversalParam,0,sizeof(gstReversalParam));
	memcpy((char *)&gstReversalParam.cIsReversal,szFramBuf + fIsReversal_off,fIsReversal_len);
	memcpy((char *)&gstReversalParam.nHaveReversalNum,szFramBuf + fHaveReversalNum_off,fHaveReversalNum_len);
	memcpy((char *)&gstReversalParam.nReversalLen,szFramBuf + fReversalLen_off,fReversalLen_len);
	memcpy((char *)&gstReversalParam.sReversalData,szFramBuf + fReversalData_off,fReversalData_len);

	/**<���в���*/
	memset(&gstBankParam,0,sizeof(gstBankParam));
	memcpy((char *)&gstBankParam.cPosMerType,szFramBuf + fPosMerType_off,fPosMerType_len);
	memcpy(gstBankParam.sRetCommHead,szFramBuf + fRetCommHead_off,fRetCommHead_len);
	memcpy((char *)&gstBankParam.cIsLogin,szFramBuf + fIsLogin_off,fIsLogin_len);
	memcpy((char *)&gstBankParam.cIsLock,szFramBuf + fIsLock_off,fIsLock_len);
	memcpy(gstBankParam.szTraceNo,szFramBuf + fTraceNo_off,fTraceNo_len);
	memcpy(gstBankParam.szBatchNo,szFramBuf + fBatchNo_off,fBatchNo_len);
	memcpy(gstBankParam.szInvoiceNo,szFramBuf + fInvoiceNo_off,fInvoiceNo_len);
	memcpy((char *)&gstBankParam.ulDialSuccSum,szFramBuf + fDialSuccSum_off,fDialSuccSum_len);
	memcpy((char *)&gstBankParam.ulDialSum,szFramBuf + fDialSum_off,fDialSum_len);
	memcpy((char *)&gstBankParam.lTimeCounter,szFramBuf + fTimeCounter_off,fTimeCounter_len);
	memcpy(gstBankParam.sLastLoginDateTime,szFramBuf + fLastLoginDateTime_off,fLastLoginDateTime_len);
	memcpy(gstBankParam.szLastLoginOper,szFramBuf + fLastLoginOperNo_off,fLastLoginOperNo_len);
	memcpy(&gstBankParam.cLastLoginOperRole,szFramBuf + fLastLoginOperRole_off,fLastLoginOperRole_len);
	memcpy((char *)&gstBankParam.nMCTimes,szFramBuf + fMC_EnterDateTime_off,fMC_EnterDateTime_len);
	memcpy((char *)&gstBankParam.nMCRandom,szFramBuf + fMC_EnterRandom_off,fMC_EnterRandom_len);
	memcpy((char *)&gstBankParam.cIsDownKek, szFramBuf+fIsDownKek_off, fIsDownKek_len);

	/**<EMV����*/
	memset(&gstEmvParam,0,sizeof(gstEmvParam));
	memcpy((char *)&gstEmvParam.nTransSerial,szFramBuf + fEmvTransSerial_off,fEmvTransSerial_len);
	memcpy((char *)&gstEmvParam.nOfflineUpNum,szFramBuf + fEmvOfflineUpNum_off,fEmvOfflineUpNum_len);
	memcpy((char *)&gstEmvParam.nOnlineUpNum,szFramBuf + fEmvOnlineUpNum_off,fEmvOnlineUpNum_len);
	memcpy((char *)&gstEmvParam.nOfflineFailUpNum,szFramBuf + fEmvOfflineFailUpNum_off,fEmvOfflineFailUpNum_len);
	memcpy((char *)&gstEmvParam.nOnlineARPCErrUpNum,szFramBuf + fEmvOnlineARPCErrUpNum_off,fEmvOnlineARPCErrUpNum_len);
	memcpy((char *)&gstEmvParam.cIsDownCAPKFlag,szFramBuf + fEmvIsDownCAPKFlag_off,fEmvIsDownCAPKFlag_len);
	memcpy((char *)&gstEmvParam.cIsDownAIDFlag,szFramBuf + fEmvIsDownAIDFlag_off,fEmvIsDownAIDFlag_len);
	memcpy((char *)&gstEmvParam.cIsScriptInform,szFramBuf + fEmvIsScriptInform_off,fEmvIsScriptInform_len);
	memcpy(gstEmvParam.sScriptData,szFramBuf + fEmvScriptData_off,fEmvScriptData_len);

	/**<POS��չ����*/
	memset(&gstExtAppParam,0,sizeof(gstExtAppParam));
	PubReadOneRec(FILE_EXTAPPPOSPARAM,1,(char *)&gstExtAppParam);

	return APP_SUCC;
}


/**
* @brief ��ȡPOS�̻�Ӧ������
* @param out pnType �̻�����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarPosMerType(int *pnType)
{
	*pnType=gstBankParam.cPosMerType-0x30;
	return APP_SUCC;
}

/**
* @brief ����POS�̻�Ӧ������
* @param in nType �̻�����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarPosMerType(const int nType)
{
	gstBankParam.cPosMerType=nType+0x30;
	PubSaveVar((char*)&gstBankParam.cPosMerType,fPosMerType_off,fPosMerType_len);
	return APP_SUCC;
}

/**
* @brief ��ȡ���ص�ͨѶ����ͷ��Ϣ
* @param out psCommHead ͨѶ����ͷ������6���ֽ�
* @return
* @li APP_SUCC
*/
int GetVarCommHead(char *psCommHead)
{
	memcpy(psCommHead,gstBankParam.sRetCommHead,fRetCommHead_len);
	return APP_SUCC;
}

/**
* @brief ���淵�ص�ͨѶ����ͷ��Ϣ
* @param in psCommHead ͨѶ����ͷ������6���ֽ�
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommHead(const char *psCommHead)
{
	STPOSCOMMHEAD *pstPosCommHead;
	char szHead[12+1], cStatus=YES;
	
	memcpy(gstBankParam.sRetCommHead,psCommHead,fRetCommHead_len);
	PubSaveVar(gstBankParam.sRetCommHead,fRetCommHead_off,fRetCommHead_len);

	memset(szHead, 0, sizeof(szHead));
	PubHexToAsc((unsigned char *)gstBankParam.sRetCommHead, 12, 0, (unsigned char *)&szHead);

	pstPosCommHead = (STPOSCOMMHEAD *)szHead;
	
	switch(pstPosCommHead->cResponse)
	{
	case '1':
		PubSaveVar(&cStatus, fIsPosParamDown_off, fIsPosParamDown_len);
		break;
	case '2':
		PubSaveVar(&cStatus, fIsPosStatusSend_off, fIsPosStatusSend_len);
		break;
	case '3':
		PubSaveVar(&cStatus, fIsReLogin_off, fIsReLogin_len);
		break;
	case '4':
#if defined(EMV_IC)
		SetVarEmvIsDownCAPKFlag(YES);
#endif
		break;
	case '5':
#if defined(EMV_IC)
		SetVarEmvIsDownAIDFlag(YES);
#endif
		break;	
#if defined(USE_TMS)
	case '6':
		PubSaveVar(&cStatus, fIsTmsDown_off, fIsTmsDown_len);
		break;	
#endif
	case '7':
		PubSaveVar(&cStatus, fIsBlackListDown_off, fIsBlackListDown_len);
		break;
	default:
		break;
	}	
	
	return APP_SUCC;
}

/**
* @brief ��ȡǩ����־
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsLogin()
{
	if (gstBankParam.cIsLogin==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ����ǩ����ʶ
* @param in pFlag ǩ����ʶ��YES/NO
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarIsLogin(const YESORNO cFlag)
{
	gstBankParam.cIsLogin=cFlag;
	PubSaveVar((char*)&gstBankParam.cIsLogin,fIsLogin_off,fIsLogin_len);
	return APP_SUCC;
}

/**
* @brief ��ȡ������ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsLock()
{
	if (gstBankParam.cIsLock==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ����������ʶ
* @param in pFlag ������ʶ��YES/NO
* @return
* @li APP_SUCC
*/
int SetVarIsLock(const YESORNO cFlag)
{
	gstBankParam.cIsLock=cFlag;
	PubSaveVar((char*)&gstBankParam.cIsLock,fIsLock_off,fIsLock_len);
	return APP_SUCC;
}


/**
* @brief ��ȡ��ˮ��
* @param out pszTraceNo ��ˮ�ţ�����6λ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarTraceNo(char *pszTraceNo)
{
	memcpy(pszTraceNo,gstBankParam.szTraceNo,fTraceNo_len);
	return APP_SUCC;
}

/**
* @brief ������ˮ��
* @param in pszTraceNo ��ˮ�ţ�����6λ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarTraceNo(const char *pszTraceNo)
{
	memcpy(gstBankParam.szTraceNo,pszTraceNo,fTraceNo_len);
	PubSaveVar(gstBankParam.szTraceNo,fTraceNo_off,fTraceNo_len);
	return APP_SUCC;
}

/**
* @brief ��ˮ�ż�һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarTraceNo()
{
	PubIncNum(gstBankParam.szTraceNo);
	SetVarTraceNo(gstBankParam.szTraceNo);
	return APP_SUCC;
}

/**
* @brief ��ȡ���κ�
* @param out pszBatchNo ���κţ�����6λ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarBatchNo(char *pszBatchNo)
{
	memcpy(pszBatchNo,gstBankParam.szBatchNo,fBatchNo_len);
	return APP_SUCC;
}

/**
* @brief �������κ�
* @param in pszBatchNo ���κţ�����6λ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarBatchNo(const char *pszBatchNo)
{
	memcpy(gstBankParam.szBatchNo,pszBatchNo,fBatchNo_len);
	PubSaveVar(gstBankParam.szBatchNo,fBatchNo_off,fBatchNo_len);
	return APP_SUCC;
}

/**
* @brief ���κż�һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarBatchNo()
{
	PubIncNum(gstBankParam.szBatchNo);
	SetVarBatchNo(gstBankParam.szBatchNo);
	return APP_SUCC;
}

/**
* @brief ��ȡ���ųɹ�����
* @param out pulSum �ɹ����Ŵ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarDialSuccSum(ulong *pulSum)
{
	*pulSum=gstBankParam.ulDialSuccSum;
	return APP_SUCC;
}

/**
* @brief ���沦�ųɹ�����
* @param in ulSum �ɹ����Ŵ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarDialSuccSum(const ulong ulSum)
{
	gstBankParam.ulDialSuccSum=ulSum;
	PubSaveVar((char *)&gstBankParam.ulDialSuccSum,fDialSuccSum_off,fDialSuccSum_len);
	return APP_SUCC;
}

/**
* @brief���ųɹ�������һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarDialSuccSum()
{
	gstBankParam.ulDialSuccSum++;
	SetVarDialSuccSum(gstBankParam.ulDialSuccSum);
	return APP_SUCC;
}

/**
* @brief ��ȡ���Ŵ���
* @param out pulSum ���Ŵ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarDialSum(ulong *pulSum)
{
	*pulSum=gstBankParam.ulDialSum;
	return APP_SUCC;
}

/**
* @brief ���沦�Ŵ���
* @param in ulSum ���Ŵ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarDialSum(const ulong ulSum)
{
	gstBankParam.ulDialSum=ulSum;
	PubSaveVar((char *)&gstBankParam.ulDialSum,fDialSum_off,fDialSum_len);
	return APP_SUCC;
}

/**
* @brief���Ŵ�����һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarDialSum()
{
	gstBankParam.ulDialSum++;
	SetVarDialSum(gstBankParam.ulDialSum);
	return APP_SUCC;
}

/**
* @brief ��ȡʱ������������㲦ͨ��ʱʹ��
* @param in plTime ʱ�䳤���α�ʾ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarTimeCounter(long *plTime)
{
	*plTime=gstBankParam.lTimeCounter;
	return APP_SUCC;
}

/**
* @brief ����ʱ�������
* @param in lTime ʱ�䳤���α�ʾ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarTimeCounter(const long lTime)
{
	gstBankParam.lTimeCounter=lTime;
	PubSaveVar((char *)&gstBankParam.lTimeCounter,fTimeCounter_off,fTimeCounter_len);
	return APP_SUCC;
}

/**
* @brief ����ϻز���Ա��¼ʱ��
* @param in psLastLoginDateTime �ϻز���Ա��¼ʱ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarLastLoginDateTime(char *psLastLoginDateTime)
{
	memcpy(psLastLoginDateTime, gstBankParam.sLastLoginDateTime, fLastLoginDateTime_len);
	return APP_SUCC;
}

/**
* @brief �����ϻز���Ա��¼ʱ��
* @param in psLastLoginDateTime �ϻز���Ա��¼ʱ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarLastLoginDateTime(const char *psLastLoginDateTime)
{
	memcpy( gstBankParam.sLastLoginDateTime, psLastLoginDateTime, fLastLoginDateTime_len);
	PubSaveVar( (char *)&(gstBankParam.sLastLoginDateTime), fLastLoginDateTime_off, fLastLoginDateTime_len);
	return APP_SUCC;
}



/**
* ������������
*/

/**
* @brief ��ȡ������־
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsReversal()
{
	if (gstReversalParam.cIsReversal==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ���������־
* @param in pFlag ��־��ֵΪYES/NO
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarIsReversal(const YESORNO cFlag)
{
	gstReversalParam.cIsReversal=cFlag;
	PubSaveVar((char*)&gstReversalParam.cIsReversal,fIsReversal_off,fIsReversal_len);
	return APP_SUCC;
}


/**
* @brief ��ȡ�ѳ�������
* @param out pnNum �ѳ�������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarHaveReversalNum(int *pnNum)
{
	*pnNum=gstReversalParam.nHaveReversalNum;
	return APP_SUCC;
}

/**
* @brief �����ѳ�������
* @param in nNum �ѳ�������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarHaveReversalNum(const int nNum)
{
	gstReversalParam.nHaveReversalNum=nNum;
	PubSaveVar((char *)&gstReversalParam.nHaveReversalNum,fHaveReversalNum_off,fHaveReversalNum_len);
	return APP_SUCC;
}

/**
* @brief �ѳ���������һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarHaveReversalNum()
{
	gstReversalParam.nHaveReversalNum++;
	SetVarHaveReversalNum(gstReversalParam.nHaveReversalNum);
	return APP_SUCC;
}

/**
* @brief ��ȡ�������ݺͳ���
* @param out psData ��󳤶�512λ
* @param out pnLen �������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarReversalData(char *psData,int *pnLen)
{
	*pnLen=gstReversalParam.nReversalLen;
	memcpy(psData,gstReversalParam.sReversalData,gstReversalParam.nReversalLen);

	return APP_SUCC;
}

/**
* @brief ����������ݺͳ���
* @param in sData ��󳤶�512λ
* @param in nLen �������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarReversalData(const char *sData,const int nLen)
{
	if ((nLen>fReversalData_len)||(nLen<0))
	{
		return APP_FAIL;
	}
	memcpy(gstReversalParam.sReversalData,sData,nLen);
	gstReversalParam.nReversalLen=nLen;
	PubSaveVar((char *)&gstReversalParam.nReversalLen,fReversalLen_off,fReversalLen_len);
	PubSaveVar(gstReversalParam.sReversalData,fReversalData_off,fReversalData_len);

	return APP_SUCC;
}


/**
* POS����
*/

/**
* @brief ��ȡ�ն˺�
* @param out pszValue ��󳤶�8λ
* @return ��
*/
void GetVarTerminalId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szTerminalId,sizeof(gstAppPosParam.szTerminalId)-1);
	return;
}

/**
* @brief ��ȡ�ն˺�
* @param out pszValue ��󳤶�8λ
* @return ��
*/
void SetVarTerminalId(char *pszValue)
{
	memcpy(gstAppPosParam.szTerminalId,pszValue,sizeof(gstAppPosParam.szTerminalId)-1);
//#if defined(USE_TMS)
	MC_SetPubParam(APP_POSID, gstAppPosParam.szTerminalId, 8);
//#endif
	PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam);
	return;
}

/**
* @brief ��ȡ�̻��ţ�
* @param out pszValue ��󳤶�15λ
* @return ��
*/
void GetVarMerchantId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantId,sizeof(gstAppPosParam.szMerchantId)-1);
	return;
}

/**
* @brief ��ȡ�̻��ţ�
* @param out pszValue ��󳤶�15λ
* @return ��
*/
void SetVarMerchantId(char *pszValue)
{
	memcpy(gstAppPosParam.szMerchantId,pszValue,sizeof(gstAppPosParam.szMerchantId)-1);
//#if defined(USE_TMS)
	MC_SetPubParam(APP_MERCHANTID, gstAppPosParam.szMerchantId, 15);
//#endif
	PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam);
	return;
}

/**
* @brief ��ȡ�����̻���
* @param out pszValue �40���ֽ�
* @return ��
*/
void GetVarMerchantNameCn(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantNameCn,sizeof(gstAppPosParam.szMerchantNameCn)-1);
	return;
}

/**
* @brief �����̻���������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetVarMerchantNameCn(const char *pszValue)
{
	memset(gstAppPosParam.szMerchantNameCn, 0, sizeof(gstAppPosParam.szMerchantNameCn));
	memcpy(gstAppPosParam.szMerchantNameCn, pszValue, sizeof(gstAppPosParam.szMerchantNameCn)-1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡӢ���̻���
* @param out pszValue �40���ֽ�
* @return ��
*/
void GetVarMerchantNameEn(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantNameEn,sizeof(gstAppPosParam.szMerchantNameEn)-1);
	return;
}

/**
* @brief ��ȡӦ�����ͣ�����2���ֽ�
* @param out pszValue ����2���ֽ�
* @return ��
*/
void GetVarAppType(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szAppType,sizeof(gstAppPosParam.szAppType)-1);
	return;
}

/**
* @brief ����Ӧ�����ͣ�����2���ֽ�
* @param int pszValue ����2���ֽ�
* @return ��
*/
int SetVarAppType(const char *pszValue)
{
	memcpy(gstAppPosParam.szAppType, pszValue, sizeof(gstAppPosParam.szAppType)-1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡӦ�ð汾��
* @param out pszValue ����2���ֽ�
* @return ��
*/
void GetVarAppVer(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szAppVer,sizeof(gstAppPosParam.szAppVer)-1);
	return;
}

/**
* @brief
* @return
* @li POS_STATE_TEST ����״̬
* @li POS_STATE_NORMAL ��������״̬
*/
int GetVarPosState()
{
	if (gstAppPosParam.cPosState==POS_STATE_TEST)
	{
		return POS_STATE_TEST;
	}
	return POS_STATE_NORMAL;
}

/**
* @brief ��ȡ���д��룬����4λ
* @param out pszValue ����4λ
* @return ��
*/
void GetVarBusinessBankId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szBusinessBankId,sizeof(gstAppPosParam.szBusinessBankId)-1);
	return;
}

/**
* @brief ��ȡ����������
* @param out pszValue ����4λ
* @return ��
*/
void GetVarLocalcode(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szLocalCode,sizeof(gstAppPosParam.szLocalCode)-1);
	return;
}

/**
* @brief ��ȡ������̱�ʶ
* @return
* @li YES ��
* @li NO ����
*/
YESORNO GetVarIsPinpad()
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_PINPAD, NULL))
	{
		if (gstAppPosParam.cIsPinPad==YES)
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ������̽ӵĴ��ں�
* @param out pnValue ���ںţ�1��AUX1,2��AUX2
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarPinpadAuxNo(int *pnValue)
{
	//ͳһƽ̨����PORT_NUM_COM2
	*pnValue = PORT_NUM_COM2;
	return APP_SUCC;
}

/**
* @brief ��ȡ������̳�ʱʱ��
* @param out pnValue ������̳�ʱʱ�䣬0-999
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarPinpadTimeOut(int *pnValue)
{
	char szTmp[10]= {0};

	memcpy(szTmp,gstAppPosParam.szPinPadTimeOut,sizeof(gstAppPosParam.szPinPadTimeOut)-1);
	if (atoi(szTmp)>0 && atoi(szTmp)<999)
	{
		*pnValue=atoi(szTmp);
	}
	else
	{
		*pnValue=60;
	}

	return APP_SUCC;
}

/**
* @brief ��ȡ�ն˰�ȫ����
* @param out pnValue ��ȫ����ֵ���֧��8���ֽ�
* @return ��
*/
void GetVarSecurityPwd(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szSecurityPwd,sizeof(gstAppPosParam.szSecurityPwd)-1);
	return;
}

/**
* @brief ��ȡ�ն�����Կ
* @param out pnValue ����Կ���
* @return ��
*/
void GetVarMainKeyNo(int *pnValue)
{
	char szTmp[10] = {0};

	memcpy(szTmp,gstAppPosParam.szMainKeyNo,sizeof(gstAppPosParam.szMainKeyNo)-1);
	*pnValue=atoi(szTmp);
	return;
}

/**
* @brief ������������Կ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetVarMainKeyNo(const int nIndex)
{
	sprintf(gstAppPosParam.szMainKeyNo, "%02d", nIndex%100);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ܷ�ʽ��DES����3des
* @return
* @li DESMODE_3DES
* @li DESMODE_DES
*/
int  GetVarEncyptMode()
{
	if (gstAppPosParam.cEncyptMode==DESMODE_3DES)
	{
		return DESMODE_3DES;
	}
	return DESMODE_DES;
}

/**
* @brief pin������ܷ�ʽ������ʱ���˺��Ƿ��������
* @return
* @li PINTYPE_WITHOUTPAN
* @li PINTYPE_WITHPAN
*/
int  GetVarPinEncyptMode()
{
	if (gstAppPosParam.cPinEncyptMode==PINTYPE_WITHOUTPAN || (GetVarIsPhoneSale() == YES))
	{
		return PINTYPE_WITHOUTPAN;
	}
	return PINTYPE_WITHPAN;
}


/**
* @brief ��ȡ�ŵ����ܱ�ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsEncryptTrack()
{
	if (gstAppPosParam.cIsEncryptTrack==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ���ôŵ����ܱ�ʶ
* @return
* @li YES
* @li NO
*/
int SetVarIsEncryptTrack(const YESORNO yesorno)
{
	gstAppPosParam.cIsEncryptTrack = yesorno;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡ�Ƿ�֧��С�����Ȩ��ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsSmallGeneAuth()
{
	if (gstAppPosParam.cIsSmallGeneAuth==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����Ƿ�֧��С�����Ȩ
* @return
* @li YES
* @li NO
*/
int SetVarIsSmallGeneAuth(const YESORNO yesorno)
{
	gstAppPosParam.cIsSmallGeneAuth = yesorno;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡ�Զ�ǩ�˱�ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsAutoLogOut()
{
	if (gstAppPosParam.cIsAutoLogOut==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����Զ�ǩ�˱�ʶ
* @return
* @li YES
* @li NO
*/
int SetVarIsAutoLogOut(const YESORNO yesorno)
{
	gstAppPosParam.cIsAutoLogOut = yesorno;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡ�Ƿ���ʾemv���׵�tvr tsi
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsDispEMV_TVRTSI(void)
{
	if (gstAppPosParam.cIsDispEMV_TVRTSI==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief ��ȡ�Ƿ����ο���
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsShieldPan(void)
{
	if (gstAppPosParam.cIsShieldPan != YES  && gstAppPosParam.cIsShieldPan != NO)
	{
		/**<���⴦����Ծɰ�����stAppPosParam.cIsShieldPan��ԭʼ��ֵ*/
		gstAppPosParam.cIsShieldPan = YES;
	}
	if (gstAppPosParam.cIsShieldPan == YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief ��ȡ�Ƿ��ӡ��ˮ��ϸ��ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsPrintWaterRec()
{
	if (gstAppPosParam.cIsPrintWaterRec==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief ��ȡ�Ƿ�����Ʊ��
* @return
* @li YES
* @li NO
*/
int GetVarIsNewTicket()
{
	return gstAppPosParam.cIsNewTicket;
}

/**
* @brief ��ȡƱ���Ƿ��Ӣ�ı�ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsTickeWithEn()
{
	if (gstAppPosParam.cIsTicketWithEn==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡԤ��Ȩ�Ƿ����ο���
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsPreauthShieldPan()
{
	if (gstAppPosParam.cIsPreauthShieldPan==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ�Ƿ��ӡ���ϱ��浥
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsPrintErrReport()
{
	if (gstAppPosParam.cIsPrintErrReport==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ��ӡ����
* @param out pnValue ��ӡ����
* @return ��
*/
void GetVarPrintPageCount(int *pnValue)
{
	if (gstAppPosParam.cPrintPageCount>='0' && gstAppPosParam.cPrintPageCount<='9')
	{
		*pnValue = gstAppPosParam.cPrintPageCount - 0x30;
	}
	else
	{
		*pnValue=1;
	}
	return;
}

/**
* @brief ��ȡPOS�洢�Ľ��ױ������ﵽ����ʱ��Ҫ�����
* @param out pnValue 1-99999
* @return ��
*/
void GetVarMaxTransCount(int *pnValue)
{
	char szTmp[10] = {0};

	memcpy(szTmp,gstAppPosParam.szMaxTransCount,sizeof(gstAppPosParam.szMaxTransCount)-1);
	if (atoi(szTmp)>0&&atoi(szTmp)<99999)
	{
		*pnValue=atoi(szTmp);
	}
	else
	{
		*pnValue=1000;
	}
	return;
}

/**
* @brief ��ȡPOS��֧�ֵ�����˻����
* @param out pulValue �޷��ų�����
* @return ��
*/
/** ����Ҫ��������ת����ֱ�Ӵ���12λ���ַ�������*/
void GetVarMaxRefundAmount(char *szMaxRefundAmt)
{
	memcpy(szMaxRefundAmt, gstAppPosParam.szMaxRefundAmount,sizeof(gstAppPosParam.szMaxRefundAmount)-1);
	return;
}

/**
* @brief ��ȡС�ѱ���
* @param out psValue 2���ֽڵ�С�ѱ���
* @return ��
*/
void GetVarTipRate(char *psValue)
{
	memcpy(psValue,gstAppPosParam.szTipRate,2);
	return;
}

/**
* @brief ��ȡС�ѱ���
* @param out psValue 2���ֽڵ�С�ѱ���
* @return
* @li APP_SUCC ���óɹ�
* @li APP_FAIL����ʧ��
*/
int SetVarTipRate(const char *psValue)
{
	memcpy(gstAppPosParam.szTipRate, psValue, 2);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
/**
* @brief ��ȡ�Ƿ�֧��С�ѽ���
* @return
* @li YES ֧��
* @li NO ��֧��
*/
YESORNO GetVarIsTipFlag(void)
{
	if (gstAppPosParam.cIsTipFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
/**
* @brief �����Ƿ�֧��С�ѽ���
* @param in pFlag�Ƿ�֧��С�ѽ��ף�YES/NO
* @return
* @li APP_SUCC ���óɹ�
* @li APP_FAIL����ʧ��
*/
int SetVarIsTipFlag(const YESORNO pFlag)
{
	gstAppPosParam.cIsTipFlag = pFlag;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡIC���أ��Ƿ�֧��IC������
* @return
* @li YES ֧��
* @li NO ��֧��
*/
YESORNO GetVarIsIcFlag()
{
	if (gstAppPosParam.cIsIcFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡIC��ȷ�ϲ�����ȷ���Ƿ���ʾ����Աȷ�Ͽ�Ƭ�Ƿ�ΪIC��
* @return
* @li YES ֧��
* @li NO ��֧��
*/
YESORNO GetVarIsIcConfirmFlag()
{
	if (gstAppPosParam.cIsIcConfirmFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ�Ƿ�֧���ֹ����뿨��
* @return
* @li YES ֧��
* @li NO ��֧��
*/
YESORNO GetVarIsCardInput()
{
	if (gstAppPosParam.cIsCardInput==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����Ƿ�֧���ֹ����뿨��
* @param in pFlag�Ƿ�֧���ֹ����뿨�ţ�YES/NO
* @return
* @li APP_SUCC ���óɹ�
* @li APP_FAIL����ʧ��
*/
int SetVarIsCardInput(const YESORNO cFlag)
{
	gstAppPosParam.cIsCardInput = cFlag;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡ���ѳ����Ƿ���Ҫˢ��
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsSaleVoidStrip()
{
	if (gstAppPosParam.cIsSaleVoidStrip==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡԤ��Ȩ��ɳ����Ƿ���Ҫˢ��
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsAuthSaleVoidStrip()
{
	if (gstAppPosParam.cIsAuthSaleVoidStrip==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ�������Ƿ���Ҫ��������
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsVoidPin()
{
	if (gstAppPosParam.cIsVoidPin==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡԤ��Ȩ��ɳ����Ƿ���Ҫ��������
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsAuthSaleVoidPin()
{
	if (gstAppPosParam.cIsAuthSaleVoidPin==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡԤ��Ȩ�����Ƿ���Ҫ��������
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsPreauthVoidPin()
{
	if (gstAppPosParam.cIsPreauthVoidPin==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡԤ��Ȩ��������Ƿ���Ҫ��������
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsAuthSalePin()
{
	if (gstAppPosParam.cIsAuthSalePin==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡĬ�Ͻ���
* @param out pcValue '0'-���ѣ�'1'-Ԥ��Ȩ
* @return ��
*/
void GetVarDefaultTransType(char *pcValue)
{
	gstAppPosParam.cDefaultTransType = gstAppPosParam.cDefaultTransType == '0' ? '0' : '1';
	*pcValue=gstAppPosParam.cDefaultTransType;
	return;
}

/**
* @brief ��ȡ��֧��Ԥ��Ȩ��ɵķ�ʽ
* @param out pcValue '0'-ͬʱ֧�֣�'1'-֧������'2'-֧��֪ͨ
* @return ��
*/
void GetVarAuthSaleMode(char *pcValue)
{
	*pcValue=gstAppPosParam.cAuthSaleMode;
	return;
}

/**
* @brief ��ȡ�����ֶ�1����
* @param out pszValue
* @return ��
*/
void GetVarReserve1(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szReserve1,sizeof(gstAppPosParam.szReserve1)-1);
	return;
}

/**
* @brief ��ȡ�����ֶ�2����
* @param out pszValue
* @return ��
*/
void GetVarReserve2(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szReserve2,sizeof(gstAppPosParam.szReserve2)-1);
	return;
}

/**
* EMV����
*/

/**
* @brief ��ȡEMV�������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEmvTransSerial(int *pnSerial)
{
	gstEmvParam.nTransSerial = gstEmvParam.nTransSerial%99999999;
	*pnSerial = gstEmvParam.nTransSerial == 0 ? ++gstEmvParam.nTransSerial : gstEmvParam.nTransSerial;
	return APP_SUCC;
}


/**
* @brief ����emv�������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvTransSerial(const int nSerial)
{
	gstEmvParam.nTransSerial=nSerial%99999999;
	PubSaveVar((char *)&gstEmvParam.nTransSerial,fEmvTransSerial_off,fEmvTransSerial_len);
	return APP_SUCC;
}

/**
* @brief emv������ż�һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarEmvTransSerial()
{
	gstEmvParam.nTransSerial = ++gstEmvParam.nTransSerial%99999999;
#if 0 //�˴����浽���籣�����ȽϷ�ʱ,�����洦��(SaveEmvTransSerial),��Ӱ��qPbocʱ��
	SetVarEmvTransSerial(gstEmvParam.nTransSerial);
#endif
	return APP_SUCC;
}

/**
* @brief emv���潻�����
* @return
* @li APP_SUCC
*/
int SaveEmvTransSerial(void)
{
	SetVarEmvTransSerial(gstEmvParam.nTransSerial);
	return APP_SUCC;
}



/**
* @brief ��ȡemv�ѻ��������ͱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEmvOfflineUpNum(int *pnSerial)
{
	*pnSerial=gstEmvParam.nOfflineUpNum;
	return APP_SUCC;
}


/**
* @brief ����emv�ѻ��������ͱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvOfflineUpNum(const int nSerial)
{
	gstEmvParam.nOfflineUpNum=nSerial;
	PubSaveVar((char *)&gstEmvParam.nOfflineUpNum,fEmvOfflineUpNum_off,fEmvOfflineUpNum_len);
	return APP_SUCC;
}

/**
* @brief emv�ѻ��������ͱ�����һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarEmvOfflineUpNum()
{
	gstEmvParam.nOfflineUpNum++;
	SetVarEmvTransSerial(gstEmvParam.nOfflineUpNum);
	return APP_SUCC;
}

/**
* @brief ��ȡemv�����������ͱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEmvOnlineUpNum(int *pnSerial)
{
	*pnSerial=gstEmvParam.nOnlineUpNum;
	return APP_SUCC;
}


/**
* @brief ����emv�ѻ��������ͱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvOnlineUpNum(const int nSerial)
{
	gstEmvParam.nOnlineUpNum=nSerial;
	PubSaveVar((char *)&gstEmvParam.nOnlineUpNum,fEmvOnlineUpNum_off,fEmvOnlineUpNum_len);
	return APP_SUCC;
}

/**
* @brief emv�����������ͱ�����һ������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarEmvOnlineUpNum()
{
	gstEmvParam.nOnlineUpNum++;
	SetVarEmvTransSerial(gstEmvParam.nOnlineUpNum);
	return APP_SUCC;
}

/**
* @brief ����EMV�ѻ�����ʧ�����ͱ���
* @param in nOfflineFailUpNum �ڼ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEMVOfflineFailHaltFlag(const int nOfflineFailUpNum)
{

	gstEmvParam.nOfflineFailUpNum = nOfflineFailUpNum;
	PubSaveVar( (char *)(&gstEmvParam.nOfflineFailUpNum), fEmvOfflineFailUpNum_off, fEmvOfflineFailUpNum_len);
	return APP_SUCC;
}

/**
* @brief ���EMV�ѻ�����ʧ�����ͱ���
* @param in pnOfflineFailUpNum ���ͱ�����ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEMVOfflineFailHaltFlag(int *pnOfflineFailUpNum)
{

	*pnOfflineFailUpNum = gstEmvParam.nOfflineFailUpNum;
	return APP_SUCC;
}

/**
* @brief ����EMV APRC����Ƭ��Ȼ�жԵ����ͱ���
* @param in nOnlineARPCErrUpNum �ڼ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEMVOnlineARPCErrHaltFlag(const int nOnlineARPCErrUpNum)
{

	gstEmvParam.nOnlineARPCErrUpNum = nOnlineARPCErrUpNum;
	PubSaveVar( (char *)(&gstEmvParam.nOnlineARPCErrUpNum), fEmvOnlineARPCErrUpNum_off, fEmvOnlineARPCErrUpNum_len);
	return APP_SUCC;
}

/**
* @brief ���EMV APRC����Ƭ��Ȼ�жԵ����ͱ���
* @param in pnOnlineARPCErrUpNum ���ͱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEMVOnlineARPCErrHaltFlag(int *pnOnlineARPCErrUpNum)
{

	*pnOnlineARPCErrUpNum = gstEmvParam.nOnlineARPCErrUpNum;
	return APP_SUCC;
}

/**
* @brief ��ȡ���ز���(EMV�����ع�Կ��ic������)�Ƿ�ɹ�
* @return
* @li YES
* @li NO
*/
YESORNO GetVarEmvIsDownCAPKFlag()
{
	if (gstEmvParam.cIsDownCAPKFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �������ز���(EMV�����ع�Կ��ic������)�Ƿ�ɹ�
* @param in pFlag ���ر�ʶ��YES/NO
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvIsDownCAPKFlag(const YESORNO cFlag)
{
	gstEmvParam.cIsDownCAPKFlag=cFlag;
	PubSaveVar((char*)&gstEmvParam.cIsDownCAPKFlag,fEmvIsDownCAPKFlag_off,fEmvIsDownCAPKFlag_len);
	return APP_SUCC;
}

/**
* @brief ��ȡ���ز���(EMV�����ع�Կ��ic������)�Ƿ�ɹ�
* @return
* @li YES
* @li NO
*/
YESORNO GetVarEmvIsDownAIDFlag()
{
	if (gstEmvParam.cIsDownAIDFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �������ز���(EMV�����ع�Կ��ic������)�Ƿ�ɹ�
* @param in pFlag ���ر�ʶ��YES/NO
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvIsDownAIDFlag(const YESORNO cFlag)
{
	gstEmvParam.cIsDownAIDFlag=cFlag;
	PubSaveVar((char*)&gstEmvParam.cIsDownAIDFlag,fEmvIsDownAIDFlag_off,fEmvIsDownAIDFlag_len);
	return APP_SUCC;
}

/**
* @brief ��ȡ�ű�֪ͨ��ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarEmvIsScriptInform()
{
	if (gstEmvParam.cIsScriptInform==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ����ű�֪ͨ��ʶ
* @param in pFlag �ű�֪ͨ��YES/NO
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvIsScriptInform(const YESORNO cFlag)
{
	gstEmvParam.cIsScriptInform=cFlag;
	PubSaveVar((char*)&gstEmvParam.cIsScriptInform,fEmvIsScriptInform_off,fEmvIsScriptInform_len);
	return APP_SUCC;
}

/**
* @brief ��ȡ�ű�֪ͨ����
* @param in nLen ���ȣ�1-256
* @param out psData
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarEmvScriptData(char *psData,int nLen)
{
	if ((nLen>fEmvScriptData_len)||(nLen<0))
		return APP_FAIL;

	memcpy(psData,gstEmvParam.sScriptData,nLen);
	return APP_SUCC;
}

/**
* @brief ����ű�֪ͨ����
* @param in nLen ���ȣ�1-256
* @param in psData
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEmvScriptData(const char *psData,const int nLen)
{
	if ((nLen>fEmvScriptData_len)||(nLen<0))
		return APP_FAIL;

	memset(gstEmvParam.sScriptData,0,sizeof(gstEmvParam.sScriptData));
	memcpy(gstEmvParam.sScriptData,psData,nLen);
	PubSaveVar(gstEmvParam.sScriptData,fEmvScriptData_off,nLen);
	return APP_SUCC;
}

/**
* @brief ������������Կ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMainKeyNo(void)
{
	int nLen, nNumber;
	char szNumber[2+1] = {0};

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}

	gstAppPosParam.szMainKeyNo[2] = 0;
	nNumber = atoi(gstAppPosParam.szMainKeyNo);
	sprintf(szNumber, "%1.1u", nNumber%10);
	ASSERT_QUIT(PubInputDlg("������Կ������", "��Կ������(0-9):", szNumber, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	sprintf(gstAppPosParam.szMainKeyNo, "0%s", szNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	Ums_SetMainKeyFromMC();
	return APP_SUCC;
}

/**
* @brief �����˻����޽��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMaxRefundAmount(void)
{
	int nLen = 12;

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}
	gstAppPosParam.szMaxRefundAmount[12] = 0;
	ASSERT_RETURNCODE(PubInputAmount("�������׿���", "�˻������:", gstAppPosParam.szMaxRefundAmount, &nLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief ����Ԥ��Ȩ��ɷ�ʽ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionAuthsaleMode(void)
{
	int nSelect;

	switch(gstAppPosParam.cAuthSaleMode)
	{
	case '0':
		nSelect = 0;
		break;
	case '1':
		nSelect = 1;
		break;
	case '2':
		nSelect = 2;
		break;
	default:
		nSelect = 0XFF;
		break;
	}
	ASSERT_RETURNCODE(PubSelectListItem("0.ͬʱ֧��||1.֧������ģʽ||2.֧��֪ͨģʽ", "��Ȩ֪ͨģʽ", NULL, nSelect, &nSelect));

	gstAppPosParam.cAuthSaleMode = nSelect + '0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ô�ӡ����,������ӡ����Ҫ����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPrintPageCount(void)
{
	char szNum[2] = {0};
	int nNum, nLen;
	
	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}

	if ( PubGetPrinter() != _PRINTTYPE_TP )
	{
		PubMsgDlg("��ܰ��ʾ","���֧�ִ�������", 0, 2);
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}


	while(1)
	{
		szNum[0] = gstAppPosParam.cPrintPageCount;
		ASSERT_RETURNCODE( PubInputDlg("ϵͳ��������", "����������ӡ����\n(    <=3��):", szNum, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
		nNum=atoi(szNum);
		if( (nNum >= 1) && (nNum <= 3) )
		{
			break;
		}
		PubMsgDlg("ϵͳ��������","��������,������!",1,1);
	}
	gstAppPosParam.cPrintPageCount = '0' + atoi(szNum);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief ���������������ױ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMaxTransCount(void)
{
	int nLen, nNumber;
	char szNumber[5+1] = {0};

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}

	while(1)
	{
		nNumber = atoi(gstAppPosParam.szMaxTransCount);
		sprintf(szNumber, "%d", nNumber );
		ASSERT_RETURNCODE(PubInputDlg("ϵͳ��������", "��������ױ���\n(   <=500��):", szNumber, &nLen, 1, 3, 0, INPUT_MODE_NUMBER));
		nNumber = atoi(szNumber);
		if ( (nNumber>0 )&&(nNumber <=500) )
		{
			break;
		}
		PubMsgDlg("ϵͳ��������","��������,������!",0,1);
	}
	sprintf(gstAppPosParam.szMaxTransCount, "%05d", nNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief �����̻���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMerchantId(void)
{
	int nLen, nWaterSum;
	char szMerchantId[15+1] = {0};
	char szContent[32+1] = {0};

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}

	memcpy(szMerchantId, gstAppPosParam.szMerchantId, sizeof(gstAppPosParam.szMerchantId));
	sprintf(szContent, "%s\n�̻���:(15λ)", szMerchantId);
	ASSERT_RETURNCODE(PubInputDlg("�̻���������", szContent, szMerchantId, &nLen, 15, 15, 0, INPUT_MODE_STRING));
	if (memcmp(szMerchantId, gstAppPosParam.szMerchantId, 15) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("�̻���������", "�н�����ˮ�Ƚ���", 3, 10);
			return APP_FAIL;
		}
	}

	if (memcmp(szMerchantId, gstAppPosParam.szMerchantId, 15) !=0 && ChkSecurityPwd() == APP_SUCC)
	{
		memcpy(gstAppPosParam.szMerchantId, szMerchantId, sizeof(gstAppPosParam.szMerchantId));
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM, 1, (char *)&gstAppPosParam));
#if defined(EMV_IC)
		EmvSetMerchantId((uchar *)gstAppPosParam.szMerchantId);
#endif
//#if defined(USE_TMS)
		MC_SetPubParam(APP_MERCHANTID, gstAppPosParam.szMerchantId, 15);
//#endif
	}

	return APP_SUCC;
}

/**
* @brief ����POS�ն˺�
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTerminalId(void)
{
	int nLen, nWaterSum;
	char szTerminalId[8+1] = {0};
	char szContent[32+1] = {0};

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}
	memcpy( szTerminalId, gstAppPosParam.szTerminalId, sizeof(gstAppPosParam.szTerminalId ) );
	sprintf(szContent, "%s\nPOS��:(8λ)", szTerminalId);
	ASSERT_RETURNCODE( PubInputDlg("�̻���������", szContent, szTerminalId, &nLen, 8, 8, 0, INPUT_MODE_STRING));
	if (memcmp(szTerminalId, gstAppPosParam.szTerminalId, 8) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("�̻���������", "�н�����ˮ�Ƚ���", 3, 10);
			return APP_FAIL;
		}
	}

	if (memcmp(szTerminalId, gstAppPosParam.szTerminalId, 8) !=0 &&  ChkSecurityPwd() == APP_SUCC)
	{
		memcpy( gstAppPosParam.szTerminalId, szTerminalId, sizeof(gstAppPosParam.szTerminalId ) );
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
#if defined(EMV_IC)
		EmvSetTerminalId((uchar *)gstAppPosParam.szTerminalId);
#endif
//#if defined(USE_TMS)
		MC_SetPubParam(APP_POSID, gstAppPosParam.szTerminalId, 8);
//#endif
	}

	return APP_SUCC;
}

/**
* @brief ����Ӧ����ʾ����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionAppName(void)
{
	int nLen, nRet;

	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}
	PubClearAll();
	PubDisplayTitle("Ӧ����������");
	PubDisplayStr(1, 2, 1, "%s", gstAppPosParam.szAppDispname);
	PubDisplayStrInline(1, 5, "�Ƿ����? 1.��");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if ( nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("Ӧ����������", NULL, gstAppPosParam.szAppDispname, &nLen, 0, 14, CHOOSE_IME_MODE_NUMPY));
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			/*��tms�汾���˴�ҲҪ����*/
			MC_SetPubParam(APP_DISPNAME,gstAppPosParam.szAppDispname,0);
			return APP_SUCC;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if(nRet == KEY_ESC)
		{
			return APP_QUIT;
		}
		else if(nRet == KEY_ENTER)
		{
			return APP_SUCC;
		}
	}
	return APP_SUCC;
}


/**
* @brief �����̻�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMerchantName(void)
{
	int nLen;
	int nRet;
	
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	PubClearAll();
	PubDisplayTitle("�̻�����������");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szMerchantNameCn);		
	PubDisplayStrInline(1, 5, "�Ƿ����? 1.��");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10) ;
		if (nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("�̻�����������", NULL, gstAppPosParam.szMerchantNameCn, &nLen, 0, 40, CHOOSE_IME_MODE_NUMPY));
#if defined(EMV_IC)
			EmvSetMerchantName((uchar *)gstAppPosParam.szMerchantNameCn);
#endif
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			return APP_SUCC;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if(nRet == KEY_ESC)
		{
			return APP_QUIT;
		}
		else if(nRet == KEY_ENTER)
		{
			return APP_SUCC;
		}
	}
	return APP_SUCC;
}

/**
* @brief �����̻�Ӣ������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionMerchantNameEn(void)
{
	int nRet, nLen;

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	PubClearAll();
	PubDisplayTitle("�̻�Ӣ��������");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szMerchantNameEn);	
	PubDisplayStrInline(1, 5, "�Ƿ����? 1.��");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if (nRet == KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("�̻�Ӣ��������", NULL, gstAppPosParam.szMerchantNameEn, &nLen, 0, 20, IME_ENGLISH));
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			return APP_SUCC;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if(nRet == KEY_ESC)
		{
			return APP_QUIT;
		}
		else if(nRet == KEY_ENTER)
		{
			return APP_SUCC;
		}	
	}
	return APP_SUCC;
}

/**
* @brief ����POS��ǰ�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPOSYear(void)
{
	int nYear;
	char szYear[5] = {0},szDateTime[20] = {0};

	PubGetCurrentDatetime(szDateTime);
	memcpy(szYear, szDateTime, 4);

	ASSERT_RETURNCODE(PubInputDlg("�ն˲�������", "�����뵱ǰ���:", szYear, &nYear, 4, 4, 0, INPUT_MODE_NUMBER));
	nYear = atoi(szYear);
	if (nYear>2099)
	{
		memcpy( szDateTime, "2099", 4);
	}
	else if(nYear<1900)
	{
		memcpy( szDateTime, "1900", 4);
	}
	else
	{
		memcpy(szDateTime, szYear, 4);
	}
	if (PubSetPosDateTime(szDateTime, "YYYYMMDD", szDateTime + 8) != APP_SUCC)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief  ����POS��ˮ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTraceNo(void)
{
	int nLen,nWaterSum;
	char szTraceNo[fTraceNo_len+1]= {0};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	memcpy(szTraceNo, gstBankParam.szTraceNo, fTraceNo_len);
	ASSERT_RETURNCODE(PubInputDlg("ϵͳ��������", "��ǰ��ˮ��:", szTraceNo, &nLen, fTraceNo_len, fTraceNo_len, 0, INPUT_MODE_NUMBER));
	if (memcmp(gstBankParam.szTraceNo, szTraceNo, fTraceNo_len) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("�ն˲�������", "�н�����ˮ�Ƚ���", 3, 10);
			return APP_FAIL;
		}
		memcpy(gstBankParam.szTraceNo, szTraceNo, fTraceNo_len);
		PubSaveVar(gstBankParam.szTraceNo,fTraceNo_off,fTraceNo_len);
	}
	return APP_SUCC;
}

/**
* @brief  ����POS���κ�
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionBatchNo(void)
{
	int nLen,nWaterSum;
	char szBatchNo[fBatchNo_len+1] = {0};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	memcpy(szBatchNo, gstBankParam.szBatchNo, sizeof(gstBankParam.szBatchNo) - 1);
	ASSERT_RETURNCODE(PubInputDlg("ϵͳ��������", "��ǰ���κ�:", szBatchNo, &nLen, fBatchNo_len, fBatchNo_len, 0, INPUT_MODE_NUMBER));

	if (memcmp(gstBankParam.szBatchNo, szBatchNo, fBatchNo_len) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("�ն˲�������", "�н�����ˮ�Ƚ���", 3, 10);
			return APP_FAIL;
		}
		memcpy(gstBankParam.szBatchNo, szBatchNo, fBatchNo_len);
		PubSaveVar(gstBankParam.szBatchNo,fBatchNo_off,fBatchNo_len);
	}

	return APP_SUCC;
}

/**
* @brief  �������д���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionBusinessBankId(void)
{
	int nLen;

	ASSERT_RETURNCODE( PubInputDlg("�ն˲�������", "���д���:", gstAppPosParam.szBusinessBankId, &nLen, 4, 4, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  ���ñ��ص�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionLocalCode(void)
{
	int nLen;

	ASSERT_RETURNCODE(PubInputDlg("�ն˲�������", "���뱾�ص�����:", gstAppPosParam.szLocalCode, &nLen, 4, 4, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  ����ǩ������������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionSafeKeep(void)
{
	char szNum[2] = {0};
	int nNum,nLen;

	szNum[0] = gstAppPosParam.cSafeKeepYear;
	ASSERT_RETURNCODE( PubInputDlg("ϵͳ��������", "����ǩ������������", szNum, &nLen, 0, 1, 0, INPUT_MODE_NUMBER));
	nNum=atoi(szNum);
	gstAppPosParam.cSafeKeepYear = '0' + atoi(szNum);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

static char gcIsChkPinpad = YES;/**<���ڱ�ʶ�Ƿ���Ҫ����������*/
static char gcIsChkInside = YES;/**<���ڱ�ʶ������������Ƿ��ʼ����>*/
static char gcIsChkRF = YES; /**<���ڱ�ʶ�Ƿ���Ҫ��ʼ��RFģ��>*/

/*
* @brief �����Ƿ���Ҫ�������������
* @param in cIsChk �Ƿ����ʶ��YES/NO
* @return  ��
*/
void SetControlChkPinpad(const YESORNO cIsChk)
{
	gcIsChkPinpad = cIsChk;
}
/**
* @brief �����Ƿ���Ҫ��ʼ�������������
* @param in cIsChk �Ƿ����ʶ��YES/NO
* @return  ��
*/
void SetControlChkInside(const YESORNO cIsChk)
{
	gcIsChkInside = cIsChk;
}
/**
* @brief �����Ƿ���Ҫ�������������
* @param in cIsChk �Ƿ����ʶ��YES/NO
* @return  ��
*/
void SetControlChkRF(const YESORNO cIsChk)
{
	gcIsChkRF = cIsChk;
}

/**
* @brief ����Ƿ���Ҫ�������������
* @return
* @li YES
* @li NO
*/
YESORNO GetControlChkPinpad(void)
{
	if (gcIsChkPinpad==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
/**
* @brief ����Ƿ���Ҫ��ʼ�������������
* @return
* @li YES
* @li NO
*/
YESORNO GetControlChkInside(void)
{
	if (gcIsChkInside==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
/**
* @brief ����Ƿ���Ҫ�������������
* @return
* @li YES
* @li NO
*/
YESORNO GetControlChkRF(void)
{
	if (gcIsChkRF ==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief �����Ƿ����������̵ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPinPad(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_PINPAD, NULL))
	{
		ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "�������������", "0.����|1.����", &gstAppPosParam.cIsPinPad));
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
		ChkPinpad();
		return APP_SUCC;
	}
	if (gstAppPosParam.cIsPinPad == YES)
	{
		return SetFunctionPinpadTimeOut();
	}
	return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
}

/**
* @brief �����������ͷ�ʽ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsOfflineSendNow(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("���߽��׿���", "�������ͷ�ʽ", "0.������ǰ����||1.�±���������", &gstAppPosParam.cIsOfflineSendNow));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief �����Ƿ�֧��IC�����ױ�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsIcFlag(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("������������", "IC������֧��", "0.��֧��|1.֧��", &gstAppPosParam.cIsIcFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief �����Ƿ���ʾȷ��IC����־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsIcConfirmFlag(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("������������", "IC������ȷ����ʾ", "0.����ʾ|1.��ʾ", &gstAppPosParam.cIsIcConfirmFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���������뿨�ſ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsCardInput(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("�������׿���", "�ֹ��俨��", "0.�ر�|1.��", &gstAppPosParam.cIsCardInput));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ��ӡ���ϱ��浥�ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPrintErrReport(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("������������", "��ӡ���ϱ��浥", "0.����ӡ|1.��ӡ", &gstAppPosParam.cIsPrintErrReport));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����POS����״̬
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPosState(void)
{
	char cPosState;
	int nWaterNum,nSelect;

	cPosState = gstAppPosParam.cPosState;
	nSelect = cPosState == '0' ? 1 : 0;
	ASSERT_QUIT(PubSelectListItem("0.����|1.����", "������������", "����״̬", nSelect, &nSelect));
	cPosState = nSelect == 0 ? '1' : '0';
	if ( cPosState != gstAppPosParam.cPosState )
	{
		GetWaterNum(&nWaterNum);
		if (nWaterNum>0)
		{
			PubMsgDlg("����POS����״̬", "����ˮ��δ����!\n���ܸı佻��״̬", 0, 3);
			return APP_SUCC;
		}
		else
		{
			gstAppPosParam.cPosState = cPosState;
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
		}
	}
	return APP_SUCC;
}

/**
* @brief ����ȱʡ��������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionDefaultTransType(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("�������׿���", "ȱʡ��������", "0.Ԥ��Ȩ|1.����", &gstAppPosParam.cDefaultTransType));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ������ʽ��ӡ��ʱʹ�õ�ǩ�����ݸ�ʽ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsNewTicket(void)
{
	int nSel;

	if (PubGetPrinter() != _PRINTTYPE_IP )
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	nSel = gstAppPosParam.cIsNewTicket-'0';
	ASSERT_RETURNCODE(ProSelectList( "0.��||1.��||2.�հ�", "�״�ǩ������ʽ", nSel, &nSel));
	gstAppPosParam.cIsNewTicket = nSel+'0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief ������Ȩ�������ʱ�Ƿ���Ҫ����ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsAuthSalePin(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("�������ܿ���", "��Ȩ�����������", "0.������|1.����", &gstAppPosParam.cIsAuthSalePin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ó������Ƿ���Ҫ����ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsVoidPin(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("�������ܿ���", "���ѳ�����������", "0.������|1.����", &gstAppPosParam.cIsVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ó�����Ȩ���ʱ�Ƿ���Ҫ����ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsAuthSaleVoidPin(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("�������ܿ���", "��Ȩ��ɳ�������", "0.������|1.����", &gstAppPosParam.cIsAuthSaleVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����Ԥ��Ȩ����ʱ�Ƿ���Ҫ��������ı�־
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPreauthVoidPin(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("�������ܿ���", "Ԥ��Ȩ��������", "0.������|1.����", &gstAppPosParam.cIsPreauthVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ó���ʱ�Ƿ���Ҫˢ���ı�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFunctionIsSaleVoidStrip(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("����ˢ������", "���ѳ���ʱˢ��", "0.��ˢ��|1.ˢ��", &gstAppPosParam.cIsSaleVoidStrip));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ó�����Ȩ���ʱ�Ƿ���Ҫˢ���ı�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFunctionIsAuthSaleVoidStrip(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("����ˢ������", "��Ȩ��ɳ���ˢ��", "0.��ˢ��|1.ˢ��", &gstAppPosParam.cIsAuthSaleVoidStrip));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ������Կ�㷨
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionDesMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ն˲�������", "��Կ�㷨", "0.��������Կ||1.˫������Կ", &gstAppPosParam.cEncyptMode));
	if(gstAppPosParam.cEncyptMode == DESMODE_DES)
		SetVarIsEncryptTrack(NO);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����PIN����ģʽ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPinEncyptMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ն˲�������", "PIN����ģʽ", "0.�������ʺż���||1.�����ʺż���", &gstAppPosParam.cPinEncyptMode));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����ǩ�����Ƿ��Ӣ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctonIsTickeWithEn(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	if (PubGetPrinter() != _PRINTTYPE_TP )
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}

	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "ǩ�����Ƿ��Ӣ��", "0.��|1.��", &gstAppPosParam.cIsTicketWithEn));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����Ԥ��Ȩ�����Ƿ����ο���,Ĭ�ϲ�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPreauthShieldPan(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "Ԥ��Ȩ���ο���", "0.��|1.��", &gstAppPosParam.cIsPreauthShieldPan));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

EM_PRN_FONT_SIZE GetVarPrnFontSize(void)
{
	if ('0' == gstAppPosParam.cPntFontSize)
	{
		return PRN_FONT_SMALL;
	}
	else
	{
		return PRN_FONT_BIG;
	}
}

/**
* @brief �����Ƿ���ʾ��ӡ��ϸ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPrintWaterRec(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("���㽻�׿���", "�Ƿ���ʾ��ӡ��ϸ", "0.��|1.��", &gstAppPosParam.cIsPrintWaterRec));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ���ʾ��ӡʧ��������ϸ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPrintFailWaterRec(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ն˲�������", "�Ƿ���ʾ��ӡʧ��������ϸ", "0.��|1.��", &gstAppPosParam.cIsPrintFailWaterRec));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�֧��С��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsTipFlag(void)
{
	if (gstAppPosParam.cIsTipFlag != '0' && gstAppPosParam.cIsTipFlag != '1' )
	{
		gstAppPosParam.cIsTipFlag = '1';
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("���׹�������", "�Ƿ�֧��С��", "0.��֧��|1.֧��", &gstAppPosParam.cIsTipFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	if( gstAppPosParam.cIsTipFlag == '1' )
	{
		return SetFunctionTipRate();
	}
	return APP_SUCC;
}

/**
* @brief ����С�ѱ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTipRate()
{
	int nLen;
	
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	if( gstAppPosParam.szTipRate[0] == 0 )
	{
		strcpy( gstAppPosParam.szTipRate, "15" );
	}

	ASSERT_RETURNCODE(PubInputDlg("ϵͳ��������", "����С�ѱ���(%):", gstAppPosParam.szTipRate, &nLen, 1, 2, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief �����Ƿ��Զ�ǩ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionAutoLogout(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("���㽻�׿���", "������Զ�ǩ��", "0.��֧��|1.֧��", &gstAppPosParam.cIsAutoLogOut));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�EMV������ʾTVR TSI
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsDispEMV_TVRTSI(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("������������", "������ʾTVR TSI", "0.����ʾ|1.��ʾ", &gstAppPosParam.cIsDispEMV_TVRTSI));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ���Ҫ���ο���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsShieldPan(void)
{
	if (gstAppPosParam.cIsShieldPan != YES  && gstAppPosParam.cIsShieldPan != NO)
	{
		/**<���⴦����Ծɰ�����stAppPosParam.cIsShieldPan��ԭʼ��ֵ*/
		gstAppPosParam.cIsShieldPan = YES;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("������������", "���ο���", "0.������|1.����", &gstAppPosParam.cIsShieldPan));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�֧�ִŵ�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsEncryptTrack(void)
{
	if(gstAppPosParam.cEncyptMode == DESMODE_DES)
	{
		gstAppPosParam.cIsEncryptTrack = NO;
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ն˲�������", "�ŵ�����", "0.��֧��|1.֧��", &gstAppPosParam.cIsEncryptTrack));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ������֧��С�����Ȩ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsSmallGeneAuth(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ն˲�������", "֧��С�����Ȩ", "0.��֧��|1.֧��", &gstAppPosParam.cIsSmallGeneAuth));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief  �жϰ�ȫ�����Ƿ���ȷ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ChkSecurityPwd(void)
{
	int nLen;
	char szAdminPasswd[8+1];

	while(1)
	{
		memset(szAdminPasswd, 0, sizeof(szAdminPasswd));
		ASSERT_QUIT(PubInputDlg("�ն˲�������", "�����밲ȫ����:", szAdminPasswd, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));
		if(memcmp(gstAppPosParam.szSecurityPwd, szAdminPasswd, 6))
		{
			char szDispBuf[100];
			PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "��ȫ�����,������");
			PubMsgDlg("�ն˲�������", szDispBuf, 0, 1 ) ;
			continue;
		}
		break;

	}
	return APP_SUCC;
}

/**
* @brief  �޸İ�ȫ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ChangeSecurityPwd(void)
{
	int nLen;
	char szAdminPasswd1[8+1];
	char szAdminPasswd2[8+1];


	while(1)
	{
		memset(szAdminPasswd1, 0, sizeof(szAdminPasswd1));
		/**
		 * У��ԭ��ȫ����
		 */
		ASSERT_QUIT(PubInputDlg("�޸İ�ȫ����", "������ԭ����", szAdminPasswd1, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));
		if( memcmp(gstAppPosParam.szSecurityPwd, szAdminPasswd1,  8) )
		{
			PubMsgDlg("�޸İ�ȫ����", "ԭ���벻��", 0, 5) ;
			continue;
		}
		/**
		* ����������
		*/
		memset(szAdminPasswd1, 0, sizeof(szAdminPasswd1));
		ASSERT_QUIT(PubInputDlg("�޸İ�ȫ����", "������������:", szAdminPasswd1, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));

		memset(szAdminPasswd2, 0, sizeof(szAdminPasswd2));
		ASSERT_QUIT(PubInputDlg("�޸İ�ȫ����", "��ȷ��������:", szAdminPasswd2, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));

		if(memcmp(szAdminPasswd1, szAdminPasswd2, 6))
		{
			PubMsgDlg("�޸İ�ȫ����", "�����������벻ͬ", 0, 5) ;
			continue;
		}
		else
		{
			SetVarSecurityPwd(szAdminPasswd1);
			PubMsgDlg("�޸İ�ȫ����", "�����޸ĳɹ�", 0, 5) ;
			return APP_SUCC;
		}
	}
}


/**
* @brief  ���ð�ȫ����
* @param in szSecurityPasswd ��ȫ����ֵ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarSecurityPwd(const char *szSecurityPasswd)
{
	memset(gstAppPosParam.szSecurityPwd,0,sizeof(gstAppPosParam.szSecurityPwd));
	strcpy(gstAppPosParam.szSecurityPwd, szSecurityPasswd);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ����������ˮ����
* @param in nWaterSum ��ˮ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarWaterSum(const int  nWaterSum)
{
	gstSettleParam.nWaterSum = nWaterSum;
	PubSaveVar( (char *)&(gstSettleParam.nWaterSum), fWaterSum_off, fWaterSum_len);
	return APP_SUCC;
}

/**
* @brief ��ý������ˮ����
* @param in pnWaterSum ��ˮ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarWaterSum(int *pnWaterSum)
{
	*pnWaterSum = gstSettleParam.nWaterSum;
	return APP_SUCC;
}

/**
* @brief �������߽������ط��Ľ��״���
* @param in nHaveReSendNum ��ˮ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarHaveReSendNum(const int  nHaveReSendNum)
{
	gstSettleParam.nHaveReSendNum = nHaveReSendNum;
	PubSaveVar( (char *)&(gstSettleParam.nHaveReSendNum), fHaveReSendNum_off, fHaveReSendNum_len);
	return APP_SUCC;
}

/**
* @brief ������߽������ط��Ľ��״���
* @param in pnHaveReSendNum ��ˮ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarHaveReSendNum(int *pnHaveReSendNum)
{
	*pnHaveReSendNum = gstSettleParam.nHaveReSendNum;
	return APP_SUCC;
}

/**
* @brief ���߽���δ���ͱ�������1
* @param in ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarOfflineUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	nNum++;
	PubSaveVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return APP_SUCC;
}

/**
* @brief ���߽���δ���ͱ�����1
* @param in ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int DelVarOfflineUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	nNum--;
	if(nNum <0)
		nNum = 0;
	PubSaveVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return APP_SUCC;
}


/**
* @brief ��ȡ���߽���δ���ͱ���
* @param in ��
* @return
* @li ���߽���δ���ͱ���
*/
int GetVarOfflineUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return nNum;
}


/**
* @brief �������߽���δ���ͱ���
* @param in ��
* @return
* @li APP_SUCC
*/
int SetVarOfflineUnSendNum(int nNum)
{
	PubSaveVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return APP_SUCC;
}

/**
* @brief �����ӡ���㵥�жϱ�ʶ
* @param in ��ӡ�ж�
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarPrintSettleHalt(const YESORNO cPrintSettleHalt)
{
	gstSettleParam.cPrintSettleHalt = cPrintSettleHalt;
	PubSaveVar( (char *)&(gstSettleParam.cPrintSettleHalt), fSettlePrintHaltFlag_off, fSettlePrintHaltFlag_len);
	return APP_SUCC;
}

/**
* @brief ��ô�ӡ���㵥�жϱ�־
* @return
* @li YES
* @li NO
*/
YESORNO GetVarPrintSettleHalt()
{
	if (gstSettleParam.cPrintSettleHalt==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����ӡ��ϸ�жϱ�־
* @param in ��ӡ�ж�
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarPrintDetialHalt(const YESORNO cPrintDetialHalt)
{
	gstSettleParam.cPrintDetialHalt = cPrintDetialHalt;
	PubSaveVar( (char *)&(gstSettleParam.cPrintDetialHalt), fPrintDetialHaltFlag_off, fPrintDetialHaltFlag_len);
	return APP_SUCC;
}

/**
* @brief ��ô�ӡ��ϸ�жϱ�־
* @return
* @li YES
* @li NO
*/
YESORNO GetVarPrintDetialHalt(void)
{
	if (gstSettleParam.cPrintDetialHalt == YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����ӡǩ�����жϱ�־
* @param in ��ӡǩ�����жϱ�־
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarPrintWaterHaltFlag(const YESORNO cPrintWaterHaltFlag)
{
	gstSettleParam.cPrintWaterHaltFlag= cPrintWaterHaltFlag;
	PubSaveVar( (char *)&(gstSettleParam.cPrintWaterHaltFlag), fPrintWaterHaltFlag_off, fPrintWaterHaltFlag_len);
	return APP_SUCC;
}

/**
* @brief ��ô�ӡǩ�����жϱ�־
* @return
* @li YES
* @li NO
*/
YESORNO GetVarPrintWaterHaltFlag(void)
{
	if (gstSettleParam.cPrintWaterHaltFlag== YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ����������������жϱ�ʶ
* @param in ������������жϱ�ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarClrSettleDataFlag(const YESORNO cClrSettleDataFlag)
{
	gstSettleParam.cClrSettleDataFlag = cClrSettleDataFlag;
	PubSaveVar( (char *)&(gstSettleParam.cClrSettleDataFlag), fClrSettleDataFlag_off, fClrSettleDataFlag_len);
	return APP_SUCC;
}

/**
* @brief ���������������жϱ�ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarClrSettleDataFlag(void)
{
	if (gstSettleParam.cClrSettleDataFlag == YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief �����ڿ�����Ӧ�����
* @param in CnCardFlag �ڿ�����Ӧ�����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCnCardFlag(const char CnCardFlag)
{
	gstSettleParam.cCnCardFlag = CnCardFlag;
	PubSaveVar( (char *)&(gstSettleParam.cCnCardFlag), fCnCardFlag_off, fCnCardFlag_len);
	return APP_SUCC;
}

/**
* @brief ����ڿ�����Ӧ�����
* @return
* @li '1'
* @li NO
*/
char GetVarCnCardFlag(void)
{
	if (gstSettleParam.cCnCardFlag == '1' || gstSettleParam.cCnCardFlag == '2' || gstSettleParam.cCnCardFlag == '3')
	{
		return gstSettleParam.cCnCardFlag;
	}
	else
	{
		return '3';
	}
}

/**
* @brief �����⿨����Ӧ�����
* @param in EnCardFlag �⿨����Ӧ�����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarEnCardFlag(const char EnCardFlag)
{
	gstSettleParam.cEnCardFlag = EnCardFlag;
	PubSaveVar( (char *)&(gstSettleParam.cEnCardFlag), fEnCardFlag_off, fEnCardFlag_len);
	return APP_SUCC;
}

/**
* @brief ����⿨��ƽ��ʶ
* @return
* @li YES
* @li NO
*/
char GetVarEnCardFlag(void)
{
	if (gstSettleParam.cEnCardFlag == '1' || gstSettleParam.cEnCardFlag == '2' || gstSettleParam.cEnCardFlag == '3')
	{
		return gstSettleParam.cEnCardFlag;
	}
	else
	{
		return '3';
	}
}

/**
* @brief �����������жϱ�ʶ
* @param in BatchHaltFlag �������жϱ�ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarBatchHaltFlag(const YESORNO cBatchHaltFlag)
{
	gstSettleParam.cBatchHaltFlag = cBatchHaltFlag;
	PubSaveVar( (char *)&(gstSettleParam.cBatchHaltFlag), fBatchHaltFlag_off, fBatchHaltFlag_len);
	return APP_SUCC;
}

/**
* @brief ����������жϱ�ʶ
* @return
* @li YES
* @li NO
*/
YESORNO GetVarBatchHaltFlag(void)
{
	if (gstSettleParam.cBatchHaltFlag == YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ������������ͱ�ʶ
* @param in nFinanceHaltFlag �ڼ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarFinanceHaltFlag(const int  nFinanceHaltFlag)
{
	gstSettleParam.nFinanceHaltFlag = nFinanceHaltFlag;
	PubSaveVar( (char *)&(gstSettleParam.nFinanceHaltFlag), fFinanceHaltNum_off, fFinanceHaltNum_len);
	return APP_SUCC;
}

/**
* @brief ��ý��������ͱ�ʶ
* @param in pnFinanceHaltFlag ���������ͱ�ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarFinanceHaltFlag(int *pnFinanceHaltFlag)
{
	*pnFinanceHaltFlag = gstSettleParam.nFinanceHaltFlag;
	return APP_SUCC;
}

/**
* @brief ����֪ͨ�����ͱ�ʶ
* @param in nMessageHaltFlag �ڼ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarMessageHaltFlag(const int  nMessageHaltFlag)
{
	gstSettleParam.nMessageHaltFlag = nMessageHaltFlag;
	PubSaveVar( (char *)(&gstSettleParam.nMessageHaltFlag), fMessageHaltNum_off, fMessageHaltNum_len);
	return APP_SUCC;
}

/**
* @brief ���֪ͨ�����ͱ�ʶ
* @param in pnMessageHaltFlag ֪ͨ�����ͱ�ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarMessageHaltFlag(int *pnMessageHaltFlag)
{
	*pnMessageHaltFlag = gstSettleParam.nMessageHaltFlag;
	return APP_SUCC;
}

/**
* @brief ��������������������ͱ�ʶ
* @param in nMessageHaltFlag �ڼ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarBatchMagOfflineHaltFlag(const int  nBatchMagOfflineHaltFlag)
{
	gstSettleParam.nBatchMagOfflinHaltFlag = nBatchMagOfflineHaltFlag;
	PubSaveVar( (char *)(&gstSettleParam.nBatchMagOfflinHaltFlag), fBatchMagOfflineHaltNum_off, fBatchMagOfflineHaltNum_len);
	return APP_SUCC;
}

/**
* @brief ��ô����������������ͱ�ʶ
* @param in pnMessageHaltFlag ֪ͨ�����ͱ�ʶ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarBatchMagOfflineHaltFlag(int *pnBatchMagOfflineHaltFlag)
{
	*pnBatchMagOfflineHaltFlag = gstSettleParam.nBatchMagOfflinHaltFlag;
	return APP_SUCC;
}

/**
* @brief ���������������ܱ���
* @param in nBatchSum �������ܱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarBatchSum(const int  nBatchSum)
{
	gstSettleParam.nBatchSum = nBatchSum;
	PubSaveVar( (char *)&(gstSettleParam.nBatchSum), fBatchUpSum_off, fBatchUpSum_len);
	return APP_SUCC;
}

/**
* @brief ��ý�����������ܱ���
* @param in pnBatchSum �������ܱ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarBatchSum(int *pnBatchSum)
{
	*pnBatchSum = gstSettleParam.nBatchSum;
	return APP_SUCC;
}

/**
* @brief ��ý����ʱ��
* @param in psSettleDateTime �����ʱ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarSettleDateTime(char *psSettleDateTime)
{
	memcpy(psSettleDateTime, gstSettleParam.sSettleDateTime, fSettleDateTime_len);
	return APP_SUCC;
}

/**
* @brief ��������ʱ��
* @param in nBatchSum �����ʱ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarSettleDateTime(const char *psSettleDateTime)
{
	memcpy( gstSettleParam.sSettleDateTime, psSettleDateTime, fSettleDateTime_len);
	PubSaveVar( (char *)&(gstSettleParam.sSettleDateTime), fSettleDateTime_off, fSettleDateTime_len);
	return APP_SUCC;
}


/**
* @brief �����������
* @param in stSettle_NK	�ڿ���������
* @param in stSettle_WK	�⿨��������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarSettleData(const STSETTLE stSettle_NK, const STSETTLE stSettle_WK)
{
	int nSizeNk, nSizeWk;

	nSizeNk = sizeof(STSETTLE);
	nSizeWk = sizeof(STSETTLE);
	memcpy( gstSettleParam.sNumAmount, (char *)(&stSettle_NK), nSizeNk);
#if WK	
	memcpy( gstSettleParam.sNumAmount+nSizeNk, (char *)(&stSettle_WK), nSizeWk);
	PubSaveVar( (char *)(&gstSettleParam.sNumAmount), fSettleData_off, nSizeNk + nSizeWk);
#else
	PubSaveVar( (char *)(&gstSettleParam.sNumAmount), fSettleData_off, nSizeNk);
#endif	
	return APP_SUCC;
}

/**
* @brief ��ý�������
* @param in pstSettle_NK	�ڿ���������
* @param in pstSettle_WK	�⿨��������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarSettleData(STSETTLE *pstSettle_NK, STSETTLE *pstSettle_WK)
{
	int nSizeNk;
	
	if ( pstSettle_NK != NULL )
	{
		nSizeNk = sizeof(STSETTLE);
		PubGetVar((char *)pstSettle_NK, fSettleData_off, nSizeNk);
	}
#if WK	
	if( pstSettle_WK != NULL )
	{
		int nSizeWk;
		nSizeWk = sizeof(STSETTLE);
		PubGetVar((char *)pstSettle_WK, fSettleData_off + nSizeNk, nSizeWk);
	}
#endif	
	return APP_SUCC;
}

/**
* @brief �������������
* @param in psSettDataItem		��Ҫ����Ľ���������
* @param in nOffset			��Ҫ����Ľ���������������ƫ��
* @param in nLen				��Ҫ����Ľ����������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SaveSettleDataItem( const char * psSettDataItem, const int nOffset, const int nLen)
{
	return PubSaveVar(psSettDataItem, fSettleData_off+nOffset, nLen);
}

/**
* @brief ���POS��Ӧ�ò������ã�
* @param out pstAppPosParam  ���ش洢POSӦ�ò������õĽṹ��ַ
* @return ��
*/
void GetAppPosParam(STAPPPOSPARAM *pstAppPosParam )
{
	memcpy( (char *)pstAppPosParam, (char *)(&gstAppPosParam), sizeof(STAPPPOSPARAM) );
}


/**
* @brief ���POS��Ӧ�ò������ã�
* @param out pstAppPosParam  ���ش洢POSӦ�ò������õĽṹ��ַ
* @return ��
*/
int SetAppPosParam(STAPPPOSPARAM *pstAppPosParam )
{
	memcpy((char *)(&gstAppPosParam),  (char *)pstAppPosParam, sizeof(STAPPPOSPARAM));

	return PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam);
}

/**
* @brief �������ؽ���ʱ��ʱ�����Ӻ������
* @param in nTimes ʱ������
* @param in nRandom �����
* @return ��
*/
int SetVarMCEnterTimeAndRandom(int nTimes, int nRandom)
{
	int nRet;

	gstBankParam.nMCTimes = nTimes;
	gstBankParam.nMCRandom = nRandom;

	nRet = PubSaveVar((char *)&gstBankParam.nMCTimes, fMC_EnterDateTime_off, fMC_EnterDateTime_len);
	nRet += PubSaveVar((char *)&gstBankParam.nMCRandom, fMC_EnterRandom_off, fMC_EnterRandom_len);

	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief ��ȡ���ؽ���ʱ��ʱ�����Ӻ������
* @param out pnTimes ʱ������
* @param out pnRandom �����
* @return ��
*/
int GetVarMCEnterTimeAndRandom(int *pnTimes, int *pnRandom)
{
	*pnTimes = gstBankParam.nMCTimes;
	*pnRandom = gstBankParam.nMCRandom;

	return APP_SUCC;
}

/**
* @brief ���õ�½�Ĳ���Ա��Ϣ
* @param in pszOperNo ����Ա����
* @param in cRole ����Ա����(��ɫ)
* @return ��
*/
int SetVarLastOperInfo(char *pszOperNo, char cRole)
{
	int nRet;

	memcpy(gstBankParam.szLastLoginOper, pszOperNo, sizeof(gstBankParam.szLastLoginOper) - 1);
	gstBankParam.cLastLoginOperRole = cRole;

	nRet = PubSaveVar(gstBankParam.szLastLoginOper, fLastLoginOperNo_off, fLastLoginOperNo_len);
	nRet += PubSaveVar(&gstBankParam.cLastLoginOperRole, fLastLoginOperRole_off, fLastLoginOperRole_len);

	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief ����ϻص�½�Ĳ���Ա��Ϣ
* @param out pszOperNo ����Ա����
* @param out pnRole ����Ա����(��ɫ)
* @return ��
*/
int GetVarLastOperInfo(char *pszOperNo, int *pnRole)
{
	memcpy(pszOperNo, gstBankParam.szLastLoginOper, sizeof(gstBankParam.szLastLoginOper) - 1);
	*pnRole = (int)gstBankParam.cLastLoginOperRole;

	return APP_SUCC;
}

/**
* @brief �����Ƿ�����KEK��ʶ
* @param in cMode �㷨��ʾ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarDownKek(const YESORNO cFlag)
{	
	gstBankParam.cIsDownKek= cFlag;
	PubSaveVar(&gstBankParam.cIsDownKek,fIsDownKek_off,fIsDownKek_len);
	return APP_SUCC;
}

/**
* @fn GetVarIDownParam
* @brief ��ȡ���ز�����ʶ��ʶ
* @return
* @li YES
* @li NO
* @author 
* @date
*/
YESORNO GetVarDownKek(void)
{	
	if (gstBankParam.cIsDownKek==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}




/**
* @brief ����UID
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionUID(char *psValue)
{
	int nLen;
	char szContent[32+1] = {0};

	if (NULL == psValue)
	{
		ASSERT_QUIT(PubInputDlg("������������", "�����û�ID(16λ)", szContent, &nLen, 0, 16, 0, INPUT_MODE_STRING));
		if(strlen(szContent)==0)
		{
			memcpy(szContent, "1234567890123456", 16);
		}
	}
	else
	{
		memcpy(szContent, psValue, 16);
	}
	memcpy(gstAppPosParam.sUID, szContent, 16);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM, 1, (char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡUID
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarUID(char *psValue)
{
	if (NULL == psValue)
	{
		return APP_FAIL;
	}

	memcpy(psValue, gstAppPosParam.sUID, 16);
	return APP_SUCC;
}



YESORNO GetVarIsPhoneSale()
{
	int nFlag = 0;

	PubGetVar((char*)&nFlag, fIsPhoneSale_off, fIsPhoneSale_len);
	if(nFlag == YES)
		return YES;
	else
		return NO;
}

int SetVarIsPhoneSale(const YESORNO cFlag)
{
	PubSaveVar((char*)&cFlag, fIsPhoneSale_off, fIsPhoneSale_len);
	return APP_SUCC;
}


int GetVarCertNo(char* pszCertNo)
{
	char szPosType[60] = {0};

	strcpy(szPosType, PubGetPosTypeStr());
PubDebug("��ȡ�����ʹ�[%s]", szPosType);
	if (0 == memcmp(szPosType, "NL-GP720", 8))
		strcpy(pszCertNo, "3201");
	else if (0 == memcmp(szPosType, "SP60", 4))
		strcpy(pszCertNo, "3212");
	else if (0 == memcmp(szPosType, "SP80", 4) || 0 == memcmp(szPosType, "SPc50", 5))
		strcpy(pszCertNo, "3211");
	else if (0 == memcmp(szPosType, "ME31", 4))
		strcpy(pszCertNo, "3188");
	else if (0 == memcmp(szPosType, "IM81", 4))
		strcpy(pszCertNo, "3183");
	else if (0 == memcmp(szPosType, "SP50", 4))
		strcpy(pszCertNo, "3137");
	else
		strcpy(pszCertNo, "    ");
	PubDebug("����֤��[%s]\n", pszCertNo);

	return APP_SUCC;
}

void GetVarSoftVer(char* pszSoftVer)
{
	memcpy(pszSoftVer, APP_VERSION, 6);
}

void GetVarDispSoftVer(char* pszSoftVer)
{
	strcpy(pszSoftVer, APP_VERSION_DISP);
}


int SetFunctionMaxOffSendNum(void)
{
	int nLen;
	int nNumber;
	char szNumber[5+1];

	nNumber = atoi(gstAppPosParam.szMaxOffSendNum);
	sprintf(szNumber,  "%d", nNumber );
	ASSERT_RETURNCODE(PubInputDlg("���׹�������", "�������ͱ���", szNumber, &nLen, 1, 2, 0, INPUT_MODE_NUMBER));
	nNumber = atoi(szNumber);

	sprintf(gstAppPosParam.szMaxOffSendNum, "%02d", nNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


void GetVarMaxOffSendNum(int *pnValue)
{
	char szTmp[10] = {0};

	memcpy(szTmp,gstAppPosParam.szMaxOffSendNum,sizeof(gstAppPosParam.szMaxOffSendNum)-1);
	*pnValue=atoi(szTmp);
	return;
}


int SetFunctionPinpadTimeOut(void)
{
	int nLen;

	if (YES == GetVarIsPinpad())
	{
		ASSERT_RETURNCODE(PubInputDlg("���̲���", "���̳�ʱʱ��:", gstAppPosParam.szPinPadTimeOut, &nLen, 1, 3, 0, INPUT_MODE_NUMBER));
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
		return APP_SUCC;
	}
	return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
}

#ifdef USE_TMS
/**
* @brief ��ȡTMS�·�������Ϣ
* @li APP_SUCC
*/
int GetTmsParamDown()
{
	int nLen = 99;

	memset(gstAppPosParam.szAdvertisement,0,sizeof(gstAppPosParam.szAdvertisement));
	MC_GetPubParam(APP_ADVERTISEMENT,gstAppPosParam.szAdvertisement,(uint *) &nLen);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	Ums_SetIsModify();
	return APP_SUCC;
}
/**
* @brief ��ȡ�����Ϣ����
* @param out pcReSendNum 1λ�ط�����
* @li APP_SUCC
*/
int GetAdvertisementParam(char * szAdv,int * plen)
{
	*plen= strlen(gstAppPosParam.szAdvertisement);
	memcpy(szAdv,gstAppPosParam.szAdvertisement,*plen);
	return APP_SUCC;
}

/**
* @brief ����TMS�ط�����
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncTmsReSendNum(void)
{	
	char szTemp[3+1] = {0};
	int nLen = 0;

	PubClear2To4();
	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%d", gstAppPosParam.cTmsReSendNum);
	ASSERT_RETURNCODE(PubInputDlg("TMS ��������", "TMS���Դ���:", szTemp, &nLen, 1, 2, 60, INPUT_MODE_NUMBER));
	
	gstAppPosParam.cTmsReSendNum = atoi(szTemp);
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
} 

int GetVarTmsReSendNum(char *pcTmsReSendNUm)
{
	*pcTmsReSendNUm = gstAppPosParam.cTmsReSendNum;
	return APP_SUCC;
}
#endif

/**
* @brief ��ȡ�Ƿ��ӡ�����յ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
YESORNO GetIsPntChAquirer(void)
{
	if (gstAppPosParam.cIsPntChAcquirer== YES || gstAppPosParam.cIsPntChAcquirer== 1)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����Ƿ��ӡ�����յ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetIsPntChAquirer(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "��ӡ�����յ���", "0.�ر�|1.��", &gstAppPosParam.cIsPntChAcquirer));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡ�Ƿ��ӡ���ķ�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
YESORNO GetIsPntChCardScheme(void)
{
	if (gstAppPosParam.cIsPntChCardScheme== YES || gstAppPosParam.cIsPntChCardScheme== 1)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �����Ƿ��ӡ���ķ�����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetIsPntChCardScheme(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("ϵͳ��������", "��ӡ���ķ�����", "0.�ر�|1.��", &gstAppPosParam.cIsPntChCardScheme));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡ���ز˵�����Ա����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int GetHideMenuPwd(char *pszPwd)
{
	memcpy(pszPwd,gstAppPosParam.szHideMenuPwd,8);
	return APP_SUCC;
}


/**
* @brief ����ǩ����̧ͷģʽ
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPrintTitleMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("ǩ������ӡ", "ǩ����̧ͷѡ��", "0.����||1.LOGO", &gstAppPosParam.cPntTitleMode));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	if ( NO == gstAppPosParam.cPntTitleMode) /*<��ӡ����*/
	{
		return SetFunctionPntTitle();
	}
	return APP_SUCC;
}
/**
* @brief ��ȡǩ����̧ͷѡ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
YESORNO GetIsPntTitleMode(void)
{
	if (gstAppPosParam.cPntTitleMode== YES || gstAppPosParam.cPntTitleMode== 1)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ����ǩ����̧ͷ����
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPntTitle(void)
{
	int nRet, nLen;

	PubClearAll();
	PubDisplayTitle("ǩ������ӡ");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szPntTitleCn);
	PubDisplayStrInline(1, 4, "�Ƿ����? 1.��");
	PubUpdateWindow();
	while(1)
	{
		nRet = PubGetKeyCode(10);
		if ( nRet == KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("ǩ������ӡ", NULL, gstAppPosParam.szPntTitleCn, &nLen, 0, 40, CHOOSE_IME_MODE_NUMPY));
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			return APP_SUCC;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if(nRet == KEY_ESC)
		{
			return APP_QUIT;
		}
		else if(nRet == KEY_ENTER)
		{
			return APP_SUCC;
		}	
	}
	return APP_SUCC;
}
/**
* @brief  ��ȡǩ����̧ͷ������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int GetFunctionPntTitle(char *pPntTitleCn)
{
	strcpy(pPntTitleCn,gstAppPosParam.szPntTitleCn);
	return APP_SUCC;
}
/**
* @brief  ���÷������ߺ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionHotLineNo(void)
{
	int nLen;
	ASSERT_RETURNCODE(PubInputDlg("ǩ������ӡ", "������������", gstAppPosParam.szHotLine, &nLen, 0, 20, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  ��ȡ�������ߺ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int GetFunctionHotLineNo(char *pszHotLine)
{
	strcpy(pszHotLine,gstAppPosParam.szHotLine);
	return APP_SUCC;
}

/**
* @brief ����ǩ���������ӡѡ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPrintSize(void)
{
	int nSelect;
	switch(gstAppPosParam.cPntFontSize)
	{
	case '0':
		nSelect = 0;
		break;
	case '1':
		nSelect = 1;
		break;
	case '2':
		nSelect = 2;
		break;
	default:
		nSelect = 0XFF;
		break;
	}
	ASSERT_RETURNCODE(PubSelectListItem("0.С||1.��||2.��", "ǩ��������ѡ��", NULL, nSelect, &nSelect));
	gstAppPosParam.cPntFontSize = (char)nSelect + '0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�������������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsAdminPin(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("�������׿���", "�Ƿ�������������", "0.������|1.����", &gstAppPosParam.cIsAdminPwd));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ��ȡ�Ƿ�������������
* @return
* @li YES ��Ҫ
* @li NO ����
*/
YESORNO GetVarIsAdminPin()
{
	if (gstAppPosParam.cIsAdminPwd==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
* @brief ���ô�ͳ�������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTraditionSwitch(void)
{
	int i, nRet;
	int nTransNum=11;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[11][17]= {"����","���ѳ���","�˻�","����ѯ","Ԥ��Ȩ",
	                      "Ԥ��Ȩ����","Ԥ��Ȩ�������","Ԥ��Ȩ���֪ͨ",
	                      "Ԥ��Ȩ��ɳ���","���߽���","�������",
	                     };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sTraditionSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("��ͳ���׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sTraditionSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sTraditionSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���õ����ֽ����Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionECashSwitch(void)
{
	int i, nRet;
	int nTransNum=7;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[7][17]= {"�Ӵ������ֽ�����","����֧��(�ǽ�)",
	                     "ָ���˻�Ȧ��","��ָ���˻�Ȧ��","�����ֽ��ֽ��ֵ",
	                     "�����ֽ��ֵ����","�����ֽ��ѻ��˻�",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sECashSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("�����ֽ𿪹ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sECashSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sECashSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���õ���Ǯ�����Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionWalletSwitch(void)
{
	int i, nRet;
	int nTransNum=4;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[4][17]= {"����Ǯ������","ָ���˻�Ȧ��",
	                     "��ָ���˻�Ȧ��","����Ǯ���ֽ��ֵ",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sWalletSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("����Ǯ�����ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sWalletSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sWalletSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���÷��ڸ������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionInstallmentSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[2][17]= {"���ڸ�������","���ڸ������ѳ���",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sInstallmentSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("���ڸ���ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sInstallmentSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sInstallmentSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���û������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionBonusSwitch(void)
{
	int i, nRet;
	int nTransNum=6;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[6][17]= {"���˻�������","�����л�������","���˻������ѳ���",
	                     "���������ѳ���","���˻��ֲ�ѯ","���˻����˻�",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sBonusSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("���ֽ��׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sBonusSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sBonusSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����ֻ�оƬ���Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPhoneChipSaleSwitch(void)
{
	int i, nRet;
	int nTransNum=9;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[9][17]= {"�ֻ�����","�ֻ����ѳ���","�ֻ�оƬ�˻�",
	                     "�ֻ�оƬԤ��Ȩ","�ֻ�Ԥ��Ȩ����","Ԥ��Ȩ�������",
	                     "Ԥ��Ȩ���֪ͨ","Ԥ��Ȩ��ɳ���","�ֻ�оƬ����ѯ",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sPhoneChipSaleSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("�ֻ�оƬ���ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sPhoneChipSaleSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sPhoneChipSaleSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ����ԤԼ���Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionAppointmentSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[2][17]= {"ԤԼ����","ԤԼ���ѳ���",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sAppointmentSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("ԤԼ���׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sAppointmentSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sAppointmentSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���ö������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionOrderSwitch(void)
{
	int i, nRet;
	int nTransNum=8;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[8][17]= {"��������","�������ѳ���","�����˻�",
	                     "����Ԥ��Ȩ","����Ԥ��Ȩ����","Ԥ��Ȩ�������",
	                     "Ԥ��Ȩ���֪ͨ","Ԥ��Ȩ��ɳ���",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sOrderSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("�������׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sOrderSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sOrderSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief �����������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionOtherSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[2][17]= {"�������ֽ��ֵ","�������˻���ֵ",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sOtherSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("�������׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sOtherSwitch[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sOtherSwitch[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ����������������
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsManagePinInput(void)
{
	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	ASSERT_RETURNCODE( ProSelectYesOrNo("�������׿���", "������������", "0.��|1.��", &gstAppPosParam.cIsAdminPwd));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
*  ��Դ�ͳ����,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetTraditionSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_SALE:						/**< ����*/
		nTransNum = 0;
		break;
	case TRANS_VOID_SALE:				/**< ���ѳ���*/
		nTransNum = 1;
		break;
	case TRANS_REFUND:					/**< �˻�*/
		nTransNum = 2;
		break;
	case TRANS_BALANCE:					/**< ��ѯ*/
		nTransNum = 3;
		break;
	case TRANS_PREAUTH:					/**< Ԥ��Ȩ*/
		nTransNum = 4;
		break;
	case TRANS_VOID_PREAUTH:				/**< ��Ȩ����*/
		nTransNum = 5;
		break;
	case TRANS_AUTHSALE:					/**< ��Ȩ�������*/
		nTransNum = 6;
		break;
	case TRANS_AUTHSALEOFF:				/**< ��Ȩ���֪ͨ*/
		nTransNum = 7;
		break;
	case TRANS_VOID_AUTHSALE:			/**< ��Ȩ���������*/
		nTransNum = 8;
		break;
	case TRANS_OFFLINE:					/**< ���߽���*/
		nTransNum = 9;
		break;
	case TRANS_ADJUST:					/**< �������*/
		nTransNum =10;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sTraditionSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
*  ��Ե����ֽ���,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetECashSwitchOnoff(char cTransType,char cAttr)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_EMV_REFUND:				/**< �����ֽ��ѻ��˻�*/
		nTransNum = 6;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sECashSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
*  ��Է��ڸ����,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetInstallmentSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_INSTALMENT:				/**< ���ڸ���*/
		nTransNum = 0;
		break;
	case TRANS_VOID_INSTALMENT:			/**< ��������*/
		nTransNum = 1;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sInstallmentSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
*  ��Ի��ֽ���,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetBonusSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_BONUS_ALLIANCE:			/**< ���˻�������*/
		nTransNum = 0;
		break;
	case TRANS_BONUS_IIS_SALE:			/**< �����л�������*/
		nTransNum = 1;
		break;
	case TRANS_VOID_BONUS_ALLIANCE:		/**< �������˻�������*/
		nTransNum = 2;
		break;
	case TRANS_VOID_BONUS_IIS_SALE:		/**< ���������л�������*/
		nTransNum = 3;
		break;
	case TRANS_ALLIANCE_BALANCE:			/**< ���˻��ֲ�ѯ*/
		nTransNum = 4;
		break;
	case TRANS_ALLIANCE_REFUND:			/**< ���˻����˻�*/
		nTransNum = 5;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sBonusSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
*  ����ֻ�оƬ����,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetPhoneChipSaleSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_PHONE_SALE:				/**< �ֻ�оƬ����*/
		nTransNum = 0;
		break;
	case TRANS_VOID_PHONE_SALE:			/**< �ֻ�оƬ���ѳ���*/
		nTransNum = 1;
		break;
	case TRANS_REFUND_PHONE_SALE:		/**< �ֻ�оƬ�˻�*/
		nTransNum = 2;
		break;
	case TRANS_PHONE_PREAUTH:			/**< �ֻ�оƬԤ��Ȩ*/
		nTransNum = 3;
		break;
	case TRANS_VOID_PHONE_PREAUTH:		/**< �ֻ�оƬԤ��Ȩ����*/
		nTransNum = 4;
		break;
	case TRANS_PHONE_AUTHSALE:			/**< �ֻ�оƬԤ��Ȩ�������*/
		nTransNum = 5;
		break;
	case TRANS_PHONE_AUTHSALEOFF:		/**< �ֻ�оƬԤ��Ȩ���֪ͨ*/
		nTransNum = 6;
		break;
	case TRANS_VOID_PHONE_AUTHSALE:		/**< �ֻ�оƬԤ��Ȩ��ɳ���*/
		nTransNum = 7;
		break;
	case TRANS_PHONE_BALANCE:			/**< �ֻ�оƬ����ѯ*/
		nTransNum = 8;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sPhoneChipSaleSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
*  ���Ԥ������,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetAppointmentSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sAppointmentSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


/**
*  ��Զ�������,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetOrderSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sOrderSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
*  �����������,��������Ľ��������ж��Ƿ�֧�ִ˽���
* @param in cTransType ��������
* @return
* @li YES
* @li NO
*/
YESORNO GetOtherSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sOtherSwitch[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
/**
* switch ���׿��ؿ���ֵ
* n ���صڼ�λ
* i ���ض�Ӧ����
*/
#define TraditionSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sTraditionSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sTraditionSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))

#define ECashSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sECashSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sECashSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
#define WalletSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sWalletSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sWalletSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
#define InstallmentSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sInstallmentSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sInstallmentSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
	
#define BonusSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sBonusSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sBonusSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
#define AppointmentSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sAppointmentSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sAppointmentSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
#define OrderSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sOrderSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sOrderSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))
#define OtherSwitch(swich,n,i) \
	if (((swich)&(n)) == (n) )\
		(gstAppPosParam.sOtherSwitch[(i)/8] |= 0x01<<(7-(i)%8));\
	else\
		(gstAppPosParam.sOtherSwitch[(i)/8] &= ~(0x01<<(7-(i)%8)))

#define TieTieSwitch(swich, i)\
		if(swich[i] == YES)\
			(gstAppPosParam.sTieTieSwich[(i)/8] |= 0x01<<(7-(i)%8));\
		else\
			(gstAppPosParam.sTieTieSwich[(i)/8] &= ~(0x01<<(7-(i)%8)))
	


int SetTransTieTieSwitchValue(char *pszTransSwtch)
{

	/*������ȯ*/
	TieTieSwitch(pszTransSwtch, 0);
	/*�ֻ�����ȯ*/
	TieTieSwitch(pszTransSwtch, 1);
	/*���п���ȯ*/
	TieTieSwitch(pszTransSwtch, 2);
	/*����*/
	TieTieSwitch(pszTransSwtch, 3);
	/*�˻�*/
	TieTieSwitch(pszTransSwtch, 4);
	/*΢��ɨ��֧��*/
	TieTieSwitch(pszTransSwtch, 5);
	/*΢������֧��*/
	TieTieSwitch(pszTransSwtch, 6);
	/*΢���˻�*/
	TieTieSwitch(pszTransSwtch, 7);
	/*�ٶ�ɨ��֧��*/
	TieTieSwitch(pszTransSwtch, 8);
	/*�ٶ��˻�*/
	TieTieSwitch(pszTransSwtch, 9);
	/*����ɨ��*/
	TieTieSwitch(pszTransSwtch, 10);
	/*��������*/
	TieTieSwitch(pszTransSwtch, 11);
	/*�����˻�*/
	TieTieSwitch(pszTransSwtch, 12);
	/*֧��������*/
	TieTieSwitch(pszTransSwtch, 13);
	/*֧�����˻�*/
	TieTieSwitch(pszTransSwtch, 14);
	/*���Ŷ�ȯ*/
	TieTieSwitch(pszTransSwtch, 15);
	/*�̽�ͨ*/
	TieTieSwitch(pszTransSwtch, 16);
	/*����������ɨ*/
	TieTieSwitch(pszTransSwtch, 17);
	/*�������ݱ�ɨ*/
	TieTieSwitch(pszTransSwtch, 18);
	/*���������˻�*/
	TieTieSwitch(pszTransSwtch, 19);
	/*��֧����ɨ*/
	TieTieSwitch(pszTransSwtch, 20);
	/*��֧���˻�*/
	TieTieSwitch(pszTransSwtch, 21);
	/*����֧��ȷ��*/
	TieTieSwitch(pszTransSwtch, 22);
	/*�����ж�*/
	TieTieSwitch(pszTransSwtch, 23);
	/*֧������ɨ*/
	TieTieSwitch(pszTransSwtch, 24);
	/*�ٶȱ�ɨ*/
	TieTieSwitch(pszTransSwtch, 25);
	/*ͳһ��ɨ*/
	TieTieSwitch(pszTransSwtch, 26);
	/*ͳһ��ɨ*/
	TieTieSwitch(pszTransSwtch, 27);
	/*ͳһ�˻�*/
	TieTieSwitch(pszTransSwtch, 28);

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}



/**
* @brief ���������������Ϳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTieTieSwitch(void)
{
	int i, nRet;
	int nTransNum=10;		/**<Ϊ�ɿ��صĽ�����*/
	char cSelect;
	char szName[11][17]= {"������ȯ","�ֻ�����ȯ","���п���ȯ","����","�˻�",
	                      "΢��ɨ��֧��","΢������֧��","΢���˻�","�ٶ�Ǯ��","�ٶ��˻�"
	                     };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	for(i = 0; i < nTransNum; i++)
	{
		/**< �������Ϊ�մ������������ã���ά��ԭֵ*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sTieTieSwich[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("�������׿��ؿ���", szName[i], "0.��֧��|1.֧��", &cSelect);
		if (nRet == KEY_UP)
		{
			i = i - 2;
			if (i  < -1)
			{
				i = -1;
			}
			continue;
		}
		else if (nRet == APP_FAIL || nRet == APP_QUIT)
		{
			break;
		}
		if (cSelect == '1')
		{
			gstAppPosParam.sTieTieSwich[i/8] |= 0x01<<(7-i%8);
		}
		else
		{
			gstAppPosParam.sTieTieSwich[i/8] &= ~(0x01<<(7-i%8));
		}
	}

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

YESORNO GetTieTieSwitchOnoff(char cTransType)
{
	int nTransNum;
	
	switch(cTransType)
	{
	case TRANS_COUPON_VERIFY:			/**< ������ȯ*/
		nTransNum = 0;
		break;
	case TRANS_TEL_VERIFY:				/**< �ֻ�����ȯ*/
		nTransNum = 1;
		break;
	case TRANS_CARD_VERIFY:				/**< ���п���ȯ*/
		nTransNum = 2;
		break;
	case TRANS_VOID_VERIFY:				/**< ����*/
		nTransNum = 3;
		break;
	case TRANS_REFUND_VERIFY:			/**< �˻�*/
		nTransNum = 4;
		break;
	case TRANS_PRECREATE:				/**<ɨ��֧��*/
		nTransNum = 5;
		break;
	case TRANS_CREATEANDPAY:			/**<����֧��*/
		nTransNum = 6;
		break;
	case TRANS_WX_REFUND:				/**<΢���˻�*/
		nTransNum = 7;
		break;
	case TRANS_CREATEANDPAYBAIDU:		/**<�ٶ�֧��*/
		nTransNum = 8;
		break;
	case TRANS_BAIDU_REFUND:			/**<�ٶ��˻�*/
		nTransNum = 9;
		break;
	case TRANS_JD_PRECREATE:			/**<����֧��*/
		nTransNum = 10;
		break;							
	case TRANS_JD_CREATEANDPAY:			/**<��������*/
		nTransNum = 11;
		break;
	case TRANS_JD_REFUND:				/**<�����˻�*/
		nTransNum = 12;
		break;
	case TRANS_ALI_CREATEANDPAY:		/**<֧�������븶*/
		nTransNum = 13;
		break;
	case TRANS_ALI_REFUND:				/**<֧�����˻�*/
		nTransNum = 14;
		break;
	case TRANS_COUPON_MEITUAN:			/**<���Ŷ�ȯ*/
		nTransNum = 15;
		break;
	case TRANS_PANJINTONG:				/**<�̽�ͨ*/
		nTransNum = 16;
		break;
	case TRANS_DZ_CREATEANDPAY:			/**<������ɨ*/
		nTransNum = 17;
		break;
	case TRANS_DZ_PRECREATE:			/**<������ɨ*/
		nTransNum = 18;
		break;
	case TRANS_DZ_REFUND:				/**<���������˻�*/
		nTransNum = 19;
		break;
	case TRANS_BESTPAY_CREATEANDPAY:	/**<��֧����ɨ*/
		nTransNum = 20;
		break;
	case TRANS_BESTPAY_REFUND:			/**<��֧���˻�*/
		nTransNum = 21;
		break;
	case TRANS_COUPONFRM_DAZHONG:		/**<��������ȷ��*/
		nTransNum = 22;
		break;
	case TRANS_COUPON_DAZHONG:			/**<�����ж�*/
		nTransNum = 23;
		break;
	case TRANS_ALI_PRECREATE:			/**<֧������ɨ*/
		nTransNum = 24;
		break;
	case TRANS_BAIDU_PRECREATE:			/**<�ٶȱ�ɨ*/
		nTransNum = 25;
		break;
	case TRANS_ALLPAY_CREATEANDPAY:		/**<ͳһ��ɨ*/
		nTransNum = 26;
		break;
	case TRANS_ALLPAY_PRECREATE:		/**<ͳһ��ɨ*/
		nTransNum = 27;
		break;
	case TRANS_ALLPAY_REFUND:			/**<ͳһ�˻�*/
		nTransNum = 28;
		break;
	default:
		return NO;
		break;
	}
	if (gstAppPosParam.sTieTieSwich[nTransNum/8] & (0x80>>(nTransNum%8)))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

	
int SetTransSwitchValue(char *pszTransSwtch)
{
	/**<��������TMS��������˳��,��˳��΢��*/	
		
/*��һ���ֽ�*/
	
	/*<1,��ѯ,3*/
	TraditionSwitch(*pszTransSwtch, 0x80, 3);
	
	/*<2,Ԥ��Ȩ,4*/
	TraditionSwitch(*pszTransSwtch, 0x40, 4);	

	/*<3,Ԥ��Ȩ����, 5*/
	TraditionSwitch(*pszTransSwtch, 0x20, 5);

	/*<4,Ԥ��Ȩ�������, 6*/
	TraditionSwitch(*pszTransSwtch, 0x10, 6);

	/*<5,Ԥ��Ȩ��ɳ���, 8*/
	TraditionSwitch(*pszTransSwtch, 0x08, 8);

	/*<6,����, 0*/
	TraditionSwitch(*pszTransSwtch, 0x04, 0);

	/*<7,���ѳ���, 1*/
	TraditionSwitch(*pszTransSwtch, 0x02, 1);

	/*<8,�˻�, 2*/
	TraditionSwitch(*pszTransSwtch, 0x01, 2);

/*�ڶ����ֽ�*/
	
	/*<9,���߽���, 9*/
	TraditionSwitch(*(pszTransSwtch+1), 0x80, 9);

	/*<10,�������, 10*/
	TraditionSwitch(*(pszTransSwtch+1), 0x40, 10);

	/*<11,Ԥ��Ȩ���֪ͨ, 7*/
	TraditionSwitch(*(pszTransSwtch+1), 0x20, 7);

	/*<12,�ű����֪ͨ, */

	/*<13,�����ֽ��ѻ�����, */
	ECashSwitch(*(pszTransSwtch+1), 0x08, 0);
	ECashSwitch(*(pszTransSwtch+1), 0x08, 1);
	/*<14,, */

	/*<15,����Ǯ��Ȧ���ཻ��, */
	WalletSwitch(*(pszTransSwtch+1), 0x02,1);
	WalletSwitch(*(pszTransSwtch+1), 0x02,2);
	WalletSwitch(*(pszTransSwtch+1), 0x02,3);
	
	/*<16,���ڸ���, */
	InstallmentSwitch(*(pszTransSwtch+1), 0x01,0);
/*�������ֽ�*/	
	/*<17,���ڸ����, */
	InstallmentSwitch(*(pszTransSwtch+2), 0x80,1);
	/*<18,��������, */
	BonusSwitch(*(pszTransSwtch+2), 0x40,0);
	BonusSwitch(*(pszTransSwtch+2), 0x40,1);
	/*<19,�������ѳ���, */
	BonusSwitch(*(pszTransSwtch+2), 0x20,2);
	BonusSwitch(*(pszTransSwtch+2), 0x20,3);
	/*<20,Ȧ����, */
	ECashSwitch(*(pszTransSwtch+2), 0x10,2);
	ECashSwitch(*(pszTransSwtch+2), 0x10,3);
	ECashSwitch(*(pszTransSwtch+2), 0x10,4);
	ECashSwitch(*(pszTransSwtch+2), 0x10,5);
	/*<21,ԤԼ����, */
	AppointmentSwitch(*(pszTransSwtch+2), 0x08,0);
	/*<22,ԤԼ���ѳ���, */
	AppointmentSwitch(*(pszTransSwtch+2), 0x04,1);
	/*<23,��������, */
	OrderSwitch(*(pszTransSwtch+2), 0x02,0);
	/*<24,�������ѳ���, */
	OrderSwitch(*(pszTransSwtch+2), 0x01,1);
/*���ĸ��ֽ�*/	
	/*<25,��������ֵ, */
	OtherSwitch(*(pszTransSwtch+3), 0x80,0);
	OtherSwitch(*(pszTransSwtch+3), 0x80,1);

	return APP_SUCC;
}

/**
* @brief  ���ñ����Ƿ������ֶ��޸Ĳ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsModification(void)
{
	ASSERT_RETURNCODE( ProSelectYesOrNo("�������׿���", "�Ƿ������ֶ��޸�POS����", "0.������|1.����", &gstAppPosParam.cIsModify));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
YESORNO GetVarIsModification(void)
{
	if (gstAppPosParam.cIsModify== YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
int SetVarIsModification(YESORNO cVal)
{
	gstAppPosParam.cIsModify = cVal;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

int SetTmsTransSwitchValue(char *pszTransSwtch,STAPPPOSPARAM *pstAppPosParam)
{
	int i;
	int j,nTemp;
	j=0;
	/*
	��1λ ���ѵ�2λ ���ѳ�����3 λ �˻���4λ ����ѯ��5λ Ԥ��Ȩ
	��6λ Ԥ��Ȩ������7λ Ԥ��Ȩ��������8λ Ԥ��Ȩ���֪ͨ��9λ Ԥ��Ȩ��ɳ���
	��10λ ���߽����11λ �������*/
	for(i=0; i<11; i++) /*�����λ��Ϊ0��������ֵ*/
	{
		pstAppPosParam->sTraditionSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*��12λ �Ӵ�ʽ�����ֽ�����
	��13λ ����֧�����ǽӵ����ֽ����ѣ���14λ �����ֽ�ָ���˻�Ȧ��
	��15λ �����ֽ��ָ���˻�Ȧ���16λ �����ֽ��ֽ��ֵ
	��48λ �����ֽ��ֽ��ֵ����
	��49λ �����ֽ��ѻ��˻�
	*/

	j+=11;
	for(i=0; i<5; i++)
	{
		pstAppPosParam->sECashSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	nTemp=47;
	for(; i<5+2; i++)
	{
		pstAppPosParam->sECashSwitch[i/8]|=(((pszTransSwtch[(i-5+nTemp)/8]&(1<<(7-((i-5+nTemp)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��17λ ����Ǯ�����ѵ�18λ ����Ǯ��ָ���˻�Ȧ��
	��19λ ����Ǯ����ָ���˻�Ȧ���20λ ����Ǯ���ֽ��ֵ*/
	j+=5;
	for(i=0; i<4; i++)
	{
		pstAppPosParam->sWalletSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��21λ ���ڸ�������
	��22λ ���ڸ������ѳ���*/
	j+=4;
	for(i=0; i<2; i++)
	{
		pstAppPosParam->sInstallmentSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��23λ ���˻�������
	��24λ �����л�������
	��25λ ���˻������ѳ���
	��26λ �����л������ѳ���
	��50λ ���˻��ֲ�ѯ��51λ ���˻����˻�
	*/
	j+=2;
	for(i=0; i<4; i++)
	{
		pstAppPosParam->sBonusSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	nTemp=49;
	for(; i<4+2; i++)
	{
		pstAppPosParam->sBonusSwitch[i/8]|=(((pszTransSwtch[(i-4+nTemp)/8]&(1<<(7-((i-4+nTemp)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��27λ �ֻ�оƬ���ѵ�28λ �ֻ�оƬ���ѳ�����29λ �ֻ�оƬ�˻�
	��30λ �ֻ�оƬԤ��Ȩ��31λ �ֻ�оƬԤ��Ȩ������32λ �ֻ�оƬԤ��Ȩ�������
	��33λ �ֻ�оƬԤ��Ȩ���֪ͨ��34λ �ֻ�оƬԤ��Ȩ��ɳ�����35λ �ֻ�оƬ����ѯ
	*/
	j+=4;
	for(i=0; i<9; i++)
	{
		pstAppPosParam->sPhoneChipSaleSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}


	/*
	��36λ ԤԼ����
	��37λ ԤԼ����
	*/
	j+=9;
	for(i=0; i<2; i++)
	{
		pstAppPosParam->sAppointmentSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��38λ �������ѵ�39λ ��������
	��40λ �����˻���41λ ����Ԥ��Ȩ��42λ ����Ԥ��Ȩ����
	��43λ ����Ԥ��Ȩ��������44λ ����Ԥ��Ȩ��ɳ�����45λ ����Ԥ��Ȩ���֪ͨ
	*/
	j+=2;
	for(i=0; i<8; i++)
	{
		pstAppPosParam->sOrderSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	��46λ �������ֽ��ֵ��47λ �������˻���ֵ
	*/
	j+=8;
	for(i=0; i<6; i++)
	{
		pstAppPosParam->sOtherSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	return APP_SUCC;
}
/*�Ƿ��ӡ����*/
YESORNO GetVarIsPrintPrintMinus(void)
{
	return gstAppPosParam.cIsPrintMinus;
}
int SetFunctionIsPrintAllTrans(void)
{
	ASSERT_RETURNCODE( ProSelectYesOrNo("�������׿���", "��ӡ���н�����ϸ", "0��|1��", &gstAppPosParam.cIsPrintAllTrans));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
YESORNO GetVarIsPrintAllTrans(void)
{
	return gstAppPosParam.cIsPrintAllTrans;
}
/*ȡδ֪��������Ϣ*/
void GetVarUnknowBankInfo(char *pszOut)
{
	strcpy(pszOut,gstAppPosParam.szPrintUnknowBankInfo);
}

#ifndef USE_TMS
int SetFunctionIsPrintPrintMinus(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "�Ƿ��ӡ����", "0��|1��", &gstAppPosParam.cIsPrintMinus));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

int SetFunctionUnknowBankInfo(void)
{
	int nLen;
	int nRet;

	PubClearAll();
	PubDisplayTitle("��������������");
	PubDisplayStr(1, 2, 1, "%-11.11s", gstAppPosParam.szPrintUnknowBankInfo);
	PubDisplayStrInline(1, 4, "�Ƿ����? 1.��");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if ( nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("��������������", NULL, gstAppPosParam.szPrintUnknowBankInfo, &nLen, 0, 11, CHOOSE_IME_MODE_NUMPY));
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			return APP_SUCC;
		}
		else if (nRet == KEY_UP || nRet == KEY_DOWN)
		{
			return nRet;
		}
		else if(nRet == KEY_ESC)
		{
			return APP_QUIT;
		}
		else if(nRet == KEY_ENTER)
		{
			return APP_SUCC;
		}
	}
	return APP_SUCC;
}
#endif

void GetVarPinKey(char *pszPinKey,int nLen)
{
	memcpy(pszPinKey,gstAppPosParam.szEncryptPinKey,nLen>(sizeof(gstAppPosParam.szEncryptPinKey)-1)?(sizeof(gstAppPosParam.szEncryptPinKey)-1):nLen);
}
void GetVarTrKey(char *pszPinKey,int nLen)
{
	memcpy(pszPinKey,gstAppPosParam.szEncryptTrk,nLen>(sizeof(gstAppPosParam.szEncryptTrk)-1)?(sizeof(gstAppPosParam.szEncryptTrk)-1):nLen);
}
/*����pinkey����*/
int SetVarPinKey(char *pszPinKey,int nLen)
{
	memcpy(gstAppPosParam.szEncryptPinKey,pszPinKey,nLen>(sizeof(gstAppPosParam.szEncryptPinKey)-1)?(sizeof(gstAppPosParam.szEncryptPinKey)-1):nLen);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
/*����trk����*/
int SetVarTrKey(char *pszTrk,int nLen)
{
	memcpy(gstAppPosParam.szEncryptTrk,pszTrk,nLen>(sizeof(gstAppPosParam.szEncryptTrk)-1)?(sizeof(gstAppPosParam.szEncryptTrk)-1):nLen);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/*****��ȡTMS�·��Ĵ�ӡ�����Ϣ******/
int GetVarAdInfo(char* pszAdInfo)
{
	memcpy(pszAdInfo, gstAppPosParam.szAdInfo, strlen(gstAppPosParam.szAdInfo));
	return APP_SUCC;
}
#if defined(USE_TMS)
int IncSettleNum(void)
{
	gstAppPosParam.nSettleNum++;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

int ClearSettleNum(void)
{
	gstAppPosParam.nSettleNum = 0;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

int GetVarSettleNum(void)
{
	return gstAppPosParam.nSettleNum;
}
#endif

int SetFunctionReprintSettle()
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("�ش���㵥����", "�ش�ӡ���㵥", "0.��֧��|1.֧��", &gstAppPosParam.cIsReprintSettle));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

YESORNO GetVarReprintSettle(void)
{
	if (YES == gstAppPosParam.cIsReprintSettle)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

void GetAcqName(const char *pszBankCode,char *pszBankName, char cFlag)
{
	int i;
	for(i = 0; i<3; i++)
	{
		if((strlen(gstExtAppParam.szBankCode[i]) >=4) && 
			(memcmp(pszBankCode, gstExtAppParam.szBankCode[i], 4)== 0) && (strlen(gstExtAppParam.szBankName[i]) > 0))
		{
			strcpy(pszBankName, gstExtAppParam.szBankName[i]);
			return ;
		}
	}
	GetBankName(pszBankCode, pszBankName, cFlag);
	return ;
}


/**
* @fn InitUpdateVar
* @brief �汾����������ʼ������
* @param nUpdateVer �汾��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitUpdateVar(const int nUpdateVer)
{
	
	/**<POS����*/
//	memset(&gstAppPosParam,0,sizeof(gstAppPosParam));
//	ASSERT_FAIL(PubReadOneRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	switch(nUpdateVer)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}

//	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @fn VerUpdateChk
* @brief �Ƿ���а汾����(��Ա���)
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/

int VerUpdateChk()
{
	int nRet = 0;
	int i = 0;
	char szUpdateCtrl[fUpdateCtrl_len+1] = {0};
	nRet = PubGetVar(szUpdateCtrl, fUpdateCtrl_off, fUpdateCtrl_len);
	if (APP_SUCC != nRet)
	{
		memcpy(szUpdateCtrl, "00", fUpdateCtrl_len);
	}
	if (memcmp(szUpdateCtrl, UPDATE_CTRL, fUpdateCtrl_len) < 0) //�汾����
	{
		for (i = atoi(szUpdateCtrl); i < atoi(UPDATE_CTRL); i++)
		{
			InitUpdateVar(i);
		}

		PubSaveVar(UPDATE_CTRL, fUpdateCtrl_off, fUpdateCtrl_len);
	}
	return APP_SUCC;
}


/**�����ͽű�֪ͨ����*/
int SetVarHaveScriptAdviseNum(const char cHaveScriptAdviseNum)
{
	PubSaveVar(&cHaveScriptAdviseNum,fHaveScriptAdviseNum_off,fHaveScriptAdviseNum_len);
	return APP_SUCC;
}

int GetVarHaveScriptAdviseNum(char *pcHaveScriptAdviseNum)
{
	PubGetVar(pcHaveScriptAdviseNum,fHaveScriptAdviseNum_off,fHaveScriptAdviseNum_len);
	return APP_SUCC;
}

/*
*�Ƿ�����Ļ����ʾRFLOGO,
*��Ϊ��8080YS�Լ�IM81����,�ǽ���������Ļ�ϵ�,�Ͳ�����Ļ��ʾLOGO,�����󵼿ͻ�
**/
YESORNO GetVarShowRFLogo()
{
	char szPosType[60] = {0};
/*
	if (YES == GetVarIsExRF())
	{
		return NO;
	}
*/
	strcpy(szPosType, PubGetPosTypeStr());

	if (0 == memcmp(szPosType, "IM81", 4))
	{
		return NO;
	}

	return YES;
}
/**
* @brief ����Ƿ���qPboc����
* @param yesorno
* @return
* @li APP_SUCC
*/
int SetFlagQpboc(const YESORNO cFlag)
{
	gcIsQpboc = cFlag;
	return APP_SUCC;
}

YESORNO GetFlagQpboc(void)
{
	return gcIsQpboc;
}


/**
* @brief ��ʾ�ն˵İ汾��Ϣ��
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
* @author
* @date
*/
int SetVarFirstRunFlag(YESORNO cFlag)
{
	gcFirstRunFlag = cFlag;
	return APP_SUCC;
}

YESORNO GetVarFirstRunFlag(void)
{
	if (YES == gcFirstRunFlag)
		return YES;
	else
		return NO;
}

int GetShowName(char *pszInOutStr)
{
	memcpy(pszInOutStr, gstExtAppParam.szShowName, sizeof(gstExtAppParam.szShowName) - 1);
	return APP_SUCC;
}

int GetPrnLogo(char *pszInOutStr, uint *pInOutXpos)
{
	memcpy(pszInOutStr, gstExtAppParam.szPrnLogoName, sizeof(gstExtAppParam.szPrnLogoName));
	*pInOutXpos = gstExtAppParam.unPrnLogoXpos;
	return APP_SUCC;
}

/**
* @fn Version
* @brief ��ʾ�ն˵İ汾��Ϣ��
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int Version(void)
{
	int nRet;
	char szSoftVer[16]= {0};

	szSoftVer[0] = 'V';
	GetVarDispSoftVer(&szSoftVer[1]);

	PubClearAll();
#ifdef USE_TMS
	PubDisplayTitle("����汾��Ϣ(TMS)");
#else
	PubDisplayTitle("����汾��Ϣ");
#endif	
	PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 2, "��������汾��:");
	PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 3, "%s", szSoftVer);
	PubUpdateWindow();
	nRet = PubGetKeyCode(0);
	if (nRet == KEY_BACK)
	{
		PubClear2To4();
		PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 2, "%s", INTER_VER);
		PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 3, "%s", __DATE__);
		PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 4, "%s", __TIME__);
		PubUpdateWindow();
		nRet = PubGetKeyCode(0);
		if (nRet == KEY_BACK)
		{
			PubDisplayVersions();
		}
	}
	return APP_SUCC;
}



#define BACK_PARAM_INI "/appfs/apps/share/DNFPARABACK.ini"

//��ԭ������Ϣ
int ParamRestore(void)
{
	const char *szIniFile = BACK_PARAM_INI;
	int nIniHandle;
	int nValueLen;
	uint unFileLen = 0;
	char szBuf[64];
	STAPPCOMMPARAM stAppCommParam;
	ASSERT_FAIL(PubReadOneRec(FILE_APPCOMMPARAM,1,(char *)&stAppCommParam));
    NDK_FsFileSize(BACK_PARAM_INI, &unFileLen);
	ASSERT_QUIT(PubOpenFile (szIniFile, "r", &nIniHandle));

	PubClearAll();
	PubMsgDlg(NULL, "���ڽ��в�����ԭ", 0, 1);
	PubUpdateWindow();
    //001 �̻���
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "001", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szMerchantId, szBuf);
    
    //002 �ն˺�
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "002", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szTerminalId, szBuf);

    //003 ��ӡҳ��
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "003", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cPrintPageCount = szBuf[0];
    
    //004 ��Ƶ��֧��
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "004", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsSupportRF = szBuf[0];
    
    //005 ���������
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "005", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsExRF = szBuf[0];
    
   
    //006 ����ǩ����־
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "006", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsSupportElecSign = szBuf[0];
     
    //007 С������
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "007", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsTipFlag = szBuf[0];

	//008 С�ѱ���
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "008", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szTipRate, szBuf);
   
    //009 �Ƿ�֧��ɨ��ǹ
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "009", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsHaveScanner = szBuf[0];

	//010 ����������ɨ��ǹ
	memset(szBuf, 0, sizeof(szBuf));
	ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "010", &nValueLen, szBuf, unFileLen));
	gstAppPosParam.cIsOutsideScaner = szBuf[0];

	//011 Ӧ����ʾ����
	memset(szBuf, 0, sizeof(szBuf));
	ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "011", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szAppDispname, szBuf);

	//201 ������̱�־
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "201", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsPinPad = szBuf[0];
    
    //202 ���볬ʱ
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "202", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szPinPadTimeOut, szBuf);
    
    //203 ��Կ����
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "203", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szMainKeyNo, szBuf);
    
    //204 �������ͨѶ��
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "204", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cPinPadAuxNo = szBuf[0];
    
    //301 ���׳�ʱʱ��
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "301", &nValueLen, szBuf, unFileLen));
    stAppCommParam.cTimeOut = szBuf[0];
    
    //302 ����TPDU
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "302", &nValueLen, szBuf, unFileLen));
    if (nValueLen == 5)
    {
        memcpy(stAppCommParam.sTpdu, szBuf, nValueLen);
    }
    //303 ͨѶ����
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "303", &nValueLen, szBuf, unFileLen));
    stAppCommParam.cCommType = szBuf[0];
    switch(stAppCommParam.cCommType)
	{
	case COMM_DIAL://����
        //311 Ԥ������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "311", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szPreDial, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szPreDial, ""); 
        }
        //312 �绰����1
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "312", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szTelNum1, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szTelNum1, ""); 
        }
        //313 �绰����2
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "313", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szTelNum2, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szTelNum2, ""); 
        }      
        //314 �绰����3
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "314", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szTelNum3, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szTelNum3, ""); 
        }     
		break;
	case COMM_ETH://��̫��
        //321 ������IP��ַ
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "321", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //322 ����˿ں�
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "322", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1);
        //323 ����IP��ַ
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "323", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIpAddr, szBuf); 
        //324 ������������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "324", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szMask, szBuf); 
        //325 ���ص�ַ
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "325", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szGate, szBuf); 
   		break;
	case COMM_CDMA://CDMA
        //331 CDMA�������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "331", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szWirelessDialNum, szBuf); 
        //332 CDMA������IP��ַ
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "332", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //333 CDMA����˿�
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "333", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1); 
        //334 CDMA�û���
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "334", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szUser, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szUser, ""); 
        }
        //335 CDMA�û�����
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "335", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szPassWd, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szPassWd, ""); 
        }
        //336 CDMA��������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "336", &nValueLen, szBuf, unFileLen));
        stAppCommParam.cMode = szBuf[0]; 
		break;
	case COMM_GPRS://GPRS
        //341 GPRS�������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "341", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szWirelessDialNum, szBuf); 
        //342 GPRS������IP��ַ
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "342", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //343 GPRS����˿�
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "343", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1); 
        //344 GPRS APN
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "344", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szAPN1, szBuf); 
        strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
        //345 GPRS �û���
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "345", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szUser, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szUser, ""); 
        }
        //346 GPRS �û�����
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "346", &nValueLen, szBuf, unFileLen));
        if (nValueLen > 0)
        {
            strcpy(stAppCommParam.szPassWd, szBuf);
        }
        else
        {
            strcpy(stAppCommParam.szPassWd, ""); 
        }
        //347 GPRS��������
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "347", &nValueLen, szBuf, unFileLen));
        stAppCommParam.cMode = szBuf[0];
        break;
	default:
		break;
	}
	PubCloseFile(&nIniHandle);

	PubMsgDlg(NULL, "������ԭ���", 0, 1);
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	ASSERT_FAIL(SetAppCommParam(&stAppCommParam));
#if defined(USE_MAINCTRL)
	MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, strlen(stAppCommParam.szPreDial));
#endif	
	return APP_SUCC;
}

//���������Ϣ
int ParamSave(void)
{
	const char *szIniFile = BACK_PARAM_INI;
	int nIniHandle;
	STAPPCOMMPARAM stAppCommParam;

    PubDelFile(szIniFile);
	ASSERT_FAIL(PubReadOneRec(FILE_APPCOMMPARAM,1,(char *)&stAppCommParam));
	ASSERT_QUIT(PubOpenFile(szIniFile, "w", &nIniHandle));
    //001 �̻���
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "001", strlen(gstAppPosParam.szMerchantId), gstAppPosParam.szMerchantId));
    //002 �ն˺�
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "002", strlen(gstAppPosParam.szTerminalId), gstAppPosParam.szTerminalId));
    //003 ��ӡҳ��
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "003", 1, &gstAppPosParam.cPrintPageCount));
    //004 ��Ƶ��֧��
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "004", 1, &gstAppPosParam.cIsSupportRF));
    //005 ���������
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "005", 1, &gstAppPosParam.cIsExRF));
    //006 ����ǩ����־
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "006", 1, &gstAppPosParam.cIsSupportElecSign));   
    //007 С������
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "007", 1, &gstAppPosParam.cIsTipFlag));
    //008 С�ѱ���
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "008", strlen(gstAppPosParam.szTipRate), gstAppPosParam.szTipRate));    
    //009 �Ƿ�֧��ɨ��
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "009", 1, &gstAppPosParam.cIsHaveScanner));
    //010 ������ɨ��ǹ
	ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "010", 1, &gstAppPosParam.cIsOutsideScaner));
    //011 Ӧ����ʾ����
	ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "011", strlen(gstAppPosParam.szAppDispname), gstAppPosParam.szAppDispname));
	//201 ������̱�־
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "201", 1, &gstAppPosParam.cIsPinPad));
    //202 ���볬ʱ
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "202", strlen(gstAppPosParam.szPinPadTimeOut), gstAppPosParam.szPinPadTimeOut));
    //203 ��Կ����
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "203", strlen(gstAppPosParam.szMainKeyNo), gstAppPosParam.szMainKeyNo));
    //204 �������ͨѶ��
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "204", 1, &gstAppPosParam.cPinPadAuxNo));
    
    //301 ���׳�ʱʱ��
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "301", 1, &stAppCommParam.cTimeOut));
    //302 ����TPDU
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "302", 5, stAppCommParam.sTpdu));
    //303 ͨѶ����
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "303", 1, &stAppCommParam.cCommType));
    switch(stAppCommParam.cCommType)
	{
	case COMM_DIAL://����
        //311 Ԥ������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "311", strlen(stAppCommParam.szPreDial), stAppCommParam.szPreDial));
        //312 �绰����1
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "312", strlen(stAppCommParam.szTelNum1), stAppCommParam.szTelNum1));
        //313 �绰����2
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "313", strlen(stAppCommParam.szTelNum2), stAppCommParam.szTelNum2));
        //314 �绰����3
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "314", strlen(stAppCommParam.szTelNum3), stAppCommParam.szTelNum3));
		break;
	case COMM_ETH://��̫��
        //321 ������IP��ַ
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "321", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //322 ����˿ں�
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "322", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //323 ����IP��ַ
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "323", strlen(stAppCommParam.szIpAddr), stAppCommParam.szIpAddr));
        //324 ������������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "324", strlen(stAppCommParam.szMask), stAppCommParam.szMask));
        //325 ���ص�ַ
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "325", strlen(stAppCommParam.szGate), stAppCommParam.szGate));
		break;
	case COMM_CDMA://CDMA
        //331 CDMA�������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "331", strlen(stAppCommParam.szWirelessDialNum), stAppCommParam.szWirelessDialNum));
        //332 CDMA������IP��ַ
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "332", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //333 CDMA����˿�
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "333", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //334 CDMA�û���
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "334", strlen(stAppCommParam.szUser), stAppCommParam.szUser));
        //335 CDMA�û�����
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "335", strlen(stAppCommParam.szPassWd), stAppCommParam.szPassWd));
        //336 CDMA��������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "336", 1, &stAppCommParam.cMode));
		break;
	case COMM_GPRS://GPRS
        //341 GPRS�������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "341", strlen(stAppCommParam.szWirelessDialNum), stAppCommParam.szWirelessDialNum));
        //342 GPRS������IP��ַ
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "342", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //343 GPRS����˿�
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "343", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //344 GPRS APN
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "344", strlen(stAppCommParam.szAPN1), stAppCommParam.szAPN1));
        //345 GPRS �û���
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "345", strlen(stAppCommParam.szUser), stAppCommParam.szUser));
        //346 GPRS �û�����
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "346", strlen(stAppCommParam.szPassWd), stAppCommParam.szPassWd));
        //347 GPRS��������
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "347", 1, &stAppCommParam.cMode));
        break;
	default:
		break;
	}
	PubCloseFile(&nIniHandle);
	return APP_SUCC;
}

//TAG:3 + LEN:1 + VALUE:N
int ParamTagGet(int nFileHandle, char *psTag, int *pnLen, char *pszValue, int unFileLen)
{
    int i;
	int nRealLen;
    char nTagLen;
    char szTmp[8];
    char szValue[64];
  
    for(i = 0; i < unFileLen; )
    {
        memset(szTmp, 0, sizeof(szTmp));
        NDK_FsSeek(nFileHandle, (long)i, SEEK_SET);
    	nRealLen = NDK_FsRead(nFileHandle, szTmp, 4);
		if (nRealLen !=  4)
    	{
    		//PubMsgDlg(NULL, "���ļ�ʧ��", 3, 10);
    		return APP_FAIL;
    	}
        nTagLen = szTmp[3];
        if (memcmp(szTmp, psTag, 3) == 0)
        {
            if (nTagLen == 0)
            {
                *pnLen = nTagLen;
                return APP_SUCC;
            }
            nRealLen = NDK_FsRead(nFileHandle, szValue, nTagLen);
			if (nRealLen != nTagLen)
        	{
                //PubMsgDlg(NULL, "���ļ�ʧ��", 3, 10);
        		return APP_FAIL;
        	}
            memcpy(pszValue, szValue, nTagLen);
            *pnLen = nTagLen;
            return APP_SUCC;
        }
        i += 4 + nTagLen;
    }
	return APP_FAIL;
}

//TAG:3 + LEN:1 + VALUE:N
int ParamTagSave(int nFileHandle, char *psTag, int nLen, char *pszValue)
{
    char szBuf[64];

    memset(szBuf, 0, sizeof(szBuf));
    memcpy(szBuf, psTag, 3);
    szBuf[3] = nLen;
    memcpy(szBuf+4, pszValue, nLen);
    
    NDK_FsSeek(nFileHandle, 0, SEEK_END);
	if (NDK_FsWrite(nFileHandle, szBuf, nLen+4) < 0)
	{
		//PubMsgDlg(NULL, "д�ļ�ʧ��", 3, 10);
		return APP_FAIL;
	}
	return APP_SUCC;
}




/**
* @brief �Ӳ����ļ��������
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetEmvErrorCodeIni(int nEmvErrCode, char *pszErr)
{
	const char *szIniFile = FILE_EMVCODE_INI;
	int nIniHandle,nRet;
	int nLen;
	char szBuf[100];
	char szErrCode[12] = {0};

	if (NULL == pszErr || nEmvErrCode >= 0)
	{
		return APP_QUIT;
	}

	sprintf(szErrCode, "%d", nEmvErrCode);


	ASSERT_QUIT(PubOpenFile (szIniFile, "r", &nIniHandle));

	nRet = PubGetINIItemStr (nIniHandle, "BASE", szErrCode+1,  sizeof(szBuf) - 1, szBuf, &nLen);
	if (nRet == APP_SUCC)
	{
		sprintf(pszErr, "%s%s", szBuf, szErrCode);
	}
	else
	{
		sprintf(pszErr, "δ֪������%s", szErrCode);
	}
	PubCloseFile(&nIniHandle);
	return nRet;
}

#if defined (SUPPORT_ELECSIGN)
/**
* @brief ��ȡδ�ɹ�����ǩ���ظ����ʹ���
* @param out pnValue 
* @return ��
*/	
void GetVarElecSendTime(int *pnValue)
{	
	*pnValue = gstAppPosParam.cElecSendTime;
	return;
}
/**
* @brief ����δ�ɹ�����ǩ���ظ����ʹ���
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionElecSendTime(void)
{
	int nLen;
	int nNumber;
	char szNumber[1+1] = {0};

	nNumber = gstAppPosParam.cElecSendTime;
	sprintf(szNumber,  "%d", nNumber );
	ASSERT_RETURNCODE(PubInputDlg("����ǩ������", "ǩ���ظ����ʹ���:", szNumber, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	nNumber = atoi(szNumber);
	gstAppPosParam.cElecSendTime = nNumber;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief ���õ���ǩ����ʱʱ��
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncElecTimeOut(void)
{	
	int nLen = 0;

	ASSERT_RETURNCODE(PubInputDlg("����ǩ������", "���׳�ʱʱ��:", gstAppPosParam.szElecTimeOut, \
		&nLen, 1, 3, 60, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
} 

/**
* @brief ��ȡ����ǩ����ʱʱ��
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarElecTimeOut(char* pcValue)
{
	memcpy(pcValue, gstAppPosParam.szElecTimeOut, sizeof(gstAppPosParam.szElecTimeOut) - 1);
	return APP_SUCC;
}

/**
* @brief ������ӵ���ǩ�ְ�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncExElecBordType(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("����ǩ������", "���ǩ��������", "0.710|1.����", &gstAppPosParam.cExElecBordType));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));	
	return APP_SUCC;
}

/**
* @brief ��ȡ��ӵ���ǩ�ְ�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarExElecBordType(void)
{
	return gstAppPosParam.cExElecBordType;
}

/**
* @brief ����ǩ�����Ƿ���Ҫ����Աȷ��
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncExElecConfirm(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("����ǩ������", "ǩ�������Աȷ��", "0.����Ҫ|1.��Ҫ", &gstAppPosParam.cExElecConfirm));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));	
	return APP_SUCC;
}

/**
* @brief ��ȡ����Աȷ�ϱ�־
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarExElecConfirm(void)
{
	return gstAppPosParam.cExElecConfirm;
}

/**
* @brief ���õ���ǩ�����ͽ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/	
int SetVarElecSignSendResult(const YESORNO yesorno)
{
	gstAppPosParam.cElecSignSendResult = yesorno;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡ�Ƿ�֧����Ƶ��
* @return
* @li YES
* @li NO
*/	
YESORNO GetVarElecSignSendResult(void)
{	
	if (gstAppPosParam.cElecSignSendResult==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief �������һ�����͵���ǩ�ֵı��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetVarLastElecSignSendNum(const char *pszValue)
{  
	memset(gstAppPosParam.sLastElecSignSendNum, 0, sizeof(gstAppPosParam.sLastElecSignSendNum));
	memcpy(gstAppPosParam.sLastElecSignSendNum, pszValue, 3);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief ��ȡ���һ�����͵���ǩ�ֵı��
* @param out pszValue 3���ֽ�
* @return ��
*/	
void GetVarLastElecSignSendNum(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.sLastElecSignSendNum,sizeof(gstAppPosParam.sLastElecSignSendNum));
	return;
}


/**
* @brief ��ȡ����ǩ���ְ���־
* @return
* @li YES
* @li NO
*/	
YESORNO GetVarElecSubFlag(void)
{	
	if (gstAppPosParam.cIsElecSubcontractFlag==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

/**
* @brief ��ȡ����ǩ���ְ���С
* @param out
* @return ��
*/	
void GetVarElecSubSize(int * size)
{	
	char szTmp[10];

	memset(szTmp,0,sizeof(szTmp));
	memcpy(szTmp,gstAppPosParam.sElecSubcontractSize,sizeof(gstAppPosParam.sElecSubcontractSize)-1);
	if (atoi(szTmp)>0)
	{
		*size=atoi(szTmp);
	}
	else
	{
		*size=900;
	}
	return;
}


/**
* @brief ���õ���ǩ���ְ���־�Լ��ְ���С
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncElecSubcontract(void)
{	
	char szTemp[4+1];
	int nLen = 0;

	memset(szTemp, 0, sizeof(szTemp));

	ASSERT_RETURNCODE(ProSelectYesOrNo("����ǩ������", "�ְ����俪��", "0.�ر�|1.��", &gstAppPosParam.cIsElecSubcontractFlag));
	if(gstAppPosParam.cIsElecSubcontractFlag == YES)
	{
		ASSERT_FAIL(PubInputDlg("����ǩ������", "�ְ��ֽ���:", gstAppPosParam.sElecSubcontractSize, \
		&nLen, 1, 4, 60, INPUT_MODE_NUMBER));
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief����ǩ������δ���ͱ�������1
* @param in ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarElecSignUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	nNum++;
	PubSaveVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	return APP_SUCC;
}

/**
* @brief ����ǩ������δ���ͱ�����1
* @param in ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int DelVarElecSignUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	nNum--;
	if(nNum <0)
		nNum = 0;
	PubSaveVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	return APP_SUCC;
}


/**
* @brief ��ȡ���߽���δ���ͱ���
* @param in ��
* @return
* @li ���߽���δ���ͱ���
*/
int GetVarElecSignUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	return nNum;
}


/**
* @brief �������߽���δ���ͱ���
* @param in ��
* @return
* @li APP_SUCC
*/
int SetVarElecSignUnSendNum(int nNum)
{
	PubSaveVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	return APP_SUCC;
}


int SetFunctionSupportElecSign(void)
{
	int nWaterSum = 0;
	char cOldFlg = gstAppPosParam.cIsSupportElecSign;
	char *pszTitle = "����ǩ������";
	
	ASSERT_QUIT(ProSelectYesOrNo(pszTitle, "�Ƿ�֧�ֵ���ǩ��", "0.��֧��|1.֧��", &cOldFlg));
	if (cOldFlg != gstAppPosParam.cIsSupportElecSign)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg(pszTitle, "�н�����ˮ�Ƚ���", 3, 10);
			return APP_FAIL;
		}
	}
	
	gstAppPosParam.cIsSupportElecSign = cOldFlg;

	if (YES == gstAppPosParam.cIsSupportElecSign)
	{
		/*<ѡ�����û������*/

		/*<����*/
		if (1 == PubScrGetColorDepth())
		{
			PubMsgDlg(pszTitle, "�ڰ����ն˲�֧�ֵ���ǩ��", 0, 3);
			gstAppPosParam.cIsSupportElecSign = NO;
		}
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

YESORNO GetVarSupportElecSign(void)
{
	if (YES == gstAppPosParam.cIsSupportElecSign)
	{
		return YES;
	}
		
	return NO;
}

#endif

//ʱ�����Ч��
static int CheckIsValidTime(const char *pszTime)
{
	char szTmp[2+1];

	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, pszTime, 2);
	if(atoi(szTmp) >= 24)
	{
		return APP_FAIL;
	}
	
	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, pszTime+2, 2);
	if(atoi(szTmp) >= 60)
	{
		return APP_FAIL;
	}
	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, pszTime+4, 2);
	if(atoi(szTmp) >= 60)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
}

/**
* @brief ��������ʱ��
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPosDateTime(void)
{
	int  nLen = 0;
	char szDateTime[14+1]={0};
	char szDate[8+1]={0};
	char szTime[6+1]={0};

	memset(szDateTime, 0, sizeof(szDateTime));
	memset(szDate, 0, sizeof(szDate));
	PubGetCurrentDatetime(szDateTime);
	memcpy(szDate, szDateTime, 8);
	ASSERT_RETURNCODE(PubInputDate("��������", "��������(YYYYMMDD):", szDate, INPUT_DATE_MODE_YYYYMMDD, 60));
	
	if(PubIsValidDate(szDate) != APP_SUCC)
	{
		PubMsgDlg("��������", "��������ڸ�ʽ����", 3, 5);
		return APP_FAIL;
	}

	memset(szTime, 0, sizeof(szTime));
	memcpy(szTime, szDateTime+8, 6);
	ASSERT_RETURNCODE(PubInputDlg("����ʱ��", "������ʱ��(HHMMSS):", szTime, &nLen, 6, 6, 60, INPUT_MODE_NUMBER));

	if(CheckIsValidTime(szTime) != APP_SUCC)
	{
		PubMsgDlg("����ʱ��", "�����ʱ���ʽ����", 3, 5);
		return APP_FAIL;
	}
	
	PubSetPosDateTime(szDate, "YYYYMMDD", szTime);
	return APP_SUCC;

}

int SetFunctionRfSearchCardDelay(void)
{
	char *pszTitle = "�ǽ�Ѱ���ӳ�";
	int nLen = 0;

	if (YES == gstAppPosParam.cIsExRF)
	{
		return APP_FUNCQUIT;
	}

	ASSERT_RETURNCODE(ProSelectYesOrNo(pszTitle, "�ǽ�Ѱ���ӳ�", "0.����ʱ|1.��ʱ", &gstAppPosParam.cIsRfSearchCardDelay));
	if (YES == gstAppPosParam.cIsRfSearchCardDelay)
	{
		ASSERT_RETURNCODE(PubInputDlg(pszTitle, "Ѱ����ʱʱ��(Ms):", gstAppPosParam.szRfSearchDelayTimeMs, &nLen, 1, 3, 60, INPUT_MODE_NUMBER));

		PubSetReadRfDelay(atoi(gstAppPosParam.szRfSearchDelayTimeMs));
	}
	else
	{
		PubSetReadRfDelay(0);
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));	
	return APP_SUCC;

}

static char gcIsInitScanner = YES;

int SetVarIsInitScanner(const char cIsInitScanner)
{
	gcIsInitScanner = cIsInitScanner;
	return APP_SUCC;
}

char GetVarIsInitScanner(void)
{
	return gcIsInitScanner;
}

int SetFunctionIsOutsideScanner(void)
{
	char cOldOutsideScanerFlag = gstAppPosParam.cIsOutsideScaner;
	
	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "������ɨ��ǹ", "0.����|1.����", &gstAppPosParam.cIsOutsideScaner));
	if(NO == gstAppPosParam.cIsOutsideScaner)
	{
		if (APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_SCANNER, NULL))
		{
			PubMsgDlg("��ܰ��ʾ", "��֧������ɨ��ǹ", 3, 3);
		}
	}
	if(YES == cOldOutsideScanerFlag && cOldOutsideScanerFlag != gstAppPosParam.cIsOutsideScaner)
	{
		SetVarIsInitScanner(YES);
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

char GetVarIsOutsideScanner(void)
{
	if(YES == gstAppPosParam.cIsOutsideScaner)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

int SetFunctionIsHaveScanner(void)
{	
	ASSERT_RETURNCODE(ProSelectYesOrNo("ϵͳ��������", "�Ƿ�֧��ɨ��ǹ", "0.��|1.��", &gstAppPosParam.cIsHaveScanner));
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	if (gstAppPosParam.cIsHaveScanner == YES)
	{
		SetFunctionIsOutsideScanner();
	}
	return APP_SUCC;
}

char GetVarIsHaveScanner(void)
{
	if(YES == gstAppPosParam.cIsHaveScanner)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}


int ResetDefaultParam()
{
	/**
	* ����ȱʡ����
	*/
	
	PubLuaDisplay("LuaFirstRunChk");
	InitOper();
	InitCommParam();
	InitWaterFile();
	ClearSettle();						/**<�����������*/
	PubDeReclFile(SETTLEFILENAME);
	InitBlkFile();
	ASSERT_FAIL(InitPosDefaultParam());/*<��һ��Ҫ���������*/
	NDK_AppDel("����Ӧ��TTYY");
    SetParamFromIni();		
	ASSERT(ParamRestore()); 		   //wanglez add 20151228 �״����м�������ԭ
	return APP_SUCC;
}


