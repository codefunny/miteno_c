/**
* @file inout.h
* @brief 输入输出模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _INOUT_H_
#define _INOUT_H_

#define PIN_MAX_LEN 12
#define PIN_MIN_LEN 4


extern int GetPin(const char *, const char *, char *);
extern int GetAuthorCode(char *, char *);
extern int GetInterOrg(char *, char *);
extern int DispCouponID(const char *pszCouponID, int nTimeOut);
extern int DispTelno(const char *pszTelno, int nTimeOut);

#endif

