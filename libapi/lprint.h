/******************************************************************************
* �´�½���Թ�˾ ��Ȩ����(c) 2005-2008
*
* POSͳһ��
* ��ӡģ��      ---lprint.h
* ��    �ߣ�    ���
* ��    �ڣ�    2012-07-12
* ����޸��ˣ�  ���
* ����޸����ڣ�2012-07-22
******************************************************************************/
#ifndef _LPRINT_H_
#define	_LPRINT_H_


/**
*	��ӡ��������ֶα�
*/
#define MAX_PRINT_TAG_NAME	32
#define PRINT_DATA_MAXLEN	128
#define MAX_PRINT_MODULE_NUM  40
#define MAX_IMAGE_BLOCK   2000    //ͼƬ�������ݴ�С2000 * 128,�ݶ�,����Ҫ����ϵ����

/**
*   �����붨��
*/
typedef enum{

PRN_OK,                                      /**�����ɹ�*/                                   
PRN_ERR = -4001,				 /**����ʧ��*/ 
PRN_ERR_FILE_LOST = -4002,         /**�ļ�ȱʧ*/
PRN_ERR_FILE_OPEN = -4003,		 /**���ļ�ʧ��*/ 
PRN_ERR_FILE_READ = -4004,		 /**��ȡ�ļ�ʧ��*/ 
PRN_ERR_FILE_WRITE = -4005,	 /**д���ļ�ʧ��*/ 
PRN_ERR_FORMAT = -4006,              /**ģ���ļ���ʽ����*/
PRN_ERR_MODULEINFO = -4007,	 /**�����ģ����Ϣ����*/
PRN_ERR_INITPNT = -4008,             /**��ʼ����ӡ��ʧ��*/
PRN_ERR_IMG_OPER = -4009,           /**����ͼƬ���ݳ���*/
PRN_ERR_IMG_DATA = -4010,         /**ȱ��ͼƬ����*/
PRN_ERR_FONT_ERR = -4011,         /**�������ô���*/

}EM_PRINT_ERR;

#if 0
typedef struct STPRINTFIELD
{
    char szTagName[MAX_PRINT_TAG_NAME+1];
    char szData[PRINT_DATA_MAXLEN+1];		
}STPRINTFIELD;
#endif

//��ӡ������
enum _PRINTTYPE_DEF{
	_PRINTTYPE_TP=0, 	//����
	_PRINTTYPE_IP, 	 	//���
};

typedef enum _PRINTFONTSIZE
{
	PRN_FONT_SMALL = 0, //С����
	PRN_FONT_MIDDLE,    //��
	PRN_FONT_BIG,       //��
}EM_PRN_FONT_SIZE;
/**
* @brief ��ʼ����ӡģ��,����PubPrintModuleǰ��Ҫ����,�ɷ�����ǰ��
* @param [in] pszFileName ��ӡ ģ���ļ�
* @param [in] unPageLen һҳ��ӡֽ��(�����Ч,������������)
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author �ֽ���
* @date 2012-11-14
*/
int PubInitPrint(const char* pszFileName, uint unPageLen);

/**
* @brief ģ���ӡ
* @param [in] pszModuleName, Ҫ��ӡ��ģ����
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author �ֽ���
* @date 2012-11-14
*/
int PubPrintModule(const char* pszModuleName);

/**
* @brief��ô�ӡ������
* @param ��
* @return ��ӡ������
*/
int PubGetPrinter(void);

/**
* @brief ��մ�ӡ������ֵ
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-05-06
*/
int PubClearFieldValue(void);

/**
* @brief ���ô�ӡ������ֵ
* @param [in] pszTagName ��ǩ
* @param [in] pszData ��ǩ��Ӧ����ֵ
* @param [in] nDataLen pszData�ĳ���
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-05-06
*/
int PubSetFieldValue(const char *pszTagName, const char *pszData, const int nDataLen);
/**
* @brief ���ô�ӡ������ֵ,DATA��ʮ������ʱʹ��
* @param [in] pszTagName ��ǩ
* @param [in] pszData ��ǩ��Ӧ����ֵ
* @param [in] nDataLen pszData�ĳ���
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-05-06
*/
int PubSetFieldValueHex(const char * pszTagName, const char * pszData, const int nDataLen);
/**
* @brief ���ô�ӡ������ֵ,VALUE�ɸ�ʽ������
* @param [in] pszTagName ��ǩ
* @param [in] lpszFormat
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-05-06
*/
int PubSetFieldValueFormat(const char *pszTagName, char* lpszFormat, ...);
/**
* @brief ���ô�ӡ������ֵ,ͼƬ����ר��
* @param [in] pszImgTagͼƬ ��ǩ
* @param [in] pszImgBuf ͼƬ����
* @param [in] nImgSize ͼƬ�����С
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-05-06
*/

int PubSetFieldValueImage(const char *pszImgTag, const char *pszImgBuf, const ulong ulImgSize);

/**
* @brief ����ͼƬ��ӡ����,�������,������ģ������(ͼƬ�ļ�)
* @param [in] pszParamTag,��ʶ������tag
* @param [in] pszPicName ͼƬ��
* @param [in] unXpos ��ӡλ��,ͼƬ���Ͻ�λ��
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-09-24
*/
int PubSetParamLogo(const char *pszParamTag,const char *pszPicName, uint unXpos);

/**
* @brief ����ͼƬ��ӡ����,�������,������ģ������(ͼƬ����)
* @param [in] pszParamTag,����ʶ����,��ͼƬ���黹�����PubSetFieldValueImage����
* @param [in] unXsize ͼƬ��
* @param [in] unYsize ͼƬ��
* @param [in] unXpos ��ӡλ��,ͼƬ���Ͻ�λ��
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2013-09-24
*/
int PubSetParamImg(const char *pszParamTag, uint unXsize,uint unYsize,uint unXpos);

/**
* @brief ���ô�ӡ����size(С,��,��),�������,���ģ��(S-S,M,B)����
* @param [in] EM_PRN_FONT_SIZE,��ӡ����size,��EM_PRN_FONT_SIZE
* @return 
* @li APP_SUCC �ɹ�
* @author �����
* @date 2014-06-27
*/
int PubSetPrnFontSize(EM_PRN_FONT_SIZE emSize);


/**
* @brief ����ӡ��״̬
* @param ��
* @return 
* @li ��ӡ״ֵ̬
* @author ���
* @date 2012-9-19
*/
int PubGetPrintStatus(void);


/**
* @brief ���ô�ӡ�ṹ
* @param [in] unBorder  ��߾� ֵ��Ϊ��0-288(Ĭ��Ϊ0)  
* @param [in] unColumn  �ּ�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)  
* @param [in] unRow     �м�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)  
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-09-19
*/
int PubSetPrintForm (uint unBorder, uint unColumn, uint unRow);

/**
* @brief ���ô�ӡ����
* @param [in] emDotFont       Ӣ������
* @param [in] emHZFont        ��������
* @param [in] emMode          ��ӡģʽ
* @param [in] unSigOrDou      ������˫���ӡ(���������Ч)
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-09-19
*/
int PubSetPrintFont (EM_PRN_ZM_FONT emDotFont,EM_PRN_HZ_FONT emHZFont,EM_PRN_MODE emMode,uint unSigOrDou);


/**
* @brief ��֯��ӡ��������
* @details	�����ݳ���һ�����üӻ��з����Զ�����.
* @param [in] pszContent   ��ӡ����
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-6-5
*/
int PubPrePrinter(const char *pszContent, ...);


/**
* @brief ����
* @param [in] nLine   ���е�����
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-6-5
*/
int PubPaperPeed(int nLine);


/**
* @brief ����PubPrintCommit�����д�ӡ����ֽ
* @param [in] nFeedPaper  0 ��ҳ��ӡǰ��ֽ, 1 ��ҳ��ӡ��ɺ��ֽ
* @return 
* @��
* @author ���
* @date 2014-04-08
*/
void PubPrintFeedPaper(int nFeedPaper);


/**
* @brief �ύ��ӡ
* @details	��ʽ��ʼ��ӡ
* @param [in] (*SendPrintData)() ---����ָ�루���ӡ�����ݣ�
* @param [in] pvParam            ---�������
* @param [in] unPrintType        ---��ӡ���� 0:һ���Դ�ӡ,1:���ͱߴ�
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-9-20
*/
int PubPrintCommit(int (*SendPrintData)(), void *pvParam,uint unPrintType);


/**
* @brief ��ӡλͼͼƬ
* @param [in] unXSize:ͼ�εĿ��
* @param [in] unYSize:ͼ�εĸ߶�
* @param [in] unXPos:ͼ�ε����Ͻǵ���λ��
* @param [in] psPrintBuf:λͼ��Դ
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-9-19
*/
int PubPrintImage(uint unXSize, uint unYSize, uint unXPos,const char *psPrintBuf);


/**
* @brief ��ӡͼƬ
* @details	��ӡbmp��png�ȸ�ʽ��ͼƬ
* @param [in] unXPos:ͼ�ε����Ͻǵ���λ��
* @param [in] pszPicPath:ͼƬ·��
* @return 
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ���
* @date 2012-9-19
*/
int PubPrintPicture(uint unXPos,const char *pszPicPath);

/**
* @brief ��ȡ��ӡ��汾
* @param pszOutVer ���ذ汾��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PubGetPrintLibVer(char *pszOutVer);

/**
* @brief�͵�ѹ���
* @param ��
* @li APP_SUCC �͵�ѹ
* @li APP_FAIL 
*/
int PubLowPowerCheck(void);

#endif
/* End of lprint.h */

