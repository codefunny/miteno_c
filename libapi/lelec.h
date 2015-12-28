/****************************************************************************
图片格式转换：bmp 2 pbm ; pbm 2 jbig

修改历史：
1.0.0 2013年10月17日    何春

****************************************************************************/


#ifndef _LELEC_H_
#define _LELEC_H_

/**
* @param pszOutVer 返回版本串
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PubGetElecLibVer(char *pszOutVer);
int PubLanchElecBorad(const char *pszFeatureCode, const int nTimeOut);

int PubBmp2Pbm(unsigned char *pszAppFileName, unsigned char *pszOutFileName);
int PubPbm2Jbig(unsigned char *pszAppFileName, unsigned char *pszOutFileName);

#endif


