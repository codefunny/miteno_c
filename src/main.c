/**
* @file main.c
* @brief 主模块（函数入口）
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#define _NO_EXTERN_GLOBAL_VAR_

#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
/**
* 内部函数声明
*/
static int AppTrans(void);
static int MenuFuncSel(void);
static int MenuManage(void);
static int MenuTransFind(void);
static int MenuTransLogin(void);
static int FirstRunChk(void);
static int AppInit(void);
static int ChkPdAndRF(void);
static int DoPosLockAndUnLock(void);
static int ChkIsMCReturn(void);
static void SaveMCEnterInfo(void);


static int RecordVersion(void);
#if 0
static void GotoSleepApp(void);
#endif

/**
* @brief 程序入口
* @param 无
* @return 返回
*/
int main(void)
{
	int nOperRole = 0xff;
	char szOper[OPERCODELEN + 1];

	PubInitUiParam();
	ASSERT_QUIT(Ums_CheckMcVer());	/*重要:判断主控版本*/
	PubLuaOpen();
	PubLuaDisplay("LuaSetUIStyle");	/*设置系统风格*/
	RecordVersion();	/*记录版本信息*/
	PubKbHit();						/*清除按键缓*/

	/**
	* 版本校验(首次运行检测)
	*/
	FirstRunChk();
	/**
	* 模块初始化,重要!!!
	*/
	AppInit();

#if defined(USE_TMS)
	/**<检查是否有应用或者参数的更新*/
	DoTmsTask(TMS_POS_CHKUPDATA);
	GetTmsParamDown();
#endif

	Cup_InitScrLed();

	SetControlChkPinpad(YES);

	
	/*
	* 运行任务
	*/
	while(1)
	{
		/**
		* 设备检查
		*/
		ChkPdAndRF();

		/**
		* 通讯初始化
		*/
		CommInit();
		memset(szOper, 0, sizeof(szOper));
		GetWaterOper(szOper);/**<取做过交易的(有流水)操作员号码*/
		SetOperLimit(szOper);

		GetVarLastOperInfo(szOper, &nOperRole);/**<取最后登陆的操作员信息(保存的)*/
		SetCurrentOper(szOper, NULL, nOperRole);/**<将最后登陆的操作员信息设置给操作员模块*/

		if (ChkIsMCReturn() == APP_SUCC || nOperRole != NORMALOPER || YES != GetVarIsLogin())
		{
			if (OperLogon() != APP_SUCC)
			{
				/*取消已设置的系统风格*/
				PubLuaDisplay("LuaUnSetUIStyle");
				PubLuaClose();
				CommDump();
				Cup_ExitScrLed();
				return APP_QUIT;
			}
		}
		GetCurrentOper(szOper, NULL, &nOperRole);/**<从操作员模块获取当前的操作员信息*/
		SetVarLastOperInfo(szOper, (char)nOperRole);/**<保存最后登陆的操作员信息*/

		/**
		* 系统管理员
		*/
		if (nOperRole == ADMINOPER)
		{
			Manage();
			ParamSave();
			PubClearAll();
		}
		else if(nOperRole == HIDEMENUOPER)
		{
			HideManage();
		}
		else if(nOperRole == MANAGEROPER)
		{
			Operate();
		}
		else
		{
			
			if (APP_SUCC != AppTrans())
			{
				PubLuaDisplay("LuaUnSetUIStyle");
				PubLuaClose();
				CommDump();
				Cup_ExitScrLed();
				return APP_QUIT;
			}
		}
	}
}


/**
* @brief 首次运行检查
* @param 无
* @return
* @li APP_SUCC
*/
static int FirstRunChk(void)
{
	if (APP_SUCC != IsFirstRun())
	{
		ResetDefaultParam();
	}
	else   //版本升级,新增变量初始化
	{
		VerUpdateChk();
	}
	return APP_SUCC;
}


/**
* @brief 初始化系统
* @param
* @return
* @li APP_SUCC
*/
static int AppInit(void)
{
	int nRet = 0;

	PubClearAll();
	PubDisplayStrInline(0, 2, "加载中");
	PubDisplayStrInline(0, 4, "请稍候..");
	PubUpdateWindow();
	nRet = ExportPosParam();
	nRet += ExportCommParam();
	if (APP_SUCC != nRet)
	{
		PubMsgDlg("警告", "POS参数丢失", 3, 10);
	}

#if defined(EMV_IC)
	if (ExportEMVModule() != APP_SUCC)
	{
		return APP_FAIL;
	}
#endif
	if (APP_SUCC != PubInitPrint(PRINTFILE, DEFAULT_PAGE_LEN))
	{
		PubDispErr("初始化打印");
		return APP_FAIL;
	}
	SetParamFromIni();
	ExportSslCa();
	return nRet;
}

/**
* @brief 应用主要交易部分
* @param 无
* @return
* @li APP_SUCC
* @li APP_QUIT
*/
static int AppTrans(void)
{
	int nRet = 0;
	char cMenuSel = 0;			/*默认进入交易界面*/
	char szDateTime[14+1];
	char szLastLoginDateTime[5+1];
	
	/*判断是否下载KEK*/
	if (YES == GetVarDownKek())
	{
		nRet = DownloadKek();
		if (nRet != APP_SUCC)
		{
			return APP_FAIL;
		}
	}

	/**
	*POS签到
	*/
	PubGetCurrentDatetime(szDateTime);
	GetVarLastLoginDateTime(szLastLoginDateTime);
	if ((YES != GetVarIsLogin() || memcmp(szDateTime, szLastLoginDateTime, 4) != 0))
	{
		nRet = Login();
		if (nRet != APP_SUCC)
		{
			if (YES == GetVarFirstRunFlag())
			{
				CommHangUp();
			}
			return APP_FAIL;
		}
	}
	SaveMCEnterInfo();//保存主控时间因子和随机数
	DealSettleTask();//处理结算中断
	
	while(1)
	{
		/**
		* 处理系统要求
		*/
		DealSysReq();

		/**
		* 银商要求，TMS版应用结算次数达到TMS系统下发的累计结算次数就需要强制联机报道
		* 该功能仅针对，当前这个子应用是主应用(一般是金融应用，如银行卡)。
		*/
		if(Ums_DealSettleNum() == KEY_FUNC_APP_SELECT)
		{
			return APP_QUIT;
		}
		
		/*
		* 判断是否自动签退，自动签退成功侧要退出
		*/
		if (YES != GetVarIsLogin())
		{
			return APP_FAIL;/**<如果已经签退成功则退出*/
		}
		
		/**
		* 进入交易菜单
		*/

		
		if (cMenuSel == 0)
		{	
			EnableDispDefault();
			DisableDispDefault();
		}
		else
		{
			nRet = MenuFuncSel();
			if (nRet == KEY_FUNC_APP_SELECT)
			{
				return APP_QUIT;
			}
			if (nRet == APP_SUCC)
			{
				continue;
			}
		}
		cMenuSel = ~cMenuSel;
	}
	return APP_SUCC;
}

/*菜单对应的功能ID*/
enum TRANSMEUN
{
	PRINT_MENU,							/*打印*/
	MANAGE_MENU,						/*管理*/
	OTHERS,								/**<其他菜单*/
	REFUND_MENU,						/**<退货显示*/
	BALANCE_MENU,						/**<查询显示*/
	COUPON_DISP,						/**<验券*/
	COUPON_VERIFY_MENU,					/**<串码验券*/
	TEL_VERIFY_MENU,	     			/**<手机验券*/
	CARD_VERIFY_MENU,	       			/**<卡号验券*/
	VOID_VERIFY_MENU,					/**<撤销交易*/
	REFUND_VERIFY_MENU,		    		/**<验证退货*/
	WX_DISP,							/**<微信支付*/
	ALI_DISP,							/**<支付宝支付*/
	BAIDU_DISP,							/**<百度支付*/
	JD_DISP,							/**<京东支付*/
	PRECREATE_MENU,						/**<扫码支付*/
	CREATEANDPAY,						/**<条码支付*/
	WX_VOIDPAY_MENU, 					/**<微信撤销*/
	WX_REFUND_MENU,						/**<微信退货*/
	WECHAT_BALANCE_MENU,				/**<微信查询*/
	BAIDU_CREATEANDPAY_MENU, 			/**<百度支付*/
	BAIDU_REFUND_MENU,					/**<百度退货*/
	BAIDU_BALANCE_MENU,					/**<百度查询*/
	JD_PRECREATE_MENU,					/**<京东支付*/
	JD_CREATEANDPAY_MENU,				/**<京东支付主扫*/
	JD_REFUND_MENU,						/**<京东退货*/
	JD_BALANCE_MENU,					/**<京东查询*/
	ALI_CREATEANDPAY_MENU, 				/**<支付宝付款*/
	ALI_REFUND_MENU,					/**<支付宝退货*/
	ALI_BALANCE_MENU,					/**<支付宝查询*/
	MEITUAN_COUPON_MENU,				/**<美团*/
	PANJIN_MENU,						/**<盘锦通*/
	MEITUAN_BALANCE_MENU,				/**<美团查询*/
	DZ_MENU_DISP,						/**<大众点评主菜单*/
	DZ_BALANCE_MENU,					/**<大众点评查询*/
	DZ_PRECREATE_MENU,					/**<大众点评被扫*/
	DZ_CREATEANDPAY_MENU,				/**<大众点评主扫*/
	DZ_REFUND_MENU,						/**<大众点评退货*/
	DZ_COUPON_MENU,						/**<大众点评兑券*/
	DZ_COUPONCONFIRM_MENU,				/**<大众点评订单确认*/
	BESTPAY_CREATEANDPAY_MENU, 			/**<翼支付付款*/
	BESTPAY_REFUND_MENU,				/**<翼支付退货*/
	BESTPAY_BALANCE_MENU,				/**<翼支付查询*/
	ALLPAY_CREATEANDPAY_MENU, 			/**<都能付付款*/
	ALLPAY_REFUND_MENU, 				/**<都能付退款*/
	ALLPAY_BALANCE_MENU, 				/**<都能付查询*/
	ALIPAY_PRECREATE_MENU,				/**<支付宝被扫*/
	BAIDU_PRECREATE_MENU,				/**<百度被扫*/
	
};

//平台被扫菜单
static int DoPREPlatformMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "平台被扫,%d/", WX_DISP);
	strcat(pszMenuResource, szTmp);

	//if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****微信支付,%d/", PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	//if(GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****支付宝,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****百度钱包,%d/", BAIDU_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****京东钱包,%d/", JD_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****大众点评,%d/", DZ_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
}

//平台主扫菜单
static int DoPlatformMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "平台主扫,%d/", WX_DISP);
	strcat(pszMenuResource, szTmp);

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****微信支付,%d/", CREATEANDPAY);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****支付宝,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****百度钱包,%d/", BAIDU_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****京东钱包,%d/", JD_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****大众点评,%d/", DZ_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****翼支付,%d/", BESTPAY_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
}



static int DoWechatMenu(char * pszMenuResource)
{
	char szTmp[128];
#if 0
	if ((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== NO))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "微信支付,%d/", PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "微信支付,%d/", CREATEANDPAY);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES))
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "微信支付,%d/", WX_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****微信支付主扫,%d/", CREATEANDPAY);
			strcat(pszMenuResource, szTmp);
		}
		if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****微信支付被扫,%d/", PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "微信支付,%d/", CREATEANDPAY);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "微信支付,%d/", PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif
	return APP_SUCC;
}


static int DoAliPayMenu(char * pszMenuResource)
{
	char szTmp[128];
#if 0
/*
	if ((GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== YES) && 
		(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== NO))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "支付宝,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "支付宝,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES))
*/		
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "支付宝,%d/", ALI_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****支付宝主扫,%d/", ALI_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****支付宝被扫,%d/", ALIPAY_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "支付宝,%d/", ALI_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "支付宝,%d/", ALIPAY_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif
	return APP_SUCC;
}




static int DoBaiduMenu(char * pszMenuResource)
{
	char szTmp[128];
#if 0
/*
	if ((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== NO))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "百度钱包主扫,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "百度钱包被扫,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES))
*/		
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "百度钱包,%d/", BAIDU_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****百度钱包主扫,%d/", BAIDU_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****百度钱包被扫,%d/", BAIDU_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "百度钱包,%d/", BAIDU_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "百度钱包,%d/", BAIDU_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif
	return APP_SUCC;
}


static int DoJDMenu(char * pszMenuResource)
{
	char szTmp[128];

#if 0
	if ((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES) && 
		(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== NO))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "京东钱包,%d/", JD_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "京东钱包,%d/", JD_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES))
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "京东钱包,%d/", JD_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****京东钱包主扫,%d/", JD_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****京东钱包被扫,%d/", JD_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else

	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "京东钱包,%d/", JD_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "京东钱包,%d/", JD_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif

	return APP_SUCC;
}


static int DoCouponMenu(char * pszMenuResource)
{
	char szTmp[128];

	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "验  券,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****串码验券,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****手机验券,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****卡号验券,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "验  券,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****串码验券,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****手机验券,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "验  券,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****手机验券,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****卡号验券,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "验  券,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****串码验券,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****卡号验券,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else
	{
		if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "串码验券,%d/", COUPON_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		
		if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES)
		{	
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "手机验券,%d/", TEL_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "卡号验券,%d/", CARD_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}

	return APP_SUCC;
}


static int DoVoucherMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "兑  券,%d/", COUPON_DISP);
	strcat(pszMenuResource, szTmp);
	
	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****美  团,%d/", MEITUAN_COUPON_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****大众兑券,%d/", DZ_COUPON_MENU);
		strcat(pszMenuResource, szTmp);
	}
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****大众订单确认,%d/", DZ_COUPONCONFIRM_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****串码验券,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES)
	{	
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****手机验券,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****卡号验券,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}

	return APP_SUCC;
}



/**
* @brief 处理菜单
* @param [out] pszMenuResource 菜单列表
* @param [out] pcMode 应用类型
* @return
* @li APP_SUCC
* @li APP_FAIL
* @author 
* @date
*/
static int DoMenu(char * pszMenuResource)
{
	char szTmp[128];
	//平台被扫菜单定制
	DoPREPlatformMenu(pszMenuResource);
	//平台主扫菜单定制
	DoPlatformMenu(pszMenuResource);
	
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "查  询,%d/", ALLPAY_BALANCE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "退  货,%d/", ALLPAY_REFUND_MENU);
		strcat(pszMenuResource, szTmp);
	}
#if 0
	//微信支付菜单定制
	DoWechatMenu(pszMenuResource);

	//支付宝菜单定制
	DoAliPayMenu(pszMenuResource);

	//百度钱包菜单定制
	DoBaiduMenu(pszMenuResource);

	//京东钱包
	DoJDMenu(pszMenuResource);
#endif


#if 0	
	//if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "大众点评,%d/", DZ_MENU_DISP);
		strcat(pszMenuResource, szTmp);
		
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****大众兑券,%d/", DZ_COUPON_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****大众订单确认,%d/", DZ_COUPONCONFIRM_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****大众点评主扫,%d/", DZ_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****大众点评被扫,%d/", DZ_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif

	
	//验券
	//DoCouponMenu(pszMenuResource);

	//兑卷
	DoVoucherMenu(pszMenuResource);

	
	/*打印*/
	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "打  印,%d/", PRINT_MENU);
	strcat(pszMenuResource, szTmp);

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "其  他,%d/", OTHERS);
	strcat(pszMenuResource, szTmp);

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "管  理,%d/", MANAGE_MENU);
	strcat(pszMenuResource, szTmp);

	return APP_SUCC;
}



int DoOtherMenu(char * pszMenuResource)
{
	char szTmp[128];

	
	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "盘锦通,%d/", PANJIN_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	//查询
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES
		|| GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES
		)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "交易查询,%d/", REFUND_MENU);
		strcat(pszMenuResource, szTmp);
	
		if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****微信查询,%d/", WECHAT_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****支付宝,%d/", ALI_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****百度查询,%d/", BAIDU_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****京东查询,%d/", JD_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	
		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****美团查询,%d/", MEITUAN_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
		

#if 0


		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****点评查询,%d/", DZ_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
#endif		
	}

	if(GetTieTieSwitchOnoff(TRANS_VOID_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "撤  销,%d/", VOID_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if( GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES
		|| GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES
		|| GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES
		|| GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES
		|| GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES
		)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "退  货,%d/", REFUND_MENU);
		strcat(pszMenuResource, szTmp);

		if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****券退货,%d/", REFUND_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****微信退货,%d/", WX_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****百度退货,%d/", BAIDU_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****京东退货,%d/", JD_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****支付宝退货,%d/", ALI_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****翼支付退货,%d/", BESTPAY_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
#if 0	

		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "*****点评闪惠退货,%d/", DZ_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
		
	
		//if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****点评兑券退货,%d/", DZ_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
#endif		

	}


	return APP_SUCC;
}


/**
* @brief 交易类菜单，其他子菜单类的菜单选择
* @param 无
* @return 
* @li APP_SUCC
* @li APP_FAIL
* @author 
* @date 
*/
static int MenuOther(void)
{
	int nRet = 0;
	char szMenuResource[2048];
	char szPrm[2048];
	
	memset(szMenuResource, 0, sizeof(szMenuResource));
	DoOtherMenu(szMenuResource);
	
	sprintf(szPrm, "'%s'", szMenuResource);
	while(1)
	{
		nRet = PubLuaDisplay2("LuaMenuFunc", szPrm);
		
		switch(nRet)
		{
		case APP_FUNCQUIT:
			return KEY_FUNC_APP_SELECT;
		case -5://确认
		case APP_TIMEOUT:
		case APP_QUIT:
			return nRet;
		case VOID_VERIFY_MENU:
			VoidSale();
			break;
		
		case REFUND_VERIFY_MENU:
			MagRefund();
			break;
		case WX_REFUND_MENU:
			MagBarCodeRefund();
			break;
		case ALI_REFUND_MENU:
			MagBarAliCodeRefund();
			break;
		case BAIDU_REFUND_MENU:
			MagBarCodeRefundBaidu();
			break;
		case JD_REFUND_MENU:
			MagBarJDCodeRefund();
			break;
		case DZ_REFUND_MENU:
			MagBarDazhongCodeRefund();
			break;
		case BESTPAY_REFUND_MENU:
			MagBarBestpayCodeRefund();
			break;
		//查询
		case WECHAT_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_WECHAT);
			break;
		case BAIDU_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_BAIDU);
			break;
		case JD_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_JD);
			break;
		case ALI_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_ALIPAY);
			break;
		case MEITUAN_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_MEITUAN);
			break;
		case DZ_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_DZ);
			break;
		case PANJIN_MENU:
			MagPanjintong();
			break;
		default:
			break;
		}
	}

	return APP_SUCC;
}


#if 0

/**
* @brief 应用交易菜单选择
* @param 无
* @return
* @li APP_SUCC
* @li APP_QUIT
* @li APP_TIMEOUT
*/
static int MenuFuncSel(void)
{
	int nTrueInput = INPUT_NO;
	int nSelect = 0;
	
	while(1)
	{
		//PubCloseShowStatusBar();
		nTrueInput = INPUT_NO;
		nSelect = PubLuaDisplay("LuaMenuFuncSel");	
		//PubShowStatusBar();
		switch(nSelect)
		{
		case KEY_ESC:
			return APP_QUIT;
		case KEY_FUNC_APP_SELECT:
			return KEY_FUNC_APP_SELECT;
		case 0:
		case KEY_ENTER:
			return APP_TIMEOUT;
			break;
		case KEY_1:
			MagTelnoVeriCoupon(&nTrueInput);
			break;
		case KEY_2:
			MagIDVeriCoupon(&nTrueInput);
			break;
		case KEY_3:
			MagCardVeriCoupon(&nTrueInput);
			break;
		case KEY_4:
			VoidSale();
			break;
		case KEY_5:
			MagRefund();
			break;
		case KEY_6:
			Reprint();
			return APP_SUCC;
			break;
		case KEY_7:
			MenuManage();
			return APP_SUCC;
			break;
		default:
			break;
		}
	}
	return APP_SUCC;
}
#endif


/**
* @brief 应用交易菜单选择
* @param 无
* @return 
* @li APP_SUCC
* @li APP_QUIT
* @li APP_TIMEOUT
* @author 
* @date
*/
static int MenuFuncSel(void)
{
	int nRet = 0;
	char szMenuResource[3072];
	char szPrm[3072];
	
	memset(szMenuResource, 0, sizeof(szMenuResource));
	DoMenu(szMenuResource);
	
	sprintf(szPrm, "'%s'", szMenuResource);
	while(1)
	{
		AutoDoReversal();	/*自动冲正*/
		nRet = PubLuaDisplay2("LuaMenuFunc", szPrm);
		
		switch(nRet)
		{
		case APP_FUNCQUIT:
			return KEY_FUNC_APP_SELECT;
		case -5://确认
		case APP_TIMEOUT:
		case APP_QUIT:
			return nRet;
		case COUPON_VERIFY_MENU:
			MagIDVeriCoupon();
			break;
		case TEL_VERIFY_MENU:
			MagTelnoVeriCoupon();
			break;
		case CARD_VERIFY_MENU:
			MagCardVeriCoupon();
			break;
		case PRECREATE_MENU:
			MagScanQrCodePay();
			break;
		case CREATEANDPAY:
			MagBarcodePay();
			break;
		case WX_VOIDPAY_MENU:
			break;
		case PRINT_MENU:
			Reprint();
			break;
		case MANAGE_MENU:
			MenuManage();
			return APP_SUCC;
			break;

		case BAIDU_PRECREATE_MENU:
			MagScanQrCodePayBaidu();
			break;
		case BAIDU_CREATEANDPAY_MENU:
			MagBarcodeBaidu();
			break;

		case JD_PRECREATE_MENU:
			MagJDScanQrCodePay();
			break;
		case JD_CREATEANDPAY_MENU:
			MagJDBarcode();
			break;

		case ALIPAY_PRECREATE_MENU:
			MagAlipayScanQrCodePay();
			break;
		case ALI_CREATEANDPAY_MENU:
			MagAliBarcode();
			break;
		
		case MEITUAN_COUPON_MENU:
			MagMeituanVeriCoupon();
			break;
			
		case DZ_PRECREATE_MENU:
			MagDazhongScanQrCodePay();
			break;
		case DZ_CREATEANDPAY_MENU:
			MagDazhongBarcode();
			break;
			
		case DZ_COUPON_MENU:
			MagDazhongVeriCoupon();
			break;
		case DZ_COUPONCONFIRM_MENU:
			MagDazhongConfirm();
			break;
			
		case BESTPAY_CREATEANDPAY_MENU:
			MagBestpayBarcode();
			break;
			
		case ALLPAY_CREATEANDPAY_MENU:
			MagAllpayBarcode();
			break;
		case OTHERS:
			MenuOther();
			break;		
		case ALLPAY_REFUND_MENU:
			MagAllPayRefund();
			break;		
		case ALLPAY_BALANCE_MENU:
			MagBalance(TRANS_BALANCE_ALLPAY);
			break;	
		default:
			break;
		}
	}
	return APP_SUCC;
}


/**
* @brief 进行结帐处理(有判断操作员的权限)
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int DoSettle(void)
{
	int nWaterSum = 0;
	int nRet = 0;

	if (APP_SUCC != ChkOperRole())
	{
		return APP_FAIL;
	}

	GetWaterNum(&nWaterSum);
	if (nWaterSum == 0)
	{
		PubMsgDlg("批结算", "无流水不允许结算\n\n按任意键退出！", 1, 30);
		return APP_FAIL;
	}
	//ASSERT_QUIT(DispTotal());
	if (Settle(0) != APP_SUCC)
	{
		PubMsgDlg("结算失败", "请重新结算", 3, 10);
		return APP_FAIL;
	}
	if (YES == GetVarIsAutoLogOut())
	{
		nRet = LogOut();
	}

#if defined(USE_TMS)
	DoTmsTask(TMS_POS_CHKUPDATA);
#endif
	return nRet;
}
/**
* @brief 进行签退处理(有判断操作员的权限)
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int DoLogOut(void)
{
	int nWaterSum = 0;
	int nRet = 0;

	if (APP_SUCC != ChkOperRole())
	{
		return APP_FAIL;
	}

	GetWaterNum(&nWaterSum);
	if (nWaterSum > 0)
	{
		PubMsgDlg("签退", "有交易流水先结算", 3, 10);
		return APP_FAIL;
	}
	if (YES == GetVarIsLogin())
	{
		nRet =  LogOut();
	}
	else
	{
		PubMsgDlg("签退", "POS已经签退", 0, 3);
		nRet = APP_FAIL;
	}

#if defined(USE_TMS)
	DoTmsTask(TMS_POS_CHKUPDATA);
#endif
	return nRet;
}



static int DoPosLockAndUnLock(void)
{
	char szOperNo[OPERCODELEN+1];
	int nRole;

	ASSERT_QUIT(PubConfirmDlg("管理", "是否锁定终端?", 1, 10));
	PosLockUnLock();
	GetCurrentOper(szOperNo, NULL, &nRole);
	SetVarLastOperInfo(szOperNo, (char)nRole);
	return APP_SUCC;
}


/**
* @brief 触摸屏幕校准
* @param
* @return
* @li
*/
int DoScrCalibrate(void)
{
	const char *pszTitle = "屏幕校准";
	
	if (APP_SUCC == CheckOper(pszTitle, MANAGEROPER))
	{
		if(APP_FUNCQUIT == PubScrTSCalibrate())
		{
			PubMsgDlg(pszTitle, "无触屏模块,无需校准", 3, 5);
			return APP_SUCC;
		}
		PubClearAll();
		PubDisplayGen(3, "正在重启");
		PubUpdateWindow();
		NDK_SysReboot();
	}
	return APP_SUCC;
}

int DoSetCommType(void)
{
	if(APP_SUCC == CheckOper("通讯方式", MANAGEROPER))
	{
		SetFuncCommType();
	}

	return APP_SUCC;
}

/**
* @brief 进行管理菜单选择
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int MenuManage(void)
{
	STMENUTYPE stMenu[] =
	{
		{"签  到",  MenuTransLogin},
		{"签  退",  DoLogOut},
		{"交易查询",MenuTransFind},
		{"柜  员",  Operate},
		{"外线号码",DoSetPreDialNum},
		{"结  算",  DoSettle},
		{"锁定终端",DoPosLockAndUnLock},
		{"版  本", 	Version},
		{"通讯方式",DoSetCommType},
		{"屏幕校准",DoScrCalibrate},
		{"", NULL}
	};
	return PubOpenEasyMenu(stMenu, "管理", MENU_MODE_AUTOESC);
}



/**
* @brief 交易查询,查询交易明细、查询交易汇总、按凭证号查询
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int MenuTransFind(void)
{
	int nSelect;
//	int nRet;
	while(1)
	{
#if defined(EMV_IC)
		ASSERT_QUIT(ProSelectList("1.查询交易明细||2.查询交易汇总||3.按凭证号查询||4.查EMV成功信息||5.查EMV失败信息", "交易查询", 0xFF, &nSelect));
#else
		ASSERT_QUIT(ProSelectList("1.查询交易明细||2.查询交易汇总||3.按凭证号查询", "交易查询", 0xFF, &nSelect));
#endif
		switch(nSelect)
		{
		case 1:
			WaterOneByOne();
			break;
		case 2:
			DispTotal();
			break;
		case 3:
			FindByInvoice(0);
			break;
#if defined(EMV_IC)
		case 4:
			EmvDispWaterOneByOne(TRUE);
			break;
		case 5:
			EmvDispWaterOneByOne(FALSE);
			break;
#endif
		default:
			break;
		}
	}
	return APP_SUCC;
}

/**
* @brief 签到，POS签到，操作员签到，收银员积分签到
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int MenuTransLogin(void)
{
	Login();
	return APP_SUCC;
}


int ChkIsMCReturn(void)
{
	long lMC_Times;
	int nSave_Times;
	int nMC_Random;
	int nSave_Random;

	MC_GetEnterTimes(&lMC_Times, &nMC_Random);
	GetVarMCEnterTimeAndRandom(&nSave_Times, &nSave_Random);

	if (nMC_Random != nSave_Random)/**<由于各个应用签到时有更新pos时钟，此处仅比较随机值*/
	{
		return APP_SUCC;
	}
	return APP_FAIL;
}

void SaveMCEnterInfo(void)
{
	long lMC_Times;
	int nMC_Random;

	MC_GetEnterTimes(&lMC_Times, &nMC_Random);
	SetVarMCEnterTimeAndRandom(lMC_Times, nMC_Random);

	return ;
}


/**
* @brief 不支持的提示
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int FUNC_NO_SUPPORT(void)
{
	PubMsgDlg("温馨提示", "暂不支持该交易", 3, 5);
	return APP_FAIL;
}

/**
* @brief 检测ESIM卡和密码键盘是否存在
* @param 无
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int ChkPdAndRF(void)
{
	/**
	* 检测密码键盘；密钥模块初始化
	*/
	ChkPinpad();
	/**
	* 检测、初始化读卡器
	*/
	ChkRF();

	return APP_SUCC;
}

#if 0

/**
*电子现金快捷键
*/
int TxnEcSale(void)
{
	int nRet;
	int cInput = INPUT_INSERTIC;
	int nTrueInput;
	int nInputMode;
	char *pszTitle = "电子现金";
	char szAmount[12+1] = {0};

	if(YES != GetVarIsIcFlag())
	{
		PubMsgDlg(NULL,  "暂不支持IC卡交易", 3, 5);
		return APP_FAIL;
	}

	Cup_OpenLedBackGround();
	
	while(1)
	{
		int nAmtLen = 12;
		memset(szAmount, 0, sizeof(szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "请输入金额:", szAmount, &nAmtLen, INPUT_MODE_AMOUNT, INPUT_AMOUNT_TIMEOUT));
		/**
		* 检查金额的有效性
		*/
		if ( CheckTransAmount(szAmount, TRANS_SALE) == APP_SUCC )
		{
			break;
		}
	}

	if (YES == GetVarIsSupportRF())
	{
		cInput |= INPUT_RFCARD;
		ShowLightReady();
	}

	nRet = CardTaskAmt(szAmount, cInput, STRIPE_TIMEOUT, &nTrueInput);
	if (APP_SUCC != nRet)
	{
		DISP_OUT_ICC;
		return nRet;
	}
	if (INPUT_INSERTIC == nTrueInput)
	{
		ShowLightOff();
		ASSERT_QUIT(TradeInit(pszTitle, TDI_DEFAULT, TRANS_EC_PURCHASE, ATTR_PBOC_EC, YES));
		nInputMode = INPUT_INSERTIC;
		EmvSale(&nInputMode, szAmount, TRANS_EC_PURCHASE);
		DISP_OUT_ICC;

	}
	else if (INPUT_RFCARD == nTrueInput)
	{
		qPbocSale(szAmount, ATTR_qPBOC);
		DISP_OUT_ICC;
	}

	return APP_SUCC;
}
#endif

#if 0

/**
*借贷记快捷键
*/
int TxnCrDr(void)
{
	int nSelect;
	int nRet;
	int nTrueInput;
	char szAmount[12+1] = {0};
	
	ASSERT_QUIT(ProSelectList("1.消 费||2.圈 存", "借贷记", 0xFF, &nSelect));
	switch(nSelect)
	{
	case 1:
		if(YES != GetVarIsIcFlag())
		{
			PubClearAll();
			PubMsgDlg(NULL,  "暂不支持IC卡交易", 3, 5);
			return APP_FAIL;
		}
		
		ASSERT_QUIT(TradeInit("消费", TDI_DEFAULT, TRANS_SALE, 0, YES));
		/**
		* 输入金额
		*/
		
		while(1)
		{
			int nAmtLen = 12;			
			memset(szAmount, 0, sizeof(szAmount));
			ASSERT_QUIT(PubInputAmount("消费", "请输入金额:", szAmount, &nAmtLen, INPUT_MODE_AMOUNT, INPUT_AMOUNT_TIMEOUT));
			/**
			* 检查金额的有效性
			*/
			if ( CheckTransAmount(szAmount, TRANS_SALE) == APP_SUCC )
			{
				break;
			}
		}
		nTrueInput = INPUT_PLEASE_INSERTIC; //只支持IC插卡
		nRet = EmvSale(&nTrueInput,szAmount, TRANS_SALE_CRDR);
		if (nRet == APP_FALLBACK)
		{	
			nTrueInput = INPUT_NO;
			nRet = MagSale(&nTrueInput, szAmount, (char)ATTR_FALLBACK);
		}
		DISP_OUT_ICC;
		break;
	case 2:
		MenuLoad();
		break;
	default:
		break;
	}
	return APP_SUCC;
}

#endif
/**
* @brief 
* @param 
* @return 
* @li APP_SUCC
* @li APP_FAIL
*/
static int RecordVersion(void)
{
	STAPPVERS stAppVers;

	memset(&stAppVers, 0, sizeof(STAPPVERS));

	strcpy(stAppVers.szProName, APP_NAMEDESC);
		
	sprintf(stAppVers.szAppVer, "AAZGYL00%8.8s", INTER_VER + 2);

#ifdef EMV_IC
	strcpy(stAppVers.szEmvVer, EMV_getVersion());
#endif

	MC_GetLibVer(stAppVers.szMcVer);	

	strcpy(stAppVers.szLuaVer, PubLuaUIVer());
	
	PubRecordVersions(stAppVers);

	return APP_SUCC;

}

#if 0
static void GotoSleepApp(void)
{
	char szToolVer[32] = {0};

	PubGetToolsVer(szToolVer);
	if(memcmp(szToolVer,"ALTOOL011405060",15)>=0)
	{
		PubCommClose();
		PubGoSuspend();
	}
	else
	{
		PubDebug("TOOLVER:%s Too Old To Support Sleep\n",szToolVer);
	}
}
#endif
