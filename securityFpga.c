/*
 * securityFpga.c
 *
 *  Created on: 12/06/2015
 *      Author: Lanhsin
 */
#include <stdlib.h>
#include "securityFpga.h"

#if defined(FPGA_SIM_RTK)
#include "sysio.h"
#else
#include <string.h>
#endif

static void
SecTest_trans_InOutAddr_to_cfg( SECURITY_Config_t *cfg, 
                                            struct_sec_reg_pattern *reg_pattern_p, 
                                            const struct_sec_test_case_macro *test_macro_p)
{
    if(test_macro_p->StartRdAddr_size)
        cfg->src_data = (unsigned char *)(reg_pattern_p->StartRdAddr);
    else
        cfg->src_data = NULL;

    if(test_macro_p->StartWrAddr_size)
        cfg->dest_data = (unsigned char *)(reg_pattern_p->StartWrAddr);
    else
        cfg->dest_data = NULL;
}

static void
SecTest_trans_desc_to_cfg(SECURITY_Config_t *cfg,  SEC_DMA_DESC *desc_p)
{
    cfg->length = desc_p->length;
    cfg->direction = desc_p->direction;
    cfg->bearer = desc_p->bearer;
    cfg->last_package = 1;
    cfg->header_type = 0;
    cfg->count = desc_p->count;

    cfg->sec_mode = (desc_p->eea_eia_mode & 0x7) | ((desc_p->eea_eia_mode & 0x18) << 1);
    if(cfg->sec_mode & 0x7)
    {
        memcpy(cfg->key, desc_p->eia_Key0, 4);
        memcpy(cfg->key+4, desc_p->eia_Key1, 4);
        memcpy(cfg->key+8, desc_p->eia_Key2, 4);
        memcpy(cfg->key+12, desc_p->eia_Key3, 4);
    }
    else
    {
        memcpy(cfg->key, desc_p->eea_Key0, 4);
        memcpy(cfg->key+4, desc_p->eea_Key1, 4);
        memcpy(cfg->key+8, desc_p->eea_Key2, 4);
        memcpy(cfg->key+12, desc_p->eea_Key3, 4);
    }
}


inline static void
SecTest_write_Indata_to_mem(unsigned int RdAddr, const unsigned char* DataIn, unsigned int size)
{

#if defined(FPGA_SIM_RTK)
    unsigned char InBuf[8189] = {0};
    memcpy(InBuf, DataIn, size);
    write_mem(RdAddr, InBuf, size);
#else
    unsigned int i;
    for(i = 0; i < size; RdAddr++)
        HWREGB(RdAddr) = DataIn[i++];
#endif
}


inline static tBoolean
SecTest_cmp_Outdata_from_mem(unsigned int WrAddr, const unsigned char* DataOut, unsigned int size)
{
    unsigned int i,j, remainding_len = size;
    unsigned char WrData[4];

    for(i = 0; (i + 4) <= size; i += 4)
    {
    #if defined(FPGA_SIM_RTK)
        read_mem(WrAddr, WrData, 4);
    #else
        for(j = 0; j < 4; j++)
            WrData[j] = HWREGB(WrAddr+j);
    #endif
        if(memcmp(WrData, DataOut, 4))
            return false;
        else
        {
            WrAddr+=4;
            DataOut+=4;
            remainding_len -=4;
        }
    }

#if defined(FPGA_SIM_RTK)
    if(remainding_len == 3)
    {
        read_mem(WrAddr++, WrData, 1);
        read_mem(WrAddr, WrData+1, 2);
    }
    else if(remainding_len != 0)
    {
        read_mem(WrAddr, WrData, remainding_len);
    }
#else
    for(j = 0; j < remainding_len; j++)
        WrData[j] = HWREGB(WrAddr+j);
#endif


    if(memcmp(WrData, DataOut, remainding_len))
        return false;
    else
        return true;
}


static tBoolean 
SecTest_Slave_mode(    struct_sec_reg_pattern *reg_pattern_p, 
                                const struct_sec_test_case_macro *test_macro_p)
{
    unsigned int retmac;
    SECURITY_Config_t cfg;
    tBoolean bIntegrity = false;

    SEC_DMA_DESC *desc_p = (SEC_DMA_DESC *)test_macro_p->Descriptor_ptr;

    if(desc_p->eea_eia_mode & 0x7)
        bIntegrity = true;
    else
        bIntegrity = false;

    if(test_macro_p->IsMasterMode_size)
        Plat_Security_SetDma_Mode(test_macro_p->IsMasterMode_ptr[0]);
    if(test_macro_p->RxFIFOAlmostEmpty_size)
        Plat_Security_SetRxFIFO_AlmostEmpty(test_macro_p->RxFIFOAlmostEmpty_ptr[0]);
    if(test_macro_p->TxFIFOAlmostFull_size)
        Plat_Security_SetTxFIFO_AlmostFull(test_macro_p->TxFIFOAlmostFull_ptr[0]);

    SecTest_trans_InOutAddr_to_cfg(&cfg, reg_pattern_p, test_macro_p);
    SecTest_trans_desc_to_cfg(&cfg, desc_p);

    SecTest_write_Indata_to_mem(reg_pattern_p->StartRdAddr, test_macro_p->Data_In_ptr, (cfg.length) >> 3);

    Plat_Security_ClearPollingBit();    // This bit will set in default.
    Plat_Security_SetCfg(&cfg);

    if(bIntegrity)
        Plat_Security_GetData(&retmac);
    else
        Plat_Security_GetData(NULL);

    if(bIntegrity)
    {
        if(memcmp((unsigned char*)(&retmac), test_macro_p->Data_Out_ptr, test_macro_p->Data_Out_size))
            return false;
    }
    else
    {
        if( false ==
            SecTest_cmp_Outdata_from_mem(reg_pattern_p->StartWrAddr, test_macro_p->Data_Out_ptr, (cfg.length) >> 3))
            return false;
    }

    return true;
}


static tBoolean  
SecTest_Master_mode(  unsigned int StartIdx, unsigned int EndIdx,
                                struct_sec_reg_pattern *reg_pattern_p,
                                const struct_sec_test_case_macro *test_macro)
{
    unsigned int i, read_p;

    if(test_macro[StartIdx].IsMasterMode_size)
        Plat_Security_SetDma_Mode(test_macro[StartIdx].IsMasterMode_ptr[0]);
    if(test_macro[StartIdx].RxFIFOAlmostEmpty_size)
        Plat_Security_SetRxFIFO_AlmostEmpty(test_macro[StartIdx].RxFIFOAlmostEmpty_ptr[0]);
    if(test_macro[StartIdx].TxFIFOAlmostFull_size)
        Plat_Security_SetTxFIFO_AlmostFull(test_macro[StartIdx].TxFIFOAlmostFull_ptr[0]);

    /* Write descriptor and data to StartRdAddr */
    for(i = StartIdx, read_p = reg_pattern_p->StartRdAddr; i < EndIdx; i++)
    {
        SecTest_write_Indata_to_mem(read_p, test_macro[i].Descriptor_ptr, test_macro[i].Descriptor_size);
        read_p += test_macro[i].Descriptor_size;
        SecTest_write_Indata_to_mem(read_p, test_macro[i].Data_In_ptr, test_macro[i].Data_In_size);
        read_p += test_macro[i].Data_In_size;
    }

    Plat_Security_SetMst(reg_pattern_p->StartRdAddr, reg_pattern_p->StartWrAddr);
    Plat_Security_GetData(NULL);

    /* Read data from StartWrAddr */
    for(i = StartIdx, read_p = reg_pattern_p->StartWrAddr; i < EndIdx; i++)
    {
        if( false ==
            SecTest_cmp_Outdata_from_mem(read_p, test_macro[i].Data_Out_ptr, test_macro[i].Data_Out_size))
            return false;
        else
            read_p += test_macro[i].Data_Out_size;
    }

    return true;
}


static void
SecTest_dump_HeaderRegister(void)
{
    unsigned int i, u32Data;
    for(i = SEC_MMR_BASE; i < (SEC_MMR_BASE+0xFC); i+= 4)
    {
#if defined(FPGA_SIM_RTK)
        read_reg(i, &u32Data, 4);
#else
        u32Data = HWREG(i);
#endif
        printf("reg[%x] = %04x\n", i, u32Data);
    }
}

static unsigned int
SecTest_remap_Addr(unsigned int Addr)
{

#ifdef ASIC2
    if(Addr >= 0x00000000 && Addr <= 0x03ffffff)
        Addr = 0x06000000 + (Addr & 0xff);
    else if(Addr >= 0x04000000 && Addr <= 0x05ffffff)
        Addr = 0x07000000 + (Addr & 0xff);
    else if(Addr >= 0x1d100000 && Addr <= 0x1d2fffff)
        Addr = 0x06000000 + (Addr & 0xff);
    else if(Addr >= 0x1d140000 && Addr <= 0x1d14ffff)
        Addr = 0x07000000 + (Addr & 0xff);
#else
    // Because DDR is not ready, change DDR address to SRAM address.
    if(Addr >= 0x00000000 && Addr <= 0x03ffffff)
        Addr = 0x1d100000 + (Addr & 0xff);
    else if(Addr >= 0x04000000 && Addr <= 0x05ffffff)
        Addr = 0x1d140000 + (Addr & 0xff);
#endif

#if !defined(FPGA_SIM_RTK)
    // For read FPGA use logical address. 
    if(Addr < 0xA0000000)
        Addr += 0xA0000000;
#endif
    return Addr;
}

static void
SecTest_fill_internal_struct( struct_sec_reg_pattern *reg_pattern,
                                       const struct_sec_test_case_macro *test_macro_p)
{
    if(test_macro_p->IsMasterMode_size)
        reg_pattern->IsMasterMode = *(test_macro_p->IsMasterMode_ptr);

    if(test_macro_p->IsLastPacket_size)
        reg_pattern->IsLastPacket = *(test_macro_p->IsLastPacket_ptr);

    if(test_macro_p->StartRdAddr_size)
        reg_pattern->StartRdAddr        = SecTest_remap_Addr(*((unsigned int *)test_macro_p->StartRdAddr_ptr));

    if(test_macro_p->StartWrAddr_size)
        reg_pattern->StartWrAddr        = SecTest_remap_Addr(*((unsigned int *)test_macro_p->StartWrAddr_ptr));
}


static tBoolean 
test_T17_01_Security(const struct_sec_test_case_macro *test_macro_p, unsigned int sizeOfdvtest)
{
    unsigned int i, startIdx;
    tBoolean bStart = false;

    struct_sec_reg_pattern reg_pattern = {0};

    for(i = 0; i < sizeOfdvtest; i++)
    {
        SecTest_fill_internal_struct(&reg_pattern, test_macro_p+i);

        if(reg_pattern.IsMasterMode)
        {
            if(reg_pattern.IsLastPacket)
            {
                if(bStart == false) // single master mode
                    startIdx = i;
                else                // multiple master mode
                    bStart = false;
                if(SecTest_Master_mode(startIdx, i+1, &reg_pattern, test_macro_p) == false)
                {
                    printf("Master item %d is fail\n", i);
                    SecTest_dump_HeaderRegister();
                    return false;
                }
                else
                    printf("Master item %d is pass\n", i);
            }
            else if(bStart == false)
            {
                bStart = true;
                startIdx = i;
            }
        }
        else
        {
            if(SecTest_Slave_mode(&reg_pattern, test_macro_p+i) == false)
            {
                printf("Slave item %d is fail\n", i);
                SecTest_dump_HeaderRegister();
                return false;
            }
            else
                printf("Slave item %d is pass\n", i);
        }            
    }

    return true;
}

#ifndef ASIC2
static tBoolean 
test_T17_01_00(void)
{
    tBoolean result;
    result = test_T17_01_Security(sec_fixed_S_macro_000, sizeof(sec_fixed_S_macro_000)/sizeof(sec_fixed_S_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_001, sizeof(sec_fixed_S_macro_001)/sizeof(sec_fixed_S_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_002, sizeof(sec_fixed_S_macro_002)/sizeof(sec_fixed_S_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_003, sizeof(sec_fixed_S_macro_003)/sizeof(sec_fixed_S_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_004, sizeof(sec_fixed_S_macro_004)/sizeof(sec_fixed_S_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_005, sizeof(sec_fixed_S_macro_005)/sizeof(sec_fixed_S_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_S_macro_006, sizeof(sec_fixed_S_macro_006)/sizeof(sec_fixed_S_macro_006[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_01(void)
{
    tBoolean result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_000, sizeof(sec_fixed_singleM_macro_000)/sizeof(sec_fixed_singleM_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_001, sizeof(sec_fixed_singleM_macro_001)/sizeof(sec_fixed_singleM_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_002, sizeof(sec_fixed_singleM_macro_002)/sizeof(sec_fixed_singleM_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_003, sizeof(sec_fixed_singleM_macro_003)/sizeof(sec_fixed_singleM_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_004, sizeof(sec_fixed_singleM_macro_004)/sizeof(sec_fixed_singleM_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_005, sizeof(sec_fixed_singleM_macro_005)/sizeof(sec_fixed_singleM_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_singleM_macro_006, sizeof(sec_fixed_singleM_macro_006)/sizeof(sec_fixed_singleM_macro_006[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_02(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_fixed_mM_macro_000, sizeof(sec_fixed_mM_macro_000)/sizeof(sec_fixed_mM_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_001, sizeof(sec_fixed_mM_macro_001)/sizeof(sec_fixed_mM_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_002, sizeof(sec_fixed_mM_macro_002)/sizeof(sec_fixed_mM_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_003, sizeof(sec_fixed_mM_macro_003)/sizeof(sec_fixed_mM_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_004, sizeof(sec_fixed_mM_macro_004)/sizeof(sec_fixed_mM_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_005, sizeof(sec_fixed_mM_macro_005)/sizeof(sec_fixed_mM_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_006, sizeof(sec_fixed_mM_macro_006)/sizeof(sec_fixed_mM_macro_006[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_fixed_mM_macro_007, sizeof(sec_fixed_mM_macro_007)/sizeof(sec_fixed_mM_macro_007[0]));
    if(result == false)
        return result;
    return true;
}

static tBoolean 
test_T17_01_03(void)
{
    tBoolean result;
    result = test_T17_01_Security(sec_random_S_macro_000, sizeof(sec_random_S_macro_000)/sizeof(sec_random_S_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_001, sizeof(sec_random_S_macro_001)/sizeof(sec_random_S_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_002, sizeof(sec_random_S_macro_002)/sizeof(sec_random_S_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_003, sizeof(sec_random_S_macro_003)/sizeof(sec_random_S_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_004, sizeof(sec_random_S_macro_004)/sizeof(sec_random_S_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_005, sizeof(sec_random_S_macro_005)/sizeof(sec_random_S_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_006, sizeof(sec_random_S_macro_006)/sizeof(sec_random_S_macro_006[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_007, sizeof(sec_random_S_macro_007)/sizeof(sec_random_S_macro_007[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_008, sizeof(sec_random_S_macro_008)/sizeof(sec_random_S_macro_008[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_macro_009, sizeof(sec_random_S_macro_009)/sizeof(sec_random_S_macro_009[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_04(void)
{
    tBoolean result;
    result = test_T17_01_Security(sec_random_singleM_macro_000, sizeof(sec_random_singleM_macro_000)/sizeof(sec_random_singleM_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_singleM_macro_001, sizeof(sec_random_singleM_macro_001)/sizeof(sec_random_singleM_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_singleM_macro_002, sizeof(sec_random_singleM_macro_002)/sizeof(sec_random_singleM_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_singleM_macro_003, sizeof(sec_random_singleM_macro_003)/sizeof(sec_random_singleM_macro_003[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_05(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_random_mM_macro_000, sizeof(sec_random_mM_macro_000)/sizeof(sec_random_mM_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_001, sizeof(sec_random_mM_macro_001)/sizeof(sec_random_mM_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_002, sizeof(sec_random_mM_macro_002)/sizeof(sec_random_mM_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_003, sizeof(sec_random_mM_macro_003)/sizeof(sec_random_mM_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_004, sizeof(sec_random_mM_macro_004)/sizeof(sec_random_mM_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_005, sizeof(sec_random_mM_macro_005)/sizeof(sec_random_mM_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_006, sizeof(sec_random_mM_macro_006)/sizeof(sec_random_mM_macro_006[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_007, sizeof(sec_random_mM_macro_007)/sizeof(sec_random_mM_macro_007[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_008, sizeof(sec_random_mM_macro_008)/sizeof(sec_random_mM_macro_008[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_009, sizeof(sec_random_mM_macro_009)/sizeof(sec_random_mM_macro_009[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_mM_macro_010, sizeof(sec_random_mM_macro_010)/sizeof(sec_random_mM_macro_010[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_06(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_mix_MS_macro_000, sizeof(sec_mix_MS_macro_000)/sizeof(sec_mix_MS_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_mix_MS_macro_001, sizeof(sec_mix_MS_macro_001)/sizeof(sec_mix_MS_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_mix_MS_macro_002, sizeof(sec_mix_MS_macro_002)/sizeof(sec_mix_MS_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_mix_MS_macro_003, sizeof(sec_mix_MS_macro_003)/sizeof(sec_mix_MS_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_mix_MS_macro_004, sizeof(sec_mix_MS_macro_004)/sizeof(sec_mix_MS_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_mix_MS_macro_005, sizeof(sec_mix_MS_macro_005)/sizeof(sec_mix_MS_macro_005[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_07(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_addr_unaligned_macro_000, sizeof(sec_addr_unaligned_macro_000)/sizeof(sec_addr_unaligned_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_001, sizeof(sec_addr_unaligned_macro_001)/sizeof(sec_addr_unaligned_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_002, sizeof(sec_addr_unaligned_macro_002)/sizeof(sec_addr_unaligned_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_003, sizeof(sec_addr_unaligned_macro_003)/sizeof(sec_addr_unaligned_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_004, sizeof(sec_addr_unaligned_macro_004)/sizeof(sec_addr_unaligned_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_005, sizeof(sec_addr_unaligned_macro_005)/sizeof(sec_addr_unaligned_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_addr_unaligned_macro_006, sizeof(sec_addr_unaligned_macro_006)/sizeof(sec_addr_unaligned_macro_006[0]));
    if(result == false)
        return result;

    return true;
}

static tBoolean 
test_T17_01_08(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_trx_fifo_macro_000, sizeof(sec_trx_fifo_macro_000)/sizeof(sec_trx_fifo_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_001, sizeof(sec_trx_fifo_macro_001)/sizeof(sec_trx_fifo_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_002, sizeof(sec_trx_fifo_macro_002)/sizeof(sec_trx_fifo_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_003, sizeof(sec_trx_fifo_macro_003)/sizeof(sec_trx_fifo_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_004, sizeof(sec_trx_fifo_macro_004)/sizeof(sec_trx_fifo_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_005, sizeof(sec_trx_fifo_macro_005)/sizeof(sec_trx_fifo_macro_005[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_006, sizeof(sec_trx_fifo_macro_006)/sizeof(sec_trx_fifo_macro_006[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_007, sizeof(sec_trx_fifo_macro_007)/sizeof(sec_trx_fifo_macro_007[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_008, sizeof(sec_trx_fifo_macro_008)/sizeof(sec_trx_fifo_macro_008[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_009, sizeof(sec_trx_fifo_macro_009)/sizeof(sec_trx_fifo_macro_009[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_010, sizeof(sec_trx_fifo_macro_010)/sizeof(sec_trx_fifo_macro_010[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_trx_fifo_macro_011, sizeof(sec_trx_fifo_macro_011)/sizeof(sec_trx_fifo_macro_011[0]));
    if(result == false)
        return result;

   return true;
}

static tBoolean 
test_T17_01_09(void)
{
    tBoolean result;

    result = test_T17_01_Security(sec_random_S_unaligned_macro_000, sizeof(sec_random_S_unaligned_macro_000)/sizeof(sec_random_S_unaligned_macro_000[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_unaligned_macro_001, sizeof(sec_random_S_unaligned_macro_001)/sizeof(sec_random_S_unaligned_macro_001[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_unaligned_macro_002, sizeof(sec_random_S_unaligned_macro_002)/sizeof(sec_random_S_unaligned_macro_002[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_unaligned_macro_003, sizeof(sec_random_S_unaligned_macro_003)/sizeof(sec_random_S_unaligned_macro_003[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_unaligned_macro_004, sizeof(sec_random_S_unaligned_macro_004)/sizeof(sec_random_S_unaligned_macro_004[0]));
    if(result == false)
        return result;
    result = test_T17_01_Security(sec_random_S_unaligned_S_macro_000, sizeof(sec_random_S_unaligned_S_macro_000)/sizeof(sec_random_S_unaligned_S_macro_000[0]));
    if(result == false)
        return result;

   return true;
}
#endif

int 
#if defined(FPGA_SIM_RTK)
main_securityFpga(void)
#else
main(void)
#endif
{
    int result;

#ifdef ASIC2
    result = test_T17_01_Security(sec_fixed_S_v2_macro_000, sizeof(sec_fixed_S_v2_macro_000)/sizeof(sec_fixed_S_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_fixed_singleM_v2_macro_000, sizeof(sec_fixed_singleM_v2_macro_000)/sizeof(sec_fixed_singleM_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_fixed_mM_v2_macro_000, sizeof(sec_fixed_mM_v2_macro_000)/sizeof(sec_fixed_mM_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_random_S_v2_macro_000, sizeof(sec_random_S_v2_macro_000)/sizeof(sec_random_S_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_random_singleM_v2_macro_000, sizeof(sec_random_singleM_v2_macro_000)/sizeof(sec_random_singleM_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_random_mM_v2_macro_000, sizeof(sec_random_mM_v2_macro_000)/sizeof(sec_random_mM_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_mix_MS_v2_macro_000, sizeof(sec_mix_MS_v2_macro_000)/sizeof(sec_mix_MS_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_addr_unaligned_v2_macro_000, sizeof(sec_addr_unaligned_v2_macro_000)/sizeof(sec_addr_unaligned_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_trx_fifo_v2_macro_000, sizeof(sec_trx_fifo_v2_macro_000)/sizeof(sec_trx_fifo_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
        result = test_T17_01_Security(sec_random_S_unaligned_v2_macro_000, sizeof(sec_random_S_unaligned_v2_macro_000)/sizeof(sec_random_S_unaligned_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_random_S_unaligned_S_v2_macro_000, sizeof(sec_random_S_unaligned_S_v2_macro_000)/sizeof(sec_random_S_unaligned_S_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_Security(sec_random_S_unaligned_S_1_16_v2_macro_000, sizeof(sec_random_S_unaligned_S_1_16_v2_macro_000)/sizeof(sec_random_S_unaligned_S_1_16_v2_macro_000[0]));
    if(result == false)
        return EXIT_FAILURE;
#else
    result = test_T17_01_00();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_01();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_02();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_03();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_04();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_05();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_06();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_07();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_08();
    if(result == false)
        return EXIT_FAILURE;
    result = test_T17_01_09();
    if(result == false)
        return EXIT_FAILURE;
#endif

    return EXIT_SUCCESS;
}



