/**
* @file param.c
* @brief 参数管理模块
* @version  1.0
* @author 戴建斌、薛震、张捷
* @date 2007-02-05
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
//2015年9月17日修改APPCHECKVALUE  上美团
#define APPCHECKVALUE		APP_NAME"021"	/**<版本说明为7个字节中国银联，4位简写+1总公司版本+01版本号*/
#define FILE_APPPOSPARAM	APP_NAME"PARA"
#define FILE_EXTAPPPOSPARAM	APP_NAME"EXTPARA"
#define APPTYPE				"60"			/**<应用类型*/
#define APPVER				"31"			/**<应用版本号,2010年新规范版本*/
#define SECURITYPWD 		"888888"	/**<终端安全密码，不超过8个字节*/
#define HIDEOPERPWD			"20100322"
#define FILE_PARAM_INI 		APP_NAME"PARA.ini"
#define FILE_PARAM_EXTERN   APP_NAME"PARAEXT"
#define FILE_EMVCODE_INI	"../share/EMVERRCODE.INI"

/**
* @struct STREVERSALPARAM 冲正参数
*/
typedef struct
{
	char cIsReversal;		/**<冲正标志，'0'-已冲正，'1'-未冲正*/
	int nHaveReversalNum;	/**<已冲正次数*/
	int nReversalLen;		/**<冲正数据长度*/
	char sReversalData[fReversalData_len+1];				/**<冲正数据*/			
} STREVERSALPARAM;

/**
* @struct STSETTLEPARAM 结算参数
*/
typedef struct
{
	int	nWaterSum;						/**<流水笔数*/
	int nHaveReSendNum;					/**<离线交易已上送次数*/
	char cPrintSettleHalt;				/**<打印结算单中断*/
	char cPrintDetialHalt;				/**<打印明细中断*/
	char cPrintWaterHaltFlag;			/**<打印签购单中断*/
	char cCnCardFlag;					/**<内卡帐平标识*/
	char cEnCardFlag;					/**<外卡帐平标识*/
	char cBatchHaltFlag;				/**<批上送中断标识*/
	char cClrSettleDataFlag;			/**<清除结算数据中断标识*/
	int	nFinanceHaltFlag;				/**<金融类上送标识*/
	int	nMessageHaltFlag;				/**<通知类上送标识*/
	int	nBatchMagOfflinHaltFlag;		/**<磁条卡离线类上送标识*/
	int	nIcPurchaseHaltFlag;			/**<电子钱包消费上送标识*/
	int	nIcLoadHaltFlag;				/**<圈存类上送标识*/
	int	nBatchSum;						/**<批上送总笔数*/
	char sSettleDateTime[5];			/**<结算时间*/
	char sNumAmount[fSettleData_len+1];	/**<结算数据*/
} STSETTLEPARAM;


/**
* @struct STBANKPARAM 银行信息
*/
typedef struct
{
	char cPosMerType;					/**<POS类型*/
	char sRetCommHead[fRetCommHead_len+1];/**<处理要求*/
	char cIsLogin;						/**<签到标识*/
	char cIsLock;						/**<锁定标识*/
	char szTraceNo[fTraceNo_len+1];		/**<流水号*/
	char szBatchNo[fBatchNo_len+1];		/**<批次号*/
	char szInvoiceNo[fBatchNo_len+1];	/**<票据号*/
	ulong ulDialSuccSum;				/**<拨通次数*/
	ulong ulDialSum;					/**<拨号次数*/
	long lTimeCounter;					/**<时间计数器*/
	char sLastLoginDateTime[5];			/**<上回操作员登录时间*/
	char szLastLoginOper[OPERCODELEN+1];/**<上回登录操作员号码*/
	char cLastLoginOperRole;			/**<上回登录操作员属性*/
	int nMCTimes;						/**<主控进入时的时间因子*/
	int nMCRandom;						/**<主控进入时的随机数*/
	char cIsDownKek;					/**<是否需要下载KEK*/
} STBANKPARAM;

/**
* @struct STEMVPARAM EMV交易信息
*/
typedef struct
{
	int	nTransSerial;				/**<EMV交易计数器*/
	int	nOfflineUpNum;				/**<EMV脱机交易上送笔数*/
	int	nOnlineUpNum;				/**<EMV联机交易上送笔数*/
	int	nOfflineFailUpNum;			/**<EMV脱机失败交易上送笔数*/
	int	nOnlineARPCErrUpNum;		/**<EMV联机成功但ARPC错误交易上送笔数*/
	char cIsDownCAPKFlag;			/**<是否需要下载参数(EMV的下载公钥)*/
	char cIsDownAIDFlag;			/**<是否需要下载参数(EMV的ic卡参数)*/
	char cIsScriptInform;			/**<脚本处理通知标识，'0'-否，'1'-是*/
	char sScriptData[256];			/**<脚本处理数据*/
} STEMVPARAM;


/**
* @struct STEXTAPPPARAM 扩展应用参数
*/
typedef struct
{
	 char szBankCode[3][8+1];
	 char szBankName[3][20+1];
	 char szShowName[8+1];
	 char szPrnLogoName[50];
	 uint unPrnLogoXpos;
	char szReserve[600];				/**<保留域*/
}STEXTAPPPARAM;

 


/**
* 定义全局变量
*/
static STAPPPOSPARAM gstAppPosParam;	/**<POS参数文件*/
static STSETTLEPARAM gstSettleParam;	/**<结算数据*/
static STREVERSALPARAM gstReversalParam;/**<冲正参数*/
static STBANKPARAM gstBankParam;		/**<收单行参数*/
static STEMVPARAM gstEmvParam;			/**<EMV参数*/
static STEXTAPPPARAM gstExtAppParam;       /**<应用扩展参数*/


	
static char gcIsQpboc = NO;
static char gcFirstRunFlag = NO;     /*首次装机标识,用于处理【签到+下参数+下公钥】过程中不挂断**/
/**
* 声明内部函数
*/


/**
* 接口函数实现
*/

/**
* @brief 校验检测POS是否是第一次运行，首次运行需要初始化。
*		想初始化参数，可修改版本号，升级时POS自动初始化。
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
* @brief 设置POS首次运行信息，使用版本号控制
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
* @brief 初始参数默认值，程序第一次运行必须调用，或手工初始
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
	/**<清除FRAM数据区数据*/
	memset(szBuf,0,sizeof(szBuf));
	PubSaveVar(szBuf,fNVR_Addr_Start,fNVR_Addr_End-fNVR_Addr_Start);

	/**<POS参数*/
	memset(&stPosParamfile, 0, sizeof(STRECFILE));
	strcpy( stPosParamfile.szFileName, FILE_APPPOSPARAM);
	stPosParamfile.cIsIndex = FILE_NOCREATEINDEX;						/**< 创建索引文件*/
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
	strcpy(gstAppPosParam.szMerchantNameCn,"中国银联");
	strcpy(gstAppPosParam.szMerchantNameEn,"China Unionpay");
//#if defined(USE_TMS)	不管是TMS还是非TMS 都把终端号和商户号 写到主控，保证主控能打印出来
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
	strcpy(gstAppPosParam.szPinPadTimeOut,"60");	/**<默认超时120秒*/
	strcpy(gstAppPosParam.szSecurityPwd,SECURITYPWD);
	strcpy(gstAppPosParam.szMainKeyNo,"00");
	gstAppPosParam.cEncyptMode=DESMODE_3DES;/**<默认双倍长，即3des*/
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
	strcpy(gstAppPosParam.szMaxRefundAmount,"000000100000");		/**<1000元，以分为单位*/
	memcpy(gstAppPosParam.sTraditionSwitch,"\xFF\xFF\xFF\xFF",4);/*传统类*/
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
	/*20120229修改默认都支持非接*/
	gstAppPosParam.cIsSupportRF = YES;
	if(APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_RADIO_FREQUENCY, NULL))
	{
		gstAppPosParam.cIsExRF = NO;/**<内置*/
	}
#endif
	strcpy(gstAppPosParam.szHideMenuPwd,HIDEOPERPWD);
	gstAppPosParam.cIsPntChAcquirer = YES;
	gstAppPosParam.cIsPntChCardScheme = YES;
	gstAppPosParam.cPntTitleMode = YES;
	strcpy(gstAppPosParam.szPntTitleCn,"贴贴");
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
	/**<结算数据*/
	memset(&gstSettleParam,0,sizeof(gstSettleParam));
	ClearSettle();

	/**<冲正参数*/
	memset(&gstReversalParam,0,sizeof(gstReversalParam));
	SetVarIsReversal(NO);
	SetVarHaveReversalNum(0);
	SetVarReversalData(gstReversalParam.sReversalData,sizeof(gstReversalParam.sReversalData));

	/**<银行参数*/
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
	/**<EMV参数*/
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

	/**<新增磁道加密标识*/
	SetVarIsEncryptTrack(YES);

	/**<默认设置不支持小额代授权*/
	SetVarIsSmallGeneAuth(NO);

	/**<应用扩展参数*/
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
	strcpy(gstExtAppParam.szShowName, "都能付");
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
	/**<设置应用验证值*/
	SetAppCheckValue();
	return APP_SUCC;
}
/**
* @brief 从参数文件读入参数
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
	PubDisplayStrInline(0, 3, "正在进行参数设置");
	PubUpdateWindow();

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTID",  sizeof(gstAppPosParam.szMerchantId) - 1, gstAppPosParam.szMerchantId, &nValueLen), "取商户号失败");
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "POSID",  sizeof(gstAppPosParam.szTerminalId) - 1, gstAppPosParam.szTerminalId, &nValueLen), "取终端号失败");
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

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "TRACENO",  sizeof(gstBankParam.szTraceNo) - 1, gstBankParam.szTraceNo, &nValueLen), "取流水号失败");
	PubSaveVar(gstBankParam.szTraceNo,fTraceNo_off,fTraceNo_len);

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "BATCHNO",  sizeof(gstBankParam.szBatchNo) - 1, gstBankParam.szBatchNo, &nValueLen), "取批次号失败");
	PubSaveVar(gstBankParam.szBatchNo,fBatchNo_off,fBatchNo_len);

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "PRTCOUNT",  &nValue), "取打印页数失败");
	gstAppPosParam.cPrintPageCount = nValue + '0';
#if 0
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTNAME",  sizeof(gstAppPosParam.szMerchantNameCn) - 1, gstAppPosParam.szMerchantNameCn, &nValueLen), "取商户名称失败");
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCHANTNAMEEN", sizeof(gstAppPosParam.szMerchantNameEn) - 1, gstAppPosParam.szMerchantNameEn, &nValueLen), "取商户名英文名称失败");

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "REFUNDAMOUNT",  &nValue), "取退货最大金额失败");
	sprintf(gstAppPosParam.szMaxRefundAmount, "%012d", nValue);

	/*累计自动上送笔数-新更改2011-09-01*/
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "LOCALCODE",  sizeof(gstAppPosParam.szLocalCode) - 1, gstAppPosParam.szLocalCode, &nValueLen), "取地区代码失败");

	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "BASE", "MERCODE",  sizeof(gstAppPosParam.szBusinessBankId) - 1, gstAppPosParam.szBusinessBankId, &nValueLen), "取商行代码失败");
	//是否支持磁道加密
	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "BASE", "ENCPYTRACK", &nValue), "取是否使用磁道加密失败");
	if (nValue)
	{
		gstAppPosParam.cIsEncryptTrack = YES;
	}
	else
	{
		gstAppPosParam.cIsEncryptTrack = NO;
	}
#endif

	/*签购单抬头*/
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

	/*热线号码*/
	memset(szBuf,0,sizeof(szBuf));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "BASE", "HOTLINE", sizeof(gstAppPosParam.szHotLine)-1, szBuf, &nValueLen))
	{
		memset(gstAppPosParam.szHotLine,0,sizeof(gstAppPosParam.szHotLine));
		memcpy(gstAppPosParam.szHotLine,szBuf,nValueLen);
	}

	/*超级管理员密码*/
	memset(szBuf,0,sizeof(szBuf));
	if(APP_SUCC == PubGetINIItemStr(nIniHandle, "BASE", "SUPERPWD", 8, szBuf, &nValueLen))
	{
		if(nValueLen == 8)
		{
			ASSERT_PARA_FAIL(SetAdminPwd(szBuf),"设置超级管理员密码失败");
		}
		else
		{
			PubMsgDlg("参数设置", "超级管理员密码长度错", 3, 3);
		}
	}

	ASSERT_PARA_FAIL(PubGetINIItemInt(nIniHandle, "BASE", "REVERSALNUM",   &nValue), "取冲正次数失败");
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
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "PIN", "PINPAD", &nValue), " 取密码键盘标志失败");
		gstAppPosParam.cIsPinPad = (nValue ==1 ? YES : NO);
	}
	else
		gstAppPosParam.cIsPinPad=NO;

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "COMM", "TIMEOUT",  &nValue), "取交易超时时间失败");
	stAppCommParam.cTimeOut = nValue;
	if (PubGetINIItemInt (nIniHandle, "COMM", "PREDIAL",  &nValue) == APP_SUCC)
	{
		stAppCommParam.cPreDialFlag = nValue + '0';
	}
	ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "TPDU",  10, szBuf, &nValueLen), "取交易TPDU失败");
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
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNS",  sizeof(stAppCommParam.szDNSIp1) - 1, stAppCommParam.szDNSIp1, &nValueLen), "取域名服务器失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNNAME1",  sizeof(stAppCommParam.szDomain) - 1, stAppCommParam.szDomain, &nValueLen), "取域名1失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "COMM", "DNNAME2",  sizeof(stAppCommReserve.szDomain2) - 1, stAppCommReserve.szDomain2, &nValueLen), "取域名2失败");
	}

	ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "COMM", "COMMTYPE",  &nCommValue), "取通讯类型失败");
	//新需求不管什么通讯类型，都要把参数取出来
	if(nCommValue != 0)
	{
		//非拨号类型也要把拨号参数取出来
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "DIAL", "DIALNUM",  &nValue), " 取拨号次数失败");
		stAppCommParam.cReDialNum = nValue;

		nValueLen = 0;
		memset(stAppCommParam.szPreDial, 0, sizeof(stAppCommParam.szPreDial));
		PubGetINIItemStr (nIniHandle, "DIAL", "PREPHONE",  sizeof(stAppCommParam.szPreDial) - 1, stAppCommParam.szPreDial, &nValueLen);
		MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, (uint)nValueLen);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE1",  sizeof(stAppCommParam.szTelNum1) - 1, stAppCommParam.szTelNum1, &nValueLen), "取电话号码1失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE2",  sizeof(stAppCommParam.szTelNum2) - 1, stAppCommParam.szTelNum2, &nValueLen), "取电话号码2失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE3",  sizeof(stAppCommParam.szTelNum3) - 1, stAppCommParam.szTelNum3, &nValueLen), "取电话号码3失败");
	}
	if(nCommValue != 1)
	{
		//非网络类型也要把以太网独有参数取出来
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "取本地IP地址失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "取本地子网掩码失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "取网关地址失败");
	}
	if(nCommValue != 2 && nCommValue != 3)
	{
		//非无线类型也要取GPRS的参数
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "取GPRS用户名失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "取GPRS用户密码失败");

		//长短链接设置
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"GPRS", "MODE", &nValue), "取长短链接设置失败");
		stAppCommParam.cMode = nValue;
	}
	if(nCommValue != 3)
	{
		//非GPRS也要把GPRS独有参数取出来
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "APN", sizeof(stAppCommParam.szAPN1)-1, stAppCommParam.szAPN1, &nValue), "取GPRS APN失败");
		strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
	}
	if(nCommValue != 1 && nCommValue != 2 && nCommValue != 3)
	{
		//非IP端口类型取GPRS的参数
		//主机地址1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "取服务器IP地址1失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT1",  &nValue), "取服务端口号1失败");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//主机地址2（备用主机地址）
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "取服务器IP地址2失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT2",  &nValue), "取服务端口号2失败");
		sprintf(stAppCommParam.szPort2, "%d", nValue);
	}

	switch(nCommValue)
	{
	case 0:
		stAppCommParam.cCommType = COMM_DIAL;
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "DIAL", "DIALNUM",  &nValue), " 取拨号次数失败");
		stAppCommParam.cReDialNum = nValue;

		nValueLen = 0;
		memset(stAppCommParam.szPreDial, 0, sizeof(stAppCommParam.szPreDial));
		PubGetINIItemStr (nIniHandle, "DIAL", "PREPHONE",  sizeof(stAppCommParam.szPreDial) - 1, stAppCommParam.szPreDial, &nValueLen);
		MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, (uint)nValueLen);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE1",  sizeof(stAppCommParam.szTelNum1) - 1, stAppCommParam.szTelNum1, &nValueLen), "取电话号码1失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE2",  sizeof(stAppCommParam.szTelNum2) - 1, stAppCommParam.szTelNum2, &nValueLen), "取电话号码2失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "DIAL", "PHONE3",  sizeof(stAppCommParam.szTelNum3) - 1, stAppCommParam.szTelNum3, &nValueLen), "取电话号码3失败");
		break;
	case 1:
		stAppCommParam.cCommType =COMM_ETH;

		//主机地址1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "取服务器IP地址1失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "LINE", "PORT1",  &nValue), "取服务端口号1失败");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//主机地址2（备用主机地址）
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "取服务器IP地址2失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "LINE", "PORT2",  &nValue), "取服务端口号2失败");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "取本地IP地址失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "取本地子网掩码失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "LINE", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "取网关地址失败");
		break;
	case 2://CDMA
		stAppCommParam.cCommType =COMM_CDMA;
		//主机地址1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "取服务器IP地址1失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "CDMA", "PORT1",  &nValue), "取服务端口号1失败");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//主机地址2（备用主机地址）
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "取服务器IP地址2失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "CDMA", "PORT2",  &nValue), "取服务端口号2失败");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "取CDMA用户名失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "CDMA", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "取CDMA密码失败");

		//长短链接设置
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"CDMA", "MODE", &nValue), "取长短链接设置失败");
		stAppCommParam.cMode = nValue;
		break;
	case 3://GPRS
		stAppCommParam.cCommType =COMM_GPRS;
		//主机地址1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "取服务器IP地址1失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT1",  &nValue), "取服务端口号1失败");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//主机地址2（备用主机地址）
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "取服务器IP地址2失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "GPRS", "PORT2",  &nValue), "取服务端口号2失败");
		sprintf(stAppCommParam.szPort2, "%d", nValue);
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "APN", sizeof(stAppCommParam.szAPN1)-1, stAppCommParam.szAPN1, &nValue), "取GPRS APN失败");
		strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "USERNAME", sizeof(stAppCommParam.szUser)-1, stAppCommParam.szUser, &nValue), "取GPRS用户名失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "GPRS", "PWD", sizeof(stAppCommParam.szPassWd)-1, stAppCommParam.szPassWd, &nValue), "取GPRS用户密码失败");

		//长短链接设置
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle,"GPRS", "MODE", &nValue), "取长短链接设置失败");
		stAppCommParam.cMode = nValue;
		break;
	case 4:
		stAppCommParam.cCommType =COMM_RS232;
		break;
	case 5:
		stAppCommParam.cCommType= COMM_WIFI;
		//主机地址1
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SERVERIP1", sizeof(stAppCommParam.szIp1)-1, stAppCommParam.szIp1, &nValue), "取服务器IP地址1失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "PORT1",  &nValue), "取服务端口号1失败");
		sprintf(stAppCommParam.szPort1, "%d", nValue);

		//主机地址2（备用主机地址）
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SERVERIP2", sizeof(stAppCommParam.szIp2)-1, stAppCommParam.szIp2, &nValue), "取服务器IP地址2失败");
		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "PORT2",  &nValue), "取服务端口号2失败");
		sprintf(stAppCommParam.szPort2, "%d", nValue);

		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "LOCALIP", sizeof(stAppCommParam.szIpAddr)-1, stAppCommParam.szIpAddr, &nValue), "取本地IP地址失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "MASK", sizeof(stAppCommParam.szMask)-1, stAppCommParam.szMask, &nValue), "取本地子网掩码失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "GATEWAY", sizeof(stAppCommParam.szGate)-1, stAppCommParam.szGate, &nValue), "取网关地址失败");

		//ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "KEYTYPE",  &nValue), "取WIFI密码格式失败");
		//stAppCommReserve.cWifiKeyType = nValue;

		ASSERT_PARA_FAIL(PubGetINIItemInt (nIniHandle, "WIFI", "MODE",  &nValue), "取WIFI加密模式失败");
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
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "SSID", sizeof(stAppCommReserve.szWifiSsid)-1, stAppCommReserve.szWifiSsid, &nValue), "取WIFI热点名失败");
		ASSERT_PARA_FAIL(PubGetINIItemStr (nIniHandle, "WIFI", "PWD", sizeof(stAppCommReserve.szWifiKey)-1, stAppCommReserve.szWifiKey, &nValue), "取WIFI密码失败");
		break;
	default:
		stAppCommParam.cCommType =COMM_DIAL;
		break;
	}

	//以下功能仅供测试人员使用，方便他们导入主密钥。正式发布的*.INI文件不配置索引和主密钥
	{
		char szMasterKey[16+1] = {0};
		memset(gstAppPosParam.szMainKeyNo, 0, sizeof(gstAppPosParam.szMainKeyNo));
		PubGetINIItemStr(nIniHandle, "BASE", "KEYINDEX", sizeof(gstAppPosParam.szMainKeyNo) - 1, gstAppPosParam.szMainKeyNo, &nValueLen);
		nValue = atoi(gstAppPosParam.szMainKeyNo);

		if (APP_SUCC == PubGetINIItemStr (nIniHandle, "BASE", "MASTERKEY", 32, szBuf, &nValueLen))
		{
		/**<如果有下主密钥的时候，现在都一机一密了，不能都要求要下密钥*/
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
			else	/**<密钥错误*/
			{
				PubMsgDlg("参数", "密钥长度错误", 3, 5);
			}
		}
	}	

	/**<应用扩展参数*/
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

	sprintf(szBuf, "终端号:%8.8s\n商户号:\n%15.15s", gstAppPosParam.szTerminalId, gstAppPosParam.szMerchantId);
	PubMsgDlg("终端参数", szBuf, 0, 3);

	//sprintf(szBuf, "商户名称:\n%s", gstAppPosParam.szMerchantNameCn);

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
* @brief 导入POS参数,开机时导出一次即可,增加参数时，需在此相应增加，
*		与InitPosDefaultParam结合使用
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ExportPosParam()
{
	char szFramBuf[fNVR_Addr_End-fNVR_Addr_Start+1];


	/**<清除FRAM数据区数据*/
	memset(szFramBuf,0,sizeof(szFramBuf));
	PubGetVar(szFramBuf,fNVR_Addr_Start,fNVR_Addr_End-fNVR_Addr_Start);
	
	/**<POS参数*/
	memset(&gstAppPosParam,0,sizeof(gstAppPosParam));
	ASSERT_FAIL(PubReadOneRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));


	/**<结算数据*/
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

	/**<冲正参数*/
	memset(&gstReversalParam,0,sizeof(gstReversalParam));
	memcpy((char *)&gstReversalParam.cIsReversal,szFramBuf + fIsReversal_off,fIsReversal_len);
	memcpy((char *)&gstReversalParam.nHaveReversalNum,szFramBuf + fHaveReversalNum_off,fHaveReversalNum_len);
	memcpy((char *)&gstReversalParam.nReversalLen,szFramBuf + fReversalLen_off,fReversalLen_len);
	memcpy((char *)&gstReversalParam.sReversalData,szFramBuf + fReversalData_off,fReversalData_len);

	/**<银行参数*/
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

	/**<EMV参数*/
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

	/**<POS扩展参数*/
	memset(&gstExtAppParam,0,sizeof(gstExtAppParam));
	PubReadOneRec(FILE_EXTAPPPOSPARAM,1,(char *)&gstExtAppParam);

	return APP_SUCC;
}


/**
* @brief 获取POS商户应用类型
* @param out pnType 商户类型
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
* @brief 保存POS商户应用类型
* @param in nType 商户类型
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
* @brief 获取返回的通讯报文头信息
* @param out psCommHead 通讯报文头，定长6个字节
* @return
* @li APP_SUCC
*/
int GetVarCommHead(char *psCommHead)
{
	memcpy(psCommHead,gstBankParam.sRetCommHead,fRetCommHead_len);
	return APP_SUCC;
}

/**
* @brief 保存返回的通讯报文头信息
* @param in psCommHead 通讯报文头，定长6个字节
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
* @brief 获取签到标志
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
* @brief 保存签到标识
* @param in pFlag 签到标识，YES/NO
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
* @brief 获取锁定标识
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
* @brief 保存锁定标识
* @param in pFlag 锁定标识，YES/NO
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
* @brief 获取流水号
* @param out pszTraceNo 流水号，定长6位
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
* @brief 保存流水号
* @param in pszTraceNo 流水号，定长6位
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
* @brief 流水号加一并保存
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
* @brief 获取批次号
* @param out pszBatchNo 批次号，定长6位
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
* @brief 保存批次号
* @param in pszBatchNo 批次号，定长6位
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
* @brief 批次号加一并保存
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
* @brief 获取拨号成功次数
* @param out pulSum 成功拨号次数
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
* @brief 保存拨号成功次数
* @param in ulSum 成功拨号次数
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
* @brief拨号成功次数加一并保存
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
* @brief 获取拨号次数
* @param out pulSum 拨号次数
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
* @brief 保存拨号次数
* @param in ulSum 拨号次数
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
* @brief拨号次数加一并保存
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
* @brief 获取时间计数器，计算拨通率时使用
* @param in plTime 时间长整形表示法
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
* @brief 保存时间计数器
* @param in lTime 时间长整形表示法
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
* @brief 获得上回操作员登录时间
* @param in psLastLoginDateTime 上回操作员登录时间
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
* @brief 保存上回操作员登录时间
* @param in psLastLoginDateTime 上回操作员登录时间
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
* 冲正参数部分
*/

/**
* @brief 获取冲正标志
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
* @brief 保存冲正标志
* @param in pFlag 标志，值为YES/NO
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
* @brief 获取已冲正次数
* @param out pnNum 已冲正次数
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
* @brief 保存已冲正次数
* @param in nNum 已冲正次数
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
* @brief 已冲正次数加一并保存
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
* @brief 获取冲正数据和长度
* @param out psData 最大长度512位
* @param out pnLen 冲正数据长度
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
* @brief 保存冲正数据和长度
* @param in sData 最大长度512位
* @param in nLen 冲正数据长度
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
* POS参数
*/

/**
* @brief 获取终端号
* @param out pszValue 最大长度8位
* @return 无
*/
void GetVarTerminalId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szTerminalId,sizeof(gstAppPosParam.szTerminalId)-1);
	return;
}

/**
* @brief 获取终端号
* @param out pszValue 最大长度8位
* @return 无
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
* @brief 获取商户号，
* @param out pszValue 最大长度15位
* @return 无
*/
void GetVarMerchantId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantId,sizeof(gstAppPosParam.szMerchantId)-1);
	return;
}

/**
* @brief 获取商户号，
* @param out pszValue 最大长度15位
* @return 无
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
* @brief 获取中文商户名
* @param out pszValue 最长40个字节
* @return 无
*/
void GetVarMerchantNameCn(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantNameCn,sizeof(gstAppPosParam.szMerchantNameCn)-1);
	return;
}

/**
* @brief 设置商户中文名称
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
* @brief 获取英文商户名
* @param out pszValue 最长40个字节
* @return 无
*/
void GetVarMerchantNameEn(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szMerchantNameEn,sizeof(gstAppPosParam.szMerchantNameEn)-1);
	return;
}

/**
* @brief 获取应用类型，定长2个字节
* @param out pszValue 定长2个字节
* @return 无
*/
void GetVarAppType(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szAppType,sizeof(gstAppPosParam.szAppType)-1);
	return;
}

/**
* @brief 设置应用类型，定长2个字节
* @param int pszValue 定长2个字节
* @return 无
*/
int SetVarAppType(const char *pszValue)
{
	memcpy(gstAppPosParam.szAppType, pszValue, sizeof(gstAppPosParam.szAppType)-1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 获取应用版本，
* @param out pszValue 定长2个字节
* @return 无
*/
void GetVarAppVer(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szAppVer,sizeof(gstAppPosParam.szAppVer)-1);
	return;
}

/**
* @brief
* @return
* @li POS_STATE_TEST 测试状态
* @li POS_STATE_NORMAL 正常交易状态
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
* @brief 获取商行代码，定长4位
* @param out pszValue 定长4位
* @return 无
*/
void GetVarBusinessBankId(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szBusinessBankId,sizeof(gstAppPosParam.szBusinessBankId)-1);
	return;
}

/**
* @brief 获取本地区区码
* @param out pszValue 定长4位
* @return 无
*/
void GetVarLocalcode(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szLocalCode,sizeof(gstAppPosParam.szLocalCode)-1);
	return;
}

/**
* @brief 获取密码键盘标识
* @return
* @li YES 接
* @li NO 不接
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
* @brief 获取密码键盘接的串口号
* @param out pnValue 串口号，1口AUX1,2口AUX2
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarPinpadAuxNo(int *pnValue)
{
	//统一平台都是PORT_NUM_COM2
	*pnValue = PORT_NUM_COM2;
	return APP_SUCC;
}

/**
* @brief 获取密码键盘超时时间
* @param out pnValue 密码键盘超时时间，0-999
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
* @brief 获取终端安全密码
* @param out pnValue 安全密码值，最长支持8个字节
* @return 无
*/
void GetVarSecurityPwd(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szSecurityPwd,sizeof(gstAppPosParam.szSecurityPwd)-1);
	return;
}

/**
* @brief 获取终端主密钥
* @param out pnValue 主密钥序号
* @return 无
*/
void GetVarMainKeyNo(int *pnValue)
{
	char szTmp[10] = {0};

	memcpy(szTmp,gstAppPosParam.szMainKeyNo,sizeof(gstAppPosParam.szMainKeyNo)-1);
	*pnValue=atoi(szTmp);
	return;
}

/**
* @brief 设置启用主密钥序号
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
* @brief 加密方式，DES或者3des
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
* @brief pin密码加密方式，加密时主账号是否参与运算
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
* @brief 获取磁道加密标识
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
* @brief 设置磁道加密标识
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
* @brief 获取是否支持小额代授权标识
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
* @brief 设置是否支持小额代授权
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
* @brief 获取自动签退标识
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
* @brief 设置自动签退标识
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
* @brief 获取是否显示emv交易的tvr tsi
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
* @brief 获取是否屏蔽卡号
* @return
* @li YES
* @li NO
*/
YESORNO GetVarIsShieldPan(void)
{
	if (gstAppPosParam.cIsShieldPan != YES  && gstAppPosParam.cIsShieldPan != NO)
	{
		/**<特殊处理，针对旧版升级stAppPosParam.cIsShieldPan的原始赋值*/
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
* @brief 获取是否打印流水明细标识
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
* @brief 获取是否是新票据
* @return
* @li YES
* @li NO
*/
int GetVarIsNewTicket()
{
	return gstAppPosParam.cIsNewTicket;
}

/**
* @brief 获取票据是否带英文标识
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
* @brief 获取预授权是否屏蔽卡号
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
* @brief 获取是否打印故障报告单
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
* @brief 获取打印几联
* @param out pnValue 打印几联
* @return 无
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
* @brief 获取POS存储的交易笔数，达到此数时，要求结算
* @param out pnValue 1-99999
* @return 无
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
* @brief 获取POS可支持的最大退货金额
* @param out pulValue 无符号长整形
* @return 无
*/
/** 不需要进行类型转换，直接传出12位的字符串即可*/
void GetVarMaxRefundAmount(char *szMaxRefundAmt)
{
	memcpy(szMaxRefundAmt, gstAppPosParam.szMaxRefundAmount,sizeof(gstAppPosParam.szMaxRefundAmount)-1);
	return;
}

/**
* @brief 获取小费比率
* @param out psValue 2个字节的小费比率
* @return 无
*/
void GetVarTipRate(char *psValue)
{
	memcpy(psValue,gstAppPosParam.szTipRate,2);
	return;
}

/**
* @brief 获取小费比率
* @param out psValue 2个字节的小费比率
* @return
* @li APP_SUCC 设置成功
* @li APP_FAIL设置失败
*/
int SetVarTipRate(const char *psValue)
{
	memcpy(gstAppPosParam.szTipRate, psValue, 2);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
/**
* @brief 获取是否支持小费交易
* @return
* @li YES 支持
* @li NO 不支持
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
* @brief 设置是否支持小费交易
* @param in pFlag是否支持小费交易，YES/NO
* @return
* @li APP_SUCC 设置成功
* @li APP_FAIL设置失败
*/
int SetVarIsTipFlag(const YESORNO pFlag)
{
	gstAppPosParam.cIsTipFlag = pFlag;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief 获取IC开关，是否支持IC卡交易
* @return
* @li YES 支持
* @li NO 不支持
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
* @brief 获取IC卡确认参数以确定是否提示操作员确认卡片是否为IC卡
* @return
* @li YES 支持
* @li NO 不支持
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
* @brief 获取是否支持手工输入卡号
* @return
* @li YES 支持
* @li NO 不支持
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
* @brief 设置是否支持手工输入卡号
* @param in pFlag是否支持手工输入卡号，YES/NO
* @return
* @li APP_SUCC 设置成功
* @li APP_FAIL设置失败
*/
int SetVarIsCardInput(const YESORNO cFlag)
{
	gstAppPosParam.cIsCardInput = cFlag;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 获取消费撤消是否需要刷卡
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取预授权完成撤消是否需要刷卡
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取撤消类是否需要输入密码
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取预授权完成撤消是否需要输入密码
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取预授权撤消是否需要输入密码
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取预授权完成请求是否需要输入密码
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 获取默认交易
* @param out pcValue '0'-消费，'1'-预授权
* @return 无
*/
void GetVarDefaultTransType(char *pcValue)
{
	gstAppPosParam.cDefaultTransType = gstAppPosParam.cDefaultTransType == '0' ? '0' : '1';
	*pcValue=gstAppPosParam.cDefaultTransType;
	return;
}

/**
* @brief 获取所支持预授权完成的方式
* @param out pcValue '0'-同时支持，'1'-支持请求，'2'-支持通知
* @return 无
*/
void GetVarAuthSaleMode(char *pcValue)
{
	*pcValue=gstAppPosParam.cAuthSaleMode;
	return;
}

/**
* @brief 获取保留字段1数据
* @param out pszValue
* @return 无
*/
void GetVarReserve1(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szReserve1,sizeof(gstAppPosParam.szReserve1)-1);
	return;
}

/**
* @brief 获取保留字段2数据
* @param out pszValue
* @return 无
*/
void GetVarReserve2(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.szReserve2,sizeof(gstAppPosParam.szReserve2)-1);
	return;
}

/**
* EMV部分
*/

/**
* @brief 获取EMV交易序号
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
* @brief 保存emv交易序号
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
* @brief emv交易序号加一并保存
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int IncVarEmvTransSerial()
{
	gstEmvParam.nTransSerial = ++gstEmvParam.nTransSerial%99999999;
#if 0 //此处保存到掉电保护区比较费时,放外面处理(SaveEmvTransSerial),不影响qPboc时间
	SetVarEmvTransSerial(gstEmvParam.nTransSerial);
#endif
	return APP_SUCC;
}

/**
* @brief emv保存交易序号
* @return
* @li APP_SUCC
*/
int SaveEmvTransSerial(void)
{
	SetVarEmvTransSerial(gstEmvParam.nTransSerial);
	return APP_SUCC;
}



/**
* @brief 获取emv脱机交易上送笔数
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
* @brief 保存emv脱机交易上送笔数
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
* @brief emv脱机交易上送笔数加一并保存
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
* @brief 获取emv联机交易上送笔数
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
* @brief 保存emv脱机交易上送笔数
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
* @brief emv联机交易上送笔数加一并保存
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
* @brief 保存EMV脱机消费失败上送笔数
* @param in nOfflineFailUpNum 第几笔
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
* @brief 获得EMV脱机消费失败上送笔数
* @param in pnOfflineFailUpNum 上送笔数标识
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
* @brief 保存EMV APRC错但卡片仍然承对的上送笔数
* @param in nOnlineARPCErrUpNum 第几笔
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
* @brief 获得EMV APRC错但卡片仍然承对的上送笔数
* @param in pnOnlineARPCErrUpNum 上送笔数
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
* @brief 获取下载参数(EMV的下载公钥和ic卡参数)是否成功
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
* @brief 保存下载参数(EMV的下载公钥和ic卡参数)是否成功
* @param in pFlag 下载标识，YES/NO
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
* @brief 获取下载参数(EMV的下载公钥和ic卡参数)是否成功
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
* @brief 保存下载参数(EMV的下载公钥和ic卡参数)是否成功
* @param in pFlag 下载标识，YES/NO
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
* @brief 获取脚本通知标识
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
* @brief 保存脚本通知标识
* @param in pFlag 脚本通知，YES/NO
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
* @brief 获取脚本通知数据
* @param in nLen 长度，1-256
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
* @brief 保存脚本通知数据
* @param in nLen 长度，1-256
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
* @brief 设置启用主密钥序号
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
	ASSERT_QUIT(PubInputDlg("交易密钥索引号", "密钥索引号(0-9):", szNumber, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	sprintf(gstAppPosParam.szMainKeyNo, "0%s", szNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	Ums_SetMainKeyFromMC();
	return APP_SUCC;
}

/**
* @brief 设置退货上限金额
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
	ASSERT_RETURNCODE(PubInputAmount("其他交易控制", "退货最大金额:", gstAppPosParam.szMaxRefundAmount, &nLen, INPUT_AMOUNT_MODE_NOT_NONE, INPUT_AMOUNT_TIMEOUT));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置预授权完成方式
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
	ASSERT_RETURNCODE(PubSelectListItem("0.同时支持||1.支持请求模式||2.支持通知模式", "授权通知模式", NULL, nSelect, &nSelect));

	gstAppPosParam.cAuthSaleMode = nSelect + '0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置打印联数,热敏打印机需要设置
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
		PubMsgDlg("温馨提示","针打不支持此项设置", 0, 2);
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}


	while(1)
	{
		szNum[0] = gstAppPosParam.cPrintPageCount;
		ASSERT_RETURNCODE( PubInputDlg("系统参数设置", "设置热敏打印联数\n(    <=3联):", szNum, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
		nNum=atoi(szNum);
		if( (nNum >= 1) && (nNum <= 3) )
		{
			break;
		}
		PubMsgDlg("系统参数设置","输入有误,请重输!",1,1);
	}
	gstAppPosParam.cPrintPageCount = '0' + atoi(szNum);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置允许保存的最大交易笔数
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
		ASSERT_RETURNCODE(PubInputDlg("系统参数设置", "保存最大交易笔数\n(   <=500笔):", szNumber, &nLen, 1, 3, 0, INPUT_MODE_NUMBER));
		nNumber = atoi(szNumber);
		if ( (nNumber>0 )&&(nNumber <=500) )
		{
			break;
		}
		PubMsgDlg("系统参数设置","输入有误,请重输!",0,1);
	}
	sprintf(gstAppPosParam.szMaxTransCount, "%05d", nNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置商户号
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
	sprintf(szContent, "%s\n商户号:(15位)", szMerchantId);
	ASSERT_RETURNCODE(PubInputDlg("商户参数设置", szContent, szMerchantId, &nLen, 15, 15, 0, INPUT_MODE_STRING));
	if (memcmp(szMerchantId, gstAppPosParam.szMerchantId, 15) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("商户参数设置", "有交易流水先结算", 3, 10);
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
* @brief 设置POS终端号
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
	sprintf(szContent, "%s\nPOS号:(8位)", szTerminalId);
	ASSERT_RETURNCODE( PubInputDlg("商户参数设置", szContent, szTerminalId, &nLen, 8, 8, 0, INPUT_MODE_STRING));
	if (memcmp(szTerminalId, gstAppPosParam.szTerminalId, 8) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("商户参数设置", "有交易流水先结算", 3, 10);
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
* @brief 设置应用显示名称
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
	PubDisplayTitle("应用名称设置");
	PubDisplayStr(1, 2, 1, "%s", gstAppPosParam.szAppDispname);
	PubDisplayStrInline(1, 5, "是否改名? 1.是");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if ( nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("应用名称设置", NULL, gstAppPosParam.szAppDispname, &nLen, 0, 14, CHOOSE_IME_MODE_NUMPY));
			ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
			/*非tms版本，此处也要加上*/
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
* @brief 设置商户名称
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}
	PubClearAll();
	PubDisplayTitle("商户中文名设置");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szMerchantNameCn);		
	PubDisplayStrInline(1, 5, "是否改名? 1.是");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10) ;
		if (nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("商户中文名设置", NULL, gstAppPosParam.szMerchantNameCn, &nLen, 0, 40, CHOOSE_IME_MODE_NUMPY));
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
* @brief 设置商户英文名称
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}
	PubClearAll();
	PubDisplayTitle("商户英文名设置");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szMerchantNameEn);	
	PubDisplayStrInline(1, 5, "是否改名? 1.是");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if (nRet == KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("商户英文名设置", NULL, gstAppPosParam.szMerchantNameEn, &nLen, 0, 20, IME_ENGLISH));
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
* @brief 设置POS当前的年份
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

	ASSERT_RETURNCODE(PubInputDlg("终端参数设置", "请输入当前年份:", szYear, &nYear, 4, 4, 0, INPUT_MODE_NUMBER));
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
* @brief  设置POS流水号
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}
	memcpy(szTraceNo, gstBankParam.szTraceNo, fTraceNo_len);
	ASSERT_RETURNCODE(PubInputDlg("系统参数设置", "当前流水号:", szTraceNo, &nLen, fTraceNo_len, fTraceNo_len, 0, INPUT_MODE_NUMBER));
	if (memcmp(gstBankParam.szTraceNo, szTraceNo, fTraceNo_len) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("终端参数设置", "有交易流水先结算", 3, 10);
			return APP_FAIL;
		}
		memcpy(gstBankParam.szTraceNo, szTraceNo, fTraceNo_len);
		PubSaveVar(gstBankParam.szTraceNo,fTraceNo_off,fTraceNo_len);
	}
	return APP_SUCC;
}

/**
* @brief  设置POS批次号
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
	ASSERT_RETURNCODE(PubInputDlg("系统参数设置", "当前批次号:", szBatchNo, &nLen, fBatchNo_len, fBatchNo_len, 0, INPUT_MODE_NUMBER));

	if (memcmp(gstBankParam.szBatchNo, szBatchNo, fBatchNo_len) !=0)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg("终端参数设置", "有交易流水先结算", 3, 10);
			return APP_FAIL;
		}
		memcpy(gstBankParam.szBatchNo, szBatchNo, fBatchNo_len);
		PubSaveVar(gstBankParam.szBatchNo,fBatchNo_off,fBatchNo_len);
	}

	return APP_SUCC;
}

/**
* @brief  设置商行代码
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionBusinessBankId(void)
{
	int nLen;

	ASSERT_RETURNCODE( PubInputDlg("终端参数设置", "商行代码:", gstAppPosParam.szBusinessBankId, &nLen, 4, 4, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  设置本地地区码
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionLocalCode(void)
{
	int nLen;

	ASSERT_RETURNCODE(PubInputDlg("终端参数设置", "输入本地地区码:", gstAppPosParam.szLocalCode, &nLen, 4, 4, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  设置签购单保管年限
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
	ASSERT_RETURNCODE( PubInputDlg("系统参数设置", "设置签购单保管年限", szNum, &nLen, 0, 1, 0, INPUT_MODE_NUMBER));
	nNum=atoi(szNum);
	gstAppPosParam.cSafeKeepYear = '0' + atoi(szNum);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

static char gcIsChkPinpad = YES;/**<用于标识是否需要检测密码键盘*/
static char gcIsChkInside = YES;/**<用于标识内置密码键盘是否初始化了>*/
static char gcIsChkRF = YES; /**<用于标识是否需要初始化RF模块>*/

/*
* @brief 控制是否需要检测外接密码键盘
* @param in cIsChk 是否检测标识，YES/NO
* @return  无
*/
void SetControlChkPinpad(const YESORNO cIsChk)
{
	gcIsChkPinpad = cIsChk;
}
/**
* @brief 控制是否需要初始化内置密码键盘
* @param in cIsChk 是否检测标识，YES/NO
* @return  无
*/
void SetControlChkInside(const YESORNO cIsChk)
{
	gcIsChkInside = cIsChk;
}
/**
* @brief 控制是否需要检测外接密码键盘
* @param in cIsChk 是否检测标识，YES/NO
* @return  无
*/
void SetControlChkRF(const YESORNO cIsChk)
{
	gcIsChkRF = cIsChk;
}

/**
* @brief 获得是否需要检测外接密码键盘
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
* @brief 获得是否需要初始化内置密码键盘
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
* @brief 获得是否需要检测外接密码键盘
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
* @brief 设置是否外接密码键盘的标志
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPinPad(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_PINPAD, NULL))
	{
		ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "内外置密码键盘", "0.内置|1.外置", &gstAppPosParam.cIsPinPad));
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
		ChkPinpad();
		return APP_SUCC;
	}
	if (gstAppPosParam.cIsPinPad == YES)
	{
		return SetFunctionPinpadTimeOut();
	}
	return APP_FUNCQUIT;//返回此值可以保证向上翻页
}

/**
* @brief 设置离线上送方式
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("离线交易控制", "离线上送方式", "0.批结算前上送||1.下笔联机上送", &gstAppPosParam.cIsOfflineSendNow));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置是否支持IC卡交易标志
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsIcFlag(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("其他参数设置", "IC卡交易支持", "0.不支持|1.支持", &gstAppPosParam.cIsIcFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置是否提示确认IC卡标志
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsIcConfirmFlag(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("其他参数设置", "IC卡交易确认提示", "0.不提示|1.提示", &gstAppPosParam.cIsIcConfirmFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置手输入卡号开关
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("其他交易控制", "手工输卡号", "0.关闭|1.打开", &gstAppPosParam.cIsCardInput));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否打印故障报告单的标志
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPrintErrReport(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("其他参数设置", "打印故障报告单", "0.不打印|1.打印", &gstAppPosParam.cIsPrintErrReport));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置POS交易状态
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
	ASSERT_QUIT(PubSelectListItem("0.测试|1.正常", "其他参数设置", "交易状态", nSelect, &nSelect));
	cPosState = nSelect == 0 ? '1' : '0';
	if ( cPosState != gstAppPosParam.cPosState )
	{
		GetWaterNum(&nWaterNum);
		if (nWaterNum>0)
		{
			PubMsgDlg("设置POS交易状态", "有流水还未结算!\n不能改变交易状态", 0, 3);
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
* @brief 设置缺省交易类型
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("其他交易控制", "缺省交易类型", "0.预授权|1.消费", &gstAppPosParam.cDefaultTransType));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置针式打印机时使用的签购单据格式
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}
	nSel = gstAppPosParam.cIsNewTicket-'0';
	ASSERT_RETURNCODE(ProSelectList( "0.新||1.旧||2.空白", "套打签购单样式", nSel, &nSel));
	gstAppPosParam.cIsNewTicket = nSel+'0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	return APP_SUCC;
}

/**
* @brief 设置授权完成联机时是否需要密码的标志
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("交易输密控制", "授权完成请求密码", "0.不输入|1.输入", &gstAppPosParam.cIsAuthSalePin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置撤消类是否需要密码的标志
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("交易输密控制", "消费撤销输入密码", "0.不输入|1.输入", &gstAppPosParam.cIsVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置撤消授权完成时是否需要密码的标志
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("交易输密控制", "授权完成撤销密码", "0.不输入|1.输入", &gstAppPosParam.cIsAuthSaleVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置预授权撤销时是否需要输入密码的标志
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("交易输密控制", "预授权撤销密码", "0.不输入|1.输入", &gstAppPosParam.cIsPreauthVoidPin));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置撤消时是否需要刷卡的标志
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("交易刷卡控制", "消费撤销时刷卡", "0.不刷卡|1.刷卡", &gstAppPosParam.cIsSaleVoidStrip));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置撤消授权完成时是否需要刷卡的标志
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("交易刷卡控制", "授权完成撤销刷卡", "0.不刷卡|1.刷卡", &gstAppPosParam.cIsAuthSaleVoidStrip));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置密钥算法
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionDesMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("终端参数设置", "密钥算法", "0.单倍长密钥||1.双倍长密钥", &gstAppPosParam.cEncyptMode));
	if(gstAppPosParam.cEncyptMode == DESMODE_DES)
		SetVarIsEncryptTrack(NO);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置PIN加密模式
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPinEncyptMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("终端参数设置", "PIN加密模式", "0.不带主帐号加密||1.带主帐号加密", &gstAppPosParam.cPinEncyptMode));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置签购单是否打英文
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}

	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "签购单是否打英文", "0.否|1.是", &gstAppPosParam.cIsTicketWithEn));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置预授权交易是否屏蔽卡号,默认不屏蔽
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "预授权屏蔽卡号", "0.否|1.是", &gstAppPosParam.cIsPreauthShieldPan));
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
* @brief 设置是否提示打印明细
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("结算交易控制", "是否提示打印明细", "0.否|1.是", &gstAppPosParam.cIsPrintWaterRec));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否提示打印失败上送明细
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsPrintFailWaterRec(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("终端参数设置", "是否提示打印失败上送明细", "0.否|1.是", &gstAppPosParam.cIsPrintFailWaterRec));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否支持小费
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("交易功能设置", "是否支持小费", "0.不支持|1.支持", &gstAppPosParam.cIsTipFlag));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	if( gstAppPosParam.cIsTipFlag == '1' )
	{
		return SetFunctionTipRate();
	}
	return APP_SUCC;
}

/**
* @brief 设置小费比率
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

	ASSERT_RETURNCODE(PubInputDlg("系统参数设置", "设置小费比率(%):", gstAppPosParam.szTipRate, &nLen, 1, 2, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief 设置是否自动签退
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("结算交易控制", "结算后自动签退", "0.不支持|1.支持", &gstAppPosParam.cIsAutoLogOut));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否EMV交易显示TVR TSI
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsDispEMV_TVRTSI(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("其他参数设置", "交易显示TVR TSI", "0.不显示|1.显示", &gstAppPosParam.cIsDispEMV_TVRTSI));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否需要屏蔽卡号
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsShieldPan(void)
{
	if (gstAppPosParam.cIsShieldPan != YES  && gstAppPosParam.cIsShieldPan != NO)
	{
		/**<特殊处理，针对旧版升级stAppPosParam.cIsShieldPan的原始赋值*/
		gstAppPosParam.cIsShieldPan = YES;
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("其他参数设置", "屏蔽卡号", "0.不屏蔽|1.屏蔽", &gstAppPosParam.cIsShieldPan));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否支持磁道加密
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
		return APP_FUNCQUIT;//返回此值可以保证向上翻页
	}
	ASSERT_RETURNCODE(ProSelectYesOrNo("终端参数设置", "磁道加密", "0.不支持|1.支持", &gstAppPosParam.cIsEncryptTrack));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是支持小额代授权
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsSmallGeneAuth(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("终端参数设置", "支持小额代授权", "0.不支持|1.支持", &gstAppPosParam.cIsSmallGeneAuth));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
* @brief  判断安全密码是否正确
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
		ASSERT_QUIT(PubInputDlg("终端参数设置", "请输入安全密码:", szAdminPasswd, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));
		if(memcmp(gstAppPosParam.szSecurityPwd, szAdminPasswd, 6))
		{
			char szDispBuf[100];
			PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "安全密码错,请重输");
			PubMsgDlg("终端参数设置", szDispBuf, 0, 1 ) ;
			continue;
		}
		break;

	}
	return APP_SUCC;
}

/**
* @brief  修改安全密码
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
		 * 校验原安全密码
		 */
		ASSERT_QUIT(PubInputDlg("修改安全密码", "请输入原密码", szAdminPasswd1, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));
		if( memcmp(gstAppPosParam.szSecurityPwd, szAdminPasswd1,  8) )
		{
			PubMsgDlg("修改安全密码", "原密码不符", 0, 5) ;
			continue;
		}
		/**
		* 输入新密码
		*/
		memset(szAdminPasswd1, 0, sizeof(szAdminPasswd1));
		ASSERT_QUIT(PubInputDlg("修改安全密码", "请输入新密码:", szAdminPasswd1, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));

		memset(szAdminPasswd2, 0, sizeof(szAdminPasswd2));
		ASSERT_QUIT(PubInputDlg("修改安全密码", "请确认新密码:", szAdminPasswd2, &nLen, 6, 6, 0, INPUT_MODE_PASSWD));

		if(memcmp(szAdminPasswd1, szAdminPasswd2, 6))
		{
			PubMsgDlg("修改安全密码", "两次密码输入不同", 0, 5) ;
			continue;
		}
		else
		{
			SetVarSecurityPwd(szAdminPasswd1);
			PubMsgDlg("修改安全密码", "密码修改成功", 0, 5) ;
			return APP_SUCC;
		}
	}
}


/**
* @brief  设置安全密码
* @param in szSecurityPasswd 安全密码值
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
* @brief 保存结算的流水笔数
* @param in nWaterSum 流水笔数
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
* @brief 获得结算的流水笔数
* @param in pnWaterSum 流水笔数
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
* @brief 保存离线交易已重发的交易次数
* @param in nHaveReSendNum 流水笔数
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
* @brief 获得离线交易已重发的交易次数
* @param in pnHaveReSendNum 流水笔数
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
* @brief 离线交易未上送笔数增加1
* @param in 无
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
* @brief 离线交易未上送笔数减1
* @param in 无
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
* @brief 获取离线交易未上送笔数
* @param in 无
* @return
* @li 离线交易未上送笔数
*/
int GetVarOfflineUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return nNum;
}


/**
* @brief 设置离线交易未上送笔数
* @param in 无
* @return
* @li APP_SUCC
*/
int SetVarOfflineUnSendNum(int nNum)
{
	PubSaveVar( (char *)&nNum, fOfflineUnSendNum_off, fOfflineUnSendNum_len);
	return APP_SUCC;
}

/**
* @brief 保存打印结算单中断标识
* @param in 打印中断
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
* @brief 获得打印结算单中断标志
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
* @brief 保存打印明细中断标志
* @param in 打印中断
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
* @brief 获得打印明细中断标志
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
* @brief 保存打印签购单中断标志
* @param in 打印签购单中断标志
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
* @brief 获得打印签购单中断标志
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
* @brief 保存清除结算数据中断标识
* @param in 清除结算数据中断标识
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
* @brief 获得清除结算数据中断标识
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
* @brief 设置内卡对帐应答代码
* @param in CnCardFlag 内卡对帐应答代码
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
* @brief 获得内卡对帐应答代码
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
* @brief 设置外卡对帐应答代码
* @param in EnCardFlag 外卡对帐应答代码
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
* @brief 获得外卡帐平标识
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
* @brief 设置批上送中断标识
* @param in BatchHaltFlag 批上送中断标识
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
* @brief 获得批上送中断标识
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
* @brief 保存金融类上送标识
* @param in nFinanceHaltFlag 第几笔
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
* @brief 获得金融类上送标识
* @param in pnFinanceHaltFlag 金融类上送标识
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
* @brief 保存通知类上送标识
* @param in nMessageHaltFlag 第几笔
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
* @brief 获得通知类上送标识
* @param in pnMessageHaltFlag 通知类上送标识
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
* @brief 保存磁条卡离线类批上送标识
* @param in nMessageHaltFlag 第几笔
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
* @brief 获得磁条卡离线类批上送标识
* @param in pnMessageHaltFlag 通知类上送标识
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
* @brief 保存结算的批上送总笔数
* @param in nBatchSum 批上送总笔数
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
* @brief 获得结算的批上送总笔数
* @param in pnBatchSum 批上送总笔数
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
* @brief 获得结算的时间
* @param in psSettleDateTime 结算的时间
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
* @brief 保存结算的时间
* @param in nBatchSum 结算的时间
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
* @brief 保存结算数据
* @param in stSettle_NK	内卡结算数据
* @param in stSettle_WK	外卡结算数据
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
* @brief 获得结算数据
* @param in pstSettle_NK	内卡结算数据
* @param in pstSettle_WK	外卡结算数据
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
* @brief 保存结算数据项
* @param in psSettDataItem		需要保存的结算数据项
* @param in nOffset			需要保存的结算数据项所处的偏移
* @param in nLen				需要保存的结算数据项长度
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SaveSettleDataItem( const char * psSettDataItem, const int nOffset, const int nLen)
{
	return PubSaveVar(psSettDataItem, fSettleData_off+nOffset, nLen);
}

/**
* @brief 获得POS的应用参数配置，
* @param out pstAppPosParam  返回存储POS应用参数配置的结构地址
* @return 无
*/
void GetAppPosParam(STAPPPOSPARAM *pstAppPosParam )
{
	memcpy( (char *)pstAppPosParam, (char *)(&gstAppPosParam), sizeof(STAPPPOSPARAM) );
}


/**
* @brief 获得POS的应用参数配置，
* @param out pstAppPosParam  返回存储POS应用参数配置的结构地址
* @return 无
*/
int SetAppPosParam(STAPPPOSPARAM *pstAppPosParam )
{
	memcpy((char *)(&gstAppPosParam),  (char *)pstAppPosParam, sizeof(STAPPPOSPARAM));

	return PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam);
}

/**
* @brief 设置主控进入时的时间因子和随机数
* @param in nTimes 时间因子
* @param in nRandom 随机数
* @return 无
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
* @brief 获取主控进入时的时间因子和随机数
* @param out pnTimes 时间因子
* @param out pnRandom 随机数
* @return 无
*/
int GetVarMCEnterTimeAndRandom(int *pnTimes, int *pnRandom)
{
	*pnTimes = gstBankParam.nMCTimes;
	*pnRandom = gstBankParam.nMCRandom;

	return APP_SUCC;
}

/**
* @brief 设置登陆的操作员信息
* @param in pszOperNo 操作员号码
* @param in cRole 操作员属性(角色)
* @return 无
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
* @brief 获得上回登陆的操作员信息
* @param out pszOperNo 操作员号码
* @param out pnRole 操作员属性(角色)
* @return 无
*/
int GetVarLastOperInfo(char *pszOperNo, int *pnRole)
{
	memcpy(pszOperNo, gstBankParam.szLastLoginOper, sizeof(gstBankParam.szLastLoginOper) - 1);
	*pnRole = (int)gstBankParam.cLastLoginOperRole;

	return APP_SUCC;
}

/**
* @brief 保存是否下载KEK标识
* @param in cMode 算法表示
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
* @brief 获取下载参数标识标识
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
* @brief 设置UID
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
		ASSERT_QUIT(PubInputDlg("其他参数设置", "设置用户ID(16位)", szContent, &nLen, 0, 16, 0, INPUT_MODE_STRING));
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
* @brief 获取UID
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
PubDebug("获取到机型串[%s]", szPosType);
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
	PubDebug("入网证号[%s]\n", pszCertNo);

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
	ASSERT_RETURNCODE(PubInputDlg("交易功能设置", "离线上送笔数", szNumber, &nLen, 1, 2, 0, INPUT_MODE_NUMBER));
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
		ASSERT_RETURNCODE(PubInputDlg("键盘参数", "键盘超时时间:", gstAppPosParam.szPinPadTimeOut, &nLen, 1, 3, 0, INPUT_MODE_NUMBER));
		ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
		return APP_SUCC;
	}
	return APP_FUNCQUIT;//返回此值可以保证向上翻页
}

#ifdef USE_TMS
/**
* @brief 获取TMS下发参数信息
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
* @brief 获取广告信息参数
* @param out pcReSendNum 1位重发次数
* @li APP_SUCC
*/
int GetAdvertisementParam(char * szAdv,int * plen)
{
	*plen= strlen(gstAppPosParam.szAdvertisement);
	memcpy(szAdv,gstAppPosParam.szAdvertisement,*plen);
	return APP_SUCC;
}

/**
* @brief 设置TMS重发次数
* @param 无
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
	ASSERT_RETURNCODE(PubInputDlg("TMS 参数设置", "TMS重试次数:", szTemp, &nLen, 1, 2, 60, INPUT_MODE_NUMBER));
	
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
* @brief 获取是否打印中文收单行
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
* @brief 设置是否打印中文收单行
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "打印中文收单行", "0.关闭|1.打开", &gstAppPosParam.cIsPntChAcquirer));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 获取是否打印中文发卡行
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
* @brief 设置是否打印中文发卡行
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("系统参数设置", "打印中文发卡行", "0.关闭|1.打开", &gstAppPosParam.cIsPntChCardScheme));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 获取隐藏菜单管理员密码
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
* @brief 设置签购单抬头模式
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPrintTitleMode(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("签购单打印", "签购单抬头选择", "0.中文||1.LOGO", &gstAppPosParam.cPntTitleMode));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));

	if ( NO == gstAppPosParam.cPntTitleMode) /*<打印中文*/
	{
		return SetFunctionPntTitle();
	}
	return APP_SUCC;
}
/**
* @brief 获取签购单抬头选择
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
* @brief 设置签购单抬头名称
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPntTitle(void)
{
	int nRet, nLen;

	PubClearAll();
	PubDisplayTitle("签购单打印");
	PubDispMultLines(0, 2, 0, gstAppPosParam.szPntTitleCn);
	PubDisplayStrInline(1, 4, "是否改名? 1.是");
	PubUpdateWindow();
	while(1)
	{
		nRet = PubGetKeyCode(10);
		if ( nRet == KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("签购单打印", NULL, gstAppPosParam.szPntTitleCn, &nLen, 0, 40, CHOOSE_IME_MODE_NUMPY));
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
* @brief  获取签购单抬头中文名
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
* @brief  设置服务热线号码
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionHotLineNo(void)
{
	int nLen;
	ASSERT_RETURNCODE(PubInputDlg("签购单打印", "服务热线设置", gstAppPosParam.szHotLine, &nLen, 0, 20, 0, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief  获取服务热线号码
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
* @brief 设置签购单字体打印选择
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
	ASSERT_RETURNCODE(PubSelectListItem("0.小||1.中||2.大", "签购单字体选择", NULL, nSelect, &nSelect));
	gstAppPosParam.cPntFontSize = (char)nSelect + '0';
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置是否输入主管密码
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsAdminPin(void)
{
	ASSERT_QUIT(ProSelectYesOrNo("其他交易控制", "是否输入主管密码", "0.不输入|1.输入", &gstAppPosParam.cIsAdminPwd));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 获取是否输入主管密码
* @return
* @li YES 需要
* @li NO 无需
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
* @brief 设置传统交易类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTraditionSwitch(void)
{
	int i, nRet;
	int nTransNum=11;		/**<为可开关的交易数*/
	char cSelect;
	char szName[11][17]= {"消费","消费撤销","退货","余额查询","预授权",
	                      "预授权撤销","预授权完成请求","预授权完成通知",
	                      "预授权完成撤销","离线结算","结算调整",
	                     };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sTraditionSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("传统交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置电子现金类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionECashSwitch(void)
{
	int i, nRet;
	int nTransNum=7;		/**<为可开关的交易数*/
	char cSelect;
	char szName[7][17]= {"接触电子现金消费","快速支付(非接)",
	                     "指定账户圈存","非指定账户圈存","电子现金现金充值",
	                     "电子现金充值撤销","电子现金脱机退货",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sECashSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("电子现金开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置电子钱包类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionWalletSwitch(void)
{
	int i, nRet;
	int nTransNum=4;		/**<为可开关的交易数*/
	char cSelect;
	char szName[4][17]= {"电子钱包消费","指定账户圈存",
	                     "非指定账户圈存","电子钱包现金充值",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sWalletSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("电子钱包开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置分期付款类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionInstallmentSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<为可开关的交易数*/
	char cSelect;
	char szName[2][17]= {"分期付款消费","分期付款消费撤销",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sInstallmentSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("分期付款开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置积分类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionBonusSwitch(void)
{
	int i, nRet;
	int nTransNum=6;		/**<为可开关的交易数*/
	char cSelect;
	char szName[6][17]= {"联盟积分消费","发卡行积分消费","联盟积分消费撤销",
	                     "发卡行消费撤销","联盟积分查询","联盟积分退货",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sBonusSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("积分交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置手机芯片类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionPhoneChipSaleSwitch(void)
{
	int i, nRet;
	int nTransNum=9;		/**<为可开关的交易数*/
	char cSelect;
	char szName[9][17]= {"手机消费","手机消费撤销","手机芯片退货",
	                     "手机芯片预授权","手机预授权撤销","预授权完成请求",
	                     "预授权完成通知","预授权完成撤销","手机芯片余额查询",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sPhoneChipSaleSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("手机芯片开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置预约类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionAppointmentSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<为可开关的交易数*/
	char cSelect;
	char szName[2][17]= {"预约消费","预约消费撤销",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sAppointmentSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("预约交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置订购类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionOrderSwitch(void)
{
	int i, nRet;
	int nTransNum=8;		/**<为可开关的交易数*/
	char cSelect;
	char szName[8][17]= {"订购消费","订购消费撤销","订购退货",
	                     "订购预授权","订购预授权撤销","预授权完成请求",
	                     "预授权完成通知","预授权完成撤销",
	                    };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sOrderSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("订购交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置其他类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionOtherSwitch(void)
{
	int i, nRet;
	int nTransNum=2;		/**<为可开关的交易数*/
	char cSelect;
	char szName[2][17]= {"磁条卡现金充值","磁条卡账户充值",};

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}

	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sOtherSwitch[i/8]&(0x80>>(i%8))) ? 1 : 0;

		nRet = ProSelectYesOrNoExt("其它交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
* @brief 设置输入主管密码
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
	ASSERT_RETURNCODE( ProSelectYesOrNo("其他交易控制", "输入主管密码", "0.否|1.是", &gstAppPosParam.cIsAdminPwd));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}


/**
*  针对传统交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
* @return
* @li YES
* @li NO
*/
YESORNO GetTraditionSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_SALE:						/**< 消费*/
		nTransNum = 0;
		break;
	case TRANS_VOID_SALE:				/**< 消费撤销*/
		nTransNum = 1;
		break;
	case TRANS_REFUND:					/**< 退货*/
		nTransNum = 2;
		break;
	case TRANS_BALANCE:					/**< 查询*/
		nTransNum = 3;
		break;
	case TRANS_PREAUTH:					/**< 预授权*/
		nTransNum = 4;
		break;
	case TRANS_VOID_PREAUTH:				/**< 授权撤销*/
		nTransNum = 5;
		break;
	case TRANS_AUTHSALE:					/**< 授权完成请求*/
		nTransNum = 6;
		break;
	case TRANS_AUTHSALEOFF:				/**< 授权完成通知*/
		nTransNum = 7;
		break;
	case TRANS_VOID_AUTHSALE:			/**< 授权完成请求撤销*/
		nTransNum = 8;
		break;
	case TRANS_OFFLINE:					/**< 离线结算*/
		nTransNum = 9;
		break;
	case TRANS_ADJUST:					/**< 结算调整*/
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
*  针对电子现金交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
* @return
* @li YES
* @li NO
*/
YESORNO GetECashSwitchOnoff(char cTransType,char cAttr)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_EMV_REFUND:				/**< 电子现金脱机退货*/
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
*  针对分期付款交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
* @return
* @li YES
* @li NO
*/
YESORNO GetInstallmentSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_INSTALMENT:				/**< 分期付款*/
		nTransNum = 0;
		break;
	case TRANS_VOID_INSTALMENT:			/**< 撤销分期*/
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
*  针对积分交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
* @return
* @li YES
* @li NO
*/
YESORNO GetBonusSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_BONUS_ALLIANCE:			/**< 联盟积分消费*/
		nTransNum = 0;
		break;
	case TRANS_BONUS_IIS_SALE:			/**< 发卡行积分消费*/
		nTransNum = 1;
		break;
	case TRANS_VOID_BONUS_ALLIANCE:		/**< 撤销联盟积分消费*/
		nTransNum = 2;
		break;
	case TRANS_VOID_BONUS_IIS_SALE:		/**< 撤销发卡行积分消费*/
		nTransNum = 3;
		break;
	case TRANS_ALLIANCE_BALANCE:			/**< 联盟积分查询*/
		nTransNum = 4;
		break;
	case TRANS_ALLIANCE_REFUND:			/**< 联盟积分退货*/
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
*  针对手机芯片交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
* @return
* @li YES
* @li NO
*/
YESORNO GetPhoneChipSaleSwitchOnoff(char cTransType)
{
	int nTransNum;

	switch(cTransType)
	{
	case TRANS_PHONE_SALE:				/**< 手机芯片消费*/
		nTransNum = 0;
		break;
	case TRANS_VOID_PHONE_SALE:			/**< 手机芯片消费撤销*/
		nTransNum = 1;
		break;
	case TRANS_REFUND_PHONE_SALE:		/**< 手机芯片退货*/
		nTransNum = 2;
		break;
	case TRANS_PHONE_PREAUTH:			/**< 手机芯片预授权*/
		nTransNum = 3;
		break;
	case TRANS_VOID_PHONE_PREAUTH:		/**< 手机芯片预授权撤销*/
		nTransNum = 4;
		break;
	case TRANS_PHONE_AUTHSALE:			/**< 手机芯片预授权完成请求*/
		nTransNum = 5;
		break;
	case TRANS_PHONE_AUTHSALEOFF:		/**< 手机芯片预授权完成通知*/
		nTransNum = 6;
		break;
	case TRANS_VOID_PHONE_AUTHSALE:		/**< 手机芯片预授权完成撤销*/
		nTransNum = 7;
		break;
	case TRANS_PHONE_BALANCE:			/**< 手机芯片余额查询*/
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
*  针对预购交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
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
*  针对订购交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
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
*  针对其他交易,根据输入的交易类型判断是否支持此交易
* @param in cTransType 交易类型
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
* switch 交易开关控制值
* n 开关第几位
* i 开关对应交易
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

	/*串码验券*/
	TieTieSwitch(pszTransSwtch, 0);
	/*手机号验券*/
	TieTieSwitch(pszTransSwtch, 1);
	/*银行卡验券*/
	TieTieSwitch(pszTransSwtch, 2);
	/*撤销*/
	TieTieSwitch(pszTransSwtch, 3);
	/*退货*/
	TieTieSwitch(pszTransSwtch, 4);
	/*微信扫码支付*/
	TieTieSwitch(pszTransSwtch, 5);
	/*微信条码支付*/
	TieTieSwitch(pszTransSwtch, 6);
	/*微信退货*/
	TieTieSwitch(pszTransSwtch, 7);
	/*百度扫码支付*/
	TieTieSwitch(pszTransSwtch, 8);
	/*百度退货*/
	TieTieSwitch(pszTransSwtch, 9);
	/*京东扫码*/
	TieTieSwitch(pszTransSwtch, 10);
	/*京东条码*/
	TieTieSwitch(pszTransSwtch, 11);
	/*京东退货*/
	TieTieSwitch(pszTransSwtch, 12);
	/*支付宝条码*/
	TieTieSwitch(pszTransSwtch, 13);
	/*支付宝退货*/
	TieTieSwitch(pszTransSwtch, 14);
	/*美团兑券*/
	TieTieSwitch(pszTransSwtch, 15);
	/*盘锦通*/
	TieTieSwitch(pszTransSwtch, 16);
	/*点评闪惠主扫*/
	TieTieSwitch(pszTransSwtch, 17);
	/*点评闪惠被扫*/
	TieTieSwitch(pszTransSwtch, 18);
	/*点评闪惠退货*/
	TieTieSwitch(pszTransSwtch, 19);
	/*翼支付主扫*/
	TieTieSwitch(pszTransSwtch, 20);
	/*翼支付退货*/
	TieTieSwitch(pszTransSwtch, 21);
	/*点评支付确认*/
	TieTieSwitch(pszTransSwtch, 22);
	/*点评承兑*/
	TieTieSwitch(pszTransSwtch, 23);
	/*支付宝被扫*/
	TieTieSwitch(pszTransSwtch, 24);
	/*百度被扫*/
	TieTieSwitch(pszTransSwtch, 25);
	/*统一主扫*/
	TieTieSwitch(pszTransSwtch, 26);
	/*统一被扫*/
	TieTieSwitch(pszTransSwtch, 27);
	/*统一退货*/
	TieTieSwitch(pszTransSwtch, 28);

	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}



/**
* @brief 设置贴贴交易类型开关
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionTieTieSwitch(void)
{
	int i, nRet;
	int nTransNum=10;		/**<为可开关的交易数*/
	char cSelect;
	char szName[11][17]= {"串码验券","手机号验券","银行卡验券","撤销","退货",
	                      "微信扫码支付","微信条码支付","微信退货","百度钱包","百度退货"
	                     };

	if(GetVarIsModification() == NO)
	{
		return APP_FUNCQUIT;
	}
	for(i = 0; i < nTransNum; i++)
	{
		/**< 如果名称为空串，不进行设置，并维持原值*/
		if (szName[i][0] == 0)
		{
			continue;
		}
		cSelect =  (gstAppPosParam.sTieTieSwich[i/8]&(0x80>>(i%8))) ? 1 : 0;
		nRet = ProSelectYesOrNoExt("贴贴交易开关控制", szName[i], "0.不支持|1.支持", &cSelect);
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
	case TRANS_COUPON_VERIFY:			/**< 串码验券*/
		nTransNum = 0;
		break;
	case TRANS_TEL_VERIFY:				/**< 手机号验券*/
		nTransNum = 1;
		break;
	case TRANS_CARD_VERIFY:				/**< 银行卡验券*/
		nTransNum = 2;
		break;
	case TRANS_VOID_VERIFY:				/**< 撤销*/
		nTransNum = 3;
		break;
	case TRANS_REFUND_VERIFY:			/**< 退货*/
		nTransNum = 4;
		break;
	case TRANS_PRECREATE:				/**<扫码支付*/
		nTransNum = 5;
		break;
	case TRANS_CREATEANDPAY:			/**<条码支付*/
		nTransNum = 6;
		break;
	case TRANS_WX_REFUND:				/**<微信退货*/
		nTransNum = 7;
		break;
	case TRANS_CREATEANDPAYBAIDU:		/**<百度支付*/
		nTransNum = 8;
		break;
	case TRANS_BAIDU_REFUND:			/**<百度退货*/
		nTransNum = 9;
		break;
	case TRANS_JD_PRECREATE:			/**<京东支付*/
		nTransNum = 10;
		break;							
	case TRANS_JD_CREATEANDPAY:			/**<京东条码*/
		nTransNum = 11;
		break;
	case TRANS_JD_REFUND:				/**<京东退货*/
		nTransNum = 12;
		break;
	case TRANS_ALI_CREATEANDPAY:		/**<支付宝条码付*/
		nTransNum = 13;
		break;
	case TRANS_ALI_REFUND:				/**<支付宝退货*/
		nTransNum = 14;
		break;
	case TRANS_COUPON_MEITUAN:			/**<美团兑券*/
		nTransNum = 15;
		break;
	case TRANS_PANJINTONG:				/**<盘锦通*/
		nTransNum = 16;
		break;
	case TRANS_DZ_CREATEANDPAY:			/**<点评主扫*/
		nTransNum = 17;
		break;
	case TRANS_DZ_PRECREATE:			/**<点评被扫*/
		nTransNum = 18;
		break;
	case TRANS_DZ_REFUND:				/**<点评闪惠退货*/
		nTransNum = 19;
		break;
	case TRANS_BESTPAY_CREATEANDPAY:	/**<翼支付主扫*/
		nTransNum = 20;
		break;
	case TRANS_BESTPAY_REFUND:			/**<翼支付退货*/
		nTransNum = 21;
		break;
	case TRANS_COUPONFRM_DAZHONG:		/**<点评订单确认*/
		nTransNum = 22;
		break;
	case TRANS_COUPON_DAZHONG:			/**<点评承兑*/
		nTransNum = 23;
		break;
	case TRANS_ALI_PRECREATE:			/**<支付宝被扫*/
		nTransNum = 24;
		break;
	case TRANS_BAIDU_PRECREATE:			/**<百度被扫*/
		nTransNum = 25;
		break;
	case TRANS_ALLPAY_CREATEANDPAY:		/**<统一主扫*/
		nTransNum = 26;
		break;
	case TRANS_ALLPAY_PRECREATE:		/**<统一被扫*/
		nTransNum = 27;
		break;
	case TRANS_ALLPAY_REFUND:			/**<统一退货*/
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
	/**<考虑银商TMS交易设置顺序,将顺序微调*/	
		
/*第一个字节*/
	
	/*<1,查询,3*/
	TraditionSwitch(*pszTransSwtch, 0x80, 3);
	
	/*<2,预授权,4*/
	TraditionSwitch(*pszTransSwtch, 0x40, 4);	

	/*<3,预授权撤销, 5*/
	TraditionSwitch(*pszTransSwtch, 0x20, 5);

	/*<4,预授权完成请求, 6*/
	TraditionSwitch(*pszTransSwtch, 0x10, 6);

	/*<5,预授权完成撤销, 8*/
	TraditionSwitch(*pszTransSwtch, 0x08, 8);

	/*<6,消费, 0*/
	TraditionSwitch(*pszTransSwtch, 0x04, 0);

	/*<7,消费撤销, 1*/
	TraditionSwitch(*pszTransSwtch, 0x02, 1);

	/*<8,退货, 2*/
	TraditionSwitch(*pszTransSwtch, 0x01, 2);

/*第二个字节*/
	
	/*<9,离线结算, 9*/
	TraditionSwitch(*(pszTransSwtch+1), 0x80, 9);

	/*<10,结算调整, 10*/
	TraditionSwitch(*(pszTransSwtch+1), 0x40, 10);

	/*<11,预授权完成通知, 7*/
	TraditionSwitch(*(pszTransSwtch+1), 0x20, 7);

	/*<12,脚本结果通知, */

	/*<13,电子现金脱机消费, */
	ECashSwitch(*(pszTransSwtch+1), 0x08, 0);
	ECashSwitch(*(pszTransSwtch+1), 0x08, 1);
	/*<14,, */

	/*<15,电子钱包圈存类交易, */
	WalletSwitch(*(pszTransSwtch+1), 0x02,1);
	WalletSwitch(*(pszTransSwtch+1), 0x02,2);
	WalletSwitch(*(pszTransSwtch+1), 0x02,3);
	
	/*<16,分期付款, */
	InstallmentSwitch(*(pszTransSwtch+1), 0x01,0);
/*第三个字节*/	
	/*<17,分期付款撤销, */
	InstallmentSwitch(*(pszTransSwtch+2), 0x80,1);
	/*<18,积分消费, */
	BonusSwitch(*(pszTransSwtch+2), 0x40,0);
	BonusSwitch(*(pszTransSwtch+2), 0x40,1);
	/*<19,积分消费撤销, */
	BonusSwitch(*(pszTransSwtch+2), 0x20,2);
	BonusSwitch(*(pszTransSwtch+2), 0x20,3);
	/*<20,圈存类, */
	ECashSwitch(*(pszTransSwtch+2), 0x10,2);
	ECashSwitch(*(pszTransSwtch+2), 0x10,3);
	ECashSwitch(*(pszTransSwtch+2), 0x10,4);
	ECashSwitch(*(pszTransSwtch+2), 0x10,5);
	/*<21,预约消费, */
	AppointmentSwitch(*(pszTransSwtch+2), 0x08,0);
	/*<22,预约消费撤销, */
	AppointmentSwitch(*(pszTransSwtch+2), 0x04,1);
	/*<23,订购消费, */
	OrderSwitch(*(pszTransSwtch+2), 0x02,0);
	/*<24,订购消费撤销, */
	OrderSwitch(*(pszTransSwtch+2), 0x01,1);
/*第四个字节*/	
	/*<25,磁条卡充值, */
	OtherSwitch(*(pszTransSwtch+3), 0x80,0);
	OtherSwitch(*(pszTransSwtch+3), 0x80,1);

	return APP_SUCC;
}

/**
* @brief  设置本地是否允许手动修改参数
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_FAIL
*/
int SetFunctionIsModification(void)
{
	ASSERT_RETURNCODE( ProSelectYesOrNo("其他交易控制", "是否允许手动修改POS参数", "0.不允许|1.允许", &gstAppPosParam.cIsModify));
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
	第1位 消费第2位 消费撤销第3 位 退货第4位 余额查询第5位 预授权
	第6位 预授权撤销第7位 预授权完成请求第8位 预授权完成通知第9位 预授权完成撤销
	第10位 离线结算第11位 结算调整*/
	for(i=0; i<11; i++) /*如果该位不为0，则重置值*/
	{
		pstAppPosParam->sTraditionSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*第12位 接触式电子现金消费
	第13位 快速支付（非接电子现金消费）第14位 电子现金指定账户圈存
	第15位 电子现金非指定账户圈存第16位 电子现金现金充值
	第48位 电子现金现金充值撤销
	第49位 电子现金脱机退货
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
	第17位 电子钱包消费第18位 电子钱包指定账户圈存
	第19位 电子钱包非指定账户圈存第20位 电子钱包现金充值*/
	j+=5;
	for(i=0; i<4; i++)
	{
		pstAppPosParam->sWalletSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	第21位 分期付款消费
	第22位 分期付款消费撤销*/
	j+=4;
	for(i=0; i<2; i++)
	{
		pstAppPosParam->sInstallmentSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	第23位 联盟积分消费
	第24位 发卡行积分消费
	第25位 联盟积分消费撤销
	第26位 发卡行积分消费撤销
	第50位 联盟积分查询第51位 联盟积分退货
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
	第27位 手机芯片消费第28位 手机芯片消费撤销第29位 手机芯片退货
	第30位 手机芯片预授权第31位 手机芯片预授权撤销第32位 手机芯片预授权完成请求
	第33位 手机芯片预授权完成通知第34位 手机芯片预授权完成撤销第35位 手机芯片余额查询
	*/
	j+=4;
	for(i=0; i<9; i++)
	{
		pstAppPosParam->sPhoneChipSaleSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}


	/*
	第36位 预约消费
	第37位 预约撤销
	*/
	j+=9;
	for(i=0; i<2; i++)
	{
		pstAppPosParam->sAppointmentSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	第38位 订购消费第39位 订购撤销
	第40位 订购退货第41位 订购预授权第42位 订购预授权撤销
	第43位 订购预授权完成请求第44位 订购预授权完成撤销第45位 订购预授权完成通知
	*/
	j+=2;
	for(i=0; i<8; i++)
	{
		pstAppPosParam->sOrderSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	/*
	第46位 磁条卡现金充值第47位 磁条卡账户充值
	*/
	j+=8;
	for(i=0; i<6; i++)
	{
		pstAppPosParam->sOtherSwitch[i/8]|=(((pszTransSwtch[(i+j)/8]&(1<<(7-((i+j)%8))))!=0)?(0x01<<(7-(i%8))):0x00);
	}
	return APP_SUCC;
}
/*是否打印负号*/
YESORNO GetVarIsPrintPrintMinus(void)
{
	return gstAppPosParam.cIsPrintMinus;
}
int SetFunctionIsPrintAllTrans(void)
{
	ASSERT_RETURNCODE( ProSelectYesOrNo("其他交易控制", "打印所有交易明细", "0否|1是", &gstAppPosParam.cIsPrintAllTrans));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
YESORNO GetVarIsPrintAllTrans(void)
{
	return gstAppPosParam.cIsPrintAllTrans;
}
/*取未知发卡行信息*/
void GetVarUnknowBankInfo(char *pszOut)
{
	strcpy(pszOut,gstAppPosParam.szPrintUnknowBankInfo);
}

#ifndef USE_TMS
int SetFunctionIsPrintPrintMinus(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "是否打印负号", "0否|1是", &gstAppPosParam.cIsPrintMinus));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

int SetFunctionUnknowBankInfo(void)
{
	int nLen;
	int nRet;

	PubClearAll();
	PubDisplayTitle("发卡行名称设置");
	PubDisplayStr(1, 2, 1, "%-11.11s", gstAppPosParam.szPrintUnknowBankInfo);
	PubDisplayStrInline(1, 4, "是否改名? 1.是");
	PubUpdateWindow();
	while(1)
	{
		nRet=PubGetKeyCode(10);
		if ( nRet== KEY_1 )
		{
			ASSERT_RETURNCODE(PubInputCNMode("发卡行名称设置", NULL, gstAppPosParam.szPrintUnknowBankInfo, &nLen, 0, 11, CHOOSE_IME_MODE_NUMPY));
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
/*保存pinkey密文*/
int SetVarPinKey(char *pszPinKey,int nLen)
{
	memcpy(gstAppPosParam.szEncryptPinKey,pszPinKey,nLen>(sizeof(gstAppPosParam.szEncryptPinKey)-1)?(sizeof(gstAppPosParam.szEncryptPinKey)-1):nLen);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}
/*保存trk密文*/
int SetVarTrKey(char *pszTrk,int nLen)
{
	memcpy(gstAppPosParam.szEncryptTrk,pszTrk,nLen>(sizeof(gstAppPosParam.szEncryptTrk)-1)?(sizeof(gstAppPosParam.szEncryptTrk)-1):nLen);
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/*****获取TMS下发的打印广告信息******/
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("重打结算单设置", "重打印结算单", "0.不支持|1.支持", &gstAppPosParam.cIsReprintSettle));
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
* @brief 版本升级变量初始化控制
* @param nUpdateVer 版本号
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitUpdateVar(const int nUpdateVer)
{
	
	/**<POS参数*/
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
* @brief 是否进行版本升级(针对变量)
* @param 无
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
	if (memcmp(szUpdateCtrl, UPDATE_CTRL, fUpdateCtrl_len) < 0) //版本升级
	{
		for (i = atoi(szUpdateCtrl); i < atoi(UPDATE_CTRL); i++)
		{
			InitUpdateVar(i);
		}

		PubSaveVar(UPDATE_CTRL, fUpdateCtrl_off, fUpdateCtrl_len);
	}
	return APP_SUCC;
}


/**已上送脚本通知次数*/
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
*是否在屏幕上显示RFLOGO,
*因为像8080YS以及IM81这种,非接区域不在屏幕上的,就不在屏幕显示LOGO,避免误导客户
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
* @brief 标记是否是qPboc交易
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
* @brief 显示终端的版本信息。
* @param 无
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
* @brief 显示终端的版本信息。
* @param 无
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
	PubDisplayTitle("软件版本信息(TMS)");
#else
	PubDisplayTitle("软件版本信息");
#endif	
	PubDisplayStrInline(DISPLAY_MODE_CLEARLINE, 2, "程序软件版本号:");
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

//还原参数信息
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
	PubMsgDlg(NULL, "正在进行参数还原", 0, 1);
	PubUpdateWindow();
    //001 商户号
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "001", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szMerchantId, szBuf);
    
    //002 终端号
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "002", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szTerminalId, szBuf);

    //003 打印页数
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "003", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cPrintPageCount = szBuf[0];
    
    //004 射频卡支持
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "004", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsSupportRF = szBuf[0];
    
    //005 内外读卡器
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "005", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsExRF = szBuf[0];
    
   
    //006 电子签名标志
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "006", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsSupportElecSign = szBuf[0];
     
    //007 小费设置
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "007", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsTipFlag = szBuf[0];

	//008 小费比率
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "008", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szTipRate, szBuf);
   
    //009 是否支持扫描枪
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "009", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsHaveScanner = szBuf[0];

	//010 设置内外置扫描枪
	memset(szBuf, 0, sizeof(szBuf));
	ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "010", &nValueLen, szBuf, unFileLen));
	gstAppPosParam.cIsOutsideScaner = szBuf[0];

	//011 应用显示名称
	memset(szBuf, 0, sizeof(szBuf));
	ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "011", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szAppDispname, szBuf);

	//201 密码键盘标志
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "201", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cIsPinPad = szBuf[0];
    
    //202 密码超时
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "202", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szPinPadTimeOut, szBuf);
    
    //203 密钥索引
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "203", &nValueLen, szBuf, unFileLen));
    strcpy(gstAppPosParam.szMainKeyNo, szBuf);
    
    //204 密码键盘通讯口
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "204", &nValueLen, szBuf, unFileLen));
    gstAppPosParam.cPinPadAuxNo = szBuf[0];
    
    //301 交易超时时间
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "301", &nValueLen, szBuf, unFileLen));
    stAppCommParam.cTimeOut = szBuf[0];
    
    //302 交易TPDU
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "302", &nValueLen, szBuf, unFileLen));
    if (nValueLen == 5)
    {
        memcpy(stAppCommParam.sTpdu, szBuf, nValueLen);
    }
    //303 通讯类型
    memset(szBuf, 0, sizeof(szBuf));
    ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "303", &nValueLen, szBuf, unFileLen));
    stAppCommParam.cCommType = szBuf[0];
    switch(stAppCommParam.cCommType)
	{
	case COMM_DIAL://拨号
        //311 预拨号码
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
        //312 电话号码1
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
        //313 电话号码2
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
        //314 电话号码3
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
	case COMM_ETH://以太网
        //321 服务器IP地址
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "321", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //322 服务端口号
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "322", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1);
        //323 本地IP地址
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "323", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIpAddr, szBuf); 
        //324 本地子网掩码
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "324", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szMask, szBuf); 
        //325 网关地址
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "325", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szGate, szBuf); 
   		break;
	case COMM_CDMA://CDMA
        //331 CDMA接入号码
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "331", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szWirelessDialNum, szBuf); 
        //332 CDMA服务器IP地址
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "332", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //333 CDMA服务端口
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "333", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1); 
        //334 CDMA用户名
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
        //335 CDMA用户密码
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
        //336 CDMA长短链接
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "336", &nValueLen, szBuf, unFileLen));
        stAppCommParam.cMode = szBuf[0]; 
		break;
	case COMM_GPRS://GPRS
        //341 GPRS接入号码
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "341", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szWirelessDialNum, szBuf); 
        //342 GPRS服务器IP地址
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "342", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szIp1, szBuf); 
        strcpy(stAppCommParam.szIp2, stAppCommParam.szIp1);
        //343 GPRS服务端口
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "343", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szPort1, szBuf); 
        strcpy(stAppCommParam.szPort2, stAppCommParam.szPort1); 
        //344 GPRS APN
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "344", &nValueLen, szBuf, unFileLen));
        strcpy(stAppCommParam.szAPN1, szBuf); 
        strcpy(stAppCommParam.szAPN2, stAppCommParam.szAPN1);
        //345 GPRS 用户名
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
        //346 GPRS 用户密码
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
        //347 GPRS长短链接
        memset(szBuf, 0, sizeof(szBuf));
        ASSERT_PARABACK_FAIL(ParamTagGet(nIniHandle, "347", &nValueLen, szBuf, unFileLen));
        stAppCommParam.cMode = szBuf[0];
        break;
	default:
		break;
	}
	PubCloseFile(&nIniHandle);

	PubMsgDlg(NULL, "参数还原完成", 0, 1);
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	ASSERT_FAIL(SetAppCommParam(&stAppCommParam));
#if defined(USE_MAINCTRL)
	MC_SetPubParam(MODEM_PRE_DAIL_NUM, stAppCommParam.szPreDial, strlen(stAppCommParam.szPreDial));
#endif	
	return APP_SUCC;
}

//保存参数信息
int ParamSave(void)
{
	const char *szIniFile = BACK_PARAM_INI;
	int nIniHandle;
	STAPPCOMMPARAM stAppCommParam;

    PubDelFile(szIniFile);
	ASSERT_FAIL(PubReadOneRec(FILE_APPCOMMPARAM,1,(char *)&stAppCommParam));
	ASSERT_QUIT(PubOpenFile(szIniFile, "w", &nIniHandle));
    //001 商户号
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "001", strlen(gstAppPosParam.szMerchantId), gstAppPosParam.szMerchantId));
    //002 终端号
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "002", strlen(gstAppPosParam.szTerminalId), gstAppPosParam.szTerminalId));
    //003 打印页数
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "003", 1, &gstAppPosParam.cPrintPageCount));
    //004 射频卡支持
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "004", 1, &gstAppPosParam.cIsSupportRF));
    //005 内外读卡器
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "005", 1, &gstAppPosParam.cIsExRF));
    //006 电子签名标志
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "006", 1, &gstAppPosParam.cIsSupportElecSign));   
    //007 小费设置
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "007", 1, &gstAppPosParam.cIsTipFlag));
    //008 小费比率
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "008", strlen(gstAppPosParam.szTipRate), gstAppPosParam.szTipRate));    
    //009 是否支持扫描
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "009", 1, &gstAppPosParam.cIsHaveScanner));
    //010 内外置扫描枪
	ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "010", 1, &gstAppPosParam.cIsOutsideScaner));
    //011 应用显示名称
	ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "011", strlen(gstAppPosParam.szAppDispname), gstAppPosParam.szAppDispname));
	//201 密码键盘标志
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "201", 1, &gstAppPosParam.cIsPinPad));
    //202 密码超时
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "202", strlen(gstAppPosParam.szPinPadTimeOut), gstAppPosParam.szPinPadTimeOut));
    //203 密钥索引
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "203", strlen(gstAppPosParam.szMainKeyNo), gstAppPosParam.szMainKeyNo));
    //204 密码键盘通讯口
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "204", 1, &gstAppPosParam.cPinPadAuxNo));
    
    //301 交易超时时间
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "301", 1, &stAppCommParam.cTimeOut));
    //302 交易TPDU
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "302", 5, stAppCommParam.sTpdu));
    //303 通讯类型
    ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "303", 1, &stAppCommParam.cCommType));
    switch(stAppCommParam.cCommType)
	{
	case COMM_DIAL://拨号
        //311 预拨号码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "311", strlen(stAppCommParam.szPreDial), stAppCommParam.szPreDial));
        //312 电话号码1
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "312", strlen(stAppCommParam.szTelNum1), stAppCommParam.szTelNum1));
        //313 电话号码2
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "313", strlen(stAppCommParam.szTelNum2), stAppCommParam.szTelNum2));
        //314 电话号码3
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "314", strlen(stAppCommParam.szTelNum3), stAppCommParam.szTelNum3));
		break;
	case COMM_ETH://以太网
        //321 服务器IP地址
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "321", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //322 服务端口号
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "322", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //323 本地IP地址
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "323", strlen(stAppCommParam.szIpAddr), stAppCommParam.szIpAddr));
        //324 本地子网掩码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "324", strlen(stAppCommParam.szMask), stAppCommParam.szMask));
        //325 网关地址
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "325", strlen(stAppCommParam.szGate), stAppCommParam.szGate));
		break;
	case COMM_CDMA://CDMA
        //331 CDMA接入号码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "331", strlen(stAppCommParam.szWirelessDialNum), stAppCommParam.szWirelessDialNum));
        //332 CDMA服务器IP地址
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "332", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //333 CDMA服务端口
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "333", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //334 CDMA用户名
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "334", strlen(stAppCommParam.szUser), stAppCommParam.szUser));
        //335 CDMA用户密码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "335", strlen(stAppCommParam.szPassWd), stAppCommParam.szPassWd));
        //336 CDMA长短链接
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "336", 1, &stAppCommParam.cMode));
		break;
	case COMM_GPRS://GPRS
        //341 GPRS接入号码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "341", strlen(stAppCommParam.szWirelessDialNum), stAppCommParam.szWirelessDialNum));
        //342 GPRS服务器IP地址
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "342", strlen(stAppCommParam.szIp1), stAppCommParam.szIp1));
        //343 GPRS服务端口
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "343", strlen(stAppCommParam.szPort1), stAppCommParam.szPort1));
        //344 GPRS APN
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "344", strlen(stAppCommParam.szAPN1), stAppCommParam.szAPN1));
        //345 GPRS 用户名
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "345", strlen(stAppCommParam.szUser), stAppCommParam.szUser));
        //346 GPRS 用户密码
        ASSERT_PARABACK_FAIL(ParamTagSave(nIniHandle, "346", strlen(stAppCommParam.szPassWd), stAppCommParam.szPassWd));
        //347 GPRS长短链接
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
    		//PubMsgDlg(NULL, "读文件失败", 3, 10);
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
                //PubMsgDlg(NULL, "读文件失败", 3, 10);
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
		//PubMsgDlg(NULL, "写文件失败", 3, 10);
		return APP_FAIL;
	}
	return APP_SUCC;
}




/**
* @brief 从参数文件读入参数
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
		sprintf(pszErr, "未知错误码%s", szErrCode);
	}
	PubCloseFile(&nIniHandle);
	return nRet;
}

#if defined (SUPPORT_ELECSIGN)
/**
* @brief 获取未成功上送签字重复上送次数
* @param out pnValue 
* @return 无
*/	
void GetVarElecSendTime(int *pnValue)
{	
	*pnValue = gstAppPosParam.cElecSendTime;
	return;
}
/**
* @brief 设置未成功上送签字重复上送次数
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
	ASSERT_RETURNCODE(PubInputDlg("电子签名设置", "签名重复上送次数:", szNumber, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
	nNumber = atoi(szNumber);
	gstAppPosParam.cElecSendTime = nNumber;
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief 设置电子签名超时时间
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncElecTimeOut(void)
{	
	int nLen = 0;

	ASSERT_RETURNCODE(PubInputDlg("电子签名设置", "交易超时时间:", gstAppPosParam.szElecTimeOut, \
		&nLen, 1, 3, 60, INPUT_MODE_NUMBER));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
} 

/**
* @brief 获取电子签名超时时间
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarElecTimeOut(char* pcValue)
{
	memcpy(pcValue, gstAppPosParam.szElecTimeOut, sizeof(gstAppPosParam.szElecTimeOut) - 1);
	return APP_SUCC;
}

/**
* @brief 设置外接电子签字板类型
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncExElecBordType(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("电子签名设置", "外接签名板类型", "0.710|1.汉王", &gstAppPosParam.cExElecBordType));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));	
	return APP_SUCC;
}

/**
* @brief 获取外接电子签字板类型
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarExElecBordType(void)
{
	return gstAppPosParam.cExElecBordType;
}

/**
* @brief 设置签名后是否需要操作员确认
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncExElecConfirm(void)
{
	ASSERT_RETURNCODE(ProSelectYesOrNo("电子签名设置", "签名后操作员确认", "0.不需要|1.需要", &gstAppPosParam.cExElecConfirm));
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));	
	return APP_SUCC;
}

/**
* @brief 获取操作员确认标志
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int GetVarExElecConfirm(void)
{
	return gstAppPosParam.cExElecConfirm;
}

/**
* @brief 设置电子签名上送结果
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
* @brief 获取是否支持射频卡
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
* @brief 设置最后一笔上送电子签字的编号
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
* @brief 获取最后一笔上送电子签字的编号
* @param out pszValue 3个字节
* @return 无
*/	
void GetVarLastElecSignSendNum(char *pszValue)
{
	memcpy(pszValue,gstAppPosParam.sLastElecSignSendNum,sizeof(gstAppPosParam.sLastElecSignSendNum));
	return;
}


/**
* @brief 获取电子签名分包标志
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
* @brief 获取电子签名分包大小
* @param out
* @return 无
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
* @brief 设置电子签名分包标志以及分包大小
* @param 无
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncElecSubcontract(void)
{	
	char szTemp[4+1];
	int nLen = 0;

	memset(szTemp, 0, sizeof(szTemp));

	ASSERT_RETURNCODE(ProSelectYesOrNo("电子签名设置", "分包传输开关", "0.关闭|1.打开", &gstAppPosParam.cIsElecSubcontractFlag));
	if(gstAppPosParam.cIsElecSubcontractFlag == YES)
	{
		ASSERT_FAIL(PubInputDlg("电子签名设置", "分包字节数:", gstAppPosParam.sElecSubcontractSize, \
		&nLen, 1, 4, 60, INPUT_MODE_NUMBER));
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPPOSPARAM,1,(char *)&gstAppPosParam));
	return APP_SUCC;
}

/**
* @brief电子签名交易未上送笔数增加1
* @param in 无
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
* @brief 电子签名交易未上送笔数减1
* @param in 无
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
* @brief 获取离线交易未上送笔数
* @param in 无
* @return
* @li 离线交易未上送笔数
*/
int GetVarElecSignUnSendNum()
{
	int nNum = 0;
	PubGetVar( (char *)&nNum, fElecSignUnSendNum_off, fElecSignUnSendNum_len);
	return nNum;
}


/**
* @brief 设置离线交易未上送笔数
* @param in 无
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
	char *pszTitle = "电子签名设置";
	
	ASSERT_QUIT(ProSelectYesOrNo(pszTitle, "是否支持电子签名", "0.不支持|1.支持", &cOldFlg));
	if (cOldFlg != gstAppPosParam.cIsSupportElecSign)
	{
		GetWaterNum(&nWaterSum);
		if (nWaterSum > 0)
		{
			PubMsgDlg(pszTitle, "有交易流水先结算", 3, 10);
			return APP_FAIL;
		}
	}
	
	gstAppPosParam.cIsSupportElecSign = cOldFlg;

	if (YES == gstAppPosParam.cIsSupportElecSign)
	{
		/*<选择内置还是外接*/

		/*<内置*/
		if (1 == PubScrGetColorDepth())
		{
			PubMsgDlg(pszTitle, "黑白屏终端不支持电子签名", 0, 3);
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

//时间的有效性
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
* @brief 设置日期时间
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
	ASSERT_RETURNCODE(PubInputDate("设置日期", "输入日期(YYYYMMDD):", szDate, INPUT_DATE_MODE_YYYYMMDD, 60));
	
	if(PubIsValidDate(szDate) != APP_SUCC)
	{
		PubMsgDlg("设置日期", "输入的日期格式错误", 3, 5);
		return APP_FAIL;
	}

	memset(szTime, 0, sizeof(szTime));
	memcpy(szTime, szDateTime+8, 6);
	ASSERT_RETURNCODE(PubInputDlg("设置时间", "请输入时间(HHMMSS):", szTime, &nLen, 6, 6, 60, INPUT_MODE_NUMBER));

	if(CheckIsValidTime(szTime) != APP_SUCC)
	{
		PubMsgDlg("设置时间", "输入的时间格式错误", 3, 5);
		return APP_FAIL;
	}
	
	PubSetPosDateTime(szDate, "YYYYMMDD", szTime);
	return APP_SUCC;

}

int SetFunctionRfSearchCardDelay(void)
{
	char *pszTitle = "非接寻卡延迟";
	int nLen = 0;

	if (YES == gstAppPosParam.cIsExRF)
	{
		return APP_FUNCQUIT;
	}

	ASSERT_RETURNCODE(ProSelectYesOrNo(pszTitle, "非接寻卡延迟", "0.不延时|1.延时", &gstAppPosParam.cIsRfSearchCardDelay));
	if (YES == gstAppPosParam.cIsRfSearchCardDelay)
	{
		ASSERT_RETURNCODE(PubInputDlg(pszTitle, "寻卡延时时间(Ms):", gstAppPosParam.szRfSearchDelayTimeMs, &nLen, 1, 3, 60, INPUT_MODE_NUMBER));

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
	
	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "内外置扫描枪", "0.内置|1.外置", &gstAppPosParam.cIsOutsideScaner));
	if(NO == gstAppPosParam.cIsOutsideScaner)
	{
		if (APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_SCANNER, NULL))
		{
			PubMsgDlg("温馨提示", "不支持内置扫描枪", 3, 3);
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
	ASSERT_RETURNCODE(ProSelectYesOrNo("系统参数设置", "是否支持扫描枪", "0.否|1.是", &gstAppPosParam.cIsHaveScanner));
	
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
	* 导入缺省参数
	*/
	
	PubLuaDisplay("LuaFirstRunChk");
	InitOper();
	InitCommParam();
	InitWaterFile();
	ClearSettle();						/**<清除结算数据*/
	PubDeReclFile(SETTLEFILENAME);
	InitBlkFile();
	ASSERT_FAIL(InitPosDefaultParam());/*<这一步要放在最后做*/
	NDK_AppDel("贴贴应用TTYY");
    SetParamFromIni();		
	ASSERT(ParamRestore()); 		   //wanglez add 20151228 首次运行检测参数还原
	return APP_SUCC;
}


