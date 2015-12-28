/**
* @file manage.c
* @brief 系统管理处理模块
* @version  1.0
* @author 薛震
* @date 2007-03-07
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"


static void ClearRevFlag(void);
static void ClearFlag(void);
static void PosAdm(void);
static void OtherFunc(void);
static void ChgPwdMenu(void);
static int MainkeyManage(void);
static int PosPrintSet(void);
static int PrintManage(void);
static int SymtemManage(void);
static int MerchantManage(void);
#if defined (SUPPORT_ELECSIGN)
static int ElectSet(void);
#endif

#if 0
static void TxnFuncSet(void);
static int ResendManage(void);
static int TxnSwipeCard(void);
static int TxnInputPin(void);
static int TxnOffline(void);
#endif

/**
* @brief 清除冲正标志函数，有冲正次数，一并清0
* @param in 无
* @return 无
 */
void ClearRevFlag(void)
{
	STREVERSAL stReversal;
	
	SetVarIsReversal(NO);
	SetVarHaveReversalNum(0);
	
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	PubMsgDlg("清冲正标志", "冲正标志清除成功", 0, 1);

	return;
}

/**
* @brief 		清除标志,菜单函数
* @param in 	无
* @return 	无
*/
void ClearFlag()
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList("1.清冲正标志 ||2.清锁定标志||3.置未签到状态||4.置已签到状态||5.清脚本标志", "清除标志", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
			return;

		switch(nSelect)
		{
		case 1: 
			nRet = PubConfirmDlg("清冲正标志", "是否清除冲正标志", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("清冲正流水", MANAGEROPER))
				{
					ClearRevFlag(); 
				}
			}
			break;
		case 2: 
			nRet = PubConfirmDlg("清锁定标志", "是否清锁定标志  ", 0, 0);
			if( nRet == APP_SUCC )
			{
				SetVarIsLock(NO);
			}
			break;
		case 3: 
			nRet = PubConfirmDlg("置未签到状态", "是否置未签到状态", 0, 0);
			if( nRet == APP_SUCC )
			{
				SetVarIsLogin(NO);
			}
			break;
		case 4:
			nRet = PubConfirmDlg("置已签到状态", "是否置已签到状态", 0, 0);
			if( nRet == APP_SUCC )
			{
				char szNowDateTime[14+1];

				SetVarIsLogin(YES);
				PubGetCurrentDatetime(szNowDateTime);
				SetVarLastLoginDateTime(szNowDateTime + 4);
			}
			break;
		case 5:
			if (YES == GetVarEmvIsScriptInform())
			{
				nRet = PubConfirmDlg("清脚本标志", "是否清除脚本上送通知标志", 0, 0);
				if( nRet == APP_SUCC )
				{
					SetVarEmvIsScriptInform(NO);
				}
			}
			else
			{
				PubMsgDlg("清脚本标志", "无脚本通知", 0, 1);
			}
			break;
		default:                 
			break;
		}
	}
}

/**
* @brief  终端管理模块,菜单函数
* @param in 无
* @return     无
*/
void PosAdm(void)
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList("1.清冲正流水||2.清交易流水", "清除交易流水", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}
		switch(nSelect)
		{
		case 1:
			nRet = PubConfirmDlg("清冲正流水", "是否清除冲正流水", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("清冲正流水", MANAGEROPER))
				{
					ClearRevFlag(); 
				}
			}
			break;
		case 2:
			nRet = PubConfirmDlg("清交易流水", "是否清除交易流水", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("清交易流水", MANAGEROPER))
				{
					PubMsgDlg("清流水","正在清除流水...", 0, 1);/**ME31清流水要老半天*/
					InitWaterFile();
					ClearSettle();
				#if defined(EMV_IC)
					EmvClearWater();
				#endif
					SetVarOfflineUnSendNum(0);
				#if defined (SUPPORT_ELECSIGN)
					SetVarElecSignUnSendNum(0);
				#endif
					PubMsgDlg("清交易流水", "流水清除成功", 0, 1);
				}
			}
			break;
		/*case 3:
			PubDeReclFile(SETTLEFILENAME);
			PubMsgDlg("清历史结算数据", "\n历史结算数据清除完毕", 1, 5);
			break;*/
		default :
			break;
		}
	}
}


/**
* @brief  其他功能的各项菜单
* @param in 无
* @return     无
*/
void OtherFunc(void)
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
#if defined (SUPPORT_ELECSIGN)	
		nRet = ProSelectList("1.清除交易流水||2.电子签名设置||3.LBS定位", "其他功能", nSelect, &nSelect);
#else
		nRet = ProSelectList("1.清除交易流水||2.LBS定位", "其他功能", nSelect, &nSelect);
#endif

		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}

		switch(nSelect)
		{
		case 1:
			PosAdm();		
			break;
#if defined (SUPPORT_ELECSIGN)
		case 3:
			ElectSet();
		case 4:
			Ums_MenuLbs();
			break;	
#else
		case 3:
			Ums_MenuLbs();
			break;	
#endif
		default:                         
			break;
		}
	}
}

/**
* @brief 交易参数设置功能的各项菜单
* @param in 无
* @return     无
*/
//贴卡无相关相应交易设置,先不开放相应交易
/*		
		nRet = ProSelectList("1.交易开关控制||2.交易输密控制||3.交易刷卡控制||4.结算交易控制||5.离线交易控制||6.重发次数控制||7.其他交易控制", "交易控制", nSelect, &nSelect);

		case 2:
			TxnInputPin();
			break;
		case 3:
			TxnSwipeCard();
			break;
		case 5:
			TxnOffline();
			break;
		case 6:
			ResendManage();
			break;
		case 7:
			{
				int (*lSetFuns[])(void) = {
					SetFunctionIsManagePinInput, 
					SetFunctionIsCardInput, 
					SetFunctionDefaultTransType, 
					SetFunctionMaxRefundAmount,
					SetFunctionIsSmallGeneAuth,
					SetFunctionIsTipFlag,
					SetFunctionAuthsaleMode,
					NULL};
					PubUpDownMenus(lSetFuns);
			}
			break;	



void TxnFuncSet(void)
{
	int nRet;
	int nSelect=0xff;
	
	while (1)
	{
		nRet = ProSelectList("1.交易开关控制||2.结算交易控制", "交易控制", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}
		switch(nSelect)
		{
		case 1:
			SetFunctionTieTieSwitch();
			break;
		case 2:
			{
				int (*lSetFuns[])(void) = {
					SetFunctionAutoLogout, 
					NULL};
					PubUpDownMenus(lSetFuns);
			}
			break;	

		default:
			break;
		}
	}
}
*/	

/**
* @brief 修改管理密码菜单功能
* @param in 无
* @return     无
*/
void ChgPwdMenu(void)
{
	int	nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList( "1.系统管理密码||2.安全密码", "密码管理",  nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}
		
		switch(nSelect)
		{
		case 1:
			GetCurrentOper(NULL, NULL, &nRet);
			ChgOperPwd(nRet);
			break;
		case 2:
			ChangeSecurityPwd();
			break;
		default:
			break;
		}
	}
}


/**
* @brief 隐藏管理模块,菜单函数
* @param in 无
* @return     无
*/
void HideManage(void)
{   
	int	nRet;
	int nSelect = 0xff;
	char szFunKey[2+1]={0};
	szFunKey[0] = '9'; //隐藏菜单键 9
	
 	while(1)
	{				
		nRet = PubSelectListItemExtend("1.管理密码重置||2.初始化POS参数||3.清标志", "厂商管理", NULL, 0xFF, &nSelect, 60, szFunKey);
		if (nRet==APP_QUIT)
		{
			if (APP_SUCC == PubConfirmDlg("厂商管理", "退出厂商管理", 0, 30))
				return;
			continue;	
		}
		if (nRet == APP_TIMEOUT)
		{
			continue;
		}
		
		switch(nSelect)
		{
		case 1:
			if (APP_SUCC == PubConfirmDlg("管理密码重置", "重置系统管理密码", 0, 60))
			{
				SetAdminPwd(NULL);	
				PubMsgDlg("管理密码重置", "密码重置成功", 0, 1);
			}
			break;
		case 2:
			if (APP_SUCC == PubConfirmDlg("初始化POS参数", "按确认键开始", 0, 0))
			{
				PubMsgDlg("初始化POS参数", "正在清除\n     请稍侯...", 0, 1);
				InitCommParam();
				InitWaterFile();			
				InitOper();
				ClearSettle();						/**<清除结算数据*/
				PubDeReclFile(SETTLEFILENAME);
			#if defined(EMV_IC)
				InitEMVParam();
				EmvClearWater();
			#endif
				InitPosDefaultParam();
				PubMsgDlg("初始化POS参数", "初始化成功", 0, 1);
				CommInit();			
			}
			break;				
		case 3:
			ClearFlag();
			break;
		default:			
			nSelect -= (0xFF + 0x30);
			if (nSelect == 9)
            {
                if ((PubGetKeyCode(1) == KEY_8)
					&&(PubGetKeyCode(1) == KEY_7)
					&&(PubGetKeyCode(1) == KEY_2))
				{
					SetFunctionIsModification();
				}
            }
			break;
		}
 	}
}

/**
* @brief 管理模块,菜单函数
* @param in 无
* @return     无
*/
/*

*/
void Manage(void)
{   
	int	nRet;
	int nSelect = 0xff;

 	while(1)
	{
		nRet = ProSelectList( "1.商户参数设置||2.系统参数设置||3.通讯参数设置||4.终端密钥管理||5.密码管理||6.打印管理设置||7.其他功能设置", "系统管理", nSelect, &nSelect);
		if (nRet==APP_QUIT)
		{
			if (APP_SUCC == PubConfirmDlg("系统管理", "退出系统管理", 0, 30))
				return;
			continue;		
		}
		if (nRet == APP_TIMEOUT)
		{
			continue;
		}
		
		switch(nSelect)
		{		
		case 1:
			MerchantManage();
			break;
#if 0
		case 2:
			TxnFuncSet();
			break;
#endif			
		case 2:
			SymtemManage();
			break;		
		case 3:
			CommMenu();	
			CommInit();
			break;
		case 4:
			MainkeyManage();
			break;
		case 5:
			if(GetVarIsModification() == NO)
			{
				PubMsgDlg("系统管理","不允许手动设置",3,5);
			}
			else
			{
				ChgPwdMenu();
			}
			break;
		case 6:
			PrintManage();
			break;
		case 7:
			OtherFunc();
			break;
		default:
			break;
		}
	}
}


int ChkPinpad(void)
{
	int nRet = 0;
	int nPinpadAux = PORT_NUM_COM2;
	int nPinpadTimeout = 120;
	int nMode = SECRITY_MODE_INSIDE;
	STPINPADPARAM stPinpadParam = 
	{	
		PORT_NUM_COM2,
		60,
		0x00,
		0x01,
		0x01,
		0x04,
		"                ",/**<仅对证通的密码键盘有用*/
		"",
		PINPAD_HY,/**PINPAD_HY,*/
		1,
		0,
		0,
		1,
		0,
		""
	};


	if((GetVarIsPinpad() == NO) && (GetControlChkInside() == YES))
	{
		SetControlChkPinpad(YES);
		nMode = SECRITY_MODE_INSIDE;
		//传入内置密码键盘超时时间
		stPinpadParam.cTimeout = DEFAULT_PIN_INPUT_TIME;
		nRet = PubInitSecrity(nMode, &stPinpadParam);
		if (APP_SUCC != nRet)
		{
			PubMsgDlg("设备检测", "加密模块加载有错", 3, 10);
			return APP_FAIL;
		}
		Ums_SetMainKeyFromMC();
		SetControlChkInside(NO);
	}
	else if ((GetVarIsPinpad() == YES) && (GetControlChkPinpad() == YES))
	{
		SetControlChkInside(YES);
		PubClearAll();
		PubDisplayStrInline(0, 2, "加载密码键盘");
		PubDisplayStrInline(0, 4, "请稍候...");
		PubUpdateWindow();

		nMode = SECRITY_MODE_PINPAD;
		
		nRet = GetVarUID(stPinpadParam.szUid);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("设备检测", "密码键盘用户ID没有设置", 3, 10);
			return APP_FAIL;
		}
		
		GetVarPinpadAuxNo(&nPinpadAux);
		stPinpadParam.cAux = nPinpadAux;
		GetVarPinpadTimeOut(&nPinpadTimeout);
		stPinpadParam.cTimeout = nPinpadTimeout;
		stPinpadParam.cAppDirID = 1;

		nRet = PubInitSecrity(nMode, &stPinpadParam);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("设备检测", "密码键盘未接", 3, 10);
			
			return APP_FAIL;
		}
		SetControlChkPinpad(NO);
	}
	
	return APP_SUCC;
}

int ChkRF(void)
{
	return APP_SUCC;
}

/**
* @brief 终端密钥管理功能菜单
* @param in 无
* @return     无
*/
int MainkeyManage(void)
{
	int	nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList( "1.清空密钥||2.KEK下载||3.密码键盘设置||4.设置主密钥索引||5.设置DES算法", "密钥管理",  nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return nRet;
		}
		switch(nSelect)
		{
		case 1:
			if( YES != GetVarIsPinpad())
			{
				nRet = PubConfirmDlg("密钥管理", "是否清空主密钥?", 0, 0);
				if (nRet == APP_SUCC)
				{
					PubClearKey();
				}
			}
			else
			{
				PubMsgDlg("密钥管理", "外接密码键盘不支持此项操作", 0, 10);
			}
			break;
		case 2:
			DownloadKek();
			break;
		case 3:
			SetFunctionIsPinPad();
			break;
		case 4:
			SetFunctionMainKeyNo();
			break;
		case 5:
			{	
				int (*lSetFuns[])(void) = {
					SetFunctionDesMode, 
					SetFunctionIsEncryptTrack,
					NULL
				};
				PubUpDownMenus(lSetFuns);
			}
			break;
		default:
			break;
		}
	}
	return APP_SUCC;
}

#if defined(EMV_IC)
/**
* @brief 	设置密码键盘的脱机密码加密密钥该密钥用于pinpad端加密pos机端解密
* @param in psKeyBuf 如为NULL，要求手工输入,不为NULL,具体的密钥指针
* @param in nKeyIndex 具体存放的密钥索引号
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetPinpadKey(char *psKeyBuf, int nKeyIndex)
{
	int nRet = APP_SUCC;
	int nPort = PORT_NUM_COM2;
	int nLen;
	char szAscMainKey[32+1], szBcdMainKey[16+1];
	char szMainKey[16+1];
	char *pszTitle = "密钥管理";

	if (YES == GetVarIsPinpad())
	{
		GetVarPinpadAuxNo(&nPort);
	}
	else
	{
		return APP_SUCC;
	}
	nKeyIndex = nKeyIndex >= 0x30 ? nKeyIndex-0x30: nKeyIndex;
	if (NULL == psKeyBuf)
	{
		nLen = 16;
		memset(szAscMainKey, 0, sizeof(szAscMainKey));	
		ASSERT_QUIT(PubInputDlg(pszTitle, "请输入IC卡脱机明文PIN加密密钥", szAscMainKey, &nLen, nLen, nLen, 0, INPUT_MODE_STRING));
		PubMsgDlg(NULL, "正在写入密钥", 0, 1);
		PubAscToHex((uchar *)szAscMainKey, 16, 0, (uchar *)szBcdMainKey);
	}
	else
	{
		memcpy(szBcdMainKey, psKeyBuf, 8);
	}
	PubSoftDes(szBcdMainKey, szBcdMainKey, szMainKey);/**<计算加密过的工作密钥*/
	nRet = PubLoadMainKey(nKeyIndex, szBcdMainKey, 8);/**<PINPAD上保存该密钥作为主密钥*/
	PubSetCurrentMainKeyIndex(nKeyIndex);
	nRet += PubLoadWorkKey(KEY_TYPE_PIN, szMainKey, 8, NULL);/**<PINPAD上保存工作密钥(密文)*/
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "密码键盘安装IC卡脱机明文PIN 加密密钥失败", 3, 10);
	}
	return nRet;
}
#endif

#if defined (SUPPORT_ELECSIGN)

/**
* @brief 		电子签名设置
* @param in 	
* @return 	无
*/
static int ElectSet(void)
{
	int nSelect=0xff;

	if (APP_SUCC != SetFunctionSupportElecSign())
	{
		return APP_SUCC;
	}

	if (YES != GetVarSupportElecSign())
	{
		return APP_SUCC;
	}

	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.签字超时时间||2.签名确认设置||3.分包传输设置", "电子签名设置", nSelect, &nSelect));

		switch(nSelect)
		{
		case 1: 
			SetFuncElecTimeOut();
			break;
		case 2:
			SetFuncExElecConfirm();
			break;
		case 3:
			SetFuncElecSubcontract();
			break;
		default  :                 
			break;
		}
	}
}

#endif


static int PosPrintSet()
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.签购单抬头||2.打印张数||3.服务热线||4.其他打印设置", "打印", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionPrintTitleMode();
			break;
		case 2:
			SetFunctionPrintPageCount();
			break;
		case 3:
			SetFunctionHotLineNo();
			break;
		case 4:
			{
				int (*lSetFuns[])(void) = {					
					SetFunctionPrintSize,
					SetFunctonIsTickeWithEn,
					SetFunctionIsPrintWaterRec,				   	
				   	SetFunctionReprintSettle,   	
					SetFunctionSafeKeep,
					SetFunctionIsPrintAllTrans,				   	
				   	SetIsPntChAquirer,
				   	SetIsPntChCardScheme,
				   	SetFunctionIsNewTicket,				   
			    #if !defined(USE_TMS)
				   	SetFunctionIsPrintPrintMinus,
			       	SetFunctionUnknowBankInfo,
			    #endif
					
				   NULL};
				PubUpDownMenus(lSetFuns);	
			}
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;
}

/**
* @brief  终端管理模块,签购单打印设置
* @param in 无
* @return     无
*/
static int PrintManage(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.打印设置||2.参数打印", "打印管理设置", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			PosPrintSet();
			break;
		case 2:
			PrintParam();
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;
}

static int SymtemManage(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.流水号设置||2.批次号设置||3.最大交易笔数||4.系统当前时间||5.条码枪设置", "系统参数设置", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionTraceNo();
			break;
		case 2:
			SetFunctionBatchNo();
			break;
		case 3:
			SetFunctionMaxTransCount();
			break;
		case 4:
			SetFunctionPosDateTime();
			break;
		case 5:
			SetFunctionIsHaveScanner();
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;
}

static int MerchantManage(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.商户号设置||2.终端号设置||3.商户中文名称||4.商户英文名称||5.应用显示名称", "商户参数设置", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionMerchantId();
			break;
		case 2:
			SetFunctionTerminalId();
			break;
		case 3:
			SetFunctionMerchantName();
			break;
		case 4:
			SetFunctionMerchantNameEn();
			break;
		case 5:
			SetFunctionAppName();
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;
}

#if 0
static int ResendManage(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.离线重发次数||2.冲正重发次数||3.签名重发次数||4.TMS重试次数", "重发次数设置", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionOffResendNum(); 
			break;
		case 2:
			SetFuncCommReSendNum();
			break;
		case 3:
			#if defined (SUPPORT_ELECSIGN)	
				SetFunctionElecSendTime();
			#else
				PubMsgDlg("温馨提示", "暂不支持该功能", 0, 2);	
			#endif
			break;
		case 4:
			#ifdef USE_TMS
				SetFuncTmsReSendNum();
			#else
				PubMsgDlg("温馨提示", "暂不支持该功能", 0, 2);	
			#endif
			break;
		default:                         
			break;
		}
	}

	return APP_SUCC;
}

static int TxnSwipeCard(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.消费撤销刷卡||2.授权完成撤销刷卡", "交易刷卡控制", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionIsSaleVoidStrip();
			break;
		case 2:
			SetFunctionIsAuthSaleVoidStrip();
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;
}

static int TxnInputPin(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.消费撤销密码||2.授权撤销密码||3.授权完成撤销密码||4.授权完成请求密码", "交易输密控制", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionIsVoidPin();
			break;
		case 2:
			SetFunctionIsPreauthVoidPin();
			break;
		case 3:
			SetFunctionIsAuthSaleVoidPin();
			break;
		case 4:
			SetFunctionIsAuthSalePin();
			break;
		default:                         
			break;
		}
	}
	return APP_SUCC;

}

static int TxnOffline(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.离线自动上送笔数||2.离线上送方式", "离线交易控制", 0xFF, &nSelect));
		switch(nSelect)
		{
		case 1:
			SetFunctionMaxOffSendNum();
			break;
		case 2:
			SetFunctionIsOfflineSendNow();
			break;		
		default:                         
			break;
		}
	}
	return APP_SUCC;

}
#endif

