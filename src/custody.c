/**
* @file cusody.c
* @brief 第三方存管打包/解包函数
* @version  1.0
* @author 任辉
* @date 2011-09-13
*/
#include <string.h>
#include "apiinc.h"
#include "libapiinc.h"
#include "posdef.h"
#include "appinc.h"
#include "managetxn.h"


/**
* @fn CustoyDispResp(const char *szRespCode, stSystem.szField44);
* @brief 对返回码进行解释
* @param in const char *szRespCode 返回码
* @return 返回
*/
void CustoyDispResp(const char *pszRespCode, const char * pszRestInfo)
{
	char szContent[100];
	
	memset(szContent, 0, sizeof(szContent));
	
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%2.2s-%s", pszRespCode, pszRestInfo);
	PubMsgDlg("交易失败", szContent, 3, 0);
	PubGetKeyCode(30);
	return;
}

static ST_CUSTODY_TAG s_stCustodyTag[]={	
	{1,   'A', 6	},     /**<交易类型	*/
	{2,   'A', 8	},	   /**<交易日期*/
	{3,   'A', 6	},	   /**<交易时间*/
	{4,   'A', 6	},	   /**<流水号*/
	{5,   'A', 6	},	   /**<批次号*/
	{6,   'A', 6	},	   /**<票据号*/
	{7,   'A', 8	},	   /**<终端号*/
	{8,   'A', 15	},	   /**<商户号*/
	{9,   'A', 12	},	   /**<参考号*/
	{10,   'A', 19	},	   /**<卡号*/
	{11,   'A', 37	},	   /**<二磁道*/
	{12,   'A', 104	},	   /**<三磁道*/
	{13,   'A', 11	},	   /**<手机号*/
	{14,   'A', 50	},	   /**<券编号*/
	{15,   'A', 3	},	   /**<服务点输入方式*/
	{16,   'A', 8	},	   /**<PIN*/
	{17,   'A', 20	},	   /**<短信验证码*/
	{18,   'A', 255	},	   /**<IC55*/
	{19,   'A', 255	},	   /**<IC56*/
	{20,   'A', 6	},	   /**<原交易流水号*/
	{21,   'A', 6	},	   /**<原交易批次号*/
	{22,   'A', 12	},	   /**<原交易系统参考号*/	
	{23,   'A', 8	},	   /**<原交易日期*/	
	{24,   'A', 6	},	   /**<原交易类型*/
	{25,   'A', 2	},	   /**<响应码*/
	{26,   'A', 100	},	   /**<响应信息*/
	{28,   'A', 255	},	   /**<TAG28*/
	{29,   'A', 12	},	   /**<交易金额*/
	{30,   'A', 255	},	   /**<第三方账户列表*/
	{31,   'A', 50 },	   /**<第三方账户*/
	{32,   'A', 255 },	   /**<卡券列表*/
	{33,   'A', 12	},	   /**<抵用金额*/
	{34,   'A', 36	},	   /**<结算信息*/
	{35,   'A', 50 },	   /**<订单号*/
	{36,   'A', 200 },	   /**<URL链接*/
	{37,   'A', 12	},	   /**<应付金额*/
	{80,   'A', 200 },	   /**<发券方标识类型*/
	{81,   'A', 200 },	   /**<券类型标识*/
	{90,   'A', 20 },	   /**<参数版本号*/
	{91,   'A', 16 },	   /**<主密钥*/
	{92,   'A', 16 },	   /**<PIN KEY*/
	{93,   'A', 20 },	   /**<MAC KEY*/
	{94,   'A', 40 },	   /**<商户名称*/
	{95,   'A', 255 },	   /**<备注信息*/
	{97,   'A', 100 },	   /**<终端信息*/
	{99,   'A', 8 },	   /**<MAC*/
	{999, 'A', 16	},     /**<END*/
};

static ST_CUSTODY_TLV s_stCustodyTlv[128];
static int s_nTlvIndex=0;

int CustodyTlvInit(void)
{
	memset(s_stCustodyTlv, 0, sizeof(s_stCustodyTlv));
	s_nTlvIndex = 0;
	return APP_SUCC;
}

static int AddTlvValue(ST_CUSTODY_TAG *pstTag, char *psValue, int nLen)
{
	memcpy((char *)&s_stCustodyTlv[s_nTlvIndex].stTag, pstTag, sizeof(ST_CUSTODY_TAG));
	s_stCustodyTlv[s_nTlvIndex].nLen = nLen;
	s_stCustodyTlv[s_nTlvIndex].psVale = psValue;
	s_nTlvIndex ++;
	return APP_SUCC;
}	

/**
* @fn CustodyTlvAddValue 
* @brief TLV 
* @param in int nTag,				需要添加的TLV标签值
* @param in psInValue				标签值
* @param out char *psTlvBuf		TLV输出
* @param out int *pnTlvBufLen 		TLV缓冲区长度
* @param out int *pnTlvNum			TLV标签数量
* @return li 	
*		FILE_SUCC 成功 
*		li FILE_FAIL   失败
* @author renhui
*/
int CustodyTlvAddValue(int nTag, char *pszInvalue, char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum)
{
	int i=0;
	int nInvalueLen;

	nInvalueLen = strlen(pszInvalue);
	if(nInvalueLen <= 0)
	{
		return APP_SUCC;
	}
	for (i = 0; ;i++)
	{
		if(s_stCustodyTag[i].nTag == 999)
		{
			return APP_FAIL;
		}
		else if(s_stCustodyTag[i].nTag == nTag)
		{
			break;
		}
	}
	if(nInvalueLen > s_stCustodyTag[i].nMaxLen)
	{
		nInvalueLen =  s_stCustodyTag[i].nMaxLen;
	}
	/*TAG*/
	sprintf(psTlvBuf+(*pnTlvBufLen), "%03d", nTag);
	*pnTlvBufLen+=4;

	/*LEN*/
	sprintf(psTlvBuf+(*pnTlvBufLen), "%04d", nInvalueLen);
	*pnTlvBufLen+=4;
	
	if('A'  == s_stCustodyTag[i].cType)
	{
		memcpy(psTlvBuf+(*pnTlvBufLen), pszInvalue, nInvalueLen);
		
		/*保存TLV值*/
		AddTlvValue(&s_stCustodyTag[i], psTlvBuf+(*pnTlvBufLen), nInvalueLen);
	
	//	PrintDebugData(psTlvBuf+(*pnTlvBufLen), nInvalueLen);
		*pnTlvBufLen+=nInvalueLen;
	}
	else if('B'==s_stCustodyTag[i].cType)
	{
		*pnTlvBufLen+=nInvalueLen;
		PubAscToHex((unsigned char *)pszInvalue, nInvalueLen, 0, (unsigned char *)psTlvBuf+(*pnTlvBufLen));
		/*保存TLV值*/
		AddTlvValue(&s_stCustodyTag[i], psTlvBuf+(*pnTlvBufLen), nInvalueLen);
	
	//PrintDebugData(psTlvBuf+(*pnTlvBufLen), nInvalueLen);
		*pnTlvBufLen+=(nInvalueLen+1)/2;	
	}
	else
	{
		return APP_FAIL;
	}
	(*pnTlvNum)++;

	return APP_SUCC;
}



int CustodyTlvAddValueLen(int nTag, char *pszInvalue,int nInValueLen, char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum)
{
	int i=0;
	int nInvalueLen;

	
	nInvalueLen = nInValueLen;
	if(nInvalueLen <= 0)
	{
		return APP_SUCC;
	}
	for (i = 0; ;i++)
	{
		if(s_stCustodyTag[i].nTag == 999)
		{
			return APP_FAIL;
		}
		else if(s_stCustodyTag[i].nTag == nTag)
		{
			break;
		}
	}

	if(nInvalueLen > s_stCustodyTag[i].nMaxLen)
	{
		nInvalueLen =  s_stCustodyTag[i].nMaxLen;
	}

	/*TAG*/
	sprintf(psTlvBuf+(*pnTlvBufLen), "%03d", nTag);
	*pnTlvBufLen+=4;

	/*LEN*/
	sprintf(psTlvBuf+(*pnTlvBufLen), "%04d", nInvalueLen);
	*pnTlvBufLen+=4;
	
	if('A'  == s_stCustodyTag[i].cType)
	{
		memcpy(psTlvBuf+(*pnTlvBufLen), pszInvalue, nInvalueLen);
		*pnTlvBufLen+=nInvalueLen;
		
	}
	else if('B'==s_stCustodyTag[i].cType)
	{
		*pnTlvBufLen+=nInvalueLen;
		PubAscToHex((unsigned char *)pszInvalue, nInvalueLen, 0, (unsigned char *)psTlvBuf+(*pnTlvBufLen));
		*pnTlvBufLen+=(nInvalueLen+1)/2;
		
	}
	else
	{
		return APP_FAIL;
	}
	(*pnTlvNum)++;
	return APP_SUCC;
}

/**
* @fn CustodyPack 
* @brief 第三方存管打包函数
* @param in char *psPack			TLV标签组
* @param in int *pnLen				TLV标签组长度
* @param in int nTagSum			TLV标签数量
* @param out char *psPack			打包后数据
* @param out int *pnLen 			数据长度
* @return li 	
*		FILE_SUCC 成功 
*		li FILE_FAIL   失败
* @author renhui
*/

int CustodyPack(char *psPack, int *pnLen, int nTagSum)
{
	char sTmp[CUSTODY_MAX_PACK_SIZE];

	memset(sTmp, 0, sizeof(sTmp));
	
	sprintf(sTmp, "%03d%05d", nTagSum, *pnLen);
	memcpy(sTmp+8, psPack, *pnLen);
	*pnLen += 8;
	memcpy(psPack, sTmp, *pnLen);

	//DebugAllPrint();

	return APP_SUCC;
}

/**
* @fn CustodyUpPack 
* @brief 第三方存管解包包函数
* @param in char *psPack			TLV标签组
* @param in int *pnLen				TLV标签组长度
* @param in int nTagSum			TLV标签数量
* @param out char *psPack			打包后数据
* @param out int *pnLen 			数据长度
* @return li 	
*		FILE_SUCC 成功 
*		li FILE_FAIL   失败
* @author renhui
*/
int CustodyUpPack(char *psPack, int *pnLen, int *pnTagSum)
{
	char szTmp[5+1];

	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, psPack, 3);
	*pnTagSum = atoi(szTmp);
	
	memset(szTmp, 0, sizeof(szTmp));
	memcpy(szTmp, psPack+3, 5);
	*pnLen = atoi(szTmp);

	memcpy(psPack, psPack+8, *pnLen);
	memset(psPack+*pnLen, 0, 8);
	
	return APP_SUCC;
}

int ProGetTlvDate(ST_CUSTODY_TAG *pstTag, int nTag)
{
	int i=0;
	
	for (i = 0; ;i++)
	{
		if(s_stCustodyTag[i].nTag == 999)
		{

			/*找不到TAG*/
			pstTag->nTag = nTag;
			pstTag->nMaxLen = 999;
			pstTag->cType = 'A';
			return APP_SUCC;
			//return APP_FAIL;
		}
		else if(s_stCustodyTag[i].nTag == nTag)
		{
			break;
		}
	}
	memcpy((char *)pstTag, (char *)&s_stCustodyTag[i], sizeof(ST_CUSTODY_TAG));
	return APP_SUCC;
}

/*解析TLV*/
int CustodyParseTlv(char *psTlvStr, int nTlvStrLen,  ST_CUSTODY_TLV *pstTlv)
{
	char szTag[3+1];
	char szValueLen[4+1];
	int i=0;
	int nOffset=0;
	
	if (psTlvStr == NULL || pstTlv == NULL || nTlvStrLen == 0) 
	{
		return APP_FAIL;
	}
	nOffset =0;
	i=0;
	for(i=0, nOffset=0; nOffset<nTlvStrLen; i++)
	{
		memset(szTag, 0, sizeof(szTag));
		memcpy(szTag, psTlvStr+nOffset, 3);		/*tag*/
		nOffset += 4;
	
		ASSERT_FAIL(ProGetTlvDate(&pstTlv[i].stTag, atoi(szTag)));
		memset(szValueLen, 0, sizeof(szValueLen));	/*len*/
		memcpy(szValueLen, psTlvStr+nOffset, 4);
		
		pstTlv[i].nLen = atoi(szValueLen);
		nOffset += 4;
		pstTlv[i].psVale = (psTlvStr+nOffset);
		if(pstTlv[i].stTag.cType == 'B')
		{
			nOffset += (pstTlv[i].nLen+1)/2;
		}
		else
		{
			nOffset += pstTlv[i].nLen;
		}
		
	}
	return APP_SUCC;	
}

int CustodyTlvGetValue(int nTag, ST_CUSTODY_TLV *pstTlv, char *psValue, int *pnLen)
{
	int i=0;

	*pnLen = 0;
	i=0;
	for(i=0; ;i++)
	{
		if(pstTlv[i].stTag.nTag == 0)
		{
			break;
		}
		if(pstTlv[i].stTag.nTag != nTag)
		{
			continue;
		}
		*pnLen = pstTlv[i].nLen;
		if(pstTlv[i].stTag.cType == 'B')
		{
			PubHexToAsc((unsigned char *)pstTlv[i].psVale, *pnLen, 0, (unsigned char * )psValue);
		}
		else
		{
			memcpy(psValue, pstTlv[i].psVale, *pnLen);
		}

		return APP_SUCC;
	}
	return APP_SUCC;
}

int CustodyGetField(int nTag, char *psValue, int *pnLen)
{
	return CustodyTlvGetValue(nTag, s_stCustodyTlv, psValue, pnLen);
}

int CustodyChkRespon(STSYSTEM *pstSystem, char * psPack, int nLen)
{
	struct
	{
		char szTransCode[6+1];		/**<1 交易类型*/
		char szAmount[12+1];			/**<4 金额*/
		char szTrace[6+1];			/**<11 POS流水号*/
		char szPosID[8+1];			/**<41 终端号 */
		char szShopID[15+1];			/**<42 商户号 */
		char szCurrencyCode[3+1];	/**<49 交易货币代码*/
	}stMustChkResp;
	int nFieldLen=0;
	char *pszTitle = "解包";
	
	memset(pstSystem->szMac, 0, sizeof(pstSystem->szMac));
	memset(s_stCustodyTlv, 0, sizeof(s_stCustodyTlv));
	memset((char *)&stMustChkResp, 0, sizeof(stMustChkResp));
	ASSERT_FAIL(CustodyParseTlv(psPack, nLen,  s_stCustodyTlv));

	/*交易类型*/
	ASSERT_FAIL(CustodyTlvGetValue(1, s_stCustodyTlv, stMustChkResp.szTransCode, &nFieldLen));
	if (memcmp(stMustChkResp.szTransCode, pstSystem->szTransCode, 6) == 0)
	{
		PubMsgDlg(pszTitle, "返回交易类型不符", 3, 10);
		return APP_FAIL;
	}	
	
	/*交易时间*/
	ASSERT_FAIL(CustodyTlvGetValue(2, s_stCustodyTlv, pstSystem->szDate, &nFieldLen));

	ASSERT_FAIL(CustodyTlvGetValue(3, s_stCustodyTlv, pstSystem->szTime, &nFieldLen));
	/*交易流水号*/
	ASSERT_FAIL(CustodyTlvGetValue(4, s_stCustodyTlv, stMustChkResp.szTrace, &nFieldLen));
	if (memcmp(stMustChkResp.szTrace, pstSystem->szTrace, 6) != 0)
	{
		PubMsgDlg(pszTitle, "返回流水号不符", 3, 10);
		return APP_FAIL;
	}	

	ASSERT_FAIL(CustodyTlvGetValue(5, s_stCustodyTlv, pstSystem->szBatchNo, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(7, s_stCustodyTlv, stMustChkResp.szPosID, &nFieldLen));
	if (memcmp(stMustChkResp.szPosID, pstSystem->szPosID, 8) != 0)
	{
		PubMsgDlg(pszTitle, "返回终端号不符", 3, 10);
		return APP_FAIL;
	}
	ASSERT_FAIL(CustodyTlvGetValue(8, s_stCustodyTlv, stMustChkResp.szShopID, &nFieldLen));
	if (memcmp(stMustChkResp.szShopID, pstSystem->szShopID, 15) != 0)
	{
		PubMsgDlg(pszTitle, "返回商户号不符", 3, 10);
		return APP_FAIL;
	}
	ASSERT_FAIL(CustodyTlvGetValue(9, s_stCustodyTlv, pstSystem->szRefnum, &nFieldLen));
	/*响应码*/
	ASSERT_FAIL(CustodyTlvGetValue(25, s_stCustodyTlv, pstSystem->szResponse, &nFieldLen));
	/*响应信息*/
	ASSERT_FAIL(CustodyTlvGetValue(26, s_stCustodyTlv, pstSystem->szRespInfo, &nFieldLen));

	ASSERT_FAIL(CustodyTlvGetValue(32, s_stCustodyTlv, pstSystem->szTag32, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(33, s_stCustodyTlv, pstSystem->szRealAmount, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(37, s_stCustodyTlv, pstSystem->szPayableAmount, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(95, s_stCustodyTlv, pstSystem->szAdvertisement, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(99, s_stCustodyTlv, pstSystem->szMac, &nFieldLen));
	return APP_SUCC;
}





/**
* @fn CalcMac(char * psMac, int nMode, const char *psData, int nDataLen)
* @brief 计算MAC值
* @param in int nMode
* @param in const char *psData
* @param in int nDataLen
* @param out  char * psMac
* @return 无
*/
static int CalcMac(char * psMac,const char *psData, int nDataLen)
{
	int nRet, nGroupNo = 0;
	

	GetVarMainKeyNo(&nGroupNo);
	PubSetCurrentMainKeyIndex(nGroupNo);
	if (YES == GetVarIsPinpad())
	{
		nRet = PubActiveKey (DESMODE_3DES == GetVarEncyptMode() ? DESMODE_3DES : DESMODE_DES, nGroupNo,KEY_TYPE_MAC);
		if (nRet != APP_SUCC)
		{
			PubMsgDlg("MAC计算", "密码键盘激活失败", 3, 10);
			return nRet;
		}
	}
	nRet = PubCalcMac(MAC_TYPE_X919, psData, nDataLen, psMac);
	if (nRet != APP_SUCC)
	{
		return nRet;
	}
	return APP_SUCC;
}

static int ProGetMacBuffer(char *pszMacBuffer, int *pnMacBufferLen)
{
	char szTmp[512+1];
	int nValueLen=0;
	int nLen=0;
	
	/*
	001	交易类型
	002	交易日期
	003	交易时间
	004	流水号
	005	批次号
	007	终端号
	008	商户号
	009	系统参考号 
	010	卡号
	013	手机号
	014	卡券码
	025	响应码
	029	交易金额
	*/


	memset(szTmp, 0, sizeof(szTmp));		
	nLen = 0;
	CustodyTlvGetValue(1, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(2, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(3, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(4, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(5, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(7, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(8, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(9, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(10, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(13, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(14, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(25, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	CustodyTlvGetValue(29, s_stCustodyTlv, szTmp+nLen, &nValueLen);
	nLen += nValueLen;
	memcpy(pszMacBuffer,szTmp,  nLen);
	*pnMacBufferLen = nLen;

	return APP_SUCC;
	
}

int CustodyAddMac(char *psTlvBuf, int *pnTlvBufLen, int *pnTlvNum)
{
	char szMacBuffer[512+1];
	int nMacBufferLen=0;
	char szMac[16+1];

	memset(szMacBuffer, 0, sizeof(szMacBuffer));
	ProGetMacBuffer(szMacBuffer, &nMacBufferLen);

	
	memset(szMac, 0,sizeof(szMac));	
	ASSERT_FAIL(CalcMac(szMac, szMacBuffer, nMacBufferLen));
	ASSERT_FAIL(CustodyTlvAddValueLen(99, szMac,8,  psTlvBuf, pnTlvBufLen, pnTlvNum));
	return APP_SUCC;
}

int CustodyChkMac(char *pszCheckMac)
{

	char szMacBuffer[512+1];
	int nMacBufferLen=0;
	char szMac[16+1];

	memset(szMacBuffer, 0, sizeof(szMacBuffer));
	ProGetMacBuffer(szMacBuffer, &nMacBufferLen);

	memset(szMac, 0,sizeof(szMac));	
	ASSERT_FAIL(CalcMac(szMac, szMacBuffer, nMacBufferLen));

	if(memcmp(pszCheckMac, szMac, 8) == 0)
	{
		return APP_SUCC;
	}
	else
	{
		return APP_FAIL;
	}
}



/**
* @brief 交易完成
* @detail [检查操作员角色],[检查交易开关],[检查签到状态][处理限制]
* @param [in] pszTitle  标题
* @param [in] cOperFlag  标志
* @param [in] cTransType  交易类型
* @param [in] cAttr  交易属性
* @return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 陈武
* @date 2014-9-14
*/
int TradeInit(char* pszTitle, ENM_TRADEINIT cOperFlag, const char cTransType, const char cAttr, const char cClearScr)
{
	
	ASSERT_QUIT(ChkOperRole());

	if(cOperFlag & TDI_CHECKOPER)
	{
		if(GetVarIsAdminPin()==YES)
		{
			ASSERT_QUIT(CheckOper(pszTitle, MANAGEROPER));
		}
	}
	
	if (YES == cClearScr)
	{
		PubClearAll();
		PubDisplayTitle(pszTitle);
	}
	
	/**
	* 检查是否签到
	*/
	ASSERT_QUIT(ChkLoginStatus());

	/**
	* 检查POS限制
	*/
	ASSERT_QUIT(DealPosLimit());

	return APP_SUCC;
}


