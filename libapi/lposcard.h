/*
* 新大陆电脑公司 版权所有(c) 2012-2016
*
* POS统一版卡库
* (磁条卡、接触式IC卡、非接射频卡)头文件  --- lposcard.h
* 作    者：    廖华仔
* 日    期：    2012-12-10
* 最后修改人：  廖华仔
* 最后修改日期：2012-12-10
*/
#ifndef _LPOSCARD_H
#define _LPOSCARD_H

#define IC1_EXIST		0x01	/**<bit0：如果IC卡1已插卡，为1*/
#define IC1_POWERON	0x02	/**<bit1：如果IC卡1已上电，为1*/
#define IC2_POWERON	0x04	/**<bit2：保留，返回值“0”*/
#define IC3_POWERON	0x08	/**<bit3：保留，返回值“0”*/
#define SAM1_POWERON	0x10	/**<bit4：如果SAM卡1已上电，为1*/
#define SAM2_POWERON	0x20	/**<bit5：如果SAM卡2已上电，为1*/
#define SAM3_POWERON	0x40	/**<bit6：如果SAM卡3已上电，为1*/
#define SAM4_POWERON	0x80	/**<bit7：如果SAM卡4已上电，为1*/

#define CARD_FUNCQUIT (-4)  		/**<功能键退出*/

typedef enum{
	RFIDREADER_POS=1,			/**<内置读卡器*/
	RFIDREADER_ZX,	 		 	/**<中兴读卡器*/
	RFIDREADER_SH,	 		 	/**<盛华(直通)读卡器*/
	RFIDREADER_829			/**<我司829*/
}ENM_RFREADERTYPE;			/**<非接读卡器类型*/

typedef enum{
	EM_RF_INSIDE  = 0x01,		/**<POS内置射频读卡模块*/
	EM_RF_OUTSIDE = 0x02		/**<POS外接射频读卡模块*/
}ENM_INOROUTFLAG;			/**<POS读卡器内置或外接标记*/

typedef enum {
	EM_CARD_RF_M1  = 0x01,		/**<mifare one*/
	EM_CARD_RF_CPU = 0x02		/**<cpu*/
}ENM_CARDTYPE;				/**<POS读卡器内置或外接标记*/

typedef enum{
	RFCARDTYPE_CPU_INSIDE=0,	/**<内置CPU卡*/
	RFCARDTYPE_M1_INSIDE,		/**<内置M1卡*/
	RFCARDTYPE_CPU_OUTSIDE,	/**<外置CPU卡*/
	RFCARDTYPE_M1_OUTSIDE	/**<外置M1卡*/
}ENM_RFIDCARDTYPE;			/**<非接卡类型标记*/

typedef enum{
	EM_RF_AKEY  = 0x00,        	/**<A Key*/
	EM_RF_BKEY = 0x01			/**<B Key*/
}ENM_RFIDM1KEYMODE;		/**<M1卡 Key类型*/

typedef enum 
{
	RD_SHOWBALANCE = 0x00,			/**< 显示交易余额*/
	RD_SHOWSUCC = 0x01,				/**< 显示交易成功*/
	RD_SHOWFAIL =0x02,					/**< 显示交易失败*/
	RD_SHOWLITTEBALANCE= 0x03,		/**< 显示余额不足*/
	RD_SHOWEXCEPTION =0x04,			/**< 显示交易异常*/
	RD_SHOWSWIPCARD =0x05,			/**< 显示请求刷卡*/
	RD_SHOWEMPTY = 0x06				/**< 显示空闲状态*/
}ENM_READERSHOW;


typedef struct{
	char cOperFlag;        		/**<EM_RF_INSIDE表示使用内置，EM_RF_OUTSIDE表示使用外置,EM_RF_INSIDE|EM_RF_OUTSIDE表示使用内外置*/
	char cAux; 		       	/**<外接读卡器所接串口(内置不用)*/
	char cDefaultType;     	/**<默认外接读卡器类型(双频:RFIDREADER_ZX,RFIDREADER_SH,单频:ENM_RFREADERTYPE,RFIDREADER_ZX暂不支持2.4G M1操作)*/
	char cUsedDoubleRF;    	/**<是否使用读卡器的双频寻卡模式0:否，1是(外接读卡器专用)	(备注：读卡器需支持)*/		
	char cCardType;			/**<需要读取的卡片类型ENM_CARDTYPE*/
	char cIs829SupportM1;	/**<829模块是否支持M1卡0:不支持，其他:支持*/
	char *pszM1AID;			/**<对2.4G读头有效m1卡的应用AID,该值为NULL时采用电信集团默认AID*/
	char cFirst;				/**<2.4G优先轮询，0:M1卡优先，其他:CPU卡优先*/
}STRFIDCARDPARAM;

enum EM_CARDINPUTMODE
{
	CARD_STRIPE = (1<<0),		/**<刷磁卡*/
	CARD_KEYIN = (1<<1),		/**<手输卡号*/
	CARD_INSERTIC = (1<<2),		/**<插入IC卡*/
	CARD_RFCARD = (1<<3),		/**<挥非接卡*/
	CARD_STRIPE_KEYIN = CARD_STRIPE|CARD_KEYIN,
	CARD_STRIPE_INSERTIC = CARD_STRIPE|CARD_INSERTIC,
	CARD_STRIPE_RFCARD = CARD_RFCARD|CARD_STRIPE,
	CARD_STRIPE_KEYIN_INSERTIC = CARD_STRIPE_KEYIN|CARD_INSERTIC,
	CARD_STRIPE_KEYIN_RFCARD = CARD_STRIPE_KEYIN|CARD_RFCARD,
	CARD_STRIPE_KEYIN_INSERTIC_RFCARD = CARD_STRIPE_KEYIN_INSERTIC|CARD_RFCARD,
	CARD_STRIPE_INSERTIC_RFCARD = CARD_STRIPE_INSERTIC |CARD_RFCARD,
	CARD_KEYIN_INSERIC = CARD_KEYIN|CARD_INSERTIC,
	CARD_KEYIN_RFCARD = CARD_KEYIN|CARD_RFCARD,
	CARD_KEYIN_INSERIC_RFCARD = CARD_KEYIN_INSERIC|CARD_RFCARD,
	CARD_INSERTIC_RFCARD = CARD_INSERTIC|CARD_RFCARD
};

enum EM_MAINTK
{
	NOMAINTK = 0x00,							/**<没有优先道，只要有1个磁道对 	*/
	MAINTK1  = (1<<0),  			  				/**<一磁道为优先道              	*/
	MAINTK2  = (1<<1),  			   				/**<二磁道为优先道              	*/
	MAINTK3  = (1<<2),  			   				/**<三磁道为优先道              	*/
	MAINTK1_2 =   (MAINTK1 | MAINTK2),			/**<一、二磁道为优先道          	*/
	MAINTK1_3 =   (MAINTK1 | MAINTK3),			/**<一、三磁道为优先道          	*/
	MAINTK2_3 =   (MAINTK2 | MAINTK3), 			/**<二、三磁道为优先道          	*/
	MAINTK1_2_3 = (MAINTK1 | MAINTK2 | MAINTK3)	/**<一、二、三磁道为优先道      	*/
};

typedef struct
{
    uint    unFlag; 	         		/**<刷卡输入方式 EM_CARDINPUTMODE */
    uint    unMaintk;			/**<优先磁道 EM_MAINTK */
    uint    unInputMinLen;		/**<手工输入最小长度*/
	uint    unInputMaxLen;	/**<手工输入最大长度*/
    uint    unTimeout;			/**<刷卡超时,单位s*/
    uint    unSleepTime;		/**<需要休眠的时间,单位s,为0的表示不休眠*/
	uint    unFunctionKeyFlag;	/**<是否支持功能键,1支持，0不支持*/
}STREADCARDPARAM;

/**
* @brief 射频刷卡检测延时等待
* @detail PubPosReadCard函数同时支持磁卡和射频卡检测，当检测到射频卡时，延时nRfMsDelay毫秒等待检测磁卡
* @param [in] nRfMsDelay  延时时间(毫秒)推荐值500
* @return 
* @li 无
* @author linw
* @date 2014-03-07
*/
void PubSetReadRfDelay(int nRfMsDelay);

/**
* @brief 刷卡
* @detail 同时支持磁卡的读卡、手输卡号、IC卡插卡检测、射频卡检测
* @detail 需要检测射频卡的时候，需确保已经执行PubInitRFCardParam
* @param [in] pstReadCardParm  刷卡参数
* @param [out] pszTk1  一磁道数据
* @param [out] pszTk2  二磁道数据
* @param [out] pszTk3  三磁道数据
* @param [out] pnCardType  返回类型 CARD_STRIPE,CARD_KEYIN,CARD_INSERTIC,CARD_RFCARD,CARD_FUNCQUIT
               CARD_KEYIN    --- pszTk2 表示输入的内容
               CARD_FUNCQUIT --- pszTk2[0] 表示功能键键值
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-10-24
*/
int PubPosReadCard(const STREADCARDPARAM *pstReadCardParm,char *pszTk1, char *pszTk2, char *pszTk3, int *pnCardType);


/**
* @brief IC卡上电，设置IC卡类型。
* @param [in] nCardPort IC卡端口号
* @param [in] nCardType IC卡类型
* @param [out] psAtr  上电返回信息
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @li APP_QUIT 退出
* @author廖华仔
* @date 2012-6-1
*/
int PubICPowerUp (const int nCardPort,const int nCardType,char *psAtr);

/**
* @brief 获取IC卡状态
* @param [out] pszStatus  IC卡状态
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubGetICStatus(char *pszStatus);

/**
* @brief 获取IC卡状态
* @param [in] nCardPort  IC卡端口号
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubICPowerDown(const int nCardPort);

/**
* @brief IC/SIM卡标准输入输出函数。
* @param [in] nCardPort IC卡端口号
* @param [in] cCLA 命令代码
* @param [in] cINS 指令代码
* @param [in] cP1 附加参数1
* @param [in] cP2 附加参数2
* @param [in] ucInLen 输入数据长度
* @param [in] psInData 输入数据
* @param [in] nLe 期待响应数据长度
* @param [out] pucOutLen  返回数据长度
* @param [out] psOutData  返回数据
* @param [out] pszSW1  返回状态1
* @param [out] pszSW2  返回状态2
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubICComm (const int nCardPort, const char cCLA, const char cINS,
			   const char cP1, const char cP2, const uchar ucInLen,
			   const char *psInData, int nLe, uchar *pucOutLen, 
			  char *psOutData, char *pszSW1, char *pszSW2);

/**
* @brief 初始化射频卡读卡模块，自适应波特率设置。
* @param [in] pstRFIDCardParam 模块参数,请参考STRFIDCARDPARAM结构体定义
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubInitRFCardParam (const STRFIDCARDPARAM * pstRFIDCardParam);

/**
* @brief 非接卡(CPU卡)上电
* @param [out] psUID  卡序列号,第一个字节为序列号长度
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @li APP_QUIT 退出
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFCPUPowerUp (char *psUID);

/**
* @brief 非接卡(CPU卡)下电
* @param 无
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFCPUPowerDown (void);

/**
* @brief 与非接卡(CPU卡)进行通讯，使用APDU。
* @param [in] psSend  发送的数据缓冲区
* @param [in] nSendLen  发送数据长度
* @param [out] psRecv  接收数据缓冲区
* @param [out]  pnRecvLen  接收数据的长度
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFCPUComm(char *psSend, int nSendLen,char *psRecv, int *pnRecvLen);

/**
* @brief 寻卡(非接射频卡，找到卡片，上电)，此处寻找在初始化参数设定中的卡种类，参考PubInitRFCardParam。
* @param [out] psUid  读取的卡序列号
* @param [out]  pnType  寻找到的非接卡类型标记 ---ENM_RFIDCARDTYPE
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFSeekCard(char* psUid,ENM_RFIDCARDTYPE *pnType);

/**
* @brief 释放卡(M1卡专用)。
* @param 无
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFFreeCard(void);

/**
* @brief 存储扇区密钥(M1卡专用)。
* @param [in] ucKeySector  扇区号,1K M1卡扇区号[0-15],4K M1卡扇区号[0-40],库里面未做限制。					
* @param [in]  psAKey  A密钥(6个字节)
* @param [in]  psBKey  B密钥(6个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFStoreKey(uchar ucKeySector,const char * psAKey,const char * psBKey);

/**
* @brief 卡片扇区认证,与PubRFStoreKey配套使用,(M1卡专用)。
* @param [in] ucKeySector  扇区号,1K M1卡扇区号[0-15],4K M1卡扇区号[0-40],库里面未做限制。					
* @param [in]  ucKeyMode  密钥模式---ENM_RFIDM1KEYMODE
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFAuthCard(uchar ucKeySector,uchar ucKeyMode);

/**
* @brief 卡片扇区认证(自带认证密钥),(M1卡专用)。
* @param [in] ucKeySector  扇区号,1K M1卡扇区号[0-15],4K M1卡扇区号[0-40],库里面未做限制。					
* @param [in]  ucKeyMode  密钥模式---ENM_RFIDM1KEYMODE
* @param [in]  psKey  密钥(6个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFAuthByKey(uchar ucKeySector,uchar ucKeyMode,const char * psKey);

/**
* @brief 卡片块号写数据,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @param [in]  psData  待写数据(16个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFWrite(uchar ucBlockNo,const char *psData);

/**
* @brief 卡片块号读数据,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @param [in]  psData  待读数据(16个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFRead(uchar ucBlockNo,char *psData);

/**
* @brief 卡片块号增量数据,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @param [in]  psData  待增量的数据(4个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFInc(uchar ucBlockNo,const char *psData);

/**
* @brief 卡片块号减量操作,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @param [in]  psData  待减量的数据(4个字节)
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFDec(uchar ucBlockNo,const char *psData);

/**
* @brief 卡片块号传送操作,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFTransfer(uchar ucBlockNo);

/**
* @brief 卡片块号复制(恢复)操作,(M1卡专用)。
* @param [in] ucBlockNo  块号,1K M1卡扇区号[0-3],4K M1卡扇区号[0-15],库里面未做限制。					
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubRFReStore(uchar ucBlockNo);

/**
* @brief 取错误代码。
* @param 无					
* @return 
* @li 错误代码
* @author 廖华仔
* @date 2012-6-1
*/
int PubGetCardErrorCode(void);

/**
* @brief 取外接读卡器版本,30+30:前30字节为829版本,后30为默认读卡器版本。
* @param [out] pszVer  版本号，缓冲区必须大于60。					
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
void PubGetReaderVer(char *pszVer);

/**
* @brief 选择只读CPU卡、M1卡，或都两个都支持。
* @param [in] eCardType  卡类型---ENM_CARDTYPE。					
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
void PubSelectCardType(ENM_CARDTYPE eCardType);

/**
* @brief 2.4G优先轮询。
* @param [in] cRf24First  2.4G优先轮询,0:M1卡优先，其他:CPU卡优先。					
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
void PubRf24First(char cRf24First);

/**
* @brief 非接触式读卡器显示函数。
* @param [in] cFlag  显示类型---ENM_READERSHOW
* @param [in] pszAmount  显示金额 12字节
* @param [in] pszBalance  显示余额 12字节				
* @return 
* @li <0 失败
* @li APP_SUCC 成功
* @author 廖华仔
* @date 2012-6-1
*/
int PubReadCardDisp(char cFlag,const char* pszAmount,const char* pszBalance );

/**
* @brief 获取寻到的卡片频率类型，13.56或2.4G。
* @param 无				
* @return 
* @li APP_FAIL  失败
* @li 0  13.56M
* @li 1  2.4G
* @author 廖华仔
* @date 2012-6-1
*/
int PubGetRfCardType(void);

/**
* @brief 取库版本号
* @param [out] 	psVer 版本号(12个字节)		
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubGetCardLibVer(char* psVer);

/**
* @brief 上电(根据寻卡类型)
* @param [out] 	pszRes 
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubCardPowerUp(char *pszRes);

/**
* @brief 下电(根据寻卡类型)
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubCardPowerDown();

/**
* @brief 收发指令(根据寻卡类型，指令已封装好)
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubCardrw(const int nCommandLen, const char *pszCommand, int *pnLen, char *pszResponse);

/**
* @brief 卡状态(根据寻卡类型)
* @return 
* @li APP_FAIL 无卡
* @li APP_SUCC 有卡
* @author 廖华仔
* @date 2012-10-31
*/
int PubGetCardStatus();

/**
* @brief 收发指令(IC卡，指令已封装好)
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubICrw(const int nCardPort, const int nCommandLen, const char *pszCommand, int *pnLen, char *pszResponse);

/**
* @brief 非接触式卡beep函数
* @param [in]  nDelayTime --- 蜂鸣器鸣叫时长，单位ms 
* @param [in]  nTimes --- 鸣叫次数
* @param [in]  nInterval --- 间隔，单位ms
* @return 
* @li 无
* @author 廖华仔
* @date 2012-6-1
*/
int PubRfReaderBeep(int nDelayTime, int nTimes,int nInterval);

/**
* @brief 开启非阻塞刷卡
* @param [in] unFlag 刷卡输入方式 EM_CARDINPUTMODE 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
*/
int PubReadCardNonBlockStart(uint unFlag);


/**
* @brief 非阻塞刷卡
* @detail 同时支持磁卡的读卡、手输卡号、IC卡插卡检测、射频卡检测
* @detail 需要检测射频卡的时候，需确保已经执行PubInitRFCardParam
* @param [in] pstReadCardParm  刷卡参数
* @param [out] pszTk1  一磁道数据
* @param [out] pszTk2  二磁道数据
* @param [out] pszTk3  三磁道数据
* @param [out] pnCardType  返回类型 CARD_STRIPE,CARD_KEYIN,CARD_INSERTIC,CARD_RFCARD,CARD_FUNCQUIT
               CARD_KEYIN    --- pszTk2 表示输入的内容
               CARD_FUNCQUIT --- pszTk2[0] 表示功能键键值
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
*/
int PubReadCardNonBlock(const STREADCARDPARAM *pstReadCardParm,
						char *pszTk1, char *pszTk2, char *pszTk3, int *pnCardType);


/**
* @brief 关闭非阻塞刷卡
* @param [in] unFlag 刷卡输入方式 EM_CARDINPUTMODE 
* @return 
* @li APP_FAIL 失败
* @li APP_SUCC 成功
*/
int PubReadCardNonBlockEnd(uint unFlag);


#endif
/* End of lposcard.h*/
