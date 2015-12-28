/**
* @file comm.h
* @brief 通讯模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _COMM_H_
#define _COMM_H_

#define FILE_APPCOMMPARAM	APP_NAME"COMM"
#define FILE_APPCOMMRESERVE	APP_NAME"RESV"

/**
* @struct STAPPCOMMPARAM应用通讯类参数
*/
typedef struct
{
	char cCommType;					/**<通讯类型见enum COMMTYPE*/
	char cPreDialFlag;				/**<预拨号标识*/
	char cReDialNum;				/**<重拨次数*/
	char cReSendNum;				/**<冲正重发次数*/
	char cTimeOut;					/**<超时时间*/
	char sTpdu[10];					/**<TPDU*/
	char szPreDial[10+1];			/**<外线号码	AN10*/
	char szTelNum1[19+1];			/**<电话号码1	N14*/
	char szTelNum2[19+1];			/**<电话号码2	N14*/
	char szTelNum3[19+1];			/**<电话号码3	N14*/
	char szManageTelNum[19+1];		/**<一个管理电话号码	An14*/
	char szIp1[16+1];				/**<Ip1地址	An16*/
	char szPort1[6+1];				/**<端口号1	N4*/
	char szIp2[16+1];				/**<Ip2地址	An16*/
	char szPort2[6+1];				/**<端口号2	N4*/
	char szWirelessDialNum[40+1];	/**<无线modem拨号号码	An20*/
	char szAPN1[40+1];				/**<APN1	An30*/
	char szAPN2[40+1];				/**<APN2	An30*/
	char szUser[40+1];				/**<用户名	An40*/
	char szPassWd[40+1];			/**<用户密码	An40*/
	char szSIMPassWd[10+1];			/**<SIM卡密码	N10*/
	char cMode;						/**<长短链接标志	N1*/
	char szIpAddr[16+1];			/**<本机ip地址	An16*/
	char szMask[16+1];				/**<子网掩码	An16*/
	char szGate[16+1];				/**<网关	An16*/
	char szDNSIp1[16+1];			/**<DNS IP1	An16*/
	char szDNSIp2[16+1];			/**<DNS IP2	An16*/
	char szDNSIp3[16+1];			/**<DNS IP3	An16*/
	char szDomain[50+1];			/**<域名	An100*/
	char cAuxIsAddTpdu;				/**<串口通讯是否需要tpdu，1是，0不需要*/
	char cIsSSL;					/*是否开启SSL模式*/
	char cIsSendSSLAuth;			/*如果开启了SSL模式的情况下，是否发送校验包*/
	char cIsDns;                    /*是否使用域名*/
    char cIsCommSendRecvContinue;   /**<发送接收的时候是否需要挂断*/   
	char cOffResendNum;				/**<离线重发次数*/
	char szRfu[23];
}STAPPCOMMPARAM;

//通讯参数预留不够大,为了升级不丢失参数,另放一个文件
typedef struct
{
	char cIsDHCP;					/**<是否使用DHCP*/
	char szWifiSsid[32+1];			/**<WIFI ssid 名称*/
	char szWifiKey[64+1];			/**<WIFI 密码*/
	char cWifiKeyType;				/**<路由器的密码格式 EM_WIFI_KEY_TYPE*/
	char cWifiMode;					/**<路由器的安全加密模式 EM_WIFI_NET_SEC*/
	char szDomain2[50+1];			/*域名AN100*/
	char szDnsPort1[6+1];			/*使用DNS时端口号*/
	char szDnsPort2[6+1];			/*使用DNS时备用端口号*/
	char szStationIp[16+1];			/*基站信息后台IP*/
	char szStationPort[6+1];		/*基站信息后台PORT*/
	char sStationTpdu[10];			/*基站信息后台TPDU*/
	char cLbsTimeOut;				/*基站信息接收超时时间*/	
	char szRfu[1000-206];	
}STAPPCOMMRESERVE;

/**
* @struct STPOSCOMMHEAD 报文头*/
typedef struct
{
	char sType[2];		//应用类别，默认是"60"
	char sSoftver[2];	/* 软件版本
				"10"-2001年人民银行POS规范之前版本
				"11"-2001年人民银行POS规范版本
				"21"-2002年银联POS规范版本 
				*/
	char cStatus;		/*'0'-正常交易状态,'1'-测试交易状态*/
	char cResponse;		/* 处理要求
				'0'-无处理要求
				'1'-下传终端参数
				'2'-上传终端状态信息
				'3'-重新签到
				'4'	通知终端发起更新公钥信息操作
				'5'	下载终端IC卡参数
				'6'	TMS参数下载
				*/
	char sOther[6];				
}STPOSCOMMHEAD;

enum COMMTYPE
{
	COMM_DIAL = 1,		/**<NAC*/
	COMM_GPRS,			/**<GPRS*/
	COMM_CDMA,			/**<CDMA*/
	COMM_RS232,			/**<串口*/
	COMM_ETH,			/**<以太网*/
	COMM_WIFI			/*WIFI*/
};

#define MAX_SEND_SIZE (MAX_PACK_SIZE + 20)
#define MAX_RECV_SIZE MAX_SEND_SIZE

#define COMM_TCPIP_HEAD_LEN 2
extern void SetControlCommInit(void);
extern int CommInit(void);
extern int CommPreDial(void);
extern int CommHangUp(void);
extern int CommHangUpSocket(void);
extern int CommConnect(void);
extern int CommSend(const char *, int);
extern int CommRecv(char *,int *);
extern int CommSendRecv(const char *, int, char *,int *);
extern int CommParamSendRecv(const char *, int , char *, int *);
extern int CommDump(void);
extern int InitCommParam(void);
extern int InitCommReserve(void);
extern int ExportCommParam(void);
extern int SetFuncCommPreDialNum(void);
extern int SetFuncCommType(void);
extern int GetVarCommTimeOut(char *);
extern int SetVarCommTimeOut(const char *);
extern int GetVarCommTelNo(char *, char *, char *, char *);
extern int SetVarCommTelNo(const char *, const char *, const char *, const char *);
extern int GetVarCommReSendNum(char * );
extern int SetVarCommReSendNum(const char * );
extern int GetVarCommReDialNum(char *);
extern int SetVarCommReDialNum(const char * );
extern int SetFuncCommReSendNum(void);
extern void GetAppCommParam(STAPPCOMMPARAM *);
extern int SetAppCommParam(STAPPCOMMPARAM *);
extern void GetAppCommReserve(STAPPCOMMRESERVE * );
extern int SetAppCommReserve(STAPPCOMMRESERVE * );
extern int GetVarCommOffReSendNum(char *);
extern void CommMenu(void);
extern int DispCommPreDialNum(void);
extern int DoSetPreDialNum(void);
extern int SetPreDialNum(void);
extern char GetVarIsSendSSL(void);
extern int IsUseSslFunction(void);
extern int GetVarCommType(void);

#endif

