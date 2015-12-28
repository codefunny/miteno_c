/**
* @file cup_ums.c
* @brief ��Ӧ�ò���ģ��
* @version  1.0
* @author �����
* @date 2014-09-23
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"




/**
* ����ȫ�ֱ���
*/
static char *gpLightBuf[6]; /**���̻����ڰ�*/
static char gcLight[4];
static char gcIsLightOff = YES;
static pthread_t gThread; 
static char gcIsCreateThread = NO;
static char gcIsApiSupport = NO;
static char gcIsSetViewPort = NO;

#define LED_HIGH (24)     /**LED�и߶�*/
#define LED_SPACING (80)  /**�Ƽ��*/
#define LED_SIZE  (20)    /**LED�Ƶĳߴ�-������*/



#define BEEP_DEV  "/dev/buzz" 
#define BUZZ_IOC_MAGIC  'N' 
#define BUZZ_IOCS_SDKNOWAIT _IO( BUZZ_IOC_MAGIC, 9) 
#define BUZZ_IOCS	        _IOW(BUZZ_IOC_MAGIC, 4, int)
#define BUZZ_IOCG			_IOR(BUZZ_IOC_MAGIC, 5, int)

typedef struct _BUZZ_PARAM {
    int pulse_width;
    int beep_time;
} BUZZ_PARAM;

/**
* �����ڲ�����
*/
static void *ThreadLedFlick(void *);
static void ShowLightFlick(void);
static void DecodeLedPic(void);
static void SetLedMode(EM_LEDMODE, EM_LEDMODE, EM_LEDMODE, EM_LEDMODE);
static void ShowLight(void);
static void CLoseLight(void);
static void CheckImgApi(void);


extern int NDK_ScrImgDestroy(char *);
extern int NDK_ScrDispImg(int, int, int, int, char *);
extern int NDK_ScrImgDecode(char *, char **);



/**
* @brief ������beep[������֤����]
* @param in unFrequency Ƶ��
* @param in unMstime ����ʱ��[������]
* @return ����
*/
int Cup_BeepAysn(uint unFrequency, uint unMstime)
{
	int beep_fd; 
	BUZZ_PARAM buzz_param; 
	 
	beep_fd = open(BEEP_DEV, O_RDWR);
	
	if (beep_fd < 0) 
	{
		return APP_FAIL;
	}
	ioctl(beep_fd, BUZZ_IOCG, &buzz_param);
	buzz_param.pulse_width = unFrequency;
	ioctl(beep_fd, BUZZ_IOCS, &buzz_param);
	ioctl(beep_fd, BUZZ_IOCS_SDKNOWAIT, unMstime);
	close(beep_fd);
	return APP_SUCC;
}


/**
* @brief ��ĻLED����س�ʼ��[������֤����]
* @return ����
*/
void Cup_InitScrLed(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //�߱�LED��
	{
		return ;
	}
	else if (16 == PubScrGetColorDepth()) //����
	{
		CheckImgApi();
		DecodeLedPic();
	}
}

/**
* @brief ��ʾ��ĻLED��[������֤����]
* @param in ��ɫLED�Ƶ���������
* @return ����
*/
void Cup_ShowScrLed(EM_LEDMODE emRed, EM_LEDMODE emGreen, EM_LEDMODE emYellow, EM_LEDMODE emBlue)
{
	SetLedMode(emRed, emGreen, emYellow, emBlue);
	
	if (LED_CLOSE == emRed 
		&& LED_CLOSE == emGreen
		&& LED_CLOSE == emYellow
		&& LED_CLOSE == emRed)
	{
		CLoseLight();
	}
	else
	{
		ShowLight();
	}
	
}


/**
* @brief ��ĻLED����Դ����,�˳�Ӧ��ʱ����[������֤����]
* @return ����
*/
void Cup_ExitScrLed(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //�߱�LED��
	{
		return;
	}
	else if (16 == PubScrGetColorDepth()) //����
	{
		if (YES == gcIsApiSupport)
		{
			int i;
		
			for(i = 0; i < 6; i++ )
			{
				NDK_ScrImgDestroy(gpLightBuf[i]);
			}
		}
	}
	
}
/**
* @brief ��ĻLED�Ʊ���[������֤����]
* @return ����
*/
void Cup_OpenLedBackGround(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //�߱�LED��
	{
		return ;
	}
	else if (16 == PubScrGetColorDepth()) //����
	{
		Cup_ShowScrLed(LED_UNCHANGED, LED_UNCHANGED, LED_UNCHANGED, LED_UNCHANGED);	
	}
}

static void CheckImgApi(void)
{
	char szVer[64]={0};

	NDK_Getlibver(szVer);

	if (((strcmp(szVer, "5.0.1") >= 0) && szVer[0] != 'V')
		|| NDK_OK == NDK_FsExist("/etc/displed")
		)
	{
		PubDebug("֧��ͼƬ����API\n");
		gcIsApiSupport = YES;
		return ;
	}
	PubDebug("��֧��ͼƬ����API\n");
	gcIsApiSupport = NO;
	return ;
}

static void *ThreadLedFlick(void *arg)
{
	int nCount = 0;
	uint x,y,w,h;
	pthread_mutex_t gMutex;
	
	pthread_mutex_init(&gMutex, NULL);
	
	NDK_ScrGetViewPort(&x,&y,&w,&h);
	
	while(gcIsCreateThread == YES)
	{
		pthread_mutex_lock(&gMutex);
		NDK_ScrSetViewPort(x,y - LED_HIGH,w,h + LED_HIGH);

		nCount++;
		if (nCount%2 == 0)
		{
			if (YES == gcIsApiSupport)
			{
				NDK_ScrDispImg(1*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[1]);
			}
			else
			{
				PubDisplayLogo(gpLightBuf[1], 1*LED_SPACING+LED_SPACING/4, 2);
			}
		}
		else
		{
			if (YES == gcIsApiSupport)
			{
				NDK_ScrDispImg(1*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[4]);
			}
			else
			{
				PubDisplayLogo(gpLightBuf[4], 1*LED_SPACING+LED_SPACING/4, 2);
			}
		}	

		PubUpdateWindow();
		NDK_ScrSetViewPort(x, y, w, h);
		pthread_mutex_unlock(&gMutex);
		NDK_SysMsDelay(500);
	}
	
	PubDebug("ThreadLedFlick->�˳��߳�\n");	
	pthread_exit(NULL);
	
}

static void ShowLightFlick(void)
{
	int nRet = -1;
	PubDebug("ShowLightFlick->�����߳�\n");
	
	nRet = pthread_create(&gThread, NULL, ThreadLedFlick, NULL);
	if (0 != nRet)
	{
		PubDebug("ShowLightFlick->�����߳�ʧ��[%d]\n", nRet);
		return;
	}
	gcIsCreateThread = YES;
}
static void DecodeLedPic(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //�߱�LED��
	{
		return;
	}
	else if (16 == PubScrGetColorDepth()) //����
	{
		if (YES == gcIsApiSupport)
		{
			NDK_ScrImgDecode("red.png", &gpLightBuf[0]);
			NDK_ScrImgDecode("green.png", &gpLightBuf[1]);
			NDK_ScrImgDecode("yellow.png", &gpLightBuf[2]);
			NDK_ScrImgDecode("blue.png", &gpLightBuf[3]);
			NDK_ScrImgDecode("black.png", &gpLightBuf[4]);
			NDK_ScrImgDecode("white.png", &gpLightBuf[5]);
		}
		else
		{
			gpLightBuf[0] = "red.png";
			gpLightBuf[1] = "green.png";
			gpLightBuf[2] = "yellow.png";
			gpLightBuf[3] = "blue.png";
			gpLightBuf[4] = "black.png";
			gpLightBuf[5] = "white.png";
		}
		
	}
}


static void SetLedMode(EM_LEDMODE emRed, EM_LEDMODE emGreen, EM_LEDMODE emYellow, EM_LEDMODE emBlue)
{	
	gcLight[0] = (gcLight[0] == emRed ? LED_UNCHANGED : emRed);
	gcLight[1] = (gcLight[1] == emGreen ? LED_UNCHANGED : emGreen);
	gcLight[2] = (gcLight[2] == emYellow ? LED_UNCHANGED : emYellow);
	gcLight[3] = (gcLight[3] == emBlue ? LED_UNCHANGED : emBlue);

	/**���ƿ�����˸,����LED_UNCHANGED*/
	if (emBlue == LED_FLICK)
	{
		gcLight[3] = emBlue;
	}
	
	if (emGreen == LED_FLICK)
	{
		gcLight[1] = LED_FLICK;
		ShowLightFlick();
	}

	if (LED_OPEN == gcLight[1]
		|| LED_UNCHANGED == gcLight[1]
		|| LED_CLOSE == gcLight[1]
		) 
	{
		gcIsCreateThread = NO; /**���ڿ����̵߳��˳�*/
	}

}

static void ShowLight(void)
{
	int i  = 0;
	uint x,y,w,h;

	if (gcIsSetViewPort == YES)
	{
		NDK_ScrGetViewPort(&x,&y,&w,&h);
		NDK_ScrSetViewPort(x, y - LED_HIGH, w, h + LED_HIGH);
	}
	if (YES == gcIsLightOff)
	{
		gcIsLightOff = NO;
		NDK_ScrClrLine(0, LED_HIGH);
		if (gcLight[3] != LED_FLICK) /**�������治��ʾ��ɫһ��*/
		{
			PubDisplayLogo("ledbackground.png", 0, 0);
			PubUpdateWindow();
		}
		
	}
	for (i = 0; i < 4; i++)
	{
		switch (gcLight[i])
		{
		case LED_UNCHANGED:
			continue;
			break;
		case LED_CLOSE:
			if (YES == gcIsApiSupport)
			{
				NDK_ScrDispImg(i*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[4]);			
			}
			else
			{
				PubDisplayLogo(gpLightBuf[4], i*LED_SPACING+LED_SPACING/4, 2);
			}
			PubUpdateWindow();
			break;
		case LED_FLICK:
			if (i == 3)//blue ��һ��
			{
				if (YES == gcIsApiSupport)
				{
					NDK_ScrDispImg(i*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[i]);
				}
				else
				{
					PubDisplayLogo(gpLightBuf[i], i*LED_SPACING+LED_SPACING/4, 2);
				}
				PubUpdateWindow();
				NDK_SysMsDelay(200);
				if (YES == gcIsApiSupport)
				{
					NDK_ScrDispImg(i*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[5]);
				}
				else
				{
					PubDisplayLogo(gpLightBuf[5], i*LED_SPACING+LED_SPACING/4, 2);
				}
				PubUpdateWindow();
			}
			break;
		case LED_OPEN:
			if (YES == gcIsApiSupport)
			{
				NDK_ScrDispImg(i*LED_SPACING+LED_SPACING/4,2,LED_SIZE,LED_SIZE,gpLightBuf[i]);
			}
			else
			{
				PubDisplayLogo(gpLightBuf[i], i*LED_SPACING+LED_SPACING/4, 2);
			} 
			PubUpdateWindow();

			break;
		default:
			break;
		}
	}		
	NDK_ScrGetViewPort(&x,&y,&w,&h);
	NDK_ScrSetViewPort(x, y + LED_HIGH, w, h - LED_HIGH);
	gcIsSetViewPort = YES;	

}

static void CLoseLight(void)
{
	gcIsCreateThread = NO;
	gcIsLightOff = YES;
	if (gcIsSetViewPort == YES)
	{
		uint x,y,w,h;
		NDK_ScrGetViewPort(&x,&y,&w,&h);
		NDK_ScrSetViewPort(x, y - LED_HIGH, w, h + LED_HIGH);
	}
	gcIsSetViewPort = NO;
}

/**
* @brief ����ǩ����̧ͷ��ĩβ��"ǩ����"[��������]
* @param in  
* @param out pszOutPrnTile ����̧ͷ
* @return ����
*/
int Cup_SetPrnTitle(const char *pszInTitle, char *pszOutPTitle)
{
	sprintf(pszOutPTitle, "%sǩ����", pszInTitle);
	return APP_SUCC;
}


/**
* @brief ѡ��Ӵ����Ƿǽ�[��������]
* @param in pszTitle ����
* @param out pnOutInputMode ��������ģʽ
* @return ����
*/
int Ums_SelectInputMode(const char *pszTitle, int *pnOutInputMode)
{
	*pnOutInputMode = INPUT_NO;
	
	return APP_SUCC;
}

/**
* @brief ȡ������ʱ̫��,qPboc���������ж�[��������]
* @return ����
*/
int Ums_DealPrintLimit(void)
{
	if (YES != GetFlagQpboc())
	{
		return DealPrintLimit();
	}
	return APP_SUCC;
}

int Ums_DealSettleNum(void)
{
	return APP_SUCC;
}

int Ums_SetMainKeyFromMC(void)
{
	return APP_SUCC;
}

int Ums_SetIsModify(void)
{
	return APP_SUCC;
}

void Ums_MenuLbs()
{
	PubMsgDlg("��ܰ��ʾ", "�ݲ�֧�ָù���", 0, 2);
	return;
}
void Ums_ResetLbsCount()
{
	return;
}
void Ums_IncLbsCount()
{
	return;
}
int Ums_DealStationInfo()
{
	return APP_SUCC;
}


/**
* @brief ��ʾȱʡ���׽���
* @param ��
* @return ��
*/
void EnableDispDefault(void)
{
	int nKey = 0;
	char szSoftVer[16+1] = {0};
	char szShowName[8+1] = {0};
	char szShowInfo[24] = {0};
	char szParamList[128+1] = {0};
	char szTran[64];

	szSoftVer[0] = 'V';
	GetVarDispSoftVer(&szSoftVer[1]);	
	GetShowName(szShowName);

	strcpy(szShowInfo, "��ӭʹ��!");
	sprintf(szParamList, "|'%s '|'%s '|'%s'|", szSoftVer, szShowName, szShowInfo);	

	PubClearAll();
	ShowLightIdle();
	PubLuaDisplay2("LuaEnableDispDefault", szParamList);

	memset(szTran, 0, sizeof(szTran));
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		strcat(szTran, "  F1΢��  ");
	}
	if (GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		strcat(szTran, " F2֧���� ");
	}
	else if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES  || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		strcat(szTran, " F2�ٶ� ");
	}
	
	PubDisplayStrInline(1, 5, szTran);
	PubUpdateWindow();

	nKey = PubGetKeyCode(0);
	
	if ((nKey == KEY_F1)&&(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES))
	{
		if (GetVarIsHaveScanner() == YES)
		{
			MagBarcodePay();
		}
		else
		{
			MagScanQrCodePay();
		}
	}
	else if (nKey == KEY_F2 && GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
	{
		if (GetVarIsHaveScanner() == YES)
		{
			MagAliBarcode();
		}
		else
		{
			MagAlipayScanQrCodePay();
		}
	}
	else if (nKey == KEY_F2 && GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
	{
		
		if (GetVarIsHaveScanner() == YES)
		{
			MagBarcodeBaidu();
		}
		else
		{
			MagScanQrCodePayBaidu();
		}
	}

	
}

/*
�������ش�V0.89��ʼ��֧���·��ĳ��򣬶����·�����
Ӧ����һЩ������UI�ӿڣ�������߱�����ж����ذ汾�š�
*/
int Ums_CheckMcVer(void)
{
	return APP_SUCC;
}


