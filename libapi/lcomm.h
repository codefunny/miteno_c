/*
* 新大陆电脑公司 版权所有(c) 2006-2008
*
* POS统一版公共库
* 公用头文件  --- lcomm.h
* 作    者：    刘罡
* 日    期：    2012-09-19
* 最后修改人：  刘罡
* 最后修改日期：2012-10-11
*/
#ifndef _LCOMM_H_
#define _LCOMM_H_

#include "NDK.h"

typedef enum 
{
	SERVERMODE_RECVQUIT = (1<<0),				/**<接收返回支持退出*/
	SERVERMODE_CLOSENOWAIT = (1<<1),	   		/**<后台执行通讯关闭*/
	SERVERMODE_INITNOWAIT = (1<<2),		   	/**<后台执行初始化*/
	SERVERMODE_CLOSEWAIT = (1<<3),		   	/**<前台执行通讯关闭*/
	SERVERMODE_INITWAIT = (1<<4),		   		/**<前台执行初始化*/
}EM_SERVERMODE;

typedef enum {
	COMMTYPE_SYNDIAL=0x00,  			   	/**<同步拨号*/
	COMMTYPE_SYNDIAL_TPDU,    				/**<同步拨号+TPDU*/
	COMMTYPE_SYNDIAL_HEADLEN, 			/**<同步拨号+两个字节头长度*/  
	COMMTYPE_SYNDIAL_TPDU_HEADLEN,    	/**<同步拨号+TPDU+两个字节头长度*/
	
	COMMTYPE_ASYNDIAL=0x10,  			   	/**<异步拨号*/
	COMMTYPE_ASYNDIAL_ASYN, 				/**<异步拨号:0x02开头+[两字节长度]+[数据]+[lrc]+0x03结尾*/  
	COMMTYPE_ASYNDIAL_ASYN_TPDU,      		/**<异步拨号:0x02开头+[两字节长度]+TPDU+[数据]+[lrc]+0x03结尾*/
	COMMTYPE_ASYNDIAL_ASYN1, 			    	/**<异步拨号:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_ASYNDIAL_ASYN1_TPDU,     	/**<异步拨号:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/
	
	COMMTYPE_PORT=0x20,  			  		/**<串口*/
	COMMTYPE_PORT_ASYN, 				  	/**<串口:0x02开头+[两字节长度]+[数据]+[lrc]+0x03结尾*/  
	COMMTYPE_PORT_ASYN_TPDU,           		/**<串口:0x02开头+[两字节长度]+TPDU+[数据]+[lrc]+0x03结尾*/
	COMMTYPE_PORT_ASYN1, 			  		/**<串口:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_PORT_ASYN1_TPDU,     	  	/**<串口:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/
	
	COMMTYPE_GPRS=0x30,  			  		/**<GPRS*/
	COMMTYPE_GPRS_HEADLEN, 			  	/**<GPRS+两个字节头长度*/  
	COMMTYPE_GPRS_TPDU_HEADLEN,        	/**<GPRS+TPDU+两个字节头长度*/
	COMMTYPE_GPRS_ASYN1, 			    	/**<GPRS:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_GPRS_ASYN1_TPDU,     	/**<GPRS:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/
	
	COMMTYPE_CDMA=0x40,  			  		/**<CDMA*/
	COMMTYPE_CDMA_HEADLEN, 			  	/**<CDMA+两个字节头长度*/  
	COMMTYPE_CDMA_TPDU_HEADLEN,        	/**<CDMA+TPDU+两个字节头长度*/
	COMMTYPE_CDMA_ASYN1, 			    	/**<CDMA:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_CDMA_ASYN1_TPDU,     	/**<CDMA:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/

	COMMTYPE_ETH=0x50,  				 	/**<ETH*/
	COMMTYPE_ETH_HEADLEN, 			  	/**<ETH+两个字节头长度*/  
	COMMTYPE_ETH_TPDU_HEADLEN,          	/**<ETH+TPDU+两个字节头长度*/
	COMMTYPE_ETH_ASYN1, 			    		/**<ETH:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_ETH_ASYN1_TPDU,     		/**<ETH:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/

	COMMTYPE_USB=0x60,  			  		/**<USB*/
	COMMTYPE_USB_ASYN, 				  	/**<USB:0x02开头+[两字节长度]+[数据]+[lrc]+0x03结尾*/  
	COMMTYPE_USB_ASYN_TPDU,           		/**<USB:0x02开头+[两字节长度]+TPDU+[数据]+[lrc]+0x03结尾*/
	COMMTYPE_USB_ASYN1, 			  		/**<USB:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_USB_ASYN1_TPDU,    	  		/**<USB:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/

	COMMTYPE_WIFI=0x70,  			  		/**<WIFI*/
	COMMTYPE_WIFI_HEADLEN, 			  	/**<WIFI+两个字节头长度*/  
	COMMTYPE_WIFI_TPDU_HEADLEN,        		/**<WIFI+TPDU+两个字节头长度*/
	COMMTYPE_WIFI_ASYN1, 			    		/**<WIFI:0x02开头+[两字节长度]+[数据]+0x03结尾+[lrc]*/  
	COMMTYPE_WIFI_ASYN1_TPDU,     			/**<WIFI:0x02开头+[两字节长度]+TPDU+[数据]+0x03结尾+[lrc]*/

}EM_COMMTYPE;


typedef struct
{
	char	szPredialNo[10];  		/**<外线号码*/
	char	lszTelNo[3][21];  			/**<3组电话号码*/
	char  lsTPDU[3][5];     			/**<3组TPDU*/
	int	nCycTimes; 		    		/**<循环拨号次数*/
	int   nCountryid ; 	    			/**<modem国家号*/
}STDIALPARAM;

typedef struct
{
	int	 nAux;  					/**<串口号*/
	char szAttr[20+1];      			/**<属性:例如："115200,8,N,1"*/
	char sTPDU[5];   	 			/**<TPDU*/
}STPORTPARAM;

typedef struct
{
	char	lszIp[2][16]; 		  		/**<服务器IP地址---2组*/
	int lnPort[2]; 			/**<服务端口号  ---2组*/
	char	szDN[50+1];         			/**<服务器域名*/
	char	szDNSIp[32+1];      		/**<域名服务器IP*/
}STSERVERADDRESS;

typedef struct 
{
 	char  szPinPassWord[20];                 /**<SIM卡密码---GPRS和CDMA必须*/
	char  szNetUsername[40]; 		/**<无线modem建立PPP链路时要用到的用户名---2组 */
	char  szNetPassword[20]; 			/**<无线modem建立PPP链路时要用到的密码 ---2组*/
	char  szModemDialNo[21]; 		/**<无线modem建立PPP链路时使用的接入号码 ---2组*/
	char  szGprsApn[40]; 				/**<移动APN名称---2组*/
	char  sTPDU[5];                    		/**<TPDU*/
}STGPRSPARAM;

typedef struct 
{
	char  szPinPassWord[20];                  /**<SIM卡密码---GPRS和CDMA必须*/
	char  szNetUsername[40]; 		 /**<PPP用户名---CDMA必须 */
	char  szNetPassword[20]; 			 /**<PPP密码---CDMA必须*/
	char  szModemDialNo[21]; 		 /**<无线modem建立PPP链路时使用的接入号码 ---2组*/
	char  sTPDU[5];                    		 /**<TPDU*/
}STCDMAPARAM;

typedef struct
{
	int nDHCP;						/**<是否使用DHCP*/
	char  szIP[16];					/**<本地IP地址*/
	char  szGateway[16];				/**<本地网关*/
	char  szMask[16];		  			/**<本地子网掩码*/
	char  szDNS[16];          			/**<DNS*/
	char sTPDU[5];            	  			/**<TPDU*/
}STETHPARAM;

typedef struct
{
	uchar ucIfDHCP;             /**<是否使用DHCP*/
	EM_WIFI_KEY_TYPE emKeyType; /**<路由器的密码格式*/
	EM_WIFI_NET_SEC emSecMode;  /**<路由器的安全加密模式*/
	char szSsid[32];               /**<路由器的Ssid,即热点名称*/
	char szKey[32];               /**<路由器的密码*/
	char szIP[16];              /**<终端IP地址*/
	char szMask[16];         /**<网络掩码地址*/
	char szGateway[16];         /**<网络网关地址*/
	char szDnsPrimary[16];      /**<网络首DNS地址*/
	char sTPDU[5];            	  			/**<TPDU*/
} STWIFIPARAM;

/**
* @struct STCOMMPARAM 应用通讯类参数
*/
typedef struct
{
	char cCommType;				      	/**<通讯类型见enum EM_COMMTYPE*/
	char cPreDialFlag;				  	/**<预拨号标识1预拨号，0无预拨号*/
	char cReDialNum;				  	/**<重拨次数*/
	int  nTimeOut;					  	/**<超时时间*/
	char cMode;						    	/**<长短链接标志*/
	char cIsSupportQuitRecv;                     /**<COMMSERVER模式 见EM_SERVERMODE枚举，同时支持用|*/
	union
	{
		STDIALPARAM stDialParam;  		/**<拨号参数结构体*/
		STPORTPARAM stPortParam;  	/**<串口参数结构体*/
		STGPRSPARAM stGprsParam;  	/**<Gprs参数结构体*/
		STCDMAPARAM stCdmaParam;  	/**<Cdma参数结构体*/
		STETHPARAM  stEthParam;   		/**<以太网参数结构体*/
		STWIFIPARAM  stWifiParam;   		/**<wifi参数结构体*/
	}ConnInfo;
	STSERVERADDRESS stServerAddress; /**<服务器参数*/
	void ( *ShowFunc )( void ) ;				/**<阻塞时显示的界面信息*/
	char cSslFlag;                      /**<ssl标志: 0.禁用ssl ; 1.ssl单向认证; 2.ssl双向认证*/
	char sRfu[39];
}STCOMMPARAM;

typedef struct
{
    int nColumn; /*列(1-10)*/
    int nRow; /*行*/
    int nType; /*显示类型 0递增 1递减*/
}STSHOWINFOXY; /*读秒显示风格结构*/

typedef union
{
	NDK_HANDLE vHandle;
	int nHandle;
}STHANDLE;

typedef struct
{
	int nType;                  	/*握手协议类型 见EM_SSL_HANDSHAKE_OPT*/
	int nAuthOpt;			/*认证模式 见EM_SSL_AUTH_OPT*/
	int nFileFormat;           	/**证书文件格式 见EM_SSL_FILE_FORMAT*/
	char szPwd[16];          	/**私钥密码*/
	char	 szServerCert[20];  	/**<CA证书文件名*/
	char	 szClientCert[20];  	/**<本地证书文件名*/
	char	 szClientPrivateKey[20];  /**<本地私钥名*/
}STSSLMODE;

typedef struct
{
	char szSubjectName[256];                  	/*证书拥有着信息*/
	char szIssuername[256];				/*证书颁发者信息*/
	char nVersion;           			/**证书版本号*/
	char szNotBeforeTime[16];          	/**证书生效时间*/
	char szNotAfterTime[16];          		/**证书过期时间*/
	char szCountryName[32];           		/**证书国家名称*/
	char szCommonName[32];           			/**证书主机名称*/
	char	 szOrganizationName[64];  	/**证书组织名称*/
	char	 szOrganizationalUnitName[64];  	/**证书组织单位名称*/
}STSSLCERTMSG;


/**
* @brief 设置通讯参数
* @param [in] pstCommParam  参数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-25
*/
int PubCommInit(const STCOMMPARAM *pstCommParam);


/**
* @brief 挂断通讯链路
* @param 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
int PubCommHangUp();

/**
* @brief 通讯关闭
* @param 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
int PubCommClose();

/**
* @brief 预拨号
* @param 无
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
int PubCommPreConnect(void);

/**
* @brief 通讯连接
* @param 无
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-25
*/
int PubCommConnect(void);

/**
* @brief 按照通讯方式规则读写数据
* @param [in]  psData        待发送数据
* @param [in]  nDataLen      数据长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubCommSend(const char *psData,int nDataLen);

/**
* @brief 读数据
* @detail  读取数据通道数据
* @param [out]  psData        接收数据
* @param [out]  *pnDataLen    返回数据长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubCommRecv(char *psData,int *pnDataLen);

/**
* @brief 写数据
* @detail 直接往数据通道写数据
* @param [in]  psData        待发送数据
* @param [in]  nDataLen      数据长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubCommWrite(const char *psData,int nDataLen);

/**
* @brief 按照通讯方式规则读取数据
* @param [out]  psData        接收数据
* @param [in]   nMaxLen       接收数据缓冲大小
* @param [out]  *pnDataLen    返回数据长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubCommRead(char *psData,int nMaxLen,int *pnDataLen);


/**
* @brief 获取通讯模块版本
* @param [out]pszVer  版本号
* @return 
* @li 无
*/
void PubGetCommVerion(char *pszVer);

/**
* @brief 设置通讯参数
* @param  pstCommParam
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
int PubSetCommParam(const STCOMMPARAM* pstCommParam);

/**
* @brief 清除通讯缓冲区
* @param 无
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
int PubCommClear();

/**
* @brief 设置超时界面显示风格 
* @param stShowInfoxy
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-24
*/
void PubSetShowXY(STSHOWINFOXY stShowInfoxy);

/**
* @brief 设置超时时间
* @param [in] ucTimeOut  超时时间(30---120)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-25
*/
int PubSetCommTimeOut(int nTimeOut);

/**
* @brief 设置重拨次数
* @param [in] ucReDialNum  重拨次数(1---9)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-25
*/
int PubSetReDialNum(int nReDialNum);

/**
* @brief 获取DHCP本地地址
* @param [in] STETHPARAM pstEthParam
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2013-5-7
*/
int PubGetDhcpLocalAddr(STETHPARAM  *pstEthParam);

/**
* @brief 获取连接索引
* @param [in] int *pnIndex
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-12-20
*/
int PubGetConnectIndex(int *pnIndex);

/**
* @brief Get 通讯参数
* @param  pstCommParam
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2013-9-15
*/
int PubGetCommParam(STCOMMPARAM* pstCommParam);

/**
* @brief 获取无线信号
* @param [out] pnWmSingal	无线信号指针
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlSq(int *pnWmSingal);

/**
* @brief 获取无线基站信息
* @param [out] pstStationInfo	基站信息
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlStation(ST_MOBILE_STATION_INFO *pstStationInfo);

/**
* @brief 获取无线模块信息
* @param  [in] emType	无限模块信息枚举见EM_WLM_TYPE_INFO
* @param  [in] unBufLen	缓冲区长度需大于等于21
* @param  [out]pnWmSingal	无线信号指针，1清接收缓冲0清发送缓冲
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlInfo(char cType,char *pszValue,uint unBufLen);

/**
* @brief 获取commserver版本号
* @param  [in] 无
* @param  [out] pszVer 版本号
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2013-10-24
*/
int PubGetCommserverVer(char *pszVer);

/**
* @brief 配置ssl，在连接之前使用，不设置则使用默认配置
* @param  [in] stSslMode ssl相关参数
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2014-1-1
*/
int PubSslSetMode(STSSLMODE *pstSslMode);

/**
* @brief 获取ssl服务端证书信息
* @param  [out] pstSslCertMsg 证书信息
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2014-1-1
*/
int PubSslGetCertMsg(STSSLCERTMSG* pstSslCertMsg);

/**
* @brief 扫描WIFI，选择SSID，并获取加密模式
* @param  [in] pszTitle ssid列表标题,传NULL则多显示一行
* @param  [in] nTimeout 超时
* @param  [out] pszOutSsid wifi热点名称
* @param  [out] pnWifiMode wifi加密模式
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2014-1-10
*/
int PubCommScanWifi(const char *pszTitle, char *pszOutSsid, int *pnWifiMode, int nTimeout);


/**
* @brief 保持socket链路alive
* @param  [in] TRUE 设置为alive
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author sunh
* @date 2014-3-18
*/
int PubSetSocketAlive(int nKeepAlive);

#endif

