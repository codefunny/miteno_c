/**
* @file settle.h
* @brief 结算管理处理模块
* @version  1.0
* @author 张捷
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
	uchar sSettleAmt[8]; /*只用到前6位用于处理压缩过后的金额*/
}SETTLE_AMT;

/**
* @struct STSETTLE结算数据结构
*/
//目前长度336，不用外卡的部分
typedef struct tagSettle
{
	SETTLE_NUM _SaleNum ;				/**<消费笔数 */
	SETTLE_AMT _SaleAmount ;			/**<消费金额*/

	SETTLE_NUM _VoidSaleNum ;			/**<消费撤销笔数*/
	SETTLE_AMT _VoidSaleAmount ;		/**<消费撤销金额*/

	SETTLE_NUM _AuthSaleNum ;			/**<授权完成笔数(联机)*/
	SETTLE_AMT _AuthSaleAmount;			/**<授权完成金额(联机)*/

	SETTLE_NUM _VoidAuthSaleNum ;		/**<撤销授权完成笔数(联机)*/
	SETTLE_AMT _VoidAuthSaleAmount ;	/**<撤销授权完成金额(联机)*/

	SETTLE_NUM _RefundNum;				/**<退货笔数*/
	SETTLE_AMT _RefundAmount; 			/**<退货金额*/

	SETTLE_NUM _CouponVerifyNum;		/**< 串码验证笔数*/
	SETTLE_AMT _CouponVerifyAmount;		/**< 串码验证金额*/

	SETTLE_NUM _TelVerifyNum;			/**< 手机号验证笔数*/
	SETTLE_AMT _TelVerifyAmount;		/**< 手机号证金额*/

	SETTLE_NUM _CardVerifyNum;			/**< 银行卡验证笔数*/
	SETTLE_AMT _CardVerifyAmount;		/**< 银行卡验证金额*/

	SETTLE_NUM _VoidCouponVerifyNum;	/**< 串码验证撤销笔数*/
	SETTLE_AMT _VoidCouponVerifyAmount;	/**< 串码验证撤销金额*/

	SETTLE_NUM _VoidTelVerifyNum;		/**< 手机号验证撤销笔数*/
	SETTLE_AMT _VoidTelVerifyAmount;	/**< 手机号证撤销金额*/

	SETTLE_NUM _VoidCardVerifyNum;		/**< 银行卡验证撤销笔数*/
	SETTLE_AMT _VodiCardVerifyAmount;	/**< 银行卡验证撤销金额*/

	SETTLE_NUM _RefundVerifyNum;		/**< 验证退货笔数*/
	SETTLE_AMT _RefundVerifyAmount;		/**< 验证退货金额*/	

	SETTLE_NUM _RefundWechatNum;		/**< 微信退货笔数*/
	SETTLE_AMT _RefundWeChatAmount;		/**< 微信退货金额*/	

	SETTLE_NUM _PrecreateNum;			/**< 扫描支付笔数*/
	SETTLE_AMT _PrecreateAmount;		/**< 扫码支付金额*/	

	SETTLE_NUM _CreatepayNum;			/**< 条码支付笔数*/
	SETTLE_AMT _CreatepayAmount;		/**< 条码支付金额*/	

	SETTLE_NUM _VoidPrecreateNum;		/**< 扫描支付撤销笔数*/
	SETTLE_AMT _VoidPrecreateAmount;	/**< 扫码支付撤销金额*/	

	SETTLE_NUM _VoidCreatepayNum;		/**< 条码支付撤销笔数*/
	SETTLE_AMT _VoidCreatepayAmount;	/**< 条码支付撤销金额*/		

	SETTLE_NUM _BaiduCreatepayNum;		/**<百度条码支付笔数*/
	SETTLE_AMT _BaiduCreatepayAmount;	/**<百度条码支付金额*/	

	SETTLE_NUM _VoidBaiduCreatepayNum;		/**< 百度条码支付撤销笔数*/
	SETTLE_AMT _VoidBaiduCreatepayAmount;	/**< 百度条码支付撤销金额*/	

	
	SETTLE_NUM _BaiduPrecreteNum;		/**<百度被扫支付笔数*/
	SETTLE_AMT _BaiduPrecreteAmount;		/**<百度被扫支付金额*/	

	SETTLE_NUM _VoidBaiduPrecreteNum;	/**< 百度被扫支付撤销笔数*/
	SETTLE_AMT _VoidBaiduPrecreteAmount;	/**< 百度被扫支付撤销金额*/	

	SETTLE_NUM _RefundBaiduNum;			/**<百度退货笔数*/
	SETTLE_AMT _RefundBaiduAmount;		/**<百度退货金额*/	

	SETTLE_NUM _JDPrecreateNum;			/**< 京东扫描支付笔数*/
	SETTLE_AMT _JDPrecreateAmount;		/**< 京东扫码支付金额*/	

	SETTLE_NUM _VoidJDPrecreateNum;		/**< 京东扫描支付撤销笔数*/
	SETTLE_AMT _VoidJDPrecreateAmount;	/**< 京东扫码支付撤销金额*/

	SETTLE_NUM _JDRefundNum;			/**<京东退货笔数*/
	SETTLE_AMT _JDRefundAmount;			/**<京东退货金额*/	

	SETTLE_NUM _AliCreatepayNum;		/**<支付宝条码支付笔数*/
	SETTLE_AMT _AliCreatepayAmount;		/**<支付宝条码支付金额*/	

	SETTLE_NUM _VoidAliCreatepayNum;	/**< 支付宝条码撤销笔数*/
	SETTLE_AMT _VoidAliCreatepayAmount;	/**< 支付宝条码撤销金额*/

	SETTLE_NUM _AliRefundNum;			/**<支付宝退货笔数*/
	SETTLE_AMT _AliRefundAmount;		/**<支付宝退货金额*/

	SETTLE_NUM _AliPrecreteNum;			/**<支付宝被扫笔数*/
	SETTLE_AMT _AliPrecreteAmount;		/**<支付宝被扫金额*/

	SETTLE_NUM _VoidAliPrecreteNum;			/**<支付宝被扫撤销笔数*/
	SETTLE_AMT _VoidAliPrecreteAmount;		/**<支付宝被扫撤销金额*/
	

	SETTLE_NUM _MeituanNum;				/**<美团笔数*/
	SETTLE_AMT _MeituanAmount;			/**<美团金额*/	

	SETTLE_NUM _PanjintongNum;			/**<盘锦通笔数*/
	SETTLE_AMT _PanjintongAmount; 		/**<盘锦通金额*/

	SETTLE_NUM _BestpayCreatepayNum;		/**<翼支付主扫笔数*/
	SETTLE_AMT _BestpayCreatepayAmount;		/**<翼支付主扫金额*/	

	SETTLE_NUM _VoidBestpayCreatepayNum;	/**<翼支付主扫撤销笔数*/
	SETTLE_AMT _VoidBestpayCreatepayAmount;	/**<翼支付主扫撤销金额*/

	SETTLE_NUM _BestpayRefundNum;			/**<翼支付主扫退货笔数*/
	SETTLE_AMT _BestpayRefundAmount;		/**<翼支付主扫金额*/
	
}STSETTLE;


/**<
--	内卡结算总额，内容为：
●　	借记总金额          N12
●　	借记总笔数          N3
●　	贷记总金额          N12
●　	贷记总笔数          N3
●　	对账应答代码		N1	
--	外卡结算总额，内容为：
●　	借记总金额          N12
●　	借记总笔数          N3
●　	贷记总金额          N12
●　	贷记总笔数          N3
●　	对账应答代码		N1
*/
typedef struct
{
	/**<
	* 内卡结算内容：
	*/
	char sDebitNum_N[6];			/**<借记总笔数			N6*/
	char sDebitAmt_N[12];			/**<借记总金额          N12*/
	char sCreditNum_N[6];			/**<贷记总笔数          N6*/
	char sCreditAmt_N[12];			/**<贷记总金额          N12*/
	char cSettleCode_N;				/**<对账应答代码	   N1*/
	/**<
	* 外卡结算内容：
	*/
	char sDebitNum_W[6];			/**<借记总笔数	 N6*/
	char sDebitAmt_W[12];			/**<借记总金额          N12*/
	char sCreditNum_W[6];			/**<贷记总笔数          N6*/
	char sCreditAmt_W[12];			/**<贷记总金额          N12*/
	char cSettleCode_W;				/**<对账应答代码	   N1*/
}STAMT_NUM_SETTLE;

typedef struct
{
	SETTLE_NUM nDebitNum_N;		/**<借记总笔数(人民币)	*/
	SETTLE_AMT nDebitAmount_N;		/**<借记总额(人民币)		*/
	SETTLE_NUM nCreditNum_N;		/**<贷记总笔数(人民币)	*/
	SETTLE_AMT nCreditAmount_N;	/**<贷记总额(人民币)		*/
	
	SETTLE_NUM nDebitNum_W;		/**<借记总笔数(外卡)		*/
	SETTLE_AMT nDebitAmount_W;		/**<借记总额(外卡)		*/
	SETTLE_NUM nCreditNum_W;		/**<贷记总笔数(外卡)		*/
	SETTLE_AMT nCreditAmount_W;	/**<贷记总额(外卡)		*/
}STAMT_NUM_INFO_SETTLE;

/**
* 以下为接口函数
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

