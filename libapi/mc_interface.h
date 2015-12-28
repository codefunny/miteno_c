
#ifndef _MC_INTERFACE_H_
#define _MC_INTERFACE_H_

#include <time.h>

enum MC_PARAM_TYPE{
	MODEM_PRE_DAIL_NUM=0,         /**<���ߺ���*/
	APP_POSID,                    /**<�ն˺�*/
	APP_MERCHANTID,               /**<�̻���*/
	APP_ADVERTISEMENT,            /**<���*/
	APP_DISPNAME,                 /**<Ӧ�ñ���*/
	DOWN_REDIAL_REVERSAL,		/**<�ز����ʱ��*/
	DOWN_WARNING_REVERSAL,		/**<��������ʱ����*/
	DOWN_ONLINE_WAITTIME,		/**<�����ȴ�ʱ��*/
	DOWN_SIGN,						/**<�������ݱ�־*/
	APP_VER,						/**<Ӧ�ð汾��*/
	CHECK_CODE,					/**<��������У����*/
	SAVE_APP_PARA,				/**<����ͨѶ����*/
	DOWN_END_FLAG,				/**<���ؽ�����־*/
	UN_DEF_TYPE=0xFF		      /**<δ��������*/
};

enum MC_FUNC_TYPE{
	EXEC_COMM_SET=0,		/**<ͨѶ��������*/
	EXEC_COMM_CONNECT,		/**<ͨѶ����*/
	EXEC_COMM_SEND,			/**<�������صķ�������*/
	EXEC_COMM_RECV,			/**<�������صĽ�������*/
	EXEC_COMM_WRITE,        /**<�������ص�PubCommWrite����*/
	EXEC_COMM_READ,			/**<�������ص�PubCommRead����*/
	EXEC_COMM_HANGUP,		/**<ͨѶ�Ҷ�*/
	EXEC_DOWNLOAD_PROGRAM,  /**<���س���*/
	EXEC_UPDATE_PROGRAM,    /**<���³���*/
	EXEC_INFO_UP,		/**<��Ϣ����*/
	EXEC_INFO_DOWN	/**<��Ϣ����*/
};

enum MC_COMMTYPE
{
	MC_COMM_RS232 = 1,			/**<����*/
	MC_COMM_ASYNCHDIAL,		/**<�첽����*/
	MC_COMM_DIAL,				/**<NAC*/
	MC_COMM_ETH,				/**<��̫��*/
	MC_COMM_GPRS,				/**<GPRS*/
	MC_COMM_CDMA,				/**<CDMA*/
	MC_COMM_PPP,				/**<ppp*/
	MC_COMM_UNDEF	=0xFF		/* δ���壬������ָ��       */
};

#define ALLOW_UPD_CURR_APP				1
#define DISALLOW_UPD_CURR_APP			0

#define TMS_DOWNLOAD_TIME_IS_UP		1		/*����ʱ�䵽*/
#define TMS_WITHOUT_DOWNLOAD			0		/*����Ҫ����*/

#define MAX_SUPPORT_APP_NUM		32		/**<Ӧ�ó���֧�ֵ����Ӧ�ø����������������*/



/**<Ӧ������ļ���Ϣ */
#define MAX_FILE_NUM		10
#define POS_FILE_NAME_LEN	8
typedef struct  {
	/**<Ӧ�ó����ļ� */
	char nIsCompressed;		                                   /**<Ӧ��ѹ��ģʽ*/
	char nAppIndex;				                               /**<Ӧ�����*/
	char szAppFileName[128 + 1];	               /**<Ӧ�����ƣ�""�մ���ʾ��������Ӧ��*/
	
	/**<�����ļ� */
	char nParamIndex;				                           /**<�����ļ����*/
	char szParamFileName[128 + 1];               /**<�������ƣ�""�մ���ʾ��������Ӧ��*/
	
	/**<�����ļ� */
	char nDataFileNum;			                               /**<�����ļ���Ŀ*/
	char nDataIndex[MAX_FILE_NUM];                             /**<�����ļ�������,���ļ��������Ӧ*/
	char szDataFileName[MAX_FILE_NUM][128 + 1];  /**<Ӧ���б�����*/
	
	/**<�����ļ� */
	char nOtherFileNum;			                                 /**<�����ļ���Ŀ*/
	char nOtherIndex[MAX_FILE_NUM];                              /**<�����ļ�������*/
	char szOtherFileName[MAX_FILE_NUM][128 + 1];   /**<�����ļ��ļ���������*/
}STTmsFileInfo;

typedef STTmsFileInfo * PSTTmsFileInfo;

typedef struct {
	STTmsFileInfo stTmsFileInfo;
	char *pszUmsTel;                                    /**<��ʾ�ĵ绰����*/
	char szUpdataTime[14+1];                            /**<Ӧ�õĸ���ʱ��*/
	int szTimes;                                        /**<��ʱ��ʾ��ʱ����*/
	char *pcIsWarning;                                  /**<�Ƿ���ʾ*/
	int nNotSettleAppNum;                               /**<δ�����Ӧ������*/
	char *pszNotSettleAppNameList[MAX_SUPPORT_APP_NUM]; /**<ָ��δ�����Ӧ��*/
}STMCUPDATAINFO;
typedef STMCUPDATAINFO * PSTMCUPDATAINFO;

typedef struct {
	char nCommType;				    // ͨѶ����		0xFF δ֪��ʹ��������TMS���õĲ���
	char szTel1[20+1];
	char szTel2[20+1];
	char szIpAndPort1[30+1];
	char szIpAndPort2[30+1];
	char szGprsParam[60+1];
	char szCdmaParam[60+1];
	char szTpdu[10];
	char cIsUseAppIP;					/**<�Ƿ�ʹ����Ӧ�õı���IP��ַ*/
	char szIpAddr[16+1];				/**<����ip��ַAn16,����Ӧ��һ��*/
	char szMask[16+1];					/**<��������	An16*/
	char szGate[16+1];					/**<����	An16*/
}STTMSDOWN;
typedef STTMSDOWN * PSTTMSDOWN;

/**
* @brief ��ȡ�����ر���Ĺ�������
* @param in nParamType ��������
* @param in psOBuf	  �����������
* @param in punDataLen ����ʱ�ǻ�������������ȣ����Ϊ��ֵ�ĳ���
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_GetPubParam(int nParamType, char *psOBuf, unsigned int *punDataLen);

/**
* @brief ��ȡ�����ر���Ĺ�������
* @param in nParamType ��������
* @param in pszIBuf	  �������뻺��
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_SetPubParam(int nParamType, const char *psIBuf,unsigned int unInLen);

/**
* @brief ��ȡ�������ص�ʱ���뱣��������
* @param in ulTimes �������е�ʱ��
* @param in unRandomNum	  ����������
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_GetEnterTimes(long *plTimes,int *pnRandomNum);

/**
* @brief �޸�POSʱ��
* @param in pt Ҫ�����ʱ��ṹ
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_SetPosTime(struct tm pt);

/**
* @brief ��ȡ���س���汾
* @param in pszVer ���ذ汾��
* @return 
* @li APP_SUCC �ɹ�
*/
int MC_GetVer(char *pszVer);

/**
* @brief ��ȡ���س���汾
* @param in pszVer ���ذ汾��
* @return 
* @li APP_SUCC �ɹ�
*/
int MC_GetLibVer(char *pszVer);


/**
* @brief ִ�����ع��ܺ���
* @param in nFuncType ��������
* @param in psInBuf	  �������뻺��
* @param in nInLen	  �������볤��
* @param out psOutBuf   �����������
* @param out pnOutLen   �����������
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_ExecFunc(int nFuncType,const char *psInBuf,int nInLen,char *psOutBuf,int *pnOutLen);


/**
* @brief ��ѯӦ�õ�������Ϣ
* @param [out] pstMcUpdataInfo ---Ӧ����������Ϣ
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL ʧ��
*/
int MC_TMSQueryInfo(STMCUPDATAINFO *pstMcUpdataInfo);


/**
* @brief �����ļ�
* @param [in]  nIndex        �ļ�����
* @param [in]  pszFileName	 �ļ���
* @param [in]  ProcFile      �ⲿִ�к���
* @return 
* @li APP_SUCC �ɹ�
* @li APP_FAIL �ɹ�
*/
int MC_TMSUpdateFile(int nIndex, char *pszFileName, int (*ProcFile)(char *));

/**
* @brief TMS����Ӧ��������־���� 1 �������� 0 ��ֹ����
* @param in	��
* @return 
* @li APP_SUCC	�ɹ�
*/
int MC_TMSAllowAppUpdCtrl(char cFlag);

#endif

