/**
* @file comm.c
* @brief ͨѶģ��
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
static STAPPCOMMPARAM gstAppCommParam; /**<Ӧ��ͨѶ�����ṹ����*/
static STAPPCOMMRESERVE	gstAppCommReserve;/*����WIFIͨѶ����*/

/**
* �����ڲ�����
*/
static int CommParamAppToPub(const STAPPCOMMPARAM *, const STAPPCOMMRESERVE *, STCOMMPARAM *);
static int SetFuncCommMode(void);
static int SetFuncCommIsPreDial(void);
static int SetFuncCommTimeOut(void);
static int SetFuncCommTpdu(void);
static int SetFuncCommTelNo(void);
static int SetFuncCommWLMDialNum(void);
static int SetFuncCommUser(void);
static int SetFuncCommPassWd(void);
static int SetFuncCommAPN(void);
static int SetFuncCommPort(void);
static int SetFuncCommBackPort(void);
static int SetFuncCommIp(void);
static int SetFuncCommBackIp(void);
static int SetFuncCommIpAddr(void);
static int SetFuncCommMask(void);
static int SetFuncCommGate(void);
static int SetFuncCommReDialNum(void);
static int SetFuncAuxIsAddTpdu(void);
static int SetFuncDnsName(void);
static int SetFuncDnsName2(void);
static int SetFuncDNSIp1(void);
static int SetFuncDNSIp2(void);
static int SetFuncDNSPort(void);
static int SetFuncDNSPort2(void);
static int SetFuncWifiMode(void);
static int SetFuncWifiPwd(void);
static int SetFuncWifiSsid(void);
static int SetFuncIsSSL(void);
static int SetFuncIsUseDns(void);
static int SetFuncIsSendSSL(void);
static int SetFuncIsDhcp(void);
static YESORNO GetControlCommInit(void);
static int SetFuncIsCommSendRecvContinue(void);
/**
* �ӿں���ʵ��
*/
static char gcIsCommInit = YES;/**<���ڱ�ʶ�Ƿ���Ҫ��ʼ��ͨѶ*/
/**
* @brief �����Ƿ���ҪͨѶ��ʼ��
* @param in cIsInit �Ƿ��ʶ��YES/NO
* @return  ��
*/
void SetControlCommInit(void)
{
	gcIsCommInit = YES;
}
/**
* @brief ����Ƿ���Ҫ��ҪͨѶ��ʼ��
* @return
* @li YES
* @li NO
*/
static YESORNO GetControlCommInit(void)
{
	if (gcIsCommInit==YES)
	{
		return YES;
	}
	else
	{
		return NO;
	} 
}


/**
* @brief ��ʼ��ͨѶ���������䲿��
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitCommReserve(void)
{
	STRECFILE stCommParamfile;

	memset(&gstAppCommReserve,0,sizeof(gstAppCommReserve));
	gstAppCommReserve.cIsDHCP = 1;
	strcpy(gstAppCommReserve.szWifiSsid,"Staff_UMS");
	strcpy(gstAppCommReserve.szWifiKey,"staffumschina");
	gstAppCommReserve.cWifiKeyType = WIFI_KEY_TYPE_ASCII;
	gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WEP_OPEN;
	memcpy(gstAppCommReserve.szDomain2,gstAppCommParam.szDomain,50);
	memcpy(gstAppCommReserve.szDnsPort1,gstAppCommParam.szPort1,6);
	memcpy(gstAppCommReserve.szDnsPort2,gstAppCommParam.szPort2,6);
	strcpy(gstAppCommReserve.szStationIp,"210.22.91.77");
	strcpy(gstAppCommReserve.szStationPort,"5601");
	memcpy(gstAppCommReserve.sStationTpdu, "\x60\x00\x00\x00\x00", 5);
	gstAppCommReserve.cLbsTimeOut = 5;
	/*Ϊ�˲�Ӱ��ԭ���Ĳ���,�����һ��������WIFI�ļ�*/
	memset(&stCommParamfile, 0, sizeof(STRECFILE));
	strcpy( stCommParamfile.szFileName, FILE_APPCOMMRESERVE);
	stCommParamfile.cIsIndex = FILE_NOCREATEINDEX;	
	stCommParamfile.unMaxOneRecLen = sizeof(STAPPCOMMRESERVE);
	stCommParamfile.unIndex1Start = 0;
	stCommParamfile.unIndex1Len = 0;
	stCommParamfile.unIndex2Start = 0;
	stCommParamfile.unIndex2Len = 0;
	ASSERT_FAIL(PubCreatRecFile(&stCommParamfile));
	ASSERT_FAIL(PubAddRec(FILE_APPCOMMRESERVE,(char *)&gstAppCommReserve));

	return APP_SUCC;
}

/**
* @brief ��ʼ��ͨѶ����,�״�����ʱ��Ҫִ��һ��
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitCommParam(void)
{
	STRECFILE stCommParamfile;
	char szBuf[50] = {0};
	
	memset(&gstAppCommParam, 0, sizeof(STAPPCOMMPARAM));
	if (APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_WIRELESS,szBuf))
	{
		gstAppCommParam.cCommType = COMM_DIAL;
	}
	else if (0 == memcmp(szBuf, "GPRS", 4))
	{
		gstAppCommParam.cCommType = COMM_GPRS;
	}
	else if (0 == memcmp(szBuf, "CDMA", 4))
	{
		gstAppCommParam.cCommType = COMM_CDMA;
	}
	else
	{
		gstAppCommParam.cCommType = COMM_DIAL;
	}

	gstAppCommParam.cPreDialFlag = 1;
	strcpy(gstAppCommParam.szPreDial,"");
	strcpy(gstAppCommParam.szTelNum1, "96305");
	strcpy(gstAppCommParam.szTelNum2, "96305");
	strcpy(gstAppCommParam.szTelNum3, "96305");
	gstAppCommParam.cTimeOut = 60;
	memcpy(gstAppCommParam.sTpdu, "\x60\x00\x03\x00\x00", 5);
	gstAppCommParam.cReDialNum = 3;
	gstAppCommParam.cReSendNum = 3;
	gstAppCommParam.cOffResendNum=3;
	strcpy(gstAppCommParam.szAPN1, "CMNET");
	strcpy(gstAppCommParam.szAPN2, "CMNET");
	strcpy(gstAppCommParam.szDNSIp1, "192.168.30.1");
	strcpy(gstAppCommParam.szDNSIp2, "192.168.30.1");
	strcpy(gstAppCommParam.szDNSIp3, "192.168.30.1");
	strcpy(gstAppCommParam.szIp1, "218.66.48.231");
	strcpy(gstAppCommParam.szIp2, "218.66.48.231");
	strcpy(gstAppCommParam.szPort1, "8584");
	strcpy(gstAppCommParam.szPort2, "8584");
	strcpy(gstAppCommParam.szIpAddr, "192.168.5.114");
	strcpy(gstAppCommParam.szMask, "255.255.255.0");
	strcpy(gstAppCommParam.szGate, "192.168.5.254");
	gstAppCommParam.cMode = 1;
	
	if (gstAppCommParam.cCommType == COMM_GPRS)
	{
		strcpy(gstAppCommParam.szWirelessDialNum, "D*99***1#");/**<"D*99***1#"*/
	}
	else if (gstAppCommParam.cCommType == COMM_CDMA)
	{
		strcpy(gstAppCommParam.szWirelessDialNum, "D#777");/**<"D#777"*/
		gstAppCommParam.cMode = 0;
	}
	else
	{
		strcpy(gstAppCommParam.szWirelessDialNum, "D*99***1#");/**<"D*99***1#"*/
	}	
	strcpy(gstAppCommParam.szUser, "card");
	strcpy(gstAppCommParam.szPassWd, "card");
	strcpy(gstAppCommParam.szSIMPassWd, "1234");
	strcpy(gstAppCommParam.szDomain, "www.163.com");
	gstAppCommParam.cAuxIsAddTpdu= 1;
	gstAppCommParam.cIsSSL = 0;
	gstAppCommParam.cIsSendSSLAuth = 1;
	gstAppCommParam.cIsDns = 0;
    gstAppCommParam.cIsCommSendRecvContinue = 0;

	/**<����POSͨѶ�����ļ�*/	
	memset(&stCommParamfile, 0, sizeof(STRECFILE));
	strcpy( stCommParamfile.szFileName, FILE_APPCOMMPARAM);
	stCommParamfile.cIsIndex = FILE_NOCREATEINDEX;						/**< ���������ļ�*/
	stCommParamfile.unMaxOneRecLen = sizeof(STAPPCOMMPARAM);
	stCommParamfile.unIndex1Start = 0;
	stCommParamfile.unIndex1Len = 0;
	stCommParamfile.unIndex2Start = 0;
	stCommParamfile.unIndex2Len = 0;
	ASSERT_FAIL(PubCreatRecFile(&stCommParamfile));	
	ASSERT_FAIL(PubAddRec(FILE_APPCOMMPARAM,(char *)&gstAppCommParam));

	ASSERT_FAIL(InitCommReserve());

	return APP_SUCC;
}

/**
* @brief ����ͨѶ����,����ʱ����һ�μ���
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ExportCommParam(void)
{
	int nLen;
	
	memset(&gstAppCommParam, 0, sizeof(STAPPCOMMPARAM));
	ASSERT_FAIL(PubReadOneRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));

	nLen = 10;
	MC_GetPubParam(MODEM_PRE_DAIL_NUM, gstAppCommParam.szPreDial, (uint *)&nLen);

	//��չ����ȡ�����ȳ�ʼ��,��ȡ
	memset(&gstAppCommReserve,0,sizeof(STAPPCOMMRESERVE));
	if(PubReadOneRec(FILE_APPCOMMRESERVE, 1, (char *)&gstAppCommReserve) != APP_SUCC)
	{
		ASSERT_FAIL(InitCommReserve());
	}

	return APP_SUCC;
}


/**
* @brief ��ʼ��ͨѶ�豸
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommInit(void)
{
	int nRet = 0;
	STCOMMPARAM stPubCommParam;
	static STCOMMPARAM stOldPubCommParam = {0};
	static char cInitFlag = 0;

	memset(&stPubCommParam, 0, sizeof(STCOMMPARAM));
	CommParamAppToPub(&gstAppCommParam, &gstAppCommReserve, &stPubCommParam);

	if (memcmp(&stOldPubCommParam, &stPubCommParam, sizeof(STCOMMPARAM)) != 0 || 0 == cInitFlag || YES == GetControlCommInit())
	{
		PubLuaDisplay("LuaCommInit");
		nRet = PubCommInit(&stPubCommParam);
		if (nRet != APP_SUCC)
		{
			PubDispErr("ͨѶʧ��");
			cInitFlag = 0;
			return APP_FAIL;
		}
		memcpy(&stOldPubCommParam, &stPubCommParam, sizeof(STCOMMPARAM));/**<����ɵ�ͨѶ����*/
		cInitFlag = 1;
		gcIsCommInit = NO;
	}

	return APP_SUCC;
}

/**
* @brief Ԥ����
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommPreDial(void)
{	
	int nRet = 0;
	
	nRet = PubCommPreConnect();
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}
	return APP_SUCC;

}

/**
* @brief ����ʹ�ñ��õ�������������
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/

int ConnectWithBakDns()
{
	int nRet;
	STCOMMPARAM stPubCommParam;
	
	if(gstAppCommParam.cCommType == COMM_DIAL)
	{
		return APP_FAIL;
	}
	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FAIL;
	}

	memset(&stPubCommParam,0,sizeof(STCOMMPARAM));
	PubGetCommParam(&stPubCommParam);
	memcpy(stPubCommParam.stServerAddress.szDN, gstAppCommReserve.szDomain2, 50);
	PubSetCommParam(&stPubCommParam);

	ASSERT_FAIL(SetSslMode());
	
	nRet = PubCommConnect();
	if(nRet == APP_QUIT)
	{	
		return APP_QUIT;
	}
	if(nRet == APP_SUCC)
	{
		//����ɹ���,�����Ͳ��Ļ�ȥ�ˣ��´�ֱ�ӳɹ�
		return APP_SUCC;
	}
	
	/*�������ʧ�ܣ��Ѳ������ȥ*/
	memcpy(stPubCommParam.stServerAddress.szDN, gstAppCommParam.szDomain, 50);
	PubSetCommParam(&stPubCommParam);

	return APP_FAIL;
}

/**
* @brief ����ͨѶ
* @param  ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommConnect(void)
{
	int nRet = 0;
	char szPrm[100] = {0};

	sprintf(szPrm, "%d", gstAppCommParam.cCommType);
	PubLuaDisplay2("LuaCommConnect", szPrm);

	//LBS��վ��Ϣ���ͣ��ɹ�ʧ�ܶ�����
	Ums_DealStationInfo();
	
	//����sslģʽ
	ASSERT_FAIL(SetSslMode());
	
	nRet = PubCommConnect();
	if (nRet == APP_QUIT)
	{
		return APP_QUIT;
	}
	if (nRet != APP_SUCC)
	{
		nRet = ConnectWithBakDns();
		if (nRet == APP_QUIT)
		{
			return APP_QUIT;
		}
		if (nRet != APP_SUCC)
		{
			PubDispErr("ͨѶʧ��");
			return APP_FAIL;
		}
	}
	
	return APP_SUCC;
}


/**
* @brief ����TPDUͷ
* @param [in] [out] ������Ҫ�ӵ�����ָ�룬�����Ѽ�TPDU������ָ��
* @return [in] [out] ������Ҫ�ӵ����ݳ���ָ�룬�����Ѽ�TPDU�����ݳ���ָ��
* @return
* @li APP_SUCC
* @li APP_FAIL
* @author 
* @date
*/
static int AddTpdu(char *psBuf, uint *punLen)
{
	char sTemp[MAX_SEND_SIZE];

	if (NULL == psBuf ||NULL == punLen)
	{
		return APP_FAIL;
	}
	memcpy(sTemp, psBuf, *punLen);
	memcpy(psBuf, gstAppCommParam.sTpdu, 5);
	memcpy(psBuf + 5, sTemp, *punLen);
	*punLen += 5;
	return APP_FAIL;
}

/**
* @brief ɾ��TPDUͷ
* @param in out ������Ҫɾ��������ָ�룬������ɾ��TPDU������ָ��
* @return in out ������Ҫɾ�������ݳ���ָ�룬������ɾ��TPDU�����ݳ���ָ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int DelTpdu(char *psBuf, uint *punLen)
{
	char sTemp[MAX_SEND_SIZE];

	if (NULL == psBuf ||NULL == punLen)
	{
		return APP_FAIL;
	}
	if (*punLen < 5)
	{
		return APP_FAIL;
	}
	*punLen -= 5;
	memcpy(sTemp, psBuf + 5, *punLen);
	memcpy(psBuf, sTemp, *punLen);
	return APP_SUCC;
}

/**
* @brief ��������
* @param in psSendBuf ��Ҫ��������ָ��
* @param in nSendLen ��Ҫ�������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommSend(const char *psSendBuf, int nSendLen)
{
	int nRet = 0;
	int nLen = 0;
	char sSendBuf[MAX_SEND_SIZE];	/**<���ͻ�����*/

	memcpy(sSendBuf, psSendBuf, nSendLen);
	nLen = nSendLen;	
	//AddCommHead(sSendBuf, (uint *)&nLen);
	AddTpdu(sSendBuf, (uint *)&nLen);

	PubCommClear();
	PubLuaDisplay("LuaCommSend");
	
      nRet = PubCommSend(sSendBuf, nLen);
	if (nRet != APP_SUCC)
	{
		PubDispErr("ͨѶʧ��");
		return APP_FAIL;
	}
	return APP_SUCC;
}



/**
* @brief ��������
* @param out psRecvBuf ���յ�������ָ��
* @param out nRecvLen ���յ������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommRecv(char *psRecvBuf, int *nRecvLen)
{
	int nRet = 0;
	int nLen = 0;
	char sRecvBuf[MAX_RECV_SIZE+1] = {0};	/**<���ջ�����*/
	char *psRecv = sRecvBuf;

	PubLuaDisplay("LuaCommRecv");
	

	nRet = PubCommRecv(sRecvBuf, &nLen);
	if(nRet != APP_SUCC)
	{
		PubDispErr("ͨѶʧ��");
		return APP_FAIL;
	}

	nRet = DelTpdu(psRecv, (uint *)&nLen);
//	nRet += DelCommHead(psRecv, (uint *)&nLen);
	if (APP_SUCC == nRet)
	{
		memcpy(psRecvBuf, psRecv, nLen);
		*nRecvLen = nLen;
		return APP_SUCC;
	}

	return APP_FAIL;
}


/**
* @brief ���ͽ�������
* @param in psSendBuf ��Ҫ��������ָ��
* @param in nSendLen ��Ҫ�������ݳ���* 
* @param out psRecvBuf ���յ�������ָ��
* @param out nRecvLen ���յ������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommSendRecv(const char *psSendBuf, int nSendLen, char *psRecvBuf, int *nRecvLen)
{
	int nRet = 0;
	nRet = CommSend(psSendBuf, nSendLen);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	nRet = CommRecv(psRecvBuf, nRecvLen);

	CommHangUpSocket();

	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	return APP_SUCC;

}


/**
* @brief ���ͽ�������
* @param in psSendBuf ��Ҫ��������ָ��
* @param in nSendLen ��Ҫ�������ݳ���* 
* @param out psRecvBuf ���յ�������ָ��
* @param out nRecvLen ���յ������ݳ���
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommParamSendRecv(const char *psSendBuf, int nSendLen, char *psRecvBuf, int *nRecvLen)
{
	int nRet = 0;
    
	nRet = CommSend(psSendBuf, nSendLen);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

	nRet = CommRecv(psRecvBuf, nRecvLen);
	if (nRet != APP_SUCC)
	{
		return APP_FAIL;
	}

    if (gstAppCommParam.cIsSSL == 1 || gstAppCommParam.cIsCommSendRecvContinue == 0)
    {
		CommHangUpSocket();
    }
	return APP_SUCC;

}

/**
* @brief �Ҷ�ͨѶ��·
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommHangUp(void)
{
	if (COMM_GPRS == gstAppCommParam.cCommType || COMM_CDMA == gstAppCommParam.cCommType)
	{
		if (gstAppCommParam.cMode == 1 || gstAppCommParam.cMode == '1' )
		{
			;
		}
		else/**<�����ӣ���Ҫ�Ҷ�PPP*/
		{
			return PubCommClose();
		}			
	}
	return PubCommHangUp();
}

/**
* @brief�Ҷ�Socket
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommHangUpSocket(void)
{
	if (COMM_DIAL != gstAppCommParam.cCommType)
	{
		PubCommHangUp();
	}
	
	return APP_SUCC;
}


/**
* @brief �Ҷ�ͨѶ��·
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CommDump(void)
{
	if (COMM_DIAL == gstAppCommParam.cCommType)
	{
		return PubCommHangUp();
	}	
	else
	{
		return PubCommClose();
	}
}

/**
* @brief �������ߺ���
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/

int DispCommPreDialNum(void)
{	
	char szTemp[14+1] = {0};
	int nLen = 10;

	MC_GetPubParam(MODEM_PRE_DAIL_NUM, szTemp, (uint *)&nLen);

	PubClearAll();
	PubDisplayTitle("ͨѶ��������");
	PubDisplayStrInline(1, 2, "���ߺ���:");
	PubDisplayStrInline(1, 3, "%s",gstAppCommParam.szPreDial);
	PubUpdateWindow();
	ASSERT_RETURNCODE(PubGetKeyCode(0));
	return APP_SUCC;
}

/**
* @brief �ڹ���˵�������Ԥ���ź���
* @param ��
* @return 
* @li APP_SUCC
* @li APP_FAIL
*/
int SetPreDialNum(void)
{
	SetFuncCommPreDialNum();
	return CommInit();	
}


/**
* @brief �ڹ���˵�������Ԥ���ź���
* @param ��
* @return 
* @li APP_SUCC
* @li APP_FAIL
*/
int DoSetPreDialNum(void)
{
	int nRet;
	
	if(GetVarIsModification() == NO)
	{
		return DispCommPreDialNum();
	}
	nRet = CheckOper("���ߺ���", MANAGEROPER);
  	if (APP_SUCC == nRet)
	{
		SetFuncCommPreDialNum();
		return CommInit();
	}	
	return nRet;		
}

/**
* @brief �������ߺ���
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncCommPreDialNum(void)
{	
	char szTemp[14+1] = {0};
	int nLen = 10;
		
	MC_GetPubParam(MODEM_PRE_DAIL_NUM, szTemp, (uint *)&nLen);
	ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "���ߺ���:", szTemp, \
		&nLen, 0, 10, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szPreDial, szTemp, sizeof(gstAppCommParam.szPreDial)-1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM, 1, (char *)&gstAppCommParam));
	MC_SetPubParam(MODEM_PRE_DAIL_NUM, gstAppCommParam.szPreDial, (uint)nLen);

	return APP_SUCC;
} 

	
/**
* @brief   ͨѶ��������
* @param in ��
* @return     ��
*/
void CommMenu(void)
{
    int nSelect;
    int nRet;
	char szFunKey[2+1]={0};
	szFunKey[0] = '0'; //���ز˵��� 0
	
	while(1)
	{
		nRet = PubSelectListItemExtend("1.TPDU|2.ͨѶ����|3.����|4.GPRS|5.CDMA|6.��̫��|7.����|8.WIFI|9.����", "ͨѶ��������",NULL,0xFF,&nSelect,60,szFunKey);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return;
		}
		switch(nSelect)
		{
		case 1:
			if(GetVarIsModification() == YES)
			{
				SetFuncCommTpdu();
			}
			break;
		case 2:
			SetFuncCommType();
			break;
		case 3:
			if(GetVarIsModification() == NO)
			{
				DispCommPreDialNum();
			}
			else
			{
			int (*lSetFuns[])(void) = {
				SetFuncCommTelNo, 
				SetPreDialNum, 
				SetFuncCommIsPreDial,
				NULL};		
				PubUpDownMenus(lSetFuns);
			}
			break;
		case 4:
			if(GetVarIsModification() == YES)
			{
			int (*lSetFuns[])(void) = {
				SetFuncCommWLMDialNum,
				SetFuncCommMode, 
				SetFuncCommAPN,
				SetFuncCommIp,
				SetFuncCommPort,
				SetFuncCommBackIp,
				SetFuncCommBackPort,
			    SetFuncCommUser,
				SetFuncCommPassWd,
				NULL};
			    PubUpDownMenus(lSetFuns);
			}
			break;
		case 5:
			if(GetVarIsModification() == YES)
			{
			int (*lSetFuns[])(void) = {
				SetFuncCommMode, 
				SetFuncCommWLMDialNum,
				SetFuncCommIp,
				SetFuncCommPort,
				SetFuncCommBackIp,
				SetFuncCommBackPort,
				SetFuncCommUser,
				SetFuncCommPassWd,
				NULL};
			    PubUpDownMenus(lSetFuns);
			}
			break;
		case 6:
			if(GetVarIsModification() == YES)
			{
			int (*lSetFuns[])(void) = {
				SetFuncIsDhcp,
				SetFuncCommIpAddr, 
			    SetFuncCommGate,
			    SetFuncCommMask,
				SetFuncCommIp,
				SetFuncCommPort,
			    SetFuncCommBackIp,
				SetFuncCommBackPort,
				NULL};
			   	PubUpDownMenus(lSetFuns);
			}
			break;
		case 7:
			if(GetVarIsModification() == YES)
			{
				SetFuncAuxIsAddTpdu();
			}
			break;
		case 8:
			if(GetVarIsModification() == YES)
			{
			int (*lSetFuns[])(void) = {
			    SetFuncWifiSsid, 
			    SetFuncWifiPwd,
				SetFuncWifiMode,
			    SetFuncIsDhcp,
				SetFuncCommIpAddr,
				SetFuncCommMask,
				SetFuncCommGate,
			    SetFuncCommIp,
				SetFuncCommPort,
				SetFuncCommBackIp,
				SetFuncCommBackPort,
				NULL};
			    PubUpDownMenus(lSetFuns);
			}
			break;	
		case 9:
			if(GetVarIsModification() == YES)
			{
			int (*lSetFuns[])(void) = {
			    SetFuncIsSSL,
				SetFuncIsSendSSL,
				SetFuncIsUseDns,
			 	SetFuncDnsName,
			  	SetFuncDNSPort,
			   	SetFuncDnsName2,
			   	SetFuncDNSPort2,
			   	SetFuncDNSIp1,
			   	SetFuncDNSIp2,
				SetFuncCommTimeOut,
				SetFuncCommReDialNum,
				NULL};
			    PubUpDownMenus(lSetFuns);
			}
			break;
		default:
			//���ز˵�
			nSelect -= (0xFF + 0x30);
			if (nSelect == 0)
			{
			int (*lSetFuns[])(void) = {
			    SetFuncCommMode, 
			    SetFuncIsCommSendRecvContinue,
				NULL};
			    PubUpDownMenus(lSetFuns);
			}
			break;
		}
	}
}

/**
* @brief ����ͨѶ���Ͳ����ݾ����ͨѶ���ͽ�����Ӧ������
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncCommType(void)
{	
	int nSelect = 0;
	char szInfo[64]={0};

	ASSERT_FAIL(ProSelectList("1.����|2.GPRS|3.CDMA|4.����|5.��̫��|6.WIFI", "ͨѶ��������", \
				gstAppCommParam.cCommType, &nSelect));
	switch(nSelect)
	{
	case 1:
		if(APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_MODEM, NULL))
		{
			PubMsgDlg(NULL, "�����޲���ģ��,������ѡ��ͨѶ��ʽ", 0, 3);
			return APP_FAIL;
		}
		gstAppCommParam.cCommType = COMM_DIAL;
		break;
	case 2:
		if(APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_WIRELESS, szInfo))
		{
			if(memcmp(szInfo, "GPRS", 4) == 0)
			{
				gstAppCommParam.cCommType = COMM_GPRS;
				break;
			}
		}
		PubMsgDlg(NULL, "������GPRSģ��,������ѡ��ͨѶ��ʽ", 0, 3);
		return APP_FAIL;
	case 3:
		if(APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_WIRELESS, szInfo))
		{
			if(memcmp(szInfo, "CDMA", 4) == 0)
			{
				gstAppCommParam.cCommType = COMM_CDMA;
				break;
			}
		}
		PubMsgDlg(NULL, "������CDMAģ��,������ѡ��ͨѶ��ʽ", 0, 3);
		return APP_FAIL;
	case 4:
		gstAppCommParam.cCommType = COMM_RS232;
		break;
	case 5:
		if(APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_ETH, NULL))
		{
			PubMsgDlg(NULL, "��������̫��ģ��,������ѡ��ͨѶ��ʽ", 0, 3);
			return APP_FAIL;
		}
		gstAppCommParam.cCommType = COMM_ETH;
		break;
	case 6:
		if(APP_SUCC != PubGetHardwareSuppot(HARDWARE_SUPPORT_WIFI, NULL))
		{
			PubMsgDlg(NULL, "������WIFIģ��,������ѡ��ͨѶ��ʽ", 0, 3);
			return APP_FAIL;
		}
		gstAppCommParam.cCommType = COMM_WIFI;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	/**
	* ��ʼ��ͨѶ����
	*/
	if (CommInit() != APP_SUCC)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
} 

int GetVarCommType()
{
	return gstAppCommParam.cCommType;
}
/**
* @brief ���ý����ط�����
* @param ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncCommReSendNum(void)
{	
	int nLen = 0,nRet;
	char szTemp[3+1];
	
	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}
	
	for (;;)
	{
		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", gstAppCommParam.cReSendNum >= 0x30 ? (gstAppCommParam.cReSendNum - 0x30)%9 : gstAppCommParam.cReSendNum%9);
		ASSERT_RETURNCODE( PubInputDlg("ϵͳ��������", "���ó����ط�����\n(    <=3��):", szTemp, \
			&nLen, 1, 1, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if ((nRet >= 1 )&& (nRet <= 3))
		{
			break; 
		}
		PubMsgDlg("ϵͳ��������","��������,������!",0,1);
	}
	gstAppCommParam.cReSendNum = nRet;
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));

	return APP_SUCC;
} 

/**
* �ڲ�����ʵ��
*/
/**
* @brief Ӧ�õ�ͨѶ�����ṹ�͹������ͨѶ�ṹת������
* @param in pstApp Ӧ�õ�ͨѶ�����ṹָ��
* @param in pstCommParam �������ͨѶ�ṹָ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int CommParamAppToPub(const STAPPCOMMPARAM *pstApp, const STAPPCOMMRESERVE *pstReserve, STCOMMPARAM *pstCommParam)
{
	pstCommParam->cPreDialFlag = pstApp->cPreDialFlag == '1' ||pstApp->cPreDialFlag == 1 ? 1 : 0;
	pstCommParam->nTimeOut = pstApp->cTimeOut;
	pstCommParam->cReDialNum = pstApp->cReDialNum > 0x30 ? pstApp->cReDialNum - 0x30 : pstApp->cReDialNum;
	pstCommParam->cMode = pstApp->cMode == '1' || pstApp->cMode == 1 ? 0x01 : 0x02;
	pstCommParam->cIsSupportQuitRecv = SERVERMODE_RECVQUIT|SERVERMODE_CLOSENOWAIT|SERVERMODE_INITNOWAIT; 
	if(pstApp->cIsSSL == 0)
	{
		pstCommParam->cSslFlag = 0;//����SSL
	}
	else
	{
		pstCommParam->cSslFlag = 1;//������֤
	}
	switch(pstApp->cCommType)
	{
	case COMM_RS232:
		pstCommParam->cCommType= COMMTYPE_PORT_ASYN1;
		pstCommParam->ConnInfo.stPortParam.nAux = DEFAULT_COMM_AUX;
		strcpy(pstCommParam->ConnInfo.stPortParam.szAttr, "9600,8,N,1");
		memcpy(pstCommParam->ConnInfo.stPortParam.sTPDU, pstApp->sTpdu, 5);
		break;
	case COMM_DIAL:
		pstCommParam->cCommType = COMMTYPE_SYNDIAL;
		memcpy(pstCommParam->ConnInfo.stDialParam.szPredialNo, pstApp->szPreDial, 10);
		memcpy(pstCommParam->ConnInfo.stDialParam.lszTelNo[0], pstApp->szTelNum1, 14);
		memcpy(pstCommParam->ConnInfo.stDialParam.lszTelNo[1], pstApp->szTelNum2, 14);
		memcpy(pstCommParam->ConnInfo.stDialParam.lszTelNo[2], pstApp->szTelNum3, 14);
		memcpy(pstCommParam->ConnInfo.stDialParam.lsTPDU[0], pstApp->sTpdu, 5);
		memcpy(pstCommParam->ConnInfo.stDialParam.lsTPDU[1], pstApp->sTpdu, 5);
		memcpy(pstCommParam->ConnInfo.stDialParam.lsTPDU[2], pstApp->sTpdu, 5);
		pstCommParam->ConnInfo.stDialParam.nCycTimes = 3;
		pstCommParam->ConnInfo.stDialParam.nCountryid = 5;
		break;
	case COMM_GPRS:
		pstCommParam->cCommType = COMMTYPE_GPRS_HEADLEN;
		memcpy(pstCommParam->ConnInfo.stGprsParam.szNetUsername, pstApp->szUser, 40);
		memcpy(pstCommParam->ConnInfo.stGprsParam.szNetPassword, pstApp->szPassWd, 20);
		memcpy(pstCommParam->ConnInfo.stGprsParam.szPinPassWord, pstApp->szSIMPassWd, 10);
		if(pstApp->szWirelessDialNum[0] == 'D' || pstApp->szWirelessDialNum[0] == 'd')
		{
			memcpy(pstCommParam->ConnInfo.stGprsParam.szModemDialNo, pstApp->szWirelessDialNum+1, 20);
		}
		else
		{
			memcpy(pstCommParam->ConnInfo.stGprsParam.szModemDialNo, pstApp->szWirelessDialNum, 20);
		}
		sprintf(pstCommParam->ConnInfo.stGprsParam.szGprsApn, "%s", pstApp->szAPN1);
		memcpy(pstCommParam->ConnInfo.stGprsParam.sTPDU, pstApp->sTpdu, 5);
		break;
	case COMM_CDMA:
		pstCommParam->cCommType = COMMTYPE_CDMA_HEADLEN;
		memcpy(pstCommParam->ConnInfo.stCdmaParam.szNetUsername, pstApp->szUser, 40);
		memcpy(pstCommParam->ConnInfo.stCdmaParam.szNetPassword, pstApp->szPassWd, 20);
		memcpy(pstCommParam->ConnInfo.stCdmaParam.szPinPassWord, pstApp->szSIMPassWd, 10);
		if(pstApp->szWirelessDialNum[0] == 'D' || pstApp->szWirelessDialNum[0] == 'd')
		{
			memcpy(pstCommParam->ConnInfo.stCdmaParam.szModemDialNo, pstApp->szWirelessDialNum+1, 20);
		}
		else
		{
			memcpy(pstCommParam->ConnInfo.stCdmaParam.szModemDialNo, pstApp->szWirelessDialNum, 20);
		}
		memcpy(pstCommParam->ConnInfo.stCdmaParam.sTPDU, pstApp->sTpdu, 5);
		break;
	case COMM_ETH:
		pstCommParam->cCommType = COMMTYPE_ETH_HEADLEN;//COMMTYPE_ETH_TPDU_HEADLEN;
		if(pstReserve->cIsDHCP != 0)
		{
			pstCommParam->ConnInfo.stEthParam.nDHCP = 1;
		}
		else
		{
			pstCommParam->ConnInfo.stEthParam.nDHCP = 0;
			memcpy(pstCommParam->ConnInfo.stEthParam.szIP, pstApp->szIpAddr, 16);
			memcpy(pstCommParam->ConnInfo.stEthParam.szGateway, pstApp->szGate, 16);
			memcpy(pstCommParam->ConnInfo.stEthParam.szMask, pstApp->szMask, 16);
		}
		memcpy(pstCommParam->ConnInfo.stEthParam.sTPDU, pstApp->sTpdu, 5);
		break;
	case COMM_WIFI:
		pstCommParam->cCommType = COMMTYPE_WIFI_HEADLEN;
		//Ŀǰֻ֧��DHCPģʽ
		pstCommParam->ConnInfo.stWifiParam.ucIfDHCP = 1; 
		pstCommParam->ConnInfo.stWifiParam.emKeyType = pstReserve->cWifiKeyType;
		pstCommParam->ConnInfo.stWifiParam.emSecMode = pstReserve->cWifiMode;
		memcpy(pstCommParam->ConnInfo.stWifiParam.szSsid,pstReserve->szWifiSsid,32);
		memcpy(pstCommParam->ConnInfo.stWifiParam.szKey,pstReserve->szWifiKey,32);
		memcpy(pstCommParam->ConnInfo.stWifiParam.sTPDU,pstApp->sTpdu, 5);
		break;
	default:
		return APP_FAIL;
	}
	
	if(pstApp->cIsDns == 0)
	{
		memcpy(pstCommParam->stServerAddress.lszIp[0], pstApp->szIp1, 16);
		memcpy(pstCommParam->stServerAddress.lszIp[1], pstApp->szIp2, 16);
	}
	else
	{
		memcpy(pstCommParam->stServerAddress.szDN, pstApp->szDomain, 50);
		memcpy(pstCommParam->stServerAddress.szDNSIp, pstApp->szDNSIp1, 16);
	}
	pstCommParam->stServerAddress.lnPort[0] = atoi(pstApp->szPort1);
	pstCommParam->stServerAddress.lnPort[1] = atoi(pstApp->szPort2);
	
	pstCommParam->ShowFunc = NULL;
	return APP_SUCC;
}

/**
* @brief ����ppp������ģʽ����ѡ������Ӻͳ�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommMode(void)
{	
	char nSelect = 0;

	nSelect = gstAppCommParam.cMode == '1' || gstAppCommParam.cMode == 1 ? 1 : 0;
	ASSERT_RETURNCODE(ProSelectYesOrNo("ͨѶ��������", "����ģʽ", "0.������|1.������", &nSelect));
	switch(nSelect)
	{
	case '0':
		gstAppCommParam.cMode = 0;
		break;
	case '1':
		gstAppCommParam.cMode = 1;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));

	return APP_SUCC;
} 


/**
* @brief �����Ƿ���ҪԤ����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommIsPreDial(void)
{	
	char nSelect = 0;

	nSelect = gstAppCommParam.cPreDialFlag == '1' || gstAppCommParam.cPreDialFlag == 1 ? 1 : 0;
	ASSERT_RETURNCODE( ProSelectYesOrNo("ͨѶ��������", "�Ƿ�Ԥ����", "0.��|1.��", &nSelect));
	switch(nSelect)
	{
	case '0':
		gstAppCommParam.cPreDialFlag = 0;
		break;
	case '1':
		gstAppCommParam.cPreDialFlag = 1;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
} 

/**
* @brief ����ͨѶ��ʱʱ��
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommTimeOut(void)
{	
	int nRet = 0;
	char szTemp[3+1];
	int nLen = 0;
	
	for (;;)
	{
		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", (uint)gstAppCommParam.cTimeOut);
		ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "���׳�ʱʱ��:", szTemp, \
			&nLen, 2, 2, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if (nRet < 10 || nRet >= 100)
		{
			continue; 
		}
		gstAppCommParam.cTimeOut = nRet;
		break;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
} 

/**
* @brief ���ý����ز�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommReDialNum(void)
{
	int nLen;
	char szNumber[2+1] = {0};

	sprintf(szNumber, "%d", gstAppCommParam.cReDialNum > 0X30 ? gstAppCommParam.cReDialNum - 0X30 : gstAppCommParam.cReDialNum );
	ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "�����ز�����:", szNumber, &nLen, 1, 1, 60, INPUT_MODE_NUMBER));
	gstAppCommParam.cReDialNum = atoi(szNumber);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ���������ط�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFunctionOffResendNum(void)
{
	char szNum[2] = {0};
	int nNum, nLen;
	
	if(GetVarIsModification() == NO)
	{
		return APP_SUCC;
	}
	
	while(1)
	{	
		szNum[0] = gstAppCommParam.cOffResendNum+'0';
		ASSERT_RETURNCODE( PubInputDlg("���߽��׿���", "�����������ʹ���\n(    <=9��):", szNum, &nLen, 1, 1, 0, INPUT_MODE_NUMBER));
		nNum=atoi(szNum);
		if( (nNum >= 1) && (nNum <= 9) )
		{
			break;
		}
		PubMsgDlg("���߽��׿���","��������,������!",0,1);
	}
	gstAppCommParam.cOffResendNum = nNum;	
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ����ͨѶ��TPDU
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommTpdu(void)
{	
	char szTemp[10+1] = {0};
	int nLen = 0;

	PubHexToAsc((uchar *)gstAppCommParam.sTpdu, 10, 0, (uchar *)szTemp);
	ASSERT_FAIL(PubInputDlg("ͨѶ��������", "TPDU:", szTemp, \
		&nLen, 10, 10, 60, INPUT_MODE_NUMBER));
	PubAscToHex((uchar *)szTemp, 10, 0, (uchar *)gstAppCommParam.sTpdu);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
} 
/**
* @brief ���ò��ŷ�ʽ�õ�����绰����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommTelNo(void)
{	
	char szTemp[19+1] = {0};
	int nLen=0, i, nRet;
	char *p = NULL;
	char szContent[16+1] = {0};

	p = gstAppCommParam.szTelNum1;
	for (i = 1; i <= 3; i++)
	{
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, p, sizeof(gstAppCommParam.szTelNum1) -1);
		sprintf(szContent, "���Ľ��׵绰%d:", i);				
		nRet = PubInputDlg("ͨѶ��������", szContent, szTemp, &nLen, 0, 14, 60, INPUT_MODE_STRING);
		if(nRet == KEY_UP)
		{
			if(i <= 1)
			{
				return KEY_UP;
			}
			else
			{
				i -= 2;
				p -= sizeof(gstAppCommParam.szTelNum1);
				continue;
			}
		}
		else if(nRet == KEY_DOWN)
		{
			p += sizeof(gstAppCommParam.szTelNum1);
			continue;
		}
		ASSERT_RETURNCODE(nRet);		
		memcpy(p, szTemp, sizeof(gstAppCommParam.szTelNum1) -1);
		p += sizeof(gstAppCommParam.szTelNum1);
		ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	}
	return APP_SUCC;
} 

/**
* @brief ��������ͨѶ�õĽ������
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommWLMDialNum(void)
{
	char szTemp[20+1] = {0};
	int nLen;
		
	memcpy(szTemp, gstAppCommParam.szWirelessDialNum, 20);
	ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "�������(D)", szTemp, \
			&nLen, 0, 20, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szWirelessDialNum, szTemp, sizeof(gstAppCommParam.szWirelessDialNum) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����gprs��ʽ�õ�apn
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommAPN(void)
{
	char szTemp[40+1] = {0};
	int nLen;

	memcpy(szTemp, gstAppCommParam.szAPN1, 40);
	ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "APN����", szTemp, \
			&nLen, 0, 40, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szAPN1, szTemp, sizeof(gstAppCommParam.szAPN1) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ����cdma��ʽ�õ��û���
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommUser(void)
{
	char szTemp[40+1];
	int nLen;

	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommParam.szUser, 40);
	ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "�û���", szTemp, \
			&nLen, 0, 40, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szUser, szTemp, sizeof(gstAppCommParam.szUser) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����cdma��ʽ�õ��û�����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommPassWd(void)
{
	char szTemp[40+1];
	int nLen;
			
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommParam.szPassWd, 40);
	ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "�û�����", szTemp, \
			&nLen, 0, 40, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szPassWd, szTemp, sizeof(gstAppCommParam.szPassWd) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

static int SetFuncDnsName2(void)
{
	char szTemp[50+1];
	int nLen;

	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommReserve.szDomain2, sizeof(gstAppCommReserve.szDomain2)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "��������", szTemp, &nLen,0,50,60,INPUT_MODE_STRING));
	memcpy(gstAppCommReserve.szDomain2, szTemp, sizeof(gstAppCommReserve.szDomain2) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}

static int SetFuncDnsName(void)
{
	char szTemp[50+1];
	int nLen;

	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommParam.szDomain, sizeof(gstAppCommParam.szDomain)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "����������", szTemp, &nLen,0,50,60,INPUT_MODE_STRING));
	memcpy(gstAppCommParam.szDomain, szTemp, sizeof(gstAppCommParam.szDomain) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����DNS������ip��ַ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncDNSIp1(void)
{
	char szTemp[16+1];
	int nLen;

	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommParam.szDNSIp1, sizeof(gstAppCommParam.szDNSIp1)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "��DNS ��ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szDNSIp1, szTemp, sizeof(gstAppCommParam.szDNSIp1) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����DNS������ip��ַ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncDNSIp2(void)
{
	char szTemp[16+1];
	int nLen;

	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, gstAppCommParam.szDNSIp2, sizeof(gstAppCommParam.szDNSIp2)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "��DNS ��ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szDNSIp2, szTemp, sizeof(gstAppCommParam.szDNSIp2) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

static int SetFuncDNSPort(void)
{
	char szTemp[20+1];
	int nRet;
	int nLen;
	
	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	while(1)
	{
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, gstAppCommParam.szPort1, sizeof(gstAppCommParam.szPort1)-1);
		ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "�����˿ں�", szTemp, \
				&nLen, 0, 6, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if (nRet < 0 || nRet > 65535)
		{
			continue; 
		}
		break;
	}
	memcpy(gstAppCommParam.szPort1, szTemp, sizeof(gstAppCommParam.szPort1) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

static int SetFuncDNSPort2(void)
{
	char szTemp[20+1];
	int nRet;
	int nLen;
	
	if(gstAppCommParam.cIsDns == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	while(1)
	{
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, gstAppCommParam.szPort2, sizeof(gstAppCommParam.szPort2)-1);
		ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "���������˿ں�", szTemp, \
				&nLen, 0, 6, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if (nRet < 0 || nRet > 65535)
		{
			continue; 
		}
		break;
	}
	memcpy(gstAppCommParam.szPort2, szTemp, sizeof(gstAppCommParam.szPort2) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ��������ip��ַ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommIp(void)
{
	char szTemp[16+1]= {0};
	int nLen;
	
	if(gstAppCommParam.cIsDns != 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	memcpy(szTemp, gstAppCommParam.szIp1, sizeof(gstAppCommParam.szIp1)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "����IP��ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szIp1, szTemp, sizeof(gstAppCommParam.szIp1) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ���ñ�������ip��ַ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommBackIp(void)
{
	char szTemp[16+1] = {0};
	int nLen;
	
	if(gstAppCommParam.cIsDns != 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	memcpy(szTemp, gstAppCommParam.szIp2, sizeof(gstAppCommParam.szIp2)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "��������IP��ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szIp2, szTemp, sizeof(gstAppCommParam.szIp2) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ���������˿ں�
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommPort(void)
{
	char szTemp[20+1];
	int nRet;
	int nLen;
	
	if(gstAppCommParam.cIsDns != 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	while(1)
	{
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, gstAppCommParam.szPort1, sizeof(gstAppCommParam.szPort1)-1);
		ASSERT_RETURNCODE(PubInputDlg("ͨѶ��������", "�����˿ں�", szTemp, \
				&nLen, 0, 6, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if (nRet < 0 || nRet > 65535)
		{
			continue; 
		}
		break;
	}
	memcpy(gstAppCommParam.szPort1, szTemp, sizeof(gstAppCommParam.szPort1) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}


/**
* @brief ���ñ��������˿ں�
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommBackPort(void)
{
	char szTemp[20+1];
	int nRet;
	int nLen;
	
	if(gstAppCommParam.cIsDns != 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}	
	while(1)
	{
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, gstAppCommParam.szPort2, sizeof(gstAppCommParam.szPort2)-1);
		ASSERT_RETURNCODE( PubInputDlg("ͨѶ��������", "���������˿ں�", szTemp, \
				&nLen, 0, 6, 60, INPUT_MODE_NUMBER));
		nRet = atoi(szTemp);
		if (nRet < 0 || nRet > 65535)
		{
			continue; 
		}
		break;
	}
	memcpy(gstAppCommParam.szPort2, szTemp, sizeof(gstAppCommParam.szPort2) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����pos����ip��ַ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommIpAddr(void)
{
	char szTemp[16+1] = {0};
	int nLen;

	if(gstAppCommReserve.cIsDHCP != 0)
	{	
		//�Զ���ȡ�����
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}

	memcpy(szTemp, gstAppCommParam.szIpAddr, sizeof(gstAppCommParam.szIpAddr)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "����IP��ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szIpAddr, szTemp, sizeof(gstAppCommParam.szIpAddr) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	
	return APP_SUCC;
}
/**
* @brief ����pos������������
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommMask(void)
{
	char szTemp[16+1] = {0};
	int nLen;

	if(gstAppCommReserve.cIsDHCP != 0)
	{
		//�Զ���ȡ�����
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memcpy(szTemp, gstAppCommParam.szMask, sizeof(gstAppCommParam.szMask)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "������������", szTemp, &nLen));
	memcpy(gstAppCommParam.szMask, szTemp, sizeof(gstAppCommParam.szMask) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));

	return APP_SUCC;
}
/**
* @brief ����pos����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncCommGate(void)
{
	char szTemp[16+1] = {0};
	int nLen;

	if(gstAppCommReserve.cIsDHCP != 0)
	{
		//�Զ���ȡ�����
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}
	
	memcpy(szTemp, gstAppCommParam.szGate, sizeof(gstAppCommParam.szGate)-1);
	nLen = strlen(szTemp);
	ASSERT_RETURNCODE(PubInputIp("ͨѶ��������", "���ص�ַ", szTemp, &nLen));
	memcpy(gstAppCommParam.szGate, szTemp, sizeof(gstAppCommParam.szGate) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ����pos����
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncAuxIsAddTpdu(void)
{
	int nSelect = 0;

	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�����Ƿ��TPDU",\
		gstAppCommParam.cAuxIsAddTpdu== '1' || gstAppCommParam.cAuxIsAddTpdu == 1 ? 1 : 0 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommParam.cAuxIsAddTpdu = 0;
		break;
	case 1:
		gstAppCommParam.cAuxIsAddTpdu = 1;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}


/**
* @brief ����pos�Ƿ�֧������ͨѶ
* @param ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncIsCommSendRecvContinue(void)
{
	int nSelect = 0;

	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�Ƿ�֧���������ز���",\
		 gstAppCommParam.cIsCommSendRecvContinue == 1 ? 1 : 0 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommParam.cIsCommSendRecvContinue = 0;
		break;
	case 1:
		gstAppCommParam.cIsCommSendRecvContinue = 1;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ȡͨѶ��ʱʱ��
* @param out pszTimeOut ��λ��ʱʱ��ASCii
* @li APP_SUCC
*/
int GetVarCommTimeOut(char *pszTimeOut)
{
	char szTmp[2+1] = {0};
	
	sprintf(szTmp, "%02d", gstAppCommParam.cTimeOut);
	memcpy(pszTimeOut, szTmp, 2);

	return APP_SUCC;
}

/**
* @brief ����ͨѶ��ʱʱ��
* @param in pszTimeOut ��λ��ʱʱ��ASCii
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommTimeOut(const char *pszTimeOut)
{
	char szTemp[2+1] = {0};

	memcpy(szTemp, pszTimeOut, 2);
	gstAppCommParam.cTimeOut = (char)atoi(szTemp);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ȡ�����ط�����
* @param out pcReSendNum 1λ�ط�����
* @li APP_SUCC
*/
int GetVarCommReSendNum(char *pcReSendNum)
{
	*pcReSendNum = gstAppCommParam.cReSendNum;
	return APP_SUCC;
}

/**
* @brief ���ý����ط�����
* @param in pcReSendNum 1λ1λ�ط�����
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommReSendNum(const char *pcReSendNum)
{
	char cReSendUum = *pcReSendNum;


	gstAppCommParam.cReSendNum  = cReSendUum >= 0x30 ? (cReSendUum - 0x30)%9 : cReSendUum%9;
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ȡ�����ط�����
* @param out pcReSendNum 1λ�ط�����
* @li APP_SUCC
*/
int GetVarCommOffReSendNum(char *pcReSendNum)
{
	*pcReSendNum = gstAppCommParam.cOffResendNum;
	return APP_SUCC;
}
/**
* @brief ���������ط�����
* @param in pcReSendNum 1λ1λ�ط�����
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommOffReSendNum(const char *pcReSendNum)
{
	char cReSendUum = *pcReSendNum;

	gstAppCommParam.cOffResendNum  = cReSendUum >= 0x30 ? (cReSendUum - 0x30)%9 : cReSendUum%9;
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ȡ�����ز�����
* @param out pcReSendNum 1λ�ط�����
* @li APP_SUCC
*/
int GetVarCommReDialNum(char *pcReDialNum)
{
	*pcReDialNum = gstAppCommParam.cReDialNum;
	return APP_SUCC;
}

/**
* @brief ���ý����ز�����
* @param in pcReSendNum 1λ1λ�ط�����
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommReDialNum(const char *pcReDialNum)
{
	char cReDialUum = *pcReDialNum;

	gstAppCommParam.cReDialNum  = cReDialUum >= 0x30 ? (cReDialUum - 0x30)%9 : cReDialUum%9;
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}
/**
* @brief ȡ���ĵ绰�绰�͹���绰����
* @param out pszTel1 ��1���绰����ָ��
* @param out pszTel2 ��2���绰����ָ��
* @param out pszTel3 ��3���绰����ָ��
* @param out pszManageTel ����绰����ָ��
* @li APP_SUCC
*/
int GetVarCommTelNo(char *pszTel1, char *pszTel2, char *pszTel3, char *pszManageTel)
{
	memcpy(pszTel1, gstAppCommParam.szTelNum1, strlen(gstAppCommParam.szTelNum1) > sizeof(gstAppCommParam.szTelNum1)-1 \
		? sizeof(gstAppCommParam.szTelNum1)-1 : strlen(gstAppCommParam.szTelNum1));
	memcpy(pszTel2, gstAppCommParam.szTelNum2, strlen(gstAppCommParam.szTelNum2) > sizeof(gstAppCommParam.szTelNum2)-1 \
		? sizeof(gstAppCommParam.szTelNum2)-1 : strlen(gstAppCommParam.szTelNum2));
	memcpy(pszTel3, gstAppCommParam.szTelNum3, strlen(gstAppCommParam.szTelNum3) > sizeof(gstAppCommParam.szTelNum3)-1 \
		? sizeof(gstAppCommParam.szTelNum3)-1 : strlen(gstAppCommParam.szTelNum3));
	memcpy(pszManageTel, gstAppCommParam.szManageTelNum, strlen(gstAppCommParam.szManageTelNum) > sizeof(gstAppCommParam.szManageTelNum)-1 \
		? sizeof(gstAppCommParam.szManageTelNum)-1 : strlen(gstAppCommParam.szManageTelNum));
	return APP_SUCC;
}

/**
* @brief �������ĵ绰�绰�͹���绰����
* @param in pszTel1 ��1���绰����ָ��
* @param in pszTel2 ��2���绰����ָ��
* @param in pszTel3 ��3���绰����ָ��
* @param in pszManageTel ����绰����ָ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetVarCommTelNo(const char *pszTel1, const char *pszTel2, const char *pszTel3, const char *pszManageTel)
{
	memset(gstAppCommParam.szTelNum1, 0, sizeof(gstAppCommParam.szTelNum1));
	memset(gstAppCommParam.szTelNum2, 0, sizeof(gstAppCommParam.szTelNum2));
	memset(gstAppCommParam.szTelNum3, 0, sizeof(gstAppCommParam.szTelNum3));
	memset(gstAppCommParam.szManageTelNum, 0, sizeof(gstAppCommParam.szManageTelNum));

	memcpy(gstAppCommParam.szTelNum1, pszTel1, strlen(pszTel1) > sizeof(gstAppCommParam.szTelNum1)-1 \
		? sizeof(gstAppCommParam.szTelNum1)-1 : strlen(pszTel1));
	memcpy(gstAppCommParam.szTelNum2, pszTel2, strlen(pszTel2) > sizeof(gstAppCommParam.szTelNum2)-1 \
		? sizeof(gstAppCommParam.szTelNum2)-1 : strlen(pszTel2));
	memcpy(gstAppCommParam.szTelNum3, pszTel3, strlen(pszTel3) > sizeof(gstAppCommParam.szTelNum3)-1 \
		? sizeof(gstAppCommParam.szTelNum3)-1 : strlen(pszTel3));
	memcpy(gstAppCommParam.szManageTelNum, pszManageTel, strlen(pszManageTel) > sizeof(gstAppCommParam.szManageTelNum)-1 \
		? sizeof(gstAppCommParam.szManageTelNum)-1 : strlen(pszManageTel));
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief ����wifi�û���
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncWifiSsid(void)
{
	char szTemp[100+1] = {0};
	int nLen, nRet, nWifiMode;

	PubClearAll();
	PubDisplayTitle("WIFI��������");
	PubDisplayStrInline(1, 2,"�û���");
	PubDisplayStrInline(1, 3, "%s",gstAppCommReserve.szWifiSsid);
	PubDisplayStrInline(1, 4, "0.ɨ��    1.����");
	PubUpdateWindow();
	nRet = PubGetKeyCode(60);
	if(nRet == KEY_F1 || nRet == KEY_F2)
		return nRet;
	else if(nRet != KEY_0 && nRet != KEY_1 && nRet != KEY_ENTER)
		return APP_FAIL;
	if(nRet == KEY_0)
	{
		PubClear2To4();
		PubDisplayStrInline(0, 2, "��������WIFI�ȵ�");
		PubDisplayStrInline(0, 3, "���Ժ�...");
		PubUpdateWindow();		
		ASSERT_RETURNCODE(PubCommScanWifi("ͨѶ��������", szTemp, &nWifiMode, 60));
		memset(gstAppCommReserve.szWifiSsid, 0, sizeof(gstAppCommReserve.szWifiSsid));
		memcpy(gstAppCommReserve.szWifiSsid, szTemp, sizeof(gstAppCommReserve.szWifiSsid));
	}
	else if(nRet == KEY_1)
	{
		memcpy(szTemp, gstAppCommReserve.szWifiSsid, 32);
		ASSERT_RETURNCODE( PubInputDlg("WIFI��������", "�û���", szTemp, \
				&nLen, 1, 32, 60, INPUT_MODE_STRING));
		memcpy(gstAppCommReserve.szWifiSsid, szTemp, sizeof(gstAppCommReserve.szWifiSsid) -1);
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}

/**
* @brief ����wifi����
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncWifiPwd(void)
{
	char szTemp[64+1] = {0};
	int nLen;
	
	memcpy(szTemp, gstAppCommReserve.szWifiKey, 40);
	ASSERT_RETURNCODE( PubInputDlg("WIFI��������", "����", szTemp, \
				&nLen, 5, 32, 60, INPUT_MODE_STRING));
	memcpy(gstAppCommReserve.szWifiKey, szTemp, sizeof(gstAppCommReserve.szWifiKey) -1);
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}


/**
* @brief ����wifi������ܷ�ʽ
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/
int SetFuncWifiMode(void)
{
	int nSelect = 0;
	
	ASSERT_RETURNCODE(ProSelectList("1.OPEN|2.WEP|3.WPA-PSK|4.WPA2-PSK|5.WPA-CCKM", "WIFI�����㷨", \
				gstAppCommReserve.cWifiMode, &nSelect));
	switch(nSelect)
	{
	case 1:
		gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WEP_OPEN;
		break;
	case 2:
		gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WEP_SHARED;
		break;
	case 3:
		gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA;
		break;
	case 4:
		gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA2;
		break;
	case 5:
		gstAppCommReserve.cWifiMode = WIFI_NET_SEC_WPA;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�����SSL
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncIsSSL(void)
{
	int nSelect = 0;
	
	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�Ƿ�����SSL",\
		gstAppCommParam.cIsSSL == '1' || gstAppCommParam.cIsSSL == 1 ? 1 : 0 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommParam.cIsSSL = 0;
		break;
	case 1:
		gstAppCommParam.cIsSSL = 1;
		break;
	default:
		return APP_QUIT;
	}
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

/**
* @brief �����Ƿ�ʹ�õ���SSL
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/
static int SetFuncIsSendSSL(void)
{
	int nSelect = 0;
	
	if(gstAppCommParam.cIsSSL == 0)
	{
		return APP_FUNCQUIT;//���ش�ֵ���Ա�֤���Ϸ�ҳ
	}

	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�Ƿ���SSL",\
		gstAppCommParam.cIsSendSSLAuth == '1' || gstAppCommParam.cIsSendSSLAuth == 1 ? 0 : 1 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommParam.cIsSendSSLAuth = 1;//�Ƿ���֤���������
		break;
	case 1:
		gstAppCommParam.cIsSendSSLAuth = 0;
		break;
	default:
		return APP_QUIT;
	}
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

char GetVarIsSendSSL(void)
{
	return gstAppCommParam.cIsSendSSLAuth;
}

int IsUseSslFunction(void)
{	
	if(gstAppCommParam.cCommType != COMM_DIAL && gstAppCommParam.cCommType != COMM_RS232)
	{
		if(gstAppCommParam.cIsSSL == 1)
		{
			return APP_SUCC;
		}
	}
	return APP_FAIL;
}


/**
* @brief �����Ƿ�ʹ��DNS
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/

static int SetFuncIsUseDns(void)
{
	int nSelect = 0;
	
	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�Ƿ�ʹ��DNS",\
		gstAppCommParam.cIsDns == '1' || gstAppCommParam.cIsDns == 1 ? 1 : 0 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommParam.cIsDns = 0;
		break;
	case 1:
		gstAppCommParam.cIsDns = 1;
		break;
	default:
		return APP_QUIT;
	}
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}


/**
* @brief �����Ƿ�ʹ��DHCP
* @param in  ��
* @li APP_SUCC
* @li APP_FAIL
*/

static int SetFuncIsDhcp(void)
{
	int nSelect = 0;	

	ASSERT_RETURNCODE(PubSelectListItem("0.��|1.��", "ͨѶ��������", "�Ƿ�ʹ��DHCP",\
		gstAppCommReserve.cIsDHCP == '1' || gstAppCommReserve.cIsDHCP == 1 ? 1 : 0 , &nSelect));
	switch(nSelect)
	{
	case 0:
		gstAppCommReserve.cIsDHCP = 0;
		break;
	case 1:
		gstAppCommReserve.cIsDHCP = 1;
		break;
	default:
		return APP_QUIT;
	}
	
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}

/**
* @brief ��ȡwifi ͨѶ����
* @param in  ��
* @param out STAPPCOMMRESERVE *pstAppCommReserve
* @li APP_SUCC
* @li APP_FAIL
*/

void GetAppCommReserve(STAPPCOMMRESERVE *pstAppCommReserve)
{
	memcpy(pstAppCommReserve, &gstAppCommReserve, sizeof(STAPPCOMMRESERVE));
}

/**
* @brief ����wifi ͨѶ����
* @param in  STAPPCOMMRESERVE *pstAppCommReserve
* @param out ��
* @li APP_SUCC
* @li APP_FAIL
*/

int SetAppCommReserve(STAPPCOMMRESERVE *pstAppCommReserve)
{
	memcpy((char *)(&gstAppCommReserve), (char *)pstAppCommReserve, sizeof(STAPPCOMMRESERVE));
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMRESERVE,1,(char *)&gstAppCommReserve));
	return APP_SUCC;
}

/**
* @brief ��ȡӦ��ͨѶ����
* @param out Ӧ��ͨѶ����ָ��
* @return ��
*/
void GetAppCommParam(STAPPCOMMPARAM *pstAppCommParam)
{
	memcpy((char *)pstAppCommParam, (char *)(&gstAppCommParam), sizeof(STAPPCOMMPARAM));
}

/**
* @brief ����Ӧ��ͨѶ����
* @param in Ӧ��ͨѶ����ָ��
* @return 
* @li APP_SUCC
* @li APP_FAIL
*/
int SetAppCommParam(STAPPCOMMPARAM *pstAppCommParam)
{
	memcpy((char *)(&gstAppCommParam), (char *)pstAppCommParam, sizeof(STAPPCOMMPARAM));
	ASSERT_FAIL(PubUpdateRec(FILE_APPCOMMPARAM,1,(char *)&gstAppCommParam));
	return APP_SUCC;
}

