/**
* @file managetxn.h
* @brief 应用管理处理模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/

#ifndef _MANAGETXN_H_
#define _MANAGETXN_H_

#define FILE_BLACKLIST APP_NAME"BLCKLIST"

/**
* 以下为接口函数
*/
extern void DealSystem(STSYSTEM *);
extern int DealSysReq(void);
extern int ChkLoginStatus(void);
extern int ChkOperRole(void);
extern int ChkRespon(STSYSTEM *, const char *);
extern int ChkPosLimit(void);
extern int ChkRespMsgID(const char *, const char *);
extern int Login(void);
extern int LogOut(void);
extern int StatusSend(const char *, const void *, const int, void *, int *);
extern int ParamTransfer(const char *, const void *, const int, void *, int *);
extern int PosParamTransfer(void);
extern int ChkTraditionOnOffStatus(char);
extern int ChkECashOnOffStatus(char, char);
extern int ChkInstallmentOnOffStatus(char);
extern int ChkBonusOnOffStatus(char);
extern int ChkPhoneChipSaleOnOffStatus(char);
extern int ChkAppointmentOnOffStatus(char);
extern int ChkOrderOnOffStatus(char);
extern int ChkOtherOnOffStatus(char);
extern int DealPosLimit(void);

extern int DownloadKek(void);
#endif

