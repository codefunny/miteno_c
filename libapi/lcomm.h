/*
* �´�½���Թ�˾ ��Ȩ����(c) 2006-2008
*
* POSͳһ�湫����
* ����ͷ�ļ�  --- lcomm.h
* ��    �ߣ�    ���
* ��    �ڣ�    2012-09-19
* ����޸��ˣ�  ���
* ����޸����ڣ�2012-10-11
*/
#ifndef _LCOMM_H_
#define _LCOMM_H_

#include "NDK.h"

typedef enum 
{
	SERVERMODE_RECVQUIT = (1<<0),				/**<���շ���֧���˳�*/
	SERVERMODE_CLOSENOWAIT = (1<<1),	   		/**<��ִ̨��ͨѶ�ر�*/
	SERVERMODE_INITNOWAIT = (1<<2),		   	/**<��ִ̨�г�ʼ��*/
	SERVERMODE_CLOSEWAIT = (1<<3),		   	/**<ǰִ̨��ͨѶ�ر�*/
	SERVERMODE_INITWAIT = (1<<4),		   		/**<ǰִ̨�г�ʼ��*/
}EM_SERVERMODE;

typedef enum {
	COMMTYPE_SYNDIAL=0x00,  			   	/**<ͬ������*/
	COMMTYPE_SYNDIAL_TPDU,    				/**<ͬ������+TPDU*/
	COMMTYPE_SYNDIAL_HEADLEN, 			/**<ͬ������+�����ֽ�ͷ����*/  
	COMMTYPE_SYNDIAL_TPDU_HEADLEN,    	/**<ͬ������+TPDU+�����ֽ�ͷ����*/
	
	COMMTYPE_ASYNDIAL=0x10,  			   	/**<�첽����*/
	COMMTYPE_ASYNDIAL_ASYN, 				/**<�첽����:0x02��ͷ+[���ֽڳ���]+[����]+[lrc]+0x03��β*/  
	COMMTYPE_ASYNDIAL_ASYN_TPDU,      		/**<�첽����:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+[lrc]+0x03��β*/
	COMMTYPE_ASYNDIAL_ASYN1, 			    	/**<�첽����:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_ASYNDIAL_ASYN1_TPDU,     	/**<�첽����:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/
	
	COMMTYPE_PORT=0x20,  			  		/**<����*/
	COMMTYPE_PORT_ASYN, 				  	/**<����:0x02��ͷ+[���ֽڳ���]+[����]+[lrc]+0x03��β*/  
	COMMTYPE_PORT_ASYN_TPDU,           		/**<����:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+[lrc]+0x03��β*/
	COMMTYPE_PORT_ASYN1, 			  		/**<����:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_PORT_ASYN1_TPDU,     	  	/**<����:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/
	
	COMMTYPE_GPRS=0x30,  			  		/**<GPRS*/
	COMMTYPE_GPRS_HEADLEN, 			  	/**<GPRS+�����ֽ�ͷ����*/  
	COMMTYPE_GPRS_TPDU_HEADLEN,        	/**<GPRS+TPDU+�����ֽ�ͷ����*/
	COMMTYPE_GPRS_ASYN1, 			    	/**<GPRS:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_GPRS_ASYN1_TPDU,     	/**<GPRS:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/
	
	COMMTYPE_CDMA=0x40,  			  		/**<CDMA*/
	COMMTYPE_CDMA_HEADLEN, 			  	/**<CDMA+�����ֽ�ͷ����*/  
	COMMTYPE_CDMA_TPDU_HEADLEN,        	/**<CDMA+TPDU+�����ֽ�ͷ����*/
	COMMTYPE_CDMA_ASYN1, 			    	/**<CDMA:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_CDMA_ASYN1_TPDU,     	/**<CDMA:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/

	COMMTYPE_ETH=0x50,  				 	/**<ETH*/
	COMMTYPE_ETH_HEADLEN, 			  	/**<ETH+�����ֽ�ͷ����*/  
	COMMTYPE_ETH_TPDU_HEADLEN,          	/**<ETH+TPDU+�����ֽ�ͷ����*/
	COMMTYPE_ETH_ASYN1, 			    		/**<ETH:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_ETH_ASYN1_TPDU,     		/**<ETH:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/

	COMMTYPE_USB=0x60,  			  		/**<USB*/
	COMMTYPE_USB_ASYN, 				  	/**<USB:0x02��ͷ+[���ֽڳ���]+[����]+[lrc]+0x03��β*/  
	COMMTYPE_USB_ASYN_TPDU,           		/**<USB:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+[lrc]+0x03��β*/
	COMMTYPE_USB_ASYN1, 			  		/**<USB:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_USB_ASYN1_TPDU,    	  		/**<USB:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/

	COMMTYPE_WIFI=0x70,  			  		/**<WIFI*/
	COMMTYPE_WIFI_HEADLEN, 			  	/**<WIFI+�����ֽ�ͷ����*/  
	COMMTYPE_WIFI_TPDU_HEADLEN,        		/**<WIFI+TPDU+�����ֽ�ͷ����*/
	COMMTYPE_WIFI_ASYN1, 			    		/**<WIFI:0x02��ͷ+[���ֽڳ���]+[����]+0x03��β+[lrc]*/  
	COMMTYPE_WIFI_ASYN1_TPDU,     			/**<WIFI:0x02��ͷ+[���ֽڳ���]+TPDU+[����]+0x03��β+[lrc]*/

}EM_COMMTYPE;


typedef struct
{
	char	szPredialNo[10];  		/**<���ߺ���*/
	char	lszTelNo[3][21];  			/**<3��绰����*/
	char  lsTPDU[3][5];     			/**<3��TPDU*/
	int	nCycTimes; 		    		/**<ѭ�����Ŵ���*/
	int   nCountryid ; 	    			/**<modem���Һ�*/
}STDIALPARAM;

typedef struct
{
	int	 nAux;  					/**<���ں�*/
	char szAttr[20+1];      			/**<����:���磺"115200,8,N,1"*/
	char sTPDU[5];   	 			/**<TPDU*/
}STPORTPARAM;

typedef struct
{
	char	lszIp[2][16]; 		  		/**<������IP��ַ---2��*/
	int lnPort[2]; 			/**<����˿ں�  ---2��*/
	char	szDN[50+1];         			/**<����������*/
	char	szDNSIp[32+1];      		/**<����������IP*/
}STSERVERADDRESS;

typedef struct 
{
 	char  szPinPassWord[20];                 /**<SIM������---GPRS��CDMA����*/
	char  szNetUsername[40]; 		/**<����modem����PPP��·ʱҪ�õ����û���---2�� */
	char  szNetPassword[20]; 			/**<����modem����PPP��·ʱҪ�õ������� ---2��*/
	char  szModemDialNo[21]; 		/**<����modem����PPP��·ʱʹ�õĽ������ ---2��*/
	char  szGprsApn[40]; 				/**<�ƶ�APN����---2��*/
	char  sTPDU[5];                    		/**<TPDU*/
}STGPRSPARAM;

typedef struct 
{
	char  szPinPassWord[20];                  /**<SIM������---GPRS��CDMA����*/
	char  szNetUsername[40]; 		 /**<PPP�û���---CDMA���� */
	char  szNetPassword[20]; 			 /**<PPP����---CDMA����*/
	char  szModemDialNo[21]; 		 /**<����modem����PPP��·ʱʹ�õĽ������ ---2��*/
	char  sTPDU[5];                    		 /**<TPDU*/
}STCDMAPARAM;

typedef struct
{
	int nDHCP;						/**<�Ƿ�ʹ��DHCP*/
	char  szIP[16];					/**<����IP��ַ*/
	char  szGateway[16];				/**<��������*/
	char  szMask[16];		  			/**<������������*/
	char  szDNS[16];          			/**<DNS*/
	char sTPDU[5];            	  			/**<TPDU*/
}STETHPARAM;

typedef struct
{
	uchar ucIfDHCP;             /**<�Ƿ�ʹ��DHCP*/
	EM_WIFI_KEY_TYPE emKeyType; /**<·�����������ʽ*/
	EM_WIFI_NET_SEC emSecMode;  /**<·�����İ�ȫ����ģʽ*/
	char szSsid[32];               /**<·������Ssid,���ȵ�����*/
	char szKey[32];               /**<·����������*/
	char szIP[16];              /**<�ն�IP��ַ*/
	char szMask[16];         /**<���������ַ*/
	char szGateway[16];         /**<�������ص�ַ*/
	char szDnsPrimary[16];      /**<������DNS��ַ*/
	char sTPDU[5];            	  			/**<TPDU*/
} STWIFIPARAM;

/**
* @struct STCOMMPARAM Ӧ��ͨѶ�����
*/
typedef struct
{
	char cCommType;				      	/**<ͨѶ���ͼ�enum EM_COMMTYPE*/
	char cPreDialFlag;				  	/**<Ԥ���ű�ʶ1Ԥ���ţ�0��Ԥ����*/
	char cReDialNum;				  	/**<�ز�����*/
	int  nTimeOut;					  	/**<��ʱʱ��*/
	char cMode;						    	/**<�������ӱ�־*/
	char cIsSupportQuitRecv;                     /**<COMMSERVERģʽ ��EM_SERVERMODEö�٣�ͬʱ֧����|*/
	union
	{
		STDIALPARAM stDialParam;  		/**<���Ų����ṹ��*/
		STPORTPARAM stPortParam;  	/**<���ڲ����ṹ��*/
		STGPRSPARAM stGprsParam;  	/**<Gprs�����ṹ��*/
		STCDMAPARAM stCdmaParam;  	/**<Cdma�����ṹ��*/
		STETHPARAM  stEthParam;   		/**<��̫�������ṹ��*/
		STWIFIPARAM  stWifiParam;   		/**<wifi�����ṹ��*/
	}ConnInfo;
	STSERVERADDRESS stServerAddress; /**<����������*/
	void ( *ShowFunc )( void ) ;				/**<����ʱ��ʾ�Ľ�����Ϣ*/
	char cSslFlag;                      /**<ssl��־: 0.����ssl ; 1.ssl������֤; 2.ssl˫����֤*/
	char sRfu[39];
}STCOMMPARAM;

typedef struct
{
    int nColumn; /*��(1-10)*/
    int nRow; /*��*/
    int nType; /*��ʾ���� 0���� 1�ݼ�*/
}STSHOWINFOXY; /*������ʾ���ṹ*/

typedef union
{
	NDK_HANDLE vHandle;
	int nHandle;
}STHANDLE;

typedef struct
{
	int nType;                  	/*����Э������ ��EM_SSL_HANDSHAKE_OPT*/
	int nAuthOpt;			/*��֤ģʽ ��EM_SSL_AUTH_OPT*/
	int nFileFormat;           	/**֤���ļ���ʽ ��EM_SSL_FILE_FORMAT*/
	char szPwd[16];          	/**˽Կ����*/
	char	 szServerCert[20];  	/**<CA֤���ļ���*/
	char	 szClientCert[20];  	/**<����֤���ļ���*/
	char	 szClientPrivateKey[20];  /**<����˽Կ��*/
}STSSLMODE;

typedef struct
{
	char szSubjectName[256];                  	/*֤��ӵ������Ϣ*/
	char szIssuername[256];				/*֤��䷢����Ϣ*/
	char nVersion;           			/**֤��汾��*/
	char szNotBeforeTime[16];          	/**֤����Чʱ��*/
	char szNotAfterTime[16];          		/**֤�����ʱ��*/
	char szCountryName[32];           		/**֤���������*/
	char szCommonName[32];           			/**֤����������*/
	char	 szOrganizationName[64];  	/**֤����֯����*/
	char	 szOrganizationalUnitName[64];  	/**֤����֯��λ����*/
}STSSLCERTMSG;


/**
* @brief ����ͨѶ����
* @param [in] pstCommParam  ����
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-25
*/
int PubCommInit(const STCOMMPARAM *pstCommParam);


/**
* @brief �Ҷ�ͨѶ��·
* @param 
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
int PubCommHangUp();

/**
* @brief ͨѶ�ر�
* @param 
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
int PubCommClose();

/**
* @brief Ԥ����
* @param ��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
int PubCommPreConnect(void);

/**
* @brief ͨѶ����
* @param ��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-25
*/
int PubCommConnect(void);

/**
* @brief ����ͨѶ��ʽ�����д����
* @param [in]  psData        ����������
* @param [in]  nDataLen      ���ݳ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubCommSend(const char *psData,int nDataLen);

/**
* @brief ������
* @detail  ��ȡ����ͨ������
* @param [out]  psData        ��������
* @param [out]  *pnDataLen    �������ݳ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubCommRecv(char *psData,int *pnDataLen);

/**
* @brief д����
* @detail ֱ��������ͨ��д����
* @param [in]  psData        ����������
* @param [in]  nDataLen      ���ݳ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubCommWrite(const char *psData,int nDataLen);

/**
* @brief ����ͨѶ��ʽ�����ȡ����
* @param [out]  psData        ��������
* @param [in]   nMaxLen       �������ݻ����С
* @param [out]  *pnDataLen    �������ݳ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubCommRead(char *psData,int nMaxLen,int *pnDataLen);


/**
* @brief ��ȡͨѶģ��汾
* @param [out]pszVer  �汾��
* @return 
* @li ��
*/
void PubGetCommVerion(char *pszVer);

/**
* @brief ����ͨѶ����
* @param  pstCommParam
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
int PubSetCommParam(const STCOMMPARAM* pstCommParam);

/**
* @brief ���ͨѶ������
* @param ��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
int PubCommClear();

/**
* @brief ���ó�ʱ������ʾ��� 
* @param stShowInfoxy
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-24
*/
void PubSetShowXY(STSHOWINFOXY stShowInfoxy);

/**
* @brief ���ó�ʱʱ��
* @param [in] ucTimeOut  ��ʱʱ��(30---120)
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-25
*/
int PubSetCommTimeOut(int nTimeOut);

/**
* @brief �����ز�����
* @param [in] ucReDialNum  �ز�����(1---9)
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-25
*/
int PubSetReDialNum(int nReDialNum);

/**
* @brief ��ȡDHCP���ص�ַ
* @param [in] STETHPARAM pstEthParam
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2013-5-7
*/
int PubGetDhcpLocalAddr(STETHPARAM  *pstEthParam);

/**
* @brief ��ȡ��������
* @param [in] int *pnIndex
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-12-20
*/
int PubGetConnectIndex(int *pnIndex);

/**
* @brief Get ͨѶ����
* @param  pstCommParam
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2013-9-15
*/
int PubGetCommParam(STCOMMPARAM* pstCommParam);

/**
* @brief ��ȡ�����ź�
* @param [out] pnWmSingal	�����ź�ָ��
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlSq(int *pnWmSingal);

/**
* @brief ��ȡ���߻�վ��Ϣ
* @param [out] pstStationInfo	��վ��Ϣ
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlStation(ST_MOBILE_STATION_INFO *pstStationInfo);

/**
* @brief ��ȡ����ģ����Ϣ
* @param  [in] emType	����ģ����Ϣö�ټ�EM_WLM_TYPE_INFO
* @param  [in] unBufLen	��������������ڵ���21
* @param  [out]pnWmSingal	�����ź�ָ�룬1����ջ���0�巢�ͻ���
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2013-10-24
*/
int PubCommGetWlInfo(char cType,char *pszValue,uint unBufLen);

/**
* @brief ��ȡcommserver�汾��
* @param  [in] ��
* @param  [out] pszVer �汾��
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2013-10-24
*/
int PubGetCommserverVer(char *pszVer);

/**
* @brief ����ssl��������֮ǰʹ�ã���������ʹ��Ĭ������
* @param  [in] stSslMode ssl��ز���
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2014-1-1
*/
int PubSslSetMode(STSSLMODE *pstSslMode);

/**
* @brief ��ȡssl�����֤����Ϣ
* @param  [out] pstSslCertMsg ֤����Ϣ
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2014-1-1
*/
int PubSslGetCertMsg(STSSLCERTMSG* pstSslCertMsg);

/**
* @brief ɨ��WIFI��ѡ��SSID������ȡ����ģʽ
* @param  [in] pszTitle ssid�б����,��NULL�����ʾһ��
* @param  [in] nTimeout ��ʱ
* @param  [out] pszOutSsid wifi�ȵ�����
* @param  [out] pnWifiMode wifi����ģʽ
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2014-1-10
*/
int PubCommScanWifi(const char *pszTitle, char *pszOutSsid, int *pnWifiMode, int nTimeout);


/**
* @brief ����socket��·alive
* @param  [in] TRUE ����Ϊalive
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author sunh
* @date 2014-3-18
*/
int PubSetSocketAlive(int nKeepAlive);

#endif

