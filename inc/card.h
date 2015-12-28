/**
* @file card.h
* @brief 卡处理模块
* @version  1.0
* @author 张捷
* @date 2007-01-22
*/

#ifndef _CARD_H_
#define _CARD_H_

#define MAX_PAN 19
#define MAX_TK1 100
#define MAX_TK2 37
#define MAX_TK3 104

#define MIN_PAN 13
#define MIN_TK1 MIN_PAN
#define MIN_TK2 MIN_PAN
#define MIN_TK3 MIN_PAN

#define KJ_MIN_TK2		21			/**linjz- 2012-09-14卡检测中心特殊要求*/

enum CARDINPUTMODE
{
	INPUT_NO = 0,
	INPUT_STRIPE = (1<<0),		   /**<刷磁卡*/
	INPUT_KEYIN = (1<<1),		   /**<手输卡号*/
	INPUT_INSERTIC = (1<<2),	   /**<插入IC卡*/
	INPUT_RFCARD = (1<<3),		   /**<挥电子现金卡*/	
	INPUT_UPCARD = (1<<4),		   /**<挥手机芯片卡*/
	INPUT_STRIPE_KEYIN = INPUT_STRIPE|INPUT_KEYIN,
	INPUT_STRIPE_INSERTIC = INPUT_STRIPE|INPUT_INSERTIC,
	INPUT_STRIPE_RFCARD = INPUT_RFCARD|INPUT_STRIPE,
	INPUT_STRIPE_KEYIN_INSERTIC = INPUT_STRIPE_KEYIN|INPUT_INSERTIC,
	INPUT_STRIPE_KEYIN_RFCARD = INPUT_STRIPE_KEYIN|INPUT_RFCARD,
	INPUT_STRIPE_KEYIN_INSERTIC_RFCARD = INPUT_STRIPE_KEYIN_INSERTIC|INPUT_RFCARD,
	INPUT_STRIPE_INSERTIC_RFCARD = INPUT_STRIPE_INSERTIC |INPUT_RFCARD,
	INPUT_KEYIN_INSERIC = INPUT_KEYIN|INPUT_INSERTIC,
	INPUT_KEYIN_RFCARD = INPUT_KEYIN|INPUT_RFCARD,
	INPUT_KEYIN_INSERIC_RFCARD = INPUT_KEYIN_INSERIC|INPUT_RFCARD,
	INPUT_INSERTIC_RFCARD = INPUT_INSERTIC|INPUT_RFCARD,
	INPUT_PLEASE_INSERTIC = 80,
	INPUT_STRIPE_FALLBACK,
	INPUT_STRIPE_TRANSFER_IN,
	INPUT_STRIPE_TRANSFER_OUT,
	INPUT_STRIPE_INSERTIC_TRANSFER_OUT,
	INPUT_INSERTIC_FORCE,
};

/**
* 以下为接口函数
*/

extern int CardTaskAmt(const char *, const int, const uint, int *);
extern int ProInputCard(int *pnInputMode, STSYSTEM *pstSystem);
extern int GetPan(char *);
extern int GetTrack(char *, char *, char *);
extern int DispPan(const char *, int);
extern int ChkIsICC(const char *, char *);
extern int GetServiceCode24FromTk2(const char *, char *);
extern void GetStripeType(const char* , char, char, char *);
extern int CtrlCardPan(uchar *, const uint, const uchar, const uint);
extern int SwipeCard(const int, const uint, int *);
extern void DisableDispDefault(void);
#endif

