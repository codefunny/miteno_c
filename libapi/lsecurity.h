/**<
* �´�½���Թ�˾ ��Ȩ����(c) 2012-2016
*
* POSͳһ�湫����
* ��ȫģ��ͷ�ļ�  --- lsecurity.h
* ��    �ߣ�    ���
* ��    �ڣ�    2012-09-19
* ����޸��ˣ�  ��� 
* ����޸����ڣ�2012-10-19
*/
#ifndef _LSECRITY_H_
#define _LSECRITY_H_



#define PINPAD_HY			0x01
#define PINPAD_PP60_829	0x22
#define PINPAD_710			0x23

#define DESMODE_DES '0'		/**< ��desģʽ*/
#define DESMODE_3DES '1'		/**<3desģʽ*/

#define PINPAD_MAINKEYTYPE 0	/**< ����Կ */
#define PINPAD_WORKKEYTYPE 1	/**<�û���Կ*/

typedef enum 
{
	SECRITY_MODE_INSIDE=0x00,  			    /**<ʹ�����ð�ȫģ��*/
	SECRITY_MODE_PINPAD,					/**<ʹ���������*/
	SECRITY_MODE_ALL,				        /**<ʹ����һ��ȫģ��*/
}EM_SECRITY_MODE;

enum EM_KEY_TYPE
{
	KEY_TYPE_PIN=0x00,  			    /**<Pin��Կ*/
	KEY_TYPE_MAC,					    /**<Mac��Կ*/	
	KEY_TYPE_TRACK,				        /**<�ŵ���Կ*/
	KEY_TYPE_DATA,				        /**<������Կ*/
};

enum EM_MAC_TYPE
{
	MAC_TYPE_ECB=0,  			    	/**<ECB�㷨---�������㷨*/
	MAC_TYPE_X99,						/**<X99�㷨*/
	MAC_TYPE_X919,						/**<X919�㷨*/
	MAC_TYPE_9606,						/**<Mac 9606�㷨*/
};

enum EM_PIN_TYPE
{
   PINTYPE_WITHPAN = 0,       	    /**<�����˺ż���*/
   PINTYPE_WITHOUTPAN = 1,     /**<�������˺ż���*/
   PINTYPE_PLAIN_STAR = 2,       /**<���ķ�ʽ(����),*��ʾ*/
   PINTYPE_PLAIN = 3,                 /**<���ķ�ʽ��ȡ������Pinpad������ʾ*/
};

typedef struct{
	char cAux;            	 /**<����������Ӵ���*/
	char cTimeout;	       /**<������̳�ʱʱ��*/
	char cVoiceFlag;       /**<0x00��ʾ�رհ�������0x01��ʾ�򿪰�����*/
	char cDesMode;         /**<0x01��ʾdes,0x00��ʾ3des*/
	char cPinMode;	       /**<0x00��ʾ�Զ�����0x01��ʾȷ�Ϸ���*/
	char cMinPinLen;       /**<��С���볤��*/	
	char szLogo[17];       /**<�������Ĭ����ʾLOGO,֧��ASCII�����ֿ��еĺ���(���ZT579)*/
	char szUid[17];	       /**<e18��826�������ר��*/
	char cDefaultType;     /**<Ĭ�������������*/
	char cAmendMode;       /**<���zt579-d2a�������F3��������ɾ��������������뻹��ɾ����������1λ��1��ʾɾ�����е� 0��ʾɾ��1λ*/
	char cAppDirID;        /**<Ӧ��Ŀ¼ID  ��Զ�Ӧ�ö��Ե�*/
	char cInitKeyFile;     /**<���826������̶��Եģ�����Ƿ��ʼ����Կ�ļ�*/
	char cShowTime;        /**<���826������̵ģ������ڵ�����ʾ������ʱ����ʾ������*/
    char cIsUseNewKey;     /**<ʹ���µ���Կ��ϵ,�������*/
	char sReserve[19];     /**<����*/
}STPINPADPARAM;

/**
* @brief ��ʼ����ǰ��ȫģ��ģʽ
* @detail ������Ӧ��ģ���Ƿ����
* @param [in] nMode           ---EM_SECRITY_MODE
* @param [in] pstPinpadParam   ---������̲���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubInitSecrity(int nMode,const STPINPADPARAM* pstPinpadParam);

/**
* @brief �����Կ��
* @detail �����Կ��
* @param [in] 
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubClearKey(void);

/**
* @brief ���õ�ǰ����Կ������
* @param [in] nIndex  ����Կ������  0-83
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubSetCurrentMainKeyIndex(int nIndex);

/**
* @brief ��װ����Կ
* @param [in] nIndex  ����Կ������  0-83
* @param [in] psKey   ����Կ����
* @param [in] nKeyLen ����Կ���� 8�ֽڻ�16�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubLoadMainKey(int nIndex,const char* psKey, int nKeyLen);

/**
* @brief ��װ������Կ
* @param [in] nKeyType      ��Կ����---EM_KEY_TYPE
* @param [in] psKey         ��Կ����
* @param [in] nKeyLen       ����Կ���� 8�ֽڻ�16�ֽ�
* @param [in] psCheckValue  �����CheckValue/(NULL)
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubLoadWorkKey(int nKeyType,const char* psKey, int nKeyLen,const char* psCheckValue);

/**
* @brief ��ȡPin
* @detail ֧�ֶ��ּ����㷨��ʵ�����������С���ȵ����ã���ʾ������ʱ����ȷ�ϼ���
          ����������ʾ�ڵڶ��С��������һ����ʾ���ݡ�����ʱ���Ǻ���ʾ���������꣬�����Ļ��Ĭ��״̬
* @param [out] pszPin        ����Pin
* @param [out] pnPinLen      ����Pin�ĳ���
* @param [in]  nMode         �������ģʽEM_PIN_TYPE
* @param [in]  pszCardno     ����/(NULL)
* @param [in]  pszAmount     ���
* @param [in]  nMaxLen       ������볤��
* @param [in]  nMinLen       ��С���볤�ȣ���֤ͨ���������Ч
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubGetPin(char *pszPin, int *pnPinLen, int nMode, const char *pszCardno,const char *pszAmount,
			  int nMaxLen, int nMinLen);

/**
* @brief ��ȡPin��չ����(������ʾλ�ÿ���NDK_ScrGotoxy����)
* @detail ֧���Զ���POS������ʾ����
* @param [out] pszPin        ����Pin
* @param [out] pnPinLen      ����Pin�ĳ���
* @param [in]  nMode         �������ģʽEM_PIN_TYPE
* @param [in]  pszCardno     ����/(NULL)
* @param [in]  nMaxLen       ������볤��
* @param [in]  nMinLen       ��С���볤�ȣ���֤ͨ���������Ч
* @param [in]  ShowFunc      ��ʾ����
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubGetPinExtern(char *pszPin, int *pnPinLen, int nMode, const char *pszCardno,
					int nMaxLen, int nMinLen,void (*ShowFunc)());

/**
* @brief ��ȡMac
* @param [in]  nMode         �㷨����---EM_MAC_TYPE
* @param [in]  psData        ����MAC������
* @param [in]  nDataLen      ���ݳ���
* @param [out] psMac         MACֵ
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubCalcMac(int nMode,const char * psData,int nDataLen,char * psMac);

/**
* @brief �������������ʾ����
* @param [in]  nLine         ָ��������1,2
* @param [in]  pszData       ����ʾ���ַ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubDispPinPad (int nLine, const char *pszData);


/**
* @brief ������ָ̻�Ĭ����ʾ����
* @param ��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubClrPinPad(void);

/**
* @brief Des��������
* @param [in]  nKeyType      ��Կ����---EM_KEY_TYPE
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubDes(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief Des��������(�����������)
* @param [in]  nKeyType      ��Կ����---EM_KEY_TYPE
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubUnDes(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief 3Des��������
* @param [in]  nKeyType      ��Կ����---EM_KEY_TYPE
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubDes3(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief 3Des��������(�����������)
* @param [in]  nKeyType      ��Կ����---EM_KEY_TYPE
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubUnDes3(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief ��Des��������
* @param [in]  psKey         ��Կ---8�ֽ�
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubSoftDes(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief ��Des��������
* @param [in]  psKey         ��Կ---8�ֽ�
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubSoftUnDes(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief ��3Des��������
* @param [in]  psKey         ��Կ---16�ֽ�
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubSoftDes3(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief ��3Des��������
* @param [in]  psKey         ��Կ---16�ֽ�
* @param [in]  psSrc         ����---8�ֽ�
* @param [out] psDest        ����---8�ֽ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-5-23
*/
int PubSoftUnDes3(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief ������Կ����ָ������(����������)
* @param [in]  pIn         �����ܵ�����
* @param [in]  nGroupNo    ����Կ��
* @param [out] pOut        ���ܺ������
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ��ε
* @date 2012-08-21
*/
int PubUndesByMk(unsigned char* pOut, const unsigned char* pIn, int nGroupNo);

/**
* @brief ���Ķ�ȡ���������Ϣ(������̵ڶ��������ķ�ʽ��ʾ��������֣���"ȷ��"����)
* @param [in]  pstString     �������������ַ���
* @param [in]  pnStringLen   �ַ�������
* @param [in]  nMaxLen       ������볤��
* @param [in]  nMinLen       ��С���볤��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ��ε
* @date 2012-08-21
*/
int PubReadString(char *pstString, int *pnStringLen, int nMaxLen, int nMinLen);

/**
* @brief ���������Һ������ָ���У���ʾ�����ַ���
* @param [in]  nLine         ָ��������1,2
* @param [in]  pszDotData    ����ʾ������
* @param [in]  nDataLen      ����ʾ�����ݳ���
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ��ε
* @date 2012-08-21
*/
int PubDispDot(int nLine, const char *pszDotData, int nDataLen);

/**
* @brief ��ȡ��ȫģ��汾
* @param [out]pszVer  12�ֽڰ汾��
* @return 
* @li ��
*/
void PubGetSecrityVerion(char *pszVer);

/**
* @brief �������Կ������㼰�������뺯��ǰ���ȵ��ô˲�����
* @param [in]  nMode : DESMODE_DES��DESMODE_3DES
* @param [in]  nIndex : ����Կ������  0-83
* @param [in]  nActiveKeyType : ������Կ��ţ�0--KEY_TYPE_PIN  / 1--KEY_TYPE_MAC    /2--KEY_TYPE_TRACK /3--KEY_TYPE_DATA����Կ����Ϊ����Կʱ���˲�����������
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ��ε
* @date 2012-08-21
*/
int PubActiveKey( int nMode, int nIndex, int nActiveKeyType );

/**
* @brief ʹ�����������ָ����ŵ�����Կ����������Կ��Ȼ�󽫽��ܺ��������Կ���浽ָ����������С�
* @param [in]  psKey		����Կ����
* @param [in]  KeyLen		����Կ���ĳ���
* @param [in]  NewGroup		�����
* @param [in]  OldGroup		���ڽ��ܵ�ԭ���
* @param [out] pCV			У��ֵ
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ��ε
* @date 2012-08-21
*/
int PubUpdateMk(unsigned char *psKey, int nKeyLen, int nNewGroup, int nOldGroup, unsigned char* pCV);
#endif

