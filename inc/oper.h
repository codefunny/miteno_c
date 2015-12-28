/**
* @file oper.h
* @brief ����Ա����ģ��
* @version  1.0
* @author Ѧ��
* @date 2007-02-5
*/

#ifndef _OPER_H_
#define _OPER_H_

#define MANAGEROPER      0				/**< ���ܲ���Ա��ʶ*/
#define NORMALOPER         1				/**< ��ͨ����Ա��ʶ*/			
#define ADMINOPER		2				/**< ϵͳ����Ա��ʶ*/
#define HIDEMENUOPER    3               /**���ز˵�����Ա*/

#define OPERCODELEN		2				/**< ����Ա������ռ�õĳ���*/

/**
* ����Ϊ�ӿں���
*/
extern void InitOper(void);
extern int ChgOperPwd(int);
extern int PosLockUnLock(void);
extern int OperLogon(void);
extern int CheckOper(const char *, int);
extern void GetCurrentOper(char *, char *, int *);
extern void SetCurrentOper(char *, char *, int);
extern int Operate(void);
extern void SetOperLimit( char *);
extern int SetAdminPwd(const char *);

#endif

