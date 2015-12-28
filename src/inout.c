/**
* @file inout.c
* @brief 输入输出模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

/**
* @brief 输入密码
* @param in pszPan 用于加密用的卡号
* @param in pszAmount 用于显示用的金额
* @param in pszPin 密码密文固定8个字节
* @return 返回
*/

int GetPin(const char *pszPan, const char *pszAmount, char *pszPin)
{
	int nRet, nMainKeyNo = 0, nPinLen=0, nMode = PINTYPE_WITHOUTPAN;
	char szDispAmt[64+1] = {0};

	GetVarMainKeyNo(&nMainKeyNo);
	PubSetCurrentMainKeyIndex(nMainKeyNo);

	if (NULL != pszAmount)
	{
		/*<处理金额用于显示*/
		ProAmtToDispOrPnt(pszAmount, szDispAmt);
	}

	if(PINTYPE_WITHPAN == GetVarPinEncyptMode())
	{
		nMode = PINTYPE_WITHPAN;
	}

	if (YES == GetVarIsPinpad())
	{
		PubClear2To4();
		if (NULL != pszAmount)
		{
			/*<处理金额用于显示*/
			PubDispPinPad(1, szDispAmt);
			PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|R%s元", szDispAmt);
		}		
		PubDisplayStrInline(0, 3, "请持卡人输入密码");
		PubUpdateWindow();

		nRet = PubActiveKey(DESMODE_3DES == GetVarEncyptMode() ? DESMODE_3DES : DESMODE_DES,\
		nMainKeyNo, KEY_TYPE_PIN);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("输入密码", "密码键盘激活失败，请重新签到", 3, 10);
			return APP_FAIL;
		}
	}
	nRet = PubGetPin(pszPin, &nPinLen, nMode, pszPan, pszAmount, PIN_MAX_LEN, PIN_MIN_LEN);
	if (nRet != APP_SUCC)
	{
		if (YES == GetVarIsPinpad())
			PubClrPinPad();
		if (nRet == APP_QUIT)
		{
			return APP_QUIT;
		}
		//调用密码错误信息
		PubDispErr("输入密码");
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief 选择授权方式
* @param out  psCode选择的授权方式固定2个字节
* @return 返回
* @li APP_SUCC成功
* @li APP_QUIT取消
*/
int GetAuthorCode(char *pszTitle, char *psCode)
{
	int nRet, nSelect, nIsSmallGeneAuth;
		
	nIsSmallGeneAuth = GetVarIsSmallGeneAuth();
	while(1)
	{
		if(nIsSmallGeneAuth == YES)
		{
			nRet = ProSelectList("1.POS|2.电话|3.小额代授权", "选择授权方式:", 0xFF, &nSelect);
		}
		else
		{
			nRet = ProSelectList("1.POS|2.电话", "选择授权方式:", 0xFF, &nSelect);
		}
		if(nRet == APP_TIMEOUT || nRet == APP_QUIT)
		{
			return APP_QUIT;
		}
		switch(nSelect)
		{	
		case 1:
			memcpy(psCode, "00", 2);
			break;
		case 2:
			memcpy(psCode, "01", 2);
			break;
		case 3:
			if(nIsSmallGeneAuth == YES)
			{
				memcpy(psCode, "02", 2);
				break;
			}
			else
				break;
		default:
			break;								
		}
		break;
	}
	return APP_SUCC;	
}


/**
* @brief 选择国际组织代码
* @param out  psInterOrg选择的国际组织代码固定3个字节
* @return 返回
* @li APP_SUCC成功
* @li APP_QUIT取消
*/
int GetInterOrg(char *pszTitle, char *psInterOrg)
{
	int nRet, nSelect;

	while(1)
	{
		nRet = ProSelectList("1.CUP|2.VIS|3.MCC|4.MAE|5.JCB|6.DCC|7.AMX", "选择国际公司代码", 0xFF, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return APP_QUIT;
		}
		switch(nSelect)
		{		
		case 1:
			memcpy(psInterOrg, "CUP", 3);
			break;
		case 2:
			memcpy(psInterOrg, "VIS", 3);
			break;
		case 3:
			memcpy(psInterOrg, "MCC", 3);
			break;
		case 4:
			memcpy(psInterOrg, "MAE", 3);
			break;
		case 5:
			memcpy(psInterOrg, "JCB", 3);
			break;
		case 6:
			memcpy(psInterOrg, "DCC", 3);
			break;
		case 7:
			memcpy(psInterOrg, "AMX", 3);
			break;
		default:
			break;
		}
		break;
	}
	return APP_SUCC;
}

int DispCouponID(const char *pszCouponID, int nTimeOut)
{
	char szMsg[64+1];
	
	PubClear2To4();
	sprintf(szMsg, "串码:%s\n", pszCouponID);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;
}

int DispTelno(const char *pszTelno, int nTimeOut)
{
	char szMsg[64+1];
	
	PubClear2To4();
	sprintf(szMsg, "手机号:%s\n", pszTelno);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;

}

