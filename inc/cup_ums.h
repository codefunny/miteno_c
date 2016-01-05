/**
* @file cup_ums.h
* @brief 银联、银商差异模块
* @version  1.0
* @author 林礼达
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

/**<调用其他应用函数传入参结构*/
typedef struct  
{
	char cFuncIndex;//调用其他应用函数的索引
	char cCommType;//传送进来的通讯类型
	char cIsCommInit;//是否需要初始化通讯参数(NO:已经初始化过 YES:未初始化)
	char stInData[1024+512];//传入的参数结构指针
}STCALLFUNCIN;

typedef struct  
{
	char cReturn;//函数的返回值
	char cOutCommType;//传出去来的通讯类型
	char cIsCommInit;//是否需要初始化通讯参数(NO:已经初始化过 YES:未初始化)
	char stOutData[1024+512];//传出的参数结构指针
}STCALLFUNCOUT;

typedef enum {
	MENU_FUNC=1,    /*主菜单*/
	SALE_FUNC,      /*消费*/
    BALANCE_FUNC,   /*余额查询*/        
	REVERSAL_FUNC,  /*冲正*/
	VOIDSALE_FUNC,  /*撤销*/
	REFUND_FUNC,    /*退货*/
	LOGIN_FUNC,     /*签到*/
	REPRINT_FUNC,   /*重打印*/
	FINDWATER_FUNC, /*查找流水*/
	SETTLE_FUNC     /*结算*/
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

