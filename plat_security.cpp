/*******************************************************************************/
/*  Copyright (c) 2014 Realtek Semiconductor Corp.                             */
/*                                                                             */
/*  This module is a confidential and proprietary property of RealTek and      */
/*  possession or use of this module requires written permission of RealTek.   */
/*******************************************************************************/

/* Project: Lambor LTE modem
 * $URL$
 * $Date$
 * $Rev$
 * $Author$
 */
#include "plat_security.h"


 void Plat_Security_SetDma_Mode(bool value)
{
    SEC_SetDMA_Mode(value);
}


/*!
 * \fn void Plat_Security_SetCfg(SECURITY_Config_t *pCipher)
 * \brief Initiation for Software Polling mode with high efficiency .
 * \param [in]  pCipher: pointer of Software Polling mode config structure
 * \return  void
 */
void Plat_Security_SetCfg(SECURITY_Config_t *pCipher)
{
    unsigned int RdAddr = (unsigned int)pCipher->src_data;
    unsigned int WrAddr = (unsigned int)pCipher->dest_data;
    SEC_SetCipher_Mode(pCipher->sec_mode);

    SEC_SetCount(pCipher->count);

    /* Set CONFIG register */
    SEC_SetConfig(pCipher->secCfg);

    SEC_SetEEA_KEY((unsigned int *)(pCipher->key));
    SEC_SetEIA_KEY((unsigned int *)(pCipher->key));

    if(RdAddr)
        SEC_SetRd_Addr(RdAddr);
    if(WrAddr)
        SEC_SetWr_Addr(WrAddr);

    SEC_SetCipher_Enable(true);
}


void Plat_Security_SetMst(unsigned int RdAddr, unsigned int WrAddr)
{
    Plat_Security_ClearPollingBit();    // This bit will set in default.

    if(RdAddr)
        SEC_SetRd_Addr(RdAddr);
    if(WrAddr)
        SEC_SetWr_Addr(WrAddr);

    SEC_SetCipher_Enable(true);
}


/*!
 * \fn void Plat_Security_GetData(unsigned int* retMac)
 * \brief Read data from memory or Mac-I from register.
 * \param [in]  retMac: pointer to MAC-I address. 
 * \return void
 */
void Plat_Security_GetData(unsigned int* retMac)
{
    unsigned int value = 0;

    while (value == 0)
    {
        value = SEC_TX_EMPTY_INT_MASK & SEC_GetTx_FIFO_Interrupt_Raw();

        if(value)
        {
            if(retMac != nullptr)
            {
                *retMac = SEC_GetMac_I();
            }
        }
    }
}


/*!
 * \fn void Plat_Security_ClearPollingBit(void)
 * \brief Clear polling bit before start cipher engine.
 * \return void
 */
void Plat_Security_ClearPollingBit(void)
{
    SEC_SetTx_FIFO_Interrupt_Status(SEC_TX_EMPTY_INT_MASK);
}


void Plat_Security_SetRxFIFO_AlmostEmpty(unsigned char value)
{
    SEC_SetRx_FIFO_Almost_Empty(value);
}


void Plat_Security_SetTxFIFO_AlmostFull(unsigned char value)
{
    SEC_SetTx_FIFO_Almost_Full(value);
}


