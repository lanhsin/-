/*********************************************************************/
// SecReg.h 
/*********************************************************************/

#ifndef _SEC_REG_H_
#define __SEC_REG_H_

#include "hw_secReg.h"

/***************************Start of Security Register Operation***************************/


#ifdef FPGA_SIM_RTK

#include "sysio.h"

inline void SEC_SetDMA_Mode(bool value)
{
    write_reg(REGS_ADDR_SEC_DMA_MODE, value, 1);
}

inline void SEC_SetCipher_Enable(bool value)
{
    write_reg(REGS_ADDR_SEC_EN, value, 1);
}


/*!
 * \fn void SEC_SetCipher_Mode(unsigned char value)
 * \brief Set the mode(EEA and EIA mode).
 * \param [in]  value of mode
 * \return  void
 */
inline void SEC_SetCipher_Mode(unsigned char value)
{
    write_reg(REGS_ADDR_SEC_CIPHER_MODE, value, 1);
}


/*!
 * \fn void SEC_SetEEA_KEY(unsigned int *value)
 * \brief Set the first 32-bit key of EEA.
 * \param [in]  value of key1
 * \return  void
 */
inline void SEC_SetEEA_KEY(unsigned int *value)
{
    write_reg(REGS_ADDR_SEC_EEA_KEY_1, *value, 4);
    write_reg(REGS_ADDR_SEC_EEA_KEY_2, *(value + 1), 4);
    write_reg(REGS_ADDR_SEC_EEA_KEY_3, *(value + 2), 4);
    write_reg(REGS_ADDR_SEC_EEA_KEY_4, *(value + 3), 4);
}


/*!
 * \fn void SEC_SetEIA_KEY(unsigned int *value)
 * \brief Set the first 32-bit key of EEA.
 * \param [in]  value of key1
 * \return  void
 */
inline void SEC_SetEIA_KEY(unsigned int *value)
{
    write_reg(REGS_ADDR_SEC_EIA_KEY_1, *value, 4);
    write_reg(REGS_ADDR_SEC_EIA_KEY_2, *(value + 1), 4);
    write_reg(REGS_ADDR_SEC_EIA_KEY_3, *(value + 2), 4);
    write_reg(REGS_ADDR_SEC_EIA_KEY_4, *(value + 3), 4);
}


/*!
 * \fn void SEC_SetCount(unsigned int value)
 * \brief Set the count, the 32-bit time variant input to the cipher function.
 * \param [in]  value of count
 * \return  void
 */
inline void SEC_SetCount(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_COUNT, value, 4);
}


/*!
 * \fn void CIPHER_SetConfig(unsigned int value)
 * \brief Set the value of CONFIG register.
 * \param [in]  value
 * \return  void
 */
inline void SEC_SetConfig(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_CONFIG, value, 4);
}


/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Raw (void)
 * \brief Get the transmit fifo raw interrupt status.
 * \return  status
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Raw (void)
{
    unsigned int u32Data;
    read_reg(REGS_ADDR_SEC_TX_FIFO_INT_RAW, &u32Data, 4);
    return u32Data;
}


/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Mask (void)
 * \brief Get the transmit fifo interrupt mask.
 * \return  mask
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Mask(void)
{
    unsigned int u32Data;
    read_reg(REGS_ADDR_SEC_TX_FIFO_INT_MSK, &u32Data, 4);
    return u32Data;
}


/*!
 * \fn void SEC_SetTx_FIFO_Interrupt_Mask(unsigned int value)
 * \brief Set the transmit fifo interrupt mask.
 * \param [in]  value of mask
 * \return  void
 */
inline void SEC_SetTx_FIFO_Interrupt_Mask(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_TX_FIFO_INT_MSK, value, 4);
}


/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Status (void)
 * \brief Get the transmit fifo interrupt status.
 * \return  status
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Status(void)
{
    unsigned int u32Data;
    read_reg(REGS_ADDR_SEC_TX_FIFO_INT_STS, &u32Data, 4);
    return u32Data;
}


/*!
 * \fn void SEC_SetTx_FIFO_Interrupt_Status(unsigned int value)
 * \brief Set the transmit fifo interrupt status register.(Only use to clear the status bit)
 * \param [in]  value
 * \return  void
 */
inline void SEC_SetTx_FIFO_Interrupt_Status(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_TX_FIFO_INT_STS, value, 4);
}


inline void SEC_SetRx_FIFO_Almost_Empty(unsigned char value)
{
    write_reg(REGS_ADDR_SEC_RX_FIFO_ALMOSTEMPTY, value & 0xf, 1);
}


inline void SEC_SetTx_FIFO_Almost_Full(unsigned char value)
{
    write_reg(REGS_ADDR_SEC_TX_FIFO_ALMOSTFULL, value & 0xf, 1);
}


/*!
 * \fn void SEC_SetRd_Addr(unsigned int value)
 * \brief Set the read start address.
 * \param [in]  value of start mode
 * \return  void
 */
inline void SEC_SetRd_Addr(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_START_RD_ADDR, value, 4);
}


/*!
 * \fn void SEC_SetWr_Addr(unsigned int value)
 * \brief Set the write start address.
 * \param [in]  value of start mode
 * \return  void
 */
inline void SEC_SetWr_Addr(unsigned int value)
{
    write_reg(REGS_ADDR_SEC_START_WR_ADDR, value, 4);
}


/*!
 * \fn unsigned int SEC_GetMac_I(void)
 * \brief Get value of MAC-I
 * \return  MAC-I
 */
inline unsigned int SEC_GetMac_I(void)
{
    unsigned int u32Data;
    read_reg(REGS_ADDR_SEC_MAC, &u32Data, 4);
    return u32Data;
}

#else
unsigned int sec_virt_to_phys(unsigned int virt)
{
    unsigned int phys; 
    phys = virt - 0xA0000000;
    return phys;
}



inline void SEC_SetDMA_Mode(bool value)
{
    HWREGB(REGS_ADDR_SEC_DMA_MODE) = value;
}


inline void SEC_SetCipher_Enable(bool value)
{
    HWREGB(REGS_ADDR_SEC_EN) = value;
}


/*!
 * \fn void SEC_SetCipher_Mode(unsigned char value)
 * \brief Set the mode(EEA and EIA mode).
 * \param [in]  value of mode
 * \return  void
 */
inline void SEC_SetCipher_Mode(unsigned char value)
{
    HWREGB(REGS_ADDR_SEC_CIPHER_MODE) = value;
}


/*!
 * \fn void SEC_SetEEA_KEY(unsigned int *value)
 * \brief Set the first 32-bit key of EEA.
 * \param [in]  value of key1
 * \return  void
 */
inline void SEC_SetEEA_KEY(unsigned int *value)
{
    HWREG(REGS_ADDR_SEC_EEA_KEY_1) = *value;
    HWREG(REGS_ADDR_SEC_EEA_KEY_2) = *(value + 1);
    HWREG(REGS_ADDR_SEC_EEA_KEY_3) = *(value + 2);
    HWREG(REGS_ADDR_SEC_EEA_KEY_4) = *(value + 3);
}


/*!
 * \fn void SEC_SetEIA_KEY(unsigned int *value)
 * \brief Set the first 32-bit key of EEA.
 * \param [in]  value of key1
 * \return  void
 */
inline void SEC_SetEIA_KEY(unsigned int *value)
{
    HWREG(REGS_ADDR_SEC_EIA_KEY_1) = *value;
    HWREG(REGS_ADDR_SEC_EIA_KEY_2) = *(value + 1);
    HWREG(REGS_ADDR_SEC_EIA_KEY_3) = *(value + 2);
    HWREG(REGS_ADDR_SEC_EIA_KEY_4) = *(value + 3);
}


/*!
 * \fn void SEC_SetCount(unsigned int value)
 * \brief Set the count, the 32-bit time variant input to the cipher function.
 * \param [in]  value of count
 * \return  void
 */
inline void SEC_SetCount(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_COUNT) =  value;
}


/*!
 * \fn void CIPHER_SetConfig(unsigned int value)
 * \brief Set the value of CONFIG register.
 * \param [in]  value
 * \return  void
 */
inline void SEC_SetConfig(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_CONFIG) = value;
}



/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Raw (void)
 * \brief Get the transmit fifo raw interrupt status.
 * \return  status
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Raw (void)
{
    return HWREG(REGS_ADDR_SEC_TX_FIFO_INT_RAW);
}



/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Mask (void)
 * \brief Get the transmit fifo interrupt mask.
 * \return  mask
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Mask(void)
{
    return HWREG(REGS_ADDR_SEC_TX_FIFO_INT_MSK);
}


/*!
 * \fn void SEC_SetTx_FIFO_Interrupt_Mask(unsigned int value)
 * \brief Set the transmit fifo interrupt mask.
 * \param [in]  value of mask
 * \return  void
 */
inline void SEC_SetTx_FIFO_Interrupt_Mask(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_TX_FIFO_INT_MSK) = value;
}


/*!
 * \fn unsigned int SEC_GetTx_FIFO_Interrupt_Status (void)
 * \brief Get the transmit fifo interrupt status.
 * \return  status
 */
inline unsigned int SEC_GetTx_FIFO_Interrupt_Status(void)
{
    return HWREG(REGS_ADDR_SEC_TX_FIFO_INT_STS);
}


/*!
 * \fn void SEC_SetTx_FIFO_Interrupt_Status(unsigned int value)
 * \brief Set the transmit fifo interrupt status register.(Only use to clear the status bit)
 * \param [in]  value
 * \return  void
 */
inline void SEC_SetTx_FIFO_Interrupt_Status(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_TX_FIFO_INT_STS) = value;
}


inline void SEC_SetRx_FIFO_Almost_Empty(unsigned char value)
{
    HWREGB(REGS_ADDR_SEC_RX_FIFO_ALMOSTEMPTY) = value & 0xf;
}


inline void SEC_SetTx_FIFO_Almost_Full(unsigned char value)
{
    HWREGB(REGS_ADDR_SEC_TX_FIFO_ALMOSTFULL) = value & 0xf;
}


/*!
 * \fn void SEC_SetRd_Addr(unsigned int value)
 * \brief Set the read start address.
 * \param [in]  value of start mode
 * \return  void
 */
inline void SEC_SetRd_Addr(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_START_RD_ADDR) = sec_virt_to_phys(value);
}


/*!
 * \fn void SEC_SetWr_Addr(unsigned int value)
 * \brief Set the write start address.
 * \param [in]  value of start mode
 * \return  void
 */
inline void SEC_SetWr_Addr(unsigned int value)
{
    HWREG(REGS_ADDR_SEC_START_WR_ADDR) = sec_virt_to_phys(value);
}


/*!
 * \fn unsigned int SEC_GetMac_I(void)
 * \brief Get value of MAC-I
 * \return  MAC-I
 */
inline unsigned int SEC_GetMac_I(void)
{
    return HWREG(REGS_ADDR_SEC_MAC);
}

/****************************End of Security Register Operation***************************/

#endif

#endif /* _SEC_REG_H_ */

