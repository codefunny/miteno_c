#ifndef _CUSTODY_H_
#define _CUSTODY_H_


#define APP_REPIN	(-5)		/**/

typedef enum
{
	DPC_DEFAULT     = 0x00,     //Ĭ������
	DPC_SETREVERSAL = 0x01,		//���ó�����ʶ
	DPC_NOCHECKRESP = 0x02,		//�������������
	DPC_EMV         = 0x04      //EMV ����
}ENM_DEALPACKCOMM;

typedef enum
{
	TDC_DEFAULT     = 0x00,     //Ĭ������
	TDC_UNSETREVERSAL = 0x01,	//�����ó�����ʶ
	TDC_OFFLINE		= 0x02
}ENM_TRADECOMPLETE;

typedef enum
{
	TDI_DEFAULT     = 0x00,     //Ĭ������
	TDI_CHECKOPER 	= 0x01,		//������Ա��Ȩ���Ƿ�����ͨ����Ա
}ENM_TRADEINIT;

typedef struct
{
	int nTag;							/**<��ǩֵ*/
	char cType;						/**<�������A:�ַ���B:BCD��*/
	int nMaxLen;						/**<��󳤶�*/
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




