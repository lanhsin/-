/*********************************************************************/
// HW_secReg.h 
/*********************************************************************/

#ifndef __HW_SEC_REG_H_
#define __HW_SEC_REG_H_


/*******************************Start of Security Register Address *****************************/
#if defined(ASIC2)
    #if defined(FPGA_SIM_RTK)
        #define	SEC_MMR_PHYSICAL_BASE                                  (0x1C280000)
    #else
        #define	SEC_MMR_PHYSICAL_BASE                                  (0x40280000)
    #endif
#else
#define	SEC_MMR_PHYSICAL_BASE                                  (0x1C048000)
#endif

#if defined(FPGA_SIM_RTK)
#define	SEC_MMR_BASE                                           SEC_MMR_PHYSICAL_BASE
#else
/* FPGA  +0xA000_0000 */
#ifdef ASIC2
#define	SEC_MMR_BASE                                           (0xBC280000)
#else
#define	SEC_MMR_BASE                                           (0xBC048000)
#endif
#endif

//------------------------------ SECURITY           SEC_MMR_BASE        32'h1C048000
#define    REGS_ADDR_SEC_DMA_MODE                   (SEC_MMR_BASE+0x000)
#define    REGS_ADDR_SEC_EN                         (SEC_MMR_BASE+0x004)
#define    REGS_ADDR_SEC_CIPHER_MODE                (SEC_MMR_BASE+0x008)
#define    REGS_ADDR_SEC_EEA_KEY_1                  (SEC_MMR_BASE+0x00c)
#define    REGS_ADDR_SEC_EEA_KEY_2                  (SEC_MMR_BASE+0x010)
#define    REGS_ADDR_SEC_EEA_KEY_3                  (SEC_MMR_BASE+0x014)
#define    REGS_ADDR_SEC_EEA_KEY_4                  (SEC_MMR_BASE+0x018)
#define    REGS_ADDR_SEC_EIA_KEY_1                  (SEC_MMR_BASE+0x01c)
#define    REGS_ADDR_SEC_EIA_KEY_2                  (SEC_MMR_BASE+0x020)
#define    REGS_ADDR_SEC_EIA_KEY_3                  (SEC_MMR_BASE+0x024)
#define    REGS_ADDR_SEC_EIA_KEY_4                  (SEC_MMR_BASE+0x028)
#define    REGS_ADDR_SEC_COUNT                      (SEC_MMR_BASE+0x02c)
#define    REGS_ADDR_SEC_CONFIG                     (SEC_MMR_BASE+0x030)
#define    REGS_ADDR_SEC_INT_RAW                    (SEC_MMR_BASE+0x034)
#define    REGS_ADDR_SEC_INT_MSK                    (SEC_MMR_BASE+0x038)
#define    REGS_ADDR_SEC_INT_STS                    (SEC_MMR_BASE+0x03c)
#define    REGS_ADDR_SEC_RX_FIFO_INT_RAW            (SEC_MMR_BASE+0x040)
#define    REGS_ADDR_SEC_RX_FIFO_INT_MSK            (SEC_MMR_BASE+0x044)
#define    REGS_ADDR_SEC_RX_FIFO_INT_STS            (SEC_MMR_BASE+0x048)
#define    REGS_ADDR_SEC_TX_FIFO_INT_RAW            (SEC_MMR_BASE+0x04c)
#define    REGS_ADDR_SEC_TX_FIFO_INT_MSK            (SEC_MMR_BASE+0x050)
#define    REGS_ADDR_SEC_TX_FIFO_INT_STS            (SEC_MMR_BASE+0x054)
#define    REGS_ADDR_SEC_RX_FIFO_ALMOSTFULL         (SEC_MMR_BASE+0x058)
#define    REGS_ADDR_SEC_RX_FIFO_ALMOSTEMPTY        (SEC_MMR_BASE+0x05C)
#define    REGS_ADDR_SEC_TX_FIFO_ALMOSTFULL         (SEC_MMR_BASE+0x060)
#define    REGS_ADDR_SEC_TX_FIFO_ALMOSTEMPTY        (SEC_MMR_BASE+0x064)
#define    REGS_ADDR_SEC_UPLINK_COUNT               (SEC_MMR_BASE+0x068)
#define    REGS_ADDR_SEC_DOWNLINK_COUNT             (SEC_MMR_BASE+0x06c)
#define    REGS_ADDR_SEC_RX_FIFO                    (SEC_MMR_BASE+0x070)
#define    REGS_ADDR_SEC_TX_FIFO                    (SEC_MMR_BASE+0x0b0)
#define    REGS_ADDR_SEC_START_RD_ADDR              (SEC_MMR_BASE+0x0F0)
#define    REGS_ADDR_SEC_START_WR_ADDR              (SEC_MMR_BASE+0x0F4)
#define    REGS_ADDR_SEC_MAC                        (SEC_MMR_BASE+0x0F8)


/*******************************End of Security Register Address *****************************/

/****************************Start of TX_FIFO_INT mask and shift  ************************/
// This macro will be used in the registers: TX_FIFO_INT_RAW, TX_FIFO_INT_MSK, TX_FIFO_INT_STS
#define    SEC_TX_ALMOSTEMPTY_INT_SHIFT             (0)
#define    SEC_TX_EMPTY_INT_SHIFT                   (1)
#define    SEC_TX_ALMOSTFULL_INT_SHIFT              (2)
#define    SEC_TX_FULL_INT_SHIFT                    (3)

#define    SEC_TX_ALMOSTEMPTY_INT_MASK              (0x1 << SEC_TX_ALMOSTEMPTY_INT_SHIFT)
#define    SEC_TX_EMPTY_INT_MASK                    (0x1 << SEC_TX_EMPTY_INT_SHIFT)
#define    SEC_TX_ALMOSTFULL_INT_MASK               (0x1 << SEC_TX_ALMOSTFULL_INT_SHIFT)
#define    SEC_TX_FULL_INT_MASK                     (0x1 << SEC_TX_FULL_INT_SHIFT)

/****************************End of TX_FIFO_INT mask and shift  *************************/


//*****************************************************************************
//
// Define a boolean type, and values for true and false.
//
//*****************************************************************************
typedef unsigned char tBoolean;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif


//*****************************************************************************
//
// Macros for hardware access, both direct and via the bit-band region.
//
//*****************************************************************************
#define HWREG(x)                                                              \
        (*((volatile unsigned long *)(x)))
#define HWREGH(x)                                                             \
        (*((volatile unsigned short *)(x)))
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#define HWREGBITW(x, b)                                                       \
        HWREG(((unsigned long)(x) & 0xF0000000) | 0x02000000 |                \
              (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITH(x, b)                                                       \
        HWREGH(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITB(x, b)                                                       \
        HWREGB(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))



#endif /* _HW_SEC_REG_H_ */

