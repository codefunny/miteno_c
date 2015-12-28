/**
* @file inout.c
* @brief �������ģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

/**
* @brief ��������
* @param in pszPan ���ڼ����õĿ���
* @param in pszAmount ������ʾ�õĽ��
* @param in pszPin �������Ĺ̶�8���ֽ�
* @return ����
*/

int GetPin(const char *pszPan, const char *pszAmount, char *pszPin)
{
	int nRet, nMainKeyNo = 0, nPinLen=0, nMode = PINTYPE_WITHOUTPAN;
	char szDispAmt[64+1] = {0};

	GetVarMainKeyNo(&nMainKeyNo);
	PubSetCurrentMainKeyIndex(nMainKeyNo);

	if (NULL != pszAmount)
	{
		/*<������������ʾ*/
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
			/*<������������ʾ*/
			PubDispPinPad(1, szDispAmt);
			PubDispStrFormat(DISPLAY_ALIGN_BIGFONT, 2, "|R%sԪ", szDispAmt);
		}		
		PubDisplayStrInline(0, 3, "��ֿ�����������");
		PubUpdateWindow();

		nRet = PubActiveKey(DESMODE_3DES == GetVarEncyptMode() ? DESMODE_3DES : DESMODE_DES,\
		nMainKeyNo, KEY_TYPE_PIN);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("��������", "������̼���ʧ�ܣ�������ǩ��", 3, 10);
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
		//�������������Ϣ
		PubDispErr("��������");
		return APP_FAIL;
	}
	return APP_SUCC;
}


/**
* @brief ѡ����Ȩ��ʽ
* @param out  psCodeѡ�����Ȩ��ʽ�̶�2���ֽ�
* @return ����
* @li APP_SUCC�ɹ�
* @li APP_QUITȡ��
*/
int GetAuthorCode(char *pszTitle, char *psCode)
{
	int nRet, nSelect, nIsSmallGeneAuth;
		
	nIsSmallGeneAuth = GetVarIsSmallGeneAuth();
	while(1)
	{
		if(nIsSmallGeneAuth == YES)
		{
			nRet = ProSelectList("1.POS|2.�绰|3.С�����Ȩ", "ѡ����Ȩ��ʽ:", 0xFF, &nSelect);
		}
		else
		{
			nRet = ProSelectList("1.POS|2.�绰", "ѡ����Ȩ��ʽ:", 0xFF, &nSelect);
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
* @brief ѡ�������֯����
* @param out  psInterOrgѡ��Ĺ�����֯����̶�3���ֽ�
* @return ����
* @li APP_SUCC�ɹ�
* @li APP_QUITȡ��
*/
int GetInterOrg(char *pszTitle, char *psInterOrg)
{
	int nRet, nSelect;

	while(1)
	{
		nRet = ProSelectList("1.CUP|2.VIS|3.MCC|4.MAE|5.JCB|6.DCC|7.AMX", "ѡ����ʹ�˾����", 0xFF, &nSelect);
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
	sprintf(szMsg, "����:%s\n", pszCouponID);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;
}

int DispTelno(const char *pszTelno, int nTimeOut)
{
	char szMsg[64+1];
	
	PubClear2To4();
	sprintf(szMsg, "�ֻ���:%s\n", pszTelno);
	ASSERT_FAIL(PubConfirmDlg(NULL, szMsg, 0, nTimeOut));
	
	return APP_SUCC;

}

