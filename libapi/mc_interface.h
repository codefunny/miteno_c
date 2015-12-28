
#ifndef _MC_INTERFACE_H_
#define _MC_INTERFACE_H_

#include <time.h>

enum MC_PARAM_TYPE{
	MODEM_PRE_DAIL_NUM=0,         /**<外线号码*/
	APP_POSID,                    /**<终端号*/
	APP_MERCHANTID,               /**<商户号*/
	APP_ADVERTISEMENT,            /**<广告*/
	APP_DISPNAME,                 /**<应用别名*/
	DOWN_REDIAL_REVERSAL,		/**<重拨间隔时间*/
	DOWN_WARNING_REVERSAL,		/**<任务提醒时间间隔*/
	DOWN_ONLINE_WAITTIME,		/**<联机等待时间*/
	DOWN_SIGN,						/**<下载内容标志*/
	APP_VER,						/**<应用版本号*/
	CHECK_CODE,					/**<下载任务校验码*/
	SAVE_APP_PARA,				/**<保存通讯参数*/
	DOWN_END_FLAG,				/**<下载结束标志*/
	UN_DEF_TYPE=0xFF		      /**<未定义类型*/
};

enum MC_FUNC_TYPE{
	EXEC_COMM_SET=0,		/**<通讯参数设置*/
	EXEC_COMM_CONNECT,		/**<通讯连接*/
	EXEC_COMM_SEND,			/**<调用主控的发送数据*/
	EXEC_COMM_RECV,			/**<调用主控的接收数据*/
	EXEC_COMM_WRITE,        /**<调用主控的PubCommWrite函数*/
	EXEC_COMM_READ,			/**<调用主控的PubCommRead函数*/
	EXEC_COMM_HANGUP,		/**<通讯挂断*/
	EXEC_DOWNLOAD_PROGRAM,  /**<下载程序*/
	EXEC_UPDATE_PROGRAM,    /**<更新程序*/
	EXEC_INFO_UP,		/**<信息上送*/
	EXEC_INFO_DOWN	/**<信息下载*/
};

enum MC_COMMTYPE
{
	MC_COMM_RS232 = 1,			/**<串口*/
	MC_COMM_ASYNCHDIAL,		/**<异步拨号*/
	MC_COMM_DIAL,				/**<NAC*/
	MC_COMM_ETH,				/**<以太网*/
	MC_COMM_GPRS,				/**<GPRS*/
	MC_COMM_CDMA,				/**<CDMA*/
	MC_COMM_PPP,				/**<ppp*/
	MC_COMM_UNDEF	=0xFF		/* 未定义，由主控指定       */
};

#define ALLOW_UPD_CURR_APP				1
#define DISALLOW_UPD_CURR_APP			0

#define TMS_DOWNLOAD_TIME_IS_UP		1		/*下载时间到*/
#define TMS_WITHOUT_DOWNLOAD			0		/*不需要下载*/

#define MAX_SUPPORT_APP_NUM		32		/**<应用程序支持的最大应用个数，包含主控与库*/



/**<应用相关文件信息 */
#define MAX_FILE_NUM		10
#define POS_FILE_NAME_LEN	8
typedef struct  {
	/**<应用程序文件 */
	char nIsCompressed;		                                   /**<应用压缩模式*/
	char nAppIndex;				                               /**<应用序号*/
	char szAppFileName[128 + 1];	               /**<应用名称，""空串表示无需下载应用*/
	
	/**<参数文件 */
	char nParamIndex;				                           /**<参数文件序号*/
	char szParamFileName[128 + 1];               /**<参数名称，""空串表示无需下载应用*/
	
	/**<数据文件 */
	char nDataFileNum;			                               /**<数据文件数目*/
	char nDataIndex[MAX_FILE_NUM];                             /**<数据文件索引号,与文件名数组对应*/
	char szDataFileName[MAX_FILE_NUM][128 + 1];  /**<应用列表数组*/
	
	/**<其他文件 */
	char nOtherFileNum;			                                 /**<其他文件数目*/
	char nOtherIndex[MAX_FILE_NUM];                              /**<其他文件索引号*/
	char szOtherFileName[MAX_FILE_NUM][128 + 1];   /**<其他文件文件名称数组*/
}STTmsFileInfo;

typedef STTmsFileInfo * PSTTmsFileInfo;

typedef struct {
	STTmsFileInfo stTmsFileInfo;
	char *pszUmsTel;                                    /**<提示的电话号码*/
	char szUpdataTime[14+1];                            /**<应用的更新时间*/
	int szTimes;                                        /**<延时提示的时间间隔*/
	char *pcIsWarning;                                  /**<是否提示*/
	int nNotSettleAppNum;                               /**<未结算的应用数量*/
	char *pszNotSettleAppNameList[MAX_SUPPORT_APP_NUM]; /**<指向未结算的应用*/
}STMCUPDATAINFO;
typedef STMCUPDATAINFO * PSTMCUPDATAINFO;

typedef struct {
	char nCommType;				    // 通讯类型		0xFF 未知，使用主控中TMS设置的参数
	char szTel1[20+1];
	char szTel2[20+1];
	char szIpAndPort1[30+1];
	char szIpAndPort2[30+1];
	char szGprsParam[60+1];
	char szCdmaParam[60+1];
	char szTpdu[10];
	char cIsUseAppIP;					/**<是否使用子应用的本机IP地址*/
	char szIpAddr[16+1];				/**<本机ip地址An16,与子应用一致*/
	char szMask[16+1];					/**<子网掩码	An16*/
	char szGate[16+1];					/**<网关	An16*/
}STTMSDOWN;
typedef STTMSDOWN * PSTTMSDOWN;

/**
* @brief 获取由主控保存的公共参数
* @param in nParamType 参数类型
* @param in psOBuf	  参数输出缓冲
* @param in punDataLen 输入时是缓冲区最大允许长度，输出为赋值的长度
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_GetPubParam(int nParamType, char *psOBuf, unsigned int *punDataLen);

/**
* @brief 获取由主控保存的公共参数
* @param in nParamType 参数类型
* @param in pszIBuf	  参数输入缓冲
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_SetPubParam(int nParamType, const char *psIBuf,unsigned int unInLen);

/**
* @brief 获取进入主控的时间与保存的随机数
* @param in ulTimes 主控运行的时间
* @param in unRandomNum	  保存的随机数
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_GetEnterTimes(long *plTimes,int *pnRandomNum);

/**
* @brief 修改POS时间
* @param in pt 要保存的时间结构
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_SetPosTime(struct tm pt);

/**
* @brief 获取主控程序版本
* @param in pszVer 主控版本串
* @return 
* @li APP_SUCC 成功
*/
int MC_GetVer(char *pszVer);

/**
* @brief 获取主控程序版本
* @param in pszVer 主控版本串
* @return 
* @li APP_SUCC 成功
*/
int MC_GetLibVer(char *pszVer);


/**
* @brief 执行主控功能函数
* @param in nFuncType 函数类型
* @param in psInBuf	  参数输入缓冲
* @param in nInLen	  参数输入长度
* @param out psOutBuf   参数输出缓冲
* @param out pnOutLen   参数输出长度
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_ExecFunc(int nFuncType,const char *psInBuf,int nInLen,char *psOutBuf,int *pnOutLen);


/**
* @brief 查询应用的升级信息
* @param [out] pstMcUpdataInfo ---应用升级的信息
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 失败
*/
int MC_TMSQueryInfo(STMCUPDATAINFO *pstMcUpdataInfo);


/**
* @brief 更新文件
* @param [in]  nIndex        文件索引
* @param [in]  pszFileName	 文件名
* @param [in]  ProcFile      外部执行函数
* @return 
* @li APP_SUCC 成功
* @li APP_FAIL 成功
*/
int MC_TMSUpdateFile(int nIndex, char *pszFileName, int (*ProcFile)(char *));

/**
* @brief TMS允许应用升级标志控制 1 允许升级 0 禁止升级
* @param in	无
* @return 
* @li APP_SUCC	成功
*/
int MC_TMSAllowAppUpdCtrl(char cFlag);

#endif

