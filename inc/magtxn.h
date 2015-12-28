/**
* @file magtxn.h
* @brief ���������״���ģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/

#ifndef _MAGTXN_H_
#define _MAGTXN_H_ 

#define STRIPE_TIMEOUT 60			/**<ˢ����ʱʱ��*/
#define DISPPAN_TIMEOUT 30			/**<����ȷ�ϳ�ʱʱ��*/
#define INPUT_AMOUNT_TIMEOUT 60	/**<�������ȷ�ϳ�ʱʱ��*/
#define INPUT_DATE_TIMEOUT 60		/**<�������ڳ�ʱʱ��*/
#define INPUT_STRING_TIMEOUT 60	/**<�����ַ���ʱʱ��*/

#if defined (SUPPORT_ELECSIGN)
#define MAX_PACK_SIZE 2000			/**<��������*/
#else
#define MAX_PACK_SIZE 1000			/**<��������*/
#endif

#define CURRENCY_CODE_CN "156"		/**< ���׻��Ҵ�(Currency Code Of Transaction)*/

enum TRANSTYPE
{
	TRANS_BALANCE,				/**< ��ѯ*/
	TRANS_SALE,					/**< ����*/
	TRANS_AUTHSALE,				/**< ��Ȩ�������*/
	TRANS_AUTHSALEOFF,			/**< ��Ȩ���֪ͨ*/
	TRANS_AUTHSETTLE,			/**< ��Ȩ����*/
	TRANS_PREAUTH,				/**< Ԥ��Ȩ*/
	TRANS_ADDTO_PREAUTH,		/**< ׷��Ԥ��Ȩ*/
	TRANS_REFUND,				/**< �˻�*/
	TRANS_VOID_SALE,			/**< ���ѳ���*/
	TRANS_VOID_AUTHSALE,		/**< ��ɳ���*/
	TRANS_VOID_AUTHSETTLE,		/**< ���㳷��*/
	TRANS_VOID_PREAUTH,			/**< ��Ȩ����*/
	TRANS_VOID_REFUND,			/**< �����˻�*/
	TRANS_OFFLINE,				/**< ���߽���*/
	TRANS_ADJUST,				/**< �������*/
	TRANS_INSTALMENT,			/**< ���ڸ���*/
	TRANS_VOID_INSTALMENT,		/**< ��������*/
	TRANS_EMV_SCRIPE,			/**< EMV�ű����֪ͨ*/
	TRANS_EMV_REFUND,			/**< EMV�ѻ��˻�*/
	TRANS_BONUS_IIS_SALE,		/**< �����л�������*/
	TRANS_VOID_BONUS_IIS_SALE,	/**< ���������л�������*/
	TRANS_BONUS_ALLIANCE,		/**< ���˻�������*/
	TRANS_VOID_BONUS_ALLIANCE,	/**< �������˻�������*/
	TRANS_ALLIANCE_BALANCE,		/**< ���˻��ֲ�ѯ*/
	TRANS_ALLIANCE_REFUND,		/**< ���˻����˻�*/
	TRANS_PHONE_SALE,			/**< �ֻ�оƬ����*/
	TRANS_VOID_PHONE_SALE,		/**< �����ֻ�оƬ����*/
	TRANS_REFUND_PHONE_SALE,	/**< �ֻ�оƬ�˻�*/
	TRANS_PHONE_PREAUTH,		/**< �ֻ�оƬԤ��Ȩ*/	
	TRANS_VOID_PHONE_PREAUTH,	/**< �����ֻ�оƬԤ��Ȩ*/
	TRANS_PHONE_AUTHSALE,		/**< �ֻ�оƬԤ��Ȩ���*/
	TRANS_PHONE_AUTHSALEOFF,	/**< �ֻ�оƬ���֪ͨ*/
	TRANS_VOID_PHONE_AUTHSALE,	/**< �����ֻ��������*/
	TRANS_PHONE_BALANCE,		/**< �ֻ�оƬ����ѯ*/
	TRANS_ORDER_CVM,			/**�ֿ�����Ϣ��֤*/
	TRANS_SALE_CRDR,            /**���������*/
	TRANS_COUPON_VERIFY,	    /**<������ȯ*/
	TRANS_TEL_VERIFY,	        /**<�ֻ���ȯ*/
	TRANS_CARD_VERIFY,	        /**<������ȯ*/
	TRANS_VOID_VERIFY,			/**<��������*/
	TRANS_VOID_COUPON_VERIFY,	/**<����������ȯ*/
	TRANS_VOID_TEL_VERIFY,		/**<�����ֻ���ȯ*/
	TRANS_VOID_CARD_VERIFY,		/**<�������п���ȯ*/
	TRANS_REFUND_VERIFY,		/**<��֤�˻�*/
	TRANS_QUERY,			    /**< �յ���ѯ*/
	TRANS_PRECREATE,		    /**< ΢��ɨ��֧��*/
	TRANS_CREATEANDPAY,		    /**<΢������֧��*/
	TRANS_VOID_PRECREATE,		/**< ΢��ɨ��֧������*/
	TRANS_VOID_CREATEANDPAY,	/**< ΢������֧������*/
	TRANS_WX_REFUND,			/**<΢���˻�*/
	TRANS_CREATEANDPAYBAIDU,    /**<�ٶ� ����֧��*/
	TRANS_VOID_CREATEANDPAYBAIDU,    /**<�ٶ� ����֧������*/
	TRANS_BAIDU_REFUND,			/**<�ٶ�֧���˻�*/
	TRANS_JD_PRECREATE,			/**<����ɨ��֧��*/
	TRANS_JD_CREATEANDPAY,      /**<������ɨ(POSɨ)*/
	TRANS_JD_REFUND,			/**<�����˻�*/
	TRANS_VOID_JD_PRECREATE,    /**<��������*/
	TRANS_VOID_JD_CREATEANDPAY, /**<��������*/
	TRANS_ALI_CREATEANDPAY,     /**<֧��������*/
	TRANS_ALI_REFUND,			/**<֧�����˻�*/
	TRANS_VOID_ALI_CREATEANDPAY,/**<֧��������*/
	TRANS_COUPON_MEITUAN,	    /**<������ȯ*/
	TRANS_VOID_MEITUAN,	        /**<���ų���*/
	TRANS_PANJINTONG,     		/**<�̽�֧ͨ��*/
	TRANS_BALANCE_WECHAT,		/**< ��ѯ*/
	TRANS_BALANCE_BAIDU,		/**< ��ѯ*/
	TRANS_BALANCE_ALIPAY,		/**< ��ѯ*/
	TRANS_BALANCE_JD,			/**< ��ѯ*/
	TRANS_BALANCE_MEITUAN,		/**< ��ѯ*/
	TRANS_BALANCE_DZ,      		/**< ���ڵ�����ѯ*/
	TRANS_BALANCE_BESTPAY,		/**<��֧�� ��ѯ*/
	TRANS_BALANCE_ALLPAY,      	/**<���ܸ���ѯ*/
	TRANS_COUPON_DAZHONG,	    /**<���ڵ�����ȯ*/
	TRANS_COUPONFRM_DAZHONG,	/**<���ڵ�������ȷ��*/
	TRANS_DZ_PRECREATE,			/**<���ڵ���(APPɨ)*/
	TRANS_DZ_CREATEANDPAY,      /**<���ڵ�����ɨ(POSɨ)*/
	TRANS_DZ_REFUND,			/**<���ڵ����˻�*/
	TRANS_VOID_DZ_PRECREATE,    /**<���ڵ�������(APPɨ)*/
	TRANS_VOID_DZ_CREATEANDPAY, /**<���ڵ�������(POSɨ)*/
	TRANS_BESTPAY_CREATEANDPAY, /**<��֧������*/
	TRANS_BESTPAY_REFUND,			/**<��֧���˻�*/
	TRANS_VOID_BESTPAY_CREATEANDPAY,/**<��֧������*/
	
	TRANS_BAIDU_PRECREATE,			/**<�ٶȱ�ɨ*/
	TRANS_VOID_BAIDU_PRECREATE,		/**< �ٶ�Ǯ����ɨ����*/
	TRANS_ALI_PRECREATE,			/**<֧������ɨ*/
	TRANS_VOID_ALI_PRECREATE,		/**<֧������ɨ����*/
	
	TRANS_ALLPAY_CREATEANDPAY,      /**<���ܸ���ɨ*/
	TRANS_ALLPAY_PRECREATE,      	/**<���ܸ���ɨ*/
	TRANS_ALLPAY_REFUND,      		/**<���ܸ��˻�*/
	
};

enum TXNATTR
{
	ATTR_MAG = 0x01,			/**< ����������*/
	ATTR_PBOC,					/**< ������PBOC����Ǯ�����Ӵ��۽���*/
	ATTR_EMV_STANDARD,			/**< ����emv�Ľ���Ǳ�׼���̽���*/
	ATTR_EMV_PREDIGEST,			/**< emv�����̽���*/
	ATTR_FALLBACK,				/**< FALLBACK����������*/
	ATTR_qPBOC,					/**< qPBOC����*/
	ATTR_PBOC_EC,				/**< �����ֽ��ף��Ӵ�ʽ*/
	ATTR_PBOC_EC_RF,			/**< �����ֽ��ף��ǽӴ�ʽ*/
	ATTR_EMV_STANDARD_RF,		/**< ����emv�Ľ���Ǳ�׼���̽��ף��ǽӴ�ʽ*/
	ATTR_EMV_PREDIGEST_RF,		/**< emv�����̽��ף��ǽӴ�ʽ*/
	ATTR_APPOINTMENT,			/**< �ֻ�ԤԼ����*/
	ATTR_UPCARD,				/**< �ֻ�оƬ����*/
	ATTR_qPBOC_UPCARD           /**< qPBOC���׺��ֻ�оƬ����*/
};

enum BONUSTYPE
{
	BONUS_IIS = 0x01,			/**< �����л���*/
	BONUS_ALLIANCE			/**< ���˻���*/
};


#if 0
typedef struct
{
	char sIisCode[11];			/**<�����б�ʶ��,���ջ�����ʶ��(���ذ�ʱ��)  */
	char sAcqCode[11];			/**<�յ�������ʶ��,�̻������б�ʶ��(���ذ�ʱ��)  */
	char sRfu[6];					/**<�����ֽ�*/
}STFIELD44;

typedef struct
{
	char sFuncCode[2];			/**<60.1 ��Ϣ������*/
	char sBatchNum[6];			/**<60.2 ���κ�*/
	char sNetManCode[3];		/**<60.3 ���������Ϣ*/
	char cTermCapacity;			/**<60.4 �ն˴�������*/
	char cIcConditionCode;		/**<60.5 ����PBOC��/���Ǳ�׼��IC����������*/
	char cPartSaleflag;			/**<60.6 ֧�ֲ��ֿۿ�ͷ�������־*/
	char sAccountType[3];		/**<60.7 �˻�����*/
	char cRfu;
}STFIELD60;

typedef struct
{
	char sOldBatch[6];			/**<61.1 ԭʼ�������κ�N6*/
	char sOldTrace[6];			/**<61.2 ԭʼ����POS��ˮ��		N6*/
	char sOldDate[4];				/**<61.3 ԭʼ��������			N4*/
	char sOldAuthWay[2];			/**<61.4 ԭ������Ȩ��ʽ			N2*/
	char sOldAuthOrgCode[11];	/**<61.5 ԭ������Ȩ��������		N11*/
	char sRfu[3];					/**<Ԥ����Ϣ*/
}STFIELD61;

typedef struct
{
	char sField63_1[3];			/**<63.1*/
	char sIisInfo[20];				/**<63.2.1 ��������Ϣ */
	char sCUP_Info[20];			/**<63.2.2 �й�������Ϣ */
	char sMerAcqInfo[20];			/**<63.2.3 �̻��յ���Ϣ*/
	char sTermInfo[60];			/**<63.2.4 POS�ն����⽻����Ϣ*/
	char sRfu[3];
}STFIELD63;
#endif

/**
* @struct STSYSTEM�������ݽṹ
*/
typedef struct
{
	/**
	* ����Ϊ��׼��������Ԫ����
	*/
	char cTransType;				/**<��������*/
	char cTransAttr;				/**<��������*/
	char cEMV_Status;
	char szTransCode[6+1];			/**<��������*/
	char szDate[8+1];				/**<��������yyyymmdd��yymmdd��(��ϵͳʹ��yyyymmdd ) */
	char szTime[6+1];				/**<����ʱ��hhmmss */
	char szTrace[6+1];				/**<POS��ˮ��*/
	char szBatchNo[6+1];			/**<���κ�*/
	char szInvoiceNo[6+1];			/**<Ʊ�ݺ�*/
	char szPosID[8+1];				/**<�ն˺� */
	char szShopID[15+1];			/**<�̻��� */
	char szRefnum[12+1];			/**<ϵͳ�ο���(���ذ�ʱ��)  */
	char szPan[19+1];				/**<���ʺ�*/
	char szExpDate[4+1];			/**<��Ч��*/
	char szTrack2[37+1];			/**<��������*/
	char szTrack3[104+1];			/**<��������*/
	char szTelNo[11+1];				/**<�ֻ���*/
	char szCouponID[20+1];			/**<��ȯ��*/
	char szInputMode[3+1];			/**<����ģʽ*/
	char szPin[8+1];				/**<��������(����)*/
	char szMsmCode[20+1];			/**<������֤��*/
	char szField55[255+1];			/**<IC55*/
	char szField56[255+1];		/**<IC55*/
	char szOldTraceNo[6+1];		/**<ԭ������ʧ��*/
	char szOldBatchNo[6+1];		/**<ԭ�������κ�*/
	char szOldVoiceNo[6+1];		/**<ԭ����Ʊ�ݺ�*/
	char szOldRefnum[12+1];		/**<ԭ����ϵͳ�ο���(���ذ�ʱ��)  */
	char szOldDate[8+1];		/**<ԭ��������*/
	char szOldTime[6+1];		/**<ԭ����ʱ��*/
	char cOldTransType;			/**<ԭ��������*/
	char szOldTransCode[6+1];	/**<ԭ��������*/
	char szResponse[2+1];		/**<��Ӧ��(���ذ�ʱ��)  */
	char szRespInfo[100+1];		/**<��Ӧ��Ϣ*/
	char szTag28[100];			/**<TAG28*/
	char szAmount[12+1];		/**<���(BCD)*/
	char szTag30[100+1];		/**<TAG30*/
	char szTag31[20+1];			/**<TAG31*/
	char szTag32[1024+1];		/**<TAG32*/
	char szRealAmount[12+1];	/**<ʵ�۽��*/
	char szPayableAmount[12+1];	/**<Ӧ�����*/
	char szMac[8+1];
	char szOperNow[2+1];		/**<63.1Ҫ�����͵���3λ��,����Ա����*/
	char szOutTradeNo[50+1];    /**<�̻�������*/
	char szCodeUrl[200+1];		/**<�ͻ�URL */
	char szPosInfo[100];		/**<POS��Ϣ*/
	char szAdvertisement[256];	/**<�������*/

	/**
	* ����Ϊ���ӵĽ�������Ԫ����
	*/
	char *psAddField;			/**<������ָ����55������,�ռ��ɾ���Ӧ���￪��*/
	int nAddFieldLen;				/**<���������ݳ���*/
	
}STSYSTEM;


typedef struct
{
	unsigned char ucCouponISS;		/*��ȯ����ʶ*/
	unsigned char ucCouponType;		/*ȯ����*/
	unsigned char usCouponID[20];	/*ȯ��*/
	unsigned char usAmount[5];		/*���*/
	unsigned char ucPinMode;		/*����ģʽ*/
	unsigned char usCouponName[20];	/*ȯ����*/
}STCOUPON;

typedef struct
{
	unsigned char usCouponID[20];	/*ȯ��*/
	unsigned char usAmount[6];		/*���*/
	unsigned char usCouponName[20];	/*ȯ����*/
}STCOUPONMEITUAN;


/**
* @struct STREVERSAL�������ݽṹ
*/
typedef struct
{
	char cTransType;				/**<��������*/
	char cTransAttr;				/**<��������*/
	char cEMV_Status;				/**<EMV���׵�ִ��״̬*/
	char szTransCode[6+1];			/**<��������*/
	char szDate[8+1];				/**<��������yyyymmdd��yymmdd��(��ϵͳʹ��yyyymmdd ) */
	char szTime[6+1];				/**<����ʱ��hhmmss */
	char szTrace[6+1];				/**<POS��ˮ��*/
	char szBatchNo[6+1];			/**<���κ�*/
	char szInvoiceNo[6+1];			/**<Ʊ�ݺ�*/
	char szPan[19+1];				/**<���ʺ�*/
	char szExpDate[4+1];			/**<��Ч��*/
	char szTelNo[11+1];				/**<�ֻ���*/
	char szCouponID[20+1];			/**<��ȯ��*/
	char szInputMode[3+1];			/**<����ģʽ*/
	char szAmount[12+1];			/**<���(BCD)*/
	char szOperNow[2+1];			/**<63.1Ҫ�����͵���3λ��,����Ա����*/
	char szOutTradeNo[50+1];		/**<�̻�������*/
	char szFieldAdd1[256];			/**<�Զ�����1����*/
	int nFieldAdd1Len;				/**<�Զ�����1���ݳ���*/
	char szFieldAdd2[100];			/**<�Զ�����2����*/
	int nFieldAdd2Len;				/**<�Զ�����2���ݳ���*/
}STREVERSAL;
#if 0

typedef struct
{
	char szInstallmentNum[2+1];	/**<���ڸ������Ѹ�������*/
	char cType;					/**<���ڸ���������֧����ʽ*/
	char szFirstAmt[12+1];		/**<���ڸ���ֿ������ڸ�����*/
	char szCurrency[3+1];		/**<���Ҵ���*/
	char szFee[12+1];			/**<���ڸ���һ����֧���ֿ���������*/
	char szBonus[12+1];			/**<��������,ֻ��һ����֧����Ч*/
	char szFirstFee[12+1];		/**<���ڸ������֧������������*/
	char szNormalFee[12+1];		/**<���ڸ������֧��ÿ��������*/
	char sRfu[13];
}STINSTALLMENT;

typedef struct
{
	char szCode[30+1];			/**<����������Ʒ����*/
	char szBonusPoint[10+1];	/**<�������ѷ���*/
	char szSelfAmt[12+1];		/**<�Ը����*/
	char szBonusBalance[12+1];	/**<�������*/
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

