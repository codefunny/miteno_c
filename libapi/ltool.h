#ifndef _LTOOLS_H_
#define _LTOOLS_H_

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "NDK.h"

#define SOH     1
#define STX     2
#define ETX     3
#define EOT     4
#define ENQ     5
#define ACK     6
#define DLE     0x10
#define NAK     0x15
#define ETB     0x17

#define FALSE 0
#define TRUE (!FALSE)

#define APP_SUCC		(0)   	/**<成功      */
#define APP_FAIL		(-1)    /**<失败      */
#define APP_QUIT		(-2)    /**<退出、取消*/
#define APP_TIMEOUT 	(-3)    /**<超时      */
#define APP_FUNCQUIT	(-4)    /**<按功能键返回*/

#define FILE_NOCREATEINDEX 0x00		/**<不创建索引文件*/
#define FILE_CREATEINDEX   0x01		/**<创建索引文件  */

#define MAX_ERR_BUF_LEN 	(128)		/**<错误码信息最大长度 */
#define MAX_ERR_NUM		(1000)		/**<错误码信息最大个数 */

#define MAX_INPUTDATASIZE  512
#define MAX_BMPBUFFSIZE	8192

typedef enum
{
	NO   = '0',
	YES  = '1'
}YESORNO;

enum EM_ADDCH_MODE
{
	ADDCH_MODE_BEFORE_STRING=0,			/**<往字符串前面加字符*/
	ADDCH_MODE_AFTER_STRING,			/**<往字符串后面加字符*/
	ADDCH_MODE_BEFORE_AFTER_STRING		/**<往字符串前后加字符*/
};

enum EM_HARDWARE_SUPPORT
{
	HARDWARE_SUPPORT_WIRELESS=0,		/**<无线模块*/
	HARDWARE_SUPPORT_RADIO_FREQUENCY,	/**<射频模块*/
	HARDWARE_SUPPORT_MAGCARD,			/**<磁卡模块*/
	HARDWARE_SUPPORT_SCANNER,			/**<扫描头模块*/
	HARDWARE_SUPPORT_PINPAD,			/**<是否支持外接密码键盘*/
	HARDWARE_SUPPORT_COMM_NUM,			/**<串口个数*/
	HARDWARE_SUPPORT_USB,				/**<是否支持USB*/
	HARDWARE_SUPPORT_MODEM,				/**<MODEM模块*/
	HARDWARE_SUPPORT_WIFI,				/**<wifi模块*/
	HARDWARE_SUPPORT_ETH,				/**<是否支持以太网*/
	HARDWARE_SUPPORT_PRINTER,			/**<打印模块*/
	HARDWARE_SUPPORT_TOUCH_SCREEN,		/**<是否支持触屏*/
	HARDWARE_SUPPORT_LED_LIGHT			/**<是否有射频LED灯*/
};

enum EM_HARDWARE_GET
{
	HARDWARE_GET_BIOS_VER=2,		/**<取bios版本信息 */
	HARDWARE_GET_POS_USN,			/**<取机器序列号 */
	HARDWARE_GET_POS_PSN,			/**<取机器机器号 */
	HARDWARE_GET_BOARD_VER,			/**<取主板号 */
	HARDWARE_GET_CREDITCARD_COUNT, /**<取pos刷卡总数 */
	HARDWARE_GET_PRN_LEN,			/**<取pos打印总长度 */
	HARDWARE_GET_POS_RUNTIME,		/**<取pos机开机运行时间 */
	HARDWARE_GET_KEY_COUNT			/**<取pos机按键次数 */
};

typedef struct tagRecFile
{
	unsigned int unMaxOneRecLen;	/**<最大的单条记录长度 */  
	unsigned int unIndex1Start;		/**<索引字段1的起始位置*/
	unsigned int unIndex1Len;		/**<索引字段1的长度    */
	unsigned int unIndex2Start; 	/**<索引字段2的起始位置*/
	unsigned int unIndex2Len;		/**<索引字段2的长度    */
	char cIsIndex;					/**<是否需要建立索引文件，0x00不建立，0x01建立*/
	char szFileName[128];			/**<记录文件名,有效的记录文件名长度为128*/
	char s[2];
}STRECFILE;

typedef struct 
{
	int nLen;								/**<数据长度 */
	int nLevel;								/**<纠错级别*/
	int nMask;								/**<掩码号*/
	int nVersion;							/**<型号*/
	char szInputData[MAX_INPUTDATASIZE];	/**<要进行编码的数据*/
} DataInfo;

typedef struct
{
	int xsize;
	int ysize;
	char bmpbuff[MAX_BMPBUFFSIZE];
}BmpInfo;

typedef struct
{
	char *pszAmt;
	BmpInfo *pBmpInfo;
}PrintDataInfo;


/**
* @brief 2个最大不超过12位的无符号数字串逐次逐位相加，相加结果不能超过12位
* @param [in] pszDigStr1 数字串1
* @param [in] pszDigStr2 数字串2
* @param [out] pszResult 相加和的数字串
* @param [out] pnOutLen  相加和的位数
* @return 
* @li APP_FAIL 失败
* @li APP_APP_SUCC 成功
* @date 2012-06-19
*/
int PubAddDigitStr (const char* pszDigStr1, const char* pszDigStr2, char* pszResult, int* pnResultLen );

/**
* @brief 将6位数字串pcStrNum增加1后放回原值
* @param [in] pcStrNum  需要被增加的数字串
* @param [out] pcStrNum 增加后的结果串
* @return 
* @li APP_FAIL 失败
* @li APP_APP_SUCC 成功
* @date 2012-06-19
*/
int PubIncNum (char* pcStrNum );

/**
* @brief 	两个ASC数字串相加。
* @param [in] pszAsc1	加数ASC字符串
* @param [in] pszAsc2	加数ASC字符串
* @param [out] pszResult	相加的结果
* @return 
* @li 无
*/
void PubAscAddAsc(uchar *pszAsc1, uchar *pszAsc2, uchar *pszSum);

/**
* @brief 两个ASC字符串相减
* @param [in] pszAsc1	被减数ASC字符串，不超过最大长整型
* @param [in] pszAsc2	减数ASC字符串，不超过最大长整型
* @param [out] pszResult	相减的结果(负数为"-XXXX")
* @return 
* @li 无
*/
void PubAscSubAsc(uchar *pszAsc1, uchar *pszAsc2, uchar *pszResult);

/**
* @brief 两个ASC字符串相乘
* @param [in] pszAsc1	乘数ASC字符串
* @param [in] pszAsc2	乘数ASC字符串
* @param [out] pszProduct	相乘的结果
* @return 
* @li 无
*/
void PubAscMulAsc(uchar *pszAsc1, uchar *pszAsc2, uchar *pszProduct );

/**
* @brief 两个ASC字符串相除
* @param [in] pszAsc1	被除数ASC字符串
* @param [in] pszAsc2	除数ASC字符串
* @param [out] pszResult	相除的结果(取整)
* @return 
* @li 无
*/
void PubAscDivAsc(uchar *pszAsc1, uchar *pszAsc2, uchar *pszResult );

/**
* @brief 把带小数点的金额字符串转为不带小数点的金额字符串
* @param [in] pszSource    待转换的金额字符串
* @param [in] pnTargetLen  目标缓冲区的大小
* @param [out] pszTarget   转换后的字符串
* @param [out] pnTargetLen 转换后的字符串长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubFmtAmtStr (const char* pszSource, char* pszTarget, int* pnTargetLen );

/**
* @brief 将AscII码的字符串转换成压缩的HEX格式，非偶数长度的字符串根据对齐方式，采取左补0，右补F方式
* @param [in] pszAsciiBuf 被转换的ASCII字符串
* @param [in] nLen        输入数据长度(ASCII字符串的长度)
* @param [in] cType       对齐方式  0－左对齐  1－右对齐
* @param [out] pszBcdBuf  转换输出的HEX数据
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubAscToHex (const unsigned char* pszAsciiBuf, int nLen, char cType, unsigned char* pszBcdBuf);

/**
* @brief 将HEX码数据转换成AscII码字符串
* @param [in] pszBcdBuf    被转换的Hex数据
* @param [in] nLen         转换数据长度(ASCII字符串的长度)
* @param [in] cType        对齐方式  1－左对齐  0－右对齐
* @param [out] pszAsciiBuf 转换输出的AscII码数据
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubHexToAsc (const unsigned char* pszBcdBuf, int nLen, char cType, unsigned char* pszAsciiBuf);

/**
* @brief 整型转换为4字节字符串（高位在前）
* @param [in] nNum    需要转换的整型数
* @param [out] pszBuf 转换输出的字符串
* @return 
* @li 无
* @date 2012-06-19
*/
void PubIntToC4 (unsigned char* pszBuf, unsigned int nNum );

/**
* @brief 整型转换为2字节字符串（高位在前）
* @param [in] nNum    需要转换的整型数
* @param [out] pszBuf 转换输出的字符串
* @return 
* @li 无
* @date 2012-06-19
*/
void PubIntToC2 (unsigned char* pszBuf, unsigned int nNum );

/**
* @brief 4字节字符串转换为整型（高位在前）
* @param [in] pszBuf 需要转换的字符串
* @param [out] nNum  转换输出的整型数
* @return 
* @li 无
* @date 2012-06-19
*/
void PubC4ToInt (unsigned int* nNum, unsigned char * pszBuf );

/**
* @brief 2字节字符串转换为整型（高位在前）
* @param [in] pszBuf 需要转换的字符串，pszBuf长度要>=2
* @param [out] nNum  转换输出的整型数
* @return 
* @li 无
* @date 2012-06-19
*/
void PubC2ToInt(unsigned int *nNum, unsigned char *pszBuf);

/**
* @brief 整数(0-99)转换为一字节BCD
* @param [in] nNum 需要转换的整型数(0-99)
* @param [out] ch  转换输出的一个BCD字符
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubByteToBcd(int nNum, char *ch);

/**
* @brief 一字节BCD转换为整数(0-99)
* @param [in] ch     需要转换的BCD字符
* @param [out] pnNum 转换输出的整数值(0-99)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubBcdToByte(char ch, int *pnNum);

/**
* @brief 整数(0-9999)转换为二字节BCD
* @param [in] nNum      需要转换的整型数(0-9999)
* @param [in] pnBcdLen  输出缓冲区的大小   
* @param [out] psBcd    转换输出的两字节BCD
* @param [out] pnBcdLen 转换后的BCD长度，如果成功此值，固定返回值为2
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubIntToBcd(char *psBcd, int *pnBcdLen, int nNum);

/**
* @brief 二字节BCD转换为整数(0-9999)
* @param [in] psBcd   需要转换的两字节BCD，长度应等于2
* @param [out] nNum   转换后的整数(0-9999)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubBcdToInt(const char * psBcd, int *nNum);

/**
* @brief 计算LRC
* @param [in] psBuf  需要计算LRC的字符串
* @param [in] nLen   需要计算LRC的字符串的长度
* @param [out] chLRC 计算得出的LRC
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubCalcLRC(const char *psBuf, int nLen, char *chLRC);

/**
* @brief 字符串去左空格
* @param [in] pszSrc  存放字符串的缓冲区
* @param [out] pszSrc 去掉左空格后的字符串
* @return 
* @li 无
* @date 2012-06-19
*/
void PubLeftTrim(char *pszSrc);

/**
* @brief 字符串去右空格
* @param [in] pszSrc  存放字符串的缓冲区
* @param [out] pszSrc 去掉右空格后的字符串
* @return 
* @li 无
* @date 2012-06-19
*/
void PubRightTrim(char *pszSrc);

/**
* @brief 字符串去右空格
* @param [in] pszSrc  存放字符串的缓冲区
* @param [out] pszSrc 去掉左右空格后的字符串
* @return 
* @li 无
* @date 2012-06-19
*/
void PubAllTrim(char *pszSrc);

/**
* @brief 从一字符串里删除某一字符
* @param [in] pszString		待处理的字符串
* @param [in] ucSymbol	所要删除的字符
* @param [out] pszString	删除特殊字符ucSymbol后的字符串
* @return
* li 转换后整数
*/
void PubDelSymbolFromStr(uchar *pszString, uchar ucSymbol);

/**
* @brief 往一字符串里加入某一字符使之长度为nLen
* @param [in] pszString  存放字符串的缓冲区
* @param [in] nLen       加入字符后字符串的长度
* @param [in] ch         所要加入的字符
* @param [in] nOption    操作类型--EM_ADDCH_MODE
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubAddSymbolToStr(char *pszString, int nLen, char ch, int nOption);

/**
* @brief 删除字符串左边连续的特定字符
* @param [in] pszStringInOut 	待转换的字符串
* @param [in] ucRemoveChar	待删除字符
* @param [out] pszString		转换后的字符串
* @return
* li 无
*/
void PubLeftTrimChar(uchar *pszSrc, uchar ucRemoveChar);

/**
* @brief 删除字符串右边连续的特定字符
* @param [in] pszStringInOut	待转换的字符串
* @param [in] ucRemoveChar	待删除字符
* @param [out] pszString		转换后的字符串
* @return
* li 无
*/
void PubRightTrimChar(uchar *pszSrc, uchar ucRemoveChar);

/**
* @brief 测试一字串是否为纯数字串
* @param [in] pszString    需要判断的字符串
* @return 
* @li APP_FAIL 否
* @li APP_SUCC 是
* @date 2012-06-19
*/
int PubIsDigitStr(const char *pszString);

/**
* @brief 判断给定一字符是不是数字字符
* @param [in] ch 需要判断的字符
* @return 
* @li APP_FAIL 否
* @li APP_SUCC 是
* @date 2012-06-19
*/
int PubIsDigitChar(char ch);

/**
* @brief 判断提供的字符串是不是合法的日期格式串
* @param [in] pDate 日期格式字符串  格式为 YYYYMMDD
* @return 
* @li APP_FAIL 非法
* @li APP_SUCC 合法
* @date 2012-06-19
*/
int PubIsValidDate(const char *pDate);

/**
* @brief	转换数字串到金额格式，如HKD$****.**, RMB****.**
* @param [in] pszPrefix 	pszPrefix  前额的前缀，如“HKD 200.00”中的“HKD”
* @param [in] pszIn			需要转换的纯数字串,过滤到前面的'0'
* @param [in] ucRadPt		小数点后的金额位数，必需小于4
* @param [out] pszOut		格式化后的金额串
* @return
* li 转换后整数
*/
void PubConvAmount(uchar *pszPrefix, uchar *pszIn, uchar ucRadPt, uchar *pszOut);

/**
* @brief 取得当前日期时间
* @param [out] pDatetime 存放当前日期时间其格式为YYYYMMDDHHMMSS，长度必需>14
* @return 
* @li 无
* @date 2012-06-19
*/
void PubGetCurrentDatetime(char *pDatetime);

/**
* @brief 设置POS日期时间
* @param in pszDate 日期
* @param in pszDateFormat 格式定义
* @param in pszTime 时间
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 失败
*/
int PubSetPosDateTime(const char *pszDate, const char *pszDateFormat, const char *pszTime);

/**
* @brief 找出某年某月的最大天数
* @param [in] nYear   年份
* @param [in] nMon    月份
* @param [out] pnDays 该年份该月对应的最大天数
* @return 
* @li APP_FAIL 非法的输入参数
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubMonthDays(int nYear, int nMon, int *pnDays);

/**
* @brief 判断某年是否闰年
* @param [in] nYear 年份
* @return 
* @li APP_FAIL 非闰年
* @li APP_SUCC 闰年
* @date 2012-06-19
*/
int PubIsLeapYear(int nYear);

/**
* @brief 获取超时的时间计数
* @param [in]  unMsTime  超时时间 (单位:ms)
* @return 
* @li 返回时间计数
* @author 刘罡
* @date 2012-10-24
*/
uint PubGetOverTimer(uint unMsTime);

/**
* @brief 判断是否到达超时时间
* @param [in]  unOverTime  之前获取的超时时间计数 (单位:ms)
* @return 
* @li APP_APP_SUCC 已超时
* @li APP_APP_FAIL 未超时
* @author 刘罡
* @date 2012-10-24
*/
int PubTimerIsOver(uint unOverTime);

/**
* @brief 取工具库版本号
* @param [out] pszVer 存放版本号，字符串长度>12
* @return 
* @li 无
* @date 2012-06-19
*/
void PubGetToolsVer(char *pszVer);

/**
* @brief 创建索引文件
* @param [in] pstRecFile   参数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubCreatRecFile(const STRECFILE *pstRecFile);

/**
* @brief 按文件名打开文件
* @param [in] pszFileName   流水文件名
* @param [in] pszMode       打开方式
* @param [out] pnFileHandle 打开的文件句柄
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubOpenFile (const char *pszFileName, const char *pszMode,int *pnFileHandle);

/**
* @brief 读文件
* @param [in] nHandle   文件句柄
* @param [in] unLength  需要读取的字符的长度
* @param [out] psBuffer 需要读入的缓冲区
* @return 
* @li APP_FAIL 失败
* @li 返回实际读到数据长度
*/
int PubReadFile(int nHandle, char * psBuffer, uint unLength);

/**
* @brief 写文件
* @param [in] nHandle   文件句柄
* @param [in] psBuffer  需要写入文件内容的缓冲区
* @param [in] unLength	需要写入的长度
* @return 
* @li APP_FAIL 失败
* @li 返回实际写入数据长度
*/
int PubWriteFile(int nHandle, const char * psBuffer, uint unLength);

/**
* @brief 关闭文件句柄
* @param [in] pnFileHandle 要关闭的文件句柄
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubCloseFile (int *pnFileHandle);

/**
* @brief 删除流水文件
* @param [in] pszFileName 流水文件名
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubDeReclFile (const char *pszFileName);

/**
* @brief 按文件名打开文件，获取文件记录总数
* @param [in] pszFileName 流水文件名
* @param [out] pnRecSum   指向记录数量的指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubGetRecSum (const char *pszFileName, int *pnRecSum);

/**
* @brief 增加流水记录，对于创建记录是指定了需要建立索引文件的会相应增加索引记录
* @param [in] pszFileName 流水文件名
* @param [in] pszRec 输入的流水记录
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubAddRec (const char *pszFileName, const char *pszRec);

/**
* @brief 按记录号读流水记录
* @param [in] nFileHandle 打开的文件句柄
* @param [in] nRecNo      要找的记录号
* @param [out] psRec      记录指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubReadRec (const int nFileHandle, const int nRecNo, char *psRec);

/**
* @brief 更新记录，按指定的记录位置重写指定的记录，同时对于创建记录是指定了需要建立索引文件的会相应更新索引记录
* @param [in] pszFileName 记录文件名称
* @param [in] nRecNo      要修改的记录号
* @param [in] psRec       记录指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubUpdateRec (const char *pszFileName, const int nRecNo, const char *psRec);

/**
* @brief 更新记录，按指定的记录位置重写指定的记录，同时对于创建记录是指定了需要建立索引文件的会相应更新索引记录
* @param [in] nFileHandle 记录文件句柄
* @param [in] nRecNo      要修改的记录号
* @param [in] psRec       记录指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubUpdateRecByHandle (int nFileHandle, const int nRecNo, const char *psRec);

/**
* @brief 按记录号读取单条记录
* @param [in] pszFileName 记录文件名称
* @param [in] nRecNo      要读取的记录号
* @param [out] psRec      记录指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubReadOneRec(const char *pszFileName, const int nRecNo, char *psRec);

/**
* @brief 按索引查找记录。根据索引字段1/索引字段2从指定的记录号开始查找记录，若同时指定索引字段1/索引字段2，则要同时满足这两个条件的记录输出，不指定时用NULL,若符合索引条件的记录存在重复的情况，输出第一条记录
* @param [in] pszFileName 记录文件名称
* @param [in] psIndexStr1 索引字段1
* @param [in] psIndexStr2 索引字段2
* @param [in] nBeginRecNo 指定的开始记录号
* @param [out] psRec 记录指针
* @param [out] pnRecNo 符合条件的记录号
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubFindRec(const char *pszFileName, const char *psIndexStr1, const char *psIndexStr2,const int nBeginRecNo, char *psRec, int *pnRecNo);

/**
* @brief 按索引删除记录。根据索引字段1/索引字段2从指定的记录号开始查找记录，若同时指定索引字段1/索引字段2，则删除同时满足这两个条件的记录，不指定时用NULL,若符合索引条件的记录存在重复的情况，删除第一条记录
* @param [in] pszFileName 记录文件名称
* @param [in] psIndexStr1 索引字段1
* @param [in] psIndexStr2 索引字段2
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubDelRec(const char *pszFileName, const char *psIndexStr1,  const char *psIndexStr2);

/**
* @brief 错误码的记录及错误日志的处理
* @param [in] nErrorCode     错误值
* @param [in] pszLogFileName Log文件名,为NULL不记录文件
* @param [in] pszMoudleName  模块名
* @param [in] nLine          行数，调用者使用__LINE__
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubErrLog (const int nErrorCode, const char *pszLogFileName, char *pszMoudleName, int nLine );

/**
* @brief 得到ini文件中指定的段和项指定的字符串
* @param [in] nHandle     INI文件句柄
* @param [in] psSeg       段指针
* @param [in] psItem      项指针
* @param [in] nMaxStrLen  最大允许的输出字符串长度
* @param [out] pcValueStr 得到的字符串值指针
* @param [out] pnValueLen 得到的字符串长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubGetINIItemStr (const int nHandle, const char *psSeg, const char *psItem, const int nMaxStrLen, char *psValueStr, int *pnValueLen);

/**
* @brief 得到ini文件中指定的段和项指定的值
* @param [in] nHandle  INI文件句柄
* @param [in] psSeg    段指针
* @param [in] psItem   项指针
* @param [out] pnValue 得到的值指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubGetINIItemInt (const int nHandle,const char *psSeg, const char *psItem, int *pnValue);

/**
* @brief 按指定的ID寻找指定的项目值，需指定参数文件的有效内容开始位置
* @param [in] pnHandle      参数文件句柄
* @param [in] nPosition     参数文件有效内容开始位置
* @param [in] psID          要找的ID项字符串指针
* @param [out] psParamValue 找到的ID项值指针
* @param [out] pnParamLen   找到的ID项值长度指针
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubFindParam (const int pnHandle, const int nPosition, const char *psID, char *psParamValue, int *pnParamLen);

/**
* @brief 删除指定的参数文件，面向目前银联tms远程下载中的特定格式的参数文件或者是INI文件
* @param [in] pszFileName 参数文件名
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @date 2012-06-19
*/
int PubDelFile (const char *pszFileName);

/**
* @brief 获取字符在字符串中的位置
* @param [in] pszData     ---字符串
* @param [in] cSubChar    ---字符
* @return 
* @li APP_FAIL  ---不存在
* @li >=0   ---在字符串中位置值
* @author 刘罡
* @date 2012-7-23
*/
int PubPosChar(const char *pszData,char cSubChar);

/**
* @brief 获取子字符串在字符串中的位置
* @param [in] pszData     ---字符串
* @param [in] pszSub      ---子字符串
* @return 
* @li APP_FAIL  ---不存在
* @li >=0   ---在字符串中位置值
* @author 刘罡
* @date 2012-7-23
*/
int PubPosStr(const char *pszData,const char *pszSub);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOL
typedef unsigned char BOOL;
#endif

#define MAX_FIELD_LEN 1024		/**<每个域的最大长度*/
#define MAX_MSG_LEN 4096		/**<报文最大长度 */

typedef enum 
{	/**<域对齐方式 */
    ALIGN_TYPE_LEFT   = 0,
    ALIGN_TYPE_RIGHT  = 1
}EM_ALIGN_TYPE;

typedef enum 
{	/**<长度类型 */
    LENGTH_TYPE_FIX    = 0,
    LENGTH_TYPE_LLVAR  = 1,
    LENGTH_TYPE_LLLVAR = 2
} EM_LENGTH_TYPE;

typedef struct STISO_FIELD STISO_FIELD;
typedef struct STISO_FIELD_DEF STISO_FIELD_DEF;

/**
* @brief 对8583域进行打包，并返回打包后的数据和长度
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] fieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li	打包后的数据长度
*/
typedef int (*FIELD_PACKAGER_PACK) (STISO_FIELD_DEF *pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 将缓冲区里面的数据解包到8583域中
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li	解包时从pindata中使用的数据长度
*/
typedef int (*FIELD_PACKAGER_UNPACK) (STISO_FIELD_DEF *pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 根据ISO域的类型计算8583报文中的长度字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] len			域数据的长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区 
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li	无
*/
typedef void (*FIELD_LENGTH_SET) (STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/**
* @brief 根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含该域报文的数据指针
* @param [out] plen			用于接收计算后的长度
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li	无
*/
typedef void (*FIELD_LENGTH_GET) (STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

struct STISO_FIELD_LENGTH_CALC
{	/**< ISO 8583 数据域长度计算函数结构 */
    FIELD_LENGTH_SET	pSet;		/**< 设置长度时调用的函数 */
    FIELD_LENGTH_GET	pGet;		/**< 取出长度时调用的函数 */
}; 

struct STISO_FIELD
{	/**< ISO 8583 数据域结构 */
    int    nLen;
    int    nOff;					/**< 指向iso->databuf缓冲区的偏移，数据中不含长度类型的几个字节 */
};

struct STISO_FIELD_DEF
{	/**< ISO 8583 数据域定义 */
    int                     nLen;			/**< 数据域最大长度 */
    EM_LENGTH_TYPE		    lentype;		/**< 数据域长度类型: 1--LLVAR型 2--LLLVAR型 */
    EM_ALIGN_TYPE           align;			/**< 对齐类型 */
    char                    cPadchar;		/**< 补齐字符 */	
    FIELD_PACKAGER_PACK		pPack;			/**< 打包函数指针 */
    FIELD_PACKAGER_UNPACK	pUnpack;		/**< 解包函数指针 */
};  


typedef struct STISODataStru
{	/**< ISO 8583 报文结构定义 */
    STISO_FIELD		fld[256];
    char			sBitmap[256];
    unsigned char 	szDatabuf[MAX_MSG_LEN + MAX_FIELD_LEN];		/**< 多留一个域的长度，防止溢出 */
    int        		nUsed;
} STISODataStru;

/**
* @brief 根据ISO域的类型计算8583报文中的长度字节(ASC形式)。ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] nLen			域数据的长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLALengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/**
* @brief 根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(ASC形式)。ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含该域报文的数据指针
* @param [out] plen			用于接收计算后的长度
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLALengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/**
* @brief 根据ISO域的类型计算8583报文中的长度字节(BCD形式)。BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] nLen			域数据的长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLBLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/**
* @brief 根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(BCD形式)。BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含该域报文的数据指针
* @param [out] plen			用于接收计算后的长度
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLBLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/**
* @brief 根据ISO域的类型计算8583报文中的长度字节(HEX形式)。HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] nLen			域数据的长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLHLengthSet(STISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);

/**
* @brief 根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(HEX形式)。HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含该域报文的数据指针
* @param [out] plen			用于接收计算后的长度
* @param [out] pcount		长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
* @return 
* @li 无
*/
void PubFLHLengthGet(STISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

/**
* @brief 为一个ISO域的数据进行打包（MSG_ID域ASC码形式）。
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPAMsgIDPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（MSG_ID域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPAMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int  *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（MSG_ID域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBMsgIDpack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（MSG_ID域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf		解包后的8583域数据缓冲区
* @param [out] pfieldlen		返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBMsgIDUnpack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（BITMAP位图域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPABitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（BITMAP位图域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPABitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（BITMAP位图域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBBitmapPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（BITMAP位图域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBBitmapUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（字符类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPACharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（字符类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPACharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（字符类型域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] pfieldbuf		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBCharPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（字符类型域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBCharUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（数字类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPADigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（数字类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPADigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（数字类型域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBDigitPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（数字类型域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBDigitUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（金额类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBAmountPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（金额类型域ASC码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBAmountUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 为一个ISO域的数据进行打包（二进制类型域BCD码形式）
* @param [in] pthis			ISO域定义的类型结构指针
* @param [in] pfieldbuf		将要打包的8583域数据指针
* @param [in] nFieldlen		将要打包的8583域数据长度
* @param [out] poutdata		用于接收计算后的数据的缓冲区
* @return 
* @li 打包后的数据长度
*/
int PubFPBBinaryPack(STISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, int fieldlen, unsigned char *poutdata);

/**
* @brief 为一个ISO域的数据进行解包（二进制类型域BCD码形式）
* @param [in] pthis			SO域定义的类型结构指针
* @param [in] pindata		含有该域报文的数据缓冲区
* @param [out] pfieldbuf	解包后的8583域数据缓冲区
* @param [out] pfieldlen	返回域数据长度
* @return 
* @li 解包时从pindata中使用的数据长度
*/
int PubFPBBinaryUnPack(STISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, int *pfieldlen, unsigned char *pindata);

/**
* @brief 转换字符串为ISO结构
* @param [in] pcSourceStr		转换的源字符串
* @param [in] nStrLen			数据长度
* @param [out] pTargetIsoStru	转换的结果ISO结构
* @param [out] pnPosition		传入为NULL时，忽略;否则存放转换出错时的域编号（仅对返回ERRFIELDLEN时有效），其余情况下均为-1
* @return 
* @li APP_FAIL		转换失败，pcSourceStr或者TargetIsoStru为空，解包出错
* @li APP_APP_SUCC	转换成功
*/
int PubStrToIso(char *pcSourceStr, int nStrLen, STISODataStru *pTargetIsoStru, int *pnPosition);

/**
* @brief 将ISO结构转换成无符号字符串
* @param [in] pSourceIsoStru	转换的源数据结构
* @param [in] pnLen				传入pcTargetStr的空间大小（包含结束符）
* @param [out] pcTargetStr		转换的结果字符串(BCD码)
* @param [out] pnLen			存放转换成功后的字符串长度，如果失败，长度为-1。
* @return 
* @li APP_FAIL		转换失败，pcTargetStr或者pSourceIsoStru为空，打包失败
* @li APP_APP_SUCC	转换成功
*/
int PubIsoToStr(char *pcTargetStr, STISODataStru *SourceIsoStru, int *pnLen);

/**
* @brief 将结构ISO中指定成员提取到字符串szTargetStr中
* @param [in] pSourceIsoStru	源数据结构ISO_data 结构指针
* @param [in] nPosition			ISO_data结构中的第N个成员
* @param [in] pnLen				传入要转换字串的空间大小（包含结尾符号）
* @param [out] pcTargetStr		目标字符串
* @param [out] pnLen			返回转换成功时字符串的长度，转换失败且pnLen非空时为-1
* @return 
* @li APP_FAIL		失败,TargetIsoStru或者pcSourceStr或者pnLen为空，或者调用HexToAsc出错，或者指定的成员无效，或者传入的pcSourceStr空间不够
* @li APP_APP_SUCC	成功
*/
int PubGetBit(STISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen);

/**
* @brief 将字符串pcSourceStr按指定长度nStrLen填充到结构TargetIsoStru中指定成员
* @param [in] nPosition			指定的ISO_data结构中的第N个成员
* @param [in] pcSourceStr		打包数据
* @param [in] nStrLen			pcSourceStr 的长度
* @param [out] pTargetIsoStru	待填充的目标结构ISO_data 结构的指针
* @return 
* @li APP_FAIL		失败，TargetIsostru或者pcSourceStr为NULL，或者指定的成员无效，或者调用AscToHex出错，或者ISO结构累计长度超过MAXISOLEN
* @li APP_APP_SUCC	设置成功或者nStrLen 等于0
*/
int PubSetBit(STISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);

/**
* @brief 清空ISO_data结构中所有成员变量值
* @param [in] pIsoStru		源数据结构ISO_data 结构指针
* @param [out] pIsoStru		清空后的ISO_data结构指针
* @return 
* @li APP_FAIL		失败，IsoStru为空
* @li APP_APP_SUCC	成功
*/
int PubDelAllBit(STISODataStru *pIsoStru);

/**
* @brief 清空IsoStru结构中第nPosition域的值，同时清除位图中的相应标志
* @param [in] pIsoStru		源数据结构ISO_data 结构指针
* @param [in] nPosition		ISO_data结构中的第nPosition个成员
* @param [out] pIsoStru		清除后的ISO_data结构指针
* @return 
* @li APP_FAIL		失败，IsoStru为NULL，或者无效的成员号（nPosition <0 或者 >=256）
* @li APP_APP_SUCC	成功
*/
int PubDelOneBit(STISODataStru *pIsoStru, int nPosition);

/**
* @brief 设置8583数据包定义
* @param [in] nPosition		域号
* @param [in] nMaxLen		最大长度
* @param [in] LenType		长度类型
* @param [in] AlignType		对齐类型
* @param [in] chPadChar		补齐字符
* @param [in] PackFunc		打包函数
* @param [in] UnpackFunc	解包函数
* @return 
* @li APP_FAIL		失败
* @li APP_APP_SUCC	成功
*/
int PubSetDefine8583(int nPosition, int nMaxLen, EM_LENGTH_TYPE	LenType, EM_ALIGN_TYPE AlignType, char chPadChar, FIELD_PACKAGER_PACK PackFunc, FIELD_PACKAGER_UNPACK UnpackFunc);

/**
* @brief 导入8583结构
* @param [in] pNewDef		要导入的8583结构
* @return 
* @li APP_FAIL		失败
* @li APP_APP_SUCC	成功
*/
int PubImport8583(const struct STISO_FIELD_DEF *pNewDef);

/**
* @brief 按域显示8583数据包内容
* @param [in] pIsoStru		待显示的ISO 结构
* @return 
* @li 无
*/
void PubFormatDisp8583(STISODataStru *pIsoStru);

/**
* @brief 导入8583域长度计算函数结构
* @param [in] pNewCalc		要导入的8583域长度计算函数结构
* @return 
* @li APP_FAIL		失败
* @li APP_APP_SUCC	成功
*/
int PubSetFieldLengthCalc(const struct STISO_FIELD_LENGTH_CALC *pNewCalc);

/**
* @brief 复制一个STISODataStru结构
* @param [in] pSourceIsoStru		源ISO_data结构的指针
* @param [out] pTargetIsoStru		待填充的目标结构ISO_data 结构的指针
* @return 
* @li APP_FAIL		失败，pSourceIsoStru或者pTargetIsoStru为空
* @li APP_APP_SUCC	设置成功
*/
int PubCopyIso(STISODataStru *pSourceIsoStru, STISODataStru *pTargetIsoStru);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @brief 通过串口1导出日志文件
* @detail 串口：1,115200
* @param 无
* @return 
* @li 无
* @author 刘罡
* @date 2012-10-24
*/
void PubExportDebugFile();

/**
* @brief 通过将二进制数据调试信息输出
* @param [in] pszTitle  --- 标题
* @param [in] pData     --- 数据
* @param [in] nLen      --- 数据长度
* @return 
* @li 无
* @author 刘罡
* @date 2012-10-24
*/
void PubDebugData(const char* pszTitle,const void* pData,int nLen);

/**
* @brief 将调试信息导出
* @param [in]  lpszFormat   --- 格式化字符串
* @return 
* @li 无
* @author 刘罡
* @date 2012-10-24
*/
void PubDebug(char* lpszFormat, ...);

/**
* @brief 选择性输出调试信息
* @param [in] cLevel 调试等级1-3 , 3为最低级
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 
*/
int PubDebugSelectly(char cLevel, char* lpszFormat, ...);

/**
* @brief 获取工具库Debug模式
* @return 
* @li 0:  发布模式:无调试输出
* @li 1:  Debug模式  :串口输出
* @li 2:  Debug模式  :文件输出
*/
int PubGetDebugMode();

/**
* @brief 设置工具库调试模式
* @param [in] nMode  后四节 调试模式 0---开关调试  1---串口调试 2---文件调试
                     前四节 调试级别 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
*/
int PubSetDebugMode(int nMode);

/**
* @brief 从掉电保护区提取数据
* @param [out]  psBuf   --- 数据
* @param [in]  nStart      --- 起始地址
* @param [in]  nLen        --- 数据长度
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 失败
* @author 刘罡
* @date 2012-10-24
*/
int PubGetVar (char *psBuf, const int nStart, const int nLen);

/**
* @brief 将数据保存到掉电保护区
* @param [in]  psSaveBuf   --- 数据
* @param [in]  nStart      --- 起始地址
* @param [in]  nLen        --- 数据长度
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 失败
* @author 刘罡
* @date 2012-10-24
*/
int PubSaveVar (const char *psSaveBuf, const int nStart, const int nLen);

/**
* @brief 获取错误码
* @param [out]  pnErrorCode			模块错误码
* @param [out]  pszErr					模块错误码提示信息
* @param [out]  pnNDKErrorCode		NDK错误码
* @return 
* @li 无
*/
void PubGetErrorCode(int *pnErrorCode, char *pszErr, int *pnNdkErrorCode);

/**
* @brief 设置错误码
* @param [in]  nErrCode			模块错误码
* @param [in]  pszErrTip			模块错误码提示信息
* @param [in]  nNdkErrCode		NDK错误码
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-10-22
*/
void PubSetErrorCode(int nErrCode,const char *pszErr,int nNdkErrCode);

/**
* @brief 设置错误信息
* @param [in]  nErrorCode			模块错误码
* @param [in]  nNDKErrorCode		NDK错误码
* @param [in]  ppszErr				错误信息(二维数组)
* @return 
* @li 无
*/
void PubSetErrorInfo(int nErrCode,const char *ppszErr[MAX_ERR_BUF_LEN],int nNdkErrCode);

/**
* @brief POS响声
* @param [in] nTimes	响声次数
* @return 
* @li 无
*/
void PubBeep(int nTimes);

/**
* @brief POS关机
* @return
* li APP_SUCC	成功 
* li APP_FAIL   失败
*/
int PubShutDown();

/**
* @brief 启动跑表，开始计时
* @return
* li APP_SUCC	成功 
* li APP_FAIL   失败
*/
int PubStartWatch();

/**
* @brief 停止跑表并保存计数值 
* @param [in] punTime   跑表结束时的计数值 (毫秒)
* @return
* li APP_SUCC	成功 
* li APP_FAIL   失败
*/
int PubStopWatch(uint * punTime);

/**
* @brief 获取POS硬件支持信息
* @param [in] emFlag	EM_HARDWARE_SUPPORT 枚举各种硬件
* @param [out] pszBuf	对应的硬件信息
* @return 
* @li APP_FAIL		不支持该硬件
* @li APP_APP_SUCC	支持该硬件
* @date 2012-11-13
*/
int PubGetHardwareSuppot(int emFlag, char* pszBuf);

/**
* @brief 获取POS硬件信息
* @param [in] emFlag	EM_HARDWARE_GET 枚举各种硬件
* @param [out] pszBuf	对应的硬件信息
* @return 
* @li APP_FAIL		失败
* @li APP_APP_SUCC	成功
*/
int PubGetPosInfo(int emFlag, char* pszBuf);

/**
* @brief 将输入数据以QR算法加密后生成位图
* @param [in] stDataInfo	
				nLen		要进行编码的数据长度
				nLevel		纠错级别(默认1，范围0到3)
				nMask		掩码号(默认-1，范围0到7)
				nVersion	型号(默认0，范围0到40)
				szInputData	要进行编码的数据
* @param [out] stBmpInfo	
				xsize		图形的宽度（像素）
				ysize		图形的高度（像素）
				bmpbuff		图象点阵数据
* @return 
* @li APP_FAIL		失败
* @li APP_APP_SUCC	成功
*/
int PubEncodeDataAndGenerateBmp(DataInfo* stDataInfo, BmpInfo* BmpBuff);

/**
* @brief ASC转UTF8
* @param [in] DataInfo	
* @param [out] DataInfo
* @return 无
*/
void PubAsciiToUtf8(DataInfo* stDataInfo);

/**
* @brief 不插电源时进入休眠(此时无读卡，无通讯PPP，无USB通讯，无打印)，
* 在NDK4.4.8版本以后，随系统休眠(读卡,通讯PPP状态可休眠)
* @param [in] 无
* @param [out] 无
* @return 
* @li 无
*/
void PubGoSuspend();

#endif

