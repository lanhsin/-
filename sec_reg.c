/*
 * sec_reg.c
 *
 *  Created on: 12/03/2015
 *      Author: Lanhsin
 */
#include <stdlib.h>
#include <string.h>
#include "sysio.h"
#include "sec_reg.h"


void secReg_initialize (SEC_REG_DESCRIPTION *sec_reg_desc_p)
{
    sec_reg_desc_p->sec_ctrl_p= (SEC_CTRL *)malloc(sizeof(SEC_CTRL));
    memset(sec_reg_desc_p->sec_ctrl_p, 0x00, sizeof(SEC_CTRL));

    sec_reg_desc_p->sec_intr_p= (SEC_INTR *)malloc(sizeof(SEC_INTR));
    memset(sec_reg_desc_p->sec_intr_p, 0x00, sizeof(SEC_INTR));
    
    sec_reg_desc_p->sec_inOut_p= (SEC_INOUT *)malloc(sizeof(SEC_INOUT));
    memset(sec_reg_desc_p->sec_inOut_p, 0x00, sizeof(SEC_INOUT));
#ifdef CO_SIM_RTK
    read_multi_regs(SEC_CONTROL_REG, sec_reg_desc_p->sec_ctrl_p, sizeof(SEC_CTRL));
    read_multi_regs(SEC_IOOUT_REG, sec_reg_desc_p->sec_inOut_p, sizeof(SEC_INOUT));
#endif
}

void secReg_deinitialize(SEC_REG_DESCRIPTION *sec_reg_desc_p)
{
    free(sec_reg_desc_p->sec_ctrl_p);
    free(sec_reg_desc_p->sec_intr_p);
    free(sec_reg_desc_p->sec_inOut_p);
}

void secReg_trigger_hw(UINT32 dataIn, UINT32 dataOut, SEC_INOUT *sec_inOut_p)
{
    secReg_set_dataIn(sec_inOut_p, dataIn);
    secReg_set_dataOut(sec_inOut_p, dataOut);
    secReg_set_xMac(sec_inOut_p, dataOut);
}

void secReg_set_eea_key (SEC_CTRL *sec_ctrl_p, UINT32 key[])
{
    sec_ctrl_p->eea_Key0 = key[0];
    sec_ctrl_p->eea_Key1 = key[1];
    sec_ctrl_p->eea_Key2 = key[2];
    sec_ctrl_p->eea_Key3 = key[3];
}

void secReg_get_eea_key (SEC_CTRL *sec_ctrl_p, UINT32 key[])
{
    key[0] = sec_ctrl_p->eea_Key0;
    key[1] = sec_ctrl_p->eea_Key1;
    key[2] = sec_ctrl_p->eea_Key2;
    key[3] = sec_ctrl_p->eea_Key3;
}

void secReg_set_eia_key (SEC_CTRL *sec_ctrl_p, UINT32 key[])
{
    sec_ctrl_p->eia_Key0 = key[0];
    sec_ctrl_p->eia_Key1 = key[1];
    sec_ctrl_p->eia_Key2 = key[2];
    sec_ctrl_p->eia_Key3 = key[3];
}

void secReg_get_eia_key (SEC_CTRL *sec_ctrl_p, UINT32 key[])
{
    key[0] = sec_ctrl_p->eia_Key0;
    key[1] = sec_ctrl_p->eia_Key1;
    key[2] = sec_ctrl_p->eia_Key2;
    key[3] = sec_ctrl_p->eia_Key3;
}



