/**<
* 新大陆电脑公司 版权所有(c) 2012-2016
*
* POS统一版公共库
* 安全模块头文件  --- lsecurity.h
* 作    者：    刘罡
* 日    期：    2012-09-19
* 最后修改人：  刘罡 
* 最后修改日期：2012-10-19
*/
#ifndef _LSECRITY_H_
#define _LSECRITY_H_



#define PINPAD_HY			0x01
#define PINPAD_PP60_829	0x22
#define PINPAD_710			0x23

#define DESMODE_DES '0'		/**< 单des模式*/
#define DESMODE_3DES '1'		/**<3des模式*/

#define PINPAD_MAINKEYTYPE 0	/**< 主密钥 */
#define PINPAD_WORKKEYTYPE 1	/**<用户密钥*/

typedef enum 
{
	SECRITY_MODE_INSIDE=0x00,  			    /**<使用内置安全模块*/
	SECRITY_MODE_PINPAD,					/**<使用密码键盘*/
	SECRITY_MODE_ALL,				        /**<使用任一安全模块*/
}EM_SECRITY_MODE;

enum EM_KEY_TYPE
{
	KEY_TYPE_PIN=0x00,  			    /**<Pin密钥*/
	KEY_TYPE_MAC,					    /**<Mac密钥*/	
	KEY_TYPE_TRACK,				        /**<磁道密钥*/
	KEY_TYPE_DATA,				        /**<数据密钥*/
};

enum EM_MAC_TYPE
{
	MAC_TYPE_ECB=0,  			    	/**<ECB算法---即银联算法*/
	MAC_TYPE_X99,						/**<X99算法*/
	MAC_TYPE_X919,						/**<X919算法*/
	MAC_TYPE_9606,						/**<Mac 9606算法*/
};

enum EM_PIN_TYPE
{
   PINTYPE_WITHPAN = 0,       	    /**<带主账号加密*/
   PINTYPE_WITHOUTPAN = 1,     /**<不带主账号加密*/
   PINTYPE_PLAIN_STAR = 2,       /**<明文方式(恒宇),*显示*/
   PINTYPE_PLAIN = 3,                 /**<明文方式读取并且在Pinpad明文显示*/
};

typedef struct{
	char cAux;            	 /**<密码键盘所接串口*/
	char cTimeout;	       /**<密码键盘超时时间*/
	char cVoiceFlag;       /**<0x00表示关闭按键音，0x01表示打开按键音*/
	char cDesMode;         /**<0x01表示des,0x00表示3des*/
	char cPinMode;	       /**<0x00表示自动返回0x01表示确认返回*/
	char cMinPinLen;       /**<最小密码长度*/	
	char szLogo[17];       /**<密码键盘默认显示LOGO,支持ASCII或者字库中的汉字(针对ZT579)*/
	char szUid[17];	       /**<e18、826密码键盘专用*/
	char cDefaultType;     /**<默认密码键盘类型*/
	char cAmendMode;       /**<针对zt579-d2a密码键盘F3更正键是删除所有输入的密码还是删除最近输入的1位，1表示删除所有的 0表示删除1位*/
	char cAppDirID;        /**<应用目录ID  针对多应用而言的*/
	char cInitKeyFile;     /**<针对826密码键盘而言的，标记是否初始化密钥文件*/
	char cShowTime;        /**<针对826密码键盘的，用来在调用显示函数的时候显示多少秒*/
    char cIsUseNewKey;     /**<使用新的密钥体系,针对内置*/
	char sReserve[19];     /**<保留*/
}STPINPADPARAM;

/**
* @brief 初始化当前安全模块模式
* @detail 会检测相应的模块是否存在
* @param [in] nMode           ---EM_SECRITY_MODE
* @param [in] pstPinpadParam   ---密码键盘参数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubInitSecrity(int nMode,const STPINPADPARAM* pstPinpadParam);

/**
* @brief 清空密钥区
* @detail 清空密钥区
* @param [in] 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubClearKey(void);

/**
* @brief 设置当前主密钥索引号
* @param [in] nIndex  主密钥索引号  0-83
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubSetCurrentMainKeyIndex(int nIndex);

/**
* @brief 安装主密钥
* @param [in] nIndex  主密钥索引号  0-83
* @param [in] psKey   主密钥明文
* @param [in] nKeyLen 主密钥长度 8字节或16字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubLoadMainKey(int nIndex,const char* psKey, int nKeyLen);

/**
* @brief 安装工作密钥
* @param [in] nKeyType      密钥类型---EM_KEY_TYPE
* @param [in] psKey         密钥密文
* @param [in] nKeyLen       主密钥长度 8字节或16字节
* @param [in] psCheckValue  传入的CheckValue/(NULL)
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubLoadWorkKey(int nKeyType,const char* psKey, int nKeyLen,const char* psCheckValue);

/**
* @brief 获取Pin
* @detail 支持多种加密算法。实现密码最大最小长度的设置，显示无密码时，按确认键。
          密码输入提示在第二行、不清除第一行显示内容、输入时以星号显示，密码输完，清除屏幕到默认状态
* @param [out] pszPin        输入Pin
* @param [out] pnPinLen      输入Pin的长度
* @param [in]  nMode         密码加密模式EM_PIN_TYPE
* @param [in]  pszCardno     卡号/(NULL)
* @param [in]  pszAmount     金额
* @param [in]  nMaxLen       最大密码长度
* @param [in]  nMinLen       最小密码长度，对证通密码键盘无效
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubGetPin(char *pszPin, int *pnPinLen, int nMode, const char *pszCardno,const char *pszAmount,
			  int nMaxLen, int nMinLen);

/**
* @brief 获取Pin扩展函数(密码显示位置可由NDK_ScrGotoxy设置)
* @detail 支持自定义POS界面显示函数
* @param [out] pszPin        输入Pin
* @param [out] pnPinLen      输入Pin的长度
* @param [in]  nMode         密码加密模式EM_PIN_TYPE
* @param [in]  pszCardno     卡号/(NULL)
* @param [in]  nMaxLen       最大密码长度
* @param [in]  nMinLen       最小密码长度，对证通密码键盘无效
* @param [in]  ShowFunc      显示函数
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubGetPinExtern(char *pszPin, int *pnPinLen, int nMode, const char *pszCardno,
					int nMaxLen, int nMinLen,void (*ShowFunc)());

/**
* @brief 获取Mac
* @param [in]  nMode         算法类型---EM_MAC_TYPE
* @param [in]  psData        计算MAC的数据
* @param [in]  nDataLen      数据长度
* @param [out] psMac         MAC值
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubCalcMac(int nMode,const char * psData,int nDataLen,char * psMac);

/**
* @brief 在密码键盘上显示数据
* @param [in]  nLine         指定的行数1,2
* @param [in]  pszData       待显示的字符串
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubDispPinPad (int nLine, const char *pszData);


/**
* @brief 密码键盘恢复默认显示界面
* @param 无
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubClrPinPad(void);

/**
* @brief Des加密数据
* @param [in]  nKeyType      密钥类型---EM_KEY_TYPE
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubDes(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief Des解密数据(内置密码键盘)
* @param [in]  nKeyType      密钥类型---EM_KEY_TYPE
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubUnDes(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief 3Des加密数据
* @param [in]  nKeyType      密钥类型---EM_KEY_TYPE
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubDes3(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief 3Des解密数据(内置密码键盘)
* @param [in]  nKeyType      密钥类型---EM_KEY_TYPE
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubUnDes3(int nKeyType,const char* psSrc,char *psDest);

/**
* @brief 软Des加密数据
* @param [in]  psKey         密钥---8字节
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubSoftDes(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief 软Des解密数据
* @param [in]  psKey         密钥---8字节
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubSoftUnDes(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief 软3Des加密数据
* @param [in]  psKey         密钥---16字节
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubSoftDes3(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief 软3Des解密数据
* @param [in]  psKey         密钥---16字节
* @param [in]  psSrc         数据---8字节
* @param [out] psDest        密文---8字节
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 刘罡
* @date 2012-5-23
*/
int PubSoftUnDes3(const char* psKey,const char* psSrc,char *psDest);

/**
* @brief 用主密钥解密指定数据(外接密码键盘)
* @param [in]  pIn         待解密的数据
* @param [in]  nGroupNo    主密钥号
* @param [out] pOut        解密后的数据
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-08-21
*/
int PubUndesByMk(unsigned char* pOut, const unsigned char* pIn, int nGroupNo);

/**
* @brief 明文读取密码键盘消息(密码键盘第二行以明文方式显示输入的数字，按"确认"结束)
* @param [in]  pstString     密码键盘输入的字符串
* @param [in]  pnStringLen   字符串长度
* @param [in]  nMaxLen       最大输入长度
* @param [in]  nMinLen       最小输入长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-08-21
*/
int PubReadString(char *pstString, int *pnStringLen, int nMaxLen, int nMinLen);

/**
* @brief 在密码键盘液晶屏的指定行，显示中文字符串
* @param [in]  nLine         指定的行数1,2
* @param [in]  pszDotData    待显示的数据
* @param [in]  nDataLen      待显示的数据长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-08-21
*/
int PubDispDot(int nLine, const char *pszDotData, int nDataLen);

/**
* @brief 获取安全模块版本
* @param [out]pszVer  12字节版本号
* @return 
* @li 无
*/
void PubGetSecrityVerion(char *pszVer);

/**
* @brief 激活工作密钥，算ｍａｃ及输入密码函数前需先调用此操作。
* @param [in]  nMode : DESMODE_DES、DESMODE_3DES
* @param [in]  nIndex : 主密钥索引号  0-83
* @param [in]  nActiveKeyType : 工作密钥序号：0--KEY_TYPE_PIN  / 1--KEY_TYPE_MAC    /2--KEY_TYPE_TRACK /3--KEY_TYPE_DATA当密钥类型为主密钥时，此参数不起作用
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-08-21
*/
int PubActiveKey( int nMode, int nIndex, int nActiveKeyType );

/**
* @brief 使用密码键盘中指定组号的主密钥解密新主密钥，然后将解密后的新主密钥保存到指定的新组号中。
* @param [in]  psKey		新密钥密文
* @param [in]  KeyLen		新密钥密文长度
* @param [in]  NewGroup		新组号
* @param [in]  OldGroup		用于解密的原组号
* @param [out] pCV			校验值
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 林蔚
* @date 2012-08-21
*/
int PubUpdateMk(unsigned char *psKey, int nKeyLen, int nNewGroup, int nOldGroup, unsigned char* pCV);
#endif

