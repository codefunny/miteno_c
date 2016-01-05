/**
* @file main.c
* @brief ��ģ�飨������ڣ�
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#define _NO_EXTERN_GLOBAL_VAR_

#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
/**
* �ڲ���������
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
* @brief �������
* @param ��
* @return ����
*/
int main(void)
{
	int nOperRole = 0xff;
	char szOper[OPERCODELEN + 1];

	PubInitUiParam();
	ASSERT_QUIT(Ums_CheckMcVer());	/*��Ҫ:�ж����ذ汾*/
	PubLuaOpen();
	PubLuaDisplay("LuaSetUIStyle");	/*����ϵͳ���*/
	RecordVersion();	/*��¼�汾��Ϣ*/
	PubKbHit();						/*���������*/

	/**
	* �汾У��(�״����м��)
	*/
	FirstRunChk();
	/**
	* ģ���ʼ��,��Ҫ!!!
	*/
	AppInit();

#if defined(USE_TMS)
	/**<����Ƿ���Ӧ�û��߲����ĸ���*/
	DoTmsTask(TMS_POS_CHKUPDATA);
	GetTmsParamDown();
#endif

	Cup_InitScrLed();

	SetControlChkPinpad(YES);

	
	/*
	* ��������
	*/
	while(1)
	{
		/**
		* �豸���
		*/
		ChkPdAndRF();

		/**
		* ͨѶ��ʼ��
		*/
		CommInit();
		memset(szOper, 0, sizeof(szOper));
		GetWaterOper(szOper);/**<ȡ�������׵�(����ˮ)����Ա����*/
		SetOperLimit(szOper);

		GetVarLastOperInfo(szOper, &nOperRole);/**<ȡ����½�Ĳ���Ա��Ϣ(�����)*/
		SetCurrentOper(szOper, NULL, nOperRole);/**<������½�Ĳ���Ա��Ϣ���ø�����Աģ��*/

		if (ChkIsMCReturn() == APP_SUCC || nOperRole != NORMALOPER || YES != GetVarIsLogin())
		{
			if (OperLogon() != APP_SUCC)
			{
				/*ȡ�������õ�ϵͳ���*/
				PubLuaDisplay("LuaUnSetUIStyle");
				PubLuaClose();
				CommDump();
				Cup_ExitScrLed();
				return APP_QUIT;
			}
		}
		GetCurrentOper(szOper, NULL, &nOperRole);/**<�Ӳ���Աģ���ȡ��ǰ�Ĳ���Ա��Ϣ*/
		SetVarLastOperInfo(szOper, (char)nOperRole);/**<��������½�Ĳ���Ա��Ϣ*/

		/**
		* ϵͳ����Ա
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
* @brief �״����м��
* @param ��
* @return
* @li APP_SUCC
*/
static int FirstRunChk(void)
{
	if (APP_SUCC != IsFirstRun())
	{
		ResetDefaultParam();
	}
	else   //�汾����,����������ʼ��
	{
		VerUpdateChk();
	}
	return APP_SUCC;
}


/**
* @brief ��ʼ��ϵͳ
* @param
* @return
* @li APP_SUCC
*/
static int AppInit(void)
{
	int nRet = 0;

	PubClearAll();
	PubDisplayStrInline(0, 2, "������");
	PubDisplayStrInline(0, 4, "���Ժ�..");
	PubUpdateWindow();
	nRet = ExportPosParam();
	nRet += ExportCommParam();
	if (APP_SUCC != nRet)
	{
		PubMsgDlg("����", "POS������ʧ", 3, 10);
	}

#if defined(EMV_IC)
	if (ExportEMVModule() != APP_SUCC)
	{
		return APP_FAIL;
	}
#endif
	if (APP_SUCC != PubInitPrint(PRINTFILE, DEFAULT_PAGE_LEN))
	{
		PubDispErr("��ʼ����ӡ");
		return APP_FAIL;
	}
	SetParamFromIni();
	ExportSslCa();
	return nRet;
}

/**
* @brief Ӧ����Ҫ���ײ���
* @param ��
* @return
* @li APP_SUCC
* @li APP_QUIT
*/
static int AppTrans(void)
{
	int nRet = 0;
	char cMenuSel = 0;			/*Ĭ�Ͻ��뽻�׽���*/
	char szDateTime[14+1];
	char szLastLoginDateTime[5+1];
	
	/*�ж��Ƿ�����KEK*/
	if (YES == GetVarDownKek())
	{
		nRet = DownloadKek();
		if (nRet != APP_SUCC)
		{
			return APP_FAIL;
		}
	}

	/**
	*POSǩ��
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
	SaveMCEnterInfo();//��������ʱ�����Ӻ������
	DealSettleTask();//��������ж�
	
	while(1)
	{
		/**
		* ����ϵͳҪ��
		*/
		DealSysReq();

		/**
		* ����Ҫ��TMS��Ӧ�ý�������ﵽTMSϵͳ�·����ۼƽ����������Ҫǿ����������
		* �ù��ܽ���ԣ���ǰ�����Ӧ������Ӧ��(һ���ǽ���Ӧ�ã������п�)��
		*/
		if(Ums_DealSettleNum() == KEY_FUNC_APP_SELECT)
		{
			return APP_QUIT;
		}
		
		/*
		* �ж��Ƿ��Զ�ǩ�ˣ��Զ�ǩ�˳ɹ���Ҫ�˳�
		*/
		if (YES != GetVarIsLogin())
		{
			return APP_FAIL;/**<����Ѿ�ǩ�˳ɹ����˳�*/
		}
		
		/**
		* ���뽻�ײ˵�
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

/*�˵���Ӧ�Ĺ���ID*/
enum TRANSMEUN
{
	PRINT_MENU,							/*��ӡ*/
	MANAGE_MENU,						/*����*/
	OTHERS,								/**<�����˵�*/
	REFUND_MENU,						/**<�˻���ʾ*/
	BALANCE_MENU,						/**<��ѯ��ʾ*/
	COUPON_DISP,						/**<��ȯ*/
	COUPON_VERIFY_MENU,					/**<������ȯ*/
	TEL_VERIFY_MENU,	     			/**<�ֻ���ȯ*/
	CARD_VERIFY_MENU,	       			/**<������ȯ*/
	VOID_VERIFY_MENU,					/**<��������*/
	REFUND_VERIFY_MENU,		    		/**<��֤�˻�*/
	WX_DISP,							/**<΢��֧��*/
	ALI_DISP,							/**<֧����֧��*/
	BAIDU_DISP,							/**<�ٶ�֧��*/
	JD_DISP,							/**<����֧��*/
	PRECREATE_MENU,						/**<ɨ��֧��*/
	CREATEANDPAY,						/**<����֧��*/
	WX_VOIDPAY_MENU, 					/**<΢�ų���*/
	WX_REFUND_MENU,						/**<΢���˻�*/
	WECHAT_BALANCE_MENU,				/**<΢�Ų�ѯ*/
	BAIDU_CREATEANDPAY_MENU, 			/**<�ٶ�֧��*/
	BAIDU_REFUND_MENU,					/**<�ٶ��˻�*/
	BAIDU_BALANCE_MENU,					/**<�ٶȲ�ѯ*/
	JD_PRECREATE_MENU,					/**<����֧��*/
	JD_CREATEANDPAY_MENU,				/**<����֧����ɨ*/
	JD_REFUND_MENU,						/**<�����˻�*/
	JD_BALANCE_MENU,					/**<������ѯ*/
	ALI_CREATEANDPAY_MENU, 				/**<֧��������*/
	ALI_REFUND_MENU,					/**<֧�����˻�*/
	ALI_BALANCE_MENU,					/**<֧������ѯ*/
	MEITUAN_COUPON_MENU,				/**<����*/
	PANJIN_MENU,						/**<�̽�ͨ*/
	MEITUAN_BALANCE_MENU,				/**<���Ų�ѯ*/
	DZ_MENU_DISP,						/**<���ڵ������˵�*/
	DZ_BALANCE_MENU,					/**<���ڵ�����ѯ*/
	DZ_PRECREATE_MENU,					/**<���ڵ�����ɨ*/
	DZ_CREATEANDPAY_MENU,				/**<���ڵ�����ɨ*/
	DZ_REFUND_MENU,						/**<���ڵ����˻�*/
	DZ_COUPON_MENU,						/**<���ڵ�����ȯ*/
	DZ_COUPONCONFIRM_MENU,				/**<���ڵ�������ȷ��*/
	BESTPAY_CREATEANDPAY_MENU, 			/**<��֧������*/
	BESTPAY_REFUND_MENU,				/**<��֧���˻�*/
	BESTPAY_BALANCE_MENU,				/**<��֧����ѯ*/
	ALLPAY_CREATEANDPAY_MENU, 			/**<���ܸ�����*/
	ALLPAY_REFUND_MENU, 				/**<���ܸ��˿�*/
	ALLPAY_BALANCE_MENU, 				/**<���ܸ���ѯ*/
	ALIPAY_PRECREATE_MENU,				/**<֧������ɨ*/
	BAIDU_PRECREATE_MENU,				/**<�ٶȱ�ɨ*/
	
};

//ƽ̨��ɨ�˵�
static int DoPREPlatformMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "ƽ̨��ɨ,%d/", WX_DISP);
	strcat(pszMenuResource, szTmp);

	//if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****΢��֧��,%d/", PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	//if(GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****֧����,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ٶ�Ǯ��,%d/", BAIDU_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****����Ǯ��,%d/", JD_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****���ڵ���,%d/", DZ_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
}

//ƽ̨��ɨ�˵�
static int DoPlatformMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "ƽ̨��ɨ,%d/", WX_DISP);
	strcat(pszMenuResource, szTmp);

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****΢��֧��,%d/", CREATEANDPAY);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****֧����,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ٶ�Ǯ��,%d/", BAIDU_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****����Ǯ��,%d/", JD_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****���ڵ���,%d/", DZ_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****��֧��,%d/", BESTPAY_CREATEANDPAY_MENU);
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
		sprintf(szTmp, "΢��֧��,%d/", PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "΢��֧��,%d/", CREATEANDPAY);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES))
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "΢��֧��,%d/", WX_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****΢��֧����ɨ,%d/", CREATEANDPAY);
			strcat(pszMenuResource, szTmp);
		}
		if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****΢��֧����ɨ,%d/", PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "΢��֧��,%d/", CREATEANDPAY);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "΢��֧��,%d/", PRECREATE_MENU);
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
		sprintf(szTmp, "֧����,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "֧����,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES))
*/		
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "֧����,%d/", ALI_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****֧������ɨ,%d/", ALI_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_ALIPAY_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****֧������ɨ,%d/", ALIPAY_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "֧����,%d/", ALI_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "֧����,%d/", ALIPAY_PRECREATE_MENU);
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
		sprintf(szTmp, "�ٶ�Ǯ����ɨ,%d/", ALI_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "�ٶ�Ǯ����ɨ,%d/", ALIPAY_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES))
*/		
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "�ٶ�Ǯ��,%d/", BAIDU_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****�ٶ�Ǯ����ɨ,%d/", BAIDU_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****�ٶ�Ǯ����ɨ,%d/", BAIDU_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "�ٶ�Ǯ��,%d/", BAIDU_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "�ٶ�Ǯ��,%d/", BAIDU_PRECREATE_MENU);
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
		sprintf(szTmp, "����Ǯ��,%d/", JD_PRECREATE_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if ((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== NO) && 
		(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES))
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "����Ǯ��,%d/", JD_CREATEANDPAY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)||\
		(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES))
	{
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "����Ǯ��,%d/", JD_DISP);
		strcat(pszMenuResource, szTmp);
		
		if(GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****����Ǯ����ɨ,%d/", JD_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****����Ǯ����ɨ,%d/", JD_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#else

	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "����Ǯ��,%d/", JD_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		else
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "����Ǯ��,%d/", JD_PRECREATE_MENU);
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
		sprintf(szTmp, "��  ȯ,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ֻ���ȯ,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ȯ,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ֻ���ȯ,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ȯ,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);
		
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ֻ���ȯ,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ȯ,%d/", COUPON_DISP);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);

		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	else
	{
		if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "������ȯ,%d/", COUPON_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		
		if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES)
		{	
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "�ֻ���ȯ,%d/", TEL_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "������ȯ,%d/", CARD_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}

	return APP_SUCC;
}


static int DoVoucherMenu(char * pszMenuResource)
{
	char szTmp[128];

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "��  ȯ,%d/", COUPON_DISP);
	strcat(pszMenuResource, szTmp);
	
	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****��  ��,%d/", MEITUAN_COUPON_MENU);
		strcat(pszMenuResource, szTmp);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****���ڶ�ȯ,%d/", DZ_COUPON_MENU);
		strcat(pszMenuResource, szTmp);
	}
	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****���ڶ���ȷ��,%d/", DZ_COUPONCONFIRM_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", COUPON_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES)
	{	
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****�ֻ���ȯ,%d/", TEL_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "****������ȯ,%d/", CARD_VERIFY_MENU);
		strcat(pszMenuResource, szTmp);
	}

	return APP_SUCC;
}



/**
* @brief ����˵�
* @param [out] pszMenuResource �˵��б�
* @param [out] pcMode Ӧ������
* @return
* @li APP_SUCC
* @li APP_FAIL
* @author 
* @date
*/
static int DoMenu(char * pszMenuResource)
{
	char szTmp[128];
	//ƽ̨��ɨ�˵�����
	DoPREPlatformMenu(pszMenuResource);
	//ƽ̨��ɨ�˵�����
	DoPlatformMenu(pszMenuResource);
	
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ѯ,%d/", ALLPAY_BALANCE_MENU);
		strcat(pszMenuResource, szTmp);
	}

	
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ��,%d/", ALLPAY_REFUND_MENU);
		strcat(pszMenuResource, szTmp);
	}
#if 0
	//΢��֧���˵�����
	DoWechatMenu(pszMenuResource);

	//֧�����˵�����
	DoAliPayMenu(pszMenuResource);

	//�ٶ�Ǯ���˵�����
	DoBaiduMenu(pszMenuResource);

	//����Ǯ��
	DoJDMenu(pszMenuResource);
#endif


#if 0	
	//if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "���ڵ���,%d/", DZ_MENU_DISP);
		strcat(pszMenuResource, szTmp);
		
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****���ڶ�ȯ,%d/", DZ_COUPON_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****���ڶ���ȷ��,%d/", DZ_COUPONCONFIRM_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****���ڵ�����ɨ,%d/", DZ_CREATEANDPAY_MENU);
			strcat(pszMenuResource, szTmp);
		}
		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****���ڵ�����ɨ,%d/", DZ_PRECREATE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	}
#endif

	
	//��ȯ
	//DoCouponMenu(pszMenuResource);

	//�Ҿ�
	DoVoucherMenu(pszMenuResource);

	
	/*��ӡ*/
	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "��  ӡ,%d/", PRINT_MENU);
	strcat(pszMenuResource, szTmp);

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "��  ��,%d/", OTHERS);
	strcat(pszMenuResource, szTmp);

	memset(szTmp, 0, sizeof(szTmp));
	sprintf(szTmp, "��  ��,%d/", MANAGE_MENU);
	strcat(pszMenuResource, szTmp);

	return APP_SUCC;
}



int DoOtherMenu(char * pszMenuResource)
{
	char szTmp[128];

	
	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "�̽�ͨ,%d/", PANJIN_MENU);
		strcat(pszMenuResource, szTmp);
	}
	
	//��ѯ
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES
		|| GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES
		|| GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES
		)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "���ײ�ѯ,%d/", REFUND_MENU);
		strcat(pszMenuResource, szTmp);
	
		if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****΢�Ų�ѯ,%d/", WECHAT_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****֧����,%d/", ALI_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****�ٶȲ�ѯ,%d/", BAIDU_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****������ѯ,%d/", JD_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
	
		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****���Ų�ѯ,%d/", MEITUAN_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
		

#if 0


		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****������ѯ,%d/", DZ_BALANCE_MENU);
			strcat(pszMenuResource, szTmp);
		}
#endif		
	}

	if(GetTieTieSwitchOnoff(TRANS_VOID_VERIFY)== YES)
	{
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "��  ��,%d/", VOID_VERIFY_MENU);
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
		sprintf(szTmp, "��  ��,%d/", REFUND_MENU);
		strcat(pszMenuResource, szTmp);

		if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****ȯ�˻�,%d/", REFUND_VERIFY_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****΢���˻�,%d/", WX_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****�ٶ��˻�,%d/", BAIDU_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****�����˻�,%d/", JD_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****֧�����˻�,%d/", ALI_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}

		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****��֧���˻�,%d/", BESTPAY_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
#if 0	

		//if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "*****���������˻�,%d/", DZ_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
		
	
		//if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES)
		{
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "****������ȯ�˻�,%d/", DZ_REFUND_MENU);
			strcat(pszMenuResource, szTmp);
		}
#endif		

	}


	return APP_SUCC;
}


/**
* @brief ������˵��������Ӳ˵���Ĳ˵�ѡ��
* @param ��
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
		case -5://ȷ��
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
		//��ѯ
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
* @brief Ӧ�ý��ײ˵�ѡ��
* @param ��
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
* @brief Ӧ�ý��ײ˵�ѡ��
* @param ��
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
		AutoDoReversal();	/*�Զ�����*/
		nRet = PubLuaDisplay2("LuaMenuFunc", szPrm);
		
		switch(nRet)
		{
		case APP_FUNCQUIT:
			return KEY_FUNC_APP_SELECT;
		case -5://ȷ��
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
* @brief ���н��ʴ���(���жϲ���Ա��Ȩ��)
* @param ��
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
		PubMsgDlg("������", "����ˮ���������\n\n��������˳���", 1, 30);
		return APP_FAIL;
	}
	//ASSERT_QUIT(DispTotal());
	if (Settle(0) != APP_SUCC)
	{
		PubMsgDlg("����ʧ��", "�����½���", 3, 10);
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
* @brief ����ǩ�˴���(���жϲ���Ա��Ȩ��)
* @param ��
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
		PubMsgDlg("ǩ��", "�н�����ˮ�Ƚ���", 3, 10);
		return APP_FAIL;
	}
	if (YES == GetVarIsLogin())
	{
		nRet =  LogOut();
	}
	else
	{
		PubMsgDlg("ǩ��", "POS�Ѿ�ǩ��", 0, 3);
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

	ASSERT_QUIT(PubConfirmDlg("����", "�Ƿ������ն�?", 1, 10));
	PosLockUnLock();
	GetCurrentOper(szOperNo, NULL, &nRole);
	SetVarLastOperInfo(szOperNo, (char)nRole);
	return APP_SUCC;
}


/**
* @brief ������ĻУ׼
* @param
* @return
* @li
*/
int DoScrCalibrate(void)
{
	const char *pszTitle = "��ĻУ׼";
	
	if (APP_SUCC == CheckOper(pszTitle, MANAGEROPER))
	{
		if(APP_FUNCQUIT == PubScrTSCalibrate())
		{
			PubMsgDlg(pszTitle, "�޴���ģ��,����У׼", 3, 5);
			return APP_SUCC;
		}
		PubClearAll();
		PubDisplayGen(3, "��������");
		PubUpdateWindow();
		NDK_SysReboot();
	}
	return APP_SUCC;
}

int DoSetCommType(void)
{
	if(APP_SUCC == CheckOper("ͨѶ��ʽ", MANAGEROPER))
	{
		SetFuncCommType();
	}

	return APP_SUCC;
}

/**
* @brief ���й���˵�ѡ��
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int MenuManage(void)
{
	STMENUTYPE stMenu[] =
	{
		{"ǩ  ��",  MenuTransLogin},
		{"ǩ  ��",  DoLogOut},
		{"���ײ�ѯ",MenuTransFind},
		{"��  Ա",  Operate},
		{"���ߺ���",DoSetPreDialNum},
		{"��  ��",  DoSettle},
		{"�����ն�",DoPosLockAndUnLock},
		{"��  ��", 	Version},
		{"ͨѶ��ʽ",DoSetCommType},
		{"��ĻУ׼",DoScrCalibrate},
		{"", NULL}
	};
	return PubOpenEasyMenu(stMenu, "����", MENU_MODE_AUTOESC);
}



/**
* @brief ���ײ�ѯ,��ѯ������ϸ����ѯ���׻��ܡ���ƾ֤�Ų�ѯ
* @param ��
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
		ASSERT_QUIT(ProSelectList("1.��ѯ������ϸ||2.��ѯ���׻���||3.��ƾ֤�Ų�ѯ||4.��EMV�ɹ���Ϣ||5.��EMVʧ����Ϣ", "���ײ�ѯ", 0xFF, &nSelect));
#else
		ASSERT_QUIT(ProSelectList("1.��ѯ������ϸ||2.��ѯ���׻���||3.��ƾ֤�Ų�ѯ", "���ײ�ѯ", 0xFF, &nSelect));
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
* @brief ǩ����POSǩ��������Աǩ��������Ա����ǩ��
* @param ��
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

	if (nMC_Random != nSave_Random)/**<���ڸ���Ӧ��ǩ��ʱ�и���posʱ�ӣ��˴����Ƚ����ֵ*/
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
* @brief ��֧�ֵ���ʾ
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int FUNC_NO_SUPPORT(void)
{
	PubMsgDlg("��ܰ��ʾ", "�ݲ�֧�ָý���", 3, 5);
	return APP_FAIL;
}

/**
* @brief ���ESIM������������Ƿ����
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int ChkPdAndRF(void)
{
	/**
	* ���������̣���Կģ���ʼ��
	*/
	ChkPinpad();
	/**
	* ��⡢��ʼ��������
	*/
	ChkRF();

	return APP_SUCC;
}

#if 0

/**
*�����ֽ��ݼ�
*/
int TxnEcSale(void)
{
	int nRet;
	int cInput = INPUT_INSERTIC;
	int nTrueInput;
	int nInputMode;
	char *pszTitle = "�����ֽ�";
	char szAmount[12+1] = {0};

	if(YES != GetVarIsIcFlag())
	{
		PubMsgDlg(NULL,  "�ݲ�֧��IC������", 3, 5);
		return APP_FAIL;
	}

	Cup_OpenLedBackGround();
	
	while(1)
	{
		int nAmtLen = 12;
		memset(szAmount, 0, sizeof(szAmount));
		ASSERT_QUIT(PubInputAmount(pszTitle, "��������:", szAmount, &nAmtLen, INPUT_MODE_AMOUNT, INPUT_AMOUNT_TIMEOUT));
		/**
		* ��������Ч��
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
*����ǿ�ݼ�
*/
int TxnCrDr(void)
{
	int nSelect;
	int nRet;
	int nTrueInput;
	char szAmount[12+1] = {0};
	
	ASSERT_QUIT(ProSelectList("1.�� ��||2.Ȧ ��", "�����", 0xFF, &nSelect));
	switch(nSelect)
	{
	case 1:
		if(YES != GetVarIsIcFlag())
		{
			PubClearAll();
			PubMsgDlg(NULL,  "�ݲ�֧��IC������", 3, 5);
			return APP_FAIL;
		}
		
		ASSERT_QUIT(TradeInit("����", TDI_DEFAULT, TRANS_SALE, 0, YES));
		/**
		* ������
		*/
		
		while(1)
		{
			int nAmtLen = 12;			
			memset(szAmount, 0, sizeof(szAmount));
			ASSERT_QUIT(PubInputAmount("����", "��������:", szAmount, &nAmtLen, INPUT_MODE_AMOUNT, INPUT_AMOUNT_TIMEOUT));
			/**
			* ��������Ч��
			*/
			if ( CheckTransAmount(szAmount, TRANS_SALE) == APP_SUCC )
			{
				break;
			}
		}
		nTrueInput = INPUT_PLEASE_INSERTIC; //ֻ֧��IC�忨
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
