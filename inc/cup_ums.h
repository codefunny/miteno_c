/**
* @file cup_ums.h
* @brief ���������̲���ģ��
* @version  1.0
* @author �����
* @date 2014-09-23
*/
#ifndef _CUP_UMS_H_
#define _CUP_UMS_H_

typedef enum
{
	LED_CLOSE=0,
	LED_FLICK,
	LED_OPEN,
	LED_UNCHANGED,
	
}EM_LEDMODE;

/**<��������Ӧ�ú�������νṹ*/
typedef struct  
{
	char cFuncIndex;//��������Ӧ�ú���������
	char cCommType;//���ͽ�����ͨѶ����
	char cIsCommInit;//�Ƿ���Ҫ��ʼ��ͨѶ����(NO:�Ѿ���ʼ���� YES:δ��ʼ��)
	char stInData[1024+512];//����Ĳ����ṹָ��
}STCALLFUNCIN;

typedef struct  
{
	char cReturn;//�����ķ���ֵ
	char cOutCommType;//����ȥ����ͨѶ����
	char cIsCommInit;//�Ƿ���Ҫ��ʼ��ͨѶ����(NO:�Ѿ���ʼ���� YES:δ��ʼ��)
	char stOutData[1024+512];//�����Ĳ����ṹָ��
}STCALLFUNCOUT;

typedef enum {
	MENU_FUNC=1,    /*���˵�*/
	SALE_FUNC,      /*����*/
    BALANCE_FUNC,   /*����ѯ*/        
	REVERSAL_FUNC,  /*����*/
	VOIDSALE_FUNC,  /*����*/
	REFUND_FUNC,    /*�˻�*/
	LOGIN_FUNC,     /*ǩ��*/
	REPRINT_FUNC,   /*�ش�ӡ*/
	FINDWATER_FUNC, /*������ˮ*/
	SETTLE_FUNC     /*����*/
}FUNCLIST;



extern int Cup_BeepAysn(uint, uint);
extern void Cup_InitScrLed(void);
extern void Cup_ShowScrLed(EM_LEDMODE, EM_LEDMODE, EM_LEDMODE, EM_LEDMODE);
extern void Cup_ExitScrLed(void);
extern void Cup_OpenLedBackGround(void);
extern int Cup_SetPrnTitle(const char *, char *);

extern int Ums_SelectInputMode(const char *, int *);
extern int Ums_DealPrintLimit(void);
extern int Ums_DealSettleNum(void);
extern int Ums_SetMainKeyFromMC(void);
extern int Ums_SetIsModify(void);
extern void EnableDispDefault(void);
extern void Ums_MenuLbs();
extern void Ums_ResetLbsCount();
extern void Ums_IncLbsCount();
extern int Ums_DealStationInfo();
extern int Ums_CheckMcVer(void);
#endif

