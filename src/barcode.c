/*
 * library.c -- external functions of libbarcode
 *
 * Copyright (c) 1999 Alessandro Rubini (rubini@gnu.org)
 * Copyright (c) 1999 Prosa Srl. (prosa@prosa.it)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H /* sometimes (windows, for instance) it's missing */
#  include <unistd.h>
#endif
#include <errno.h>

#include "lui.h"
#include "barcode.h"

/*
 * This function allocates a barcode structure and strdup()s the
 * text string. It returns NULL in case of error
 */
struct Barcode_Item *Barcode_Create(char *text)
{
    struct Barcode_Item *bc;

    bc = malloc(sizeof(*bc));
    if (!bc) return NULL;

    memset(bc, 0, sizeof(*bc));
    bc->ascii = strdup(text);
    bc->margin = BARCODE_DEFAULT_MARGIN; /* default margin */
    return bc;
}


/*
 * Free a barcode structure
 */
int Barcode_Delete(struct Barcode_Item *bc)
{
    if (bc->ascii)
	free(bc->ascii);
    if (bc->partial)
	free(bc->partial);
    if (bc->textinfo)
	free(bc->textinfo);
    if (bc->encoding)
	free(bc->encoding);
    free(bc);
    return 0; /* always success */
}


/*
 * The various supported encodings.  This might be extended to support
 * dynamic addition of extra encodings
 */
//extern int Barcode_ean_verify(unsigned char *text);
//extern int Barcode_ean_encode(struct Barcode_Item *bc);
//extern int Barcode_upc_verify(unsigned char *text);
//extern int Barcode_upc_encode(struct Barcode_Item *bc);
//extern int Barcode_isbn_verify(unsigned char *text);
//extern int Barcode_isbn_encode(struct Barcode_Item *bc);
//extern int Barcode_39_verify(unsigned char *text);
//extern int Barcode_39_encode(struct Barcode_Item *bc);
extern int Barcode_128b_verify(unsigned char *text);
extern int Barcode_128b_encode(struct Barcode_Item *bc);
extern int Barcode_128c_verify(unsigned char *text);
extern int Barcode_128c_encode(struct Barcode_Item *bc);
extern int Barcode_128_verify(unsigned char *text);
extern int Barcode_128_encode(struct Barcode_Item *bc);
//extern int Barcode_128raw_verify(unsigned char *text);
//extern int Barcode_128raw_encode(struct Barcode_Item *bc);
//extern int Barcode_i25_verify(unsigned char *text);
//extern int Barcode_i25_encode(struct Barcode_Item *bc);
//extern int Barcode_cbr_verify(unsigned char *text);
//extern int Barcode_cbr_encode(struct Barcode_Item *bc);
//extern int Barcode_msi_verify(unsigned char *text);
//extern int Barcode_msi_encode(struct Barcode_Item *bc);
//extern int Barcode_pls_verify(unsigned char *text);
//extern int Barcode_pls_encode(struct Barcode_Item *bc);
//extern int Barcode_93_verify(unsigned char *text);
//extern int Barcode_93_encode(struct Barcode_Item *bc);


struct encoding {
    int type;
    int (*verify)(unsigned char *text);
    int (*encode)(struct Barcode_Item *bc);
};

struct encoding encodings[] = {
//    {BARCODE_EAN,    Barcode_ean_verify,    Barcode_ean_encode},
 //   {BARCODE_UPC,    Barcode_upc_verify,    Barcode_upc_encode},
 //   {BARCODE_ISBN,   Barcode_isbn_verify,   Barcode_isbn_encode},
    {BARCODE_128, Barcode_128_verify, Barcode_128_encode},
//    {BARCODE_128B,   Barcode_128b_verify,   Barcode_128b_encode},
//    {BARCODE_128C,   Barcode_128c_verify,   Barcode_128c_encode},
 //   {BARCODE_128RAW, Barcode_128raw_verify, Barcode_128raw_encode},
  //  {BARCODE_39,     Barcode_39_verify,     Barcode_39_encode},
 //   {BARCODE_I25,    Barcode_i25_verify,    Barcode_i25_encode},
 //  {BARCODE_128,    Barcode_128_verify,    Barcode_128_encode},
  //  {BARCODE_CBR,    Barcode_cbr_verify,    Barcode_cbr_encode},
  //  {BARCODE_PLS,    Barcode_pls_verify,    Barcode_pls_encode},
  //  {BARCODE_MSI,    Barcode_msi_verify,    Barcode_msi_encode},
  //  {BARCODE_93,     Barcode_93_verify,     Barcode_93_encode},
    {0,              NULL,                  NULL}
};

void BinToInt(const char *pszBin, int *pnInt)
{
	int  nInt= 0;
	nInt = (*pszBin- '0')*128+(*(pszBin+1)- '0')*64+(*(pszBin+2)- '0')*32+(*(pszBin+3)- '0')*16
		+(*(pszBin+4)- '0')*8+(*(pszBin+5)- '0')*4+(*(pszBin+6)- '0')*2+(*(pszBin+7)- '0');
	*pnInt = nInt;
}


/*
 * A function to encode a string into bc->partial, ready for
 * postprocessing to the output file. Meaningful bits for "flags" are
 * the encoding mask and the no-checksum flag. These bits
 * get saved in the data structure.
 */
int Barcode_Encode(struct Barcode_Item *bc, int flags)
{
    int validbits = BARCODE_ENCODING_MASK | BARCODE_NO_CHECKSUM;
    struct encoding *cptr;

    /* If any flag is cleared in "flags", inherit it from "bc->flags" */
    if (!(flags & BARCODE_ENCODING_MASK))
	flags |= bc->flags & BARCODE_ENCODING_MASK;
    if (!(flags & BARCODE_NO_CHECKSUM))
	flags |= bc->flags & BARCODE_NO_CHECKSUM;
    flags = bc->flags = (flags & validbits) | (bc->flags & ~validbits);

    if (!(flags & BARCODE_ENCODING_MASK)) {
	/* get the first code able to handle the text */
	for (cptr = encodings; cptr->verify; cptr++)
	    if (cptr->verify((unsigned char *)bc->ascii)==0)
		break;
	if (!cptr->verify) {
	    bc->error = EINVAL; /* no code can handle this text */
	    return -1;
	}
	flags |= cptr->type; /* this works */
	bc->flags |= cptr->type;
    }
    for (cptr = encodings; cptr->verify; cptr++)
	if (cptr->type == (flags & BARCODE_ENCODING_MASK))
	    break;
    if (!cptr->verify) {
	bc->error = EINVAL; /* invalid barcode type */
	return -1;
    }
    if (cptr->verify((unsigned char *)bc->ascii) != 0) {
	bc->error = EINVAL;
	return -1;
    }
    return cptr->encode(bc);
}


/* 
 * When multiple output formats are supported, there will
 * be a jumpt table like the one for the types. Now we don't need it
 */



int  MakeCode128Bmp_L(struct Barcode_Item * bc, STBMPIMAGE * pstBmpImage ,int nSize ,int Width)
{
	int i, nOffset = 0;
	char szBmpStr[30*6+19]; //最多30个字符,19由开始符、校验符和结算符组成
	char szBmpArray[33*11]; //转换成1、0表示
	char szBmp[20*600];
	char szBuf[48];
	memset(szBmpStr, 0, sizeof(szBmpStr));
	memset(szBmpArray, 0, sizeof(szBmpArray));
	memset(szBmp, 0, sizeof(szBmp));
	memset(szBuf, 0, sizeof(szBuf));

	for (i=0; i<strlen(bc->partial); i++) 
   	{
        unsigned char c = bc->partial[i];
	 if (isdigit(c)) 
	 {
		szBmpStr[i] = c;
	 }
	 if (islower(c))
	 {
		szBmpStr[i] = c-'a'+'1';
      	 }
	 if (isupper(c)) 
	 {
		szBmpStr[i] = c-'A'+'1';
 	 }
    }

//	DebugData("szBmpStr",szBmpStr,strlen(szBmpStr));
//	fprintf(stderr,"\n szBmpStr:\n%s",szBmpStr);

	for(i = 1; i < strlen(szBmpStr); i += 2)
	{
		int j,nNum  = 0;
		nNum =szBmpStr[i] -'0';
		for(j = 0; j < nNum; j++)
		{
			szBmpArray[nOffset] ='1';
			nOffset++;
		}
		nNum =szBmpStr[i+1] -'0';

		for(j= 0;j < nNum; j++)
		{
			szBmpArray[nOffset] ='0';
			nOffset++;
		}
	}

//DebugData("szBmpArray",szBmpArray,strlen(szBmpArray));
	fprintf(stderr,"\n szBmpArray:\n%s\nstrlen:%02d",szBmpArray,strlen(szBmpArray));

	for(i = 0;i < nOffset; i ++)
	{
		if(szBmpArray[i] == '0')
		{

			memset(szBuf, 0, sizeof(szBuf)-1);
			memcpy(szBmp+(Width/8)*nSize*i,  szBuf, (Width/8)*nSize);
		}
		if(szBmpArray[i] == '1')
		{
			memset(szBuf, 0xff, sizeof(szBuf)-1);

			memcpy(szBmp+(Width/8)*nSize*i,  szBuf, (Width/8)*nSize);

		}

	}
//	DebugData("szBmp",szBmp,20*250);

	memcpy(pstBmpImage->sBmpImage,szBmp,sizeof(szBmp));
	pstBmpImage->width = Width;
	pstBmpImage->height = strlen(szBmpArray)*nSize;

	//DebugData("BMP",szBmp,sizeof(szBmp));
	return 0;
}


int  MakeCode128Bmp_H(struct Barcode_Item * bc, STBMPIMAGE * pstBmpImage ,int nSize ,int Width)
{
	int i, nOffset = 0;
	char szBmpStr[100*6+19]; //最多30个字符,19由开始符、校验符和结算符组成
	char szBmpArray[33*110]; //转换成1、0表示
	char szBmp[20*600];
	char szBuf[480];

	memset(szBmpStr, 0, sizeof(szBmpStr));
	memset(szBmpArray, 0, sizeof(szBmpArray));
	memset(szBmp, 0, sizeof(szBmp));
	memset(szBuf, 0, sizeof(szBuf));

	for (i=0; i<strlen(bc->partial); i++) 
   	{
        unsigned char c = bc->partial[i];
	 if (isdigit(c)) 
	 {
		szBmpStr[i] = c;
	 }
	 if (islower(c))
	 {
		szBmpStr[i] = c-'a'+'1';
     }
	 if (isupper(c)) 
	 {
		szBmpStr[i] = c-'A'+'1';
 	 }
    }

//	DebugData("szBmpStr",szBmpStr,strlen(szBmpStr));
//	fprintf(stderr,"\n szBmpStr:\n%s",szBmpStr);

	for(i = 1; i < strlen(szBmpStr); i += 2)
	{
		int j,nNum  = 0;
		nNum =szBmpStr[i] -'0';
		for(j = 0; j < nNum*nSize; j++)
		{
			szBmpArray[nOffset] ='1';
			nOffset++;
		}
		nNum =szBmpStr[i+1] -'0';

		for(j= 0;j < nNum*nSize; j++)
		{
			szBmpArray[nOffset] ='0';
			nOffset++;
		}
	}

	//DebugData("szBmpArray",szBmpArray,strlen(szBmpArray));
	fprintf(stderr,"\n szBmpArray:\n%s\nstrlen:%02d\n",szBmpArray,strlen(szBmpArray));
	//补足为8的倍数
	strncat(szBmpArray, "0000000",(8 -strlen(szBmpArray)%8));
	fprintf(stderr,"%s,%d",__FUNCTION__,__LINE__);

	for(i = 0; i <strlen(szBmpArray)/8;i ++ )
	{
		int nTemp = 0;
		BinToInt(szBmpArray+8*i, &nTemp);
		szBuf[i] = nTemp;
	}
		fprintf(stderr,"\n%s,%d\n",__FUNCTION__,__LINE__);

	for(i= 0 ; i< Width; i++)
	{
		memcpy(szBmp+(strlen(szBmpArray)/8)*i, szBuf, strlen(szBmpArray)/8 );
	}
//	DebugData("szBmp",szBmp,20*250);
	fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__);

	memcpy(pstBmpImage->sBmpImage,szBmp,sizeof(szBmp));
	pstBmpImage->width =  strlen(szBmpArray);
	pstBmpImage->height = Width;    //在横向打印中宽即是高
	fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__);

	//DebugData("BMP",szBmp,sizeof(szBmp));
	return 0;
}
/*
 * Do it all in one step
 */
int MakeCode128(char *pszInStr ,int nSize ,int Width ,STBMPIMAGE  *pstBmpImage,int  nDirection)
{
	int flags = BARCODE_OUT_PS | BARCODE_OUT_NOHEADERS;
    struct Barcode_Item * bc;

    if (!(bc=Barcode_Create(pszInStr))) 
   	{
		PubMsgDlg("警告","生成码制结构体出错!", 3,3);
		return  -1;
    }
    
    if (Barcode_Encode(bc, flags) < 0  ) 
   	{
		Barcode_Delete(bc);
		PubMsgDlg("错误","生成码制错误!", 3,3);
		return -1;
    }
	if(nDirection ==LONGITUDINAL)
		MakeCode128Bmp_L(bc, pstBmpImage,nSize, Width );
	else
		MakeCode128Bmp_H(bc, pstBmpImage,nSize, Width );
	//PubPrePrinter("\r\r\r\r\r\r\r\r");
	fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__);

    Barcode_Delete(bc);
			fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__);

    return 0;
}


