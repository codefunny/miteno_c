/**
* @file param.h
* @brief ��������ģ��
* @version  1.0
* @author ������
* @date 2007-02-05
*/
#ifndef _PARAM_H_
#define _PARAM_H_

#include "settle.h"

#define ASSERT_PARA_FAIL(e, msg) \
	if ((e) != APP_SUCC)\
	{\
		PubMsgDlg("��������", msg, 3, 3);\
		PubCloseFile(&nIniHandle);\
		return APP_FAIL;\
	}

#define POS_STATE_NORMAL	'0'		/**<��������״̬*/
#define POS_STATE_TEST		'1'		/**<���Խ���״̬*/

/**
* @struct pos�����ļ�
*/
typedef struct
{
	char szTerminalId[8+1];				/**<�ն˺�*/
	char szMerchantId[15+1];			/**<�̻���*/
	char szMerchantNameCn[40+1];		/**<�̻���������*/
	char szMerchantNameEn[40+1];		/**<�̻�����Ӣ��*/
	char szAppType[2+1];				/**<Ӧ�����ͣ�"60" */
	char szAppVer[2+1];					/**<Ӧ�ð汾��"31" */
	char cPosState;						/**<POS״̬��'0'��������'1'������*/
	char szBusinessBankId[4+1];			/**<���д���*/
	char szLocalCode[4+1];				/**<����������*/	
	char cIsPinPad;						/**<�Ƿ���������	'0'-���ӣ�'1'-��*/
	char cPinPadAuxNo;					/**<������̴��ں�*/
	char szPinPadTimeOut[3+1];			/**<����������볬ʱ*/
	char szSecurityPwd[8+1];			/**<�ն˰�ȫ����*/
	char szMainKeyNo[2+1];				/**<����ԿINDEX	��'00'-'99'*/
	char cEncyptMode;					/**<�㷨��ʶ des 3des��'0'-des��'1'-3des*/
	char cPinEncyptMode;				/**<PIN����ģʽ,'0'-�����˺ţ�'1'-�������˺�*/
	char cIsEncryptTrack;				/**<�Ƿ���Ҫ�ŵ�����*/
	char cIsAutoLogOut;					/**<�Ƿ��Զ�ǩ��,'0'-��,'1'-��*/
	char cIsOfflineSendNow;				/**<���߽������ͷ�ʽ,'0'-������֮ǰһ������,'1'-���±�������������һ�����߽���*/
	char cIsOfflineSendAll;				/**<���߽������ͱ�����ʶ,'0'-��һ��,'1'-ȫ��,����ǰ�ͣ��˲�����Ч*/	
	char cIsPrintWaterRec;				/**<�Ƿ��ӡ��ϸ*/
	char cIsNewTicket;					/**<ǩ������ʶ��'1'��ʾ�ɣ�'0'��ʾ�£�'2'��ʾ�հ״�ӡ*/
	char cIsTicketWithEn;				/**<ǩ������Ӣ��*/
	char cIsPrintErrReport;				/**<��ӡ���ϱ��浥*/
	char cPrintPageCount;				/**<��ӡ������,'0'-'9'*/
	char szMaxTransCount[5+1];			/**<POS�洢���ױ���*/
	char szMaxRefundAmount[12+1];		/**<����˻����*/	
	char cIsTipFlag;					/**<֧��С�ѽ��ױ�־*/
	char szTipRate[2+1];				/**<С�ѱ���*/
	char cIsIcFlag;						/**<֧��IC����־*/
	char cIsIcConfirmFlag;				/**<IC��ȷ�ϲ���*/
	char cIsCardInput;					/**<�Ƿ�֧���ֹ����뿨��*/
	char cIsSaleVoidStrip;				/**<���ѳ����Ƿ�ˢ��*/
	char cIsAuthSaleVoidStrip;			/**<Ԥ��Ȩ��ɳ����Ƿ�ˢ��*/
	char cIsAuthSaleVoidPin;			/**<Ԥ��Ȩ��ɳ����Ƿ�������*/
	char cIsPreauthVoidPin;				/**<Ԥ��Ȩ�����Ƿ�������*/
	char cIsVoidPin;					/**<�������Ƿ�������*/
	char cIsAuthSalePin;				/**<Ԥ��Ȩ��������Ƿ�������*/
	char cDefaultTransType;				/**<Ĭ�Ͻ���,'1'-����,'0'-Ԥ��Ȩ*/
	char cAuthSaleMode;					/**<�ſ���Ȩ��ɷ�ʽ,'0'-ͬʱ֧�֣�'1'-֧������'2'-֧��֪ͨ*/
	char cIsDispEMV_TVRTSI;				/**<�Ƿ���ʾEMV���׵�tvr��tsi,'1'��ʾ,'0'����ʾ*/
	char sUID[16];						/**<AK*/
	char cIsShieldPan;					/**<�Ƿ����ο���*/
	char cEMV_Type;						/**<EMV���ĵ�����*/
	char cIsExRF;						/**<�Ƿ������Ƶ����������'1'֧��*/
	char cIsSupportRF;					/**<�Ƿ�֧����Ƶ����'1'֧��*/
	char cTmsReSendNum;					/**<TMS�ط�����*/
	char cIsSmallGeneAuth;				/**<�Ƿ�֧��С�����Ȩ*/

	
	char cExElecBordType;				/**<���ǩ�ְ�����'0':��˾710,'1':����ǩ�ְ�*/
	char cElecSignSendResult;			/**<����ǩ�����ͽ��*/
	char cExElecConfirm;				/**<����ǩ�����Ƿ���Ҫ����Ա����ȷ��'0'����Ҫ'1'��Ҫ*/
	char cIsElecSubcontractFlag;		/**<����ǩ�ְַ������־*/
	char sElecSubcontractSize[4+1];		/**<����ǩ�ְַ�����ĵ�λ�ֽ���*/

	char cIsPrintFailWaterRec;			/**<�Ƿ���ʾ��ӡʧ��������ϸ*/
	char cIsUsePP60RF;					/**<�Ƿ�ʹ��PP60RF*/
	char sLastElecSignSendNum[3];		/**<ĩ�����͵���ǩ�ֵı��*/
	char szAdvertisement[99+1];    		/**TMS�·��Ĺ����Ϣ*/
	char cIsModify;						/**�ն˲����Ƿ���޸�*/
	char szHideMenuPwd[8+1];           	/**ϵͳ���ز˵�����*/
	char cPntTitleMode;					/**<ǩ����̧ͷģʽ*/
	char szPntTitleCn[40+1];			/**<ǩ����̧ͷ��������*/
	char szHotLine[20+1];				/**ǩ������������*/
	char szAppDispname[20+1];			/**Ӧ����ʾ����*/
	char cIsAdminPwd;                	/**�Ƿ�������������*/
	char cIsPntChAcquirer;				/**�Ƿ��ӡ�����յ���*/
	char cIsPntChCardScheme;            /**�Ƿ��ӡ���ķ�����*/
	/*���ؿ���*/
	char sTraditionSwitch[4];			/**<��ͳ�ཻ������*/	
	char sECashSwitch[4];				/**<�����ֽ�������*/
	char sWalletSwitch[4];				/**<����Ǯ����������*/
	char sInstallmentSwitch[4];			/**<���ڸ��������*/
	char sBonusSwitch[4];				/**<���ֽ�������*/
	char sPhoneChipSaleSwitch[4];		/**<�ֻ�оƬ��������*/
	char sAppointmentSwitch[4];			/**<ԤԼ�ཻ������*/
	char sOrderSwitch[4];				/**<�����ཻ������*/
	char sOtherSwitch[4];				/**<������������*/
	char sTieTieSwich[4];				/**<����ҵ��������*/
	char cIsOfflineSendTimes;			/**<���߽������ʹ���,δ���*/
	char cPntFontSize;					/*��ӡ�����С*/
	char cSafeKeepYear;					/*ǩ������������*/
	char cIsPrintMinus;					/*�����Ƚ����Ƿ��ӡ����1��ӡ��0����ӡ*/
	char cIsPrintAllTrans;				/*���㵥��ϸ��ӡ��1��ӡ���н��ף�0����ӡ��Ч���� */
	char szPrintUnknowBankInfo[20+1];	/*δ֪�����д����ӡ��ʽ��Ϊ��ʱ��ӡ���룬�����ӡ��������*/
	
	char cIsSupportElecSign;			/**�Ƿ�֧�ֵ���ǩ��*/
	char cElecSendTime;					/**<����ǩ���ظ����ʹ���*/
	char szElecTimeOut[3+1];			/**<�ȴ�ǩ�����볬ʱʱ��*/
	
	char cIsRfSearchCardDelay;            /**<*�ǽ�Ѱ���ӳٱ�־*/
	char szRfSearchDelayTimeMs[3+1];	/**<�ǽ�Ѱ����ʱʱ��*/
	char szReserve1[6+1];				/**<������1*/
	
	char szEncryptPinKey[24+1];			/*���ܵ�pin key*/
	char szEncryptTrk[24+1];			/*���ܵ�TRK key*/
	char szMaxOffSendNum[2 + 1];		/*�������ͱ���*/
	char szAdInfo[60 + 1];				/*0.88����,TMS�·��Ĺ����Ϣ*/
	int nSettleNum;						/*�������*/

	/*����PBOC3.0���� ��ռ1�ֽڣ�Ԥ��ȥ��1��*/
	char cIsPreauthShieldPan;			/**����ϵͳ��������:Ԥ��Ȩ�����Ƿ����ο���*/
	char cIsReprintSettle;				/**�Ƿ��ش�ӡ���㵥*/
	char cIsUseLbs;						/*�Ƿ�ʹ��LBS��վ��λ*/
	int nLbsNum;						/*LBS��վ��Ϣ���ͼ������*/
	char cIsOutsideScaner;				/**<���û�����ɨ��ǹ*/
	char cIsHaveScanner;					/**<�Ƿ�֧��SCANER*/
	char szReserve2[36];				/**<������2*/
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

