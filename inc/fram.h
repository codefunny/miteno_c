/**
* @file fram.h
* @brief FRAM����
* @version  1.0
* @author ������
* @date 2007-02-05
*/
#ifndef _FRAM_H_
#define _FRAM_H_

/**
* ����fram�ռ����ݱ���ƫ�Ƽ�����
*/

/**
* ���в���
*/
#define fNVR_Addr_Start				0

/**<�汾�ţ�Ϊ���жϱ��汾�Ƿ��һ������*/
#define fPosCheck_off				fNVR_Addr_Start
#define fPosCheck_len				8
/**<POS���ͣ��Ƶꡢ�̳���ת�˵�*/
#define fPosMerType_off				fPosCheck_off+fPosCheck_len
#define fPosMerType_len				1
/**<�ط�����ͷ*/
#define fRetCommHead_off			fPosMerType_off+fPosMerType_len	
#define fRetCommHead_len			6
/**<ǩ����־*/
#define fIsLogin_off				fRetCommHead_off+fRetCommHead_len	
#define fIsLogin_len				1
/**<������־*/
#define fIsLock_off					fIsLogin_off+fIsLogin_len
#define fIsLock_len					1
/**<��ˮ��*/
#define fTraceNo_off				fIsLock_off+fIsLock_len
#define fTraceNo_len				6
/**<���κ�*/
#define fBatchNo_off				fTraceNo_off+fTraceNo_len
#define fBatchNo_len				6
/**<Ʊ�ݺ�*/
#define fInvoiceNo_off				fBatchNo_off+fBatchNo_len
#define fInvoiceNo_len				6
/**<��ͨ����unsigned long*/
#define fDialSuccSum_off			fInvoiceNo_off+fInvoiceNo_len
#define fDialSuccSum_len			8
/**<���Ŵ���unsigned long*/
#define fDialSum_off				fDialSuccSum_off+fDialSuccSum_len
#define fDialSum_len				8
/**<ʱ�������long*/
#define fTimeCounter_off			fDialSum_off+fDialSum_len
#define fTimeCounter_len			4
/**<�ϻز���Ա��¼ʱ��*/
#define fLastLoginDateTime_off		fTimeCounter_off+fTimeCounter_len
#define fLastLoginDateTime_len		5
/**<�ϻص�½�Ĳ���Ա����*/
#define fLastLoginOperNo_off		fLastLoginDateTime_off+fLastLoginDateTime_len
#define fLastLoginOperNo_len		2
/**<�ϻص�½�Ĳ���Ա����*/
#define fLastLoginOperRole_off		fLastLoginOperNo_off+fLastLoginOperNo_len
#define fLastLoginOperRole_len		1
/**<���ص�¼��ʱ������*/
#define fMC_EnterDateTime_off		fLastLoginOperRole_off+fLastLoginOperRole_len
#define fMC_EnterDateTime_len		4
/**<���ص�¼ʱ�������*/
#define fMC_EnterRandom_off			fMC_EnterDateTime_off+fMC_EnterDateTime_len
#define fMC_EnterRandom_len			4

/**
* �������� 
*/
/**<������ʶ*/
#define fIsReversal_off				fMC_EnterRandom_off+fMC_EnterRandom_len
#define fIsReversal_len				1
/**<�ѳ�������*/
#define fHaveReversalNum_off		fIsReversal_off+fIsReversal_len
#define fHaveReversalNum_len		4		
/**<�������ݳ���*/
#define fReversalLen_off			fHaveReversalNum_off+fHaveReversalNum_len
#define fReversalLen_len			4
/**<��������*/
#define fReversalData_off			fReversalLen_off+fReversalLen_len
#define fReversalData_len			600

/**
* ��������
*/
/**<��ˮ����*/
#define fWaterSum_off				fReversalData_off+fReversalData_len
#define fWaterSum_len				4
/**<���߽��������ʹ���*/		
#define fHaveReSendNum_off			fWaterSum_off+fWaterSum_len
#define fHaveReSendNum_len			4
/**<���㵥��ӡ�жϱ�ʶ*/
#define fSettlePrintHaltFlag_off	fHaveReSendNum_off+fHaveReSendNum_len
#define fSettlePrintHaltFlag_len	1
/**<��ӡ��ϸ�жϱ�ʶ*/
#define fPrintDetialHaltFlag_off	fSettlePrintHaltFlag_off+fSettlePrintHaltFlag_len
#define fPrintDetialHaltFlag_len	1
/**<��ӡǩ�����жϱ�ʶ*/
#define fPrintWaterHaltFlag_off		fPrintDetialHaltFlag_off+fPrintDetialHaltFlag_len
#define fPrintWaterHaltFlag_len		1
/**<�ڿ�������ƽ��ʶ*/
#define fCnCardFlag_off				fPrintWaterHaltFlag_off+fPrintWaterHaltFlag_len
#define fCnCardFlag_len				1
/**<�⿨������ƽ��ʶ*/
#define fEnCardFlag_off				fCnCardFlag_off+fCnCardFlag_len
#define fEnCardFlag_len				1
/**<�������жϱ�־��0-û���ж�,1���ڽ����жϣ�2֪ͨ���ж�, 3Ȧ�������ж�*/
#define fBatchHaltFlag_off			fEnCardFlag_off+fEnCardFlag_len
#define fBatchHaltFlag_len			1
/**<������������жϱ�ʶ*/
#define fClrSettleDataFlag_off		fBatchHaltFlag_off+fBatchHaltFlag_len
#define fClrSettleDataFlag_len		1
/**<�������������жϱ�ʶ����¼����*/
#define fFinanceHaltNum_off			fClrSettleDataFlag_off+fClrSettleDataFlag_len
#define fFinanceHaltNum_len			4
/**<֪ͨ���������жϱ�ʶ����¼����*/
#define fMessageHaltNum_off			fFinanceHaltNum_off+fFinanceHaltNum_len
#define fMessageHaltNum_len			4
/**<�������������������жϱ�ʶ����¼����*/
#define fBatchMagOfflineHaltNum_off	fMessageHaltNum_off+fMessageHaltNum_len
#define fBatchMagOfflineHaltNum_len	4
/**<Ȧ�����������жϱ�ʶ����¼����*/
#define fIcPurchaseHaltNum_off		fBatchMagOfflineHaltNum_off+fBatchMagOfflineHaltNum_len
#define fIcPurchaseHaltNum_len		4
/**<Ȧ�����������жϱ�ʶ����¼����*/
#define fIcLoadHaltNum_off			fIcPurchaseHaltNum_off+fIcPurchaseHaltNum_len
#define fIcLoadHaltNum_len			4
/**<�������ܱ���*/
#define fBatchUpSum_off				fIcLoadHaltNum_off+fIcLoadHaltNum_len
#define fBatchUpSum_len				4
/**<����ʱ��*/
#define fSettleDateTime_off			fBatchUpSum_off+fBatchUpSum_len
#define fSettleDateTime_len			5
/**<�������ݣ�����N4�����N8���ɴ洢30�ֽ���*/
#define fSettleData_off				fSettleDateTime_off+fSettleDateTime_len
#define fSettleData_len				600

//140913chenwu ����205�ֽڵĿռ���ԭ��������Ǯ���õģ����Ѿ�ɾ������Ǯ�������Կ������������ط�
/**
*EMV����
*/
/**<EMV�������*/
#define fEmvTransSerial_off			fSettleData_off+fSettleData_len+205
#define fEmvTransSerial_len			4
/**<EMV�ѻ��������ͱ�ʶ*/
#define fEmvOfflineUpNum_off		fEmvTransSerial_off+fEmvTransSerial_len
#define fEmvOfflineUpNum_len		4
/**<EMV�����������ͱ�ʶ*/
#define fEmvOnlineUpNum_off			fEmvOfflineUpNum_off+fEmvOfflineUpNum_len
#define fEmvOnlineUpNum_len			4
/**<EMV�ѻ�ʧ�ܽ������ͱ�ʶ*/
#define fEmvOfflineFailUpNum_off	fEmvOnlineUpNum_off+fEmvOnlineUpNum_len
#define fEmvOfflineFailUpNum_len	4
/**<EMV����APRC���������ͱ�ʶ*/
#define fEmvOnlineARPCErrUpNum_off	fEmvOfflineFailUpNum_off+fEmvOfflineFailUpNum_len
#define fEmvOnlineARPCErrUpNum_len	4
/**<�Ƿ���Ҫ����(EMV��Կ)*/
#define fEmvIsDownCAPKFlag_off		fEmvOnlineARPCErrUpNum_off+fEmvOnlineARPCErrUpNum_len
#define fEmvIsDownCAPKFlag_len		1
/**<�Ƿ���Ҫ����(EMV��ic������)*/
#define fEmvIsDownAIDFlag_off		fEmvIsDownCAPKFlag_off+fEmvIsDownCAPKFlag_len
#define fEmvIsDownAIDFlag_len		1
/**<�ű�����֪ͨ��ʶ*/
#define fEmvIsScriptInform_off		fEmvIsDownAIDFlag_off+fEmvIsDownAIDFlag_len
#define fEmvIsScriptInform_len		1
/**<�ű���������*/
#define fEmvScriptData_off			fEmvIsScriptInform_off+fEmvIsScriptInform_len
#define fEmvScriptData_len			256

#define fIsPhoneSale_off			fEmvScriptData_off+fEmvScriptData_len
#define fIsPhoneSale_len			1

//140913chenwu ����55�ֽڵĿռ���ԭ�����������õģ����Ѿ�ɾ������Ǯ�������Կ������������ط�
/**<�´��ն˲���*/
#define fIsPosParamDown_off			fIsPhoneSale_off+fIsPhoneSale_len
#define fIsPosParamDown_len			1

/**<�ϴ��ն�״̬��Ϣ*/
#define fIsPosStatusSend_off		fIsPosParamDown_off+fIsPosParamDown_len
#define fIsPosStatusSend_len		1

/**<����ǩ��*/
#define fIsReLogin_off				fIsPosStatusSend_off+fIsPosStatusSend_len
#define fIsReLogin_len				1

/**<TMS��������*/
#define fIsTmsDown_off				fIsReLogin_off+fIsReLogin_len
#define fIsTmsDown_len				1

/**<����������*/
#define fIsBlackListDown_off		fIsTmsDown_off+fIsTmsDown_len
#define fIsBlackListDown_len		1

/**TMS����ʧ�ܴ���*/
#define fTmsFailNum_off				fIsBlackListDown_off+fIsBlackListDown_len
#define fTmsFailNum_len				1

/**TMS����ȡ������*/
#define fTmsEscNum_off  	        fTmsFailNum_off+fTmsFailNum_len
#define fTmsEscNum_len 	    	    1

/**����ǩ��δ���ͱ���*/
#define fElecSignUnSendNum_off  	fTmsEscNum_off+fTmsEscNum_len
#define fElecSignUnSendNum_len  	4

#define fOfflineUnSendNum_off		fElecSignUnSendNum_off+fElecSignUnSendNum_len+44
#define fOfflineUnSendNum_len		4

/**����*/
#define fIsEmvAidDown_off			fOfflineUnSendNum_off+fOfflineUnSendNum_len
#define fIsEmvAidDown_len			1

/**����*/
#define fIsEmvCapkDown_off			fIsEmvAidDown_off+fIsEmvAidDown_len
#define fIsEmvCapkDown_len			1

/**�����Ƿ�����9F36*/
#define fIsReversalPack9F36_off 	fIsEmvCapkDown_off+fIsEmvCapkDown_len
#define fIsReversalPack9F36_len 	1

/**<�����ͽű�֪ͨ����*/
#define fHaveScriptAdviseNum_off	fIsReversalPack9F36_off+fIsReversalPack9F36_len
#define fHaveScriptAdviseNum_len	1		

/**���ڰ汾����*/
#define fUpdateCtrl_off				fHaveScriptAdviseNum_off+fHaveScriptAdviseNum_len
#define fUpdateCtrl_len 		    2


/**KEK����*/
#define fIsDownKek_off			fUpdateCtrl_off+fUpdateCtrl_len
#define fIsDownKek_len		1


#define fNVR_Addr_End				fIsDownKek_off+fIsDownKek_len

#define fMAX_FRAM_SIZE 				2000

#if fNVR_Addr_End > (fNVR_Addr_Start+fMAX_FRAM_SIZE)
#error Define NVR Err!
#endif

#endif
