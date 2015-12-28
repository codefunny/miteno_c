/**
* @file main.h
* @brief 主模块（函数入口）
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _MAIN_H_
#define _MAIN_H_

/*银商版本号在Makefile中统一修改,
应用名注意makefile main.h headerinfo里统一*/
#define APP_NAME			"ALLPAY"
#define APP_NAMEDESC		"都能付"APP_NAME

#ifndef APP_VERSION
#define APP_VERSION			"311004"			/*上传银联版本*/	
#endif

#ifndef APP_VERSION_DISP
#define APP_VERSION_DISP	"31100405"			/*终端显示版本*/
#endif

#ifndef CVS_TAG
#define CVS_TAG		"CN_UMS2_M2_V0-1B"	
#endif

#define USE_TMS_FIELD62_8 0x01

#endif
