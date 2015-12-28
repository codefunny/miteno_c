/**
* @file manage.h
* @briefϵͳ������ģ��
* @version  1.0
* @author Ѧ��
* @date 2007-03-07
*/

#ifndef _MANAGE_H_
#define _MANAGE_H_


extern void Manage(void);
extern int ChkRF(void);
extern int ChkPinpad(void);
#if defined(EMV_IC)
extern int SetPinpadKey(char *, int);
#endif
extern int SetFunctionIsPrintFailWaterRec(void);
extern int SaveReaderKey(int, int, char *, char *);
extern void HideManage(void);
#endif

