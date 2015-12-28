/**
* @file oper.h
* @brief 操作员处理模块
* @version  1.0
* @author 薛震
* @date 2007-02-5
*/

#ifndef _OPER_H_
#define _OPER_H_

#define MANAGEROPER      0				/**< 主管操作员标识*/
#define NORMALOPER         1				/**< 普通操作员标识*/			
#define ADMINOPER		2				/**< 系统管理员标识*/
#define HIDEMENUOPER    3               /**隐藏菜单管理员*/

#define OPERCODELEN		2				/**< 操作员编码所占用的长度*/

/**
* 以下为接口函数
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

