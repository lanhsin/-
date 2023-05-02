/*
 * Security.c
 *
 *  Created on: Dec 18, 2014
 *      Author: Lanhsin
 */

#include "sysio.h"
#include "security.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void sec_eiaChoice(bool bMasterMode, cipherPara_S* psInPara)
{
    uint8_t *dataIn = psInPara->aData;

    if(bMasterMode) {
        dataIn = dataIn + SEC_DESC_SIZE;
    }

	switch(psInPara->u32Mode & 0xF){
		case EIA1:
			eia1(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->aMac);
			break;
		case EIA2:
			eia2(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->aMac);
			break;
		case EIA3:
			eia3(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->aMac);
			break;
        case EIA0:
            memset(psInPara->aMac, 0, 4);
            break;
		default:	// EEA0
			break;
    }
}

static void sec_eeaChoice(bool bMasterMode, cipherPara_S* psInPara)
{
    uint8_t *dataIn = psInPara->aData;

    if(bMasterMode) {
        dataIn = dataIn + SEC_DESC_SIZE;
    }

	switch((psInPara->u32Mode >> 4)&(0xf)){
		case EEA1:
			eea1(psInPara->aEeaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->u32Offset, psInPara->dataOut);
			break;
		case EEA2:
			eea2(psInPara->aEeaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->u32Offset, psInPara->dataOut);
			break;
		case EEA3:
			eea3(psInPara->aEeaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->u32Offset, psInPara->dataOut);
			break;
        case EEA0:
            memcpy(psInPara->dataOut, dataIn, (psInPara->u32Len)/8);
            break;
		default:	// EEA0
			break;
    }
}

static void sec_para_print(bool bMasterMode, cipherPara_S *secPara_p)
{
    uint32_t i, j;
    
    printf("=========================================\n");
    printf("secPara_p->u32Len = %d\n", secPara_p->u32Len);
    printf("secPara_p->u32Dir = %d\n", secPara_p->u32Dir);
    printf("secPara_p->u32Bearer = %d\n", secPara_p->u32Bearer);
    printf("secPara_p->u32Mode = %d\n", secPara_p->u32Mode);
    printf("secPara_p->u32Count = %X\n", secPara_p->u32Count);

    if(secPara_p->u32Mode & 0x0F)
    {
        printf("secPara_p->aEiaKey[] = \n");
        for (i=0; i<16; i++)
        {
            printf("%02X ", secPara_p->aEiaKey[i]);
        }
    }
    else if(secPara_p->u32Mode & 0xF0)
    {
        printf("secPara_p->aEeaKey[] = \n");
        for (i=0; i<16; i++)
        {
            printf("%02X ", secPara_p->aEeaKey[i]);
        }
    }

    printf("\nsecPara_p->aData[] = \n");
    if(bMasterMode)
        j = SEC_DESC_SIZE;
    else
        j = 0;

    for (i=0; i< (secPara_p->u32Len) / 8; i++, j++)
    {
        if(i%8 == 0 && i != 0)
            printf(" ");
        if(i%16 == 0 && i != 0)
            printf("\n");

        printf("%02X", secPara_p->aData[j]);
    }
    
    printf("\nsecPara_p->aDataOut[] = \n");
    if(secPara_p->u32Mode & 0xF)
    {
            for (i=0; i< 4; i++)
            {
                printf("%02X", secPara_p->aMac[i]);
            }   
    }
    else
    {
        for (i=0; i< (secPara_p->u32Len) / 8; i++)
        {
            if(i%8 == 0 && i != 0)
                printf(" ");
            if(i%16 == 0 && i != 0)
                printf("\n");

            printf("%02X", secPara_p->dataOut[i]);
        }
    }    
    printf("\n=========================================\n\n");
}


void sec_get_dataOut(bool bMasterMode, cipherPara_S *secPara_p, SEC_INOUT *reg_inOut_p)
{
    uint32_t dataOut;

    // get data
    if(bMasterMode)
        dataOut = secReg_get_dataOut(reg_inOut_p);
    else if(secPara_p->u32Mode & 0xF)
        dataOut = (uint32_t)(&secReg_get_xMac(reg_inOut_p));
    else
        dataOut = secReg_get_dataOut(reg_inOut_p);

    if(secPara_p->u32Mode & 0xF)
    {
        memcpy( (uint8_t *)(&secReg_get_xMac(reg_inOut_p)), secPara_p->aMac, 4);
        if(bMasterMode)
            write_mem( dataOut, secPara_p->aMac, 4);
    }
    else
        write_mem( dataOut, secPara_p->dataOut, secPara_p->u32Len / 8);
}


static void secReg_fill_content_from_desc(cipherPara_S *secPara_p)
{
    SEC_DMA_DESC *secDesc_p = (SEC_DMA_DESC *)(secPara_p->aData);
    secPara_p->u32Mode = (secDesc_p->eea_eia_mode & 0x7) | ((secDesc_p->eea_eia_mode & 0x18) << 1);
    secPara_p->u32Count = secDesc_p->count; 
    secPara_p->u32Bearer = secDesc_p->bearer;
    secPara_p->u32Dir = secDesc_p->direction;
    secPara_p->u32Len = secDesc_p->length;
    secPara_p->u32Offset = 0;

    memcpy(secPara_p->aEeaKey, secDesc_p->eea_Key0, 4);
    memcpy(secPara_p->aEeaKey+4, secDesc_p->eea_Key1, 4);
    memcpy(secPara_p->aEeaKey+8, secDesc_p->eea_Key2, 4);
    memcpy(secPara_p->aEeaKey+12, secDesc_p->eea_Key3, 4);

    memcpy(secPara_p->aEiaKey, secDesc_p->eia_Key0, 4);
    memcpy(secPara_p->aEiaKey+4, secDesc_p->eia_Key1, 4);
    memcpy(secPara_p->aEiaKey+8, secDesc_p->eia_Key2, 4);
    memcpy(secPara_p->aEiaKey+12, secDesc_p->eia_Key3, 4);
}


static void secReg_fill_content_from_ctrl_p(cipherPara_S *secPara_p, SEC_CTRL    *reg_ctrl_p)
{
    // pass IV
    secPara_p->u32Mode = secReg_get_cipher_mode(reg_ctrl_p);
    secPara_p->u32Count= secReg_get_count(reg_ctrl_p); 
    secPara_p->u32Bearer = secReg_get_bearer(reg_ctrl_p);
    secPara_p->u32Dir = secReg_get_direction(reg_ctrl_p);
    secPara_p->u32Len = secReg_get_length(reg_ctrl_p);
    secPara_p->u32Offset = 0;
    // pass key
    secReg_get_eea_key(reg_ctrl_p, (uint32_t *)(secPara_p->aEeaKey));
    secReg_get_eia_key(reg_ctrl_p, (uint32_t *)(secPara_p->aEiaKey));
}


void sec_cipherSdu(cipherPara_S *secPara_p, SEC_CTRL    *reg_ctrl_p)
{
    bool bMasterMode = secReg_get_mode(reg_ctrl_p);
    if(bMasterMode)
        secReg_fill_content_from_desc(secPara_p);
    else
        secReg_fill_content_from_ctrl_p(secPara_p, reg_ctrl_p);

    if(secPara_p->u32Mode & 0xF)
        sec_eiaChoice(bMasterMode, secPara_p);
    else
        sec_eeaChoice(bMasterMode, secPara_p);

    // get data ~~ out of the function ~~
    sec_para_print(bMasterMode, secPara_p);
}


