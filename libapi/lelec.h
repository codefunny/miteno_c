/****************************************************************************
ͼƬ��ʽת����bmp 2 pbm ; pbm 2 jbig

�޸���ʷ��
1.0.0 2013��10��17��    �δ�

****************************************************************************/


#ifndef _LELEC_H_
#define _LELEC_H_

/**
* @param pszOutVer ���ذ汾��
* @return
* @li APP_SUCC
* @li APP_FAIL
*/
int PubGetElecLibVer(char *pszOutVer);
int PubLanchElecBorad(const char *pszFeatureCode, const int nTimeOut);

int PubBmp2Pbm(unsigned char *pszAppFileName, unsigned char *pszOutFileName);
int PubPbm2Jbig(unsigned char *pszAppFileName, unsigned char *pszOutFileName);

#endif


