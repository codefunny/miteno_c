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
* @brief 初始化黑名单记录文件
*	流水文件不存在则创建新文件，如果已存在则重新创建原有的流水记录被删除。
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int InitBlkFile(void)
{
	STRECFILE stBlkFile;

	strcpy( stBlkFile.szFileName, FILE_BLKCARD);
	stBlkFile.cIsIndex = FILE_NOCREATEINDEX;			/**< 不创建索引文件*/
	stBlkFile.unMaxOneRecLen = sizeof(STBLKCARD);
	stBlkFile.unIndex1Start =0;
	stBlkFile.unIndex1Len =  0;
	stBlkFile.unIndex2Start =  0;
	stBlkFile.unIndex2Len = 0;
	ASSERT_FAIL(PubCreatRecFile(&stBlkFile));
	return APP_SUCC;
}

/**
* @brief 获取黑名单文件记录总数
* @param out pnBlkNum 记录总数
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int GetBlkNum(int *pnBlkNum)
{
	return PubGetRecSum(FILE_BLKCARD, pnBlkNum);
}


/**
* @brief 增加黑名单
* @param in const STBLKCARD *pstBlk 黑名单结构指针
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
* @brief 判断是否不在黑名单中
* @param in const char* pszCardNo 需要进行判断的卡号
* @return
* @li APP_SUCC	不在黑名单中
* @li APP_FAIL	在黑名单中
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
			PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "|C该卡被列入黑名单");
			PubMsgDlg(NULL, szContent, 3, 3);
			return APP_FAIL;
		}
	}
	return APP_SUCC;
}

/**
* @brief 解析62域并保存黑名单到对应文件
* @param in const char* szField62 62域数据，int nField62Len 63域对应的长度
* @return
* @li APP_SUCC	解析并保存成功
* @li APP_FAIL	解析并保存失败
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

