/**
* @file print.h
* @brief 主模块（函数入口）
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _PRINT_H_
#define _PRINT_H_

enum EPRINTFLAG{
	FIRSTPRINT,
	REPRINT,
	REVERSAL_PRINT,
	OFFLINE_PRINT
};

#define SETTLEFILENAME "OLD_SETT"
#define PRINTFILE "ALLPAYPNT"

extern int PrintWater(const STWATER *, int);
extern int PrintAllWater(void);
extern int PrintSettle(int);
extern int PrintTotal(void);
extern int PrintFailWater(char);
extern int PrintParam(void);
extern int Reprint(void);
extern void GetBankName(const char *,char *, char);
extern int DealPrintLimit(void);
extern void GetTransNameForTP(char, char *, char *);
extern int Print2DScan(char *pszAscInfo);
extern int PrintImage(PrintDataInfo stDataInfo);
#endif

