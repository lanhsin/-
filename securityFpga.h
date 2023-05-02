/*
 * securityFpga.h
 *
 *  Created on: 11/06/2015
 *      Author: Lanhsin
 */

#ifndef _SECURITY_FPGA_H_
#define _SECURITY_FPGA_H_


#include "pattern_fpga/sec_tp_types.h"
#ifdef ASIC2
#include "pattern_fpga/sec_fixed_S_v2_000.h"
#include "pattern_fpga/sec_fixed_singleM_v2_000.h"
#include "pattern_fpga/sec_fixed_mM_v2_000.h"
#include "pattern_fpga/sec_random_S_v2_000.h"
#include "pattern_fpga/sec_random_singleM_v2_000.h"
#include "pattern_fpga/sec_random_mM_v2_000.h"
#include "pattern_fpga/sec_mix_MS_v2_000.h"
#include "pattern_fpga/sec_addr_unaligned_v2_000.h"
#include "pattern_fpga/sec_random_S_unaligned_v2_000.h"
#include "pattern_fpga/sec_random_S_unaligned_S_v2_000.h"
#include "pattern_fpga/sec_random_S_unaligned_S_1_16_v2_000.h"
#include "pattern_fpga/sec_trx_fifo_v2_000.h"
#else
#include "pattern_fpga/sec_fixed_S_000.h"
#include "pattern_fpga/sec_fixed_S_001.h"
#include "pattern_fpga/sec_fixed_S_002.h"
#include "pattern_fpga/sec_fixed_S_003.h"
#include "pattern_fpga/sec_fixed_S_004.h"
#include "pattern_fpga/sec_fixed_S_005.h"
#include "pattern_fpga/sec_fixed_S_006.h"
#include "pattern_fpga/sec_fixed_singleM_000.h"
#include "pattern_fpga/sec_fixed_singleM_001.h"
#include "pattern_fpga/sec_fixed_singleM_002.h"
#include "pattern_fpga/sec_fixed_singleM_003.h"
#include "pattern_fpga/sec_fixed_singleM_004.h"
#include "pattern_fpga/sec_fixed_singleM_005.h"
#include "pattern_fpga/sec_fixed_singleM_006.h"
#include "pattern_fpga/sec_fixed_mM_000.h"
#include "pattern_fpga/sec_fixed_mM_001.h"
#include "pattern_fpga/sec_fixed_mM_002.h"
#include "pattern_fpga/sec_fixed_mM_003.h"
#include "pattern_fpga/sec_fixed_mM_004.h"
#include "pattern_fpga/sec_fixed_mM_005.h"
#include "pattern_fpga/sec_fixed_mM_006.h"
#include "pattern_fpga/sec_fixed_mM_007.h"
#include "pattern_fpga/sec_random_S_000.h"
#include "pattern_fpga/sec_random_S_001.h"
#include "pattern_fpga/sec_random_S_002.h"
#include "pattern_fpga/sec_random_S_003.h"
#include "pattern_fpga/sec_random_S_004.h"
#include "pattern_fpga/sec_random_S_005.h"
#include "pattern_fpga/sec_random_S_006.h"
#include "pattern_fpga/sec_random_S_007.h"
#include "pattern_fpga/sec_random_S_008.h"
#include "pattern_fpga/sec_random_S_009.h"
#include "pattern_fpga/sec_random_singleM_000.h"
#include "pattern_fpga/sec_random_singleM_001.h"
#include "pattern_fpga/sec_random_singleM_002.h"
#include "pattern_fpga/sec_random_singleM_003.h"
#include "pattern_fpga/sec_random_mM_000.h"
#include "pattern_fpga/sec_random_mM_001.h"
#include "pattern_fpga/sec_random_mM_002.h"
#include "pattern_fpga/sec_random_mM_003.h"
#include "pattern_fpga/sec_random_mM_004.h"
#include "pattern_fpga/sec_random_mM_005.h"
#include "pattern_fpga/sec_random_mM_006.h"
#include "pattern_fpga/sec_random_mM_007.h"
#include "pattern_fpga/sec_random_mM_008.h"
#include "pattern_fpga/sec_random_mM_009.h"
#include "pattern_fpga/sec_random_mM_010.h"
#include "pattern_fpga/sec_mix_MS_000.h"
#include "pattern_fpga/sec_mix_MS_001.h"
#include "pattern_fpga/sec_mix_MS_002.h"
#include "pattern_fpga/sec_mix_MS_003.h"
#include "pattern_fpga/sec_mix_MS_004.h"
#include "pattern_fpga/sec_mix_MS_005.h"
#include "pattern_fpga/sec_addr_unaligned_000.h"
#include "pattern_fpga/sec_addr_unaligned_001.h"
#include "pattern_fpga/sec_addr_unaligned_002.h"
#include "pattern_fpga/sec_addr_unaligned_003.h"
#include "pattern_fpga/sec_addr_unaligned_004.h"
#include "pattern_fpga/sec_addr_unaligned_005.h"
#include "pattern_fpga/sec_addr_unaligned_006.h"
#include "pattern_fpga/sec_random_S_unaligned_000.h"
#include "pattern_fpga/sec_random_S_unaligned_001.h"
#include "pattern_fpga/sec_random_S_unaligned_002.h"
#include "pattern_fpga/sec_random_S_unaligned_003.h"
#include "pattern_fpga/sec_random_S_unaligned_004.h"
#include "pattern_fpga/sec_random_S_unaligned_S_000.h"
#include "pattern_fpga/sec_trx_fifo_000.h"
#include "pattern_fpga/sec_trx_fifo_001.h"
#include "pattern_fpga/sec_trx_fifo_002.h"
#include "pattern_fpga/sec_trx_fifo_003.h"
#include "pattern_fpga/sec_trx_fifo_004.h"
#include "pattern_fpga/sec_trx_fifo_005.h"
#include "pattern_fpga/sec_trx_fifo_006.h"
#include "pattern_fpga/sec_trx_fifo_007.h"
#include "pattern_fpga/sec_trx_fifo_008.h"
#include "pattern_fpga/sec_trx_fifo_009.h"
#include "pattern_fpga/sec_trx_fifo_010.h"
#include "pattern_fpga/sec_trx_fifo_011.h"
#endif

struct SEC_DMA_DESC
{
    union
    {
        unsigned int    descCfg;
        struct
        {
            unsigned short  eea_eia_mode: 5;
            unsigned short  mask: 4;
            unsigned short  last: 1;
            unsigned short  bearer: 5;
            unsigned short  direction:1;
            unsigned short  length;
        };
    };
    
    unsigned int  count;

    unsigned char  eea_Key3[4];
    unsigned char  eea_Key2[4];
    unsigned char  eea_Key1[4];
    unsigned char  eea_Key0[4];

    unsigned char  eia_Key3[4];
    unsigned char  eia_Key2[4];
    unsigned char  eia_Key1[4];
    unsigned char  eia_Key0[4];

    unsigned int  header_type: 2;
    unsigned int  reserved: 30;
};

#endif
