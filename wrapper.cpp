#include "sysio.h"
#include "security.h"
#include "testpattern.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main_sec_process(FILE *file, cipherPara_S *sec_Para, SEC_REG_DESCRIPTION *sec_reg_desc)
{
    uint32_t idx = 0;

    if(secReg_get_mode(sec_reg_desc->sec_ctrl_p))
    {
        uint16_t dataLen = 0;
        while(1){
            read_mem( secReg_get_dataIn(sec_reg_desc->sec_inOut_p), sec_Para->aData, 4);

            dataLen = sec_Para->aData[2] | (sec_Para->aData[3] << 8);

            read_mem( secReg_get_dataIn(sec_reg_desc->sec_inOut_p), sec_Para->aData, dataLen/8 + SEC_DESC_SIZE);
            
            sec_cipherSdu(sec_Para, sec_reg_desc->sec_ctrl_p);
            
            sec_get_dataOut(true, sec_Para, sec_reg_desc->sec_inOut_p);

            //Shift to next dataIn ,Multiple of 4 for total dataIn len.
            secReg_set_dataIn(  sec_reg_desc->sec_inOut_p, 
                                 secReg_get_dataIn(sec_reg_desc->sec_inOut_p) + SEC_DESC_SIZE + (((dataLen/8 + 3) >> 2) << 2));

            // Log dataOut to file
            if(sec_Para->u32Mode & 0xF)
                dataLen = 4;
            else
                dataLen = dataLen /8;
            sec_log_file_dataOut(file, idx++, dataLen, secReg_get_dataOut(sec_reg_desc->sec_inOut_p));
            // Shift to next dataOut
            secReg_set_dataOut(sec_reg_desc->sec_inOut_p, secReg_get_dataOut(sec_reg_desc->sec_inOut_p));
            // Last packet
            if(sec_Para->aData[1] & 0x2)
                break;
        }
    }
    else
    {
        read_mem(   secReg_get_dataIn(sec_reg_desc->sec_inOut_p), 
                    sec_Para->aData, 
                    secReg_get_length(sec_reg_desc->sec_ctrl_p) / 8);
        
        sec_cipherSdu(sec_Para, sec_reg_desc->sec_ctrl_p);

        sec_get_dataOut(false, sec_Para, sec_reg_desc->sec_inOut_p);

        if(sec_Para->u32Mode & 0xF)
        {
            sec_log_file_dataOut(file, 0, 4, (uint8_t *) &secReg_get_xMac(sec_reg_desc->sec_inOut_p));
            /* Copy result of integrity to DV team  */
            /* Don't touch the core                  */
            memcpy( (uint8_t *)secReg_get_dataOut(sec_reg_desc->sec_inOut_p),
                    (uint8_t *)(&secReg_get_xMac(sec_reg_desc->sec_inOut_p)), 4);
        }            
        else
            sec_log_file_dataOut(   file, 0, 
                                    secReg_get_length(sec_reg_desc->sec_ctrl_p) / 8, 
                                    secReg_get_dataOut(sec_reg_desc->sec_inOut_p));
    }
}

#ifdef CO_SIM_RTK
int main_sec(UINT32 file_seed, UINT8* dataOut)
{
    int ret_val = EXIT_SUCCESS;
    cipherPara_S           sec_Para;
    SEC_REG_DESCRIPTION    sec_reg_desc;
    FILE                   *file  = NULL;

    if(sec_log_file_load(file_seed, &file) == FALSE)
        return EXIT_FAILURE;

    secReg_initialize(&sec_reg_desc);

    secReg_trigger_hw(secReg_get_dataIn(sec_reg_desc.sec_inOut_p),(UINT32)dataOut,sec_reg_desc.sec_inOut_p);
    main_sec_process(file, &sec_Para, &sec_reg_desc);
    secReg_deinitialize(&sec_reg_desc);    
    sec_log_file_unload(file);
    return ret_val;
}


#else

struct test2{
    int a;
    int b;
    int c;
};

struct test1{
    struct test2 *test2_object;
};

int main(void)
{
    uint8_t dataIn[MAX_DATA_LEN];
    uint8_t dataOut[MAX_DATA_LEN];
    cipherPara_S           sec_Para;
    SEC_REG_DESCRIPTION    sec_reg_desc;

    FILE                   *file  = NULL;

    secReg_initialize(&sec_reg_desc);

    if(sec_log_file_load(0, &file) == false)
    {
        secReg_deinitialize(&sec_reg_desc);
        return EXIT_FAILURE;
    }

    if(sec_profile_read(dataIn, sec_reg_desc.sec_ctrl_p) == false)
    {
        secReg_deinitialize(&sec_reg_desc);
        printf("lanhsin %d\n", __LINE__);
        sec_log_file_unload(file);
        return EXIT_FAILURE;
    }

    secReg_trigger_hw((uint32_t)dataIn,(uint32_t)dataOut,sec_reg_desc.sec_inOut_p);

    secReg_set_mode(sec_reg_desc.sec_ctrl_p, 0);

    if(secReg_get_mode(sec_reg_desc.sec_ctrl_p))
        sec_master_profile_read(dataIn, sec_reg_desc.sec_ctrl_p);

    main_sec_process(file, &sec_Para, &sec_reg_desc);

    secReg_deinitialize(&sec_reg_desc);
    sec_log_file_unload(file);

    return EXIT_SUCCESS;
}

#endif

