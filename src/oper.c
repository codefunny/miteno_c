/**
* @file oper.c
* @brief 操作员处理模块
* @version  1.0
* @author 薛震
* @date 2007-01-26
*/
#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

#define MAXOPERNUM		22				/**< 系统允许的最大操作员数*/				
#define ADMINPWD		"12345678"		/**< 系统管理员初始密码*/
#define MANAGERPWD		"123456"			/**< 主管初始密码*/
#define NORMALOPERPWD	"0000"			/**< 普通操作员初始密码*/

#define MANAGERPWDLEN		6				/**< 主管操作员密码长度*/
#define NORMALPWDLEN		4				/**< 普通操作员密码长度*/			
#define ADMINOPERPWDLEN	8				/**< 系统管理员密码长度*/

#define LOGINERRCOUNT		3				/**< 允许操作员脱机签到的错误次数*/

#define FILE_OPER		APP_NAME"OPER"

typedef struct
{
	char szOperNo[OPERCODELEN+1];
	char cOperRole;
	char cOperPwdLen;
	char szPasswd[12+1];
	char cDeleFlag;
	char cRfu;/**<预留*/
} STOPER_RECORD;

static char gszCurrentOperNo[OPERCODELEN+1];
static char gszCurrentOperPasswd[ADMINOPERPWDLEN+1];
static char gszOperLimit[OPERCODELEN+1];
static int gnCurrentOperRole;

/**
* 以下为本模块内部使用的函数
*/
static void AppendOperRec(const char *, const char *);
static int FindOper(int , STOPER_RECORD *, int * );
static int CountOper(int *);

/**
* @brief 初始化操作员记录
*
*	初始化系统操作员数据，生成一个系统管理员，一个主管，
* 五个普通操作员编号及相应的初始密码
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
	* 创建操作员文件，并进行初始化
	*/
	memset(&stOperFile, 0, sizeof(STRECFILE));
	strcpy( stOperFile.szFileName, FILE_OPER );
	stOperFile.cIsIndex = FILE_CREATEINDEX;						/**< 创建索引文件*/
	stOperFile.unMaxOneRecLen = sizeof(STOPER_RECORD);
	stOperFile.unIndex1Start =(uint)( (char *)(stOperRec.szOperNo) - (char *)&stOperRec)+1;
	stOperFile.unIndex1Len =  OPERCODELEN;
	stOperFile.unIndex2Start =  (uint)( (char *)(&stOperRec.cDeleFlag) - (char *)&stOperRec)+1;
	stOperFile.unIndex2Len = sizeof(stOperRec.cDeleFlag);
	ASSERT(PubCreatRecFile(&stOperFile));	

	/**
	* 初始化 系统管理员记录，编号99
	*/

	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 99 );
	stOperRec.cOperRole = ADMINOPER;
	stOperRec.cOperPwdLen = ADMINOPERPWDLEN;
	//sprintf( stOperRec.szPasswd, "%-12.12s", ADMINPWD );
	strcpy(stOperRec.szPasswd, ADMINPWD );
	stOperRec.cDeleFlag = '+';
	PubAddRec(FILE_OPER, (const char *)&stOperRec );

	/**
	* 初始化 主管操作员记录，编号00
	*/

	sprintf( stOperRec.szOperNo, "%0*d", OPERCODELEN, 0 );
	stOperRec.cOperRole = MANAGEROPER;
	stOperRec.cOperPwdLen = MANAGERPWDLEN;
	//sprintf( stOperRec.szPasswd, "%-12.12s", MANAGERPWD );
	strcpy(stOperRec.szPasswd, MANAGERPWD );
	stOperRec.cDeleFlag = '+';
	PubAddRec(FILE_OPER, (const char *)&stOperRec );
	
	/**
	* 初始化五个普通操作员记录,编号01-05
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
* @brief 统计系统操作员数
* @param in int *pnOperCount  操作员个数
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
* @brief 增加操作员
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
	* 统计系统总操作员数
	*/
	ASSERT_FAIL(CountOper(&nOperNum));

	/**
	* 判断操作员数是否超最大限制数
	*/
	if (nOperNum >= MAXOPERNUM)
	{
      	PubClearAll();
 		PubDisplayGen( 2, "操作员人数已满" );
 		PubDisplayGen( 3, "请先删除一个" );
 		PubUpdateWindow();
		PubGetKeyCode(5);
		return APP_FAIL;
	}
	
	/**
	* 输入需要增加的操作员编号
	*/
	memset(szOperNo, 0, sizeof(szOperNo));
	nRet = PubInputDlg("增加操作员", "请输入操作员号：", szOperNo, &nLen1, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}

	if( nLen1 != OPERCODELEN )
	{
		PubMsgDlg("增加操作员", "号码位数不符合", 0, 5 );
		return APP_FAIL;
	}
	
	/**
	* 判断操作员编号是否存在
	*/
	if (PubFindRec (FILE_OPER, szOperNo, "+", 1, (char *)&stOperRec, &nCurOperRecNo) == APP_SUCC )
	{
		PubMsgDlg("增加操作员", "该操作员已存在!", 0, 5 );
		return APP_FAIL;
	}
	
	/**
	* 输入两次密码并进行比较
	*/
	sprintf(szTempStr, "操作员编号:%2.2s",szOperNo);
	memset(szPassW1, 0, sizeof(szPassW1));
	nRet = PubInputDlg(szTempStr, "请输操作员密码:", szPassW1, &nLen1, 0, NORMALPWDLEN, 0, INPUT_MODE_PASSWD);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}

	/**
	* 判断密码位数的合法性
	*/
	if (nLen1 == NORMALPWDLEN || nLen1 == 0)
	{
		;
	}
	else
	{
		PubMsgDlg("增加操作员", "密码位数不符!", 0, 5 );
		return APP_FAIL;
	}

	memset(szPassW2, 0, sizeof(szPassW2));
	nRet = PubInputDlg(szTempStr, "请再输一次密码:", szPassW2, &nLen2, 0, NORMALPWDLEN, 0, INPUT_MODE_PASSWD);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}


	if ( nLen1 != nLen2 || memcmp(szPassW1,szPassW2,NORMALPWDLEN) !=0 )
	{
		PubMsgDlg("增加操作员", "两次输入操作员密码不一致", 0, 3 );
		return APP_FAIL;
	}

	if( nLen2 == 0 )
	{
		strcpy(szPassW1, NORMALOPERPWD);	
	}
	AppendOperRec(szOperNo, szPassW1);
	PubMsgDlg("增加操作员", "操作员增加成功!", 0, 1);
	return APP_SUCC;
}

/**
* @brief  根据给定的操作员编号和密码增加操作员记录
* @param in char *pszOperNo 操作员编号
* @param out char *pszPasswd 操作员密码
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
static void AppendOperRec(const char *pszOperNo, const char *pszPasswd)
{
	STOPER_RECORD stOperRec;
	int nCurOperRecNo, nRet;

	/**
	* 判断操作员编号是否在记录文件中使用过
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
* @brief  删除操作员
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
	* 统计系统总操作员数
	*/
	ASSERT_FAIL(CountOper( &nOperNum ));
	if ( nOperNum == 2 )
	{
		PubMsgDlg("删除操作员", "对不起，无操作员", 0, 3);
		return APP_QUIT;
	}

	/**
	* 输入需要删的操作员编号
	*/
	memset(szOperNo, 0, sizeof(szOperNo));
	nRet = PubInputDlg("删除操作员", "请输入操作员号：", szOperNo, &nLen, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
	if (nRet != APP_SUCC )
	{
		return APP_QUIT;
	}
	if( nLen != OPERCODELEN )
	{
		PubMsgDlg("删除操作员", "号码位数不符合", 0, 5);
		return APP_FAIL;
	}
	sprintf(szTmpOperNo, "%0*d", OPERCODELEN, 99 );
	if (!memcmp(szOperNo,szTmpOperNo,OPERCODELEN))
	{
		PubMsgDlg("删除操作员", "系统管理员不能删除", 0, 3 );
		return APP_QUIT;
	}
	if( !memcmp( szOperNo, gszCurrentOperNo, OPERCODELEN ) )
	{
		PubMsgDlg("删除操作员", "当前操作员不能删除", 0, 3 );
		return APP_QUIT;			
	}

	if( gszOperLimit[0] != 0 && memcmp( szOperNo, gszOperLimit, OPERCODELEN ) == 0 )
	{
		PubMsgDlg("删除操作员", "操作员有流水未结算，不能删除", 0, 3 );
		return APP_QUIT;
	}

	if ( APP_SUCC==IsOperHasWater(szOperNo) )
	{
		PubMsgDlg("删除操作员", "操作员有流水未结算，不能删除", 0, 3 );
		return APP_QUIT;
	}
	
	if( PubFindRec (FILE_OPER, szOperNo, "+", 1, (char *)&stOperRec, &nCurOperRecNo) != APP_SUCC )
	{
		PubMsgDlg("删除操作员", "无此操作员", 0, 3 );
		return APP_FAIL;
	}
	else
	{
		if ( stOperRec.cOperRole == MANAGEROPER || stOperRec.cOperRole == ADMINOPER )
		{
			PubMsgDlg("删除操作员", "主管操作员不能删除", 0, 3 );
			return APP_QUIT;
		}
		stOperRec.cDeleFlag = '-';
		PubUpdateRec (FILE_OPER, nCurOperRecNo, (const char *)&stOperRec );	
		PubMsgDlg("删除操作员", "操作员删除成功", 0, 3 );
	}
	return APP_SUCC;
}

/**
* @brief 查看现有的普通操作员信息
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
		PubMsgDlg("查询操作员", "对不起，无操作员", 0, 3 );
		return;
	}

	PubUpDownMsgDlg("查询操作员", szDispStr, YES, 60, NULL);
	return;
}

/**
* @brief 更改当前操作员密码
* @param in nRole 操作员角色
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
			strcpy( szTmpDispStr1, "修改主管密码" );
			strcpy( szTmpDispStr2, "请输入原密码:" );	
			nPasswdLen = MANAGERPWDLEN;
			break;
		case NORMALOPER:
			sprintf( szTmpDispStr1, "操作员编号:%-4.*s", OPERCODELEN, gszCurrentOperNo );
			if( ( nRet = PubConfirmDlg("修改操作员密码", szTmpDispStr1, 0, 50) ) !=APP_SUCC )
			{
				return nRet;
			}
			strcpy( szTmpDispStr1, "修改操作员密码" );
			strcpy( szTmpDispStr2, "请输入原密码:" );
			nPasswdLen = NORMALPWDLEN;
			break;
		case ADMINOPER:
			strcpy( szTmpDispStr1, "修改管理员密码" );			
			strcpy( szTmpDispStr2, "请输管理员密码:" );			
			nPasswdLen = ADMINOPERPWDLEN;
			break;
		default:
			return APP_FAIL;
		}
		/**
		 * 输入原密码并校验
		 */
		memset(szOperPasswd, 0, sizeof(szOperPasswd));		
		nRet = PubInputDlg(szTmpDispStr1, szTmpDispStr2, szOperPasswd, &nLen, stOperRec.cOperPwdLen, stOperRec.cOperPwdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}
		if( memcmp(szOperPasswd, stOperRec.szPasswd,  stOperRec.cOperPwdLen) )
		{
			PubMsgDlg(szTmpDispStr1, "原密码不符", 0, 5 ) ;
			continue;
		}
		memset(szOperPasswd, 0, sizeof(szOperPasswd));
		nRet = PubInputDlg(szTmpDispStr1, "请输入新密码:", szOperPasswd, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}
		memset(szPasswdStr, 0, sizeof(szPasswdStr));
		nRet = PubInputDlg(szTmpDispStr1, "请确认新密码:", szPasswdStr, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD);
		if( nRet != APP_SUCC )
		{
			return nRet;
		}

		if(  memcmp(szOperPasswd, szPasswdStr, nPasswdLen) )
		{
			PubMsgDlg(szTmpDispStr1, "两次密码输入不同", 0, 5 ) ;
			continue;
		}
		else
		{
			strcpy( stOperRec.szPasswd,  szOperPasswd );
			stOperRec.cOperPwdLen = nPasswdLen;
			nRet = PubUpdateRec (FILE_OPER, nOperRecNum, (const char *)&stOperRec );	
			if(nRet == APP_SUCC)
			{
				PubMsgDlg(szTmpDispStr1, "密码修改成功", 0, 5 ) ;
			}
			else
			{
				PubMsgDlg(szTmpDispStr1, "密码修改失败", 0, 5 ) ;
			}
			return nRet;
		}

	}
}

/**
* @brief 根据提供的操作员的角色，得到操作员的信息。
* @param in nRole 操作员角色
* @param out STOPER_RECORD *pstOperRec 操作员记录信息
* @param out int *pnOperRecNum  操作员记录号
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
* @brief POS锁定以及解锁
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
		nRet = PubInputDlg("终端已锁定", "请输入操作员号", szOper, &nLen, OPERCODELEN, OPERCODELEN, 0, INPUT_MODE_NUMBER);
		if (nRet != APP_SUCC)
		{
			continue;
		}

		nRet = PubFindRec(FILE_OPER, szOper, "+", 1, (char *)&stOperRec, &nOperRecNum);
		if (nRet !=APP_SUCC )
		{
			PubMsgDlg("终端已锁定", "操作员号码不符", 0, 3 ) ;
			continue;
		}

		if( (stOperRec.cOperRole !=MANAGEROPER)  && ( memcmp( szOper, gszCurrentOperNo, OPERCODELEN )))
		{
			PubMsgDlg("终端已锁定", "操作员号码不符", 0, 3 ) ;
			continue;				
		}

		if( CheckOper("终端已锁定", stOperRec.cOperRole) == APP_SUCC )
		{
			if ((stOperRec.cOperRole == MANAGEROPER) && (memcmp( szOper,gszCurrentOperNo, OPERCODELEN ) != 0))
			{
				LockOperChgFlag = 1;
				strcpy(szOldOperNo, gszCurrentOperNo);
				strcpy(szOldOperPasswd, gszCurrentOperPasswd);
				nOldOperRole = gnCurrentOperRole;
				
				strcpy( gszCurrentOperNo, szOper );
				memcpy(gszCurrentOperPasswd, stOperRec.szPasswd, ADMINOPERPWDLEN);
				//strcpy( gszCurrentOperPasswd, stOperRec.szPasswd ); 此行造成程序故障，
				//因此故障，同时调整 InitOper中的处理
				gnCurrentOperRole = MANAGEROPER;
			}
			else
			{
				LockOperChgFlag = 0;
			}
			PubMsgDlg("终端已锁定", "解锁成功", 0, 1); 
			return APP_SUCC;
		}
		else
		{
			continue;
		}
	}
}

/**
* @brief 操作员签到
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
			PubMsgDlg("签到", "签到失败！", 0, 3 ) ;
			return APP_FAIL;
		}
		memset(szOper, 0, sizeof(szOper));
		nRet = PubInputDlg("签到", "请输入操作员号", szOper, &nLen, 0, OPERCODELEN, 0, INPUT_MODE_NUMBER);
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
			PubMsgDlg("签到", "无此操作员", 3, 3);
			nErrCount++;
			continue;
		}
		
		memset(szOperPasswd, 0, sizeof(szOperPasswd));
		if (PubInputDlg("签到", "请输入密码", szOperPasswd, &nLen, nPasswdLen, nPasswdLen, 0, INPUT_MODE_PASSWD) != APP_SUCC)
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
			PubMsgDlg("签到", "密码错误", 3, 3);
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
* @brief 验证操作员
* @param in char *pszTitle 验证提示信息
* @param in nRole 操作员角色
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
		strcpy( szDispStr1, "请输入主管密码" );
		strcpy( szDispStr2, "主管密码错" );		
		nPasswdLen = MANAGERPWDLEN;
		break;
	case NORMALOPER:
		strcpy( szDispStr1, "请输入操作员密码" );
		strcpy( szDispStr2, "操作员密码错" );
		nPasswdLen = NORMALPWDLEN;
		break;
	case ADMINOPER:
		nPasswdLen = ADMINOPERPWDLEN;
		strcpy( szDispStr1, "请输系统管理密码" );
		strcpy( szDispStr2, "系统管理员密码错" );
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
* @brief 获得当前操作员的编号和密码
* @return
* @param out char *pszOperNo 当前操作员编号
* @param out char *pszOperPasswd 当前操作员密码
* @int *pnRole 当前操作员角色
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
* @brief 设置当前操作员的编号和密码
* @return
* @param out char *pszOperNo 当前操作员编号
* @param out char *pszOperPasswd 当前操作员密码
* @int *pnRole 当前操作员角色
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
* @brief 限制操作员的登录
* @return
* @param in char *pszOper 允许登录的操作员编号
*            如果pszOper为NULL或者""则允许POS的任意操作员进行登录操作
*            否则POS机将只能以指定的操作员号进行登录
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
* @brief 操作员管理,菜单函数
* @parm 无
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
			nRet = ProSelectList("1.主管改密|2.增加|3.删除|4.查询|5.操作员改密", "柜员管理", 0xFF, &nSelect);
		}
		else
		{
			nRet = ProSelectList("1.主管改密|2.增加|3.删除|4.查询", "柜员管理", 0xFF, &nSelect);
		}
		if (nRet==APP_QUIT)
		{
			if(gnCurrentOperRole == NORMALOPER)
			{
				return APP_SUCC;
			}
			else
			{
				if (APP_SUCC == PubConfirmDlg("柜员管理", "退出管理界面", 0, 30))
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
			if (CheckOper("增加操作员",MANAGEROPER)==APP_SUCC)
			{
				AddOper();
			}
			break;
		case 3:
			if (CheckOper("删除操作员",MANAGEROPER)==APP_SUCC)
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
* @brief 根据Ini文件提供的系统管理员密码进行密码文件信息的更新
* @parm in char *szPasswd 	系统管理员密码 
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

