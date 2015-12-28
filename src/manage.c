/**
* @file manage.c
* @brief ϵͳ������ģ��
* @version  1.0
* @author Ѧ��
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
* @brief ���������־�������г���������һ����0
* @param in ��
* @return ��
 */
void ClearRevFlag(void)
{
	STREVERSAL stReversal;
	
	SetVarIsReversal(NO);
	SetVarHaveReversalNum(0);
	
	memset(&stReversal, 0, sizeof(STREVERSAL));
	SetVarReversalData((char *)&stReversal, sizeof(STREVERSAL));
	PubMsgDlg("�������־", "������־����ɹ�", 0, 1);

	return;
}

/**
* @brief 		�����־,�˵�����
* @param in 	��
* @return 	��
*/
void ClearFlag()
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList("1.�������־ ||2.��������־||3.��δǩ��״̬||4.����ǩ��״̬||5.��ű���־", "�����־", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
			return;

		switch(nSelect)
		{
		case 1: 
			nRet = PubConfirmDlg("�������־", "�Ƿ����������־", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("�������ˮ", MANAGEROPER))
				{
					ClearRevFlag(); 
				}
			}
			break;
		case 2: 
			nRet = PubConfirmDlg("��������־", "�Ƿ���������־  ", 0, 0);
			if( nRet == APP_SUCC )
			{
				SetVarIsLock(NO);
			}
			break;
		case 3: 
			nRet = PubConfirmDlg("��δǩ��״̬", "�Ƿ���δǩ��״̬", 0, 0);
			if( nRet == APP_SUCC )
			{
				SetVarIsLogin(NO);
			}
			break;
		case 4:
			nRet = PubConfirmDlg("����ǩ��״̬", "�Ƿ�����ǩ��״̬", 0, 0);
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
				nRet = PubConfirmDlg("��ű���־", "�Ƿ�����ű�����֪ͨ��־", 0, 0);
				if( nRet == APP_SUCC )
				{
					SetVarEmvIsScriptInform(NO);
				}
			}
			else
			{
				PubMsgDlg("��ű���־", "�޽ű�֪ͨ", 0, 1);
			}
			break;
		default:                 
			break;
		}
	}
}

/**
* @brief  �ն˹���ģ��,�˵�����
* @param in ��
* @return     ��
*/
void PosAdm(void)
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList("1.�������ˮ||2.�彻����ˮ", "���������ˮ", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}
		switch(nSelect)
		{
		case 1:
			nRet = PubConfirmDlg("�������ˮ", "�Ƿ����������ˮ", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("�������ˮ", MANAGEROPER))
				{
					ClearRevFlag(); 
				}
			}
			break;
		case 2:
			nRet = PubConfirmDlg("�彻����ˮ", "�Ƿ����������ˮ", 0, 0);
			if( nRet == APP_SUCC )
			{
				if (APP_SUCC == CheckOper("�彻����ˮ", MANAGEROPER))
				{
					PubMsgDlg("����ˮ","���������ˮ...", 0, 1);/**ME31����ˮҪ�ϰ���*/
					InitWaterFile();
					ClearSettle();
				#if defined(EMV_IC)
					EmvClearWater();
				#endif
					SetVarOfflineUnSendNum(0);
				#if defined (SUPPORT_ELECSIGN)
					SetVarElecSignUnSendNum(0);
				#endif
					PubMsgDlg("�彻����ˮ", "��ˮ����ɹ�", 0, 1);
				}
			}
			break;
		/*case 3:
			PubDeReclFile(SETTLEFILENAME);
			PubMsgDlg("����ʷ��������", "\n��ʷ��������������", 1, 5);
			break;*/
		default :
			break;
		}
	}
}


/**
* @brief  �������ܵĸ���˵�
* @param in ��
* @return     ��
*/
void OtherFunc(void)
{
	int nRet;
	int nSelect=0xff;
	
	while(1)
	{
#if defined (SUPPORT_ELECSIGN)	
		nRet = ProSelectList("1.���������ˮ||2.����ǩ������||3.LBS��λ", "��������", nSelect, &nSelect);
#else
		nRet = ProSelectList("1.���������ˮ||2.LBS��λ", "��������", nSelect, &nSelect);
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
* @brief ���ײ������ù��ܵĸ���˵�
* @param in ��
* @return     ��
*/
//�����������Ӧ��������,�Ȳ�������Ӧ����
/*		
		nRet = ProSelectList("1.���׿��ؿ���||2.�������ܿ���||3.����ˢ������||4.���㽻�׿���||5.���߽��׿���||6.�ط���������||7.�������׿���", "���׿���", nSelect, &nSelect);

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
		nRet = ProSelectList("1.���׿��ؿ���||2.���㽻�׿���", "���׿���", nSelect, &nSelect);
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
* @brief �޸Ĺ�������˵�����
* @param in ��
* @return     ��
*/
void ChgPwdMenu(void)
{
	int	nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList( "1.ϵͳ��������||2.��ȫ����", "�������",  nSelect, &nSelect);
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
* @brief ���ع���ģ��,�˵�����
* @param in ��
* @return     ��
*/
void HideManage(void)
{   
	int	nRet;
	int nSelect = 0xff;
	char szFunKey[2+1]={0};
	szFunKey[0] = '9'; //���ز˵��� 9
	
 	while(1)
	{				
		nRet = PubSelectListItemExtend("1.������������||2.��ʼ��POS����||3.���־", "���̹���", NULL, 0xFF, &nSelect, 60, szFunKey);
		if (nRet==APP_QUIT)
		{
			if (APP_SUCC == PubConfirmDlg("���̹���", "�˳����̹���", 0, 30))
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
			if (APP_SUCC == PubConfirmDlg("������������", "����ϵͳ��������", 0, 60))
			{
				SetAdminPwd(NULL);	
				PubMsgDlg("������������", "�������óɹ�", 0, 1);
			}
			break;
		case 2:
			if (APP_SUCC == PubConfirmDlg("��ʼ��POS����", "��ȷ�ϼ���ʼ", 0, 0))
			{
				PubMsgDlg("��ʼ��POS����", "�������\n     ���Ժ�...", 0, 1);
				InitCommParam();
				InitWaterFile();			
				InitOper();
				ClearSettle();						/**<�����������*/
				PubDeReclFile(SETTLEFILENAME);
			#if defined(EMV_IC)
				InitEMVParam();
				EmvClearWater();
			#endif
				InitPosDefaultParam();
				PubMsgDlg("��ʼ��POS����", "��ʼ���ɹ�", 0, 1);
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
* @brief ����ģ��,�˵�����
* @param in ��
* @return     ��
*/
/*

*/
void Manage(void)
{   
	int	nRet;
	int nSelect = 0xff;

 	while(1)
	{
		nRet = ProSelectList( "1.�̻���������||2.ϵͳ��������||3.ͨѶ��������||4.�ն���Կ����||5.�������||6.��ӡ��������||7.������������", "ϵͳ����", nSelect, &nSelect);
		if (nRet==APP_QUIT)
		{
			if (APP_SUCC == PubConfirmDlg("ϵͳ����", "�˳�ϵͳ����", 0, 30))
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
				PubMsgDlg("ϵͳ����","�������ֶ�����",3,5);
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
		"                ",/**<����֤ͨ�������������*/
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
		//��������������̳�ʱʱ��
		stPinpadParam.cTimeout = DEFAULT_PIN_INPUT_TIME;
		nRet = PubInitSecrity(nMode, &stPinpadParam);
		if (APP_SUCC != nRet)
		{
			PubMsgDlg("�豸���", "����ģ������д�", 3, 10);
			return APP_FAIL;
		}
		Ums_SetMainKeyFromMC();
		SetControlChkInside(NO);
	}
	else if ((GetVarIsPinpad() == YES) && (GetControlChkPinpad() == YES))
	{
		SetControlChkInside(YES);
		PubClearAll();
		PubDisplayStrInline(0, 2, "�����������");
		PubDisplayStrInline(0, 4, "���Ժ�...");
		PubUpdateWindow();

		nMode = SECRITY_MODE_PINPAD;
		
		nRet = GetVarUID(stPinpadParam.szUid);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("�豸���", "��������û�IDû������", 3, 10);
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
			PubMsgDlg("�豸���", "�������δ��", 3, 10);
			
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
* @brief �ն���Կ�����ܲ˵�
* @param in ��
* @return     ��
*/
int MainkeyManage(void)
{
	int	nRet;
	int nSelect=0xff;
	
	while(1)
	{
		nRet = ProSelectList( "1.�����Կ||2.KEK����||3.�����������||4.��������Կ����||5.����DES�㷨", "��Կ����",  nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return nRet;
		}
		switch(nSelect)
		{
		case 1:
			if( YES != GetVarIsPinpad())
			{
				nRet = PubConfirmDlg("��Կ����", "�Ƿ��������Կ?", 0, 0);
				if (nRet == APP_SUCC)
				{
					PubClearKey();
				}
			}
			else
			{
				PubMsgDlg("��Կ����", "���������̲�֧�ִ������", 0, 10);
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
* @brief 	����������̵��ѻ����������Կ����Կ����pinpad�˼���pos���˽���
* @param in psKeyBuf ��ΪNULL��Ҫ���ֹ�����,��ΪNULL,�������Կָ��
* @param in nKeyIndex �����ŵ���Կ������
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
	char *pszTitle = "��Կ����";

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
		ASSERT_QUIT(PubInputDlg(pszTitle, "������IC���ѻ�����PIN������Կ", szAscMainKey, &nLen, nLen, nLen, 0, INPUT_MODE_STRING));
		PubMsgDlg(NULL, "����д����Կ", 0, 1);
		PubAscToHex((uchar *)szAscMainKey, 16, 0, (uchar *)szBcdMainKey);
	}
	else
	{
		memcpy(szBcdMainKey, psKeyBuf, 8);
	}
	PubSoftDes(szBcdMainKey, szBcdMainKey, szMainKey);/**<������ܹ��Ĺ�����Կ*/
	nRet = PubLoadMainKey(nKeyIndex, szBcdMainKey, 8);/**<PINPAD�ϱ������Կ��Ϊ����Կ*/
	PubSetCurrentMainKeyIndex(nKeyIndex);
	nRet += PubLoadWorkKey(KEY_TYPE_PIN, szMainKey, 8, NULL);/**<PINPAD�ϱ��湤����Կ(����)*/
	if (nRet != APP_SUCC)
	{
		PubMsgDlg(pszTitle, "������̰�װIC���ѻ�����PIN ������Կʧ��", 3, 10);
	}
	return nRet;
}
#endif

#if defined (SUPPORT_ELECSIGN)

/**
* @brief 		����ǩ������
* @param in 	
* @return 	��
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
		ASSERT_RETURNCODE(ProSelectList("1.ǩ�ֳ�ʱʱ��||2.ǩ��ȷ������||3.�ְ���������", "����ǩ������", nSelect, &nSelect));

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
		ASSERT_RETURNCODE(ProSelectList("1.ǩ����̧ͷ||2.��ӡ����||3.��������||4.������ӡ����", "��ӡ", 0xFF, &nSelect));
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
* @brief  �ն˹���ģ��,ǩ������ӡ����
* @param in ��
* @return     ��
*/
static int PrintManage(void)
{
	int nSelect=0xff;
	
	while(1)
	{
		ASSERT_RETURNCODE(ProSelectList("1.��ӡ����||2.������ӡ", "��ӡ��������", 0xFF, &nSelect));
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
		ASSERT_RETURNCODE(ProSelectList("1.��ˮ������||2.���κ�����||3.����ױ���||4.ϵͳ��ǰʱ��||5.����ǹ����", "ϵͳ��������", 0xFF, &nSelect));
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
		ASSERT_RETURNCODE(ProSelectList("1.�̻�������||2.�ն˺�����||3.�̻���������||4.�̻�Ӣ������||5.Ӧ����ʾ����", "�̻���������", 0xFF, &nSelect));
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
		ASSERT_RETURNCODE(ProSelectList("1.�����ط�����||2.�����ط�����||3.ǩ���ط�����||4.TMS���Դ���", "�ط���������", 0xFF, &nSelect));
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
				PubMsgDlg("��ܰ��ʾ", "�ݲ�֧�ָù���", 0, 2);	
			#endif
			break;
		case 4:
			#ifdef USE_TMS
				SetFuncTmsReSendNum();
			#else
				PubMsgDlg("��ܰ��ʾ", "�ݲ�֧�ָù���", 0, 2);	
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
		ASSERT_RETURNCODE(ProSelectList("1.���ѳ���ˢ��||2.��Ȩ��ɳ���ˢ��", "����ˢ������", 0xFF, &nSelect));
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
		ASSERT_RETURNCODE(ProSelectList("1.���ѳ�������||2.��Ȩ��������||3.��Ȩ��ɳ�������||4.��Ȩ�����������", "�������ܿ���", 0xFF, &nSelect));
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
		ASSERT_RETURNCODE(ProSelectList("1.�����Զ����ͱ���||2.�������ͷ�ʽ", "���߽��׿���", 0xFF, &nSelect));
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

