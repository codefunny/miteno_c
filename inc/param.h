/**
* @file param.h
* @brief 参数管理模块
* @version  1.0
* @author 戴建斌
* @date 2007-02-05
*/
#ifndef _PARAM_H_
#define _PARAM_H_

#include "settle.h"

#define ASSERT_PARA_FAIL(e, msg) \
	if ((e) != APP_SUCC)\
	{\
		PubMsgDlg("参数设置", msg, 3, 3);\
		PubCloseFile(&nIniHandle);\
		return APP_FAIL;\
	}

#define POS_STATE_NORMAL	'0'		/**<正常交易状态*/
#define POS_STATE_TEST		'1'		/**<测试交易状态*/

/**
* @struct pos参数文件
*/
typedef struct
{
	char szTerminalId[8+1];				/**<终端号*/
	char szMerchantId[15+1];			/**<商户号*/
	char szMerchantNameCn[40+1];		/**<商户名称中文*/
	char szMerchantNameEn[40+1];		/**<商户名称英文*/
	char szAppType[2+1];				/**<应用类型，"60" */
	char szAppVer[2+1];					/**<应用版本，"31" */
	char cPosState;						/**<POS状态，'0'－正常，'1'－测试*/
	char szBusinessBankId[4+1];			/**<商行代码*/
	char szLocalCode[4+1];				/**<本地区区号*/	
	char cIsPinPad;						/**<是否接密码键盘	'0'-不接，'1'-接*/
	char cPinPadAuxNo;					/**<密码键盘串口号*/
	char szPinPadTimeOut[3+1];			/**<密码键盘输入超时*/
	char szSecurityPwd[8+1];			/**<终端安全密码*/
	char szMainKeyNo[2+1];				/**<主密钥INDEX	，'00'-'99'*/
	char cEncyptMode;					/**<算法标识 des 3des，'0'-des，'1'-3des*/
	char cPinEncyptMode;				/**<PIN加密模式,'0'-带主账号，'1'-不带主账号*/
	char cIsEncryptTrack;				/**<是否需要磁道加密*/
	char cIsAutoLogOut;					/**<是否自动签退,'0'-否,'1'-是*/
	char cIsOfflineSendNow;				/**<离线交易上送方式,'0'-批结算之前一并上送,'1'-随下笔联机交易上送一笔离线交易*/
	char cIsOfflineSendAll;				/**<离线交易上送笔数标识,'0'-送一笔,'1'-全送,结算前送，此参数无效*/	
	char cIsPrintWaterRec;				/**<是否打印明细*/
	char cIsNewTicket;					/**<签购单标识，'1'表示旧，'0'表示新，'2'表示空白打印*/
	char cIsTicketWithEn;				/**<签购单带英文*/
	char cIsPrintErrReport;				/**<打印故障报告单*/
	char cPrintPageCount;				/**<打印几联单,'0'-'9'*/
	char szMaxTransCount[5+1];			/**<POS存储交易笔数*/
	char szMaxRefundAmount[12+1];		/**<最大退货金额*/	
	char cIsTipFlag;					/**<支持小费交易标志*/
	char szTipRate[2+1];				/**<小费比率*/
	char cIsIcFlag;						/**<支持IC卡标志*/
	char cIsIcConfirmFlag;				/**<IC卡确认参数*/
	char cIsCardInput;					/**<是否支持手工输入卡号*/
	char cIsSaleVoidStrip;				/**<消费撤消是否刷卡*/
	char cIsAuthSaleVoidStrip;			/**<预授权完成撤消是否刷卡*/
	char cIsAuthSaleVoidPin;			/**<预受权完成撤消是否输密码*/
	char cIsPreauthVoidPin;				/**<预授权撤消是否输密码*/
	char cIsVoidPin;					/**<撤销类是否输密码*/
	char cIsAuthSalePin;				/**<预授权完成请求是否输密码*/
	char cDefaultTransType;				/**<默认交易,'1'-消费,'0'-预授权*/
	char cAuthSaleMode;					/**<磁卡授权完成方式,'0'-同时支持，'1'-支持请求，'2'-支持通知*/
	char cIsDispEMV_TVRTSI;				/**<是否显示EMV交易的tvr和tsi,'1'显示,'0'不显示*/
	char sUID[16];						/**<AK*/
	char cIsShieldPan;					/**<是否屏蔽卡号*/
	char cEMV_Type;						/**<EMV核心的配置*/
	char cIsExRF;						/**<是否外接射频卡读卡器，'1'支持*/
	char cIsSupportRF;					/**<是否支持射频卡，'1'支持*/
	char cTmsReSendNum;					/**<TMS重发次数*/
	char cIsSmallGeneAuth;				/**<是否支持小额代授权*/

	
	char cExElecBordType;				/**<外接签字板类型'0':我司710,'1':汉王签字板*/
	char cElecSignSendResult;			/**<电子签名上送结果*/
	char cExElecConfirm;				/**<电子签名完是否需要操作员按键确认'0'不需要'1'需要*/
	char cIsElecSubcontractFlag;		/**<电子签字分包传输标志*/
	char sElecSubcontractSize[4+1];		/**<电子签字分包传输的单位字节数*/

	char cIsPrintFailWaterRec;			/**<是否提示打印失败上送明细*/
	char cIsUsePP60RF;					/**<是否使用PP60RF*/
	char sLastElecSignSendNum[3];		/**<末笔上送电子签字的编号*/
	char szAdvertisement[99+1];    		/**TMS下发的广告信息*/
	char cIsModify;						/**终端参数是否可修改*/
	char szHideMenuPwd[8+1];           	/**系统隐藏菜单密码*/
	char cPntTitleMode;					/**<签购单抬头模式*/
	char szPntTitleCn[40+1];			/**<签购单抬头名称中文*/
	char szHotLine[20+1];				/**签购单服务热线*/
	char szAppDispname[20+1];			/**应用显示名称*/
	char cIsAdminPwd;                	/**是否输入主管密码*/
	char cIsPntChAcquirer;				/**是否打印中文收单行*/
	char cIsPntChCardScheme;            /**是否打印中文发卡行*/
	/*开关控制*/
	char sTraditionSwitch[4];			/**<传统类交易类型*/	
	char sECashSwitch[4];				/**<电子现金交易类型*/
	char sWalletSwitch[4];				/**<电子钱包交易类型*/
	char sInstallmentSwitch[4];			/**<分期付款交易类型*/
	char sBonusSwitch[4];				/**<积分交易类型*/
	char sPhoneChipSaleSwitch[4];		/**<手机芯片交易类型*/
	char sAppointmentSwitch[4];			/**<预约类交易类型*/
	char sOrderSwitch[4];				/**<订购类交易类型*/
	char sOtherSwitch[4];				/**<其他交易类型*/
	char sTieTieSwich[4];				/**<贴贴业务交易类型*/
	char cIsOfflineSendTimes;			/**<离线交易上送次数,未完成*/
	char cPntFontSize;					/*打印字体大小*/
	char cSafeKeepYear;					/*签购单保管年限*/
	char cIsPrintMinus;					/*撤销等交易是否打印负号1打印，0不打印*/
	char cIsPrintAllTrans;				/*结算单明细打印，1打印所有交易，0仅打印有效交易 */
	char szPrintUnknowBankInfo[20+1];	/*未知发卡行代码打印方式：为空时打印代码，否则打印该域数据*/
	
	char cIsSupportElecSign;			/**是否支持电子签名*/
	char cElecSendTime;					/**<电子签名重复上送次数*/
	char szElecTimeOut[3+1];			/**<等待签字输入超时时间*/
	
	char cIsRfSearchCardDelay;            /**<*非接寻卡延迟标志*/
	char szRfSearchDelayTimeMs[3+1];	/**<非接寻卡延时时间*/
	char szReserve1[6+1];				/**<保留域1*/
	
	char szEncryptPinKey[24+1];			/*加密的pin key*/
	char szEncryptTrk[24+1];			/*加密的TRK key*/
	char szMaxOffSendNum[2 + 1];		/*离线上送笔数*/
	char szAdInfo[60 + 1];				/*0.88需求,TMS下发的广告信息*/
	int nSettleNum;						/*结算次数*/

	/*以下PBOC3.0增加 ，占1字节，预留去掉1个*/
	char cIsPreauthShieldPan;			/**新增系统参数设置:预授权交易是否屏蔽卡号*/
	char cIsReprintSettle;				/**是否重打印结算单*/
	char cIsUseLbs;						/*是否使用LBS基站定位*/
	int nLbsNum;						/*LBS基站信息上送间隔笔数*/
	char cIsOutsideScaner;				/**<内置或外置扫描枪*/
	char cIsHaveScanner;					/**<是否支持SCANER*/
	char szReserve2[36];				/**<保留域2*/
}STAPPPOSPARAM;

extern int IsFirstRun(void);
extern int InitPosDefaultParam(void);
extern int ExportPosParam(void);
extern int SetParamFromIni(void);
extern int GetVarPosMerType(int *);
extern int SetVarPosMerType(const int);
extern int GetVarCommHead(char *);
extern int SetVarCommHead(const char *);
extern YESORNO GetVarIsLogin(void);
extern int SetVarIsLogin(const YESORNO);
extern YESORNO GetVarIsLock(void);
extern int SetVarIsLock(const YESORNO);
extern int GetVarTraceNo(char *);
extern int SetVarTraceNo(const char *);
extern int IncVarTraceNo(void);
extern int GetVarBatchNo(char *);
extern int SetVarBatchNo(const char *);
extern int IncVarBatchNo(void);
extern int GetVarDialSuccSum(ulong *);
extern int SetVarDialSuccSum(const ulong);
extern int IncVarDialSuccSum(void);
extern int GetVarDialSum(ulong *);
extern int SetVarDialSum(const ulong);
extern int IncVarDialSum(void);
extern int GetVarTimeCounter(long *);
extern int SetVarTimeCounter(const long);
extern int GetVarLastLoginDateTime(char *);
extern int SetVarLastLoginDateTime(const char *);
extern YESORNO GetVarIsReversal(void);
extern int SetVarIsReversal(const YESORNO);
extern int GetVarHaveReversalNum(int *);
extern int SetVarHaveReversalNum(const int);
extern int IncVarHaveReversalNum(void);
extern int GetVarReversalData(char *, int *);
extern int SetVarReversalData(const char *, const int);
extern void GetVarTerminalId(char *);
extern void GetVarMerchantId(char *);
extern void GetVarMerchantNameCn(char *);
extern int SetVarMerchantNameCn(const char *);
extern void GetVarMerchantNameEn(char *);
extern void GetVarAppType(char *);
extern int SetVarAppType(const char *);
extern void GetVarDispSoftVer(char *);
extern void GetVarAppVer(char *);
extern int  GetVarPosState(void);
extern void GetVarBusinessBankId(char *);
extern void GetVarLocalcode(char *);
extern YESORNO GetVarIsPinpad(void);
extern int GetVarPinpadAuxNo(int *);
extern int GetVarPinpadTimeOut(int *);
extern void GetVarSecurityPwd(char *);
extern void GetVarMainKeyNo(int *);
extern  int SetVarMainKeyNo(const int);
extern int  GetVarEncyptMode(void);
extern int  GetVarPinEncyptMode(void);
extern YESORNO GetVarIsAutoLogOut(void);
extern int SetVarIsAutoLogOut(const YESORNO);
extern YESORNO GetVarIsPrintWaterRec(void);
extern int GetVarIsNewTicket(void);
extern YESORNO GetVarIsTickeWithEn(void);
extern YESORNO GetVarIsPrintErrReport(void);
extern void GetVarPrintPageCount(int *);
extern void GetVarMaxTransCount(int *);
void GetVarMaxRefundAmount(char *);
extern int SetVarTipRate(const char *);
extern void GetVarTipRate(char *);
extern int SetFunctionNewTransSwitch(void);
extern int SetVarIsTipFlag(const YESORNO);
extern YESORNO GetVarIsIcFlag(void);
extern YESORNO GetVarIsCardInput(void);
extern int SetVarIsCardInput(const YESORNO);
extern YESORNO GetVarIsSaleVoidStrip(void);
extern YESORNO GetVarIsAuthSaleVoidStrip(void);
extern YESORNO GetVarIsVoidPin(void);
extern YESORNO GetVarIsAuthSaleVoidPin(void);
extern YESORNO GetVarIsPreauthVoidPin(void);
extern YESORNO GetVarIsAuthSalePin(void);
extern void GetVarDefaultTransType(char *);
extern void GetVarAuthSaleMode(char *);
extern YESORNO GetVarIsDispEMV_TVRTSI(void);
extern void GetVarReserve1(char *);
extern void GetVarReserve2(char *);
extern int GetVarEmvTransSerial(int *);
extern int SetVarEmvTransSerial(const int);
extern int IncVarEmvTransSerial(void);
extern int GetVarEmvOfflineUpNum(int *);
extern int SetVarEmvOfflineUpNum(const int);
extern int IncVarEmvOfflineUpNum(void);
extern int GetVarEmvOnlineUpNum(int *);
extern int SetVarEmvOnlineUpNum(const int);
extern int IncVarEmvOnlineUpNum(void);
extern int GetVarEMVOfflineFailHaltFlag(int *);
extern int SetVarEMVOfflineFailHaltFlag(const int);
extern int GetVarEMVOnlineARPCErrHaltFlag(int *);
extern int SetVarEMVOnlineARPCErrHaltFlag(const int);
extern YESORNO GetVarEmvIsDownCAPKFlag(void);
extern int SetVarEmvIsDownCAPKFlag(const YESORNO);
extern YESORNO GetVarEmvIsDownAIDFlag(void);
extern int SetVarEmvIsDownAIDFlag(const YESORNO);
extern YESORNO GetVarEmvIsScriptInform(void);
extern int SetVarEmvIsScriptInform(const YESORNO);
extern int GetVarEmvScriptData(char *, int);
extern int SetVarEmvScriptData(const char *, const int);
extern int SetReversalReSendNum(void);
extern int SetFunctionMainKeyNo(void);
extern int SetFunctionMaxRefundAmount(void);
extern int SetFunctionAuthsaleMode(void);
extern int SetFunctionPrintPageCount(void);
extern int SetFunctionMaxTransCount(void);
extern int SetFunctionMerchantId(void);
extern int SetFunctionTerminalId(void);
extern int SetFunctionMerchantName(void);
extern int SetFunctionPOSYear(void);
extern int SetFunctionTraceNo(void);
extern int SetFunctionBatchNo(void);
extern int SetFunctionBusinessBankId(void);
extern int SetFunctionLocalCode(void);
extern int SetFunctionIsPinPad(void);
extern int SetFunctionIsIcFlag(void);
extern int SetFunctionIsCardInput(void);
extern int SetFunctionIsPrintErrReport(void);
extern int SetFunctionPosState(void);
extern int SetFunctionDefaultTransType(void);
extern int SetFunctionIsAuthSalePin(void);
extern int SetFunctionIsVoidPin(void);
extern int SetFunctionIsAuthSaleVoidPin(void);
extern int SetFunctionIsPreauthVoidPin(void);
extern int SetFunctionIsSaleVoidStrip(void);
extern int SetFunctionIsAuthSaleVoidStrip(void);
extern int SetFunctionDesMode(void);
extern int SetFunctionPinEncyptMode(void);
extern int SetFunctonIsTickeWithEn(void);
extern int SetFunctionIsPrintWaterRec(void);
extern YESORNO GetVarIsTipFlag(void);
extern int SetFunctionIsTipFlag(void);
extern int SetFunctionAutoLogout(void);
extern int ChkSecurityPwd(void);
extern int SetVarSecurityPwd(const char *);
extern int ChangeSecurityPwd(void);
extern int SetFunctionIsIcConfirmFlag(void);
extern int SetFunctionIsDispEMV_TVRTSI(void);
extern int SetFunctionIsEncryptTrack(void);
extern int SetFunctionIsSmallGeneAuth(void);
extern int SetFunctionIsNewTicket(void);
extern int SetVarWaterSum(const int);
extern int GetVarWaterSum(int *);
extern int SetVarHaveReSendNum(const int);
extern int GetVarHaveReSendNum(int *);
extern int SetVarPrintSettleHalt(const YESORNO);
extern YESORNO GetVarPrintSettleHalt(void);
extern int SetVarCnCardFlag(const char);
extern char GetVarCnCardFlag(void);
extern int SetVarEnCardFlag(const char);
extern char GetVarEnCardFlag(void);
extern int SetVarBatchHaltFlag(const YESORNO);
extern YESORNO GetVarBatchHaltFlag(void);
extern int SetVarFinanceHaltFlag(const int);
extern int GetVarFinanceHaltFlag(int *);
extern int SetVarMessageHaltFlag(const int);
extern int GetVarMessageHaltFlag(int *);
extern int SetVarBatchMagOfflineHaltFlag(const int);
extern int GetVarBatchMagOfflineHaltFlag(int *);
extern int SetVarBatchSum(const int);
extern int GetVarBatchSum(int *);
extern int GetVarSettleDateTime(char *);
extern int SetVarSettleDateTime(const char *);
extern int SetVarSettleData(const STSETTLE, const STSETTLE);
extern int GetVarSettleData(STSETTLE *, STSETTLE *);
extern int SaveSettleDataItem( const char *, const int, const int);
extern int SetVarPrintDetialHalt(const YESORNO);
extern YESORNO GetVarPrintDetialHalt(void);
extern int SetVarPrintWaterHaltFlag(const YESORNO);
extern YESORNO GetVarPrintWaterHaltFlag(void);
extern int SetVarClrSettleDataFlag(const YESORNO);
extern YESORNO GetVarClrSettleDataFlag(void);
extern void GetAppPosParam(STAPPPOSPARAM *);
extern int SetAppPosParam(STAPPPOSPARAM *);
extern int SetFunctionTipRate(void);
extern void SetControlChkPinpad(const YESORNO);
extern YESORNO GetControlChkPinpad(void);
extern void SetControlChkPinpad(const YESORNO);
extern void SetControlChkInside(const YESORNO);
extern void SetControlChkRF(const YESORNO);
extern YESORNO GetControlChkPinpad(void);
extern YESORNO GetControlChkInside(void);
extern YESORNO GetControlChkRF(void);
extern YESORNO GetVarIsIcConfirmFlag(void);
extern int SetVarMCEnterTimeAndRandom(int, int);
extern int GetVarMCEnterTimeAndRandom(int *, int *);
extern int SetVarLastOperInfo(char *, char);
extern int GetVarLastOperInfo(char *, int *);
extern int SetFunctionUID(char *);
extern int GetVarUID(char *);
#if defined(EMV_IC)
extern YESORNO GetVarIsExRF(void);
extern int SetFunctionIsExRF(void);
extern YESORNO GetVarIsSupportRF(void);
extern int SetFunctionIsSupportRF(void);
extern YESORNO GetVarIsUsePP60RF(void);
extern int SetFunctionIsUsePP60RF(void);
#endif
extern int SetVarIsPhoneSale(const YESORNO);
extern YESORNO GetVarIsPhoneSale(void);
extern YESORNO GetVarIsEncryptTrack(void);
extern int SetVarIsEncryptTrack(const YESORNO);
extern YESORNO GetVarIsSmallGeneAuth(void);
extern int SetVarIsSmallGeneAuth(const YESORNO);
extern YESORNO GetVarIsShieldPan(void);
extern int SetFunctionIsShieldPan(void);
extern int GetVarCertNo(char *);
extern void GetVarSoftVer(char *);
extern int IncVarOfflineUnSendNum(void);
extern int DelVarOfflineUnSendNum(void);
extern int GetVarOfflineUnSendNum(void);
extern int SetVarOfflineUnSendNum(int);
extern YESORNO GetVarIsModification(void);
extern int SetVarIsModification(YESORNO);
extern int GetHideMenuPwd(char *);
extern int SetTmsTransSwitchValue(char *,STAPPPOSPARAM *);
extern int SetFunctionIsPrintAllTrans(void);
extern YESORNO GetVarIsPrintAllTrans(void);
extern YESORNO GetVarIsPrintPrintMinus(void);
extern void GetVarUnknowBankInfo(char *);
#ifndef USE_TMS
extern int SetFunctionIsPrintPrintMinus(void);
extern int SetFunctionUnknowBankInfo(void);
#else
extern int GetAdvertisementParam(char *,int *);
extern int IncSettleNum(void);
extern int ClearSettleNum(void);
extern int GetVarSettleNum(void);
extern int GetTmsParamDown();
extern int SetFuncTmsReSendNum(void);
extern int GetVarTmsReSendNum(char *);

#endif
extern int SetFunctionAppName(void);
extern int SetFunctionMerchantNameEn(void);
extern int SetFunctionIsManagePinInput(void);
extern int SetFunctionIsOfflineSendNow(void);
extern int SetFunctionOffResendNum(void);
extern int SetIsPntChAquirer(void);
extern int SetIsPntChCardScheme(void);
extern int SetFunctionIsPrintFailWaterRec(void);
extern int SetFunctionSafeKeep(void);
extern int SetFunctionIsModification(void);
extern int SetFunctionPrintTitleMode(void);
extern int SetFunctionPntTitle(void);
extern int SetFunctionHotLineNo(void);
extern int SetFunctionPrintSize(void);
extern int SetVarPinKey(char *, int);
extern int SetVarTrKey(char *, int);
extern void GetVarPinKey(char *, int);
extern void GetVarTrKey(char *, int);
extern int SetFunctionMaxOffSendNum(void);
extern void GetVarMaxOffSendNum(int *);
extern int SetFunctionPinpadTimeOut(void);
extern YESORNO GetVarIsAdminPin();
extern int GetVarAdInfo(char *);
extern YESORNO GetIsPntChAquirer(void);
extern YESORNO GetIsPntChCardScheme(void);
extern YESORNO GetIsPntTitleMode(void);
extern int GetFunctionPntTitle(char *);
extern int GetFunctionHotLineNo(char *);
extern YESORNO GetTraditionSwitchOnoff(char);
extern YESORNO GetECashSwitchOnoff(char, char);
extern YESORNO GetInstallmentSwitchOnoff(char);
extern YESORNO GetBonusSwitchOnoff(char);
extern YESORNO GetPhoneChipSaleSwitchOnoff(char);
extern YESORNO GetAppointmentSwitchOnoff(char);
extern YESORNO GetOrderSwitchOnoff(char);	
extern YESORNO GetOtherSwitchOnoff(char);
extern int SetFunctionTraditionSwitch(void);
extern int SetFunctionECashSwitch(void);
extern int SetFunctionWalletSwitch(void);
extern int SetFunctionInstallmentSwitch(void);
extern int SetFunctionBonusSwitch(void);
extern int SetFunctionPhoneChipSaleSwitch(void);
extern int SetFunctionAppointmentSwitch(void);
extern int SetFunctionOrderSwitch(void);
extern int SetFunctionOtherSwitch(void);
extern int SetTransSwitchValue(char *);
extern int SetFunctionIsPreauthShieldPan(void);
extern YESORNO GetVarIsPreauthShieldPan();
extern int SetFunctionReprintSettle();
extern YESORNO GetVarReprintSettle(void);
extern void GetAcqName(const char *, char *, char);
extern int  GetShowName(char *);
extern YESORNO GetVarShowRFLogo();
extern int SaveEmvTransSerial(void);
extern int VerUpdateChk();
extern int SetVarHaveScriptAdviseNum(const char);
extern int SetFlagQpboc(const YESORNO);
extern YESORNO GetFlagQpboc(void);
extern int SetVarFirstRunFlag(YESORNO);
extern YESORNO GetVarFirstRunFlag(void);
extern EM_PRN_FONT_SIZE GetVarPrnFontSize(void);
extern int Version(void);
extern int GetEmvErrorCodeIni(int, char *);
extern int GetPrnLogo(char *, uint *);
#if defined (SUPPORT_ELECSIGN)
extern void GetVarElecSendTime(int *);
extern int SetFunctionElecSendTime(void);
extern int SetFuncElecTimeOut(void);
extern int GetVarElecTimeOut(char*);
extern int SetFuncExElecConfirm(void);
extern int GetVarExElecConfirm(void);
extern int SetVarElecSignSendResult(const YESORNO);
extern YESORNO GetVarElecSignSendResult(void);
extern int SetVarLastElecSignSendNum(const char *);
extern void GetVarLastElecSignSendNum(char *);
extern YESORNO GetVarElecSubFlag(void);
extern void GetVarElecSubSize(int *);
extern int SetFuncElecSubcontract(void);
extern int IncVarElecSignUnSendNum(void);
extern int DelVarElecSignUnSendNum(void);
extern int GetVarElecSignUnSendNum(void);
extern int SetVarElecSignUnSendNum(int);
extern int SetFunctionSupportElecSign(void);
extern YESORNO GetVarSupportElecSign(void);
#endif

extern int SetFunctionPosDateTime(void);
extern int SetFunctionRfSearchCardDelay(void);
extern int SetVarIsInitScanner(const char cIsInitScanner);
extern char GetVarIsInitScanner(void);
extern int SetFunctionIsOutsideScanner(void);	
extern char GetVarIsOutsideScanner(void);
extern int SetFunctionIsHaveScanner(void);
extern char GetVarIsHaveScanner(void);
extern int SetTransTieTieSwitchValue(char *pszTransSwtch);
extern int SetVarDownKek(const YESORNO cFlag);
extern YESORNO GetVarDownKek(void);
extern YESORNO GetTieTieSwitchOnoff(char cTransType);
extern int SetFunctionTieTieSwitch(void);
extern int ParamRestore(void);
extern int ParamSave(void);
extern int ParamTagGet(int , char *, int *, char *, int );
extern int ParamTagSave(int , char *, int , char *);
extern int ResetDefaultParam();



#endif

