/**
* @file print.c
* @brief 打印处理模块
* @version  1.0
* @author 薛震
* @date 2007-01-25
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"
#include "barcode.h"

#define DISP_CHK_CARD_HOLDER_SIGNATURE \
{\
	PubDisplayTail(DISPLAY_ALIGN_BIGFONT, DISPLAY_MODE_CENTER, YES, "请核对签名");\
	PubUpdateWindow();\
	PubBeep(3);\
}

/**
* 以下为本模块内部使用的函数
*/
static int SetPrintData_Water(const STWATER *, int, const int);
static int SetAndPrint_Settle(void *);
static int SetAndPrint_Allwater(void *);
static int SetAndPrint_Total(void *);
static int SetAndPrint_Param(void * );
//static int SetAndPrint_Failwater(void *);
static int _printwater(void *);
//static int Print_GetTransType(STWATER *, char *);
//static int PntHex(const char *, const uchar *, const int);
//static int  _printemvparam();
static void DealPrintAmount(char, char *);
static int printYiwei(const char *pszCode);
//static int PntHex(const char *pszTitle, const uchar *psContent, const int nConternLen);


enum PRINT_PARAM_TYPE
{
	PRINT_PARAM_TYPE_MERCHANTINFO=0x01,		/*商户信息*/
	PRINT_PARAM_TYPE_TRANSCCTRL=0x02,		/*交易控制*/
	PRINT_PARAM_TYPE_SYSTEMCTRL=0x04,		/*系统控制*/
	PRINT_PARAM_TYPE_COMM=0x08,				/*通讯参数*/
	PRINT_PARAM_TYPE_VERSION=0x10,			/*版本信息*/
	PRINT_PARAM_TYPE_EMV=0x20,			/*EMV参数*/
	PRINT_PARAM_TYPE_OTHER=0x40				/*其他*/
};


/* 格式修改[1234+简称+银行名称]*/
static const char gszBankName[][20]={//存放银行名称
	{"0100邮储邮储银行  "},
	{"0102工行工商银行  "},
	{"0103农行农业银行  "},
	{"0104中行中国银行  "},
	{"0105建行建设银行  "},
	{"0301交行交通银行  "},
	{"0302中信中信银行  "},
	{"0303光大光大银行  "},
	{"0304华夏华夏银行  "},
	{"0305民生民生银行  "},
	{"0306广发广发银行  "},
	{"0307深发深发银行  "},
	{"0308招行招商银行  "},
	{"0309兴业兴业银行  "},
	{"0310浦发浦发银行  "},
	{"0001异地异地银行  "},
	{"0313商行商业银行  "},
	{"0401上海上海银行  "},
	{"0403北京北京银行  "},
	{"1409农信农村信用社"},
	{"0464泉商泉州商行  "},	/* 泉州商业*/
	{"0405福商福州商行  "},	/* 福州商业*/
	{"0410平安平安银行  "},
	{"1410农信农村信用社"},
	{"0414武商武汉商行  "},	/* 武汉商行*/
	{"0425东银东莞银行  "},
	{"0432宜商宜昌商行  "},	/* 宜昌商行*/
	{"0461长银长沙银行  "},
	{"0464泉商泉州商行  "},	/* 泉州商业*/
	{"0489南银南粤银行  "},
	{"0570华湘华融湘江  "},
	{"1401上农上海农商  "},
	{"1418北农北京农商  "},
	{"1438湖农湖南农信  "},
	{"4802银商银联商务  "},
	{"0000"},
};					

typedef struct tagPrintPara
{
	const STWATER *pstWater;
	int nReprintFlag;
	int nCurrentPage;
} STPRINTPATA;


typedef struct tagPrintImage
{
	const BmpInfo *pBmpInfo;
}STPRINTIMAGE;



/**
* @brief 打印签购单
*
*    通过调用_printwater 实现
* @param in const STWATER *pstWater 流水结构指针
* @param in int nReprintFlag  打印控制标志
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintWater(const STWATER *pstWater, int nReprintFlag)
{
	int nCurrentPage,nPage=1;
	STPRINTPATA stPara;
	
	if (PubGetPrinter() == _PRINTTYPE_TP)
	{
		GetVarPrintPageCount(&nPage);
	}

	for(nCurrentPage = nPage; nCurrentPage >= 1; nCurrentPage--)
	{
		stPara.pstWater = pstWater;
		stPara.nReprintFlag = nReprintFlag;
		stPara.nCurrentPage = nCurrentPage;

		ASSERT_FAIL(PubPrintCommit( _printwater, (void *)&stPara, 1));
		if (nCurrentPage > 1)
		{
			NDK_ScrPush();
			PubClearAll();
			PubDisplayStrInline(DISPLAY_MODE_CENTER, 4, "请 撕 票 据...");
			PubUpdateWindow();
			if(PubGetKeyCode(30) == KEY_ESC)//0.88需求改成30秒
			{
				//141103根据江苏银商提的需求，加入响应取消键不继续打印
				return APP_SUCC;
			}
			PubClearLine(4, 4);
			NDK_ScrPop();
		}
	}
	return APP_SUCC;
}



/**
* @brief 打印签购单
*
*    供函数PrintWater调用以实现签购单据的打印
* @param in void *ptrPara 参数结构指针
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int _printwater( void *ptrPara )
{
	STPRINTPATA *pstPara;
	pstPara = (STPRINTPATA *)ptrPara;
	char szModuleName[50] = {0};
	// 判断打印机是热敏还是针打
	ASSERT_FAIL(SetPrintData_Water(pstPara->pstWater, pstPara->nReprintFlag, pstPara->nCurrentPage));
	if (PubGetPrinter()==_PRINTTYPE_TP)	// 热敏
	{
		PubSetPrnFontSize(GetVarPrnFontSize());
		strcpy(szModuleName, "PRINT_WATER_WITHOUT_ENGLISH");
	}
	else if (PubGetPrinter()==_PRINTTYPE_IP)// 针打
	{
		//判断针打使用的打印票据格式
		if (YES == GetVarIsNewTicket())
		{
			strcpy(szModuleName, "PRINT_WATER_IP_UNBLANK");
		}
		else
		{
			strcpy(szModuleName, "PRINT_WATER_IP_BLANK");
		}
	}	
	else
		return APP_FAIL;
	PubPrintModule(szModuleName);

	
	return APP_SUCC;
}


/**
* @brief 根据所给的交易类型获得交易的中英文名称(供热敏打印机的打印函数调用)
* @param in char cTransType 交易类型
* @param out char *pszTransName1 交易类型对应的中文名称
* @param out char *pszTransName2 交易类型对应的英文名称
* @return 无
*/
void GetTransNameForTP(const STWATER *pstWater, char *pszTransName1, char *pszTransName2)
{	

	switch( pstWater->cTransType )
	{
		case TRANS_COUPON_VERIFY:
			strcpy(pszTransName1, "串码承兑");
			strcpy(pszTransName2, "Verify By Coupon number");
			break;
		case TRANS_TEL_VERIFY:
			strcpy(pszTransName1, "手机号承兑");
			strcpy(pszTransName2, "Verify By phonenumber");
			break;
		case TRANS_CARD_VERIFY:
			strcpy(pszTransName1, "银行卡验券");
			strcpy(pszTransName2, "Verify By card");
			break;
		case TRANS_VOID_COUPON_VERIFY:	
			strcpy(pszTransName1, "串码验券撤销");
			strcpy(pszTransName2, "Void Coupon Verify(COU.)");
			break;
		case TRANS_VOID_TEL_VERIFY: 
			strcpy(pszTransName1, "手机号验券撤销");
			strcpy(pszTransName2, "Void Coupon Verify(PHO.)");
			break;
		case TRANS_VOID_CARD_VERIFY: 
			strcpy(pszTransName1, "银行卡验券撤销");
			strcpy(pszTransName2, "Void Coupon Verify(CAR.)");
			break;
		case TRANS_REFUND_VERIFY:
			strcpy(pszTransName1, "退货");
			strcpy(pszTransName2, "Refund");
			break;			
		case TRANS_PRECREATE:
			strcpy(pszTransName1, "微信支付被扫");
			strcpy(pszTransName2, "Scan by wechat");
			break;
		case TRANS_VOID_PRECREATE:
			strcpy(pszTransName1, "微信支付被扫撤销");
			strcpy(pszTransName2, "Void Scan by wechat");
			break;
		case TRANS_CREATEANDPAY:
			strcpy(pszTransName1, "微信支付主扫");
			strcpy(pszTransName2, "Scan by POS(Wechat)");
			break;	
		case TRANS_VOID_CREATEANDPAY:
			strcpy(pszTransName1, "微信支付主扫撤销");
			strcpy(pszTransName2, "Void Scan by POS");
			break;
		case TRANS_WX_REFUND:
			strcpy(pszTransName1, "微信退货");
			strcpy(pszTransName2, "Refund for Wechat");
			break;	
		case TRANS_CREATEANDPAYBAIDU:
			strcpy(pszTransName1, "百度钱包主扫");
			strcpy(pszTransName2, "Scan by POS(Baidu)");
			break;
		case TRANS_BAIDU_PRECREATE:
			strcpy(pszTransName1, "百度钱包被扫");
			strcpy(pszTransName2, "Scan by Baidu");
			break;
		case TRANS_BAIDU_REFUND:
			strcpy(pszTransName1, "百度钱包退货");
			strcpy(pszTransName2, "Refund for Baidu");
			break;	
		case TRANS_VOID_CREATEANDPAYBAIDU:
			strcpy(pszTransName1, "百度钱包主扫撤销");
			strcpy(pszTransName2, "Void for Baidu");
			break;	
		case TRANS_VOID_BAIDU_PRECREATE:
			strcpy(pszTransName1, "百度钱包被扫撤销");
			strcpy(pszTransName2, "Void for Baidu");
			break;	
		case TRANS_JD_PRECREATE:
			strcpy(pszTransName1, "京东钱包被扫");
			strcpy(pszTransName2, "Scan by JD");
			break;
		case TRANS_JD_CREATEANDPAY:
			strcpy(pszTransName1, "京东钱包主扫");
			strcpy(pszTransName2, "Scan by POS(JD)");
			break;
		case TRANS_JD_REFUND:
			strcpy(pszTransName1, "京东退货");
			strcpy(pszTransName2, "Refund for JD");
			break;	
		case TRANS_VOID_JD_PRECREATE:
			strcpy(pszTransName1, "京东钱包被扫撤销");
			strcpy(pszTransName2, "Void for JD");
			break;	
		case TRANS_VOID_JD_CREATEANDPAY:
			strcpy(pszTransName1, "京东钱包主扫撤销");
			strcpy(pszTransName2, "Void for JD");
			break;	
		case TRANS_ALI_CREATEANDPAY:
			strcpy(pszTransName1, "支付宝主扫");
			strcpy(pszTransName2, "Alipay payment");
			break;	
		case TRANS_ALI_PRECREATE:
			strcpy(pszTransName1, "支付宝被扫");
			strcpy(pszTransName2, "Scan by Alipay");
			break;
		case TRANS_ALI_REFUND:
			strcpy(pszTransName1, "支付宝退货");
			strcpy(pszTransName2, "Refund for Alipay");
			break;	
		case TRANS_VOID_ALI_CREATEANDPAY:
			strcpy(pszTransName1, "支付宝主扫撤销");
			strcpy(pszTransName2, "Void for Alipay");
			break;	
		case TRANS_VOID_ALI_PRECREATE:
			strcpy(pszTransName1, "支付宝被扫撤销");
			strcpy(pszTransName2, "Void for Alipay");
			break;	
		case TRANS_COUPON_MEITUAN:
			strcpy(pszTransName1, "美团承兑");
			strcpy(pszTransName2, "Meituan Coupon");
			break;
		case TRANS_VOID_MEITUAN:
			strcpy(pszTransName1, "美团撤销");
			strcpy(pszTransName2, "Void for Meituan");
			break;	
		case TRANS_PANJINTONG:
			strcpy(pszTransName1, "盘锦通支付");
			strcpy(pszTransName2, "Panjin payment");
			break;
		case TRANS_DZ_PRECREATE:
			strcpy(pszTransName1, "大众点评被扫");
			strcpy(pszTransName2, "Scan by Dianping");
			break;
		case TRANS_DZ_CREATEANDPAY:
			strcpy(pszTransName1, "大众点评主扫");
			strcpy(pszTransName2, "Scan by POS(Dianping)");
			break;
		case TRANS_DZ_REFUND:
			strcpy(pszTransName1, "大众点评退货");
			strcpy(pszTransName2, "Refund for Dianping");
			break;	
		case TRANS_VOID_DZ_PRECREATE:
			strcpy(pszTransName1, "大众点评被扫撤销");
			strcpy(pszTransName2, "Void for Dianping");
			break;	
		case TRANS_VOID_DZ_CREATEANDPAY:
			strcpy(pszTransName1, "大众点评主扫撤销");
			strcpy(pszTransName2, "Void for Dianping");
			break;	
		case TRANS_COUPON_DAZHONG:
			strcpy(pszTransName1, "大众点评承兑");
			strcpy(pszTransName2, "Dianping Coupon");
			break;
		case TRANS_BESTPAY_CREATEANDPAY:
			strcpy(pszTransName1, "翼支付");
			strcpy(pszTransName2, "Bestpay");
			break;
		case TRANS_BESTPAY_REFUND:
			strcpy(pszTransName1, "翼支付退货");
			strcpy(pszTransName2, "Refund for Bestpay");
			break;
		case TRANS_VOID_BESTPAY_CREATEANDPAY:
			strcpy(pszTransName1, "翼支付撤销");
			strcpy(pszTransName2, "Void for Bestpay");
			break;
		case TRANS_ALLPAY_CREATEANDPAY:
			sprintf(pszTransName1, "都能付(%s)", pstWater->szTransName);
			strcpy(pszTransName2, "Allpay");
			break;
		case TRANS_ALLPAY_REFUND:
			sprintf(pszTransName1, "都能付退货(%s)", pstWater->szTransName);
			strcpy(pszTransName2, "Refund for Allpay");
			break;
		default:
			pszTransName1[0] = 0;
			pszTransName2[0] = 0;
			break;
	}
}

void GetTransNameForTP2(char cTransType, char *psTransAttrAndStatus, char *pszTransName1, char *pszTransName2)
{	

	switch( cTransType )
	{

		case TRANS_COUPON_VERIFY:
			strcpy(pszTransName1, "优惠券承兑");
			break;
		case TRANS_TEL_VERIFY:
			strcpy(pszTransName1, "优惠券承兑");
			break;
		case TRANS_CARD_VERIFY:
			strcpy(pszTransName1, "消费");
			break;
		default:
			pszTransName1[0] = 0;
			pszTransName2[0] = 0;
			break;
	}
}


/**
* @brief 打印全部流水明细
*
*    通过调用_printallwater 实现
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintAllWater(void)
{
	int nWaterNum, nCurPrintRecNo=1;
	int nRet;
	
	//计算总笔数
	GetWaterNum(&nWaterNum);
	if( nWaterNum<=0 )
	{
		PubMsgDlg("打印交易明细", "无交易", 0, 5);
		return APP_SUCC;
	}
	
	while(1)
	{
		if(nCurPrintRecNo > nWaterNum)
		{
			break;
		}

		PubPrintFeedPaper(1);
		
		nRet = PubPrintCommit(SetAndPrint_Allwater, (void *)&nCurPrintRecNo, 0);
		if(nRet != APP_SUCC)
		{
			if (APP_FUNCQUIT == nRet)
			{
				PubMsgDlg("打印交易明细", "无可打印交易", 1, 3);
			}
			return nRet;
		}
	}
	
	return APP_SUCC;
}


/**
* @brief 打印结算单
*
*    通过调用_printsettle 实现
* @param in int nReprintFlag  打印控制标志
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintSettle(int nReprintFlag)
{	
	ASSERT_FAIL(PubPrintCommit( SetAndPrint_Settle, (void *)&nReprintFlag, 1));
	return APP_SUCC;
}

/**
* @brief 打印汇总数据
*
*    通过调用_printsettle 实现
* @param in int nReprintFlag  打印控制标志
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintTotal()
{
	ASSERT(PubPrintCommit( SetAndPrint_Total, (void *)NULL, 1));
	return APP_SUCC;
}





int PrintParam(void)
{
	int nRet;
	int nSelect=0xff;
	int nType=0;

	while(1)
	{
		nRet = ProSelectList("1.商户信息||2.系统控制||3.通讯参数||4.版本信息||5.其他", "参数打印", nSelect, &nSelect);
		if (nRet==APP_QUIT || nRet==APP_TIMEOUT)
		{
			return nRet;
		}

		switch(nSelect)
		{
		case 1:
			nType=PRINT_PARAM_TYPE_MERCHANTINFO;
			break;
#if 0			
		case 2:
			nType=PRINT_PARAM_TYPE_TRANSCCTRL;	
			break;
#endif			
		case 2:
			nType=PRINT_PARAM_TYPE_SYSTEMCTRL;
			break;
		case 3:
			nType=PRINT_PARAM_TYPE_COMM;
			break;
		case 4:
			nType=PRINT_PARAM_TYPE_VERSION;
			break;
		case 5:
			nType=PRINT_PARAM_TYPE_OTHER;
			break;
		default :                         
			continue;
		}

		ASSERT_FAIL(PubPrintCommit(SetAndPrint_Param, (void *)&nType, 1));
	}
	return APP_SUCC;
}


/**
* @brief 重打印交易流水和结算单
*    供main.c 模块中的函数MenuFuncSel
* @param 无
* @return 无
*/
int Reprint()
{
	int nWaterNum;
	int nRet;
	STWATER stWater;
	int nSelect;
	while(1)
	{
		if (YES == GetVarReprintSettle())
		{
			ASSERT_QUIT(ProSelectList("1.重打最后一笔||2.重打任意一笔||3.打印交易明细||4.打印交易汇总||5.重打印结算单", "打印", 0xFF, &nSelect));
		}
		else
		{
			ASSERT_QUIT(ProSelectList("1.重打最后一笔||2.重打任意一笔||3.打印交易明细||4.打印交易汇总", "打印", 0xFF, &nSelect));		
		}
		switch (nSelect)
		{
		case 1:
			GetWaterNum(&nWaterNum);
			if (nWaterNum <=0 )
			{
				char szDispBuf[100];
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C无交易");
				PubMsgDlg("重打最后一笔", szDispBuf, 0, 5);
				break;
			}
			FetchLastWater(&stWater);
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintWater(&stWater, REPRINT);
			break;			
		case 2:
			GetWaterNum(&nWaterNum);
			if (nWaterNum <=0 )
			{
				char szDispBuf[100];
				PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szDispBuf, "|C无交易");
				PubMsgDlg("重打任意一笔", szDispBuf, 0, 5);
				break;
			}
			FindByInvoice(1);
			break;
		case 3:   
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintAllWater();
			break;		
		case 4:
			nRet = PubConfirmDlg("打印交易汇总", "按确认键打印", 0, 5);
			if (nRet != APP_SUCC)
			{
				break;
			}
			PubClearAll();
			DISP_PRINTING_NOW;
			PrintTotal();
			break;
		case 5:
			if (YES == GetVarReprintSettle())
			{
				nRet = PubConfirmDlg("打印结算单", "按确认键打印", 0, 5);
				if (nRet != APP_SUCC)
				{
					break;
				}
				PubClearAll();
				DISP_PRINTING_NOW;
				if (APP_SUCC != PrintSettle(REPRINT))
				{
					PubMsgDlg(NULL, "  无旧结算数据", 0, 3);
				}
			}
			break;
		default:
			break;
		}
	}
	return APP_SUCC;
}

#if 0
static int Print_GetTransType(STWATER *pstWater, char* pcPrintType)
{
	switch (pstWater->cTransType)
	{
	case TRANS_SALE:
		if(pstWater->cTransAttr == ATTR_qPBOC || pstWater->cTransAttr == ATTR_PBOC_EC)
		{
			*pcPrintType = 'E';
		}
		else
			*pcPrintType = 'S';
		break;
	case TRANS_PHONE_SALE:
		*pcPrintType= 'S';
		break;
	case TRANS_REFUND:
	case TRANS_ALLIANCE_REFUND:
	case TRANS_REFUND_PHONE_SALE:
	case TRANS_EMV_REFUND:
		*pcPrintType = 'R';
		break;
	case TRANS_AUTHSALE:
	case TRANS_PHONE_AUTHSALE:
		*pcPrintType = 'P';
		break;
	case TRANS_AUTHSALEOFF:
	case TRANS_PHONE_AUTHSALEOFF:
		*pcPrintType = 'C';
		break;
	case TRANS_ADJUST:
		*pcPrintType = 'L';
		if (memcmp(pstWater->sFuncCode, "34", 2) == 0)/** 如果是追加小费，则按小费打印*/
		{
			*pcPrintType = 'S';
		}
		break;
	case TRANS_OFFLINE:
		*pcPrintType = 'L';
		break;
	case TRANS_INSTALMENT:
		*pcPrintType = 'T';
		break;
	case TRANS_BONUS_IIS_SALE:
	case TRANS_BONUS_ALLIANCE:
		*pcPrintType = 'B';
		break;
	case TRANS_VOID_AUTHSALE:
		*pcPrintType = 'A';
		break;			
	case TRANS_VOID_SALE:
		*pcPrintType = 'V';
		break;
	case TRANS_COUPON_VERIFY:
	case TRANS_TEL_VERIFY:
	case TRANS_CARD_VERIFY:
		*pcPrintType = 'D';
		break;
	case TRANS_REFUND_VERIFY:
		*pcPrintType = 'R';
		break;
	default:
		*pcPrintType = ' ';
		return APP_FAIL;
	}
	return APP_SUCC;
}
#endif
static void DealPrintAmount(char cTransType, char *pszAmount)
{		
	char szAmt[20] = {0};
	
	if(GetVarIsPrintPrintMinus()==YES)
	{
		switch(cTransType)
		{
		case TRANS_VOID_SALE:				/**< 消费撤销*/
		case TRANS_VOID_AUTHSALE: 			/**< 完成撤销*/
		case TRANS_VOID_AUTHSETTLE: 		/**< 结算撤销*/
		case TRANS_VOID_PREAUTH:			/**< 授权撤销*/
		case TRANS_VOID_REFUND:				/**< 撤销退货*/
		case TRANS_VOID_INSTALMENT: 		/**< 撤销分期*/
		case TRANS_VOID_BONUS_IIS_SALE: 	/**< 撤销发卡行积分消费*/
		case TRANS_VOID_BONUS_ALLIANCE: 	/**< 撤销联盟积分消费*/
		case TRANS_VOID_PHONE_SALE: 		/**< 撤销手机芯片消费*/
		case TRANS_VOID_PHONE_PREAUTH:		/**< 撤销手机芯片预授权*/
		case TRANS_VOID_PHONE_AUTHSALE: 	/**< 撤销手机完成请求*/
		case TRANS_EMV_REFUND:				/**< EMV脱机退货*/
		case TRANS_REFUND:  				/**< 退货*/
		case TRANS_ALLIANCE_REFUND:			/**< 联盟积分退货*/
		case TRANS_REFUND_PHONE_SALE:		/**< 手机芯片退货*/
		case TRANS_VOID_COUPON_VERIFY:		/**<撤销串码验券*/
		case TRANS_VOID_TEL_VERIFY:			/**<撤销手机验券*/
		case TRANS_VOID_CARD_VERIFY: 		/**<撤销银行卡验券*/
		case TRANS_REFUND_VERIFY:			/**<验证退货*/
			sprintf(szAmt, "-%s", pszAmount);
			strcpy(pszAmount, szAmt);
			break;
		default:
			break;
		}
	}
}

#if 0
/*功能和GetBankName类似，用在获取发卡行中文名称。
不同的是当找不到发卡行信息的时候，如果有配置未知
发卡行信息，则取该值当做发卡行名字*/
static void GetCardBankName(const char *pszBankCode,char *pszBankName, char cFlag)
{
	int i=0;
	char szBankCode[11+1] = {0};	/**< 商行编码*/
	char szLocalCode[11+1] = {0};	/**< 本地代码*/
	char szUnknowBankInfo[20+1]={0};
	
	GetVarBusinessBankId(szBankCode);	 /**<获取商行代码，定长4位*/
	GetVarLocalcode(szLocalCode);			/**<获取本地区区码，定长4位*/
	while(1)
	{
		if(!memcmp(pszBankCode, gszBankName[i], 4))
		{
			break;
		}
		i++;
		if(!memcmp(gszBankName[i], "0000", 4))
		{
			if (memcmp(pszBankCode, szBankCode, 4) == 0)
			{
				memcpy(pszBankName,"商业银行",8);
			}
			else
			{
				GetVarUnknowBankInfo(szUnknowBankInfo);
				if(strlen(szUnknowBankInfo)>0)
				{
					memcpy(pszBankName,szUnknowBankInfo, 11);
				}
				else
				{
					memcpy(pszBankName,pszBankCode, 11);
				}
			}
			return;
		}
	}
	if (memcmp(pszBankCode, szBankCode, 4) == 0)
	{
		memcpy(pszBankName,"商业银行",8);
		return;
	}
	
	// 检查发卡行代码与本地代码的符合条件，条件参见银联商务补充规范
	if (cFlag)
	{
		if (memcmp(szLocalCode,"0000",4) == 0)
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		else if ((i<15)&&(memcmp(pszBankCode+4,szLocalCode,2) != 0))
		{
			memcpy(pszBankName,gszBankName[i]+4,4);
			memcpy(pszBankName+4,"  银联",6);
		}
		else
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		return ;
	}

	memcpy(pszBankName, gszBankName[i]+4+4, 10);
	return ;
}
#endif

/**
* @brief 根据提供的银行代码，取得银行对应的中文名称
* @param in char *pszBankCode 银行代码
* @param out char *pszBankName 银行中文名称
* @param in char cFlag 	符合条件标志
* @return 无
*/
void GetBankName(const char *pszBankCode,char *pszBankName, char cFlag)
{
	int i=0;
	char szBankCode[11+1] = {0};	/**< 商行编码*/
	char szLocalCode[11+1] = {0};	/**< 本地代码*/

	GetVarBusinessBankId(szBankCode);	 /**<获取商行代码，定长4位*/
	GetVarLocalcode(szLocalCode);			/**<获取本地区区码，定长4位*/
	while(1)
	{
		if(!memcmp(pszBankCode, gszBankName[i], 4))
		{
			break;
		}
		i++;
		if(!memcmp(gszBankName[i], "0000", 4))
		{
			if (memcmp(pszBankCode, szBankCode, 4) == 0)
			{
				memcpy(pszBankName,"商业银行",8);
			}
			else
			{
				memcpy(pszBankName, pszBankCode, 11);	
			}
			return;
		}
	}
	if (memcmp(pszBankCode, szBankCode, 4) == 0)
	{
		memcpy(pszBankName,"商业银行",8);
		return;
	}
	
	// 检查发卡行代码与本地代码的符合条件，条件参见银联商务补充规范
	if (cFlag)
	{
		if (memcmp(szLocalCode,"0000",4) == 0)
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		else if ((i<15)&&(memcmp(pszBankCode+4,szLocalCode,2) != 0))
		{
			memcpy(pszBankName,gszBankName[i]+4,4);
			memcpy(pszBankName+4,"  银联",6);
		}
		else
		{
			memcpy(pszBankName,gszBankName[i]+4+4,10);
		}
		return ;
	}

	memcpy(pszBankName, gszBankName[i]+4+4, 10);
	return ;
}


/**
* @brief 处理打印限制(电源)
* @param 无
* @return
* @li APP_SUCC 
* @li APP_FAIL
* @li APP_QUIT
* @author 
* @date
*/
int DealPrintLimit(void)
{
	ST_POWER_INFO stPowerInfo;
	char szVer[64]={0};

	NDK_Getlibver(szVer);
	
	if ((strcmp(szVer, "4.0.0") >= 0) && szVer[0] != 'V')
	{
		memset(&stPowerInfo, 0, sizeof(ST_POWER_INFO));
		NDK_SysGetPowerInfo(&stPowerInfo);
		
		if (stPowerInfo.unPowerType & (1<<0))
			return APP_SUCC;
		if ((stPowerInfo.unPowerType & (1<<1)) && stPowerInfo.unBattryPercent <= 10)
		{
			PubMsgDlg("温馨提示", "电量不足,请充电!", 3, 10);
			return APP_QUIT;
		}
	}
	
	return APP_SUCC;
}



/**
*	传统交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntTraditionOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=11;		/**<为可开关的交易数*/
	char szName[11][17]={"ISSALE","ISVOIDSALE","ISREFUND","ISBALANCE","ISPREAUTH",
		"ISVOIDPREAUTH","ISAUTHSALE","ISAUTHSALEOFF",
		"ISVOIDAUTHSALE","ISOFFLINE","ISREJUST"};	

				
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);

	}
}


/**
*	电子现金交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntECashOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=7;		/**<为可开关的交易数*/
	char szName[7][17]={"ISECSALE","ISECFASTSALE",
		"ISECLOAD","NOTBINDECLOAD","ISECLOADCASH",
		"ISECVOIDLOADCASH","ISECOFFREFUND"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue(szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持",6 );	
	}
}


/**
*	电子钱包交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/

void PntWalletOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=4;		/**<为可开关的交易数*/
	char szName[4][17]={"ISEPSALE","ISEPLOAD",
		"ISEPNOTBINDLOAD","ISEPCASHLOAD"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);

	}
}


/**
*	分期付款交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/

void PntInstallmentOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<为可开关的交易数*/
	char szName[2][17]={"ISINSTALL","ISVOIDINSTALL"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}

/**
*	积分交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntBonusOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=6;		/**<为可开关的交易数*/
	char szName[6][17]={"BONUS_ALLIANCE","BONUS_IIS_SALE","VOIDBONUS_ALL",
		"VOID_BONUS_IIS","ALLIANCE_BALANCE","ALLIANCE_REFUND"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}


/**
*	手机芯片交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntPhoneChipSaleOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=9;		/**<为可开关的交易数*/
	char szName[9][17]={"ISPHONESALE","ISVOIDPHONESALE","ISREFUNDPHONE",
		"ISPHONEPREAUTH","ISPHONEVOIDPRE","ISPHONEAUTH",
		"ISPHONEAUTHOFF","ISPHONEVOIDAUTH","ISPHONEBALANCE"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}


/**
*	预约交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntAppointmentOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<为可开关的交易数*/
	char szName[2][17]={"ISAPPOINT","ISVOIDAPPOINT"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}


/**
*	订购交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntOrderOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=8;		/**<为可开关的交易数*/
	char szName[8][17]={"ISORDERSALE","ISORDERVOIDSALE","ISREFUNDORDER",
		"ISORDERPREAUTH","ISORDERVOIDPRE","ISORDERAUTHSALE",
		"ISORDERAUTHOFF","ISORDERVOIDAUTH"};
	
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue( szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}

/**
* @其他交易开关配置打印
* @param TransFlag 支持的交易标志串
* @return 无
*/
void PntOtherOnoff(const char *szTransFlag)
{
	int i;
	int nTransNum=2;		/**<为可开关的交易数*/
	char szName[2][17]={"ISSTRIPECASHLOAD","ISSTRIPELOAD"};
	for(i=0;i<nTransNum;i++)
	{	
		/**<如果名称为空串，不打印*/
		if (szName[i][0]==0)
			continue;
		PubSetFieldValue(szName[i], szTransFlag[i/8] & (0x80>>(i%8)) ? "支持" : "不支持" ,6);
	}
}

#if 0
int SetPrintData_Water(const STWATER *pstWater, int nReprintFlag, const int nCurrentPage)
{
    int	nPage=3;				/**< 打印的单据总数*/
	int nMaxLine = 14;
	int nLen = 0;
	uint unLogoXpos = 0;
	char szShopname[40+1] = {0};
	char szPrintName[100+1] = {0};
	char szShopid[15+1] = {0};
	char szTerminalid[8+1] = {0};
	char szTmpStr[80];
	char szStr[30+1];
	char szPosYear[4+1] = {0};			/**< 交易年份*/
	char szTransAttrAndStatus[2];
	char szStripeType[2];
	char szDispAmt[14+1] = {0};
	char szTmpRefNum[12+1] = {0};
	char szHotLine[20+1] = {0};
	char szSoftVer[16+1] = {0};
	char cSTUB = 0;	// 1商户、2银行、3持卡人存根标志

	szTransAttrAndStatus[0] = pstWater->cTransAttr;
	szTransAttrAndStatus[1] = pstWater->cEMV_Status;

	PubGetCurrentDatetime(szTmpStr);
	memcpy(szPosYear, szTmpStr, 4);
	
	PubClearFieldValue();
	GetVarTerminalId(szTerminalid);
	GetVarMerchantId(szShopid);
	GetVarMerchantNameCn(szShopname);
	GetFunctionPntTitle(szPrintName);
	GetFunctionHotLineNo(szHotLine);		/**< 获得热线号码*/
	GetVarDispSoftVer(szSoftVer);
	GetVarPrintPageCount(&nPage);	/**< 获得打印的总联数*/

	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	if(GetIsPntTitleMode()==YES)			/**< 打印LOGO*/
	{		
		GetPrnLogo(szTmpStr, &unLogoXpos);
		PubSetParamLogo("LOGOTAG", szTmpStr, unLogoXpos);
	}
	else
	{
		Cup_SetPrnTitle(szPrintName, szPrintName);
		PubSetFieldValue("PRINTNAME", szPrintName, strlen(szPrintName));
	}

	/*增值业务签购单*/
	
	PubSetFieldValue("PRINTMITENONAME", "增值业务签购单", strlen("增值业务签购单"));
	
	if ((nReprintFlag == REVERSAL_PRINT) || (nReprintFlag == OFFLINE_PRINT))
	{
		//是否打印 故障报告单
		PubSetFieldValue("CISPRINTFAIL", "1", 1);
	}

	switch(nCurrentPage)
	{
	case 2:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//银行存根
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	case 1:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//  持卡人存根
			PubSetFieldValue("STUB", "3", 1);
			cSTUB = 3;
		}
		break;
	default:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//银行存根
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	}

	//商户名称
	PubAllTrim(szShopname);

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nMaxLine = 20;
	}
	if(strlen(szShopname) <= nMaxLine)
	{
		//针打印制单子用
		PubSetFieldValue("ISUNBLANK", "1", 1);
	}
	PubSetFieldValue("MERCHANTNAME",  szShopname, strlen(szShopname));
	// 商户号
	PubSetFieldValue("SHOPID",  szShopid, strlen(szShopid));
	// POS编号
	PubSetFieldValue("TERMINALID", szTerminalid, strlen(szTerminalid));
	//操作员编号
	PubSetFieldValue("OPERID", pstWater->szOper, strlen(pstWater->szOper));


	

	// 卡号
	memset(szTmpStr, 0, sizeof(szTmpStr));
	PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szTmpStr);
	CtrlCardPan((uchar*)szTmpStr, pstWater->nPanLen, pstWater->cTransType, pstWater->cEMV_Status);
	GetStripeType(pstWater->szInputMode, pstWater->cTransType, pstWater->cTransAttr,szStripeType);
	PubSetFieldValueFormat("CARDNO", "%s /%s", szTmpStr, szStripeType);	

	
	PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));

	// 交易类型
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));

	GetTransNameForTP( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_CN", "%s", szStr);
	PubSetFieldValueFormat("TRANSTYPE_EN", "%s(%s)", szStr, szTmpStr);

	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	GetTransNameForTP2( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_MITENO_CN", "%s", szStr);
	PubSetFieldValueFormat("TRANSTYPE_MITENO_EN", "%s(%s)", szStr, szTmpStr);

	
	// 有效期
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2))
	{	
		PubSetFieldValueFormat( "EXPDATE", "%2.2s%02x/%02x", szPosYear, pstWater->sExpDate[0], pstWater->sExpDate[1] );
	}
	
	//批次号
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0,  (uchar *)szTmpStr);
	PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));
	//凭证号
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0,  (uchar *)szTmpStr+10);	
	PubSetFieldValue("TRACENO", szTmpStr + 10, strlen(szTmpStr + 10));

	// 授权码
	if (memcmp(pstWater->szAuthCode, "\x00\x00\x00\x00\x00\x00", 6))
	{
		PubSetFieldValue("AUTHCODE", pstWater->szAuthCode, strlen(pstWater->szAuthCode));
	}
	if (0 == strlen(pstWater->szRefnum))
	{
		memset(szTmpRefNum, 0, sizeof(szTmpRefNum));
		memset(szTmpRefNum, ' ', 12);
	}
	else
	{
		memcpy(szTmpRefNum, pstWater->szRefnum, 12);
	}
	//系统参考号
	PubSetFieldValue("REFNUM", szTmpRefNum, 12);

	// 交易日期、时间
	PubSetFieldValueFormat( "DATETIME", "%4.4s/%02x/%02x %02x:%02x:%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1], pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	PubSetFieldValueFormat( "DATE", "%4.4s/%02x/%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1] );
	PubSetFieldValueFormat( "TIME",  "%02x:%02x:%02x", pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	// 打印交易金额
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
	memset(szDispAmt, 0, sizeof(szDispAmt));
	ProAmtToDispOrPnt(szStr, szDispAmt);
	PubAllTrim(szDispAmt);
	DealPrintAmount(pstWater->cTransType, szDispAmt);
	PubSetFieldValueFormat( "TOTALAMOUNT", "RMB %s", szDispAmt);	

	memset(szDispAmt, 0, sizeof(szDispAmt));
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
	PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

	ProAmtToDispOrPnt(szTmpStr, szDispAmt);
	PubAllTrim(szDispAmt);
	PubSetFieldValueFormat( "REALAMOUNT", "RMB %s", szDispAmt);	

	/*银行卡消费金额*/
	memset(szDispAmt, 0, sizeof(szDispAmt));
	sprintf(szDispAmt, "%012d", atol(szStr)-atol(szTmpStr));
	ProAmtToDispOrPnt(szDispAmt, szDispAmt);
	PubAllTrim(szDispAmt);
	PubSetFieldValueFormat("CARDAMOUNT", "RMB %s", szDispAmt);	

	//  重打标志
	if (nReprintFlag==REPRINT)
	{
		//重打印凭证
		PubSetFieldValue("ISREPRINT", "1", 1);
	}
	if(nReprintFlag==REVERSAL_PRINT)
	{
		//冲正不成功,请人工处理
		PubSetFieldValue("ISREVERSALFAIL", "1", 1);
	}
	
	if(nReprintFlag==OFFLINE_PRINT)
	{
		//"上送不成功,请人工处理
		PubSetFieldValue("ISSENDFAIL", "1", 1);
	}

	/*
		银商要求，设置打印3联签购单时，只在商户联签购单及银行联签购单中
		保留持卡人签名空间，持卡人联签购单不保留持卡人签名空间。
	*/
	if (cSTUB != 3)
	{
		//持卡人签名
		//本人确认以上交易，同意将其记入本卡账户
		PubSetFieldValue("ISCARDSIGN", "1", 1); 
		PubSetFieldValue("ISFEEDLINE", "1", 1); //打印空白换行
	}	

	if(strlen(szHotLine) > 0)
	{
		PubSetFieldValueFormat( "HOTLINE",  "%s", szHotLine);
	}
	memset(szTmpStr,0,sizeof(szTmpStr));
	sprintf(szTmpStr, "%s-%-8.8s", PubGetPosTypeStr(), szSoftVer);
	PubSetFieldValueFormat( "POSTYPEANDVER",  "%s", szTmpStr);
	return APP_SUCC;
}

#endif

int SetPrintData_Water(const STWATER *pstWater, int nReprintFlag, const int nCurrentPage)
{
    int	nPage=3;				/**< 打印的单据总数*/
	int nMaxLine = 14;
	uint unLogoXpos = 0;
	char szShopname[40+1] = {0};
	char szPrintName[100+1] = {0};
	char szShopid[15+1] = {0};
	char szTerminalid[8+1] = {0};
	char szTmpStr[80];
	char szStr[30+1],szResultAmt[32];
	char szPosYear[4+1] = {0};			/**< 交易年份*/
	char szTransAttrAndStatus[2];
	char szStripeType[2];
	char szDispAmt[14+1] = {0};
	char szTmpRefNum[12+1] = {0};
	char szHotLine[20+1] = {0};
	char szSoftVer[16+1] = {0};
	char cSTUB = 0;	// 1商户、2银行、3持卡人存根标志

	szTransAttrAndStatus[0] = pstWater->cTransAttr;
	szTransAttrAndStatus[1] = pstWater->cEMV_Status;

	PubGetCurrentDatetime(szTmpStr);
	memcpy(szPosYear, szTmpStr, 4);
	
	PubClearFieldValue();
	GetVarTerminalId(szTerminalid);
	GetVarMerchantId(szShopid);
	GetVarMerchantNameCn(szShopname);
	GetFunctionPntTitle(szPrintName);
	GetFunctionHotLineNo(szHotLine);		/**< 获得热线号码*/
	GetVarDispSoftVer(szSoftVer);
	GetVarPrintPageCount(&nPage);	/**< 获得打印的总联数*/

	memset(szTmpStr, 0, sizeof(szTmpStr));
	
	if(GetIsPntTitleMode()==YES)			/**< 打印LOGO*/
	{		
		GetPrnLogo(szTmpStr, &unLogoXpos);
		PubSetParamLogo("LOGOTAG", szTmpStr, unLogoXpos);
	}
	else
	{
		Cup_SetPrnTitle(szPrintName, szPrintName);
		PubSetFieldValue("PRINTNAME", szPrintName, strlen(szPrintName));
	}

	/*增值业务签购单*/
	
	PubSetFieldValue("PRINTMITENONAME", "增值业务签购单", strlen("增值业务签购单"));
	
	if ((nReprintFlag == REVERSAL_PRINT) || (nReprintFlag == OFFLINE_PRINT))
	{
		//是否打印 故障报告单
		PubSetFieldValue("CISPRINTFAIL", "1", 1);
	}

	switch(nCurrentPage)
	{
	case 2:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//银行存根
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	case 1:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//  持卡人存根
			PubSetFieldValue("STUB", "3", 1);
			cSTUB = 3;
		}
		break;
	default:
		if (nCurrentPage == nPage)
		{
			// 商户存根
			PubSetFieldValue("STUB", "1", 1);
			cSTUB = 1;
		}
		else
		{
			//银行存根
			PubSetFieldValue("STUB", "2", 1);
			cSTUB = 2;
		}
		break;
	}

	//商户名称
	PubAllTrim(szShopname);

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nMaxLine = 20;
	}
	if(strlen(szShopname) <= nMaxLine)
	{
		//针打印制单子用
		PubSetFieldValue("ISUNBLANK", "1", 1);
	}
	PubSetFieldValue("MERCHANTNAME",  szShopname, strlen(szShopname));
	// 商户号
	PubSetFieldValue("SHOPID",  szShopid, strlen(szShopid));
	// POS编号
	PubSetFieldValue("TERMINALID", szTerminalid, strlen(szTerminalid));
	//操作员编号
	PubSetFieldValue("OPERID", pstWater->szOper, strlen(pstWater->szOper));

	switch(pstWater->cTransType)
	{
		case TRANS_COUPON_VERIFY:		
			PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));
			break;
		case TRANS_TEL_VERIFY:
			PubSetFieldValue("TELNONO", pstWater->sTelNo, strlen(pstWater->sTelNo));
			break;
		case TRANS_CARD_VERIFY:
			memset(szTmpStr, 0, sizeof(szTmpStr));
			PubHexToAsc((uchar *)pstWater->sPan, pstWater->nPanLen, 0, (uchar *)szTmpStr);
			CtrlCardPan((uchar*)szTmpStr, pstWater->nPanLen, pstWater->cTransType, pstWater->cEMV_Status);
			GetStripeType(pstWater->szInputMode, pstWater->cTransType, pstWater->cTransAttr,szStripeType);
			PubSetFieldValueFormat("CARDNO", "%s /%s", szTmpStr, szStripeType); 
			break;
		case TRANS_COUPON_MEITUAN:		
			PubSetFieldValue("COUPONNO", pstWater->sCouponID, strlen(pstWater->sCouponID));
			break;
		case TRANS_VOID_VERIFY:
			break;
		case TRANS_REFUND_VERIFY:
			break;
	}
 	// 交易类型
	memset(szStr, 0, sizeof(szStr));
	memset(szTmpStr, 0, sizeof(szTmpStr));

	GetTransNameForTP( pstWater, szStr, szTmpStr );
	PubSetFieldValueFormat("TRANSTYPE_CN", "%s", szStr);		
	PubSetFieldValueFormat("TRANSTYPE_EN", "%s(%s)", szStr, szTmpStr);

	//GetTransNameForTP2( pstWater->cTransType, szTransAttrAndStatus, szStr, szTmpStr );
	//PubSetFieldValueFormat("TRANSTYPE_MITENO_CN", "%s", szStr);
 	
	// 有效期
	if (memcmp(pstWater->sExpDate, "\x00\x00", 2))
	{	
		PubSetFieldValueFormat( "EXPDATE", "%2.2s%02x/%02x", szPosYear, pstWater->sExpDate[0], pstWater->sExpDate[1] );
	}
	
	//批次号
	PubHexToAsc((uchar *)pstWater->szBatchNum, 6, 0,  (uchar *)szTmpStr);
	PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));
	//凭证号
	PubHexToAsc((uchar *)pstWater->sTrace, 6, 0,  (uchar *)szTmpStr+10);	
	PubSetFieldValue("TRACENO", szTmpStr + 10, strlen(szTmpStr + 10));

	// 授权码
	if (memcmp(pstWater->szAuthCode, "\x00\x00\x00\x00\x00\x00", 6))
	{
		PubSetFieldValue("AUTHCODE", pstWater->szAuthCode, strlen(pstWater->szAuthCode));
	}
	if (0 == strlen(pstWater->szRefnum))
	{
		memset(szTmpRefNum, 0, sizeof(szTmpRefNum));
		memset(szTmpRefNum, ' ', 12);
	}
	else
	{
		memcpy(szTmpRefNum, pstWater->szRefnum, 12);
	}
	//系统参考号
	PubSetFieldValue("REFNUM", szTmpRefNum, 12);

	// 交易日期、时间
	PubSetFieldValueFormat( "DATETIME", "%02x%02x/%02x/%02x %02x:%02x:%02x", pstWater->sDate[0], pstWater->sDate[1], pstWater->sDate[2], pstWater->sDate[3],pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);
	PubSetFieldValueFormat( "DATE", "%4.4s/%02x/%02x", szPosYear, pstWater->sDate[0], pstWater->sDate[1] );
	PubSetFieldValueFormat( "TIME",  "%02x:%02x:%02x", pstWater->sTime[0], pstWater->sTime[1], pstWater->sTime[2]);

	// 打印交易金额
	if (pstWater->cTransType == TRANS_COUPON_DAZHONG)
	{
		memset(szStr, 0, sizeof(szStr));
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);

		PubSetFieldValueFormat( "COUPONCOUNT", "%d张", atoi(szStr));
	}

	if (pstWater->cTransType== TRANS_COUPON_VERIFY/**<标准EMV流程交易*/
			|| pstWater->cTransType == TRANS_TEL_VERIFY
			|| pstWater->cTransType == TRANS_CARD_VERIFY)
	{
		memset(szDispAmt, 0, sizeof(szDispAmt));
		memset(szStr, 0, sizeof(szStr));
		memset(szTmpStr, 0, sizeof(szTmpStr));
		
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
		PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

		ProAmtToDispOrPnt(szTmpStr, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "REALAMOUNT", "RMB %s", szDispAmt);	

		memset(szDispAmt, 0, sizeof(szDispAmt));
		memset(szStr, 0, sizeof(szStr));
		memset(szTmpStr, 0, sizeof(szTmpStr));
		
		PubHexToAsc((uchar *)pstWater->sPayableAmount, 12, 0, (uchar *)szTmpStr);

		ProAmtToDispOrPnt(szTmpStr, szDispAmt);
		PubAllTrim(szDispAmt);
		
		PubSetFieldValueFormat("CARDAMOUNT", "RMB %s", szDispAmt);	
		/*应付金额*/
		PubSetFieldValueFormat("PAYABLEAMOUNT", "RMB %s", szDispAmt);	

	}
	else
	{
		//总金额
		memset(szStr, 0, sizeof(szStr));
		PubHexToAsc((uchar *)pstWater->szAmount, 12, 0, (uchar *)szStr);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szStr, szDispAmt);
		PubAllTrim(szDispAmt);
		DealPrintAmount(pstWater->cTransType, szDispAmt);
		PubSetFieldValueFormat( "TOTALAMOUNT", "RMB %s", szDispAmt);

		//实付金额
		memset(szTmpStr, 0, sizeof(szTmpStr));
		PubHexToAsc((uchar *)pstWater->szRealAmount, 12, 0, (uchar *)szTmpStr);

		if (atoi(szTmpStr)>0)
		{
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt(szTmpStr, szDispAmt);
			PubAllTrim(szDispAmt);
			PubSetFieldValueFormat( "CUSTOMERAMT", "RMB %s", szDispAmt);	

			//优惠金额
			memset(szResultAmt, 0, sizeof(szResultAmt));
			PubAscSubAsc((uchar *)szStr, (uchar *)szTmpStr, (uchar *)szResultAmt);

			if (atoi(szResultAmt)>0)
			{
				memset(szDispAmt, 0, sizeof(szDispAmt));
				ProAmtToDispOrPnt(szResultAmt, szDispAmt);
				PubAllTrim(szDispAmt);
				PubSetFieldValueFormat( "DISCOUNTAMT", "RMB %s", szDispAmt);
			}
		}


	}

	//  重打标志
	if (nReprintFlag==REPRINT)
	{
		//重打印凭证
		PubSetFieldValue("ISREPRINT", "1", 1);
	}
	if(nReprintFlag==REVERSAL_PRINT)
	{
		//冲正不成功,请人工处理
		PubSetFieldValue("ISREVERSALFAIL", "1", 1);
	}
	
	if(nReprintFlag==OFFLINE_PRINT)
	{
		//"上送不成功,请人工处理
		PubSetFieldValue("ISSENDFAIL", "1", 1);
	}

	if(strlen(pstWater->szOutTradeNo) > 0)
	{
		PubSetFieldValueFormat("OUTTRADENO", "%s", pstWater->szOutTradeNo);	
		printYiwei(pstWater->szOutTradeNo);
		
	}
	
	if (pstWater->cTransType== TRANS_COUPON_VERIFY/**<标准EMV流程交易*/
			|| pstWater->cTransType == TRANS_TEL_VERIFY
			|| pstWater->cTransType == TRANS_CARD_VERIFY)
	{
		STCOUPON *pstAddition = (STCOUPON *)pstWater->sAddition;

		#if 0
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetCouponIss(szTmpStr, pstAddition->ucCouponISS);
		GetCouponType(szTmpStr+strlen(szTmpStr), NULL, pstAddition->ucCouponType);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
		memset(szTmpStr, 0, sizeof(szTmpStr));
		PubHexToAsc(pstAddition->usAmount,10, 0,szTmpStr);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		sprintf(szDispAmt, "%012d", atol(szTmpStr));
		ProAmtToDispOrPnt(szDispAmt, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat("COUPONAMOUNT", "RMB %s", szDispAmt);	
		#endif
		
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memcpy(szTmpStr, pstAddition->usCouponName, 20);
		PubAllTrim(szTmpStr);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
		//printYiwei(szTmpStr);
		
	}
	else if(pstWater->cTransType== TRANS_COUPON_MEITUAN)
	{
		STCOUPONMEITUAN *pstAddition = (STCOUPONMEITUAN *)pstWater->sAddition;
		
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memcpy(szTmpStr, pstAddition->usCouponName, 20);
		PubAllTrim(szTmpStr);
		PubSetFieldValueFormat( "COUPONINFO",	"%s", szTmpStr);
	}
	
	if (strlen(pstWater->szAdvertisement) > 0)
	{
		PubSetFieldValueFormat("ADINFO", "%s", pstWater->szAdvertisement);
	}

	/*
		银商要求，设置打印3联签购单时，只在商户联签购单及银行联签购单中
		保留持卡人签名空间，持卡人联签购单不保留持卡人签名空间。
	*/
	if (cSTUB != 3)
	{
		//持卡人签名
		//本人确认以上交易，同意将其记入本卡账户
		PubSetFieldValue("ISCARDSIGN", "1", 1); 
		PubSetFieldValue("ISFEEDLINE", "1", 1); //打印空白换行
	}	

	if(strlen(szHotLine) > 0)
	{
		PubSetFieldValueFormat( "HOTLINE",  "%s", szHotLine);
	}
	memset(szTmpStr,0,sizeof(szTmpStr));
	sprintf(szTmpStr, "%s-%-8.8s", PubGetPosTypeStr(), szSoftVer);
	PubSetFieldValueFormat( "POSTYPEANDVER",  "%s", szTmpStr);
	return APP_SUCC;
}


int SetAndPrint_Settle(void *ptrPara)
{
	struct {
		char OperNo[OPERCODELEN+1];	/**<结算操作员编号*/
		char BatchNo[6+1];			/**<结算批次号*/
		char BatchDatetime[14+1];	/**<结算日期时间*/
		char SettleFlag[2];			/**<结算标识，'0'对帐平,'1'对帐不平, SettleFlag[0]内卡、SettleFlag[1]外卡*/
		STSETTLE stSettle[2];			/**<stSettle[0] 存储内卡结算信息,stSettle[1] 存储外卡结算信息*/
	} ST_SettleRec;
	STRECFILE stSettleFile;
	char szTmpstr[41];
	int i;
	char szTmpTag[MAX_PRINT_TAG_NAME + 1] = {0};
	int *pnReprintFlag = (int *)ptrPara;
	char *pszModuleName = "PRINT_SETTLE";
	uchar szAmt[13] = {0};
	uchar szAmt1[13] = {0};
	uchar szAmt2[13] = {0};
	char szDispAmt[14+1] = {0};
	uchar szAmtSum[13];
	uchar szAmtSumTemp[14+1];
	ulong _NumSum;

	PubClearFieldValue();

	if (*pnReprintFlag==REPRINT)
	{
		if (YES == GetVarReprintSettle())
		{
			ASSERT_FAIL(PubReadOneRec(SETTLEFILENAME, 1, (char *)(&ST_SettleRec)));
		}
	}
	else
	{
		GetCurrentOper(ST_SettleRec.OperNo, NULL, NULL);	/**< 结算操作员号*/
		GetVarBatchNo(ST_SettleRec.BatchNo);				/**< 结算批次号*/
		
		/**< 结算日期*/
		/**< 结算时间*/
		PubGetCurrentDatetime(ST_SettleRec.BatchDatetime);
		GetVarSettleDateTime(szTmpstr);
		if (memcmp(szTmpstr, "\x00\x00\x00\x00\x00", 5) == 0)
		{
			;
		}
		else
		{
			PubHexToAsc((uchar *)szTmpstr, 10, 0, (uchar *)(ST_SettleRec.BatchDatetime+4));
		}
		/**< 内卡结算标识*/
		ST_SettleRec.SettleFlag[0] = GetVarCnCardFlag();
		/**< 外卡结算标识*/		
		ST_SettleRec.SettleFlag[1] = GetVarEnCardFlag();
		GetSettleDataNK(&(ST_SettleRec.stSettle[0])); 			/**< 内卡结算数据*/
		GetSettleDataWK(&(ST_SettleRec.stSettle[1]));			/**< 外卡结算数据*/
		if (YES == GetVarReprintSettle()) /**<支持重打结算单的时候需要保存的信息*/
		{
			// 创建结算数据文件，并保存当前结算数据
			strcpy( stSettleFile.szFileName, SETTLEFILENAME );
			stSettleFile.cIsIndex = FILE_NOCREATEINDEX;						
			stSettleFile.unMaxOneRecLen = sizeof(ST_SettleRec);
			stSettleFile.unIndex1Start =1;
			stSettleFile.unIndex1Len =  1;
			stSettleFile.unIndex2Start =  1;
			stSettleFile.unIndex2Len = 1;
			ASSERT(PubCreatRecFile(&stSettleFile));
			ASSERT(PubAddRec(SETTLEFILENAME, (char *)(&ST_SettleRec)));
		}
	}
 
	//商户名称
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarMerchantNameCn(szTmpstr);
	PubSetFieldValue("MERCHANTNAME",  szTmpstr, strlen(szTmpstr));
	// 商户号
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarMerchantId(szTmpstr);
	PubSetFieldValue("SHOPID",  szTmpstr, strlen(szTmpstr));
	
	// POS编号
	memset(szTmpstr, 0, sizeof(szTmpstr));
	GetVarTerminalId(szTmpstr);
	PubSetFieldValue("TERMINALID", szTmpstr, strlen(szTmpstr));
	//操作员编号
	PubSetFieldValue("OPERID", ST_SettleRec.OperNo, OPERCODELEN);

	// 批次号
	PubSetFieldValue("BATCHNO", ST_SettleRec.BatchNo, 6);

	// 结算时间和日期
	PubSetFieldValueFormat( "DATETIME", "%4.4s/%2.2s/%2.2s  %2.2s:%2.2s:%2.2s", 
		ST_SettleRec.BatchDatetime, ST_SettleRec.BatchDatetime+4, ST_SettleRec.BatchDatetime+6, 
		ST_SettleRec.BatchDatetime+8, ST_SettleRec.BatchDatetime+10, ST_SettleRec.BatchDatetime+12);

	//0内卡1外卡	
	for(i=0; i<1; i++)		/**< i=0 人民币卡, i=1 外卡*/
	{
		memcpy(szAmtSum,"000000000000", 12);
		szAmtSum[12] = 0;
		memcpy(szAmtSumTemp,"000000000000", 12);
		szAmtSumTemp[12] = 0;
		
		_NumSum = 0;
		sprintf(szTmpTag, "ISSETTLEOK%d", i);
		if (ST_SettleRec.SettleFlag[i] == '2')
		{
			//对账不平
			PubSetFieldValue(szTmpTag, "2", 1);
		}
		else if (ST_SettleRec.SettleFlag[i] == '1')
		{
			//对账平
			PubSetFieldValue(szTmpTag, "1", 1);
		}
		else
		{
			//对账错
			PubSetFieldValue(szTmpTag, "3", 1);
		}

		if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && ST_SettleRec.stSettle[i]._CouponVerifyNum > 0)
		{
			//串码验证
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CouponVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "COUPONNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._CouponVerifyNum, szDispAmt);
				
			memcpy(szAmtSum,szAmt,12 );
			_NumSum += ST_SettleRec.stSettle[i]._CouponVerifyNum;

			//串码承兑撤销
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidCouponVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCOUPONNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCouponVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCouponVerifyNum, szDispAmt);
			}
			
			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCouponVerifyNum;
		}

		if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && ST_SettleRec.stSettle[i]._TelVerifyNum > 0)
		{
			//手机号验证
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._TelVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "TELNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);			
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._TelVerifyNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum);		
			_NumSum += ST_SettleRec.stSettle[i]._TelVerifyNum;	

			//手机号承兑撤销
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidTelVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDTELNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidTelVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidTelVerifyNum, szDispAmt);
			}
			
			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidTelVerifyNum;
			
		}

		if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && ST_SettleRec.stSettle[i]._CardVerifyNum > 0)
		{
			//银行卡验证
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CardVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "CARDNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._CardVerifyNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._CardVerifyNum;	

			//银行卡承兑撤销
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VodiCardVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCARDNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCardVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidCardVerifyNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCardVerifyNum; 
			
		}
		
		if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && ST_SettleRec.stSettle[i]._RefundVerifyNum > 0)
		{
			//退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundVerifyAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundVerifyNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundVerifyNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundVerifyNum;  
		}

		if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
			&& (ST_SettleRec.stSettle[i]._PrecreateNum > 0 || ST_SettleRec.stSettle[i]._CreatepayNum > 0))
		{
			//微信支付
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._PrecreateAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._CreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "WECHATNUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._PrecreateNum + ST_SettleRec.stSettle[i]._CreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._PrecreateNum;	
			_NumSum += ST_SettleRec.stSettle[i]._CreatepayNum;
		
			//微信支付撤销
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidPrecreateAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDWECHATNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidPrecreateNum + ST_SettleRec.stSettle[i]._VoidCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidPrecreateNum + ST_SettleRec.stSettle[i]._VoidCreatepayNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidPrecreateNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidCreatepayNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && ST_SettleRec.stSettle[i]._RefundWechatNum > 0)
		{
			//微信退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundWeChatAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDWECHATNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundWechatNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundWechatNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundWechatNum;
		}


		if((GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES)
			&& (ST_SettleRec.stSettle[i]._BaiduPrecreteNum > 0 || ST_SettleRec.stSettle[i]._BaiduCreatepayNum > 0))
		{
			//百度支付
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "CREATEBAIDUNUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BaiduPrecreteNum + ST_SettleRec.stSettle[i]._BaiduCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._BaiduPrecreteNum;	
			_NumSum += ST_SettleRec.stSettle[i]._BaiduCreatepayNum;
		
			//百度支付撤销
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDCREATEBAIDUNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum + ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum + ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBaiduPrecreteNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBaiduCreatepayNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && ST_SettleRec.stSettle[i]._RefundBaiduNum > 0)
		{
			//百度退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._RefundBaiduAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDBAIDUNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundBaiduNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._RefundBaiduNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._RefundBaiduNum;
		}

		if((GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_JD_CREATEANDPAY)== YES) && ST_SettleRec.stSettle[i]._JDPrecreateNum > 0)
		{
			//京东支付
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._JDPrecreateAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "JDCREATENUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDPrecreateNum, szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._JDPrecreateNum;	

			//京东撤销
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidJDPrecreateAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDJDCREATENUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidJDPrecreateNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidJDPrecreateNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidJDPrecreateNum; 
		}
		
		
		if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && ST_SettleRec.stSettle[i]._JDRefundNum > 0)
		{
			//京东退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._JDRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "JDREFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._JDRefundNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._JDRefundNum;
		}


		if((GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES)
			&& (ST_SettleRec.stSettle[i]._AliPrecreteNum > 0 || ST_SettleRec.stSettle[i]._AliCreatepayNum > 0))
		{
			//支付宝支付
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliPrecreteAmount.sSettleAmt, 12, 0, szAmt2);
			memset(szAmt1, 0, sizeof(szAmt1));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliCreatepayAmount.sSettleAmt, 12, 0, szAmt1);

			memset(szAmt, 0, sizeof(szAmt));
			AmtAddAmt(szAmt1,szAmt2,szAmt); 

			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 
			
			sprintf(szTmpTag, "ALICREATEALINUM%d", i);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliPrecreteNum + ST_SettleRec.stSettle[i]._AliCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._AliPrecreteNum;	
			_NumSum += ST_SettleRec.stSettle[i]._AliCreatepayNum;
		
			//支付撤销
			memset(szAmt1, 0, sizeof(szAmt1));
			memset(szAmt2, 0, sizeof(szAmt2));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidAliPrecreteAmount.sSettleAmt, 12, 0, szAmt1);
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidAliCreatepayAmount.sSettleAmt, 12, 0, szAmt2);

			memset(szAmt, 0, sizeof(szAmt));
			PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
			
			sprintf(szTmpTag, "VOIDALICREATENUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidAliPrecreteNum + ST_SettleRec.stSettle[i]._VoidAliCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidAliPrecreteNum + ST_SettleRec.stSettle[i]._VoidAliCreatepayNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidAliPrecreteNum; 
			_NumSum -= ST_SettleRec.stSettle[i]._VoidAliCreatepayNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && ST_SettleRec.stSettle[i]._AliRefundNum > 0)
		{
			//支付宝退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._AliRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "ALIREFUNDNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._AliRefundNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._AliRefundNum;
		}

		if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && ST_SettleRec.stSettle[i]._MeituanNum> 0)
		{
			//美团承兑
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._MeituanAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "MEITUANNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._MeituanNum, szDispAmt);

			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._MeituanNum; 
		}

		if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && ST_SettleRec.stSettle[i]._PanjintongNum> 0)
		{
			//盘锦通
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._PanjintongAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "PANJINTONGNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._PanjintongNum, szDispAmt);

			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._PanjintongNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && ST_SettleRec.stSettle[i]._BestpayCreatepayNum > 0)
		{
			//翼支付
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "CREATEBESTPAYNUM%d", i);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt); 		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayCreatepayNum , szDispAmt);			
			
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));			
			AmtAddAmt(szAmtSumTemp,szAmt,szAmtSum); 	
			_NumSum += ST_SettleRec.stSettle[i]._BestpayCreatepayNum; 

			//翼支付撤销
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._VoidBestpayCreatepayAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "VOIDCREATEBESTPAYNUM%d", i);	 
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum, szDispAmt);
			}

			/*这里要减去撤销金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._VoidBestpayCreatepayNum; 
		}
		
		if(GetTieTieSwitchOnoff(TRANS_BESTPAY_REFUND)== YES && ST_SettleRec.stSettle[i]._BestpayRefundNum > 0)
		{
			//翼支付退货
			memset(szAmt, 0, sizeof(szAmt));
			PubHexToAsc(ST_SettleRec.stSettle[i]._BestpayRefundAmount.sSettleAmt, 12, 0, szAmt);
			sprintf(szTmpTag, "REFUNDBESTPAYNUM%d", i);  
			memset(szDispAmt, 0, sizeof(szDispAmt));
			ProAmtToDispOrPnt((char*)szAmt, szDispAmt);
			if(GetVarIsPrintPrintMinus()==YES)
			{
				PubAllTrim(szDispAmt);
				memset(szAmtSumTemp, 0, sizeof(szAmtSumTemp));
				sprintf((char *)szAmtSumTemp, "-%s", szDispAmt);
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayRefundNum, szAmtSumTemp);
			}
			else
			{
				PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", ST_SettleRec.stSettle[i]._BestpayRefundNum, szDispAmt);
			}

			/*这里要减去退货金额*/
			memcpy(szAmtSumTemp,szAmtSum, 13);
			memset(szAmtSum, 0, sizeof(szAmtSum));
			AmtSubAmt(szAmtSumTemp,szAmt,szAmtSum);
			_NumSum -= ST_SettleRec.stSettle[i]._BestpayRefundNum;
		}
#if 0		
		//总计:AmtSubAmt有问题，负减负有问题
		{
			sprintf(szTmpTag, "AMTANDNUM%d", i);	
			memset(szDispAmt, 0, sizeof(szDispAmt));	
			ProAmtToDispOrPnt((char*)szAmtSum, szDispAmt);		
			PubSetFieldValueFormat(szTmpTag, "%3ld %14.14s", _NumSum, szDispAmt);
		}
#endif		
	}
	if (*pnReprintFlag==REPRINT)
	{
		//结算单据重打印凭证
		PubSetFieldValue( "ISREPRINT", "1", 1);
	}

	PubPrintModule(pszModuleName);
	return APP_SUCC;
}

/*
输入密码相关该应用无关

PubSetFieldValue("ISPRINTINPUTPIN", "1", 1);			
//撤消类交易操作询问是否刷卡
//分期付款撤销是否刷卡
PubSetFieldValue("SALEVOIDSTRIP", stAppPosParam.cIsSaleVoidStrip == '1' ? "是" : "不是", 4);			
//预授权完成撤销是否刷卡
PubSetFieldValue("AUTHSALEVOIDSTRIP", stAppPosParam.cIsAuthSaleVoidStrip == '1' ? "是" : "不是", 4);			
//消费撤销是否输密码
//分期付款撤销是否输密码		
PubSetFieldValue("VOIDPIN", stAppPosParam.cIsVoidPin == '1' ? "是" : "不是", 4);			
//预授权撤消密码是否输入密码
PubSetFieldValue("PREAUTHVOIDPIN", stAppPosParam.cIsPreauthVoidPin == '1' ? "是" : "不是", 4);	
//预授权完成撤销是否输密
PubSetFieldValue("AUTHSALEVOIDPIN", stAppPosParam.cIsAuthSaleVoidPin == '1' ? "是" : "不是", 4);			
//授权完成联机密码
PubSetFieldValue("AUTHSALEPIN", stAppPosParam.cIsAuthSalePin == '1' ?  "是" : "不是",4);	

//EMV参数打印

if((*pnPrintType)&PRINT_PARAM_TYPE_EMV) 
	{
		return _printemvparam();
	}


*/
int SetAndPrint_Param(void *ptrPara)
{
	char szTmpStr[21];
	STAPPPOSPARAM stAppPosParam;
	STAPPCOMMPARAM stAppCommParam;
	STAPPCOMMRESERVE stAppCommReserve;
	
	int *pnPrintType=(int *)ptrPara	;
	char *pszModuleName = NULL;
	char szSoftVer[16+1];	
	

	PubClearAll();
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 3, "正在打印");
	PubUpdateWindow();
		
	PubClearFieldValue();

	// 获取参数
	GetAppCommParam(&stAppCommParam);
	GetAppPosParam(&stAppPosParam);
	GetAppCommReserve(&stAppCommReserve);

	if((*pnPrintType)&PRINT_PARAM_TYPE_MERCHANTINFO) /*<商户信息*/
	{
		pszModuleName = "PRINT_PARAM_MERCHANTINFO";
		
		PubSetFieldValue("ISPRINTSHOPPARAM", "1", 1);
		//商户中文名称
		PubSetFieldValue( "MERCHANTNAME", stAppPosParam.szMerchantNameCn, strlen(stAppPosParam.szMerchantNameCn));
		// 商户编号
		PubSetFieldValue( "SHOPID", stAppPosParam.szMerchantId, strlen(stAppPosParam.szMerchantId));
		// POS编号
		PubSetFieldValue( "TERMINALID", stAppPosParam.szTerminalId, strlen(stAppPosParam.szTerminalId));
		//应用名
		if(strlen(stAppPosParam.szAppDispname)>0)
		{
			PubSetFieldValue( "APPNAME", stAppPosParam.szAppDispname, strlen(stAppPosParam.szAppDispname));	
		}
		else
		{
			PubSetFieldValue( "APPNAME", APP_NAMEDESC, strlen(APP_NAMEDESC));	
		}
		/*ADTMS下发广告信息*/
		if (strlen(stAppPosParam.szAdInfo) > 0)
		{
			PubSetFieldValue("ADSINFO", stAppPosParam.szAdInfo, strlen(stAppPosParam.szAdInfo));
		}
	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_VERSION) /*<版本信息*/
	{
		pszModuleName = "PRINT_PARAM_VERSION";
		
		//程序版本号
		PubSetFieldValue("ISPRINTVER", "1", 1);		
		memset(szSoftVer, 0, sizeof(szSoftVer));
		memcpy(szSoftVer, "V", 1);
		GetVarDispSoftVer(&szSoftVer[1]);	
		PubSetFieldValue("APPVER", szSoftVer, strlen(szSoftVer));
	}
#if 0	
	if((*pnPrintType)&PRINT_PARAM_TYPE_TRANSCCTRL) /*<交易控制*/
	{
		pszModuleName = "PRINT_PARAM_TRANSCCTRL";
		
		PubSetFieldValue("ISPRINTTRANS", "1", 1);	
		PntTraditionOnoff(stAppPosParam.sTraditionSwitch);
		PntECashOnoff(stAppPosParam.sECashSwitch);
		PntWalletOnoff(stAppPosParam.sWalletSwitch);
		PntInstallmentOnoff(stAppPosParam.sInstallmentSwitch);
		PntBonusOnoff(stAppPosParam.sBonusSwitch);
		PntPhoneChipSaleOnoff(stAppPosParam.sPhoneChipSaleSwitch);
		PntAppointmentOnoff(stAppPosParam.sAppointmentSwitch);
		PntOrderOnoff(stAppPosParam.sOrderSwitch);
		PntOtherOnoff(stAppPosParam.sOtherSwitch);
		//自动签退设置
		PubSetFieldValue("AUTOLOGOUT", stAppPosParam.cIsAutoLogOut == '1' ? "是" : "不是",4);	
		//结算是否打印明细
		PubSetFieldValue("PRNWATERREC", stAppPosParam.cIsPrintWaterRec == '1' ? "是" : "不是", 4);	
		//离线上送方式
		
		PubSetFieldValue("OFFLINESENDTYPE", stAppPosParam.cIsOfflineSendNow == '1' ? "联机前" : "结算前", 6);
		//离线上送次数
		PubSetFieldValueFormat( "OFFLINESENDCOUNT", "%d", stAppCommParam.cOffResendNum >= 0x30 ? stAppCommParam.cOffResendNum - 0x30 : stAppCommParam.cOffResendNum);
		//是否输入主管密码
		PubSetFieldValue("ADMINPWD", stAppPosParam.cIsAdminPwd == '1' ? "是" : "不是",4);	
		//是否允许手输卡号
		PubSetFieldValue("CARDINPUT", stAppPosParam.cIsCardInput == '1' ? "是" : "不是", 4);	
		//默认刷卡交易
		PubSetFieldValue("DEFAULTTRANSTYPE", stAppPosParam.cDefaultTransType== '1' ? "消费" : "预授权", 6);	
		//退货最大限额
		LeftTrimZero(stAppPosParam.szMaxRefundAmount);
		PubSetFieldValueFormat( "MAXREFUNDAMT", "%lld.%02lld",  AtoLL(stAppPosParam.szMaxRefundAmount)/100,AtoLL(stAppPosParam.szMaxRefundAmount)%100);
	}
#endif	
	if((*pnPrintType)&PRINT_PARAM_TYPE_SYSTEMCTRL) /*<系统参数*/
	{
		pszModuleName = "PRINT_PARAM_SYSTEMCTRL";
		
		PubSetFieldValue("ISPRINTSYSPARAM", "1", 1);	
		//当前交易凭证号
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetVarTraceNo(szTmpStr);
		PubSetFieldValue("TRACENO", szTmpStr, strlen(szTmpStr));	
		//当前交易批次号
		memset(szTmpStr, 0, sizeof(szTmpStr));
		GetVarBatchNo(szTmpStr);
		PubSetFieldValue("BATCHNO", szTmpStr, strlen(szTmpStr));	
		//是否打印中文收单行
		PubSetFieldValue("PNTCHACQUIRER", stAppPosParam.cIsPntChAcquirer == '1' ? "是" : "不是", 4);	
		//是否打印中文发卡行
		PubSetFieldValue("PNTCHCARDSCHEME", stAppPosParam.cIsPntChCardScheme == '1' ? "是" : "不是", 4);	
		//套打签购单样式
		switch(stAppPosParam.cIsNewTicket)
		{
		case '0':
			PubSetFieldValue("PNTTYPE", "新",2);	
			break;
		case '1':
			PubSetFieldValue("PNTTYPE", "旧",2);	
			break;
		case '2':
			PubSetFieldValue("PNTTYPE", "空白",4);	
			break;
		default:
			PubSetFieldValue("PNTTYPE", "  ",2);	
			break;
		}
		// 打印联数设置
		PubSetFieldValueFormat( "PRINTPAGE", "%d", stAppPosParam.cPrintPageCount - '0');		
		//签购单是否打印英文
		PubSetFieldValue("TICKETWITHEN", stAppPosParam.cIsTicketWithEn == '1' ? "是" : "不是", 4);	
		//冲正次数
		PubSetFieldValueFormat( "REVERSALNUM", "%d", stAppCommParam.cReSendNum >= 0x30 ? stAppCommParam.cReSendNum - 0x30 : stAppCommParam.cReSendNum);		
		//最大交易笔数
		PubSetFieldValueFormat( "MAXTRANSCOUNT", "%d", atoi(stAppPosParam.szMaxTransCount));
		//小费比例
		PubSetFieldValueFormat( "TIPRATE", "%d",  atoi(stAppPosParam.szTipRate));		
		//打印字体
		PubSetFieldValue("PNTFONTSIZE", (stAppPosParam.cPntFontSize== '0' )? "小" : (stAppPosParam.cPntFontSize== '1' ?"中":"大"), 2);	
		//打印负号
		PubSetFieldValue("PRINTMINUS", stAppPosParam.cIsPrintMinus == '1' ? "是" : "不是", 4);	
		//打印所有交易
		PubSetFieldValue("PNTALLTRANS", stAppPosParam.cIsPrintAllTrans == '1' ? "是" : "不是", 4);	
		//收单行名称
		PubSetFieldValue("UNKNOWBANK", stAppPosParam.szPrintUnknowBankInfo, strlen(stAppPosParam.szPrintUnknowBankInfo));	
	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_OTHER) /*<其他*/
	{
		pszModuleName = "PRINT_PARAM_OTHER";

		PubSetFieldValue("ISPRINTKEY", "1", 1);	
		//主密钥索引号
		PubSetFieldValue("MAINKEYNO", stAppPosParam.szMainKeyNo, strlen(stAppPosParam.szMainKeyNo));
		//密钥算法
		PubSetFieldValue("ENCYPTMODE", stAppPosParam.cEncyptMode == '0' ? "单DES" : "3DES",5);

	}

	if((*pnPrintType)&PRINT_PARAM_TYPE_COMM)
	{
		pszModuleName = "PRINT_PARAM_COMM";
		
		PubSetFieldValue("ISPRINTCOMM", "1", 1);		//通讯方式
		switch(stAppCommParam.cCommType)
		{
		case COMM_RS232:
			PubSetFieldValue( "COMMTYPE", "串口", 4);
			break;
		case COMM_DIAL:
			PubSetFieldValue( "COMMTYPE", "拨号", 4);
			PubSetFieldValue( "TELNO1",  stAppCommParam.szTelNum1, strlen(stAppCommParam.szTelNum1));
			PubSetFieldValue( "TELNO2",  stAppCommParam.szTelNum2, strlen(stAppCommParam.szTelNum2));
			PubSetFieldValue( "TELNO3",  stAppCommParam.szTelNum3, strlen(stAppCommParam.szTelNum3));
			PubSetFieldValue( "MANAGETELNO",  stAppCommParam.szManageTelNum, strlen(stAppCommParam.szManageTelNum));
			break;
		case COMM_GPRS:
		case COMM_CDMA:
		case COMM_ETH:
		case COMM_WIFI:
			if (stAppCommParam.cCommType == COMM_GPRS)
			{
				PubSetFieldValue( "COMMTYPE", "GPRS", 4);
				PubSetFieldValue( "APN1", stAppCommParam.szAPN1, strlen(stAppCommParam.szAPN1));
				PubSetFieldValue( "APN2", stAppCommParam.szAPN2, strlen(stAppCommParam.szAPN2));
			}
			else if (stAppCommParam.cCommType == COMM_ETH)
			{
				PubSetFieldValue( "COMMTYPE", "以太网", 6);
				PubSetFieldValue( "IPADDR", stAppCommParam.szIpAddr, strlen(stAppCommParam.szIpAddr));
				PubSetFieldValue( "MASK", stAppCommParam.szMask, strlen(stAppCommParam.szMask));
				PubSetFieldValue( "GATE",  stAppCommParam.szGate, strlen(stAppCommParam.szGate));
			}
			else if (stAppCommParam.cCommType == COMM_CDMA)
			{
				PubSetFieldValue( "COMMTYPE", "CDMA", 4);
			
			}
			else if(stAppCommParam.cCommType == COMM_WIFI)
			{
				PubSetFieldValue( "COMMTYPE", "WIFI", 4);
				PubSetFieldValue("WIFISSID", stAppCommReserve.szWifiSsid, strlen(stAppCommReserve.szWifiSsid));
				switch(stAppCommReserve.cWifiMode)
				{
				case WIFI_NET_SEC_NONE:
					PubSetFieldValue( "WIFIMODE", "不加密", 6);
					break;
				case WIFI_NET_SEC_WEP_OPEN:
					PubSetFieldValue( "WIFIMODE", "OPEN", 4);
					break;
				case WIFI_NET_SEC_WEP_SHARED:
					PubSetFieldValue( "WIFIMODE", "SHARED", 6);
					break;
				case WIFI_NET_SEC_WPA:
					PubSetFieldValue( "WIFIMODE", "WAP", 3);
					break;
				case WIFI_NET_SEC_WPA2:
					PubSetFieldValue( "WIFIMODE", "WAP2", 4);
					break;
				}
			}

			if(stAppCommParam.cIsDns)
			{
				PubSetFieldValue("DNSIP",stAppCommParam.szDNSIp1, strlen(stAppCommParam.szDNSIp1));
				PubSetFieldValue("DOMAIN1",stAppCommParam.szDomain, strlen(stAppCommParam.szDomain));
				PubSetFieldValue("DNSPORT1",stAppCommParam.szPort1, strlen(stAppCommParam.szPort1));
				PubSetFieldValue("DOMAIN2",stAppCommReserve.szDomain2, strlen(stAppCommReserve.szDomain2));
				PubSetFieldValue("DNSPORT2",stAppCommParam.szPort2, strlen(stAppCommParam.szPort2));
			}
			else
			{
				PubSetFieldValue( "IP1", stAppCommParam.szIp1, strlen(stAppCommParam.szIp1));
				PubSetFieldValue( "PORT1", stAppCommParam.szPort1, strlen(stAppCommParam.szPort1));
				PubSetFieldValue( "IP2", stAppCommParam.szIp2, strlen(stAppCommParam.szIp2));
				PubSetFieldValue( "PORT2", stAppCommParam.szPort2, strlen(stAppCommParam.szPort2));
			}

			if ((stAppCommParam.cCommType == COMM_GPRS)||(stAppCommParam.cCommType == COMM_CDMA))
			{
				//用户名
				PubSetFieldValueFormat( "USERNAME", "%3.3s****", stAppCommParam.szUser);
				
				//用户密码
				PubSetFieldValueFormat( "USERPWD", "%3.3s****", stAppCommParam.szPassWd);	

				//呼叫中心号码
				PubSetFieldValue( "WIRELESSDIALNUN", stAppCommParam.szWirelessDialNum, strlen(stAppCommParam.szWirelessDialNum));

				//是否短链接
				PubSetFieldValue( "COMMMODE", (stAppCommParam.cMode == '1' || stAppCommParam.cMode == 1) ? "是" : "否", 2);
			}
			break;
		default:
			break;
		}
		
		//TPDU
		PubSetFieldValueFormat( "TPDU", "%02x%02x%02x%02x%02x", 
			stAppCommParam.sTpdu[0], stAppCommParam.sTpdu[1], stAppCommParam.sTpdu[2], stAppCommParam.sTpdu[3], stAppCommParam.sTpdu[4]);
		
		//重拨次数
		PubSetFieldValueFormat( "REDIALNUM", "%d", stAppCommParam.cReDialNum >= 0x30 ? stAppCommParam.cReDialNum - 0x30 : stAppCommParam.cReDialNum);

		//交易超时时间
		PubSetFieldValueFormat( "TIMEOUT", "%d", stAppCommParam.cTimeOut);

		//预拨号设置
		PubSetFieldValue( "ISPREDIAL",  stAppCommParam.cPreDialFlag=='1' || stAppCommParam.cPreDialFlag==1 ? "是" : "否", 2);
	}
	
	PubPrintModule(pszModuleName);
	return APP_SUCC ;
}

int SetAndPrint_Allwater(void *ptrPara)
{
	static int nHasRecord;
	int nFileHandle;
	STWATER stWater;
	int nWaterNum;
	int i, nPrintNum = 30;
	int nStartRecNo;
	char szTmpStr[64];
	char szStr[32];
	int *pnCurPrintRecNo;
	char *pszModuleName = "PRINT_ALL_WATER";
	char szTmpAmt[12+1] = {0};

	if(PubGetPrinter() == _PRINTTYPE_IP)
	{
		nPrintNum = 16; 	
	}	
	pnCurPrintRecNo = (int *)ptrPara;
	//计算总笔数
	GetWaterNum(&nWaterNum);
	nStartRecNo = *pnCurPrintRecNo;
	if (nStartRecNo==1)
	{
		nHasRecord=0;
	}
	
	ASSERT_FAIL(PubOpenFile(FILE_WATER, "r", &nFileHandle));

	for(i=0; i<nPrintNum && (*pnCurPrintRecNo <=nWaterNum); (*pnCurPrintRecNo)++)
	{
		PubClearFieldValue();
		PubReadRec(nFileHandle, *pnCurPrintRecNo, (char *)&stWater);
		
		if (stWater.cStatus == '1' || stWater.cStatus == 0x01)
		{
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}
		if (stWater.cStatus == 0x04)	/**<卡检测中心测试要求不打印被调整的交易injz-20121219*/	
		{
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}

		if(stWater.cTransType == TRANS_VOID_SALE ||stWater.cTransType == TRANS_VOID_AUTHSALE ||stWater.cTransType == TRANS_VOID_INSTALMENT
			||stWater.cTransType == TRANS_VOID_COUPON_VERIFY || stWater.cTransType == TRANS_VOID_CARD_VERIFY
			||stWater.cTransType == TRANS_VOID_TEL_VERIFY || stWater.cTransType == TRANS_VOID_PRECREATE 
			|| stWater.cTransType == TRANS_VOID_CREATEANDPAY || stWater.cTransType == TRANS_VOID_CREATEANDPAYBAIDU
			|| stWater.cTransType == TRANS_VOID_JD_PRECREATE || stWater.cTransType == TRANS_VOID_ALI_CREATEANDPAY
			|| stWater.cTransType == TRANS_VOID_MEITUAN || stWater.cTransType == TRANS_VOID_DZ_PRECREATE || stWater.cTransType == TRANS_VOID_DZ_CREATEANDPAY
			|| stWater.cTransType == TRANS_VOID_BESTPAY_CREATEANDPAY
		)

		{	
			/*撤销交易不打印*/
			if(GetVarIsPrintAllTrans()==NO)
			{
				continue;
			}
		}
		
		// 打印单据头
		if (nStartRecNo==1 && i==0 )
		{
			PubSetFieldValue("ISTITLE", "1", 1);
			PubSetFieldValue("ISDESCRIPT", "1", 1);
		}
		// 凭证号
		PubSetFieldValueFormat("TRACENO", "%02x%02x%02x", stWater.sTrace[0], stWater.sTrace[1], stWater.sTrace[2]);
        	
		// 类型
		memset(szTmpStr, 0, sizeof(szTmpStr));
 		GetTransNameForTP(&stWater, szTmpStr, szStr);
		PubAddSymbolToStr(szTmpStr, 16, ' ', 1);
		PubSetFieldValueFormat("TYPE", "%s",  szTmpStr);
        	
		// 卡号
		/*
		switch(stWater.cTransType)
		{
			case TRANS_COUPON_VERIFY:		
				PubSetFieldValue("CARDNO", stWater.sCouponID, strlen(stWater.sCouponID));
				break;
			case TRANS_TEL_VERIFY:
				PubSetFieldValue("CARDNO",stWater.sTelNo, strlen(stWater.sTelNo));
				break;
			case TRANS_CARD_VERIFY:
				PubHexToAsc((uchar *)stWater.sPan, stWater.nPanLen, 0, (uchar *)szTmpStr);
				CtrlCardPan((uchar*)szTmpStr, stWater.nPanLen, stWater.cTransType, stWater.cEMV_Status);
				PubSetFieldValueFormat("CARDNO", "%-19.19s", szTmpStr);
				break;
			case TRANS_VOID_VERIFY:
				break;
			case TRANS_REFUND_VERIFY:
				break;
		}
		*/

		
		// 金额
		memset(szTmpStr, 0, sizeof(szTmpStr));
		memset(szTmpAmt, 0, sizeof(szTmpAmt));
		if ((stWater.cTransType == TRANS_COUPON_VERIFY) || (stWater.cTransType == TRANS_TEL_VERIFY) ||
			(stWater.cTransType == TRANS_CARD_VERIFY) || (stWater.cTransType == TRANS_VOID_COUPON_VERIFY) || 
			(stWater.cTransType == TRANS_VOID_TEL_VERIFY) || (stWater.cTransType == TRANS_VOID_CARD_VERIFY))
		{
			PubHexToAsc((uchar *)stWater.szRealAmount, 12, 0, (uchar *)szTmpAmt);
		}
		else
		{
			PubHexToAsc((uchar *)stWater.szAmount, 12, 0, (uchar *)szTmpAmt);
		}
		ProAmtToDispOrPnt(szTmpAmt, szTmpStr);
		PubAllTrim(szTmpStr);
		DealPrintAmount(stWater.cTransType, szTmpStr);
		PubAddSymbolToStr(szTmpStr, 13, ' ', 0);
		PubSetFieldValueFormat("AMOUNT", "%s", szTmpStr);	
		
		// 授权码
		//PubSetFieldValue("AUTHNO", stWater.szAuthCode, strlen(stWater.szAuthCode));
		
		i++;
		PubPrintModule(pszModuleName);
	}
	ASSERT_FAIL(PubCloseFile(&nFileHandle));
	if (i>0)
	{
		nHasRecord = 1;
	}
	else  //无可打印明细纪录
	{
		return APP_FUNCQUIT;
	}
		
	// 打印单据尾
	if (*pnCurPrintRecNo>nWaterNum && nHasRecord==1)
	{
		PubClearFieldValue();
		
		// 交易类型:S-消费 R-退货 P-授权完成  L-离线  A-调整 
		if(GetVarIsPrintAllTrans()==NO)
		{
			PubSetFieldValue( "ISEXPLAIN", "1", 1);
		}
		else
		{
			PubSetFieldValue( "ISEXPLAINALL", "1", 1);
		}
		if (PubGetPrinter()==_PRINTTYPE_TP)
		{
			//- - - - - - - X - - - - - - - X - - - - - - - 
			PubSetFieldValue( "ISEND", "1", 1);
		}
		PubPrintModule(pszModuleName);
	}

	return APP_SUCC;
}

//汇总
int SetAndPrint_Total(void *ptrPara)
{
	STSETTLE stSettle_NK, stSettle_WK;
	char *pszModuleName = "PRINT_TOTAL";
	char szAmt1[13] = {0};
	char szAmt2[13] = {0};
	char szAmt[13] = {0};
	char szDispAmt[16] = {0};
	
	GetSettleDataNK(&stSettle_NK);
	GetSettleDataWK(&stSettle_WK);

	PubClearFieldValue();

	if(GetTieTieSwitchOnoff(TRANS_COUPON_VERIFY)== YES && stSettle_NK._CouponVerifyNum)
	{
		//串码验券
		PubSetFieldValueFormat( "COUPONNUM", "%4ld笔", stSettle_NK._CouponVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._CouponVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "COUPONAMT", "  %s" , szDispAmt);

		//串码撤销
		PubSetFieldValueFormat( "VOIDCOUPONNUM", "%4ld笔", stSettle_NK._VoidCouponVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidCouponVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCOUPONAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_TEL_VERIFY)== YES && stSettle_NK._TelVerifyNum > 0)
	{
		//手机号验券
		PubSetFieldValueFormat( "TELNUM", "%4ld笔", stSettle_NK._TelVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._TelVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "TELAMT", "  %s" , szDispAmt);

		//手机号验证撤销
		PubSetFieldValueFormat( "VOIDTELNUM", "%4ld笔", stSettle_NK._VoidTelVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidTelVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDTELAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_CARD_VERIFY)== YES && stSettle_NK._CardVerifyNum > 0)
	{
		//银行卡验券
		PubSetFieldValueFormat( "CARDNUM", "%4ld笔", stSettle_NK._CardVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._CardVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt1, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "CARDAMT", "  %s" , szDispAmt);

		//银行卡验证撤销
		PubSetFieldValueFormat( "VOIDCARDNUM", "%4ld笔", stSettle_NK._VoidCardVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VodiCardVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCARDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_REFUND_VERIFY)== YES && stSettle_NK._RefundVerifyNum > 0)
	{
		//验证退货
		PubSetFieldValueFormat( "REFUNDNUM", "%4ld笔", stSettle_NK._RefundVerifyNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundVerifyAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDAMT", "  %s" , szDispAmt);
	}

	if((GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES || GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES)
		&& (stSettle_NK._PrecreateNum > 0 || stSettle_NK._CreatepayNum > 0))
	{
		//微信支付扫码+ 条码
		PubSetFieldValueFormat( "WECHATNUM", "%4ld笔", stSettle_NK._PrecreateNum+stSettle_NK._CreatepayNum);

		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._PrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt2, 0, sizeof(szAmt2));
		PubHexToAsc(stSettle_NK._CreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt2);

		memset(szAmt, 0, sizeof(szAmt));
		PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);

		memset(szDispAmt, 0, sizeof(szDispAmt));
		ProAmtToDispOrPnt(szAmt, szDispAmt);
		PubAllTrim(szDispAmt);
		PubSetFieldValueFormat( "WECHATAMT", "  %s" , szDispAmt);
	}
	
	if((GetTieTieSwitchOnoff(TRANS_CREATEANDPAY)== YES || GetTieTieSwitchOnoff(TRANS_PRECREATE)== YES)
		&& (stSettle_NK._VoidPrecreateNum > 0 || stSettle_NK._VoidCreatepayNum > 0))
	{
		//扫码支付撤销 +  条码支付撤销
		PubSetFieldValueFormat( "VOIDWECHARTNUM", "%4ld笔", stSettle_NK._VoidPrecreateNum + stSettle_NK._VoidCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt2, 0, sizeof(szAmt2));
		PubHexToAsc(stSettle_NK._VoidCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt2);

		memset(szAmt, 0, sizeof(szAmt));
		PubAscAddAsc((uchar *)szAmt1,(uchar *)szAmt2,(uchar *)szAmt);
		
		ProAmtToDispOrPnt(szAmt, szAmt1);
		PubAllTrim(szAmt1);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt1);
		}
		else
		{
			strcpy(szDispAmt, szAmt1);
		}
		PubSetFieldValueFormat( "VOIDWECHARTAMT", "  %s" , szDispAmt);
		
	}

	if(GetTieTieSwitchOnoff(TRANS_WX_REFUND)== YES && stSettle_NK._RefundWechatNum > 0)
	{
		//微信退货
		PubSetFieldValueFormat( "REFUNDWECHATNUM", "%4ld笔", stSettle_NK._RefundWechatNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundWeChatAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDWECHATAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_CREATEANDPAYBAIDU)== YES && stSettle_NK._BaiduCreatepayNum > 0)
	{
		//百度支付
		PubSetFieldValueFormat( "CREATEBAIDUNUM", "%4ld笔", stSettle_NK._BaiduCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BaiduCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEBAIDUAMT", "  %s" , szDispAmt);
		
		//百度支付撤销
		PubSetFieldValueFormat( "VOIDCREATEBAIDUNUM", "%4ld笔", stSettle_NK._VoidBaiduCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidBaiduCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEBAIDUAMT", "  %s" , szDispAmt);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_BAIDU_PRECREATE)== YES && stSettle_NK._BaiduPrecreteNum > 0)
	{
		//百度被扫支付
		PubSetFieldValueFormat( "PRECREATEBAIDUNUM", "%4ld笔", stSettle_NK._BaiduPrecreteNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BaiduPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PRECREATEBAIDUAMT", "  %s" , szDispAmt);
		
		//百度被扫支付撤销
		PubSetFieldValueFormat( "VOIDPRECREATEBAIDUNUM", "%4ld笔", stSettle_NK._VoidBaiduPrecreteNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidBaiduPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDPRECREATEBAIDUAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BAIDU_REFUND)== YES && stSettle_NK._RefundBaiduNum > 0)
	{
		//百度退货
		PubSetFieldValueFormat( "REFUNDBAIDUNUM", "%4ld笔", stSettle_NK._RefundBaiduNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._RefundBaiduAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDBAIDUAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_JD_PRECREATE)== YES && stSettle_NK._JDPrecreateNum > 0)
	{
		//京东支付
		PubSetFieldValueFormat( "CREATEJDNUM", "%4ld笔", stSettle_NK._JDPrecreateNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._JDPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEJDAMT", "  %s" , szDispAmt);
		
		//京东支付撤销
		PubSetFieldValueFormat( "VOIDCREATEJDNUM", "%4ld笔", stSettle_NK._VoidJDPrecreateNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidJDPrecreateAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEJDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_JD_REFUND)== YES && stSettle_NK._JDRefundNum > 0)
	{
		//京东退货
		PubSetFieldValueFormat( "REFUNDJDNUM", "%4ld笔", stSettle_NK._JDRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._JDRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDJDAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_CREATEANDPAY)== YES && stSettle_NK._AliCreatepayNum > 0)
	{
		//支付宝支付
		PubSetFieldValueFormat( "CREATEALINUM", "%4ld笔", stSettle_NK._AliCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEALIAMT", "  %s" , szDispAmt);
		
		//支付宝支付撤销
		PubSetFieldValueFormat( "VOIDCREATEALINUM", "%4ld笔", stSettle_NK._VoidAliCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidAliCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEALIAMT", "  %s" , szDispAmt);
	}

	
	if(GetTieTieSwitchOnoff(TRANS_ALI_REFUND)== YES && stSettle_NK._AliRefundNum > 0)
	{
		//支付宝退货
		PubSetFieldValueFormat( "REFUNDALINUM", "%4ld笔", stSettle_NK._AliRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDALIAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_ALI_PRECREATE)== YES && stSettle_NK._AliPrecreteNum > 0)
	{
		//支付宝被扫支付
		PubSetFieldValueFormat( "PRECREATEALINUM", "%4ld笔", stSettle_NK._AliPrecreteNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._AliPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PRECREATELIAMT", "  %s" , szDispAmt);
		
		//支付宝被扫支付撤销
		PubSetFieldValueFormat( "VOIDPRECREATEALINUM", "%4ld笔", stSettle_NK._VoidAliPrecreteNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._VoidAliPrecreteAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDPRECREATELIAMT", "  %s" , szDispAmt);
	}

	

	if(GetTieTieSwitchOnoff(TRANS_COUPON_MEITUAN)== YES && stSettle_NK._MeituanNum > 0)
	{
		//美团兑券
		PubSetFieldValueFormat( "MEITUANNUM", "%4ld笔", stSettle_NK._MeituanNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._MeituanAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "MEITUANAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_PANJINTONG)== YES && stSettle_NK._PanjintongNum > 0)
	{
		//盘锦通支付
		PubSetFieldValueFormat( "PANJINTONGNUM", "%4ld笔", stSettle_NK._PanjintongNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._PanjintongAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "PANJINTONGAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && stSettle_NK._BestpayCreatepayNum > 0)
	{
		//翼支付
		PubSetFieldValueFormat( "CREATEBESTPAYNUM", "%4ld笔", stSettle_NK._BestpayCreatepayNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		strcpy(szDispAmt, szAmt);
		PubSetFieldValueFormat( "CREATEBESTPAYAMT", "  %s" , szDispAmt);
		
		//翼支付撤销
		PubSetFieldValueFormat( "VOIDCREATEBESTPAYNUM", "%4ld笔", stSettle_NK._BestpayCreatepayNum > 0);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayCreatepayAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "VOIDCREATEBESTPAYAMT", "  %s" , szDispAmt);
	}

	if(GetTieTieSwitchOnoff(TRANS_BESTPAY_CREATEANDPAY)== YES && stSettle_NK._BestpayRefundNum > 0)
	{
		//翼支付退货
		PubSetFieldValueFormat( "REFUNDBESTPAYNUM", "%4ld笔", stSettle_NK._BestpayRefundNum);
		memset(szAmt1, 0, sizeof(szAmt1));
		PubHexToAsc(stSettle_NK._BestpayRefundAmount.sSettleAmt, 12, 0, (uchar *)szAmt1);
		memset(szAmt, 0, sizeof(szAmt));
		ProAmtToDispOrPnt(szAmt1, szAmt);
		PubAllTrim(szAmt);
		if(GetVarIsPrintPrintMinus()==YES)
		{
			sprintf(szDispAmt, "%c%s", '-', szAmt);
		}
		else
		{
			strcpy(szDispAmt, szAmt);
		}
		PubSetFieldValueFormat( "REFUNDBESTPAYAMT", "  %s" , szDispAmt);
	}

	PubPrintModule(pszModuleName);
	return APP_SUCC;
}

#if 0

static int PntHex(const char *pszTitle, const uchar *psContent, const int nConternLen)
{
	int i;
	
	ASSERT_FAIL(PubPrePrinter(pszTitle));
	for (i = 0; i < nConternLen; i++)
	{
		ASSERT_FAIL(PubPrePrinter("%02X", psContent[i]));
	}
	ASSERT_FAIL(PubPrePrinter("\r"));
	return APP_SUCC;		
}


int  _printemvparam()
{
#if defined(EMV_IC)
	int nRet;
	int i;
	char sAid[20][17];
	char sCapk[50][6];
	emvparam stAID;
	capk stCAPK;
	int nRes;
	uint nNum;

	PubClearAll();
	PubDisplayStrInline(DISPLAY_MODE_CENTER, 3, "正在打印");
	PubUpdateWindow();
	
	nRet = EMV_EnumAID(sAid);
	if (nRet > 0)
	{
		ASSERT_FAIL(PubPrePrinter("AID_Num:%d\r", nRet));
		for (i = 0; i < nRet; i++)
		{	
			stAID._aid_len = sAid[i][0];
			memcpy(stAID._aid, &sAid[i][1], sAid[i][0]);
			nRes = EMV_OperAID(&stAID,AID_GET);
			if (nRes == APP_SUCC)
			{
				ASSERT_FAIL(PubPrePrinter("---------------------\r"));
				PntHex("AID:", stAID._aid, stAID._aid_len);
				PntHex("VER:", stAID._app_ver, 2);
				PntHex("TAC_Default:", stAID._tac_default, 5);
				PntHex("TAC_Decline:", stAID._tac_denial, 5);
				PntHex("TAC_Online :", stAID._tac_online, 5);
				PubC4ToInt(&nNum, (uchar *)stAID._floorlimit);
				ASSERT_FAIL(PubPrePrinter("FloorLimit:%d\r", nNum));
				PubC4ToInt(&nNum, (uchar *)stAID._threshold_value);
				ASSERT_FAIL(PubPrePrinter("ThresholdValue:%d\r", nNum));
				PntHex("MaxTargetPercent:", &stAID._max_target_percent, 1);
				PntHex("TargetPercent:", &stAID._target_percent, 1);
				PntHex("Default DDOL:", stAID._default_ddol, stAID._default_ddol_len);
				PntHex("Default TDOL:", stAID._default_tdol, stAID._default_tdol_len);
				PntHex("CAP:", stAID._cap, 3);
				PntHex("ADDCAP:", stAID._add_cap, 5);
				PntHex("ASI(PartialAID):", &stAID._app_sel_indicator, 1);
				PntHex("ICS:", stAID._ics, 7);
				PntHex("TestType:", &stAID._status, 1);
				PntHex("_limit_exist:", &stAID._limit_exist, 1);
				PntHex("_ec_limit:", stAID._ec_limit, 6);
				PntHex("_cl_limit:", stAID._cl_limit, 6);
				PntHex("_cl_offline_limit:", stAID._cl_offline_limit, 6);
				PntHex("_cvm_limit:", stAID._cvm_limit, 6);
				ASSERT_FAIL(PubPrePrinter("---------------------\r"));
			}

		}
	}
	else
	{
		ASSERT_FAIL(PubPrePrinter("EMV_EnumAID=%d\r", nRet));
	}
	
	nRet = EMV_EnumCAPK(0, 49, sCapk);
	if (nRet > 0)
	{
		ASSERT_FAIL(PubPrePrinter("CAPK_Num:%d\r", nRet));
		for (i = 0; i < nRet; i++)
		{
			memcpy(stCAPK._rid, &sCapk[i][0], 5);
			stCAPK._index = sCapk[i][5];
			nRes = EMV_OperCAPK(&stCAPK, CAPK_GET);
			if (nRes == APP_SUCC)
			{
				ASSERT_FAIL(PubPrePrinter("Index:%02x ", stCAPK._index));
				PntHex("RID:", stCAPK._rid, 5);
				PntHex("   ExpDate:", stCAPK._expired_date, 4);
			}
		}
	}
	else
	{
		ASSERT_FAIL(PubPrePrinter("EMV_EnumCAPK=%d\r", nRet));
	}
#endif

	if (PubGetPrinter()==_PRINTTYPE_TP)
	{
		ASSERT_FAIL(PubPrePrinter("\r\r\r" ));
		ASSERT_FAIL(PubPrePrinter("- - - - - - - X - - - - - - - X - - - - - - - "));
	}
	
	return APP_SUCC;
}
#endif

int Print2DScan(char *pszAscInfo)
{
	DataInfo stDataInfo;
	BmpInfo stBmpInfo;
	memset(&stDataInfo,0,sizeof(DataInfo));
	memset(&stBmpInfo,0,sizeof(BmpInfo));
	

	stDataInfo.nLevel = 3;
	stDataInfo.nMask = -1;
	stDataInfo.nVersion = 6;
	strcpy(stDataInfo.szInputData,pszAscInfo);
	stDataInfo.nLen = strlen(stDataInfo.szInputData);

	if(APP_SUCC == PubEncodeDataAndGenerateBmp(&stDataInfo,&stBmpInfo))
	{
		PubPrintImage(stBmpInfo.xsize, stBmpInfo.ysize, 100, stBmpInfo.bmpbuff);
	}
	else
	{
		PubClearAll();
		PubMsgDlg(NULL, "生成条码失败", 0, 10);
		return APP_FAIL;
	}
	return APP_SUCC;
}



int _PrintImage(void *ptrPara)
{
	char szStr[16], szShow[64];
	int nLen;
	PrintDataInfo *pstDataInfo = (PrintDataInfo *)ptrPara;

	PubPrintImage(pstDataInfo->pBmpInfo->xsize, pstDataInfo->pBmpInfo->ysize, 105, pstDataInfo->pBmpInfo->bmpbuff);

	memset(szStr, 0, sizeof(szStr));
	ProAmtToDispOrPnt(pstDataInfo->pszAmt, szStr);
	PubAllTrim(szStr);

	sprintf(szShow, "金额:%s%s", "RMB", szStr);
	nLen = (32 - strlen(szShow))/2;//空格数
	nLen += strlen(szShow);
	PubAddSymbolToStr(szShow, nLen, ' ', 0);
	
	ASSERT(PubSetPrintFont(PRN_ZM_FONT_12x24A, PRN_HZ_FONT_24x24A, PRN_MODE_NORMAL, 0));
	ASSERT_FAIL(PubPrePrinter("\r\r%s", szShow));
	ASSERT_FAIL(PubPrePrinter("\r       欢迎使用都能付支付," ));
	ASSERT_FAIL(PubPrePrinter("\r      扫描并支付后按确认键!\r" ));
	
	ASSERT_FAIL(PubPrePrinter("\r\r\r" ));
	return APP_SUCC;
}


/**
* @brief 打印签购单
*
*    通过调用_printwater 实现
* @param in const STWATER *pstWater 流水结构指针
* @param in int nReprintFlag  打印控制标志
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PrintImage(PrintDataInfo stDataInfo)
{
 	ASSERT_FAIL(PubPrintCommit( _PrintImage, (void *)&stDataInfo, 1)); 
	return APP_SUCC;
}


int printYiwei(const char *pszCode)
{
	char szCode[128];
    STBMPIMAGE stBmpImage;

	memset(szCode, 0, sizeof(szCode));
    memset(&stBmpImage, 0, sizeof(STBMPIMAGE));
#if 1

	strcpy(szCode, pszCode);
	ASSERT_FAIL(MakeCode128(szCode, 1, 80, &stBmpImage, HORIZONTAL));
	PubSetFieldValueImage("COUPONPICPARAM",stBmpImage.sBmpImage,stBmpImage.height * stBmpImage.width);
	PubSetParamImg("COUPONPICSRC", stBmpImage.width, stBmpImage.height, (384 - stBmpImage.width)/2);

//	ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
//    PubPrePrinter("\r\r");

#else

    //setprintgreyscale(5);
    //setprintheatmode();

//	{
//		int i = 0;
		//while (i++ < 10)
//			{
//		ASSERT_FAIL(MakeCode128(pszCode, 20, 80, &stBmpImage, HORIZONTAL));
		//ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
//		PrintImage((BmpInfo *)&stBmpImage);
//			}
	
//	}



//    ASSERT_FAIL(MakeCode128(pszCode, 1, 160, &stBmpImage, HORIZONTAL));

//	PubSetFieldValueImage("COUPONPICPARAM",stBmpImage.sBmpImage,stBmpImage.height * stBmpImage.width);
//	PubSetParamImg("COUPONPICSRC",stBmpImage.width, stBmpImage.height, 10);

//	PrintImage((BmpInfo *)&stBmpImage);

	//PubSetParamLogo("COUPONPIC", szTmpStr, unLogoXpos);
	
    //ASSERT_FAIL(PubPrintImage(stBmpImage.width, stBmpImage.height, 10, stBmpImage.sBmpImage));
    
//    PubPrePrinter("\r\r\r");
    //setprintgreyscale(3);
    //setprintheatmode();
    printimage(301, 52, 0, tucBMPym_code128_stand);
    PubPrePrinter("\r\r");
#endif
    return APP_SUCC;
}



