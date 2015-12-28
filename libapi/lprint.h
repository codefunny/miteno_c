/******************************************************************************
* 新大陆电脑公司 版权所有(c) 2005-2008
*
* POS统一版
* 打印模块      ---lprint.h
* 作    者：    刘罡
* 日    期：    2012-07-12
* 最后修改人：  刘罡
* 最后修改日期：2012-07-22
******************************************************************************/
#ifndef _LPRINT_H_
#define	_LPRINT_H_


/**
*	打印数据域的字段表
*/
#define MAX_PRINT_TAG_NAME	32
#define PRINT_DATA_MAXLEN	128
#define MAX_PRINT_MODULE_NUM  40
#define MAX_IMAGE_BLOCK   2000    //图片数组数据大小2000 * 128,暂定,有需要可联系更改

/**
*   错误码定义
*/
typedef enum{

PRN_OK,                                      /**操作成功*/                                   
PRN_ERR = -4001,				 /**操作失败*/ 
PRN_ERR_FILE_LOST = -4002,         /**文件缺失*/
PRN_ERR_FILE_OPEN = -4003,		 /**打开文件失败*/ 
PRN_ERR_FILE_READ = -4004,		 /**读取文件失败*/ 
PRN_ERR_FILE_WRITE = -4005,	 /**写入文件失败*/ 
PRN_ERR_FORMAT = -4006,              /**模板文件格式有误*/
PRN_ERR_MODULEINFO = -4007,	 /**保存的模块信息有误*/
PRN_ERR_INITPNT = -4008,             /**初始化打印机失败*/
PRN_ERR_IMG_OPER = -4009,           /**处理图片数据出错*/
PRN_ERR_IMG_DATA = -4010,         /**缺少图片数据*/
PRN_ERR_FONT_ERR = -4011,         /**字体设置错误*/

}EM_PRINT_ERR;

#if 0
typedef struct STPRINTFIELD
{
    char szTagName[MAX_PRINT_TAG_NAME+1];
    char szData[PRINT_DATA_MAXLEN+1];		
}STPRINTFIELD;
#endif

//打印机类型
enum _PRINTTYPE_DEF{
	_PRINTTYPE_TP=0, 	//热敏
	_PRINTTYPE_IP, 	 	//针打
};

typedef enum _PRINTFONTSIZE
{
	PRN_FONT_SMALL = 0, //小字体
	PRN_FONT_MIDDLE,    //中
	PRN_FONT_BIG,       //大
}EM_PRN_FONT_SIZE;
/**
* @brief 初始化打印模板,调用PubPrintModule前需要调用,可放在最前面
* @param [in] pszFileName 打印 模板文件
* @param [in] unPageLen 一页打印纸长(针打有效,热敏不起作用)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林金珠
* @date 2012-11-14
*/
int PubInitPrint(const char* pszFileName, uint unPageLen);

/**
* @brief 模块打印
* @param [in] pszModuleName, 要打印的模块名
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林金珠
* @date 2012-11-14
*/
int PubPrintModule(const char* pszModuleName);

/**
* @brief获得打印机类型
* @param 无
* @return 打印机类型
*/
int PubGetPrinter(void);

/**
* @brief 清空打印数据域值
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-05-06
*/
int PubClearFieldValue(void);

/**
* @brief 设置打印数据域值
* @param [in] pszTagName 标签
* @param [in] pszData 标签对应的域值
* @param [in] nDataLen pszData的长度
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-05-06
*/
int PubSetFieldValue(const char *pszTagName, const char *pszData, const int nDataLen);
/**
* @brief 设置打印数据域值,DATA是十六进制时使用
* @param [in] pszTagName 标签
* @param [in] pszData 标签对应的域值
* @param [in] nDataLen pszData的长度
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-05-06
*/
int PubSetFieldValueHex(const char * pszTagName, const char * pszData, const int nDataLen);
/**
* @brief 设置打印数据域值,VALUE可格式化传入
* @param [in] pszTagName 标签
* @param [in] lpszFormat
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-05-06
*/
int PubSetFieldValueFormat(const char *pszTagName, char* lpszFormat, ...);
/**
* @brief 设置打印数据域值,图片数组专用
* @param [in] pszImgTag图片 标签
* @param [in] pszImgBuf 图片数组
* @param [in] nImgSize 图片数组大小
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-05-06
*/

int PubSetFieldValueImage(const char *pszImgTag, const char *pszImgBuf, const ulong ulImgSize);

/**
* @brief 设置图片打印参数,代码控制,而不在模板配置(图片文件)
* @param [in] pszParamTag,标识参数的tag
* @param [in] pszPicName 图片名
* @param [in] unXpos 打印位置,图片左上角位置
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-09-24
*/
int PubSetParamLogo(const char *pszParamTag,const char *pszPicName, uint unXpos);

/**
* @brief 设置图片打印参数,代码控制,而不在模板配置(图片数组)
* @param [in] pszParamTag,仅标识参数,而图片数组还需调用PubSetFieldValueImage设置
* @param [in] unXsize 图片宽
* @param [in] unYsize 图片高
* @param [in] unXpos 打印位置,图片左上角位置
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2013-09-24
*/
int PubSetParamImg(const char *pszParamTag, uint unXsize,uint unYsize,uint unXpos);

/**
* @brief 设置打印字体size(小,中,大),代码控制,配合模板(S-S,M,B)配置
* @param [in] EM_PRN_FONT_SIZE,打印字体size,见EM_PRN_FONT_SIZE
* @return 
* @li APP_SUCC 成功
* @author 林礼达
* @date 2014-06-27
*/
int PubSetPrnFontSize(EM_PRN_FONT_SIZE emSize);


/**
* @brief 检测打印机状态
* @param 无
* @return 
* @li 打印状态值
* @author 刘罡
* @date 2012-9-19
*/
int PubGetPrintStatus(void);


/**
* @brief 设置打印结构
* @param [in] unBorder  左边距 值域为：0-288(默认为0)  
* @param [in] unColumn  字间距 值域为：0-255(默认为0)  
* @param [in] unRow     行间距 值域为：0-255(默认为0)  
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-09-19
*/
int PubSetPrintForm (uint unBorder, uint unColumn, uint unRow);

/**
* @brief 设置打印字体
* @param [in] emDotFont       英文字体
* @param [in] emHZFont        汉字字体
* @param [in] emMode          打印模式
* @param [in] unSigOrDou      单向还是双向打印(仅对针打有效)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-09-19
*/
int PubSetPrintFont (EM_PRN_ZM_FONT emDotFont,EM_PRN_HZ_FONT emHZFont,EM_PRN_MODE emMode,uint unSigOrDou);


/**
* @brief 组织打印单行内容
* @details	若数据超过一行则不用加换行符。自动换行.
* @param [in] pszContent   打印内容
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-6-5
*/
int PubPrePrinter(const char *pszContent, ...);


/**
* @brief 换行
* @param [in] nLine   换行的行数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-6-5
*/
int PubPaperPeed(int nLine);


/**
* @brief 设置PubPrintCommit函数中打印机退纸
* @param [in] nFeedPaper  0 单页打印前退纸, 1 单页打印完成后进纸
* @return 
* @无
* @author 刘罡
* @date 2014-04-08
*/
void PubPrintFeedPaper(int nFeedPaper);


/**
* @brief 提交打印
* @details	正式开始打印
* @param [in] (*SendPrintData)() ---函数指针（需打印的数据）
* @param [in] pvParam            ---函数入参
* @param [in] unPrintType        ---打印类型 0:一次性打印,1:边送边打
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-9-20
*/
int PubPrintCommit(int (*SendPrintData)(), void *pvParam,uint unPrintType);


/**
* @brief 打印位图图片
* @param [in] unXSize:图形的宽度
* @param [in] unYSize:图形的高度
* @param [in] unXPos:图形的左上角的列位置
* @param [in] psPrintBuf:位图资源
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-9-19
*/
int PubPrintImage(uint unXSize, uint unYSize, uint unXPos,const char *psPrintBuf);


/**
* @brief 打印图片
* @details	打印bmp，png等格式的图片
* @param [in] unXPos:图形的左上角的列位置
* @param [in] pszPicPath:图片路径
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-9-19
*/
int PubPrintPicture(uint unXPos,const char *pszPicPath);

/**
* @brief 获取打印库版本
* @param pszOutVer 返回版本串
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PubGetPrintLibVer(char *pszOutVer);

/**
* @brief低电压检测
* @param 无
* @li APP_SUCC 低电压
* @li APP_FAIL 
*/
int PubLowPowerCheck(void);

#endif
/* End of lprint.h */

