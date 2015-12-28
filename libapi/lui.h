#ifndef _LUI_H_
#define _LUI_H_
#include "NDK.h"

#define KEY_0				'0'
#define KEY_1				'1'
#define KEY_2				'2'
#define KEY_3				'3'
#define KEY_4				'4'
#define KEY_5				'5'
#define KEY_6				'6'
#define KEY_7				'7'
#define KEY_8				'8'
#define KEY_9				'9'
#define KEY_F1				0x01
#define KEY_F2				0x02
#define KEY_F3				0x03
#define KEY_F4				0x04
#define KEY_F5				0x05
#define KEY_F6				0x06
#define KEY_F7				0x07
#define KEY_F8				0x08
#define KEY_F9				0x09
#define KEY_LEFT			KEY_F1
#define KEY_RIGHT			KEY_F2
#define KEY_UP				KEY_F1
#define KEY_DOWN			KEY_F2
#define KEY_UP_720			0x11
#define KEY_DOWN_720		0x12
#define KEY_LEFT_720		0x13
#define KEY_RIGHT_720		0x14
#define KEY_POINT			0x2e
#define KEY_BACK			0x0a		/**<退格键*/
#define KEY_ENTER			0x0d		/**<确认键*/
#define KEY_ESC				0x1b		/**<取消键*/
#define KEY_MENU			0x03		/**<菜单键*/
#define KEY_ENTERPAPER  	0xb1		/**<进纸键*/
#define KEY_BACKPAPER   	0xb2		/**<退纸键*/
#define KEY_NULL			0xfe		/**<键盘上无此按键，值定为0xfe*/

/*<POS机型定义*/
#define NL8000				0x80
#define NL8080				0x88
#define NLE80				0xE8
#define NL8200				0x82
#define NL8300				0x83
#define NL8510				0x85
#define NLGP8510			0xF5
#define NLGP710				0x71
#define NLGP720				0x72
#define NLGP730				0x73
#define NLGP730CS			0x74		/**<730彩屏*/
#define NLGP750				0x75
#define SP50				0xA5
#define SP60				0xA6
#define SP80				0xA8

typedef enum
{
	MENU_TITLE_MUST = 0,				/**<Menu函数标题必需存在*/
	MENU_TITLE_AUTO					/**<标题存在自适应 */
}EM_MENU_TITLE;

typedef enum
{
	DISPLAY_MODE_NORMAL=0,			/**<显示前不清行*/
	DISPLAY_MODE_CLEARLINE=1,			/**<显示前清行*/
	DISPLAY_MODE_LEFT=2,				/**<靠左显示*/
	DISPLAY_MODE_CENTER=4,			/**<靠右显示*/
	DISPLAY_MODE_RIGHT=8,				/**<居中显示*/
}EM_DISPLAY_MODE;

typedef enum
{
	DISPLAY_ALIGN_BIGFONT	=0,			/**<按大字体排列对齐*/
	DISPLAY_ALIGN_SMALLFONT			/**<按小字体排列对齐*/
}EM_DISPLAY_ALIGN;

typedef enum
{
	TIME_MODE_HOUR_MIN_SEC=0,			/**<时:分:秒*/
	TIME_MODE_ONELINE_YMD_HM,			/**<一行显示 年:月:日 时:分*/
	TIME_MODE_TWOLINE_YMD_HM			/**<俩行显示 年:月:日 时:分*/
}EM_TIME_MODE;

typedef enum 
{
	MENU_MODE_NONE = 0,				/**<不允许菜单退出*/
	MENU_MODE_ESCCONFIRM = 0x01,		/**<退出是否需要确认*/
	MENU_MODE_ESC = 0x02,				/**<按ESC退出*/
	MENU_MODE_AUTOESC=0x03			/**<执行完自动退出*/
}EM_MENU_MODE;

typedef enum
{
	INPUT_MODE_STRING,					/**<输入模式:一般字串*/
	INPUT_MODE_NUMBER,					/**<输入模式:只允许数字*/
	INPUT_MODE_PASSWD,					/**<输入模式:输入密码*/
	INPUT_MODE_AMOUNT,					/**<输入模式:金额*/
	INPUT_MODE_NUMBERPOINT			/**<输入数字和点*/
}EM_INPUT_MODE;

typedef enum
{
	INPUT_DATE_MODE_YYMM,				/**<日期格式输入模式*/
	INPUT_DATE_MODE_MMDD,
	INPUT_DATE_MODE_YYMMDD,
	INPUT_DATE_MODE_YYYYMMDD,
	INPUT_DATE_MODE_MMDD_NULL,
	INPUT_DATE_MODE_YYMM_NULL,
	INPUT_DATE_MODE_YYMMDD_NULL,
	INPUT_DATE_MODE_YYYYMMDD_NULL
}EM_INPUT_DATE_MODE;

typedef enum
{
	INPUT_AMOUNT_MODE_NONE,			/**<金额输入允许为空(输入的金额0)*/
	INPUT_AMOUNT_MODE_NOT_NONE		/**<金额输入必须大于0*/
}EM_INPUT_AMOUNT_MODE;

typedef enum
{
	CHOOSE_IME_MODE_NUMPY,			/**<选择数拼*/
	CHOOSE_IME_MODE_ENGLISH ,			/**<选择英文*/
	CHOOSE_IME_MODE_IME_NUM,    		/**<数字*/
	CHOOSE_IME_MODE_IME_BIHUA,		/**<笔画*/
	CHOOSE_IME_MODE_QUWEI,			/**<选择区位*/
	CHOOSE_IME_MODE_GBK, 	  			/**<内码*/
	CHOOSE_IME_MODE_HANDWRITE	 		/**<手写*/
}EM_CHOOSE_IME_MODE;

typedef struct
{
	char szContent[16];				/**<项目内容         */
	int	(*action)(void);				/**<项目执行函数的指针*/
} STMENUTYPE;

typedef struct
{
	uchar ucSum;						/**<菜单总数*/
	uchar ucIndex;						/**<菜单索引*/
	uchar ucFlag;						/**<菜单从0开始还是1开始*/
	uchar ucDefaultFlag;				/**<是否默认1是，0否*/
	char szText[21][16+1];				/**< 菜单提示信息,最大菜单数21*/
}STMENU_LIST;

typedef struct
{
	EM_MENU_TITLE emMenu;						/**<EM_MENU_TITLE*/
	char szPageDown[4+1];						/**<翻页标题提示信息*/
	char szPageUp[4+1];						/**<翻页标题提示信息*/
}STUIFUN_STYLE;

typedef struct
{
	char szProName[20+1];	/**<项目名称，不可为空*/
	char szAppVer[16+1];	/**<16字节应用版本信息，不可为空*/
	char szEmvVer[16+1];	/**<16字节EMV版本信息，可为空*/
	char szMcVer[16+1];	/**<16字节主控库版本信息，可为空*/
	char szLuaVer[16+1];	/**<16字节LUA库版本信息，可为空*/
	char szPriLib1[16+1];	/**<16字节应用私有库1版本信息，可为空*/
	char szPriLib2[16+1];	/**<16字节应用私有库2版本信息，可为空*/
	char szPriLib3[16+1];	/**<16字节应用私有库3版本信息，可为空*/
	char szPriLib4[16+1];	/**<16字节应用私有库4版本信息，可为空*/
	char szPriLib5[16+1];	/**<16字节应用私有库5版本信息，可为空*/
}STAPPVERS;


typedef struct 
{
    char szMenuName[32];		/*菜单名称*/
    int nMenuID;				/*对应的功能ID*/
    int nMenuDepth;			    /*菜单级数*/
    int nShortcutKey;			/*快捷键*/
    int nIsVisible;				/*当前菜单是否可见*/
} STDYNMENU_RESOURCE;

typedef struct
{
    char szMenuName[32];		/*菜单名称*/
    int nMenuID;				/*对应的功能ID*/
    int nIsVisible;				/*当前菜单是否可见*/
} STMENU_ITEM;

enum {
    NEXTPAGE,         /**< 按键时翻页 */    
    NEXTITEM,         /**< 按键时滑动项 */   
    NEXTLINE          /**< 逐行 */
}EM_PAGETYPE;

/**
* @brief 取UI库版本号
* @param [out] pszVer 存放UI版本号
* @return 
* @li无
* @author 林蔚
* @date 2012-06-26
*/
void PubGetUIVer(char *pszVer);

/**
* @brief 获取POS机型
* @return 
* @li APP_FAIL 失败
* @li Others --成功，NL8000,NL8080,NLE80,NL8200,NL8300,NL8510,NLGP8510,NLGP710,NLGP730,NLGP730CS
* @author 林蔚
* @date 2012-07-12
*/
int PubGetPosType();

/**
* @brief 获取POS机型
* @return 
* @li NULL 失败
* @li POS机型字符串 --成功
* @author 林蔚
* @date 2013-05-27
*/
char * PubGetPosTypeStr();


/**
* @brief 初始化UI参数，使用UI库前必须调用该函数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-07-12
*/
int PubInitUiParam();

/**
 *@brief	设置使用的显示字体。
 *@param	emType	选择字体
 *@return
* @li APP_FAIL 失败
* @li APP_SUCC 成功
*/
int PubSetFontType(EM_DISPFONT emType);

/**
* @brief 设置UI函数风格
* @return 
* @li 无
* @author 林蔚
* @date 2012-11-28
*/
void PubSetUiStyle(STUIFUN_STYLE stUiFunStyle);


/**
* @brief 清空屏幕的内容
* @return 
* @li无
*/
void PubClearAll(void);

/**
* @brief 清空屏幕的第二到最后一行。
* @return 
* @li 无
*/
void PubClear2To4 ( void );

/**
* @brief 清空屏幕某行（大字体）内容
* @param [in] ucStartLine 需要清空的行号开始行，从1开始
* @param [in] ucEndLine  需要清空的行号结束行，末行可输入(0xff)
* @return 
* @li 无
*/
void PubClearLine(uchar ucStartLine, uchar ucEndLine);

/**
* @brief 清空屏幕某行（小字体）内容
* @param [in] ucStartLine 需要清空的行号开始行，从1开始
* @param [in] ucEndLine  需要清空的行号结束行，末行可输入(0xff)
* @return 
* @li 无
*/
void PubClearLines(uchar ucStartLine, uchar ucEndLine);

/**
* @brief 获取缓冲区中的首个键盘键值，立即返回
* @return 
* @li 按键返回值,无按键按下时为0
*/
int PubKbHit();

/**
* @brief 获取按键值
* @param [in] nTimeout	超时时间
* @return 
* @li 按键返回值
*/
int PubGetKeyCode(int nTimeout);

/**
* @brief 在指定位置按格式显示大字符串（显示汉字字符）
* @param [in] nMode   显示模式---EM_DISPLAY_MODE 
* @param [in] nLineno 指定行位置，从1开始
* @param [in] nClumno 指定列位置，从1开始
* @param [in] pFormat 显示格式 
* @return 
* @li 无
*/
void PubDisplayStr(int nMode, int nLineno, int nClumno, char * pFormat,...);

/**
* @brief 在指定位置按格式显示小字符串(显示西文字符)
* @param [in] nAlign   小字体的对齐模式---EM_DISPLAY_ALIGN
* @param [in] nMode   显示模式---EM_DISPLAY_MODE
* @param [in] nLineno 指定行位置，从1开始
* @param [in] nClumno 指定列位置，从1开始
* @param [in] pFormat 显示格式 
* @return 
* @li 无
*/
void PubDisplayStrs(int nAlign, int nMode, int nLineno, int nClumno, char *pFormat,...);

/**
* @brief 在POS屏幕指定行，居中显示大字符串（显示前清行）
* @param [in] pStr    输入字符串
* @param [in] nLineno 指定行位置，从1开始
* @return 
* @li 无
*/
void PubDisplayGen(int nLineno, char *pStr); 

/**
* @brief 在POS屏幕指定行，居中显示小字符串（显示前清行）
* @param [in] nAlign   小字体的对齐模式---EM_DISPLAY_ALIGN
* @param [in] pStr    输入字符串
* @param [in] nLineno 指定行位置，从1开始
* @return 
* @li 无
*/
void PubDisplayGens(int nAlign, int nLineno, char *pStr);

/**
* @brief 在POS屏幕指定行反白显示大字符串
* @param [in] pStr    输入字符串
* @param [in] nLineno 指定行位置，取值范围为1~7
* @return 
* @li 无
*/
void PubDisplayInv(int nLineno, char *pstr);

/**
* @brief 在POS屏幕指定行反白显示小字符串（显示前清行）
* @param [in] nAlign   小字体的对齐模式---EM_DISPLAY_ALIGN
* @param [in] nLineno 指定行位置，从1开始
* @param [in] pStr    输入字符串
* @return 
* @li 无
*/
void PubDisplayInvs(int nAlign, int nLineno, char *pstr); 

/**
* @brief 显示标题
* @param [in] pszTitle ---显示标题
* @return 
* @author 刘罡
* @date 2012-7-23
*/
void PubDisplayTitle(char *pszTitle);

/**
* @brief 在末显示
* @param [in] nFont	字体
* @param [in] nMode	显示模式---EM_DISPLAY_MODE 
* @param [in] nIsInv	是否反白显示'0'正常'1'反白
* @param [in] pFormat	显示内容
* @return 
*/
void PubDisplayTail(int nFont, int nMode, int nIsInv, char * pFormat,...);

/**
* @brief 在指定位置显示多行(大小)字符串
* @param [in] nFont	0 大字体, 1 小字体
* @param [in] nLineno	指定行位置，取值范围为1~8
* @param [in] nClumno	指定列位置,取值范围为1~16
* @param [in] pFormat		显示内容
* @return 
* @li 无
*/
void PubDispMultLines(int nFont, int nLineno, int nClumno, char * pFormat,...);

/**
* @brief 在指定行显示(大小)字符串
* @param [in] nFont	0 大字体, 1 小字体
* @param [in] nLineno	指定行位置，取值范围为1~8
* @param [in] pStr		显示内容带格式L靠左C居中R靠右。例如("L%s|C%s|R%s",szLeft,szCenter,szRight)
* @return 
* @li 无
*/
void PubDispStrFormat(int nFont, int nLineno, char * pFormat,...);

/**
* @brief 获取指定格式(大小)字符串，可多行内容
* @param [in] nFont	0 大字体, 1 小字体
* @param [in] pFormat 显示格式: 内容带格式L靠左C居中R靠右。例如("L%s|C%s|R%s",szLeft,szCenter,szRight)
* @param [out] pOutStr 按格式对齐的字符串
* @return 
* @li 无
*/
void PubGetStrFormat(int nFont, char * pOutStr, char * pFormat,...);

/**
* @brief 显示图形宽为64高为48的图片
* @param [in] pszPath 要显示的图片路径
* @return 
* @li 无
*/
void PubDisplayLogo(char *pszPath, int nX, int nY);

/**
* @brief 显示时间
* @param [in] nLineno 指定行位置，取值范围为1~8
* @param [in] nClumno 指定列位置,取值范围为1~16
* @param [in] nStyle  时间显示风格---EM_SHOWTIME_MODE
* @return 
* @li 无
*/
void PubShowRtc(int nLineno, int nClumno, int nStyle);

/**
* @brief 关闭时间显示
* @return 
* @li 无
*/
void PubCloseShowRtc(void);

/**
* @brief 获取一次无线信号值显示图标(一个汉字大小)
* @param [in] nX 点行位置,取值范围为0~127
* @param [in] nY 点列位置,取值范围为0~63
* @return 
* @li 无
*/
void PubShowSignal(int nX, int nY);

/**
* @brief 获取一次电源电压值并显示图标(一个汉字大小)
* @param [in] nX 点行位置,取值范围为0~127
* @param [in] nY 点列位置,取值范围为0~63
* @return 
* @li 无
*/
void PubShowBattery(int nX, int nY);

/**
* @brief 状态栏显示
* @return 
* @li 无
*/
void PubShowStatusBar();

/**
* @brief 关闭状态栏显示
* @return 
* @li 无
*/
void PubCloseShowStatusBar();

/**
* @brief 在POS屏幕指定行，按格式显示大字符串(显示汉字字符)
* @param [in] nMode	显示模式---EM_DISPLAY_MODE
* @param [in] nLineno 指定行位置，从1开始
* @param [in] pFormat 显示格式
* @return 
* @li 无
*/
void PubDisplayStrInline(int nMode, int nLineno, char *pFormat,...);

/**
* @brief 在POS屏幕指定行，按格式显示小字符串(显示西文字符)
* @param [in] nAlign   小字体的对齐模式---EM_DISPLAY_ALIGN
* @param [in] nMode	显示模式---EM_DISPLAY_MODE
* @param [in] nLineno 指定行位置，从1开始
* @param [in] pFormat 显示格式
* @return 
* @li 无
*/
void PubDisplayStrInlines(int nAlign, int nMode, int nLineno, char *pFormat,...);

/**
* @brief 获取液晶屏色深,可用于判断液晶屏是单色屏或彩屏
* @return 
* @li APP_FAIL    失败
* @li 否则        返回液晶屏色深：1----黑白两色, 16----16位色，彩屏
*/
int PubScrGetColorDepth(void);

/**
* @brief 从键盘读入字串
* @param [in] nX        列位置(1<= nX <=16)
* @param [in] nY        行位置(1<= nY <=8)
* @param [in] nMinLen   最小输入长度
* @param [in] nMaxLen   最大输入长度
* @param [in] nTimeOut  输入超时时间
* @param [in] nEditMask 输入模式---EM_INPUT_MODE
* @param [out] pszOut   输入缓冲区
* @param [out] pnOutLen 输入的字符长度
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_FAIL    表示异常退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    输入成功
*/
int PubInputxy(int nX, int nY, char* pszOut, int* pnOutLen, int nMinLen, int nMaxLen, int nTimeOut, int nEditMask);

/**
* @brief 输入对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nMinLen    最小输入长度
* @param [in] nMaxLen    最大输入长度
* @param [in] nTimeOut   输入超时时间
* @param [in] nEditMask  输入模式---EM_INPUT_MODE
* @param [out] pszOut    输入缓冲区
* @param [out] pnOutLen  输入的字符长度
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_FAIL    表示异常退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    输入成功
*/
int PubInputDlg(const char *pszTitle, const char *pszContent, char *pszOut, int *pnOutLen, \
				int nMinLen, int nMaxLen, int nTimeOut, int nEditMask);

/**
* @brief 输入对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] pInfo      如果没有为NULL
* @param [in] x          x坐标
* @param [in] y          y坐标
* @param [in] nMinLen    最小输入长度
* @param [in] nMaxLen    最大输入长度
* @param [in] nTimeOut   输入超时时间
* @param [in] nEditMask  输入模式---EM_INPUT_MODE
* @param [out] pszOut    输入缓冲区
* @param [out] pnOutLen  输入的字符长度
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_FAIL    表示异常退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    输入成功
*/
int PubExtInputDlg(const char *pszTitle, const char *pszContent, char* psInfo, \
                int x, int y, char *pszOut, int *pnOutLen,int nMinLen, \
                int nMaxLen, int nTimeOut, int nEditMask);

/**
* @brief 提示对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nBeep      提示响铃次数
* @param [in] nTimeOut   输入超时时间
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_SUCC    输入成功
*/
int PubMsgDlg(const char *pszTitle, const char *pszContent, int nBeep, int nTimeOut);

/**
* @brief 确认对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nBeep      提示响铃次数
* @param [in] nTimeOut   输入超时时间
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_QUIT    用户取消
* @li APP_SUCC    用户确定
*/
int PubConfirmDlg(const char *pszTitle, const char *pszContent, int nBeep, int nTimeOut);

/**
* @brief 输入日期对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nFormat    日期格式---EM_INPUT_DATE_MODE
* @param [in] nTimeOut   输入超时时间
* @param [out] pszDate   日期
* @return 
* @li APP_FAIL    表示异常退出
* @li APP_TIMEOUT 超时退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    表示输入成功
*/
int PubInputDate(const char *pszTitle, const char *pszContent, char *pszDate, int nFormat, int nTimeOut);

/**
* @brief 输入日期对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] pInfo      如果没有为NULL
* @param [in] x          x坐标
* @param [in] y          y坐标
* @param [in] nFormat    日期格式---EM_INPUT_DATE_MODE
* @param [in] nTimeOut   输入超时时间
* @param [out] pszDate   日期
* @return 
* @li APP_FAIL    表示异常退出
* @li APP_TIMEOUT 超时退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    表示输入成功
*/
int PubExtInputDate(const char *pszTitle, const char *pszContent, const char *pInfo, int x, int y,\
					char *pszDate,int nFormat, int nTimeOut);

/**
* @brief 输入金额对话框
* @param [in] pszTitle   对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nNoneFlag  EM_INPUT_AMOUNT_MODE
* @param [in] nMaxLen    最大输入长度
* @param [in] nTimeOut   输入超时时间
* @param [out] pszAmount 金额
* @param [out] pnInputLen 输入长度
* @return 
* @li APP_TIMEOUT 超时退出
* @li APP_FAIL    表示异常退出
* @li APP_QUIT    用户退出界面
* @li APP_Others  输入的长度
*/
int PubInputAmount(const char *pszTitle, const char *pszContent, char *pszAmount, int *pnInputLen,\
				   int nNoneFlag, int nTimeOut);

/**
* @brief 输入汉字对话框
* @param [in] pszStr      对话框初始值
* @param [in] nMinLen     输入的最小长度
* @param [in] nMaxLen     输入的最大长度
* @param [in] nImeMode    默认选择的输入法---EM_CHOOSE_IME_MODE
* @param [out] pszStr     汉字信息
* @param [out] pnInputLen 输入的长度
* @return 
* @li APP_FAIL    表示异常退出
* @li APP_TIMEOUT 超时退出
* @li APP_QUIT    用户退出界面
* @li APP_SUCC    表示输入成功
*/
int PubInputCNMode(const char *pszTitle, const char *pszContent,char *pszStr, int *pnInputLen, int nMinLen, int nMaxLen, int nImeMode);

/**
* @brief 菜单显示函数 显示页面、处理按键
* @param [in] pstMenu      菜单指针
* @param [in] pszTitle     菜单标题，如果没有就为NULL
* @param [in] cMenuFlag    菜单标志---MENU_OPTION
* @param [in] (*pnTimer)() 后台函数函数指针，如果没有就为NULL
* @param [in] nInterval    在停留菜单界面时候，间隔时间进入pnTimer
* @return 
* @li APP_FAIL 表示pstMenu格式不可识别
* @li APP_QUIT 用户退出界面
*/
int PubOpenMenu(const STMENUTYPE *pstMenu, const char *pszTitle, char cMenuFlag, int(*pnTimer)(), int nInterval);

/**
* @brief 菜单显示函数 显示页面、处理按键
* @param [in] pstMenu      菜单指针
* @param [in] pszTitle     菜单标题，如果没有就为NULL
* @param [in] cMenuFlag    菜单标志---MENU_OPTION
* @return 
* @li APP_FAIL 表示pstMenu格式不可识别
* @li APP_QUIT 用户退出界面
*/
int PubOpenEasyMenu(const STMENUTYPE * pstMenu, const char * pszTitle, char cMenuFlag);

/**
* @brief 列表显示要选择的项目
* @param [in] pszStr       列表内容，用'|'隔开
* @param [in] pszTitle     菜单标题，如果没有就为NULL
* @param [in] pszContent   菜单内容提示，没有则为NULL
* @param [in] nDefault     默认的选择(0-9)，无默认为0
* @param [out] pnItemnum   返回的选项或 键值
* @return 
* @li APP_FAIL 表示pstMenu格式不可识别
* @li APP_QUIT 用户退出界面
* @li APP_SUCC 用户进行了正确了选择
*/
int PubSelectListItem(const char *pszStr, const char *pszTitle, const char* pszContent, int nDefault, int *pnItemnum);

/**
* @brief 列表显示要选择的项目
* @param [in] pszStr       列表内容，用'|'隔开
* @param [in] pszTitle     菜单标题，如果没有就为NULL
* @param [in] pszContent   菜单内容提示，没有则为NULL
* @param [in] nDefault     默认的选择(0-9)，无默认为0
* @param [in] PnItemnum    返回的选项或 键值
* @param [in] nTimeOut     等待按键超时时间
* @param [in] pszFunKey    期待返回功能键码串，如果没有则为NULL
* @param [out] pnItemnum   客户现在的项目(1-9)
* @return 
* @li APP_FAIL 表示pszStr格式不可识别,显示数目过多等
* @li APP_QUIT 用户退出界面
* @li APP_SUCC 用户进行了正确了选择
*/
int PubSelectListItemExtend( const char* pszStr, const char* pszTitle, const char* pszContent, int nDefault,\
							int* pnItemNum, int nTimeOut, char* pszFunKey);

/**
* @brief 显示图片和内容
* @param [in] unWidth		图片显示的宽
* @param [in] unHeight		图片显示的高
* @param [in] pszBmp  		图片文件数组
* @param [in] pszStr     	若是列表内容，用'|'隔开选项，否则直接显示内容
* @param [out] PnItemnum    返回的选项
* @return 
* @li 无
*/
void PubDrawBitmapList(const uint unWidth, const uint unHeight, const char *pszBitmap, const char *pszStr, int nTimeout, int* pnItemNum);

/**
* @brief 显示图片和内容
* @param [in] unWidth		图片显示的宽
* @param [in] unHeight		图片显示的高
* @param [in] pszPic   		图片文件所在的路径
* @param [in] pszStr     	若是列表内容，用'|'隔开选项，否则直接显示内容
* @param [out] PnItemnum    返回的选项
* @return 
* @li 无
*/
void PubDispPicList(const uint unWidth, const uint unHeight, const char *pszPic, const char *pszStr, int nTimeout, int* pnItemNum);

/**
* @brief 菜单列表显示选择
* @param [in] pszTitle  菜单标题，如果没有就为NULL
* @param [in] pstMenu   菜单列表
* @param [in] pszFunKey 功能键列表,如果无传入NULL,
* @param [in] nTimeOut  按键超时时间
* @param [in] nTimeOut  等待按键超时时间
* @param [out] pnKey    按键输出,正常输出菜单键值，如果是功能键则输出功能键值+0xff
* @return 
* @li APP_FAIL    表示pszStr格式不可识别,显示数目过多等
* @li APP_TIMEOUT 超时退出
* @li APP_SUCC    用户进行了正确了选择
*/
int PubDispSelectMenu (const char * pszTitle, STMENU_LIST * pstMenu, const char* pszFunKey, int nTimeOut, int* pnKey);

/**
* @brief 输入IP
* @param [in] pszTitle   菜单标题
* @param [in] pszContent 菜单内容提示
* @param [in] pnOutLen   传入字符串pszOut的长度
* @param [out] pszOut    输入的IP
* @param [out] pnOutLen  传出字符串pszOut的长度
* @return
* @li APP_QUIT    退出
* @li APP_SUCC    正确
*/
int PubInputIp(const char * pszTitle, const char * pszContent, char * pszOut, int * pnOutLen);

/**
* @brief 输入Pin明文
* @param [in] pszTitle    ---标题
* @param [in] pszAmount   ---输入的金额或者NULL
* @param [in] pszContent  ---提示内容或者NULL
* @param [in] nMinLen     ---输入金额的最小位数
* @param [in] nMaxLen     ---输入金额的最大位数
* @param [out] pszOut     ---密码明文
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败 
* @author 刘罡
* @date 2012-7-23
*/
int PubInputPin(const char *pszTitle,const char * pszAmount,const char * pszContent,int nMinLen, int nMaxLen,char *pszOut);

/**
* @brief 上下翻页显示对话框
* @param [in] pszTitle     ---标题
* @param [in] pszContent   ---显示内容
* @param [in] nIsResKey	---是否响应按键YES or No
* @param [in] nTimeOut     ---超时时间
* @param [out] nResKey	---返回按键值
* @return 
* @li APP_SUCC  ---成功
* @li APP_QUIT  ---退出
* @li APP_FAIL  ---失败 
* @author 刘罡
* @date 2012-8-2
*/
int PubUpDownMsgDlg(const char* pszTitle,const char* pszContent,int nIsResKey,int nTimeOut,int* nResKey);

/**
* @brief 确认对话框(F1F2翻页)
* @param [in] pszTitle	 对话框标题
* @param [in] pszContent 对话框提示信息
* @param [in] nBeep 	 提示响铃次数
* @param [in] nTimeOut	 输入超时时间
* @return 
* @li TIMEOUT 超时退出
* @li APP_QUIT	  用户取消
* @li APP_SUCC	  用户确定
*/
int PubUpDownConfirmDlg(const char* pszTitle,const char* pszContent,int nBeep,int nTimeOut);

/**
* @brief 更新显示窗体 
* @param 无
* @return 无
* @author 刘罡
* @date 2012-7-20
*/
void PubUpdateWindow(void);

/**
* @brief 显示错误码
* @param [in] pszTitle 显示标题
* @return 
* @li APP_SUCC  ---无错误
* @li APP_FAIL  ---有错误并显示错误码
*/
int PubDispErr(const char *pszTitle);

/**
* @brief 获取当前UI函数类型
* @param [out] pnColor	1彩屏UI函数，0普屏UI函数
* @return 
* @li 无
*/
void PubGetUIFunType(int* pnColor);

/**
* @brief 设置UI函数类型(黑白屏设置为彩屏UI函数无效)
* @param [in] nColor	1彩屏UI函数，0普屏UI函数
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败
*/
int PubSetUIFunType(int nColor);

/**
* @brief 记录版本信息
* @param [in] stVersion 版本信息具体查看STAPPVERS定义
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败
*/
int PubRecordVersions(STAPPVERS stVersion);

/**
* @brief 显示PubRecordVersions函数记录的版本信息
* @param 无
* @return 无
*/
void PubDisplayVersions();

/**
* @brief 获取行间距和字间距。
* @param [out] punWspace -- 字间距（单位：像素）
* @param [out] punHspace -- 行间距（单位：像素）
* @return 
* @li 无
*/
void PubScrGetSpace(uint *punWspace, uint *punHspace);

/**
* @brief 设置行间距和字间距。
* @param [in] unWspace	字间距（单位：像素）
* @param [in] unHspace	行间距（单位：像素）
* @return 
* @li 无
*/
void PubScrSetSpace(uint unWspace, uint unHspace);

/**
* @brief  中文智能截断函数, 解决一行中文显示半个汉字的问题
* @param in  scr:源数据串
* @param in  AscLen:源数据长度
* @param out Dst:目标数据串
* @return
* @li  返回已拷贝的字符串长度
*/
int PubCnStrCopy(char *psDst, const char *psSrc, int nLen);

/**
* @brief  获取显示参数
* @param out  pnMaxLine:可显示中文行数
* @param out  pnLineCol:一行可显示中文长度
* @return
* @li  无
*/
void PubGetDispView(int* pnMaxLine, int* pnLineCol);

/**
 *@brief    运行触屏校准程序(NDK版本4.4.8以前返回值都为0)
 *@return
 *@li       APP_SUCC    校准成功
 *@li       APP_FUNCQUIT POS无触屏设备
 *@li       APP_QUIT 用户退出
 *@li       APP_FAIL 操作失败
*/
int  PubScrTSCalibrate(void);


/**
* @brief 设置UI显示字体
* @param [in] unNormalColor    普通显示字体
* @param [in] unAmountColor   金额显示字体
* @param [in] unTitleColor        标题显示字体
* @param [in] unBgColor          背景显示字体
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败
*/
void PubSetUiFont(uint unNormalColor, uint unAmountColor, uint unTitleColor, uint unBgColor);

/**
* @brief 获取UI显示字体
* @param [out] unNormalColor    普通显示字体
* @param [out] unAmountColor   金额显示字体
* @param [out] unTitleColor        标题显示字体
* @param [out] unBgColor          背景显示字体
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败
*/
void PubGetUiFont(uint* punNormalColor, uint* punAmountColor, uint* punTitleColor, uint* punBgColor);


/**
* @brief 格式化动态菜单数据
* @param [in]   pszMenuString    列表内容
* @param [out] plstMenuResource   菜单数组
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败pszMenuString 格式不正确
*/
 int PubLoadDynMenu(const char *pszMenuString, STDYNMENU_RESOURCE *plstMenuResource);

/**
* @brief 显示动态菜单数据
* @param [in]   plstMenuResource       菜单数组
* @param [out] pnItem                      选中菜单对应的功能ID
* @param [in]   nTimeOut                   超时时间
* @param [in]   nESCFlag                   EM_MENU_MODE
* @param [in]   cIcoFlag                    是否显示翻页提示0否1是
* @param [in]   nStartLine                 起始行
* @param [in]   nEndLine                   结束行
* @param [in]   nPageType                EM_PAGETYPE
* @return 
* @li APP_SUCC  ---成功
* @li APP_FAIL  ---失败
* @li APP_TIMEOUT ---超时
* @li APP_QUIT ---退出
*/
 int PubProcDynMenu(const STDYNMENU_RESOURCE *plstMenuResource, int *pnItem, int nTimeOut, char nESCFlag, 
    const char cIcoFlag, const int nStartLine, const int nEndLine, const int nPageType);

/**
 *@brief    上下菜单翻页
 * @param in  plSetFuns  :页面设置函数列表
 *@return
 *@li       APP_SUCC    操作成功
 *@li       APP_QUIT 用户退出
 *@li       APP_FAIL 操作失败
*/
int PubUpDownMenus(int (*plSetFuns[])(void));


#endif

