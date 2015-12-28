/*
* �´�½���Թɷ����޹�˾ ��Ȩ����(c) 2006-2014
*
* POS��׼�湫����
* ͨѶͷ�ļ� --- llua.h
* ��    �ߣ�    ���
* ��    �ڣ�    2013-06-13
* ����޸��ˣ�  ���
* ����޸����ڣ�2013-06-13
*/
#ifndef _LLUA_H_
#define	_LLUA_H_


/**
* @brief ��ʾ��Ļ,����Lua��Ӧ����ʾ������
* @param [in] pszFunctionName   ������
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaDisplay(const char *pszFunctionName);


/**
* @brief ��ʾ��Ļ,����Lua��Ӧ����ʾ������
* @param [in] pszFunctionName   ������
* @param [in] pszInParamList     �������
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaDisplay2(const char *pszFunctionName,const  char * pszInParamList);


/**
* @brief ���������ַ����Ի���
* @param [in] pszFunctionName   ������
* @param [out] pszOut  �������ݻ�����
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaInputString(const char *pszFunctionName , char * pszOut);

/**
* @brief ���������ַ����Ի���
* @param [in] pszFunctionName   ������
* @param [in] pszInParamList     �������
* @param [out] pszOut  �������ݻ�����
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaInputString2(const char *pszFunctionName, const char * pszInParamList, char * pszOut);

/**
* @brief ��������ʮ���������ݶԻ���
* @param [in] pszFunctionName   ������
* @param [out] psOut  �������ݻ�����
* @param [out] pnOutLen  �������ݳ���
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaInputData(const char *pszFunctionName,char * psOut, int * pnOutLen);

/**
* @brief ��������ʮ���������ݶԻ���
* @param [in] pszFunctionName   ������
* @param [in] pszInParamList     �������
* @param [out] psOut  �������ݻ�����
* @param [out] pnOutLen  �������ݳ���
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaInputData2(const char *pszFunctionName, const char * pszInParamList, char * psOut, int * pnOutLen);

/**
* @brief ����Lua ��Դ
* @param ��
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaOpen(void);

/**
* @brief ����Lua�ļ�
* @param [in] pszFileName  �ļ���
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaLoadFile(const char * pszFileName);

/**
* @brief �ͷ�Lua ��Դ
* @param ��
* @return 
* @li <0 ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-6-8
*/
int PubLuaClose(void);

/**
* @brief ��ʾVER
* @param ��
* @return �汾
* @author �޻���
* @date 2013-7-11
*/
char* PubLuaUIVer(void);

#endif
/* End of llua.h */
