/**
* @file cusody.c
* @brief ��������ܴ��/�������
* @version  1.0
* @author �λ�
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
* @brief �Է�������н���
* @param in const char *szRespCode ������
* @return ����
*/
void CustoyDispResp(const char *pszRespCode, const char * pszRestInfo)
{
	char szContent[100];
	
	memset(szContent, 0, sizeof(szContent));
	
	PubGetStrFormat(DISPLAY_ALIGN_BIGFONT, szContent, "%2.2s-%s", pszRespCode, pszRestInfo);
	PubMsgDlg("����ʧ��", szContent, 3, 0);
	PubGetKeyCode(30);
	return;
}

static ST_CUSTODY_TAG s_stCustodyTag[]={	
	{1,   'A', 6	},     /**<��������	*/
	{2,   'A', 8	},	   /**<��������*/
	{3,   'A', 6	},	   /**<����ʱ��*/
	{4,   'A', 6	},	   /**<��ˮ��*/
	{5,   'A', 6	},	   /**<���κ�*/
	{6,   'A', 6	},	   /**<Ʊ�ݺ�*/
	{7,   'A', 8	},	   /**<�ն˺�*/
	{8,   'A', 15	},	   /**<�̻���*/
	{9,   'A', 12	},	   /**<�ο���*/
	{10,   'A', 19	},	   /**<����*/
	{11,   'A', 37	},	   /**<���ŵ�*/
	{12,   'A', 104	},	   /**<���ŵ�*/
	{13,   'A', 11	},	   /**<�ֻ���*/
	{14,   'A', 50	},	   /**<ȯ���*/
	{15,   'A', 3	},	   /**<��������뷽ʽ*/
	{16,   'A', 8	},	   /**<PIN*/
	{17,   'A', 20	},	   /**<������֤��*/
	{18,   'A', 255	},	   /**<IC55*/
	{19,   'A', 255	},	   /**<IC56*/
	{20,   'A', 6	},	   /**<ԭ������ˮ��*/
	{21,   'A', 6	},	   /**<ԭ�������κ�*/
	{22,   'A', 12	},	   /**<ԭ����ϵͳ�ο���*/	
	{23,   'A', 8	},	   /**<ԭ��������*/	
	{24,   'A', 6	},	   /**<ԭ��������*/
	{25,   'A', 2	},	   /**<��Ӧ��*/
	{26,   'A', 100	},	   /**<��Ӧ��Ϣ*/
	{28,   'A', 255	},	   /**<TAG28*/
	{29,   'A', 12	},	   /**<���׽��*/
	{30,   'A', 255	},	   /**<�������˻��б�*/
	{31,   'A', 50 },	   /**<�������˻�*/
	{32,   'A', 255 },	   /**<��ȯ�б�*/
	{33,   'A', 12	},	   /**<���ý��*/
	{34,   'A', 36	},	   /**<������Ϣ*/
	{35,   'A', 50 },	   /**<������*/
	{36,   'A', 200 },	   /**<URL����*/
	{37,   'A', 12	},	   /**<Ӧ�����*/
	{80,   'A', 200 },	   /**<��ȯ����ʶ����*/
	{81,   'A', 200 },	   /**<ȯ���ͱ�ʶ*/
	{90,   'A', 20 },	   /**<�����汾��*/
	{91,   'A', 16 },	   /**<����Կ*/
	{92,   'A', 16 },	   /**<PIN KEY*/
	{93,   'A', 20 },	   /**<MAC KEY*/
	{94,   'A', 40 },	   /**<�̻�����*/
	{95,   'A', 255 },	   /**<��ע��Ϣ*/
	{97,   'A', 100 },	   /**<�ն���Ϣ*/
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
* @param in int nTag,				��Ҫ��ӵ�TLV��ǩֵ
* @param in psInValue				��ǩֵ
* @param out char *psTlvBuf		TLV���
* @param out int *pnTlvBufLen 		TLV����������
* @param out int *pnTlvNum			TLV��ǩ����
* @return li 	
*		FILE_SUCC �ɹ� 
*		li FILE_FAIL   ʧ��
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
		
		/*����TLVֵ*/
		AddTlvValue(&s_stCustodyTag[i], psTlvBuf+(*pnTlvBufLen), nInvalueLen);
	
	//	PrintDebugData(psTlvBuf+(*pnTlvBufLen), nInvalueLen);
		*pnTlvBufLen+=nInvalueLen;
	}
	else if('B'==s_stCustodyTag[i].cType)
	{
		*pnTlvBufLen+=nInvalueLen;
		PubAscToHex((unsigned char *)pszInvalue, nInvalueLen, 0, (unsigned char *)psTlvBuf+(*pnTlvBufLen));
		/*����TLVֵ*/
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
* @brief ��������ܴ������
* @param in char *psPack			TLV��ǩ��
* @param in int *pnLen				TLV��ǩ�鳤��
* @param in int nTagSum			TLV��ǩ����
* @param out char *psPack			���������
* @param out int *pnLen 			���ݳ���
* @return li 	
*		FILE_SUCC �ɹ� 
*		li FILE_FAIL   ʧ��
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
* @brief ��������ܽ��������
* @param in char *psPack			TLV��ǩ��
* @param in int *pnLen				TLV��ǩ�鳤��
* @param in int nTagSum			TLV��ǩ����
* @param out char *psPack			���������
* @param out int *pnLen 			���ݳ���
* @return li 	
*		FILE_SUCC �ɹ� 
*		li FILE_FAIL   ʧ��
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

			/*�Ҳ���TAG*/
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

/*����TLV*/
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
		char szTransCode[6+1];		/**<1 ��������*/
		char szAmount[12+1];			/**<4 ���*/
		char szTrace[6+1];			/**<11 POS��ˮ��*/
		char szPosID[8+1];			/**<41 �ն˺� */
		char szShopID[15+1];			/**<42 �̻��� */
		char szCurrencyCode[3+1];	/**<49 ���׻��Ҵ���*/
	}stMustChkResp;
	int nFieldLen=0;
	char *pszTitle = "���";
	
	memset(pstSystem->szMac, 0, sizeof(pstSystem->szMac));
	memset(s_stCustodyTlv, 0, sizeof(s_stCustodyTlv));
	memset((char *)&stMustChkResp, 0, sizeof(stMustChkResp));
	ASSERT_FAIL(CustodyParseTlv(psPack, nLen,  s_stCustodyTlv));

	/*��������*/
	ASSERT_FAIL(CustodyTlvGetValue(1, s_stCustodyTlv, stMustChkResp.szTransCode, &nFieldLen));
	if (memcmp(stMustChkResp.szTransCode, pstSystem->szTransCode, 6) == 0)
	{
		PubMsgDlg(pszTitle, "���ؽ������Ͳ���", 3, 10);
		return APP_FAIL;
	}	
	
	/*����ʱ��*/
	ASSERT_FAIL(CustodyTlvGetValue(2, s_stCustodyTlv, pstSystem->szDate, &nFieldLen));

	ASSERT_FAIL(CustodyTlvGetValue(3, s_stCustodyTlv, pstSystem->szTime, &nFieldLen));
	/*������ˮ��*/
	ASSERT_FAIL(CustodyTlvGetValue(4, s_stCustodyTlv, stMustChkResp.szTrace, &nFieldLen));
	if (memcmp(stMustChkResp.szTrace, pstSystem->szTrace, 6) != 0)
	{
		PubMsgDlg(pszTitle, "������ˮ�Ų���", 3, 10);
		return APP_FAIL;
	}	

	ASSERT_FAIL(CustodyTlvGetValue(5, s_stCustodyTlv, pstSystem->szBatchNo, &nFieldLen));
	ASSERT_FAIL(CustodyTlvGetValue(7, s_stCustodyTlv, stMustChkResp.szPosID, &nFieldLen));
	if (memcmp(stMustChkResp.szPosID, pstSystem->szPosID, 8) != 0)
	{
		PubMsgDlg(pszTitle, "�����ն˺Ų���", 3, 10);
		return APP_FAIL;
	}
	ASSERT_FAIL(CustodyTlvGetValue(8, s_stCustodyTlv, stMustChkResp.szShopID, &nFieldLen));
	if (memcmp(stMustChkResp.szShopID, pstSystem->szShopID, 15) != 0)
	{
		PubMsgDlg(pszTitle, "�����̻��Ų���", 3, 10);
		return APP_FAIL;
	}
	ASSERT_FAIL(CustodyTlvGetValue(9, s_stCustodyTlv, pstSystem->szRefnum, &nFieldLen));
	/*��Ӧ��*/
	ASSERT_FAIL(CustodyTlvGetValue(25, s_stCustodyTlv, pstSystem->szResponse, &nFieldLen));
	/*��Ӧ��Ϣ*/
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
* @brief ����MACֵ
* @param in int nMode
* @param in const char *psData
* @param in int nDataLen
* @param out  char * psMac
* @return ��
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
			PubMsgDlg("MAC����", "������̼���ʧ��", 3, 10);
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
	001	��������
	002	��������
	003	����ʱ��
	004	��ˮ��
	005	���κ�
	007	�ն˺�
	008	�̻���
	009	ϵͳ�ο��� 
	010	����
	013	�ֻ���
	014	��ȯ��
	025	��Ӧ��
	029	���׽��
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
* @brief �������
* @detail [������Ա��ɫ],[��齻�׿���],[���ǩ��״̬][��������]
* @param [in] pszTitle  ����
* @param [in] cOperFlag  ��־
* @param [in] cTransType  ��������
* @param [in] cAttr  ��������
* @return
* @li APP_FAIL ʧ��
* @li APP_SUCC �ɹ�
* @author ����
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
	* ����Ƿ�ǩ��
	*/
	ASSERT_QUIT(ChkLoginStatus());

	/**
	* ���POS����
	*/
	ASSERT_QUIT(DealPosLimit());

	return APP_SUCC;
}


