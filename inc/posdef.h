/**
* @file podef.h
* @brief 
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/
#ifndef _POSDEF_H_
#define _POSDEF_H_

#define KEY_FUNC_APP_SELECT KEY_MENU
#define KEY_FUNC_QPBOC_SALE_LINK KEY_RIGHT
#define KEY_FUNC_QPBOC_BALANCE_LINK KEY_LEFT


#if defined(EMV_IC)
#define DISP_OUT_ICC DispOutICC(NULL, "", "")
#else
#define DISP_OUT_ICC //
#endif

#if defined(SUPPORT_ELECSIGN)
#define ELEC_IMG_DIR "ELEC_IMG"
#endif

#define DISP_TRANS_SUCC \
{\
	PubDisplayGen(2,"交易成功");\
	PubUpdateWindow();\
}

#define DISP_PRINTING_NOW \
{\
	PubDisplayGen(3,"正在打印");\
	PubUpdateWindow();\
}

typedef enum
{
	LOGO_RF = 1,
	LOGO_MAIN_MENU,
	LOGO_STANDBY,
	
}EM_LOGO_TYPE;
#define _TEST_
#ifdef _TEST_
#define DEFAULT_COMM_AUX PORT_NUM_COM2
#else
#define DEFAULT_COMM_AUX PORT_NUM_COM1
#endif
#define DEFAULT_PAGE_LEN  792      //针打默认页长 
#define DEFAULT_PIN_INPUT_TIME 60 //内置密码键盘超时,外接的在99设置

#define UPDATE_CTRL                "00" /**用于版本升级,部分参数的初始化控制*/

//#define VISA_TEST 1     /*<VISA认证版本*/

/*<非接圈存不支持时,不应有非接图标,卡检TA要求*/
#define RF_PBOC_NO_SUPPORT 1

#define APP_OTHER_FAIL	-6

//#define BCTC_TEST  1  /*<检测中心认证版本*/

#endif

