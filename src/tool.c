/**
* @file too.c
* @brief ����ģ�飨������ڣ�
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "ltool.h"
#include "print.h"

/**
* @brief ת�����Ϊ����,����atoll,��ucosƽ̨��֧��atoll,Ϊ�˴���ͳһ��д
* @param in pszAmt ת��ǰ�Ľ���ʽ[12����,����С����]
* @return 
* @li ת����Ľ��
*/
unsigned long long AtoLL(const char *pszAmt)
{
	char szTmpAmt[12+1] = {0};
	int i;
	unsigned long long ullAmount = 0;
	
	memcpy(szTmpAmt, pszAmt, 12);
	PubLeftTrim(szTmpAmt);
	
	for (i = 0; i< strlen(szTmpAmt); i++)
	{
		if (PubIsDigitChar(szTmpAmt[i]) == APP_SUCC)
		{
			ullAmount = ullAmount * 10 + (szTmpAmt[i] - '0');
		}
		else
		{
			break;
		}
	}
	return ullAmount;
}

//�ַ���ȥ�����0,��ȫ0������һ��0
void LeftTrimZero(char *pszSrc)
{
	PubLeftTrimChar((uchar *)pszSrc, '0');
	if(pszSrc[0] == 0)
	{
		pszSrc[0] = '0';
		pszSrc[1] = 0;
	}
}

int AscBigCmpAsc(const char *pszNum1,const char *pszNum2)
{
	int nLen1=0,nLen2=0;
	char szNum1[100+1], szNum2[100+1];
	strcpy(szNum1, pszNum1);
	strcpy(szNum2, pszNum2);
	LeftTrimZero(szNum1);
	LeftTrimZero(szNum2);
	
	nLen1=strlen(szNum1);
	nLen2=strlen(szNum2);
	if (nLen1 == nLen2)
		return strcmp(szNum1,szNum2);
	else if (nLen1 > nLen2)
		return 1;
	else
		return -1;
}


/**
* @brief ת������ʽ������ʾ�ʹ�ӡ
* @param in pszInAmt ת��ǰ�Ľ���ʽ
* @param out pszOutAmt ת����Ľ���ʽ
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL ʧ��
*/
int ProAmtToDispOrPnt(const char *pszInAmt, char *pszOutAmt)
{
	int i;
	char szAmt[12+1] = {0}, szTempAmt[12+1] = {0};
	
	if (NULL == pszInAmt ||NULL == pszOutAmt)
	{
		return APP_FAIL;
	}
	if (memcmp(pszInAmt, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0)
	{
		return APP_FAIL;
	}
	memcpy(szAmt, pszInAmt, 12);
	PubAllTrim(szAmt);
	if (0 == strlen(szAmt))
	{
		return APP_FAIL;
	}
	
	PubAddSymbolToStr(szAmt,12,'0',ADDCH_MODE_BEFORE_STRING);/*�ر����λ������12λ���������--ljz20140224*/
	strcpy(szTempAmt, szAmt);
	for (i = 0; i < strlen(szAmt); i++)
	{
		if (PubIsDigitChar(szAmt[i]) == APP_FAIL)
		{
			return APP_FAIL;
		}
	}
	memset(szAmt, 0, sizeof(szAmt));
	memcpy(szAmt, szTempAmt, 10);
	PubAllTrim(szAmt);
	if ((strlen(szAmt) > 9) && (szAmt[0] != '0'))  /**10λȫ���֣����ܳ���atol����߽�*/
	{
		sprintf(pszOutAmt, "%s.%02d", szAmt, atoi(szTempAmt + 10));
	}
	else
	{
		sprintf(pszOutAmt, "%ld.%02d", atol(szAmt), atoi(szTempAmt + 10));
		PubAddSymbolToStr(pszOutAmt, 13, ' ', 0);
	}
	return APP_SUCC;
}


/**
* @brief ת�����ָ�ʽ������ʾ�ʹ�ӡ
* @param in pszInAmt ת��ǰ�Ľ���ʽ
* @param out pszOutAmt ת����Ľ���ʽ
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL ʧ��
*/
int ProDataToDispOrPnt(const char *pszInData, char *pszOutData)
{
	int i;
	char szData[12+1];
	if (NULL == pszInData ||NULL == pszOutData)
	{
		return APP_FAIL;
	}
	if (memcmp(pszInData, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0)
	{
		return APP_FAIL;
	}
	memset(szData, 0, sizeof(szData));
	memcpy(szData, pszInData, 12);
	for (i = 0; i < 12; i++)
	{
		if (szData[i] == '0')
		{
			szData[i] = ' ';
		}
		else if (szData[i] == ' ')
		{
			;
		}
		else
		{
			break;
		}
	}
	if(szData[12] == ' ')
		szData[12] = '0';
	sprintf(pszOutData, "%12s", szData);
	return APP_SUCC;
}

/*
������������Ҫ֧�����·�ҳ����Ҫ��PubUpDownMenus��ϲ��С�
*/
int ProSelectYesOrNo(char *pszMainMenu, char *pszTitle, char *pszStr, char *pcSelect)
{
	int nSelect;

	if (*pcSelect == 0 || *pcSelect =='0')
	{
		nSelect = 0;
	}
	else if ( *pcSelect ==1 || *pcSelect =='1')
	{
		nSelect = 1;
	}
	else
	{
		nSelect = 0XFF;
	}
	ASSERT_RETURNCODE(PubSelectListItem(pszStr, pszMainMenu, pszTitle, nSelect, &nSelect));
	*pcSelect = nSelect + '0';
	return APP_SUCC;
}

/*
����������Բ���PubUpDownMenus���ֱ��ʵ�����·�ҳ��
*/
int ProSelectYesOrNoExt(char *pszMainMenu, char *pszTitle, char *pszStr, char *pcSelect)
{
	int nRet, nSelect;
	char szFunKey[2+1]={0};
	szFunKey[0] = KEY_F1; //���ز˵��� F1
	szFunKey[1] = KEY_F2; //���ز˵��� F2
	
	if (*pcSelect == 0 || *pcSelect =='0')
	{
		nSelect = 0;
	}
	else if ( *pcSelect ==1 || *pcSelect =='1')
	{
		nSelect = 1;
	}
	else
	{
		nSelect = 0XFF;
	}
	nRet = PubSelectListItemExtend(pszStr, pszMainMenu, pszTitle, nSelect, &nSelect, 60, szFunKey);
	if (nRet==APP_QUIT || nRet == APP_TIMEOUT)
	{
		return APP_QUIT;
	}
	if (nSelect - 0xFF == KEY_UP)
    {
		return KEY_UP;
    }
	else if(nSelect - 0xFF == KEY_DOWN)
	{
		*pcSelect = nSelect + '0';
		return KEY_DOWN;
	}
	*pcSelect = nSelect + '0';
	return APP_SUCC;
}

int ProChkDigitStr(const char *psStr, const int nLen)
{
	int i;
	
	for (i = 0; i < nLen; i++)
	{
		if (PubIsDigitChar(psStr[i]) != APP_SUCC)
		{
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}


int ProConfirm(void)
{
	while(1)
	{
	    PubUpdateWindow();
		switch(PubGetKeyCode(0))
		{
		case KEY_ENTER:
			break;
		case KEY_ESC:
			return APP_QUIT;
		default:
			continue;
		}
		break;
	}
	return APP_SUCC;
}


/*	Func:	����bcd��ŵ���ֵת��Ϊunsigned LONGLONG  ֵ.
��str[3] = "\x12\x34", ��ת��Ϊunsigned LONGLONG :1234.
*	Para:	*bcd			���BCD�ַ�����ͷָ��
*		bcd_len		BCD�ַ����ĳ���
*	Return:		�ɹ�		unsigned LONGLONG ֵ
ʧ��		0
*/
unsigned long long Bcd2Unit64( uchar *bcd,  int bcd_len )
{
	unsigned long long  result = 0;
	uchar *ptr = bcd;

	if( bcd == NULL || bcd_len <= 0 )
		return 0;

	while( ptr < bcd + bcd_len ){
		result = result * 10  + (*ptr >> 4);
		result = result  * 10 + (*ptr++ & 0x0f) ;
	}

	return result;
}

/**
*ͼƬ
*/
const char * ImgManage(EM_LOGO_TYPE emLogoType)
{
	char cIsColor = NO;

	if (1 == PubScrGetColorDepth())  //�ڰ���
	{
		cIsColor = NO; 
	}
	else if (16 == PubScrGetColorDepth())  
	{
		cIsColor = YES;
	}
	
	switch(emLogoType)
	{
	case LOGO_MAIN_MENU:
		if (YES == cIsColor)
		{
			return "CUP320x64.jpg";
		}
		else
		{
			return "CUP128x48.bmp";
		}
		break;
	case LOGO_STANDBY:
		if (YES == cIsColor)
		{
			PubDisplayLogo("CUP130x97.jpg", 0, 6);
		}
		else
		{
			PubDisplayLogo("CUP64x48.bmp", 0, 0);
		}
		break;
	case LOGO_RF:
		if (YES == cIsColor)
		{
			PubDisplayLogo("RF150x93.jpg", 152, 80);
		}
		else
		{
			PubDisplayLogo("RF64x39.bmp", 64, 15);
		}
		break;	
	default:
		break;
	}
	
	return NULL;
}

void AmtAddAmt(uchar *pszAsc1, uchar *pszAsc2, uchar *pszSum)
{
	PubAscAddAsc(pszAsc1, pszAsc2, pszSum);			
	PubAddSymbolToStr((char *)pszSum, 12, '0', 0);
}

void AmtSubAmt(uchar *pszAsc1, uchar *pszAsc2, uchar *pszResult)
{
	PubAscSubAsc(pszAsc1, pszAsc2, pszResult);
	PubAddSymbolToStr((char *)pszResult, 12, '0', 0);
}




void ShowLightIdle()
{
	
}

void ShowLightReady()
{
   
}


void ShowLightDeal()
{
  
}

void ShowLightSucc()
{
   
}

void ShowLightFail()
{
    
}



void ShowLightOff()
{
   
}

void ShowLightOnline()
{
   
}

/**ISO 4217*/
int CurrencyAmtToDisp(const char *pszCurrency, const char *pszAmt, char *pszOutstr)
{
	char *pszCurrcyName[] = 
	{
		"156RMB",	/**�����*/
		"344HKD",	/**�۱�*/ 
		"446MOP",	/**�ı�*/ 
		"458MYR",	/**�������Ǳ�*/ 
		"702SGD",	/**�¼Ӳ�Ԫ*/ 
		"764THB",	/**̩��*/
		"360IDR",	/**ӡ�������Ƕ�*/
		"840USD",	/**��Ԫ*/ 
		"392JPY",	/**��Ԫ*/ 
		"978EUR",	/**ŷԪ*/ 	
		"826GBP",	/**Ӣ��*/
		"124CAD",	/**���ô�Ԫ*/
		NULL,		
	};	

	char **p = NULL;	
	char szDispAmt[13+1] = {0};

	PubDebug("CurrencyAmtToDisp->���Ҵ���[%s]\n", pszCurrency);

	ProAmtToDispOrPnt(pszAmt, szDispAmt);
	PubAllTrim(szDispAmt);
	
	for (p = pszCurrcyName; *p != NULL; p++)
	{
		if (0 == memcmp(pszCurrency, *p, 3))
		{
			sprintf(pszOutstr, "%3.3s %s", *p + 3, szDispAmt);
			return APP_SUCC;
		}
	}

	sprintf(pszOutstr, "OTHER %s", szDispAmt);
	return APP_FAIL;
}

#define SCANAUX PORT_NUM_COM1
//#define SCANAUX PORT_NUM_USB_HOST
//#define SCANAUX PORT_NUM_COM2

#if 0
static int Do_Scan(int nTimeOut, char * pszScanStr)
{
	char szContent[64] = {0};
	char sBuf[128] = {0};
	int nLen = 0;
	int nRet;
	int i;

	nRet = NDK_PortOpen(SCANAUX, "9600");
	if(nRet != NDK_OK)
	{
	    memset(szContent, 0, sizeof(szContent));
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "ɨ��ͷ��ʼ������%d\n|C����������",nRet);
		PubMsgDlg(NULL, szContent, 3, 5);
		return APP_FAIL;
	}
	NDK_PortClrBuf(SCANAUX);
	for(i = 0; i < nTimeOut; i++)
	{
		nRet = NDK_PortRead(SCANAUX, sizeof(sBuf) - 1, sBuf, 1000, &nLen);
		if(nRet != NDK_OK)
		{
			if(KEY_ESC == PubKbHit())
			{
				return APP_QUIT;
			}
		}
		else
		{
			break;
		}
	}
	if(i >= nTimeOut)
	{
		return APP_TIMEOUT;
	}
	//TRACE_HEX((sBuf, nLen, "nRet:[%d]", nRet));
	strcpy(pszScanStr, sBuf);
	return APP_SUCC;
}
#endif

#define SCANNER_DEV_NAME "/dev/scanner"

#define SCANNER_DEV_STRING "scanner"
#define SCANNER_DEV_MINOR 45
#define SCANNER_DEV_MAGIC 'S'

#define SCAN_IOCS_INIT _IO(SCANNER_DEV_MAGIC,0)
#define SCAN_IOCS_TRIG _IO(SCANNER_DEV_MAGIC,1)
#define SCAN_IOCS_EXIT _IO(SCANNER_DEV_MAGIC,2)

int Inner_Scan(int nTimeOut, char * pszScanStr,  int nMax, char *pszContent, char *pszTitle, YESORNO cIsVerify)
{
	int nLen = 0;
	int nRet;
	int nAux;
	int fScanner;
	int nTime;
	time_t oldtime,changetime;
	char szContent[64];
	char szBarCode[256];
	char szBarCodeTmp[128];

	memset(szBarCode, 0, sizeof(szBarCode));
	
	nAux = PORT_NUM_SCAN;
	oldtime=time(NULL);
	
	fScanner=open("/dev/scanner",O_RDONLY);
	if(fScanner<0)
	{
		return APP_FAIL;
	}
	
	ioctl(fScanner,SCAN_IOCS_INIT);
    NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,0);
	ioctl(fScanner,SCAN_IOCS_TRIG);
	
	NDK_PortClrBuf(nAux);
	nRet = NDK_PortOpen(nAux,"9600,8,N,1");
	if(nRet != NDK_OK)
	{
	    memset(szContent, 0, sizeof(szContent));
		PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "ɨ��ͷ��ʼ������%d\n|C����������",nRet);
		PubMsgDlg(NULL, szContent, 3, 5);
		return APP_FAIL;
	}
	
	for(;;)
	{
		nRet = PubKbHit();
		if (nRet == KEY_ESC)
		{
			NDK_PortClose(nAux);
			ioctl(fScanner,SCAN_IOCS_EXIT);
			return APP_QUIT;
		}

		memset(szBarCode, 0, sizeof(szBarCode));
		if(PubIsDigitChar(nRet) == APP_SUCC)
		{
			szBarCode[0] = nRet;
			if (PubInputDlg(pszTitle, pszContent, szBarCode, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
			{	
				ioctl(fScanner,SCAN_IOCS_EXIT);
				return APP_QUIT;
			}
			if (cIsVerify == YES)
			{
				memset(szBarCodeTmp, 0, sizeof(szBarCodeTmp));
				if(PubInputDlg(pszTitle, "��������һ��", szBarCodeTmp, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
				{
					return APP_QUIT;
				}
				if(strcmp(szBarCodeTmp, szBarCode) != 0)
				{
					PubMsgDlg(pszTitle, "����������벻ͬ", 0, 3 );
					return APP_QUIT;
				}
			}
			strcpy(pszScanStr, szBarCode);
			return APP_SUCC;
		}
	
		nRet = NDK_PortRead(nAux, sizeof(szBarCode) - 1, szBarCode, 30, &nLen);
		
		if(nRet == NDK_OK)
		{
			ioctl(fScanner,SCAN_IOCS_EXIT);
			
			strcpy(pszScanStr, szBarCode);
			NDK_PortClose(nAux);
			NDK_SysBeep();
			return APP_SUCC;
		}
		
		changetime = time(NULL);
		nTime = changetime - oldtime;
		//����ɨ��ǹ15���ͻ��Զ��رգ�����15����Ҫ���´�
		if (nTime % 15 == 0)
		{
			ioctl(fScanner,SCAN_IOCS_INIT);
		    NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,0);
			ioctl(fScanner,SCAN_IOCS_TRIG);
		}
		if (nTime > nTimeOut)
		{
			ioctl(fScanner,SCAN_IOCS_EXIT);
			return APP_TIMEOUT;
		}
		
	}
	return APP_QUIT;
}

#if 0
{	
	int n;

	n = nTimeOut/15;
	
	for (j = 0; j < n; j++)
	{
		NDK_ScanInit();
		//���ɨ��ͷ15��ͻ����
		for (i = 0 ; i < 15 ; i++)
		{
			memset(szBarCode, 0, sizeof(szBarCode));
			nRet = NDK_ScanDoScan(1, szBarCode, &nLen);
			if(nRet == NDK_OK)
			{
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);
				strcpy(pszBarCode, szBarCode);
				return APP_SUCC;
			}
			if(KEY_ESC == PubKbHit())
			{
				return APP_QUIT;
			}
		}
	}
	
	return APP_QUIT;
}	


static int Inner_Scan2(char * pszScanStr)
{
	int nLen = 0;
	int nRet;
	int nAux;
	int fScanner;
	char szContent[64];
	char szBarCode[256];

	memset(szBarCode, 0, sizeof(szBarCode));
	
	nAux = PORT_NUM_SCAN;

	while(1)
	{
		nRet = PubKbHit();
		if (nRet == KEY_F3)
		{
			fScanner=open("/dev/scanner",O_RDONLY);
			if(fScanner<0)
			{
				return APP_FAIL;
			}
			nRet = NDK_PortOpen(nAux,"9600,8,N,1");
			if(nRet != NDK_OK)
			{
			    memset(szContent, 0, sizeof(szContent));
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "ɨ��ͷ��ʼ������%d\n|C����������",nRet);
				PubMsgDlg(NULL, szContent, 3, 5);
				return APP_FAIL;
			}
			
			ioctl(fScanner,SCAN_IOCS_INIT);
		    NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,0);
			ioctl(fScanner,SCAN_IOCS_TRIG);
			NDK_PortClrBuf(nAux);
	
			nRet = NDK_PortRead(nAux, sizeof(szBarCode) - 1, szBarCode, 1, &nLen);
			
			if(nRet == NDK_OK)
			{
				ioctl(fScanner,SCAN_IOCS_EXIT);
				
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);

				strcpy(pszScanStr, szBarCode);
				NDK_PortClose(nAux);
				return APP_SUCC;
			}
		}
		else
		{
			NDK_PortClose(nAux);
			ioctl(fScanner,SCAN_IOCS_EXIT);
			return APP_QUIT;
		}
		
	}
	return APP_QUIT;
}



int ScanBarCode4(char *pszTitle, char *pszContent, int nMax, char * pszBarCode, YESORNO cIsVerify)
{
	int nRet;
	int nLen = 0;
	int nAux;
	int fScanner;
	int nTimeOut = 60;
	char cisScan;
	char szBarCode[256];
	char szBarCodeTmp[256];

	memset(szBarCode, 0, sizeof(szBarCode));
	
	PubClear2To4();
	PubDisplayStr(DISPLAY_MODE_LEFT, 2, 1, pszContent);
	PubDisplayStr(DISPLAY_MODE_LEFT, 3, 1, "��ɨ��ǹɨ��");
	PubUpdateWindow();

	if(GetVarIsOutsideScanner() == YES)
	{
		nAux = SCANAUX;
		nRet = NDK_PortOpen(nAux, "9600");
		NDK_PortClrBuf(nAux);
	}
	else
	{
		nAux = PORT_NUM_SCAN;
		fScanner=open("/dev/scanner",O_RDONLY);
    	if(fScanner<0)
    	{
        	nRet = APP_FAIL;
    	}
		else
		{
			nRet = NDK_PortOpen(nAux,"9600,8,N,1");
			NDK_PortClrBuf(nAux);

			ioctl(fScanner,SCAN_IOCS_INIT);
		    NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,0);
		
			ioctl(fScanner,SCAN_IOCS_TRIG);
		}
	}
	if (nRet != APP_SUCC)
	{
		cisScan = NO;
		PubClear2To4();
		PubDisplayStr(DISPLAY_MODE_LEFT, 2, 1, pszContent);
		PubUpdateWindow();
	}
	else
	{
		cisScan = YES;
	}
	
	while(1)
	{
		//����
		nRet = 0;
		nRet = PubKbHit();
		if (nRet == KEY_ESC)
		{
			NDK_PortClose(nAux);
			if(GetVarIsOutsideScanner() == NO)
			{
				ioctl(fScanner,SCAN_IOCS_EXIT);
			}
			return APP_QUIT;
		}
		else if(PubIsDigitChar(nRet) == APP_SUCC)
		{
			szBarCode[0] = nRet;
			if (PubInputDlg(pszTitle, pszContent, szBarCode, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
			{
				if(GetVarIsOutsideScanner() == NO)
				{
					ioctl(fScanner,SCAN_IOCS_EXIT);
				}
				return APP_QUIT;
			}
			if (cIsVerify == YES)
			{
				memset(szBarCodeTmp, 0, sizeof(szBarCodeTmp));
				if(PubInputDlg(pszTitle, "��������һ��", szBarCodeTmp, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
				{
					if(GetVarIsOutsideScanner() == NO)
					{
						ioctl(fScanner,SCAN_IOCS_EXIT);
					}
					return APP_QUIT;
				}
				if(strcmp(szBarCodeTmp, szBarCode) != 0)
				{
					PubMsgDlg(pszTitle, "����������벻ͬ", 0, 3 );
					memset(szBarCode, 0, sizeof(szBarCode));
					if(GetVarIsOutsideScanner() == NO)
					{
						ioctl(fScanner,SCAN_IOCS_EXIT);
					}
					return APP_QUIT;
				}
			}
			strcpy(pszBarCode, szBarCode);
			if(GetVarIsOutsideScanner() == NO)
			{
				ioctl(fScanner,SCAN_IOCS_EXIT);
			}
			return APP_SUCC;
		}
		
		//ɨ��
		if (cisScan == YES)
		{
			nRet = NDK_ERR;
			nLen = 0;
			if (NDK_PortReadLen(nAux, &nLen) != NDK_OK)
			{
				NDK_PortClrBuf(nAux);
			}
			if(nLen > 0)
			{
				nRet = NDK_PortRead(nAux, sizeof(szBarCode) - 1, szBarCode, 1, &nLen);
			}

			if(nRet == NDK_OK)
			{
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
				PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);
				strcpy(pszBarCode, szBarCode);
				NDK_PortClose(nAux);
				if(GetVarIsOutsideScanner() == NO)
				{
					ioctl(fScanner,SCAN_IOCS_EXIT);
				}
				return APP_SUCC;
			}
		}

	}
	
	if(GetVarIsOutsideScanner() == NO)
	{
		ioctl(fScanner,SCAN_IOCS_EXIT);
	}
	return APP_QUIT;
}




int ScanBarCode2(char *pszTitle, int nTimeOut , char * pszBarCode)
{
	int nRet;
	int nLen = 0;
	int i;
	char szBarCode[128+1]={0};

	PubKbHit();
	memset(szBarCode, 0, sizeof(szBarCode));
	if(GetVarIsHaveScanner() != YES)
	{
		ASSERT_RETURNCODE(PubInputDlg(pszTitle, "������΢�ź�:", szBarCode, \
			&nLen, 0, 10, 60, INPUT_MODE_STRING));
	}
	else
	{	
		PubClearAll();
		PubDisplayTitle(pszTitle);
		PubDisplayGen(2, "��ɨ��");
		PubDisplayGen(3, "΢��������");
		PubUpdateWindow();
		
		if(GetVarIsOutsideScanner() == YES)
		{
			nRet = Do_Scan(nTimeOut, szBarCode);
			if(nRet != APP_SUCC)
			{
				return nRet;
			}
		}
		else
		{
			for (i = 0 ; i < nTimeOut ; i++)
			{
			    memset(szBarCode, 0, sizeof(szBarCode));
				nRet = NDK_ScanDoScan(1, szBarCode, &nLen);
				if(nRet == NDK_OK)
				{
					break;
				}
				if(KEY_ESC == PubKbHit())
				{
					return APP_QUIT;
				}
			}
			if(i >= nTimeOut)
			{
				return APP_TIMEOUT;
			}
		}	
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);
	}
	strcpy(pszBarCode, szBarCode);
	return APP_SUCC;
}
#endif

int ScanBarCode(char *pszTitle, char *pszContent, int nMax, char * pszBarCode, YESORNO cIsVerify)
{
	int nRet;
	int nTimeOut = 60;
	char szBarCode[128];

	PubClear2To4();
	PubDisplayStr(DISPLAY_MODE_LEFT, 2, 1, pszContent);
	PubDisplayStr(DISPLAY_MODE_LEFT, 3, 1, "��ʹ��ɨ��ǹɨ��");
	PubUpdateWindow();

	

	if(GetVarIsOutsideScanner() == YES)
	{	

		nRet = Outlay_Scan(nTimeOut, szBarCode, nMax, pszContent, pszTitle, cIsVerify);
		if(nRet != APP_SUCC)
		{
			return APP_QUIT;
		}
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);
		if (strlen(szBarCode) > nMax)
		{
			return APP_FAIL;
		}
		strcpy(pszBarCode, szBarCode);
		return APP_SUCC;
	
	}
	else
	{
		nRet = Inner_Scan(nTimeOut, szBarCode, nMax, pszContent, pszTitle, cIsVerify);
		if(nRet != APP_SUCC)
		{
			return APP_QUIT;
		}
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0D);
		PubDelSymbolFromStr((uchar*)szBarCode, 0x0A);
		if (strlen(szBarCode) > nMax)
		{
			return APP_FAIL;
		}
		strcpy(pszBarCode, szBarCode);
		return APP_SUCC;
	}
	return APP_FAIL;
}


//���������������
int CommSetSuspend(uint unFlag)
{
	int nRet = -1;
	int i = 0;
	
	while (1)
	{
		if (i == 3)
		{
			break;
		}
		nRet = NDK_SysSetSuspend(unFlag);
		if (NDK_OK != nRet)
		{
			NDK_SysDelay(30);//��ʱ3��
			i++;
			continue;
		}
		return APP_SUCC;
	}
	return APP_FAIL;
}


int ProShowAndPrint(BmpInfo *pBmpBuff, char *pszAmount)
{
	PrintDataInfo stDataInfo;

	//��ʾ
	PubClearAll();
	//NDK_ScrDrawBitmap((320 - BmpBuff.xsize) / 2, 10, BmpBuff.xsize, BmpBuff.ysize, BmpBuff.bmpbuff);
	NDK_ScrDrawBitmap(30, 15, pBmpBuff->xsize, pBmpBuff->ysize, pBmpBuff->bmpbuff);
	PubDisplayStr(8, 3, 2, "F2��ӡ");
	PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "ɨ�貢֧����ȷ��");
	PubUpdateWindow();
	PubBeep(1);

	stDataInfo.pszAmt = pszAmount;
	stDataInfo.pBmpInfo = pBmpBuff;
	
	while(1)
	{
	    PubUpdateWindow();
		switch(PubGetKeyCode(0))
		{
		case KEY_F2://��ӡ
			PrintImage(stDataInfo);
			continue;
		case KEY_ENTER:
			break;
		case KEY_ESC:
			return APP_QUIT;
		default:
			continue;
		}
		break;
	}
	return APP_SUCC;
}



int ProJDShowAndPrint(BmpInfo *pBmpBuff, char *pszAmount)
{
	PrintDataInfo stDataInfo;

	//��ʾ
	PubClearAll();
	//NDK_ScrDrawBitmap((320 - BmpBuff.xsize) / 2, 10, BmpBuff.xsize, BmpBuff.ysize, BmpBuff.bmpbuff);
	NDK_ScrDrawBitmap(15, 15, pBmpBuff->xsize, pBmpBuff->ysize, pBmpBuff->bmpbuff);
	PubDisplayStr(8, 2, 2, "F2��ӡ");
	PubDisplayStr(8, 3, 2, "��ɰ�");
	PubDisplayStr(8, 4, 2, "[ȷ��]");
	//PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "ɨ�貢֧����ȷ��");
	PubUpdateWindow();
	PubBeep(1);

	stDataInfo.pszAmt = pszAmount;
	stDataInfo.pBmpInfo = pBmpBuff;
	
	while(1)
	{
	    PubUpdateWindow();
		switch(PubGetKeyCode(0))
		{
		case KEY_F2://��ӡ
			PrintImage(stDataInfo);
			continue;
		case KEY_ENTER:
			break;
		case KEY_ESC:
			return APP_QUIT;
		default:
			continue;
		}
		break;
	}
	return APP_SUCC;
}


int DealQRCode(char *pszCodeUrl, char *pszAmount, char cTransType)
{
	DataInfo stDataInfo;
	BmpInfo BmpBuff;

	memset(&stDataInfo, 0, sizeof(DataInfo));
	memset(&BmpBuff, 0, sizeof(BmpInfo));

	stDataInfo.nLen = strlen(pszCodeUrl);
	stDataInfo.nLevel = 3;
	stDataInfo.nMask = -1;
	stDataInfo.nVersion = 0;
	strcpy(stDataInfo.szInputData, pszCodeUrl);

	if(APP_SUCC == PubEncodeDataAndGenerateBmp(&stDataInfo, &BmpBuff))
	{
		
		CommSetSuspend(0);	//��ֹ����
		if (cTransType == TRANS_JD_PRECREATE)
		{
			ASSERT_QUIT(ProJDShowAndPrint(&BmpBuff, pszAmount));
		}
		else
		{
			ASSERT_QUIT(ProShowAndPrint(&BmpBuff, pszAmount));
		}
		CommSetSuspend(1);	//��������
	}
	else
	{
		PubClearAll();
		PubMsgDlg(NULL, "��������ʧ��", 0, 10);
		return APP_FAIL;
	}

	return APP_SUCC;
}


void BcdAdd(unsigned char *Dest,unsigned char *Src,unsigned int Len)
{
    unsigned int i;
    unsigned char high,low;
    unsigned char carry=0;                     //�����λ��־

    for(i=Len;i>0;i--)
    {
         //��4λ�ֽڵ�����
         low = (*(Dest+i-1) & 0x0F) + (*(Src+i-1) & 0x0F) + carry;
         carry=0;
         if(low > 0x09)
         {
            low = low-0x0A; 
            carry = 1;  
         }
         //�ߵ�4λ�ֽڵ�����
         high  = ( (*(Dest+i-1) & 0xF0) >> 4) + ((*(Src+i-1) & 0xF0) >> 4) + carry;  
         carry=0;
         if(high > 0x09)
         {
            high= high-0x0A; 
            carry = 1;  
         }  
         *(Dest+i-1) = (high << 4) | low;
    }   
}


void BcdSub(uchar *Dest,uchar *Src,unsigned int Len)
{
    unsigned int i;
    unsigned char high,low;
    unsigned char carry=0;                     //�����λ��־

    for(i=Len;i>0;i--)
    {
         //��nibble������
         if( (*(Dest+i-1) & 0x0F) < ( (*(Src+i-1) & 0x0F) + carry) )
         {
            low = ((*(Dest+i-1) & 0x0F)+0x0A) - (*(Src+i-1) & 0x0F) - carry;
            carry=1;
         }
         else
         {
            low = (*(Dest+i-1) & 0x0F) - (*(Src+i-1) & 0x0F) - carry;
            carry=0;
         }
         
         //��nibble������ 
         if( ((*(Dest+i-1) & 0xF0) >> 4) < (((*(Src+i-1) & 0xF0) >> 4) + carry) )
         {
            high = (((*(Dest+i-1) & 0xF0) >> 4)+0x0A) - ((*(Src+i-1) & 0xF0) >> 4)  - carry;
            carry=1;
         }
         else 
         {
            high = ((*(Dest+i-1) & 0xF0) >> 4) - ((*(Src+i-1) & 0xF0) >> 4)  - carry;
            carry=0;         
         }   
         *(Dest+i-1) = (high << 4) | low;
    }   
}     


int Outlay_Scan(int nTimeOut, char * pszScanStr,  int nMax, char *pszContent, char *pszTitle, YESORNO cIsVerify)
{
	int nLen = 0;
	int nRet;
	int nAux;
	int nTime;
	time_t oldtime,changetime;
	char szBarCode[256];
	char szBarCodeTmp[128];
	int nKeycode;

	nAux = SCANAUX;
	nRet = NDK_PortOpen(nAux, "9600");
	NDK_PortClrBuf(nAux);
	oldtime=time(NULL);

	//����
	nKeycode = 0;
	memset(szBarCode, 0, sizeof(szBarCode));
	
	for(;;)
	{
		nKeycode = PubKbHit();
		if (nKeycode == KEY_ESC)
		{
			NDK_PortClose(nAux);
			return APP_QUIT;
		}
		
		if(PubIsDigitChar(nKeycode) == APP_SUCC)
		{
			szBarCode[0] = nKeycode;
			if (PubInputDlg(pszTitle, pszContent, szBarCode, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
			{
				NDK_PortClose(nAux);
				return APP_QUIT;
			}
			if (cIsVerify == YES)
			{
				memset(szBarCodeTmp, 0, sizeof(szBarCodeTmp));
				if(PubInputDlg(pszTitle, "��������һ��", szBarCodeTmp, &nLen, 1, nMax, nTimeOut, INPUT_MODE_STRING) != APP_SUCC)
				{
					NDK_PortClose(nAux);
					return APP_QUIT;
				}
				if(strcmp(szBarCodeTmp, szBarCode) != 0)
				{
					PubMsgDlg(pszTitle, "����������벻ͬ", 0, 3 );
					NDK_PortClose(nAux);
					return APP_QUIT;
				}
			}
			strcpy(pszScanStr, szBarCode);	
			NDK_PortClose(nAux);
			return APP_SUCC;
		}
			
		nRet = NDK_ERR;
		nLen = 0;
		
		if (NDK_PortReadLen(nAux, &nLen) != NDK_OK)
		{
			NDK_PortClrBuf(nAux);
		}
		if(nLen > 0)
		{
			nRet = NDK_PortRead(nAux, sizeof(szBarCode) - 1, szBarCode, 100, &nLen);
			strcpy(pszScanStr, szBarCode);
			NDK_PortClose(nAux);
			NDK_SysBeep();
			return APP_SUCC;
		}

		changetime = time(NULL);
		nTime = changetime - oldtime;
		if (nTime > nTimeOut)
		{
			NDK_PortClose(nAux);
			return APP_TIMEOUT;
		}
	}
	return APP_SUCC;
}


