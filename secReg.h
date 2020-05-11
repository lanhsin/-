/*********************************************************************/
// SecReg.h 
/*********************************************************************/

#ifndef _SEC_REG_H_
#define __SEC_REG_H_

#include "hw_secReg.h"


/***********************Start of Security Register function statement ***********************/

/* Security control register setting */
void SEC_SetDMA_Mode(unsigned char value);
void SEC_SetCipher_Enable(unsigned char value);


void SEC_SetCipher_Mode(unsigned char value);
void SEC_SetEEA_KEY(unsigned int* value);
void SEC_SetEIA_KEY(unsigned int* value);
void SEC_SetCount(unsigned int value);
void SEC_SetConfig(unsigned int value);


/* TRx FIFO regsiter setting */
unsigned int SEC_GetTx_FIFO_Interrupt_Raw (void);
unsigned int SEC_GetTx_FIFO_Interrupt_Mask(void);
void SEC_SetTx_FIFO_Interrupt_Mask(unsigned int value);
unsigned int SEC_GetTx_FIFO_Interrupt_Status(void);
void SEC_SetTx_FIFO_Interrupt_Status(unsigned int value);
void SEC_SetRx_FIFO_Almost_Empty(unsigned char value);
void SEC_SetTx_FIFO_Almost_Full(unsigned char value);


/* Input & output register setting */
void SEC_SetRd_Addr(unsigned int value);
void SEC_SetWr_Addr(unsigned int value);
unsigned int SEC_GetMac_I(void);


/***********************End of Security Register function statement ***********************/



#endif /* _SEC_REG_H_ */

