/**
* @file tool.h
* @brief 工具模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _TOOL_H_
#define _TOOL_H_

#include "posdef.h"

#define ProSelectList(pstr, pstitle, nselect, pnselect) \
	PubSelectListItem( pstr, pstitle, NULL, nselect, pnselect)

extern int ProAmtToDispOrPnt(const char *, char *);
extern int ProSelectYesOrNo(char *, char *, char *, char *);
extern int ProSelectYesOrNoExt(char *, char *, char *, char *);
extern int ProChkDigitStr(const char *, const int);
extern int ProConfirm(void);
extern unsigned long long Bcd2Unit64(uchar *,  int);
extern int ProDataToDispOrPnt(const char *, char *);
extern unsigned long long AtoLL(const char *);
extern const char * ImgManage(EM_LOGO_TYPE);
extern int AscBigCmpAsc(const char *, const char *);
extern void AmtSubAmt(uchar *, uchar *, uchar *);
extern void AmtAddAmt(uchar *, uchar *, uchar *);
extern void ShowLightIdle();
extern void ShowLightReady();
extern void ShowLightDeal();
extern void ShowLightFail();
extern void ShowLightSucc();
extern void ShowLightOnline();
extern void ShowLightOff();
extern void LeftTrimZero(char *);
extern int CurrencyAmtToDisp(const char *, const char *, char *);
extern int CommSetSuspend(uint unFlag);
extern int ScanBarCode(char *pszTitle, char *pszContent, int nMax, char * pszBarCode, YESORNO cIsVerify);
extern int DealQRCode(char *pszCodeUrl, char *pszAmount, char cTransType);
extern int Inner_Scan(int , char * ,  int , char *, char *, YESORNO );
extern int Outlay_Scan(int , char * ,  int , char *, char *, YESORNO );
#endif

