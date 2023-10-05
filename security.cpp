/*
 * Security.c
 *
 *  Created on: Dec 18, 2014
 *      Author: Lanhsin
 */

#include "sysio.h"
#include "security.h"
#include <cstring>
#include <iostream>
#include <iomanip>

static void sec_eiaChoice(cipherPara_S* psInPara)
{
    unsigned char* dataIn = psInPara->aData;

	switch(psInPara->u32Mode & 0xF){
		case EIA1:
			eia1(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->dataOut);
			break;
		case EIA2:
			eia2(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->dataOut);
			break;
		case EIA3:
			eia3(psInPara->aEiaKey, psInPara->u32Count, psInPara->u32Bearer, psInPara->u32Dir, dataIn, psInPara->u32Len, psInPara->dataOut);
			break;
        case EIA0:
            memset(psInPara->dataOut, 0, 4);
            break;
		default:	// EEA0
			break;
    }
}

static void sec_eeaChoice(cipherPara_S* psInPara)
{
    unsigned char* dataIn = psInPara->aData;

	switch((psInPara->u32Mode >> 4)& 0xf){
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

static void sec_para_print(cipherPara_S *secPara_p)
{
    uint32_t i, j;
    
    std::cout << "=========================================\n";
    std::cout << "secPara_p->u32Len = " << secPara_p->u32Len << std::endl;
    std::cout << "secPara_p->u32Dir = " << secPara_p->u32Dir << std::endl;
    std::cout << "secPara_p->u32Bearer = " << secPara_p->u32Bearer << std::endl;
    std::cout << "secPara_p->u32Mode = " << secPara_p->u32Mode << std::endl;
    std::cout << "secPara_p->u32Count = " << secPara_p->u32Count << std::endl;

    std::cout << std::hex;
    if(secPara_p->u32Mode & 0x0F)
    {
        std::cout << "secPara_p->aEiaKey[] = \n";
        for (i = 0; i < 16; i++)
            std::cout << std::setfill('0') << std::setw(2) << +(secPara_p->aEiaKey[i]) << ' ';
    }
    else if(secPara_p->u32Mode & 0xF0)
    {
        std::cout << "secPara_p->aEeaKey[] = \n";
        for (i = 0; i < 16; i++)
            std::cout << +secPara_p->aEeaKey[i] << ' ';
    }

    std::cout << "\nsecPara_p->aData[] = \n";

    j = 0;
    for (i = 0; i< (secPara_p->u32Len + 16 + 7) / 8; i++, j++)
    {
        if(i%8 == 0 && i != 0)
            std::cout << " ";
        if(i%16 == 0 && i != 0)
            std::cout << "\n";
        std::cout << std::setfill('0') << std::setw(2) << +secPara_p->aData[j] << ' ';
    }
    
    std::cout << "\nsecPara_p->aDataOut[] = \n";
    if(secPara_p->u32Mode & 0xF)
    {
        for (i = 0; i < 4; i++)
            std::cout << std::setfill('0') << std::setw(2) << +secPara_p->dataOut[i];
    }
    else
    {
        for (i = 0; i< (secPara_p->u32Len) / 8; i++)
        {
            if(i%8 == 0 && i != 0)
                std::cout << " ";
            if(i%16 == 0 && i != 0)
                std::cout << "\n";
            std::cout << std::setfill('0') << std::setw(2) << +secPara_p->dataOut[i] << ' ';
        }
    }    
    std::cout<<"\n=========================================\n\n";
}

void sec_cipherSdu(cipherPara_S *secPara_p)
{
    if(secPara_p->u32Mode & 0xF)
        sec_eiaChoice(secPara_p);
    else
        sec_eeaChoice(secPara_p);

    // get data ~~ out of the function ~~
    sec_para_print(secPara_p);
}