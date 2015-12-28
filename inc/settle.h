/**
* @file settle.h
* @brief ���������ģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/

#ifndef _SETTLE_H_
#define _SETTLE_H_

#include "magtxn.h"
#include "water.h"

typedef unsigned long SETTLE_NUM;
typedef union 
{
	unsigned long long ullSettleAmt;
	uchar sSettleAmt[8]; /*ֻ�õ�ǰ6λ���ڴ���ѹ������Ľ��*/
}SETTLE_AMT;

/**
* @struct STSETTLE�������ݽṹ
*/
//Ŀǰ����336�������⿨�Ĳ���
typedef struct tagSettle
{
	SETTLE_NUM _SaleNum ;				/**<���ѱ��� */
	SETTLE_AMT _SaleAmount ;			/**<���ѽ��*/

	SETTLE_NUM _VoidSaleNum ;			/**<���ѳ�������*/
	SETTLE_AMT _VoidSaleAmount ;		/**<���ѳ������*/

	SETTLE_NUM _AuthSaleNum ;			/**<��Ȩ��ɱ���(����)*/
	SETTLE_AMT _AuthSaleAmount;			/**<��Ȩ��ɽ��(����)*/

	SETTLE_NUM _VoidAuthSaleNum ;		/**<������Ȩ��ɱ���(����)*/
	SETTLE_AMT _VoidAuthSaleAmount ;	/**<������Ȩ��ɽ��(����)*/

	SETTLE_NUM _RefundNum;				/**<�˻�����*/
	SETTLE_AMT _RefundAmount; 			/**<�˻����*/

	SETTLE_NUM _CouponVerifyNum;		/**< ������֤����*/
	SETTLE_AMT _CouponVerifyAmount;		/**< ������֤���*/

	SETTLE_NUM _TelVerifyNum;			/**< �ֻ�����֤����*/
	SETTLE_AMT _TelVerifyAmount;		/**< �ֻ���֤���*/

	SETTLE_NUM _CardVerifyNum;			/**< ���п���֤����*/
	SETTLE_AMT _CardVerifyAmount;		/**< ���п���֤���*/

	SETTLE_NUM _VoidCouponVerifyNum;	/**< ������֤��������*/
	SETTLE_AMT _VoidCouponVerifyAmount;	/**< ������֤�������*/

	SETTLE_NUM _VoidTelVerifyNum;		/**< �ֻ�����֤��������*/
	SETTLE_AMT _VoidTelVerifyAmount;	/**< �ֻ���֤�������*/

	SETTLE_NUM _VoidCardVerifyNum;		/**< ���п���֤��������*/
	SETTLE_AMT _VodiCardVerifyAmount;	/**< ���п���֤�������*/

	SETTLE_NUM _RefundVerifyNum;		/**< ��֤�˻�����*/
	SETTLE_AMT _RefundVerifyAmount;		/**< ��֤�˻����*/	

	SETTLE_NUM _RefundWechatNum;		/**< ΢���˻�����*/
	SETTLE_AMT _RefundWeChatAmount;		/**< ΢���˻����*/	

	SETTLE_NUM _PrecreateNum;			/**< ɨ��֧������*/
	SETTLE_AMT _PrecreateAmount;		/**< ɨ��֧�����*/	

	SETTLE_NUM _CreatepayNum;			/**< ����֧������*/
	SETTLE_AMT _CreatepayAmount;		/**< ����֧�����*/	

	SETTLE_NUM _VoidPrecreateNum;		/**< ɨ��֧����������*/
	SETTLE_AMT _VoidPrecreateAmount;	/**< ɨ��֧���������*/	

	SETTLE_NUM _VoidCreatepayNum;		/**< ����֧����������*/
	SETTLE_AMT _VoidCreatepayAmount;	/**< ����֧���������*/		

	SETTLE_NUM _BaiduCreatepayNum;		/**<�ٶ�����֧������*/
	SETTLE_AMT _BaiduCreatepayAmount;	/**<�ٶ�����֧�����*/	

	SETTLE_NUM _VoidBaiduCreatepayNum;		/**< �ٶ�����֧����������*/
	SETTLE_AMT _VoidBaiduCreatepayAmount;	/**< �ٶ�����֧���������*/	

	
	SETTLE_NUM _BaiduPrecreteNum;		/**<�ٶȱ�ɨ֧������*/
	SETTLE_AMT _BaiduPrecreteAmount;		/**<�ٶȱ�ɨ֧�����*/	

	SETTLE_NUM _VoidBaiduPrecreteNum;	/**< �ٶȱ�ɨ֧����������*/
	SETTLE_AMT _VoidBaiduPrecreteAmount;	/**< �ٶȱ�ɨ֧���������*/	

	SETTLE_NUM _RefundBaiduNum;			/**<�ٶ��˻�����*/
	SETTLE_AMT _RefundBaiduAmount;		/**<�ٶ��˻����*/	

	SETTLE_NUM _JDPrecreateNum;			/**< ����ɨ��֧������*/
	SETTLE_AMT _JDPrecreateAmount;		/**< ����ɨ��֧�����*/	

	SETTLE_NUM _VoidJDPrecreateNum;		/**< ����ɨ��֧����������*/
	SETTLE_AMT _VoidJDPrecreateAmount;	/**< ����ɨ��֧���������*/

	SETTLE_NUM _JDRefundNum;			/**<�����˻�����*/
	SETTLE_AMT _JDRefundAmount;			/**<�����˻����*/	

	SETTLE_NUM _AliCreatepayNum;		/**<֧��������֧������*/
	SETTLE_AMT _AliCreatepayAmount;		/**<֧��������֧�����*/	

	SETTLE_NUM _VoidAliCreatepayNum;	/**< ֧�������볷������*/
	SETTLE_AMT _VoidAliCreatepayAmount;	/**< ֧�������볷�����*/

	SETTLE_NUM _AliRefundNum;			/**<֧�����˻�����*/
	SETTLE_AMT _AliRefundAmount;		/**<֧�����˻����*/

	SETTLE_NUM _AliPrecreteNum;			/**<֧������ɨ����*/
	SETTLE_AMT _AliPrecreteAmount;		/**<֧������ɨ���*/

	SETTLE_NUM _VoidAliPrecreteNum;			/**<֧������ɨ��������*/
	SETTLE_AMT _VoidAliPrecreteAmount;		/**<֧������ɨ�������*/
	

	SETTLE_NUM _MeituanNum;				/**<���ű���*/
	SETTLE_AMT _MeituanAmount;			/**<���Ž��*/	

	SETTLE_NUM _PanjintongNum;			/**<�̽�ͨ����*/
	SETTLE_AMT _PanjintongAmount; 		/**<�̽�ͨ���*/

	SETTLE_NUM _BestpayCreatepayNum;		/**<��֧����ɨ����*/
	SETTLE_AMT _BestpayCreatepayAmount;		/**<��֧����ɨ���*/	

	SETTLE_NUM _VoidBestpayCreatepayNum;	/**<��֧����ɨ��������*/
	SETTLE_AMT _VoidBestpayCreatepayAmount;	/**<��֧����ɨ�������*/

	SETTLE_NUM _BestpayRefundNum;			/**<��֧����ɨ�˻�����*/
	SETTLE_AMT _BestpayRefundAmount;		/**<��֧����ɨ���*/
	
}STSETTLE;


/**<
--	�ڿ������ܶ����Ϊ��
��	����ܽ��          N12
��	����ܱ���          N3
��	�����ܽ��          N12
��	�����ܱ���          N3
��	����Ӧ�����		N1	
--	�⿨�����ܶ����Ϊ��
��	����ܽ��          N12
��	����ܱ���          N3
��	�����ܽ��          N12
��	�����ܱ���          N3
��	����Ӧ�����		N1
*/
typedef struct
{
	/**<
	* �ڿ��������ݣ�
	*/
	char sDebitNum_N[6];			/**<����ܱ���			N6*/
	char sDebitAmt_N[12];			/**<����ܽ��          N12*/
	char sCreditNum_N[6];			/**<�����ܱ���          N6*/
	char sCreditAmt_N[12];			/**<�����ܽ��          N12*/
	char cSettleCode_N;				/**<����Ӧ�����	   N1*/
	/**<
	* �⿨�������ݣ�
	*/
	char sDebitNum_W[6];			/**<����ܱ���	 N6*/
	char sDebitAmt_W[12];			/**<����ܽ��          N12*/
	char sCreditNum_W[6];			/**<�����ܱ���          N6*/
	char sCreditAmt_W[12];			/**<�����ܽ��          N12*/
	char cSettleCode_W;				/**<����Ӧ�����	   N1*/
}STAMT_NUM_SETTLE;

typedef struct
{
	SETTLE_NUM nDebitNum_N;		/**<����ܱ���(�����)	*/
	SETTLE_AMT nDebitAmount_N;		/**<����ܶ�(�����)		*/
	SETTLE_NUM nCreditNum_N;		/**<�����ܱ���(�����)	*/
	SETTLE_AMT nCreditAmount_N;	/**<�����ܶ�(�����)		*/
	
	SETTLE_NUM nDebitNum_W;		/**<����ܱ���(�⿨)		*/
	SETTLE_AMT nDebitAmount_W;		/**<����ܶ�(�⿨)		*/
	SETTLE_NUM nCreditNum_W;		/**<�����ܱ���(�⿨)		*/
	SETTLE_AMT nCreditAmount_W;	/**<�����ܶ�(�⿨)		*/
}STAMT_NUM_INFO_SETTLE;

/**
* ����Ϊ�ӿں���
*/
extern int GetSettleDataNK(STSETTLE *);
extern int GetSettleDataWK(STSETTLE *);
extern int DispTotal(void);
extern void ClearSettle(void);
extern int ChangeSettle(const char *, const char, const char *);
extern int ChgSettleForAdjust(const char *, const char *, const char, const char *);
extern int DealSettle(STAMT_NUM_INFO_SETTLE *);
extern int DealSettleTask(void);
extern int FindSettle(void);
extern int Settle(char);
extern int DoSettle(void);
extern void InitSettleData(void);
extern int CheckTransAmount(const char *, const char);
extern int PackRefund(STSYSTEM *, STWATER *);
extern int DealBatchUpFail(STWATER *, int, int);
#endif

