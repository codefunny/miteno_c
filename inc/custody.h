#ifndef _CUSTODY_H_
#define _CUSTODY_H_


#define APP_REPIN	(-5)		/**/

typedef enum
{
	DPC_DEFAULT     = 0x00,     //默认设置
	DPC_SETREVERSAL = 0x01,		//设置冲正标识
	DPC_NOCHECKRESP = 0x02,		//不检验包返回码
	DPC_EMV         = 0x04      //EMV 操作
}ENM_DEALPACKCOMM;

typedef enum
{
	TDC_DEFAULT     = 0x00,     //默认设置
	TDC_UNSETREVERSAL = 0x01,	//不设置冲正标识
	TDC_OFFLINE		= 0x02
}ENM_TRADECOMPLETE;

typedef enum
{
	TDI_DEFAULT     = 0x00,     //默认设置
	TDI_CHECKOPER 	= 0x01,		//检测操作员的权限是否是普通操作员
}ENM_TRADEINIT;

typedef struct
{
	int nTag;							/**<标签值*/
	char cType;						/**<打包类型A:字符串B:BCD码*/
	int nMaxLen;						/**<最大长度*/
}ST_CUSTODY_TAG;


typedef struct
{
	ST_CUSTODY_TAG stTag;
	int nLen;
	char *psVale;
}ST_CUSTODY_TLV;



#define CUSTODY_MAX_PACK_SIZE		1024

extern int CustodyTlvInit(void);
extern int CustodyTlvAddValue(int nTag, char *pszInvalue, char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum);
extern int CustodyPack(char *psPack, int *pnLen, int nTagSum);
extern int CustodyUpPack(char *psPack, int *pnLen, int *pnTagSum);
extern int CustodyChkRespon(STSYSTEM *pstSystem, char * psPack, int nLen);
extern void CustoyDispResp(const char *pszRespCode, const char * pszRestInfo);
extern int CustodyGetField(int nTag, char *psValue, int *pnLen);
extern int MakePin(const char *pszCardNo, const char *pszPin, char *psEncryptPin);
extern int CustodyAddMac(char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum);
extern int CustodyChkMac(char *pszCheckMac);
extern int TradeInit(char* pszTitle, ENM_TRADEINIT cOperFlag, const char cTransType, const char cAttr, const char cClearScr);
extern int CustodyTlvAddValueLen(int nTag, char *pszInvalue,int nInValueLen, char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum);
#endif




