/**
* @file magtxn.h
* @brief 磁条卡交易处理模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/

#ifndef _MAGTXN_H_
#define _MAGTXN_H_ 

#define STRIPE_TIMEOUT 60			/**<刷卡超时时间*/
#define DISPPAN_TIMEOUT 30			/**<卡号确认超时时间*/
#define INPUT_AMOUNT_TIMEOUT 60	/**<金额输入确认超时时间*/
#define INPUT_DATE_TIMEOUT 60		/**<输入日期超时时间*/
#define INPUT_STRING_TIMEOUT 60	/**<输入字符超时时间*/

#if defined (SUPPORT_ELECSIGN)
#define MAX_PACK_SIZE 2000			/**<最大包长度*/
#else
#define MAX_PACK_SIZE 1000			/**<最大包长度*/
#endif

#define CURRENCY_CODE_CN "156"		/**< 交易货币代(Currency Code Of Transaction)*/

enum TRANSTYPE
{
	TRANS_BALANCE,				/**< 查询*/
	TRANS_SALE,					/**< 消费*/
	TRANS_AUTHSALE,				/**< 授权完成请求*/
	TRANS_AUTHSALEOFF,			/**< 授权完成通知*/
	TRANS_AUTHSETTLE,			/**< 授权结算*/
	TRANS_PREAUTH,				/**< 预授权*/
	TRANS_ADDTO_PREAUTH,		/**< 追加预授权*/
	TRANS_REFUND,				/**< 退货*/
	TRANS_VOID_SALE,			/**< 消费撤销*/
	TRANS_VOID_AUTHSALE,		/**< 完成撤销*/
	TRANS_VOID_AUTHSETTLE,		/**< 结算撤销*/
	TRANS_VOID_PREAUTH,			/**< 授权撤销*/
	TRANS_VOID_REFUND,			/**< 撤销退货*/
	TRANS_OFFLINE,				/**< 离线结算*/
	TRANS_ADJUST,				/**< 结算调整*/
	TRANS_INSTALMENT,			/**< 分期付款*/
	TRANS_VOID_INSTALMENT,		/**< 撤销分期*/
	TRANS_EMV_SCRIPE,			/**< EMV脚本结果通知*/
	TRANS_EMV_REFUND,			/**< EMV脱机退货*/
	TRANS_BONUS_IIS_SALE,		/**< 发卡行积分消费*/
	TRANS_VOID_BONUS_IIS_SALE,	/**< 撤销发卡行积分消费*/
	TRANS_BONUS_ALLIANCE,		/**< 联盟积分消费*/
	TRANS_VOID_BONUS_ALLIANCE,	/**< 撤销联盟积分消费*/
	TRANS_ALLIANCE_BALANCE,		/**< 联盟积分查询*/
	TRANS_ALLIANCE_REFUND,		/**< 联盟积分退货*/
	TRANS_PHONE_SALE,			/**< 手机芯片消费*/
	TRANS_VOID_PHONE_SALE,		/**< 撤销手机芯片消费*/
	TRANS_REFUND_PHONE_SALE,	/**< 手机芯片退货*/
	TRANS_PHONE_PREAUTH,		/**< 手机芯片预授权*/	
	TRANS_VOID_PHONE_PREAUTH,	/**< 撤销手机芯片预授权*/
	TRANS_PHONE_AUTHSALE,		/**< 手机芯片预授权完成*/
	TRANS_PHONE_AUTHSALEOFF,	/**< 手机芯片完成通知*/
	TRANS_VOID_PHONE_AUTHSALE,	/**< 撤销手机完成请求*/
	TRANS_PHONE_BALANCE,		/**< 手机芯片余额查询*/
	TRANS_ORDER_CVM,			/**持卡人信息验证*/
	TRANS_SALE_CRDR,            /**借贷记消费*/
	TRANS_COUPON_VERIFY,	    /**<串码验券*/
	TRANS_TEL_VERIFY,	        /**<手机验券*/
	TRANS_CARD_VERIFY,	        /**<卡号验券*/
	TRANS_VOID_VERIFY,			/**<撤销交易*/
	TRANS_VOID_COUPON_VERIFY,	/**<撤销串码验券*/
	TRANS_VOID_TEL_VERIFY,		/**<撤销手机验券*/
	TRANS_VOID_CARD_VERIFY,		/**<撤销银行卡验券*/
	TRANS_REFUND_VERIFY,		/**<验证退货*/
	TRANS_QUERY,			    /**< 收单查询*/
	TRANS_PRECREATE,		    /**< 微信扫描支付*/
	TRANS_CREATEANDPAY,		    /**<微信条码支付*/
	TRANS_VOID_PRECREATE,		/**< 微信扫描支付撤销*/
	TRANS_VOID_CREATEANDPAY,	/**< 微信条码支付撤销*/
	TRANS_WX_REFUND,			/**<微信退货*/
	TRANS_CREATEANDPAYBAIDU,    /**<百度 条码支付*/
	TRANS_VOID_CREATEANDPAYBAIDU,    /**<百度 条码支付撤销*/
	TRANS_BAIDU_REFUND,			/**<百度支付退货*/
	TRANS_JD_PRECREATE,			/**<京东扫码支付*/
	TRANS_JD_CREATEANDPAY,      /**<京东主扫(POS扫)*/
	TRANS_JD_REFUND,			/**<京东退货*/
	TRANS_VOID_JD_PRECREATE,    /**<京东撤销*/
	TRANS_VOID_JD_CREATEANDPAY, /**<京东撤销*/
	TRANS_ALI_CREATEANDPAY,     /**<支付宝付款*/
	TRANS_ALI_REFUND,			/**<支付宝退货*/
	TRANS_VOID_ALI_CREATEANDPAY,/**<支付宝撤销*/
	TRANS_COUPON_MEITUAN,	    /**<美团验券*/
	TRANS_VOID_MEITUAN,	        /**<美团撤销*/
	TRANS_PANJINTONG,     		/**<盘锦通支付*/
	TRANS_BALANCE_WECHAT,		/**< 查询*/
	TRANS_BALANCE_BAIDU,		/**< 查询*/
	TRANS_BALANCE_ALIPAY,		/**< 查询*/
	TRANS_BALANCE_JD,			/**< 查询*/
	TRANS_BALANCE_MEITUAN,		/**< 查询*/
	TRANS_BALANCE_DZ,      		/**< 大众点评查询*/
	TRANS_BALANCE_BESTPAY,		/**<翼支付 查询*/
	TRANS_BALANCE_ALLPAY,      	/**<都能付查询*/
	TRANS_COUPON_DAZHONG,	    /**<大众点评验券*/
	TRANS_COUPONFRM_DAZHONG,	/**<大众点评订单确认*/
	TRANS_DZ_PRECREATE,			/**<大众点评(APP扫)*/
	TRANS_DZ_CREATEANDPAY,      /**<大众点评主扫(POS扫)*/
	TRANS_DZ_REFUND,			/**<大众点评退货*/
	TRANS_VOID_DZ_PRECREATE,    /**<大众点评撤销(APP扫)*/
	TRANS_VOID_DZ_CREATEANDPAY, /**<大众点评撤销(POS扫)*/
	TRANS_BESTPAY_CREATEANDPAY, /**<翼支付付款*/
	TRANS_BESTPAY_REFUND,			/**<翼支付退货*/
	TRANS_VOID_BESTPAY_CREATEANDPAY,/**<翼支付撤销*/
	
	TRANS_BAIDU_PRECREATE,			/**<百度被扫*/
	TRANS_VOID_BAIDU_PRECREATE,		/**< 百度钱包被扫撤销*/
	TRANS_ALI_PRECREATE,			/**<支付宝被扫*/
	TRANS_VOID_ALI_PRECREATE,		/**<支付宝被扫撤销*/
	
	TRANS_ALLPAY_CREATEANDPAY,      /**<都能付主扫*/
	TRANS_ALLPAY_PRECREATE,      	/**<都能付被扫*/
	TRANS_ALLPAY_REFUND,      		/**<都能付退货*/
	
};

enum TXNATTR
{
	ATTR_MAG = 0x01,			/**< 磁条卡交易*/
	ATTR_PBOC,					/**< 磁条卡PBOC电子钱包电子存折交易*/
	ATTR_EMV_STANDARD,			/**< 基于emv的借贷记标准流程交易*/
	ATTR_EMV_PREDIGEST,			/**< emv简化流程交易*/
	ATTR_FALLBACK,				/**< FALLBACK磁条卡交易*/
	ATTR_qPBOC,					/**< qPBOC交易*/
	ATTR_PBOC_EC,				/**< 电子现金交易，接触式*/
	ATTR_PBOC_EC_RF,			/**< 电子现金交易，非接触式*/
	ATTR_EMV_STANDARD_RF,		/**< 基于emv的借贷记标准流程交易，非接触式*/
	ATTR_EMV_PREDIGEST_RF,		/**< emv简化流程交易，非接触式*/
	ATTR_APPOINTMENT,			/**< 手机预约交易*/
	ATTR_UPCARD,				/**< 手机芯片交易*/
	ATTR_qPBOC_UPCARD           /**< qPBOC交易和手机芯片交易*/
};

enum BONUSTYPE
{
	BONUS_IIS = 0x01,			/**< 发卡行积分*/
	BONUS_ALLIANCE			/**< 联盟积分*/
};


#if 0
typedef struct
{
	char sIisCode[11];			/**<发卡行标识码,接收机构标识码(返回包时用)  */
	char sAcqCode[11];			/**<收单机构标识码,商户结算行标识码(返回包时用)  */
	char sRfu[6];					/**<保留字节*/
}STFIELD44;

typedef struct
{
	char sFuncCode[2];			/**<60.1 消息类型码*/
	char sBatchNum[6];			/**<60.2 批次号*/
	char sNetManCode[3];		/**<60.3 网络管理信息*/
	char cTermCapacity;			/**<60.4 终端处理能力*/
	char cIcConditionCode;		/**<60.5 基于PBOC借/贷记标准的IC卡条件代码*/
	char cPartSaleflag;			/**<60.6 支持部分扣款和返回余额标志*/
	char sAccountType[3];		/**<60.7 账户类型*/
	char cRfu;
}STFIELD60;

typedef struct
{
	char sOldBatch[6];			/**<61.1 原始交易批次号N6*/
	char sOldTrace[6];			/**<61.2 原始交易POS流水号		N6*/
	char sOldDate[4];				/**<61.3 原始交易日期			N4*/
	char sOldAuthWay[2];			/**<61.4 原交易授权方式			N2*/
	char sOldAuthOrgCode[11];	/**<61.5 原交易授权机构代码		N11*/
	char sRfu[3];					/**<预留信息*/
}STFIELD61;

typedef struct
{
	char sField63_1[3];			/**<63.1*/
	char sIisInfo[20];				/**<63.2.1 发卡行信息 */
	char sCUP_Info[20];			/**<63.2.2 中国银联信息 */
	char sMerAcqInfo[20];			/**<63.2.3 商户收单信息*/
	char sTermInfo[60];			/**<63.2.4 POS终端特殊交易信息*/
	char sRfu[3];
}STFIELD63;
#endif

/**
* @struct STSYSTEM交易数据结构
*/
typedef struct
{
	/**
	* 以下为标准交易数据元内容
	*/
	char cTransType;				/**<交易类型*/
	char cTransAttr;				/**<交易属性*/
	char cEMV_Status;
	char szTransCode[6+1];			/**<交易类型*/
	char szDate[8+1];				/**<交易日期yyyymmdd、yymmdd、(本系统使用yyyymmdd ) */
	char szTime[6+1];				/**<交易时间hhmmss */
	char szTrace[6+1];				/**<POS流水号*/
	char szBatchNo[6+1];			/**<批次号*/
	char szInvoiceNo[6+1];			/**<票据号*/
	char szPosID[8+1];				/**<终端号 */
	char szShopID[15+1];			/**<商户号 */
	char szRefnum[12+1];			/**<系统参考号(返回包时用)  */
	char szPan[19+1];				/**<主帐号*/
	char szExpDate[4+1];			/**<有效期*/
	char szTrack2[37+1];			/**<二道数据*/
	char szTrack3[104+1];			/**<三道数据*/
	char szTelNo[11+1];				/**<手机号*/
	char szCouponID[20+1];			/**<卡券码*/
	char szInputMode[3+1];			/**<输入模式*/
	char szPin[8+1];				/**<个人密码(密文)*/
	char szMsmCode[20+1];			/**<短信验证码*/
	char szField55[255+1];			/**<IC55*/
	char szField56[255+1];		/**<IC55*/
	char szOldTraceNo[6+1];		/**<原交易流失号*/
	char szOldBatchNo[6+1];		/**<原交易批次号*/
	char szOldVoiceNo[6+1];		/**<原交易票据号*/
	char szOldRefnum[12+1];		/**<原交易系统参考号(返回包时用)  */
	char szOldDate[8+1];		/**<原交易日期*/
	char szOldTime[6+1];		/**<原交易时间*/
	char cOldTransType;			/**<原交易类型*/
	char szOldTransCode[6+1];	/**<原交易类型*/
	char szResponse[2+1];		/**<响应码(返回包时用)  */
	char szRespInfo[100+1];		/**<响应信息*/
	char szTag28[100];			/**<TAG28*/
	char szAmount[12+1];		/**<金额(BCD)*/
	char szTag30[100+1];		/**<TAG30*/
	char szTag31[20+1];			/**<TAG31*/
	char szTag32[1024+1];		/**<TAG32*/
	char szRealAmount[12+1];	/**<实扣金额*/
	char szPayableAmount[12+1];	/**<应付金额*/
	char szMac[8+1];
	char szOperNow[2+1];		/**<63.1要求上送的是3位的,操作员号码*/
	char szOutTradeNo[50+1];    /**<商户订单号*/
	char szCodeUrl[200+1];		/**<客户URL */
	char szPosInfo[100];		/**<POS信息*/
	char szAdvertisement[256];	/**<广告内容*/

	/**
	* 以下为附加的交易数据元内容
	*/
	char *psAddField;			/**<附加域指针如55域内容,空间由具体应用里开辟*/
	int nAddFieldLen;				/**<附加域数据长度*/
	
}STSYSTEM;


typedef struct
{
	unsigned char ucCouponISS;		/*发券方标识*/
	unsigned char ucCouponType;		/*券类型*/
	unsigned char usCouponID[20];	/*券码*/
	unsigned char usAmount[5];		/*金额*/
	unsigned char ucPinMode;		/*验密模式*/
	unsigned char usCouponName[20];	/*券名称*/
}STCOUPON;

typedef struct
{
	unsigned char usCouponID[20];	/*券码*/
	unsigned char usAmount[6];		/*金额*/
	unsigned char usCouponName[20];	/*券名称*/
}STCOUPONMEITUAN;


/**
* @struct STREVERSAL冲正数据结构
*/
typedef struct
{
	char cTransType;				/**<交易类型*/
	char cTransAttr;				/**<交易属性*/
	char cEMV_Status;				/**<EMV交易的执行状态*/
	char szTransCode[6+1];			/**<交易类型*/
	char szDate[8+1];				/**<交易日期yyyymmdd、yymmdd、(本系统使用yyyymmdd ) */
	char szTime[6+1];				/**<交易时间hhmmss */
	char szTrace[6+1];				/**<POS流水号*/
	char szBatchNo[6+1];			/**<批次号*/
	char szInvoiceNo[6+1];			/**<票据号*/
	char szPan[19+1];				/**<主帐号*/
	char szExpDate[4+1];			/**<有效期*/
	char szTelNo[11+1];				/**<手机号*/
	char szCouponID[20+1];			/**<卡券码*/
	char szInputMode[3+1];			/**<输入模式*/
	char szAmount[12+1];			/**<金额(BCD)*/
	char szOperNow[2+1];			/**<63.1要求上送的是3位的,操作员号码*/
	char szOutTradeNo[50+1];		/**<商户订单号*/
	char szFieldAdd1[256];			/**<自定义域1数据*/
	int nFieldAdd1Len;				/**<自定义域1数据长度*/
	char szFieldAdd2[100];			/**<自定义域2数据*/
	int nFieldAdd2Len;				/**<自定义域2数据长度*/
}STREVERSAL;
#if 0

typedef struct
{
	char szInstallmentNum[2+1];	/**<分期付款消费付款期数*/
	char cType;					/**<分期付款手续费支付方式*/
	char szFirstAmt[12+1];		/**<分期付款持卡人首期付款金额*/
	char szCurrency[3+1];		/**<货币代码*/
	char szFee[12+1];			/**<分期付款一次性支付持卡人手续费*/
	char szBonus[12+1];			/**<奖励积分,只对一次性支付有效*/
	char szFirstFee[12+1];		/**<分期付款分期支付首期手续费*/
	char szNormalFee[12+1];		/**<分期付款分期支付每期手续费*/
	char sRfu[13];
}STINSTALLMENT;

typedef struct
{
	char szCode[30+1];			/**<积分消费商品代码*/
	char szBonusPoint[10+1];	/**<积分消费分数*/
	char szSelfAmt[12+1];		/**<自付金额*/
	char szBonusBalance[12+1];	/**<积分余额*/
	char sRfu[5];
}STBONUS;

#endif

extern int MagCardVeriCoupon(void);
extern int MagIDVeriCoupon(void);
extern int MagTelnoVeriCoupon(void);
extern int MagScanQrCodePay(void);
extern int MagBarcodePay(void);
extern int MagBarCodeRefund(void);
extern int VoidSale(void);
extern int MagRefund(void);
extern int GetCouponIss(char *pszIssName, const char cIss);
extern int GetCouponType(char *pszTypeName,char *pszTypeNameShort, const char cType);
extern int AutoDoReversal(void);
extern int MagBarcodeBaidu(void);
extern int MagBarCodeRefundBaidu(void);
extern int MagJDScanQrCodePay(void);
extern int MagJDBarcode(void);

extern int MagBarJDCodeRefund(void);
extern int MagAliBarcode(void);
extern int MagBarAliCodeRefund(void);
extern int MagMeituanVeriCoupon(void);
extern int MagPanjintong(void);
extern int MagBalance(char cTransType);

extern int DoReversal(void);



#endif

