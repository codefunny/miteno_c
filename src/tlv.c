#include "apiinc.h"
#include "libapiinc.h"
#include "appinc.h"

#if defined(EMV_IC)
/*
*	@brief          往oTlvBuf中加入一个Tag-Len-Value
*	@param          const uint		iTag
*	@param          const int		iLen
*	@param          const char		*iValue
*	@return			TLVERR_xxx
*/
int TlvAdd(const uint unTag, const int nInLen, const char *pszInValue, char *pszOutTlvBuf, int *pnOutTlvBufLen)
{
	int i, nTagLen,nLen, nLenLen;
	char sTag[5];
	char sLenBuf[8];
	sprintf(sTag, "%x", unTag);
	nTagLen = strlen(sTag);
	PubAscToHex((uchar *)sTag, nTagLen, 0, (uchar *)(pszOutTlvBuf+*pnOutTlvBufLen));
	nTagLen >>= 1;
	*pnOutTlvBufLen += nTagLen;

	if (nInLen > 127)
	{
		nLen = nInLen;
		nLenLen = nInLen/256+1;	//跟随n个字节
		sLenBuf[0] = 128 | nLenLen;//zj &改成|
		for (i=0; i<nLenLen; i++)
		{
			sLenBuf[nLenLen-i] = nLen%256;
			nLen /= 256;
		}
	} else
	{
		sLenBuf[0] = nInLen;
		nLenLen = 0;
	}
	memcpy(pszOutTlvBuf+*pnOutTlvBufLen, sLenBuf, nLenLen+1);
	*pnOutTlvBufLen += nLenLen+1;

	memcpy(pszOutTlvBuf+*pnOutTlvBufLen, pszInValue, nInLen);
	*pnOutTlvBufLen += nInLen;
	
	return 0;
}

#endif
