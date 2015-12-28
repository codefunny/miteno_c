/**
* @file fram.h
* @brief FRAM定义
* @version  1.0
* @author 戴建斌
* @date 2007-02-05
*/
#ifndef _FRAM_H_
#define _FRAM_H_

/**
* 定义fram空间数据保存偏移及长度
*/

/**
* 银行参数
*/
#define fNVR_Addr_Start				0

/**<版本号，为了判断本版本是否第一次运行*/
#define fPosCheck_off				fNVR_Addr_Start
#define fPosCheck_len				8
/**<POS类型，酒店、商场、转账等*/
#define fPosMerType_off				fPosCheck_off+fPosCheck_len
#define fPosMerType_len				1
/**<回返报文头*/
#define fRetCommHead_off			fPosMerType_off+fPosMerType_len	
#define fRetCommHead_len			6
/**<签到标志*/
#define fIsLogin_off				fRetCommHead_off+fRetCommHead_len	
#define fIsLogin_len				1
/**<锁定标志*/
#define fIsLock_off					fIsLogin_off+fIsLogin_len
#define fIsLock_len					1
/**<流水号*/
#define fTraceNo_off				fIsLock_off+fIsLock_len
#define fTraceNo_len				6
/**<批次号*/
#define fBatchNo_off				fTraceNo_off+fTraceNo_len
#define fBatchNo_len				6
/**<票据号*/
#define fInvoiceNo_off				fBatchNo_off+fBatchNo_len
#define fInvoiceNo_len				6
/**<拨通次数unsigned long*/
#define fDialSuccSum_off			fInvoiceNo_off+fInvoiceNo_len
#define fDialSuccSum_len			8
/**<拨号次数unsigned long*/
#define fDialSum_off				fDialSuccSum_off+fDialSuccSum_len
#define fDialSum_len				8
/**<时间计数器long*/
#define fTimeCounter_off			fDialSum_off+fDialSum_len
#define fTimeCounter_len			4
/**<上回操作员登录时间*/
#define fLastLoginDateTime_off		fTimeCounter_off+fTimeCounter_len
#define fLastLoginDateTime_len		5
/**<上回登陆的操作员号码*/
#define fLastLoginOperNo_off		fLastLoginDateTime_off+fLastLoginDateTime_len
#define fLastLoginOperNo_len		2
/**<上回登陆的操作员属性*/
#define fLastLoginOperRole_off		fLastLoginOperNo_off+fLastLoginOperNo_len
#define fLastLoginOperRole_len		1
/**<主控登录的时间因子*/
#define fMC_EnterDateTime_off		fLastLoginOperRole_off+fLastLoginOperRole_len
#define fMC_EnterDateTime_len		4
/**<主控登录时的随机数*/
#define fMC_EnterRandom_off			fMC_EnterDateTime_off+fMC_EnterDateTime_len
#define fMC_EnterRandom_len			4

/**
* 冲正数据 
*/
/**<冲正标识*/
#define fIsReversal_off				fMC_EnterRandom_off+fMC_EnterRandom_len
#define fIsReversal_len				1
/**<已冲正次数*/
#define fHaveReversalNum_off		fIsReversal_off+fIsReversal_len
#define fHaveReversalNum_len		4		
/**<冲正数据长度*/
#define fReversalLen_off			fHaveReversalNum_off+fHaveReversalNum_len
#define fReversalLen_len			4
/**<冲正数据*/
#define fReversalData_off			fReversalLen_off+fReversalLen_len
#define fReversalData_len			600

/**
* 结算数据
*/
/**<流水笔数*/
#define fWaterSum_off				fReversalData_off+fReversalData_len
#define fWaterSum_len				4
/**<离线交易已上送次数*/		
#define fHaveReSendNum_off			fWaterSum_off+fWaterSum_len
#define fHaveReSendNum_len			4
/**<结算单打印中断标识*/
#define fSettlePrintHaltFlag_off	fHaveReSendNum_off+fHaveReSendNum_len
#define fSettlePrintHaltFlag_len	1
/**<打印明细中断标识*/
#define fPrintDetialHaltFlag_off	fSettlePrintHaltFlag_off+fSettlePrintHaltFlag_len
#define fPrintDetialHaltFlag_len	1
/**<打印签购单中断标识*/
#define fPrintWaterHaltFlag_off		fPrintDetialHaltFlag_off+fPrintDetialHaltFlag_len
#define fPrintWaterHaltFlag_len		1
/**<内卡结算帐平标识*/
#define fCnCardFlag_off				fPrintWaterHaltFlag_off+fPrintWaterHaltFlag_len
#define fCnCardFlag_len				1
/**<外卡结算帐平标识*/
#define fEnCardFlag_off				fCnCardFlag_off+fCnCardFlag_len
#define fEnCardFlag_len				1
/**<批上送中断标志，0-没有中断,1金融交易中断，2通知类中断, 3圈存上送中断*/
#define fBatchHaltFlag_off			fEnCardFlag_off+fEnCardFlag_len
#define fBatchHaltFlag_len			1
/**<清除结算数据中断标识*/
#define fClrSettleDataFlag_off		fBatchHaltFlag_off+fBatchHaltFlag_len
#define fClrSettleDataFlag_len		1
/**<金融类批上送中断标识，记录笔数*/
#define fFinanceHaltNum_off			fClrSettleDataFlag_off+fClrSettleDataFlag_len
#define fFinanceHaltNum_len			4
/**<通知类批上送中断标识，记录笔数*/
#define fMessageHaltNum_off			fFinanceHaltNum_off+fFinanceHaltNum_len
#define fMessageHaltNum_len			4
/**<磁条卡离线类批上送中断标识，记录笔数*/
#define fBatchMagOfflineHaltNum_off	fMessageHaltNum_off+fMessageHaltNum_len
#define fBatchMagOfflineHaltNum_len	4
/**<圈存类批上送中断标识，记录笔数*/
#define fIcPurchaseHaltNum_off		fBatchMagOfflineHaltNum_off+fBatchMagOfflineHaltNum_len
#define fIcPurchaseHaltNum_len		4
/**<圈存类批上送中断标识，记录笔数*/
#define fIcLoadHaltNum_off			fIcPurchaseHaltNum_off+fIcPurchaseHaltNum_len
#define fIcLoadHaltNum_len			4
/**<批上送总笔数*/
#define fBatchUpSum_off				fIcLoadHaltNum_off+fIcLoadHaltNum_len
#define fBatchUpSum_len				4
/**<结算时间*/
#define fSettleDateTime_off			fBatchUpSum_off+fBatchUpSum_len
#define fSettleDateTime_len			5
/**<结算数据，笔数N4＋金额N8，可存储30种交易*/
#define fSettleData_off				fSettleDateTime_off+fSettleDateTime_len
#define fSettleData_len				600

//140913chenwu 以下205字节的空间是原来给电子钱包用的，现已经删除电子钱包，所以可以用在其他地方
/**
*EMV数据
*/
/**<EMV交易序号*/
#define fEmvTransSerial_off			fSettleData_off+fSettleData_len+205
#define fEmvTransSerial_len			4
/**<EMV脱机交易上送标识*/
#define fEmvOfflineUpNum_off		fEmvTransSerial_off+fEmvTransSerial_len
#define fEmvOfflineUpNum_len		4
/**<EMV联机交易上送标识*/
#define fEmvOnlineUpNum_off			fEmvOfflineUpNum_off+fEmvOfflineUpNum_len
#define fEmvOnlineUpNum_len			4
/**<EMV脱机失败交易上送标识*/
#define fEmvOfflineFailUpNum_off	fEmvOnlineUpNum_off+fEmvOnlineUpNum_len
#define fEmvOfflineFailUpNum_len	4
/**<EMV联机APRC错误交易上送标识*/
#define fEmvOnlineARPCErrUpNum_off	fEmvOfflineFailUpNum_off+fEmvOfflineFailUpNum_len
#define fEmvOnlineARPCErrUpNum_len	4
/**<是否需要下载(EMV公钥)*/
#define fEmvIsDownCAPKFlag_off		fEmvOnlineARPCErrUpNum_off+fEmvOnlineARPCErrUpNum_len
#define fEmvIsDownCAPKFlag_len		1
/**<是否需要下载(EMV的ic卡参数)*/
#define fEmvIsDownAIDFlag_off		fEmvIsDownCAPKFlag_off+fEmvIsDownCAPKFlag_len
#define fEmvIsDownAIDFlag_len		1
/**<脚本处理通知标识*/
#define fEmvIsScriptInform_off		fEmvIsDownAIDFlag_off+fEmvIsDownAIDFlag_len
#define fEmvIsScriptInform_len		1
/**<脚本处理数据*/
#define fEmvScriptData_off			fEmvIsScriptInform_off+fEmvIsScriptInform_len
#define fEmvScriptData_len			256

#define fIsPhoneSale_off			fEmvScriptData_off+fEmvScriptData_len
#define fIsPhoneSale_len			1

//140913chenwu 以下55字节的空间是原来给读卡器用的，现已经删除电子钱包，所以可以用在其他地方
/**<下传终端参数*/
#define fIsPosParamDown_off			fIsPhoneSale_off+fIsPhoneSale_len
#define fIsPosParamDown_len			1

/**<上传终端状态信息*/
#define fIsPosStatusSend_off		fIsPosParamDown_off+fIsPosParamDown_len
#define fIsPosStatusSend_len		1

/**<重新签到*/
#define fIsReLogin_off				fIsPosStatusSend_off+fIsPosStatusSend_len
#define fIsReLogin_len				1

/**<TMS参数下载*/
#define fIsTmsDown_off				fIsReLogin_off+fIsReLogin_len
#define fIsTmsDown_len				1

/**<黑名单下载*/
#define fIsBlackListDown_off		fIsTmsDown_off+fIsTmsDown_len
#define fIsBlackListDown_len		1

/**TMS下载失败次数*/
#define fTmsFailNum_off				fIsBlackListDown_off+fIsBlackListDown_len
#define fTmsFailNum_len				1

/**TMS任务取消次数*/
#define fTmsEscNum_off  	        fTmsFailNum_off+fTmsFailNum_len
#define fTmsEscNum_len 	    	    1

/**电子签名未上送笔数*/
#define fElecSignUnSendNum_off  	fTmsEscNum_off+fTmsEscNum_len
#define fElecSignUnSendNum_len  	4

#define fOfflineUnSendNum_off		fElecSignUnSendNum_off+fElecSignUnSendNum_len+44
#define fOfflineUnSendNum_len		4

/**舍弃*/
#define fIsEmvAidDown_off			fOfflineUnSendNum_off+fOfflineUnSendNum_len
#define fIsEmvAidDown_len			1

/**舍弃*/
#define fIsEmvCapkDown_off			fIsEmvAidDown_off+fIsEmvAidDown_len
#define fIsEmvCapkDown_len			1

/**冲正是否上送9F36*/
#define fIsReversalPack9F36_off 	fIsEmvCapkDown_off+fIsEmvCapkDown_len
#define fIsReversalPack9F36_len 	1

/**<已上送脚本通知次数*/
#define fHaveScriptAdviseNum_off	fIsReversalPack9F36_off+fIsReversalPack9F36_len
#define fHaveScriptAdviseNum_len	1		

/**用于版本升级*/
#define fUpdateCtrl_off				fHaveScriptAdviseNum_off+fHaveScriptAdviseNum_len
#define fUpdateCtrl_len 		    2


/**KEK下载*/
#define fIsDownKek_off			fUpdateCtrl_off+fUpdateCtrl_len
#define fIsDownKek_len		1


#define fNVR_Addr_End				fIsDownKek_off+fIsDownKek_len

#define fMAX_FRAM_SIZE 				2000

#if fNVR_Addr_End > (fNVR_Addr_Start+fMAX_FRAM_SIZE)
#error Define NVR Err!
#endif

#endif
