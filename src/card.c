/**
* @file card.c
* @brief 卡处理模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

/**
* 定义全局变量
*/
static char gszPan[MAX_PAN+1];
static char gszTk1[MAX_TK1+1];
static char gszTk2[MAX_TK2+1];
static char gszTk3[MAX_TK3+1];


/**
* 内部函数声明
*/

static void DispDefaultTxnTitle(void);
static int validTrack(const char *);

/**
* 接口函数实现
*/

/**
* @brief 刷卡任务
* @param in cInPutType 允许的操作事件见enum CARDINPUTMODE
* @param in nTimeOut 超时时间，等于0一直等待刷卡事件
* @param out pcType 发生的操作事件见enum CARDINPUTMODE
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SwipeCard(const int nInPutType, const uint nTimeOut, int *pnType)
{
	int nRet = 0,i =0,nRes = 0;
	STREADCARDPARAM stReadCardParam;

	memset(&stReadCardParam, 0, sizeof(STREADCARDPARAM));
	NDK_ScrPush();
	while(1)
	{
		switch(nInPutType)
		{
		case INPUT_KEYIN:
			stReadCardParam.unFlag = CARD_KEYIN;
			break;	
		case INPUT_STRIPE:
			stReadCardParam.unFlag = CARD_STRIPE;
			break;
		case INPUT_INSERTIC_FORCE:
		case INPUT_INSERTIC:	
			stReadCardParam.unFlag = CARD_INSERTIC;	
			break;
		case INPUT_RFCARD:
			stReadCardParam.unFlag = CARD_RFCARD;
			break;
		case INPUT_UPCARD:
			stReadCardParam.unFlag = CARD_RFCARD;
			break;
		case INPUT_STRIPE_KEYIN:
			stReadCardParam.unFlag = CARD_STRIPE_KEYIN;
			break;
		case INPUT_STRIPE_INSERTIC:
			stReadCardParam.unFlag = CARD_STRIPE_INSERTIC;
			break;			
		case INPUT_STRIPE_RFCARD:
			stReadCardParam.unFlag = CARD_STRIPE_RFCARD;
			break;	
		case INPUT_STRIPE_KEYIN_INSERTIC:
			stReadCardParam.unFlag = CARD_STRIPE_KEYIN_INSERTIC;
			break;		
		case INPUT_STRIPE_KEYIN_RFCARD:
			stReadCardParam.unFlag = CARD_STRIPE_KEYIN_RFCARD;
			break;		
		case INPUT_STRIPE_KEYIN_INSERTIC_RFCARD:
			stReadCardParam.unFlag = CARD_STRIPE_KEYIN_INSERTIC_RFCARD;
			break;	
		case INPUT_STRIPE_INSERTIC_RFCARD:
			stReadCardParam.unFlag = CARD_STRIPE_INSERTIC_RFCARD;
			break;			
		case INPUT_KEYIN_INSERIC:
			stReadCardParam.unFlag = CARD_KEYIN_INSERIC;
			break;
		case INPUT_KEYIN_RFCARD:
			stReadCardParam.unFlag = CARD_KEYIN_RFCARD;
			break;	
		case INPUT_KEYIN_INSERIC_RFCARD:
			stReadCardParam.unFlag = CARD_KEYIN_INSERIC_RFCARD;
			break;
		case INPUT_INSERTIC_RFCARD:
			stReadCardParam.unFlag = CARD_INSERTIC_RFCARD;		
			break;		
		case INPUT_STRIPE_FALLBACK:
			stReadCardParam.unFlag = CARD_STRIPE;
			break;
		case INPUT_STRIPE_TRANSFER_IN:
			stReadCardParam.unFlag = CARD_STRIPE;
			break;
		case INPUT_STRIPE_TRANSFER_OUT:
			stReadCardParam.unFlag = CARD_STRIPE;
			break;
		case INPUT_STRIPE_INSERTIC_TRANSFER_OUT:
			stReadCardParam.unFlag = CARD_STRIPE|CARD_INSERTIC;
			break;
		default:
			return APP_QUIT;
			break;
		}					


		if(stReadCardParam.unFlag == 0)
			return APP_QUIT;

		if (nTimeOut == 0)
		{
			stReadCardParam.unFunctionKeyFlag = 1;
		}
		else
		{
			stReadCardParam.unFunctionKeyFlag = 0;
		}
		
		stReadCardParam.unInputMinLen = 13;		
		stReadCardParam.unInputMaxLen = 19;
		stReadCardParam.unTimeout = (nTimeOut == 0 ? 5 : nTimeOut);
		stReadCardParam.unSleepTime= stReadCardParam.unTimeout + 10;
		stReadCardParam.unMaintk = MAINTK2;

		memset(gszTk1, 0, sizeof(gszTk1));
		memset(gszTk2, 0, sizeof(gszTk2));
		memset(gszTk3, 0, sizeof(gszTk3));
		memset(gszPan, 0, sizeof(gszPan));
		nRet = PubPosReadCard(&stReadCardParam, gszTk1, gszTk2, gszTk3, &nRes);
		if(nRet == APP_SUCC)
		{
			gszTk1[MAX_TK1] = gszTk2[MAX_TK2] =  gszTk3[MAX_TK3] = 0;
			if (validTrack(gszTk2) == APP_FAIL || validTrack(gszTk3) == APP_FAIL || (nRes == CARD_STRIPE && strlen(gszTk2)< KJ_MIN_TK2)) 
			{
				nRet = APP_FAIL;
				nRes = APP_FAIL;
			}
		}

		if (nRet != APP_SUCC)
		{
			if (nRes >= 0xffffff08 && nRes <= 0xfffffffa)/**<IC卡返回值*/
			{			
				if (NULL != pnType)
				{
					*pnType = INPUT_INSERTIC;
				}
				return APP_SUCC;
						
			}
			switch(nRes)
			{
			case CARD_FUNCQUIT:
				if (NULL != pnType)
				{
					*pnType = gszTk2[0];
				}
				if(gszTk2[0] == KEY_ENTER )
				{
					return KEY_ENTER;
				}
				if (gszTk2[0] == KEY_ESC)
				{
					return APP_QUIT;
				}
				if(nTimeOut==0)
				{
					return gszTk2[0];
				}
				continue;
				break;
			case APP_TIMEOUT:
				return APP_TIMEOUT;
				break;
			case APP_QUIT:
				return APP_QUIT;
				break;
			default:
				if (nTimeOut == 0)
				{
					DispDefaultTxnTitle();
				}
				PubClear2To4();
				PubDisplayStrInline(0, 2, "刷卡错误");
				PubDisplayStrInline(0, 3, "请继续刷卡->");
				PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "按<取消>键退出");
				PubUpdateWindow();
				nRet = PubGetKeyCode(1);
				if (nRet == KEY_ESC)
				{
					return APP_QUIT;
				}
				NDK_ScrPop();
				PubUpdateWindow();	
				continue;
			}
		}
		
		switch(nRes)
		{
		case CARD_INSERTIC:
			if (NULL != pnType)
			{
				*pnType = INPUT_INSERTIC;
			}
			return APP_SUCC;
			break;
		case CARD_STRIPE:
			if (NULL != pnType)
			{
				*pnType = INPUT_STRIPE;
			}
			break;
		case CARD_RFCARD:
			if (NULL != pnType)
			{
				if(nInPutType & INPUT_RFCARD)
					*pnType = INPUT_RFCARD;
				else if(nInPutType & INPUT_UPCARD)
					*pnType = INPUT_UPCARD;
				else
					return APP_FAIL;
			}
			return APP_SUCC;
			break;
		case CARD_KEYIN:
			if (strlen(gszTk2) < MIN_PAN)
			{
				if (nTimeOut == 0)
				{
					DispDefaultTxnTitle();
				}
				PubClear2To4();
				PubDisplayStrInline(0, 2, "手输错误");
				PubDisplayStrInline(0, 3, "请继续...");
				PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "按<取消>键退出");
				PubUpdateWindow();
				nRet = PubGetKeyCode(1);
				if (nRet == KEY_ESC)
				{
					return APP_QUIT;
				}
				NDK_ScrPop();
				PubUpdateWindow();
				continue;
			}
			if (NULL != pnType)
			{
				*pnType = INPUT_KEYIN;
			}
			memcpy(gszPan, gszTk2, strlen(gszTk2));
			return APP_SUCC;
			break;
		default:
			return APP_FAIL;
			break;
		}
		break;
	}
	if (0 != gszTk2[0])
	{
		for (i = 0; i < 37; i++)
		{
			if (gszTk2[i] == '=')
			{
				break;
			}
		}
		if (i == 37)
		{
			return APP_FAIL;
		}
		
		if (i > 19)
		{
			memcpy(gszPan, gszTk2, 19);
		}
		else
		{
			memcpy(gszPan, gszTk2, i);
		}
		return APP_SUCC;
	}
	else if (0 != gszTk3[0])
	{
		for (i = 0; i < 104; i++)
		{
			if (gszTk3[i] == '=')
			{
				break;
			}
		}
		if (i == 104)
		{
			return APP_FAIL;
		}
		
		if (i > 19)
		{
			memcpy(gszPan, gszTk3, 19);
		}
		else
		{
			memcpy(gszPan, gszTk3, i);
		}
		return APP_SUCC;
	}
	return APP_FAIL;
}
/**
* @brief 刷卡任务(显示金额)
* @param [in]  pszAmount  金额指针
* @param [in]  cInPutType 允许的操作事件，见enum EM_CARDINPUTMODE
* @param [in]  nTimeOut 超时时间，等于0一直等待刷卡事件
* @param [out] pnType 发生的操作事件，见enum EM_CARDINPUTMODE
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 
* @date
*/

int CardTaskAmt(const char *pszAmount, const int nInPutType, const uint nTimeOut, int *pnType)
{
	int nRet = 0;
	char szDispAmt[13+1] = {0};
	char cIsDisp = FALSE;
	

	PubClear2To4();
	
	if(pszAmount != NULL)
	{	
		ProAmtToDispOrPnt(pszAmount, szDispAmt);
		PubAllTrim(szDispAmt);
		if(nInPutType == INPUT_INSERTIC_RFCARD 
			//|| nInPutType == INPUT_RFCARD 
			|| nInPutType == INPUT_STRIPE_INSERTIC_RFCARD
			)
		{
			if (YES == GetVarShowRFLogo())				
			{
				ImgManage(LOGO_RF);				
				PubDisplayStr(DISPLAY_MODE_NORMAL, 2, 1,  szDispAmt);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "请使用您");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "的银行卡");
			
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%s元", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请使用您的银行卡");
			}
						
			cIsDisp = TRUE;
		}
		else if (nInPutType == INPUT_RFCARD)
		{	
			if (YES == GetVarShowRFLogo())
			{
				ImgManage(LOGO_RF);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 2, 1,  szDispAmt);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "请挥卡");
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%s元", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请挥卡");
			}
			cIsDisp = TRUE;			
		}
		else if (nInPutType == INPUT_UPCARD )
		{				
			if (YES == GetVarShowRFLogo())	
			{
				ImgManage(LOGO_RF);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 2, 1,  szDispAmt);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "请挥卡或");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "放手机");
			
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%s元", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请挥卡或放手机");
			}
						
			cIsDisp = TRUE;
		}
		else if(nInPutType == INPUT_INSERTIC)
		{
			PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%s元", szDispAmt);
			PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请使用您的银行卡");
			cIsDisp = TRUE;
		}

	}	

	switch(nInPutType)
	{
	case INPUT_KEYIN:
		PubDisplayStr(1, 2, 1, "请输入卡号:");
		break;	
	case INPUT_STRIPE:
		PubDisplayStr(1, 3, 1, "请刷卡-->");
		break;
	case INPUT_INSERTIC_FORCE:
	case INPUT_INSERTIC:	
		if(!cIsDisp)
		{
			PubDisplayStr(1, 3, 1, "请插入IC卡");
		}
		break;
	case INPUT_RFCARD:
		if(!cIsDisp)
		{
			PubDisplayStr(1, 3, 1, "请挥卡");
			if (YES == GetVarShowRFLogo())
			{
				ImgManage(LOGO_RF);
			}
		}
		break;
	case INPUT_UPCARD:
		if(!cIsDisp)
		{				
			if (YES == GetVarShowRFLogo())
			{
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "请挥卡或");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "放手机");				
				ImgManage(LOGO_RF);
			}
			else
			{
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请挥卡或放手机");
			}
			
		}
		break;
	case INPUT_STRIPE_KEYIN:
		PubDisplayStr(1, 2, 1, "请刷卡或输入卡号");
		break;
	case INPUT_STRIPE_INSERTIC:
		PubDisplayStr(1, 3, 1, "请插入IC卡或刷卡");
		break;			
	case INPUT_STRIPE_RFCARD:
		if (YES == GetVarShowRFLogo())
		{
			PubDisplayStr(1, 2, 1, "请刷卡");
			PubDisplayStr(1, 3, 1, "或挥卡");
			ImgManage(LOGO_RF);
		}
		else
		{
			PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请刷卡或挥卡");
		}
		break;	
	case INPUT_STRIPE_KEYIN_INSERTIC:
		PubDisplayStr(1, 2, 1, "请插入IC卡");
		PubDisplayStr(1, 3, 1, "或刷卡或输入卡号");
		break;		
	case INPUT_STRIPE_KEYIN_RFCARD:
		PubDisplayStr(1, 2, 1, "请输入卡号");
		PubDisplayStr(1, 3, 1, "或刷卡或挥卡");
		break;		
	case INPUT_STRIPE_KEYIN_INSERTIC_RFCARD:
		PubDisplayStr(1, 2, 1, "请输入卡号或挥卡");
		PubDisplayStr(1, 3, 1, "或刷卡或插卡");
		break;	
	case INPUT_STRIPE_INSERTIC_RFCARD:
		if(!cIsDisp)
		{
			PubDisplayStr(1, 2, 1, "请插入IC卡或刷卡");
			PubDisplayStr(1, 3, 1, "或挥卡");
		}
		break;			
	case INPUT_KEYIN_INSERIC:
		PubDisplayStr(1, 2, 1, "请插入IC卡");
		PubDisplayStr(1, 3, 1, "或输入卡号");
		break;
	case INPUT_KEYIN_RFCARD:
		PubDisplayStr(1, 2, 1, "请挥卡");
		PubDisplayStr(1, 3, 1, "或输入卡号");
		break;	
	case INPUT_KEYIN_INSERIC_RFCARD:
		PubDisplayStr(1, 2, 1, "请插入IC卡或挥卡");
		PubDisplayStr(1, 3, 1, "或输入卡号");
		break;
	case INPUT_INSERTIC_RFCARD:
		if(!cIsDisp)
		{
			if (YES == GetVarShowRFLogo())
			{
				PubDisplayStr(1, 2, 1, "请插入IC");
				PubDisplayStr(1, 3, 1, "卡或挥卡");
				ImgManage(LOGO_RF);
			
			}
			else
			{
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"请插入IC卡或挥卡");
			}
					
		}	
		break;		
	case INPUT_STRIPE_FALLBACK:
		PubDisplayStr(1, 2, 1, "读取IC卡错误");
		PubDisplayStr(1, 3, 1, "请刷卡-->");
		PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "按<取消>键退出");
		break;
	case INPUT_STRIPE_TRANSFER_IN:
		PubDisplayStr(1, 3, 1, "请刷转入卡-->");
		break;
	case INPUT_STRIPE_TRANSFER_OUT:
		PubDisplayStr(1, 3, 1, "请刷转出卡-->");
		break;
	case INPUT_STRIPE_INSERTIC_TRANSFER_OUT:
		PubDisplayStr(1, 3, 1, "请刷或插转出卡");
		break;
	default:
		return APP_QUIT;
		break;
	}					
	PubUpdateWindow();

	nRet = SwipeCard(nInPutType, nTimeOut, pnType);

	return nRet;
	

}



/**
* @brief 提取主帐号
* @param out 主帐号指针
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetPan(char *pszPan)
{	
	if (NULL == pszPan)
	{
		return APP_FAIL;
	}
	else
	{
		memcpy(pszPan, gszPan, MAX_PAN);
	}
	return APP_SUCC;
}

/**
* @brief 提取磁道信息
* @param out pszTk1磁道1指针
* @param out pszTk2磁道2指针
* @param out pszTk3磁道3指针
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetTrack(char *pszTk1, char *pszTk2, char *pszTk3)
{
	if (NULL != pszTk1)
	{
		memcpy(pszTk1, gszTk1, MAX_TK1);
	}
	if (NULL != pszTk2)
	{
		memcpy(pszTk2, gszTk2, MAX_TK2);
	}
	if (NULL != pszTk3)
	{
		memcpy(pszTk3, gszTk3, MAX_TK3);
	}

	return APP_SUCC;
}
/**
* @brief 显示卡号
* @param in pszPan 卡号指针
* @param in nTimeOut 超时时间
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int DispPan(const char *pszPan, int nTimeOut)
{
	char szMsg[64+1];
	
	PubClear2To4();
	sprintf(szMsg, "卡号:%s\n", pszPan);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;
}
/**
* @brief 检查是否是ic卡
* @param in pszTk2 2磁道数据指针
* @param out pcSerCode 服务代码
* @return
* @li APP_SUCC 是ic卡
* @li APP_FAIL不是ic卡
* @li APP_QUIT退出
*/
int ChkIsICC(const char *pszTk2, char *pcSerCode)
{
	int i;
	char szContent[50] = {0};

	for(i = 0; i < 38; i++)
	{
		if ('=' == *(pszTk2+i))
		{
			break;
		}
	}
	if (38 == i)
	{
		return APP_FAIL;
	}
	if (('2' == *(pszTk2+i+5)) || ('6' == *(pszTk2+i+5)))
	{
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C本卡为IC卡");
		PubMsgDlg(NULL, szContent, 0, 1);

		if (NULL != pcSerCode)
		{
			*pcSerCode = (*(pszTk2+i+5));/**<输出该位置的值*/
		}
		return APP_SUCC;
	}
	return APP_FAIL;
}

/**
* @brief 关闭显示缺省交易界面
* @param 无
* @return 无
*/
void DisableDispDefault(void)
{
	PubClearAll();
	ShowLightOff();
	PubLuaDisplay("LuaDisableDispDefault");
	return ;
}

/**
* @brief 显示缺省交易title
* @param 无
* @return 无
*/
static void DispDefaultTxnTitle(void)
{
	char cDefaultType;
	char *pszTitle;
	
	GetVarDefaultTransType(&cDefaultType);
	if ('1' == cDefaultType)
	{
		pszTitle = "消费";
	}
	else
	{
		pszTitle = "预授权";
	}
	PubDisplayTitle(pszTitle);
	return ;
}
/**
* @brief  确定磁道信息有效 
* @param const char *pszTk，磁道信息
* @param out 无
* @return
* @li APP_SUCC 
* @li APP_FAIL
*/
static int validTrack(const char *pszTk)
{
	int i;

	for(i=0; i<strlen(pszTk); i++)
	{
		if ( (pszTk[i] < '0' || pszTk[i] > '9') && pszTk[i] != '=' )
		{
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}

/**
* @brief 从二磁道信息中获取ServiceCode
* @param in  const char *pszTk2 二磁道信息
* @param out   char *pszServiceCode 
* @return 
* @li APP_SUCC 
* @li APP_FAIL
*/
int GetServiceCode24FromTk2(const char *pszTk2, char *pszServiceCode24)
{
	int i;

	for(i=0; i<strlen(pszTk2); i++)
	{
		if (pszTk2[i] == '=' )
		{
			if (NULL == pszServiceCode24)
			{
				return APP_FAIL;
			}
			memcpy(pszServiceCode24, &pszTk2[i+5], 3);
			return APP_SUCC;
		}
	}
	return APP_FAIL;
}
/**
* @brief  卡号部分内容*号替代
* @param [in]   uchar *pCardPan 	   待处理主账户
* @param [in]   const uint nPanLen     待处理主账户长度
* @param [in]   const uchar cTransType 交易类型
* @param [in]   const uint  nMode      交易方式
* @param [out]  uchar *pCardPan        处理之后的主账户
* @return 
* @li APP_SUCC 
* @li APP_FAIL
*/
int CtrlCardPan(uchar *pCardPan, const uint nPanLen, const uchar cTransType, const uint nMode)
{
	if (NULL == pCardPan)
	{
		return APP_FAIL;
	}

	if (19 < nPanLen || 12 > nPanLen) 
	{
		return APP_FAIL;
	}

	/**< 除对银行卡卡号前6位和后4位予以显示外，
	其余所有卡号字段均予以屏蔽，用"*"或"#"代替。*/
	memset(pCardPan + 6, '*', nPanLen - 6 - 4);

	return APP_SUCC;
}

/**
* @brief 获取卡处理类型
* @param [in]  pszInputMode   刷卡模式
* @param [in]   cTransType   交易类型
* @param [out]  pszType   刷卡类型
* @return 无
* @author 
* @date 
*/
void GetStripeType(const char* pszInputMode, char cTransType, char cTransAttr,char* pszType)
{
	if(pszType == NULL)
		return;
	if(memcmp(pszInputMode, "02", 2) == 0)		//刷磁条卡
	{
		if (cTransAttr == ATTR_FALLBACK)
		{
			strcpy(pszType, "F");
		}
		else
		{
			strcpy(pszType, "S");
		}
	}
	else if(memcmp(pszInputMode, "05", 2) == 0 || memcmp(pszInputMode, "95", 2) == 0)		//芯片式接触卡
	{
		strcpy(pszType, "I");	
	}
	else if((memcmp(pszInputMode, "07", 2) == 0 || memcmp(pszInputMode, "91", 2) == 0 || memcmp(pszInputMode, "96", 2) == 0 || memcmp(pszInputMode, "98", 2) == 0))//非接触式卡
	{
		strcpy(pszType, "C");
	}
	else if(memcmp(pszInputMode, "01", 2) == 0)
	{
		strcpy(pszType, "M");
	}
	else
		strcpy(pszType, "N");		//无卡交易
}


//整合刷卡预留IC卡
int ProInputCard(int *pnInputMode, STSYSTEM *pstSystem)
{
	int nRet;
	int nInput = INPUT_STRIPE;
	int nTrueInput;
    
	if (*pnInputMode != INPUT_NO)
	{
		nInput = *pnInputMode;
	}

	while(1)
	{
		nRet = CardTaskAmt(NULL,nInput, STRIPE_TIMEOUT, &nTrueInput);
		if (nRet != APP_SUCC)
		{
			return APP_QUIT;
		}
		if (nTrueInput == INPUT_STRIPE)
		{
			*pnInputMode = INPUT_STRIPE;
			memcpy(pstSystem->szInputMode, "021", 3);
			GetTrack(NULL,pstSystem->szTrack2, pstSystem->szTrack3);
			GetPan(pstSystem->szPan);
			break;
		}
		else if (nTrueInput == INPUT_INSERTIC)
		{
			*pnInputMode = INPUT_INSERTIC_FORCE;
			return APP_QUIT;
		}
		else
		{
			return APP_FAIL;
		}
	}

	return APP_SUCC;
}


