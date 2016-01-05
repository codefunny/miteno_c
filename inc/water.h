/**
* @file water.h
* @brief 流水处理模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/

#ifndef _WATER_H_
#define _WATER_H_

#define FILE_WATER APP_NAME"WATE"

#define TAG_INSTALLMENT  0xD1
#define TAG_BONUS 		 0xD2
#define TAG_PAN  		 0xD3
#define TAG_DATE	     0xD4
#define TAG_FIELD55		 0xD5
#define TAG_BALANCE      0xD6
#define TAG_SIGN_NUM	 0xD7   /**电子签字编号*/
#define TAG_TELNO		 0XD8  


/**
* @struct STWATER流水结构
*/
typedef struct 
{

/**
	* 以下为标准交易数据元内容
	*/
	char cTransType;			/**<交易类型*/
	char cOldTransType;			/**<原交易类型主要是在结算调整时用*/
	char cTransAttr;			/**<交易属性*/
	char cEMV_Status;			/**<EMV交易的执行状态*/
	char sTransCode[6];			/**<交易码*/
	char sPan[10];				/**<2 主帐号(BCD)*/
	int nPanLen;				/**<2 主帐号长度*/
	char szAmount[6];			/**<4 金额(BCD)*/
	char sTrace[3];				/**<11 POS流水号(BCD)    */
	char sTime[3];				/**<12 域；交易时间hhmmss(BCD)  */
	char sDate[4];				/**<13 域；交易日期yyyymmdd、yymmdd、mmdd  (BCD)  */
	char sExpDate[2];			/**<14 卡有效期(同原请求交易)*/
	char sSettleDate[2];		/**<15 清算日期(BCD)  */
	char szInputMode[3+1];		/**<22 输入模式*/
	char szCardSerialNo[3+1];	/**<23 卡片序列号*/
	char sTrack2[19];			/**<35 二道数据(BCD)*/
	int  nTrack2Len;			/**<35 二道数据长度*/
	char sTrack3[53];			/**<36 三道数据(BCD)*/
	int  nTrack3Len;			/**<36 三道数据长度*/
	char szRefnum[12+1];		/**<37 系统参考号*/
	char szAuthCode[6+1];		/**<38 授权码*/
	char szResponse[2+1];		/**<39 响应码*/
	char szBatchNum[3];			/**<批次号(BCD)*/
	char sFuncCode[2];			/**<60.1 消息类型码*/
	char sOldBatch[3];			/**<原交易批次号(BCD)*/
	char sOldTrace[3];			/**<原流水号(BCD)    */
	char szOldAuthCode[6+1];	/**<原授权码*/
	char szOldRefnum[12+1];   	/**<原交易参考号*/
	char sTelNo[11 + 1];		/**<手机号码*/
	char sCouponID[20+1];		/**<串码*/
	char szRealAmount[6];		/**<抵用金额(BCD)*/
 	char cStatus;				/**<已取消1,已调整2,已退货3;  上送后被调整4*/
	char szOper[2+1];			/**<操作员号                  */
	char cSendFlag;				/**<上送标志0－未上送，0xFD－已上送, 0xFE-上送被拒, 0xFF-上送失败, 其他数字-已上送次数*/
	char szOutTradeNo[50+1];	/**<商户订单号*/
	char sPayableAmount[6];	    /**<应付金额*/
	/**
	* 以下为流水附加内容，可以是多个字段的组合，其解释由具体应用程序负责。
	* 可能会是多个变量的组合。
	*/
	int nAdditionLen;		/**<附加流水内容长度*/
	char sAddition[64];	/**<附加流水内容*/
	char szAdvertisement[256];	/**<广告内容*/
	char szTransName[32];		/**<交易名称*/

	
	int nAdditionLen2;		
	char sAddition2[257];
	char cBatchUpFlag;				/**<上送标志0－未上送，0xFD－已上送, 0xFE-上送被拒, 0xFF-上送失败, 其他数字-已上送次数*/
}STWATER;


/**
* 以下为接口函数
*/

extern int InitWaterFile(void);
extern int AppendWater(const STWATER *);
extern int UpdateWater(const STWATER *);
extern int FindWaterWithInvno( const char *, STWATER *);
extern int FetchFirstWater(STWATER *);
extern int FetchNextWater(STWATER *);
extern int FetchPreviousWater(STWATER *);
extern int FetchLastWater(STWATER *);
extern int DispWater(const STWATER *);
extern int WaterToSys(const STWATER *, STSYSTEM *);
extern int SysToWater(const STSYSTEM *, STWATER *);
extern void GetWaterNum(int *);
extern int FetchCurrentWater(STWATER *);
extern int WaterOneByOne(void);
extern int FindByInvoice(int);
extern int DispVoidWater(const char *, const STWATER *);
extern int GetWaterOper(char *);
extern int IsOperHasWater(const char *);
extern int GetUnSendNum(int *);
extern int GetFailSendNum(char);
#endif

