/**
* @file ssl.c
* @brief sslģ��
* @version  1.0
* @author �����
* @date 2014-09-25
*/

#ifndef _SSL_H_
#define _SSL_H_

extern void ExportSslCa(void);
extern int SetSslMode(void);
extern int SslCommSend(char *, int);
extern int SslCommRecv(char *, int *);

#endif

