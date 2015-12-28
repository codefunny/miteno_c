/**
* @file card.c
* @brief ������ģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

/**
* ����ȫ�ֱ���
*/
static char gszPan[MAX_PAN+1];
static char gszTk1[MAX_TK1+1];
static char gszTk2[MAX_TK2+1];
static char gszTk3[MAX_TK3+1];


/**
* �ڲ���������
*/

static void DispDefaultTxnTitle(void);
static int validTrack(const char *);

/**
* �ӿں���ʵ��
*/

/**
* @brief ˢ������
* @param in cInPutType ����Ĳ����¼���enum CARDINPUTMODE
* @param in nTimeOut ��ʱʱ�䣬����0һֱ�ȴ�ˢ���¼�
* @param out pcType �����Ĳ����¼���enum CARDINPUTMODE
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
			if (nRes >= 0xffffff08 && nRes <= 0xfffffffa)/**<IC������ֵ*/
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
				PubDisplayStrInline(0, 2, "ˢ������");
				PubDisplayStrInline(0, 3, "�����ˢ��->");
				PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "��<ȡ��>���˳�");
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
				PubDisplayStrInline(0, 2, "�������");
				PubDisplayStrInline(0, 3, "�����...");
				PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "��<ȡ��>���˳�");
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
* @brief ˢ������(��ʾ���)
* @param [in]  pszAmount  ���ָ��
* @param [in]  cInPutType ����Ĳ����¼�����enum EM_CARDINPUTMODE
* @param [in]  nTimeOut ��ʱʱ�䣬����0һֱ�ȴ�ˢ���¼�
* @param [out] pnType �����Ĳ����¼�����enum EM_CARDINPUTMODE
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
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
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "��ʹ����");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "�����п�");
			
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%sԪ", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ʹ���������п�");
			}
						
			cIsDisp = TRUE;
		}
		else if (nInPutType == INPUT_RFCARD)
		{	
			if (YES == GetVarShowRFLogo())
			{
				ImgManage(LOGO_RF);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 2, 1,  szDispAmt);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "��ӿ�");
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%sԪ", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ӿ�");
			}
			cIsDisp = TRUE;			
		}
		else if (nInPutType == INPUT_UPCARD )
		{				
			if (YES == GetVarShowRFLogo())	
			{
				ImgManage(LOGO_RF);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 2, 1,  szDispAmt);
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "��ӿ���");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "���ֻ�");
			
			}
			else
			{
				PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%sԪ", szDispAmt);
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ӿ�����ֻ�");
			}
						
			cIsDisp = TRUE;
		}
		else if(nInPutType == INPUT_INSERTIC)
		{
			PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|C%sԪ", szDispAmt);
			PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ʹ���������п�");
			cIsDisp = TRUE;
		}

	}	

	switch(nInPutType)
	{
	case INPUT_KEYIN:
		PubDisplayStr(1, 2, 1, "�����뿨��:");
		break;	
	case INPUT_STRIPE:
		PubDisplayStr(1, 3, 1, "��ˢ��-->");
		break;
	case INPUT_INSERTIC_FORCE:
	case INPUT_INSERTIC:	
		if(!cIsDisp)
		{
			PubDisplayStr(1, 3, 1, "�����IC��");
		}
		break;
	case INPUT_RFCARD:
		if(!cIsDisp)
		{
			PubDisplayStr(1, 3, 1, "��ӿ�");
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
				PubDisplayStr(DISPLAY_MODE_NORMAL, 3, 1,  "��ӿ���");
				PubDisplayStr(DISPLAY_MODE_NORMAL, 4, 1,  "���ֻ�");				
				ImgManage(LOGO_RF);
			}
			else
			{
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ӿ�����ֻ�");
			}
			
		}
		break;
	case INPUT_STRIPE_KEYIN:
		PubDisplayStr(1, 2, 1, "��ˢ�������뿨��");
		break;
	case INPUT_STRIPE_INSERTIC:
		PubDisplayStr(1, 3, 1, "�����IC����ˢ��");
		break;			
	case INPUT_STRIPE_RFCARD:
		if (YES == GetVarShowRFLogo())
		{
			PubDisplayStr(1, 2, 1, "��ˢ��");
			PubDisplayStr(1, 3, 1, "��ӿ�");
			ImgManage(LOGO_RF);
		}
		else
		{
			PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"��ˢ����ӿ�");
		}
		break;	
	case INPUT_STRIPE_KEYIN_INSERTIC:
		PubDisplayStr(1, 2, 1, "�����IC��");
		PubDisplayStr(1, 3, 1, "��ˢ�������뿨��");
		break;		
	case INPUT_STRIPE_KEYIN_RFCARD:
		PubDisplayStr(1, 2, 1, "�����뿨��");
		PubDisplayStr(1, 3, 1, "��ˢ����ӿ�");
		break;		
	case INPUT_STRIPE_KEYIN_INSERTIC_RFCARD:
		PubDisplayStr(1, 2, 1, "�����뿨�Ż�ӿ�");
		PubDisplayStr(1, 3, 1, "��ˢ����忨");
		break;	
	case INPUT_STRIPE_INSERTIC_RFCARD:
		if(!cIsDisp)
		{
			PubDisplayStr(1, 2, 1, "�����IC����ˢ��");
			PubDisplayStr(1, 3, 1, "��ӿ�");
		}
		break;			
	case INPUT_KEYIN_INSERIC:
		PubDisplayStr(1, 2, 1, "�����IC��");
		PubDisplayStr(1, 3, 1, "�����뿨��");
		break;
	case INPUT_KEYIN_RFCARD:
		PubDisplayStr(1, 2, 1, "��ӿ�");
		PubDisplayStr(1, 3, 1, "�����뿨��");
		break;	
	case INPUT_KEYIN_INSERIC_RFCARD:
		PubDisplayStr(1, 2, 1, "�����IC����ӿ�");
		PubDisplayStr(1, 3, 1, "�����뿨��");
		break;
	case INPUT_INSERTIC_RFCARD:
		if(!cIsDisp)
		{
			if (YES == GetVarShowRFLogo())
			{
				PubDisplayStr(1, 2, 1, "�����IC");
				PubDisplayStr(1, 3, 1, "����ӿ�");
				ImgManage(LOGO_RF);
			
			}
			else
			{
				PubDisplayStr(DISPLAY_MODE_CENTER,3,1 ,"�����IC����ӿ�");
			}
					
		}	
		break;		
	case INPUT_STRIPE_FALLBACK:
		PubDisplayStr(1, 2, 1, "��ȡIC������");
		PubDisplayStr(1, 3, 1, "��ˢ��-->");
		PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "��<ȡ��>���˳�");
		break;
	case INPUT_STRIPE_TRANSFER_IN:
		PubDisplayStr(1, 3, 1, "��ˢת�뿨-->");
		break;
	case INPUT_STRIPE_TRANSFER_OUT:
		PubDisplayStr(1, 3, 1, "��ˢת����-->");
		break;
	case INPUT_STRIPE_INSERTIC_TRANSFER_OUT:
		PubDisplayStr(1, 3, 1, "��ˢ���ת����");
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
* @brief ��ȡ���ʺ�
* @param out ���ʺ�ָ��
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
* @brief ��ȡ�ŵ���Ϣ
* @param out pszTk1�ŵ�1ָ��
* @param out pszTk2�ŵ�2ָ��
* @param out pszTk3�ŵ�3ָ��
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
* @brief ��ʾ����
* @param in pszPan ����ָ��
* @param in nTimeOut ��ʱʱ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int DispPan(const char *pszPan, int nTimeOut)
{
	char szMsg[64+1];
	
	PubClear2To4();
	sprintf(szMsg, "����:%s\n", pszPan);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;
}
/**
* @brief ����Ƿ���ic��
* @param in pszTk2 2�ŵ�����ָ��
* @param out pcSerCode �������
* @return
* @li APP_SUCC ��ic��
* @li APP_FAIL����ic��
* @li APP_QUIT�˳�
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
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C����ΪIC��");
		PubMsgDlg(NULL, szContent, 0, 1);

		if (NULL != pcSerCode)
		{
			*pcSerCode = (*(pszTk2+i+5));/**<�����λ�õ�ֵ*/
		}
		return APP_SUCC;
	}
	return APP_FAIL;
}

/**
* @brief �ر���ʾȱʡ���׽���
* @param ��
* @return ��
*/
void DisableDispDefault(void)
{
	PubClearAll();
	ShowLightOff();
	PubLuaDisplay("LuaDisableDispDefault");
	return ;
}

/**
* @brief ��ʾȱʡ����title
* @param ��
* @return ��
*/
static void DispDefaultTxnTitle(void)
{
	char cDefaultType;
	char *pszTitle;
	
	GetVarDefaultTransType(&cDefaultType);
	if ('1' == cDefaultType)
	{
		pszTitle = "����";
	}
	else
	{
		pszTitle = "Ԥ��Ȩ";
	}
	PubDisplayTitle(pszTitle);
	return ;
}
/**
* @brief  ȷ���ŵ���Ϣ��Ч 
* @param const char *pszTk���ŵ���Ϣ
* @param out ��
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
* @brief �Ӷ��ŵ���Ϣ�л�ȡServiceCode
* @param in  const char *pszTk2 ���ŵ���Ϣ
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
* @brief  ���Ų�������*�����
* @param [in]   uchar *pCardPan 	   ���������˻�
* @param [in]   const uint nPanLen     ���������˻�����
* @param [in]   const uchar cTransType ��������
* @param [in]   const uint  nMode      ���׷�ʽ
* @param [out]  uchar *pCardPan        ����֮������˻�
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

	/**< �������п�����ǰ6λ�ͺ�4λ������ʾ�⣬
	�������п����ֶξ��������Σ���"*"��"#"���档*/
	memset(pCardPan + 6, '*', nPanLen - 6 - 4);

	return APP_SUCC;
}

/**
* @brief ��ȡ����������
* @param [in]  pszInputMode   ˢ��ģʽ
* @param [in]   cTransType   ��������
* @param [out]  pszType   ˢ������
* @return ��
* @author 
* @date 
*/
void GetStripeType(const char* pszInputMode, char cTransType, char cTransAttr,char* pszType)
{
	if(pszType == NULL)
		return;
	if(memcmp(pszInputMode, "02", 2) == 0)		//ˢ������
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
	else if(memcmp(pszInputMode, "05", 2) == 0 || memcmp(pszInputMode, "95", 2) == 0)		//оƬʽ�Ӵ���
	{
		strcpy(pszType, "I");	
	}
	else if((memcmp(pszInputMode, "07", 2) == 0 || memcmp(pszInputMode, "91", 2) == 0 || memcmp(pszInputMode, "96", 2) == 0 || memcmp(pszInputMode, "98", 2) == 0))//�ǽӴ�ʽ��
	{
		strcpy(pszType, "C");
	}
	else if(memcmp(pszInputMode, "01", 2) == 0)
	{
		strcpy(pszType, "M");
	}
	else
		strcpy(pszType, "N");		//�޿�����
}


//����ˢ��Ԥ��IC��
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


