#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

#define FILE_BLKCARD APP_NAME"BLK"

typedef struct
{
	char cLen;
	char szCardNo[14+1];
}STBLKCARD;



/**
* @brief ��ʼ����������¼�ļ�
*	��ˮ�ļ��������򴴽����ļ�������Ѵ��������´���ԭ�е���ˮ��¼��ɾ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitBlkFile(void)
{
	STRECFILE stBlkFile;

	strcpy( stBlkFile.szFileName, FILE_BLKCARD);
	stBlkFile.cIsIndex = FILE_NOCREATEINDEX;			/**< �����������ļ�*/
	stBlkFile.unMaxOneRecLen = sizeof(STBLKCARD);
	stBlkFile.unIndex1Start =0;
	stBlkFile.unIndex1Len =  0;
	stBlkFile.unIndex2Start =  0;
	stBlkFile.unIndex2Len = 0;
	ASSERT_FAIL(PubCreatRecFile(&stBlkFile));
	return APP_SUCC;
}

/**
* @brief ��ȡ�������ļ���¼����
* @param out pnBlkNum ��¼����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetBlkNum(int *pnBlkNum)
{
	return PubGetRecSum(FILE_BLKCARD, pnBlkNum);
}


/**
* @brief ���Ӻ�����
* @param in const STBLKCARD *pstBlk �������ṹָ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int AppendBlk(const STBLKCARD *pstBlk)
{	
	ASSERT_FAIL(PubAddRec(FILE_BLKCARD, (const char *)pstBlk ));
	return APP_SUCC;
}


/**
* @brief �ж��Ƿ��ں�������
* @param in const char* pszCardNo ��Ҫ�����жϵĿ���
* @return
* @li APP_SUCC	���ں�������
* @li APP_FAIL	�ں�������
*/
int CheckIsNotInBlk(const char* pszCardNo)
{
	int i, nSum = 0; 
	STBLKCARD stBlkCard;
	char szContent[100];

	GetBlkNum(&nSum);
	for(i = 1; i <= nSum; i++)
	{
		memset(&stBlkCard, 0, sizeof(stBlkCard));
		PubReadOneRec(FILE_BLKCARD, i, (char*)&stBlkCard);
		if(memcmp(stBlkCard.szCardNo, pszCardNo, stBlkCard.cLen) == 0)
		{
			memset(szContent, 0, sizeof(szContent));
			PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C�ÿ������������");
			PubMsgDlg(NULL, szContent, 3, 3);
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}

/**
* @brief ����62�򲢱������������Ӧ�ļ�
* @param in const char* szField62 62�����ݣ�int nField62Len 63���Ӧ�ĳ���
* @return
* @li APP_SUCC	����������ɹ�
* @li APP_FAIL	����������ʧ��
*/
int SaveBlkList(const char* pszField62, int nField62Len)
{
	int i, nLen =0;
	char szCardLen[2+1];
	STBLKCARD stBlkCard;

	for(i = 0; nLen < nField62Len; i++)
	{
		memset(szCardLen, 0, sizeof(szCardLen));
		memset(&stBlkCard, 0, sizeof(stBlkCard));
		
		memcpy(szCardLen, pszField62+nLen, 2);
		stBlkCard.cLen = atoi(szCardLen);
		if(stBlkCard.cLen <= 0 || stBlkCard.cLen > 14)
		{
			return APP_FAIL;
		}
		memcpy(stBlkCard.szCardNo, pszField62+nLen+2, stBlkCard.cLen);
		nLen += 2;
		nLen += stBlkCard.cLen;
		if(AppendBlk(&stBlkCard) != APP_SUCC)
		{
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}

