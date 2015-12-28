/*
* 新大陆电脑股份有限公司 版权所有(c) 2006-2014
*
* POS标准版公共库
* 通讯头文件 --- llua.h
* 作    者：    刘罡
* 日    期：    2013-06-13
* 最后修改人：  刘罡
* 最后修改日期：2013-06-13
*/
#ifndef _LLUA_H_
#define	_LLUA_H_


/**
* @brief 显示屏幕,调用Lua对应的显示函数名
* @param [in] pszFunctionName   函数名
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaDisplay(const char *pszFunctionName);


/**
* @brief 显示屏幕,调用Lua对应的显示函数名
* @param [in] pszFunctionName   函数名
* @param [in] pszInParamList     输入参数
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaDisplay2(const char *pszFunctionName,const  char * pszInParamList);


/**
* @brief 调用输入字符串对话框
* @param [in] pszFunctionName   函数名
* @param [out] pszOut  接收数据缓冲区
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaInputString(const char *pszFunctionName , char * pszOut);

/**
* @brief 调用输入字符串对话框
* @param [in] pszFunctionName   函数名
* @param [in] pszInParamList     输入参数
* @param [out] pszOut  接收数据缓冲区
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaInputString2(const char *pszFunctionName, const char * pszInParamList, char * pszOut);

/**
* @brief 调用输入十六进制数据对话框
* @param [in] pszFunctionName   函数名
* @param [out] psOut  接收数据缓冲区
* @param [out] pnOutLen  接收数据长度
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaInputData(const char *pszFunctionName,char * psOut, int * pnOutLen);

/**
* @brief 调用输入十六进制数据对话框
* @param [in] pszFunctionName   函数名
* @param [in] pszInParamList     输入参数
* @param [out] psOut  接收数据缓冲区
* @param [out] pnOutLen  接收数据长度
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaInputData2(const char *pszFunctionName, const char * pszInParamList, char * psOut, int * pnOutLen);

/**
* @brief 申请Lua 资源
* @param 无
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaOpen(void);

/**
* @brief 加载Lua文件
* @param [in] pszFileName  文件名
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaLoadFile(const char * pszFileName);

/**
* @brief 释放Lua 资源
* @param 无
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-6-8
*/
int PubLuaClose(void);

/**
* @brief 显示VER
* @param 无
* @return 版本
* @author 罗辉祥
* @date 2013-7-11
*/
char* PubLuaUIVer(void);

#endif
/* End of llua.h */
