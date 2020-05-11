/*
 * security_tp_types.h
 *
 *  Created on: June 12, 2015
 *      Author: jimmycheng
 */

#ifndef _SECURITY_TP_TYPES_H_
#define _SECURITY_TP_TYPES_H_

/* Used to keep last valid value*/
 typedef struct
{
    unsigned char IsMasterMode;
    unsigned char IsLastPacket;
    unsigned int StartRdAddr;
    unsigned int StartWrAddr;
} struct_sec_reg_pattern;

typedef struct
{
    const unsigned int    IsMasterMode_size;
    const unsigned char  *IsMasterMode_ptr;
    const unsigned int    IsLastPacket_size;
    const unsigned char  *IsLastPacket_ptr;
    const unsigned int    RxFIFOAlmostEmpty_size;
    const unsigned char  *RxFIFOAlmostEmpty_ptr;
    const unsigned int    TxFIFOAlmostFull_size;
    const unsigned char  *TxFIFOAlmostFull_ptr;
    const unsigned int    StartRdAddr_size;
    const unsigned char  *StartRdAddr_ptr;
    const unsigned int    StartWrAddr_size;
    const unsigned char  *StartWrAddr_ptr;
    const unsigned int    Descriptor_size;
    const unsigned char  *Descriptor_ptr;
    const unsigned int    Data_In_size;
    const unsigned char  *Data_In_ptr;
    const unsigned int    Data_Out_size;
    const unsigned char  *Data_Out_ptr;
} struct_sec_test_case_macro;

#endif /* _SECURITY_TP_TYPES_H_ */
