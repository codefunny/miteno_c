/**
* @file water.h
* @brief ��ˮ����ģ��
* @version  1.0
* @author �Ž�
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
#define TAG_SIGN_NUM	 0xD7   /**����ǩ�ֱ��*/
#define TAG_TELNO		 0XD8  


/**
* @struct STWATER��ˮ�ṹ
*/
typedef struct 
{

/**
	* ����Ϊ��׼��������Ԫ����
	*/
	char cTransType;			/**<��������*/
	char cOldTransType;			/**<ԭ����������Ҫ���ڽ������ʱ��*/
	char cTransAttr;			/**<��������*/
	char cEMV_Status;			/**<EMV���׵�ִ��״̬*/
	char sTransCode[6];			/**<������*/
	char sPan[10];				/**<2 ���ʺ�(BCD)*/
	int nPanLen;				/**<2 ���ʺų���*/
	char szAmount[6];			/**<4 ���(BCD)*/
	char sTrace[3];				/**<11 POS��ˮ��(BCD)    */
	char sTime[3];				/**<12 �򣻽���ʱ��hhmmss(BCD)  */
	char sDate[4];				/**<13 �򣻽�������yyyymmdd��yymmdd��mmdd  (BCD)  */
	char sExpDate[2];			/**<14 ����Ч��(ͬԭ������)*/
	char sSettleDate[2];		/**<15 ��������(BCD)  */
	char szInputMode[3+1];		/**<22 ����ģʽ*/
	char szCardSerialNo[3+1];	/**<23 ��Ƭ���к�*/
	char sTrack2[19];			/**<35 ��������(BCD)*/
	int  nTrack2Len;			/**<35 �������ݳ���*/
	char sTrack3[53];			/**<36 ��������(BCD)*/
	int  nTrack3Len;			/**<36 �������ݳ���*/
	char szRefnum[12+1];		/**<37 ϵͳ�ο���*/
	char szAuthCode[6+1];		/**<38 ��Ȩ��*/
	char szResponse[2+1];		/**<39 ��Ӧ��*/
	char szBatchNum[3];			/**<���κ�(BCD)*/
	char sFuncCode[2];			/**<60.1 ��Ϣ������*/
	char sOldBatch[3];			/**<ԭ�������κ�(BCD)*/
	char sOldTrace[3];			/**<ԭ��ˮ��(BCD)    */
	char szOldAuthCode[6+1];	/**<ԭ��Ȩ��*/
	char szOldRefnum[12+1];   	/**<ԭ���ײο���*/
	char sTelNo[11 + 1];		/**<�ֻ�����*/
	char sCouponID[20+1];		/**<����*/
	char szRealAmount[6];		/**<���ý��(BCD)*/
 	char cStatus;				/**<��ȡ��1,�ѵ���2,���˻�3;  ���ͺ󱻵���4*/
	char szOper[2+1];			/**<����Ա��                  */
	char cSendFlag;				/**<���ͱ�־0��δ���ͣ�0xFD��������, 0xFE-���ͱ���, 0xFF-����ʧ��, ��������-�����ʹ���*/
	char szOutTradeNo[50+1];	/**<�̻�������*/
	char sPayableAmount[6];	    /**<Ӧ�����*/
	/**
	* ����Ϊ��ˮ�������ݣ������Ƕ���ֶε���ϣ�������ɾ���Ӧ�ó�����
	* ���ܻ��Ƕ����������ϡ�
	*/
	int nAdditionLen;		/**<������ˮ���ݳ���*/
	char sAddition[64];	/**<������ˮ����*/
	char szAdvertisement[256];	/**<�������*/
	char szTransName[32];		/**<��������*/

	
	int nAdditionLen2;		
	char sAddition2[257];
	char cBatchUpFlag;				/**<���ͱ�־0��δ���ͣ�0xFD��������, 0xFE-���ͱ���, 0xFF-����ʧ��, ��������-�����ʹ���*/
}STWATER;


/**
* ����Ϊ�ӿں���
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

