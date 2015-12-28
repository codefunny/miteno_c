/**
* @file debug.h
* @brief 调试模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "ltool.h"

extern int Debug(const char *pszFile, const char *pszFunc, const int nLine, const int nRes);
extern void SetDebugData(const char *pszFile, const char *pszFunc, const int nLine);
extern void Trace(char* lpszFormat, ...);
extern void TraceHex(char* pszHexBuf, int nLen, char* lpszFormat, ...);
extern void Lindebug(char* lpszFormat, ...);
extern int PrintDebugData(unsigned char *psData, int nLen);
#define ASSERT(e) Debug(__FILE__, __FUNCTION__, __LINE__,e)

#define ASSERT_FAIL(e) \
	if (ASSERT(e) != APP_SUCC)\
	{\
		return APP_FAIL;\
	}
	
#define ASSERT_QUIT(e) \
	if (ASSERT(e) != APP_SUCC)\
	{\
		return APP_QUIT;\
	}

#define ASSERT_HANGUP_QUIT(e) \
	if (ASSERT(e) != APP_SUCC)\
	{\
		CommHangUp();\
		return APP_QUIT;\
	}

#define ASSERT_HANGUP_FAIL(e) \
	if (ASSERT(e) != APP_SUCC)\
	{\
		CommHangUp();\
		return APP_FAIL;\
	}
	
#define ASSERT_UPCARD_POWERDOWN_FAIL(e)\
	if (ASSERT(e) != APP_SUCC)\
	{\
		PubMsgDlg(NULL, "读取失败", 3, 3);\
		PubCardPowerDown();\
		return APP_FAIL;\
	}
#define ASSERT_RETURNCODE(e) \
	{\
		int nTemp=e;\
		if (nTemp != APP_SUCC)\
		{\
			return nTemp;\
		}\
	}

#define ASSERT_PARABACK_FAIL(e) \
if (ASSERT(e) != APP_SUCC)\
{\
    PubCloseFile(&nIniHandle);\
	return APP_FAIL;\
}

	
#define TRACE(a) do{SetDebugData(__FILE__, __FUNCTION__, __LINE__);\
	Trace a;}while(0);

#define TRACE_HEX(a) do{SetDebugData(__FILE__, __FUNCTION__, __LINE__);\
	TraceHex a;}while(0);
	
#endif



