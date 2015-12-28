/**
* @file comm.h
* @brief ͨѶģ��
* @version  1.0
* @author �Ž�
* @date 2007-01-22
*/
#ifndef _COMM_H_
#define _COMM_H_

#define FILE_APPCOMMPARAM	APP_NAME"COMM"
#define FILE_APPCOMMRESERVE	APP_NAME"RESV"

/**
* @struct STAPPCOMMPARAMӦ��ͨѶ�����
*/
typedef struct
{
	char cCommType;					/**<ͨѶ���ͼ�enum COMMTYPE*/
	char cPreDialFlag;				/**<Ԥ���ű�ʶ*/
	char cReDialNum;				/**<�ز�����*/
	char cReSendNum;				/**<�����ط�����*/
	char cTimeOut;					/**<��ʱʱ��*/
	char sTpdu[10];					/**<TPDU*/
	char szPreDial[10+1];			/**<���ߺ���	AN10*/
	char szTelNum1[19+1];			/**<�绰����1	N14*/
	char szTelNum2[19+1];			/**<�绰����2	N14*/
	char szTelNum3[19+1];			/**<�绰����3	N14*/
	char szManageTelNum[19+1];		/**<һ������绰����	An14*/
	char szIp1[16+1];				/**<Ip1��ַ	An16*/
	char szPort1[6+1];				/**<�˿ں�1	N4*/
	char szIp2[16+1];				/**<Ip2��ַ	An16*/
	char szPort2[6+1];				/**<�˿ں�2	N4*/
	char szWirelessDialNum[40+1];	/**<����modem���ź���	An20*/
	char szAPN1[40+1];				/**<APN1	An30*/
	char szAPN2[40+1];				/**<APN2	An30*/
	char szUser[40+1];				/**<�û���	An40*/
	char szPassWd[40+1];			/**<�û�����	An40*/
	char szSIMPassWd[10+1];			/**<SIM������	N10*/
	char cMode;						/**<�������ӱ�־	N1*/
	char szIpAddr[16+1];			/**<����ip��ַ	An16*/
	char szMask[16+1];				/**<��������	An16*/
	char szGate[16+1];				/**<����	An16*/
	char szDNSIp1[16+1];			/**<DNS IP1	An16*/
	char szDNSIp2[16+1];			/**<DNS IP2	An16*/
	char szDNSIp3[16+1];			/**<DNS IP3	An16*/
	char szDomain[50+1];			/**<����	An100*/
	char cAuxIsAddTpdu;				/**<����ͨѶ�Ƿ���Ҫtpdu��1�ǣ�0����Ҫ*/
	char cIsSSL;					/*�Ƿ���SSLģʽ*/
	char cIsSendSSLAuth;			/*���������SSLģʽ������£��Ƿ���У���*/
	char cIsDns;                    /*�Ƿ�ʹ������*/
    char cIsCommSendRecvContinue;   /**<���ͽ��յ�ʱ���Ƿ���Ҫ�Ҷ�*/   
	char cOffResendNum;				/**<�����ط�����*/
	char szRfu[23];
}STAPPCOMMPARAM;

//ͨѶ����Ԥ��������,Ϊ����������ʧ����,���һ���ļ�
typedef struct
{
	char cIsDHCP;					/**<�Ƿ�ʹ��DHCP*/
	char szWifiSsid[32+1];			/**<WIFI ssid ����*/
	char szWifiKey[64+1];			/**<WIFI ����*/
	char cWifiKeyType;				/**<·�����������ʽ EM_WIFI_KEY_TYPE*/
	char cWifiMode;					/**<·�����İ�ȫ����ģʽ EM_WIFI_NET_SEC*/
	char szDomain2[50+1];			/*����AN100*/
	char szDnsPort1[6+1];			/*ʹ��DNSʱ�˿ں�*/
	char szDnsPort2[6+1];			/*ʹ��DNSʱ���ö˿ں�*/
	char szStationIp[16+1];			/*��վ��Ϣ��̨IP*/
	char szStationPort[6+1];		/*��վ��Ϣ��̨PORT*/
	char sStationTpdu[10];			/*��վ��Ϣ��̨TPDU*/
	char cLbsTimeOut;				/*��վ��Ϣ���ճ�ʱʱ��*/	
	char szRfu[1000-206];	
}STAPPCOMMRESERVE;

/**
* @struct STPOSCOMMHEAD ����ͷ*/
typedef struct
{
	char sType[2];		//Ӧ�����Ĭ����"60"
	char sSoftver[2];	/* ����汾
				"10"-2001����������POS�淶֮ǰ�汾
				"11"-2001����������POS�淶�汾
				"21"-2002������POS�淶�汾 
				*/
	char cStatus;		/*'0'-��������״̬,'1'-���Խ���״̬*/
	char cResponse;		/* ����Ҫ��
				'0'-�޴���Ҫ��
				'1'-�´��ն˲���
				'2'-�ϴ��ն�״̬��Ϣ
				'3'-����ǩ��
				'4'	֪ͨ�ն˷�����¹�Կ��Ϣ����
				'5'	�����ն�IC������
				'6'	TMS��������
				*/
	char sOther[6];				
}STPOSCOMMHEAD;

enum COMMTYPE
{
	COMM_DIAL = 1,		/**<NAC*/
	COMM_GPRS,			/**<GPRS*/
	COMM_CDMA,			/**<CDMA*/
	COMM_RS232,			/**<����*/
	COMM_ETH,			/**<��̫��*/
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

