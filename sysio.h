/*
 * sysio.h
 *
 *  Created on: 06/01/2015
 *      Author: Lanhsin.lin
 */

#ifndef _SYS_IO_H_
#define _SYS_IO_H_

#if (defined(CO_SIM_RTK) || defined(FPGA_SIM_RTK))
#include <svdpi.h>
#elif defined(QEMU_RTK_CUS)
#include "hw/sysbus.h"
#endif

#define MAX_QUEUE_SIZE                  1024 //must be the same with size of queue in SV code

#ifdef ASIC2
  #define IPFILTER_REGISTER_BASE          0x1C240000
  #define SEC_REGISTER_BASE               0x1C280000
  #define RLCUL_REGISTER_BASE             0x1C242000
  #define SRAM_SYSTEM_ADDRESS             0x1D100000
  #define SRAM_UPLINK_ADDRESS             0x1D140000
  #define SRAM_DOWNLINK_ADDRESS           0x1D180000
#else
  #define IPFILTER_REGISTER_BASE          0x1C04A000
  #define SEC_REGISTER_BASE               0x1C048000
  #define RLCUL_REGISTER_BASE             0x1c040000
  #define SRAM_SYSTEM_ADDRESS             0x1d080000
  #define SRAM_UPLINK_ADDRESS             0x1d100000
  #define SRAM_DOWNLINK_ADDRESS           0x1d140000
#endif

#define IPFILTER_HEADER_PARA            IPFILTER_REGISTER_BASE+0x00
#define IPFILTER_HEADER_CTRL_REG        IPFILTER_REGISTER_BASE+0x40
#define IPFILTER_MMR_REG                IPFILTER_REGISTER_BASE+0x180
#define IPFILTER_PACKET_FILTER_PARA     IPFILTER_REGISTER_BASE+0x1000

#define SEC_CONTROL_REG                 SEC_REGISTER_BASE+0x00
#define SEC_INTERRUPT_REG               SEC_REGISTER_BASE+0x4C
#define SEC_IOOUT_REG                   SEC_REGISTER_BASE+0xF0  

#define DRAM_ADDRESS                    0x00000000

#define REF(v, tYPE)      ((tYPE *)(&(v)))
#define DEREF(p, tYPE)    (*((tYPE *)(p)))

#define GET_ADDR_B(addr)	(&DEREF(addr, unsigned char))
#define GET_ADDR_W(addr)	(&DEREF(addr, unsigned short))
#define GET_ADDR_D(addr)	(&DEREF(addr, unsigned int))

extern void write_reg( unsigned int addr, unsigned int data, unsigned int byte_size);
extern void read_reg( unsigned int addr, unsigned int* data, unsigned int byte_size);
extern void read_multi_regs(unsigned int addr, unsigned int* data, unsigned int byte_size);

extern void write_mem( unsigned int addr, unsigned char* data, unsigned int byte_size);
extern void read_mem( unsigned int addr, unsigned char* data, unsigned int byte_size);

#endif /* _SYS_IO_H_ */
