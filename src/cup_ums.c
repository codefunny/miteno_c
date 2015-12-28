/**
* @file cup_ums.c
* @brief 各应用差异模块
* @version  1.0
* @author 林礼达
* @date 2014-09-23
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"




/**
* 定义全局变量
*/
static char *gpLightBuf[6]; /**红绿黄蓝黑白*/
static char gcLight[4];
static char gcIsLightOff = YES;
static pthread_t gThread; 
static char gcIsCreateThread = NO;
static char gcIsApiSupport = NO;
static char gcIsSetViewPort = NO;

#define LED_HIGH (24)     /**LED行高度*/
#define LED_SPACING (80)  /**灯间距*/
#define LED_SIZE  (20)    /**LED灯的尺寸-正方形*/



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
* 声明内部函数
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
* @brief 非阻塞beep[银联认证需求]
* @param in unFrequency 频率
* @param in unMstime 蜂鸣时长[非阻塞]
* @return 返回
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
* @brief 屏幕LED灯相关初始化[银联认证需求]
* @return 返回
*/
void Cup_InitScrLed(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //具备LED灯
	{
		return ;
	}
	else if (16 == PubScrGetColorDepth()) //彩屏
	{
		CheckImgApi();
		DecodeLedPic();
	}
}

/**
* @brief 显示屏幕LED灯[银联认证需求]
* @param in 四色LED灯的亮灭请求
* @return 返回
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
* @brief 屏幕LED灯资源回收,退出应用时调用[银联认证需求]
* @return 返回
*/
void Cup_ExitScrLed(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //具备LED灯
	{
		return;
	}
	else if (16 == PubScrGetColorDepth()) //彩屏
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
* @brief 屏幕LED灯背景[银联认证需求]
* @return 返回
*/
void Cup_OpenLedBackGround(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //具备LED灯
	{
		return ;
	}
	else if (16 == PubScrGetColorDepth()) //彩屏
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
		PubDebug("支持图片解码API\n");
		gcIsApiSupport = YES;
		return ;
	}
	PubDebug("不支持图片解码API\n");
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
	
	PubDebug("ThreadLedFlick->退出线程\n");	
	pthread_exit(NULL);
	
}

static void ShowLightFlick(void)
{
	int nRet = -1;
	PubDebug("ShowLightFlick->创建线程\n");
	
	nRet = pthread_create(&gThread, NULL, ThreadLedFlick, NULL);
	if (0 != nRet)
	{
		PubDebug("ShowLightFlick->创建线程失败[%d]\n", nRet);
		return;
	}
	gcIsCreateThread = YES;
}
static void DecodeLedPic(void)
{
	if (APP_SUCC == PubGetHardwareSuppot(HARDWARE_SUPPORT_LED_LIGHT, NULL))  //具备LED灯
	{
		return;
	}
	else if (16 == PubScrGetColorDepth()) //彩屏
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

	/**蓝灯空闲闪烁,不能LED_UNCHANGED*/
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
		gcIsCreateThread = NO; /**用于控制线程的退出*/
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
		if (gcLight[3] != LED_FLICK) /**待机界面不显示黑色一栏*/
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
			if (i == 3)//blue 闪一次
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
* @brief 银联签购单抬头在末尾加"签购单"[银联需求]
* @param in  
* @param out pszOutPrnTile 最终抬头
* @return 返回
*/
int Cup_SetPrnTitle(const char *pszInTitle, char *pszOutPTitle)
{
	sprintf(pszOutPTitle, "%s签购单", pszInTitle);
	return APP_SUCC;
}


/**
* @brief 选择接触还是非接[银商需求]
* @param in pszTitle 标题
* @param out pnOutInputMode 返回输入模式
* @return 返回
*/
int Ums_SelectInputMode(const char *pszTitle, int *pnOutInputMode)
{
	*pnOutInputMode = INPUT_NO;
	
	return APP_SUCC;
}

/**
* @brief 取电量耗时太久,qPboc不做电量判断[银联需求]
* @return 返回
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
	PubMsgDlg("温馨提示", "暂不支持该功能", 0, 2);
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
* @brief 显示缺省交易界面
* @param 无
* @return 无
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

	strcpy(szShowInfo, "欢迎使用!");
	sprintf(szParamList, "|'%s '|'%s '|'%s'|", szSoftVer, szShowName, szShowInfo);	

	PubClearAll();
	ShowLightIdle();
	PubLuaDisplay2("LuaEnableDispDefault", szParamList);

	memset(szTran, 0, sizeof(szTran));
	if(GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
	{
		strcat(szTran, "  F1微信  ");
	}
	if (GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES)
	{
		strcat(szTran, " F2支付宝 ");
	}
	else if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES  || GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES)
	{
		strcat(szTran, " F2百度 ");
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
银商主控从V0.89开始才支持新风格的程序，而且新风格的子
应用有一些新增的UI接口，所以这边必须得判断主控版本号。
*/
int Ums_CheckMcVer(void)
{
	return APP_SUCC;
}


