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

