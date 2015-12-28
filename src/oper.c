/**
* @file oper.c
* @brief ����Ա����ģ��
* @version  1.0
* @author Ѧ��
* @date 2007-01-26
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

#define MAXOPERNUM		22				/**< ϵͳ�����������Ա��*/				
#define ADMINPWD		"12345678"		/**< ϵͳ����Ա��ʼ����*/
#define MANAGERPWD		"123456"			/**< ���ܳ�ʼ����*/
#define NORMALOPERPWD	"0000"			/**< ��ͨ����Ա��ʼ����*/

#define MANAGERPWDLEN		6				/**< ���ܲ���Ա���볤��*/
#define NORMALPWDLEN		4				/**< ��ͨ����Ա���볤��*/			
#define ADMINOPERPWDLEN	8				/**< ϵͳ����Ա���볤��*/

#define LOGINERRCOUNT		3				/**< �������Ա�ѻ�ǩ���Ĵ������*/

#define FILE_OPER		APP_NAME"OPER"

typedef struct
{
	char szOperNo[OPERCODELEN+1];
	char cOperRole;
	char cOperPwdLen;
	char szPasswd[12+1];
	char cDeleFlag;
	char cRfu;/**<Ԥ��*/
} STOPER_RECORD;

static char gszCurrentOperNo[OPERCODELEN+1];
static char gszCurrentOperPasswd[ADMINOPERPWDLEN+1];
static char gszOperLimit[OPERCODELEN+1];
static int gnCurrentOperRole;

/**
* ����Ϊ��ģ���ڲ�ʹ�õĺ���
*/
static void AppendOperRec(const char *, const char *);
static int FindOper(int , STOPER_RECORD *, int * );
static int CountOper(int *);

/**
* @brief ��ʼ������Ա��¼
*
*	��ʼ��ϵͳ����Ա���ݣ�����һ��ϵͳ����Ա��һ�����ܣ�
* �����ͨ����Ա��ż���Ӧ�ĳ�ʼ����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
void InitOper(void)
{
	int i;
	STOPER_RECORD stOperRec;
	STRECFILE stOperFile;
	
	/**
	* ��������Ա�ļ��������г�ʼ��
	*/
	memset(&stOperFile, 0, sizeof(STRECFILE));
	strcpy( stOperFile.szFileName, FILE_OPER );
	stOperFile.cIsIndex = FILE_CREATEINDEX;						/**< ���������ļ�*/
	stOperFile.unMaxOneRecLen = sizeof(STOPER_RECORD);
	stOperFile.unIndex1Start =(uint)( (char *)(stOperRec.szOperNo) - (char *)&stOperRec)+1;
	stOperFile.unIndex1Len =  OPERCODELEN;
	stOperFile.unIndex2Start =  (uint)( (char *)(&stOperRec.cDeleFlag) - (char *)&stOperRec)+1;
	stOperFile.unIndex2Len = sizeof(stOperRec.cDeleFlag);
	ASSERT(PubCreatRecFile(&stOperFile));	

	/**
	* ��ʼ�� ϵͳ����Ա��¼�����99
	*/

	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 99 );
	stOperRec.cOperRole = ADMINOPER;
	stOperRec.cOperPwdLen = ADMINOPERPWDLEN;
	//sprintf( stOperRec.szPasswd, "%-12.12s", ADMINPWD );
	strcpy(stOperRec.szPasswd, ADMINPWD );
	stOperRec.cDeleFlag = '+';
	PubAddRec(FILE_OPER, (const char *)&stOperRec );

	/**
	* ��ʼ�� ���ܲ���Ա��¼�����00
	*/

	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 0 );
	stOperRec.cOperRole = MANAGEROPER;
	stOperRec.cOperPwdLen = MANAGERPWDLEN;
	//sprintf( stOperRec.szPasswd, "%-12.12s", MANAGERPWD );
	strcpy(stOperRec.szPasswd, MANAGERPWD );
	stOperRec.cDeleFlag = '+';
	PubAddRec(FILE_OPER, (const char *)&stOperRec );
	
	/**
	* ��ʼ�������ͨ����Ա��¼,���01-05
	*/
	
	stOperRec.cOperRole = NORMALOPER;
	stOperRec.cOperPwdLen = NORMALPWDLEN;
	//sprintf( stOperRec.szPasswd, "%-12.12s", NORMALOPERPWD );
	strcpy(stOperRec.szPasswd, NORMALOPERPWD );
	stOperRec.cDeleFlag = '+';

	for(i = 0; i < 5; i++)
	{
		sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, i+1 );
		PubAddRec(FILE_OPER, (const char *)&stOperRec );
	}

	memset(gszCurrentOperNo, 0, OPERCODELEN+1);
	memset(gszCurrentOperPasswd, 0, ADMINOPERPWDLEN+1);
	memset(gszOperLimit, 0, OPERCODELEN+1);
	gnCurrentOperRole = ADMINOPER;
}

/**
* @brief ͳ��ϵͳ����Ա��
* @param in int *pnOperCount  ����Ա����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int CountOper(int *pnOperCount)
{
	int i;
	int nRecNum=0;
	STOPER_RECORD stOperRec;
	
	*pnOperCount = 0;

	ASSERT_FAIL(PubGetRecSum(FILE_OPER, &nRecNum));
	for(i = 0; i < nRecNum; i++)
	{
		ASSERT_FAIL(PubReadOneRec(FILE_OPER, i+1, (char *)&stOperRec));
		if ( stOperRec.cDeleFlag == '+' )
		{
			*pnOperCount = *pnOperCount + 1;
		}
	}
	return APP_SUCC;
}

/**
* @brief ���Ӳ���Ա
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int AddOper(void)
{
	STOPER_RECORD stOperRec;
	int nCurOperRecNo, nOperNum;
	int nRet, nLen1, nLen2;
	char szOperNo[OPERCODELEN+1]={0};
	char szPassW1[ADMINOPERPWDLEN+1]={0},szPassW2[ADMINOPERPWDLEN+1]={0};
	char szTempStr[20];

	/**
	* ͳ��ϵͳ�ܲ���Ա��
	*/
	ASSERT_FAIL(CountOper(&nOperNum));

	/**
	* �жϲ���Ա���Ƿ����������
	*/
	if (nOperNum >= MAXOPERNUM)
	{
      	PubClearAll();
 		PubDisplayGen( 2, "����Ա��������" );
 		PubDisplayGen( 3, "����ɾ��һ��" );
 		PubUpdateWindow();
		PubGetKeyCode(5);
		return APP_FAIL;
	}
	
	/**
	* ������Ҫ���ӵĲ���Ա���
	*/
	memset(szOperNo, 0, sizeof(szOperNo));
	nRet = PubInputDlg("���Ӳ���Ա", "���������Ա�ţ�", szOperNo, &nLen1, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}

	if( nLen1 != OPERCODELEN )
	{
		PubMsgDlg("���Ӳ���Ա", "����λ��������", 0, 5 );
		return APP_FAIL;
	}
	
	/**
	* �жϲ���Ա����Ƿ����
	*/
	if (PubFindRec (FILE_OPER, szOperNo, "+", 1, (char *)&stOperRec, &nCurOperRecNo) == APP_SUCC )
	{
		PubMsgDlg("���Ӳ���Ա", "�ò���Ա�Ѵ���!", 0, 5 );
		return APP_FAIL;
	}
	
	/**
	* �����������벢���бȽ�
	*/
	sprintf(szTempStr, "����Ա���:%2.2s",szOperNo);
	memset(szPassW1, 0, sizeof(szPassW1));
	nRet = PubInputDlg(szTempStr, "�������Ա����:", szPassW1, &nLen1, 0, NORMALPWDLEN, 0, INPUT_MODE_PASSWD);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}

	/**
	* �ж�����λ���ĺϷ���
	*/
	if (nLen1 == NORMALPWDLEN || nLen1 == 0)
	{
		;
	}
	else
	{
		PubMsgDlg("���Ӳ���Ա", "����λ������!", 0, 5 );
		return APP_FAIL;
	}

	memset(szPassW2, 0, sizeof(szPassW2));
	nRet = PubInputDlg(szTempStr, "������һ������:", szPassW2, &nLen2, 0, NORMALPWDLEN, 0, INPUT_MODE_PASSWD);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}


	if ( nLen1 != nLen2 || memcmp(szPassW1,szPassW2,NORMALPWDLEN) !=0 )
	{
		PubMsgDlg("���Ӳ���Ա", "�����������Ա���벻һ��", 0, 3 );
		return APP_FAIL;
	}

	if( nLen2 == 0 )
	{
		strcpy(szPassW1, NORMALOPERPWD);	
	}
	AppendOperRec(szOperNo, szPassW1);
	PubMsgDlg("���Ӳ���Ա", "����Ա���ӳɹ�!", 0, 1);
	return APP_SUCC;
}

/**
* @brief  ���ݸ����Ĳ���Ա��ź��������Ӳ���Ա��¼
* @param in char *pszOperNo ����Ա���
* @param out char *pszPasswd ����Ա����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static void AppendOperRec(const char *pszOperNo, const char *pszPasswd)
{
	STOPER_RECORD stOperRec;
	int nCurOperRecNo, nRet;

	/**
	* �жϲ���Ա����Ƿ��ڼ�¼�ļ���ʹ�ù�
	*/
	nRet = PubFindRec (FILE_OPER, pszOperNo, "-", 1, (char *)&stOperRec, &nCurOperRecNo);

	strcpy( stOperRec.szOperNo, pszOperNo );
	stOperRec.cOperRole = NORMALOPER;
	stOperRec.cOperPwdLen = NORMALPWDLEN;
	strcpy( stOperRec.szPasswd, pszPasswd );
	stOperRec.cDeleFlag = '+';	

	if (nRet == APP_SUCC )
	{
		ASSERT(PubUpdateRec (FILE_OPER, nCurOperRecNo, (const char *)&stOperRec ));	
	}
	else
	{
		ASSERT(PubAddRec(FILE_OPER, (const char *)&stOperRec ));		
	}
}



/**
* @brief  ɾ������Ա
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int DelOper(void)
{
	STOPER_RECORD stOperRec;
	int nCurOperRecNo, nOperNum;
	int nRet, nLen;
	char szOperNo[OPERCODELEN+1], szTmpOperNo[OPERCODELEN+1];

	/**
	* ͳ��ϵͳ�ܲ���Ա��
	*/
	ASSERT_FAIL(CountOper( &nOperNum ));
	if ( nOperNum == 2 )
	{
		PubMsgDlg("ɾ������Ա", "�Բ����޲���Ա", 0, 3);
		return APP_QUIT;
	}

	/**
	* ������Ҫɾ�Ĳ���Ա���
	*/
	memset(szOperNo, 0, sizeof(szOperNo));
	nRet = PubInputDlg("ɾ������Ա", "���������Ա�ţ�", szOperNo, &nLen, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}
	if( nLen != OPERCODELEN )
	{
		PubMsgDlg("ɾ������Ա", "����λ��������", 0, 5);
		return APP_FAIL;
	}
	sprintf(szTmpOperNo, "%0*d", OPERCODELEN, 99 );
	if (!memcmp(szOperNo,szTmpOperNo,OPERCODELEN))
	{
		PubMsgDlg("ɾ������Ա", "ϵͳ����Ա����ɾ��", 0, 3 );
		return APP_QUIT;
	}
	if( !memcmp( szOperNo, gszCurrentOperNo, OPERCODELEN ) )
	{
		PubMsgDlg("ɾ������Ա", "��ǰ����Ա����ɾ��", 0, 3 );
		return APP_QUIT;			
	}

	if( gszOperLimit[0] != 0 && memcmp( szOperNo, gszOperLimit, OPERCODELEN ) == 0 )
	{
		PubMsgDlg("ɾ������Ա", "����Ա����ˮδ���㣬����ɾ��", 0, 3 );
		return APP_QUIT;
	}

	if ( APP_SUCC==IsOperHasWater(szOperNo) )
	{
		PubMsgDlg("ɾ������Ա", "����Ա����ˮδ���㣬����ɾ��", 0, 3 );
		return APP_QUIT;
	}
	
	if( PubFindRec (FILE_OPER, szOperNo, "+", 1, (char *)&stOperRec, &nCurOperRecNo) != APP_SUCC )
	{
		PubMsgDlg("ɾ������Ա", "�޴˲���Ա", 0, 3 );
		return APP_FAIL;
	}
	else
	{
		if ( stOperRec.cOperRole == MANAGEROPER || stOperRec.cOperRole == ADMINOPER )
		{
			PubMsgDlg("ɾ������Ա", "���ܲ���Ա����ɾ��", 0, 3 );
			return APP_QUIT;
		}
		stOperRec.cDeleFlag = '-';
		PubUpdateRec (FILE_OPER, nCurOperRecNo, (const char *)&stOperRec );	
		PubMsgDlg("ɾ������Ա", "����Աɾ���ɹ�", 0, 3 );
	}
	return APP_SUCC;
}

/**
* @brief �鿴���е���ͨ����Ա��Ϣ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static void QueryOper(void)
{
	STOPER_RECORD stOperRec;
	int nOperNum = 0, nRecNum, i;
	char szDispStr[512+1]={0};
	
	PubGetRecSum(FILE_OPER, &nRecNum);

	for(i = 0; i < nRecNum; i++)
	{
		PubReadOneRec(FILE_OPER, i+1, (char *)&stOperRec);
		if ( stOperRec.cDeleFlag != '+' )
		{
			continue;
		}
		nOperNum++;			
		sprintf( szDispStr+strlen(szDispStr), "%-3.*s", OPERCODELEN, stOperRec.szOperNo );
	}

	if( nOperNum == 0 )
	{
		PubMsgDlg("��ѯ����Ա", "�Բ����޲���Ա", 0, 3 );
		return;
	}

	PubUpDownMsgDlg("��ѯ����Ա", szDispStr, YES, 60, NULL);
	return;
}

/**
* @brief ���ĵ�ǰ����Ա����
* @param in nRole ����Ա��ɫ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int ChgOperPwd(int nRole)
{
	STOPER_RECORD stOperRec;
	char szOperPasswd[ADMINOPERPWDLEN+1]={0}, szPasswdStr[ADMINOPERPWDLEN+1]={0};
	char szTmpDispStr1[32]={0}, szTmpDispStr2[32]={0};
	int nRet, nLen, nOperRecNum, nPasswdLen;

	if( (nRet = FindOper( nRole, &stOperRec, &nOperRecNum  ) )!=APP_SUCC)
	{
		return nRet;
	}

	while(1)
	{
		switch( nRole )
		{
		case MANAGEROPER:
			strcpy( szTmpDispStr1, "�޸���������" );
			strcpy( szTmpDispStr2, "������ԭ����:" );	
			nPasswdLen = MANAGERPWDLEN;
			break;
		case NORMALOPER:
			sprintf( szTmpDispStr1, "����Ա���:%-4.*s", OPERCODELEN, gszCurrentOperNo );
			if( ( nRet = PubConfirmDlg("�޸Ĳ���Ա����", szTmpDispStr1, 0, 50) ) !=APP_SUCC )
			{
				return nRet;
			}
			strcpy( szTmpDispStr1, "�޸Ĳ���Ա����" );
			strcpy( szTmpDispStr2, "������ԭ����:" );
			nPasswdLen = NORMALPWDLEN;
			break;
		case ADMINOPER:
			strcpy( szTmpDispStr1, "�޸Ĺ���Ա����" );			
			strcpy( szTmpDispStr2, "�������Ա����:" );			
			nPasswdLen = ADMINOPERPWDLEN;
			break;
		default:
			return APP_FAIL;
		}
		/**
		 * ����ԭ���벢У��
		 */
		memset(szOperPasswd, 0, sizeof(szOperPasswd));		
		nRet = PubInputDlg(szTmpDispStr1, szTmpDispStr2, szOperPasswd, &nLen, stOperRec.cOperPwdLen, stOperRec.cOperPwdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}
		if( memcmp(szOperPasswd, stOperRec.szPasswd,  stOperRec.cOperPwdLen) )
		{
			PubMsgDlg(szTmpDispStr1, "ԭ���벻��", 0, 5 ) ;
			continue;
		}
		memset(szOperPasswd, 0, sizeof(szOperPasswd));
		nRet = PubInputDlg(szTmpDispStr1, "������������:", szOperPasswd, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}
		memset(szPasswdStr, 0, sizeof(szPasswdStr));
		nRet = PubInputDlg(szTmpDispStr1, "��ȷ��������:", szPasswdStr, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}

		if(  memcmp(szOperPasswd, szPasswdStr, nPasswdLen) )
		{
			PubMsgDlg(szTmpDispStr1, "�����������벻ͬ", 0, 5 ) ;
			continue;
		}
		else
		{
			strcpy( stOperRec.szPasswd,  szOperPasswd );
			stOperRec.cOperPwdLen = nPasswdLen;
			nRet = PubUpdateRec (FILE_OPER, nOperRecNum, (const char *)&stOperRec );	
			if(nRet == APP_SUCC)
			{
				PubMsgDlg(szTmpDispStr1, "�����޸ĳɹ�", 0, 5 ) ;
			}
			else
			{
				PubMsgDlg(szTmpDispStr1, "�����޸�ʧ��", 0, 5 ) ;
			}
			return nRet;
		}

	}
}

/**
* @brief �����ṩ�Ĳ���Ա�Ľ�ɫ���õ�����Ա����Ϣ��
* @param in nRole ����Ա��ɫ
* @param out STOPER_RECORD *pstOperRec ����Ա��¼��Ϣ
* @param out int *pnOperRecNum  ����Ա��¼��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static int FindOper( int nRole, STOPER_RECORD *pstOperRec, int *pnOperRecNum )
{
	int i, nRet;
	int nRecNum;

	if (nRole == NORMALOPER)
	{
		nRet = PubFindRec (FILE_OPER, gszCurrentOperNo, "+", 1, (char *)pstOperRec, pnOperRecNum) ;
		if( nRet == APP_SUCC )
		{
			return APP_SUCC;
		}
		else
		{
			return APP_FAIL;
		}
	}

	PubGetRecSum(FILE_OPER, &nRecNum);
	for(i = 0; i < nRecNum; i++)
	{
		ASSERT_FAIL(PubReadOneRec(FILE_OPER, i+1, (char *)pstOperRec));
		if ( pstOperRec->cDeleFlag == '+'  && pstOperRec->cOperRole == nRole )
		{
			*pnOperRecNum = i + 1;
			return APP_SUCC;
		}
	}
	return APP_FAIL;
}


/**
* @brief POS�����Լ�����
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PosLockUnLock(void)
{
	STOPER_RECORD stOperRec;
	char szOper[OPERCODELEN+1];
	int nRet, nLen, nOperRecNum;

	static int LockOperChgFlag = 0;
	static int nOldOperRole;
	static char szOldOperNo[OPERCODELEN+1], szOldOperPasswd[ADMINOPERPWDLEN+1];

	if( LockOperChgFlag )
	{
		strcpy(gszCurrentOperNo,  szOldOperNo);
		strcpy(gszCurrentOperPasswd, szOldOperPasswd);
		gnCurrentOperRole = nOldOperRole;
	}
	
	while(1)
	{
		memset(szOper, 0, sizeof(szOper));
		nRet = PubInputDlg("�ն�������", "���������Ա��", szOper, &nLen, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
		if (nRet != APP_SUCC)
		{
			continue;
		}

		nRet = PubFindRec(FILE_OPER, szOper, "+", 1, (char *)&stOperRec, &nOperRecNum);
		if (nRet !=APP_SUCC )
		{
			PubMsgDlg("�ն�������", "����Ա���벻��", 0, 3 ) ;
			continue;
		}

		if( (stOperRec.cOperRole !=MANAGEROPER)  && ( memcmp( szOper, gszCurrentOperNo, OPERCODELEN )))
		{
			PubMsgDlg("�ն�������", "����Ա���벻��", 0, 3 ) ;
			continue;				
		}

		if( CheckOper("�ն�������", stOperRec.cOperRole) == APP_SUCC )
		{
			if ((stOperRec.cOperRole == MANAGEROPER) && (memcmp( szOper,gszCurrentOperNo, OPERCODELEN ) != 0))
			{
				LockOperChgFlag = 1;
				strcpy(szOldOperNo, gszCurrentOperNo);
				strcpy(szOldOperPasswd, gszCurrentOperPasswd);
				nOldOperRole = gnCurrentOperRole;
				
				strcpy( gszCurrentOperNo, szOper );
				memcpy(gszCurrentOperPasswd, stOperRec.szPasswd, ADMINOPERPWDLEN);
				//strcpy( gszCurrentOperPasswd, stOperRec.szPasswd ); ������ɳ�����ϣ�
				//��˹��ϣ�ͬʱ���� InitOper�еĴ���
				gnCurrentOperRole = MANAGEROPER;
			}
			else
			{
				LockOperChgFlag = 0;
			}
			PubMsgDlg("�ն�������", "�����ɹ�", 0, 1); 
			return APP_SUCC;
		}
		else
		{
			continue;
		}
	}
}

/**
* @brief ����Աǩ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int OperLogon(void)
{
	STOPER_RECORD stOperRec;
	int nRet, nLen,nPasswdLen, nOperRecNum, nErrCount,nKeyCode;
	char szOper[OPERCODELEN+1]={0};
	char szOperPasswd[ADMINOPERPWDLEN+1]={0};
	char szExistPwd[8+1]={0};
	
	memset(szExistPwd,0,sizeof(szExistPwd));
	GetHideMenuPwd(szExistPwd);

	nErrCount = 0;
	for(;;)
	{
		if ( nErrCount == LOGINERRCOUNT )
		{
			PubMsgDlg("ǩ��", "ǩ��ʧ�ܣ�", 0, 3 ) ;
			return APP_FAIL;
		}
		memset(szOper, 0, sizeof(szOper));
		nRet = PubInputDlg("ǩ��", "���������Ա��", szOper, &nLen, 0, OPERCODELEN, 0, INPUT_MODE_NUMBER);
		if (nRet == APP_QUIT)
		{
			return APP_QUIT;
		}
		if (nRet != APP_SUCC)
		{
			continue;
		}
		else if(nLen != 2)
		{
			nKeyCode = PubGetKeyCode(1);
			if(nKeyCode == KEY_F1)
			{
				Version();
			}
			continue;
		}

		nRet = PubFindRec(FILE_OPER, szOper, "+", 1, (char *)&stOperRec, &nOperRecNum);
		nPasswdLen = NORMALPWDLEN;
		if(nRet == APP_SUCC)
		{
			nPasswdLen = stOperRec.cOperPwdLen;
		}

		if (nRet!=APP_SUCC)
		{
			PubMsgDlg("ǩ��", "�޴˲���Ա", 3, 3);
			nErrCount++;
			continue;
		}
		
		memset(szOperPasswd, 0, sizeof(szOperPasswd));
		if (PubInputDlg("ǩ��", "����������", szOperPasswd, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD) != APP_SUCC)
		{
			continue;
		}
		
		if(memcmp(szOperPasswd, stOperRec.szPasswd, stOperRec.cOperPwdLen ))
		{
			if((memcmp(szOper,"99",2)==0) && memcmp(szOperPasswd,szExistPwd,8)==0)
			{
				strcpy(gszCurrentOperNo, szOper);			
				strcpy(gszCurrentOperPasswd,  szOperPasswd);
				gnCurrentOperRole = HIDEMENUOPER;
				return APP_SUCC;
			}
			PubMsgDlg("ǩ��", "�������", 3, 3);
			nErrCount++;
			continue;
		}
		else
		{
			strcpy(gszCurrentOperNo, szOper);			
			strcpy(gszCurrentOperPasswd,  szOperPasswd);
			gnCurrentOperRole = stOperRec.cOperRole;
			return APP_SUCC;
		}
	}
	
}

/**
* @brief ��֤����Ա
* @param in char *pszTitle ��֤��ʾ��Ϣ
* @param in nRole ����Ա��ɫ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int CheckOper(const char *pszTitle,int nRole)
{
	STOPER_RECORD stOperRec;
	int nRet, nLen,nPasswdLen, nOperRecNum;
	char szOperPasswd[ADMINOPERPWDLEN+1];
	char szDispStr1[32], szDispStr2[32];

	switch(nRole)
	{
	case MANAGEROPER:
		strcpy( szDispStr1, "��������������" );
		strcpy( szDispStr2, "���������" );		
		nPasswdLen = MANAGERPWDLEN;
		break;
	case NORMALOPER:
		strcpy( szDispStr1, "���������Ա����" );
		strcpy( szDispStr2, "����Ա�����" );
		nPasswdLen = NORMALPWDLEN;
		break;
	case ADMINOPER:
		nPasswdLen = ADMINOPERPWDLEN;
		strcpy( szDispStr1, "����ϵͳ��������" );
		strcpy( szDispStr2, "ϵͳ����Ա�����" );
		break;
	default:
		return APP_FAIL;
	}

	if( (nRet = FindOper( nRole, &stOperRec, &nOperRecNum ) )!=APP_SUCC)
	{
		return nRet;
	}
	memset(szOperPasswd, 0, sizeof(szOperPasswd));
	ASSERT_RETURNCODE(PubInputDlg(pszTitle, szDispStr1, szOperPasswd, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD));

	if( memcmp(szOperPasswd, stOperRec.szPasswd,  nPasswdLen) )
	{
		PubMsgDlg(pszTitle, szDispStr2, 0, 5 ) ;
		return APP_FAIL;
	}
	else
	{
		return APP_SUCC;
	}
}

/**
* @brief ��õ�ǰ����Ա�ı�ź�����
* @return
* @param out char *pszOperNo ��ǰ����Ա���
* @param out char *pszOperPasswd ��ǰ����Ա����
* @int *pnRole ��ǰ����Ա��ɫ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
void GetCurrentOper(char *pszOperNo, char *pszOperPasswd, int *pnRole)
{
	if (pszOperNo !=NULL )
	{
		strcpy( pszOperNo, gszCurrentOperNo );
	}
	if (pszOperPasswd != NULL )
	{
		strcpy( pszOperPasswd, gszCurrentOperPasswd );
	}
	if (pnRole !=NULL )
	{
		*pnRole = gnCurrentOperRole;
	}
}


/**
* @brief ���õ�ǰ����Ա�ı�ź�����
* @return
* @param out char *pszOperNo ��ǰ����Ա���
* @param out char *pszOperPasswd ��ǰ����Ա����
* @int *pnRole ��ǰ����Ա��ɫ
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
void SetCurrentOper(char *pszOperNo, char *pszOperPasswd, int nRole)
{
	if (pszOperNo !=NULL )
	{
		memcpy(gszCurrentOperNo, pszOperNo, OPERCODELEN);
	}
	if (pszOperPasswd != NULL )
	{
		memcpy(gszCurrentOperPasswd, pszOperPasswd, ADMINOPERPWDLEN);
	}
	if (nRole >= 0 )
	{
		gnCurrentOperRole = nRole;
	}
}

/**
* @brief ���Ʋ���Ա�ĵ�¼
* @return
* @param in char *pszOper �����¼�Ĳ���Ա���
*            ���pszOperΪNULL����""������POS���������Ա���е�¼����
*            ����POS����ֻ����ָ���Ĳ���Ա�Ž��е�¼
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
void SetOperLimit( char *pszOper )
{
	memset( gszOperLimit, 0, OPERCODELEN+1);
	if ( pszOper != NULL )
	{
		strncpy( gszOperLimit, pszOper, OPERCODELEN );
	}
}

/**
* @brief ����Ա����,�˵�����
* @parm ��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int Operate(void)
{
	int nRet = 0, nSelect;

	for (;;)
	{
		if(gnCurrentOperRole == NORMALOPER)
		{
			nRet = ProSelectList("1.���ܸ���|2.����|3.ɾ��|4.��ѯ|5.����Ա����", "��Ա����", 0xFF, &nSelect);
		}
		else
		{
			nRet = ProSelectList("1.���ܸ���|2.����|3.ɾ��|4.��ѯ", "��Ա����", 0xFF, &nSelect);
		}
		if (nRet==APP_QUIT)
		{
			if(gnCurrentOperRole == NORMALOPER)
			{
				return APP_SUCC;
			}
			else
			{
				if (APP_SUCC == PubConfirmDlg("��Ա����", "�˳��������", 0, 30))
					return APP_SUCC;
				continue;	
			}
		}
		switch(nSelect)
		{
		case 1:
			ChgOperPwd(MANAGEROPER);
			break;
		case 2:
			if (CheckOper("���Ӳ���Ա",MANAGEROPER)==APP_SUCC)
			{
				AddOper();
			}
			break;
		case 3:
			if (CheckOper("ɾ������Ա",MANAGEROPER)==APP_SUCC)
			{
				DelOper();
			}
			break;
		case 4:
			QueryOper();
			break;
		case 5:
			ChgOperPwd(gnCurrentOperRole);
			break;
		default:
			break;
		}
	}
	return APP_SUCC;
}

/**
* @brief ����Ini�ļ��ṩ��ϵͳ����Ա������������ļ���Ϣ�ĸ���
* @parm in char *szPasswd 	ϵͳ����Ա���� 
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int SetAdminPwd(const char *pszPasswd)
{
	int nFileHandle;
	int nRet;
	int nCurOperRecNo;
	STOPER_RECORD stOperRec;

	nRet = PubOpenFile(FILE_OPER, "r", &nFileHandle);
	if (nRet != APP_SUCC )
	{
		InitOper();
	}
	else
	{
		PubCloseFile(&nFileHandle);
	}
	
	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 99 );

	nRet = PubFindRec (FILE_OPER, stOperRec.szOperNo, "+", 1, (char *)&stOperRec, &nCurOperRecNo);

	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 99 );
	stOperRec.cOperRole = ADMINOPER;
	stOperRec.cOperPwdLen = ADMINOPERPWDLEN;
	if (NULL == pszPasswd)
	{
		strcpy(stOperRec.szPasswd, ADMINPWD);
	}
	else
	{
		strcpy(stOperRec.szPasswd, pszPasswd);
	}
	
	stOperRec.cDeleFlag = '+';

	if (nRet == APP_SUCC )
	{
		ASSERT_FAIL(PubUpdateRec (FILE_OPER, nCurOperRecNo, (const char *)&stOperRec ));	
	}
	else
	{
		ASSERT_FAIL(PubAddRec(FILE_OPER, (const char *)&stOperRec ));		
	}
	return APP_SUCC;
}

