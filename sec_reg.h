/*
 * sec_reg.h
 *
 *  Created on: 11/03/2015
 *      Author: lanhsin
 */

#ifndef _SEC_REG_H_
#define _SEC_REG_H_

#include "sec_type.h"

typedef struct SEC_CTRL_TAG
{
    /* 0x0000_0000: CIPHER_MODE */
    UINT32    cipher_mode:1;
    UINT32    reserved0:31;     

    /* 0x0000_0004: CIPHER_EN */
    UINT32    cipher_en:1;
    UINT32    reserved1:31;  

    /* 0x0000_0008: EEA_EIA_MODE */
    union
    {
        UINT32  eea_eia_mode;
        struct
        {
            UINT32    eia_mode:3;       /* 2:0 */
            UINT32    reserved2:1;     /* 3 */
            UINT32    eea_mode:2;      /* 5:4 */
            UINT32    reserved3:26;   /* 31:6 */
        };
    };
    /* 0x0000_000C: EEA_KEY_0 */
    UINT32    eea_Key0;

    /* 0x0000_0010: EEA_KEY_1 */
    UINT32    eea_Key1;

    /* 0x0000_0014: EEA_KEY_2 */
    UINT32    eea_Key2;

    /* 0x0000_0018: EEA_KEY_3 */
    UINT32    eea_Key3;

    /* 0x0000_001C: EIA_KEY_0 */
    UINT32    eia_Key0;

    /* 0x0000_0020: EIA_KEY_1 */
    UINT32    eia_Key1;

    /* 0x0000_0024: EIA_KEY_2 */
    UINT32    eia_Key2;

    /* 0x0000_0028: EIA_KEY_3 */
    UINT32    eia_Key3;

    /* 0x0000_002C: COUNT */
    UINT32    count;

    /* 0x0000_0030: CONFIG */
    union
    {
        UINT32    secCfg;
        struct
        {
            UINT32    length : 16;          /* 15:0 */
            UINT32    direction : 1;        /* 16 */
            UINT32    bearer : 5;           /* 21:17 */
            UINT32    last_package : 1; /* 22 */
            UINT32    reserved4 : 1;      /* 23 */
            UINT32    header_type : 2;  /* 25:24 */
            UINT32    reserved5 : 6;      /* 31:26 */
        };
    };
} SEC_CTRL;

typedef struct TX_FIFO_INT_TAG
{
    UINT32    tx_almostempty : 1;
    UINT32    tx_empty : 1;
    UINT32    tx_almostfull : 1;
    UINT32    tx_full : 1;
    UINT32    reserved0 : 28;
} TX_FIFO_INT;


typedef struct SEC_INTR_TAG
{
    /* 0x004C -- 0x0054 */
    union
    {
        struct
        {
            /* 0x004C */
            TX_FIFO_INT    int_raw_field;

            /* 0x0050 */
            TX_FIFO_INT    int_mask_field;

            /* 0x0054 */
            TX_FIFO_INT    int_sts_field;
        };
        struct
        {
            /* 0x004C */
            UINT32                int_raw;

            /* 0x0050 */
            UINT32                int_mask;

            /* 0x0054 */
            UINT32                int_sts;
        };
    };
} SEC_INTR;


/* 0x0058 -- 0x006C */
typedef struct SEC_FIFO_CTRL_TAG
{
    /* 0x0058 */
    UINT32 rx_almostfull;
    /* 0x005C */
    UINT32 rx_almostempty;
    /* 0x0060 */
    UINT32 tx_almostfull;
    /* 0x0064 */
    UINT32 tx_almostempty;
    /* 0x0068 */
    UINT32 uplink_count : 16;
    UINT32 reserved0 : 16;
    /* 0x006C */
    UINT32 downlink_count : 16;
    UINT32 reserved1 : 16;
} SEC_FIFO_CTRL;


typedef struct SEC_INOUT_TAG
{
    /* 0x00F0 */
    UINT32  rd_addr;    
    /* 0x00F4 */
    UINT32  wr_addr;    
    /* 0x00F8 */
    UINT32  x_mac;
} SEC_INOUT;


typedef struct SEC_REG_DESCRIPTION_TAG
{
    SEC_CTRL    *sec_ctrl_p;
    SEC_INTR    *sec_intr_p;
    SEC_FIFO_CTRL*sec_fifo_p;
    SEC_INOUT   *sec_inOut_p;
} SEC_REG_DESCRIPTION;


#define SEC_DESC_SIZE                       44

typedef struct
{
    union
    {
        UINT32    descCfg;
        struct
        {
            UINT16  eea_eia_mode: 5;
            UINT16  mask: 4;
            UINT16  last: 1;
            UINT16  bearer: 5;
            UINT16  direction:1;
            UINT16  length;
        };
    };
    
    UINT32  count;

    UINT8  eea_Key3[4];
    UINT8  eea_Key2[4];
    UINT8  eea_Key1[4];
    UINT8  eea_Key0[4];

    UINT8  eia_Key3[4];
    UINT8  eia_Key2[4];
    UINT8  eia_Key1[4];
    UINT8  eia_Key0[4];

    UINT32  header_type: 2;
    UINT32  reserved: 30;
}SEC_DMA_DESC;


#define secReg_set_mode(_reg, _val)         {_reg->cipher_mode = _val;}
#define secReg_set_cipher_mode(_reg, _val)  {_reg->eea_eia_mode = _val;}
#define secReg_set_count(_reg, _val)        {_reg->count = _val;}
#define secReg_set_bearer(_reg, _val)       {_reg->bearer = _val;}
#define secReg_set_direction(_reg, _val)    {_reg->direction = _val;}
#define secReg_set_length(_reg, _val)       {_reg->length = _val;}

#define secReg_set_dataIn(_reg, _val)       {_reg->rd_addr = _val;}
#define secReg_set_dataOut(_reg, _val)      {_reg->wr_addr = _val;}
#define secReg_set_xMac(_reg, _val)         {_reg->x_mac = _val;}


#define secReg_get_mode(_reg)               (_reg->cipher_mode)
#define secReg_get_cipher_mode(_reg)        (_reg->eea_eia_mode)
#define secReg_get_count(_reg)              (_reg->count)
#define secReg_get_bearer(_reg)             (_reg->bearer)
#define secReg_get_direction(_reg)          (_reg->direction)
#define secReg_get_length(_reg)             (_reg->length)

#define secReg_get_dataIn(_reg)             (_reg->rd_addr)
#define secReg_get_dataOut(_reg)            (_reg->wr_addr)
#define secReg_get_xMac(_reg)               (_reg->x_mac)

extern void secReg_initialize (SEC_REG_DESCRIPTION *sec_reg_desc_p);
extern void secReg_deinitialize(SEC_REG_DESCRIPTION *sec_reg_desc_p);
extern void secReg_trigger_hw(UINT32 dataIn, UINT32 dataOut, SEC_INOUT *sec_inOut_p);

extern void secReg_set_eea_key (SEC_CTRL *sec_ctrl_p, UINT32 key[]);
extern void secReg_get_eea_key (SEC_CTRL *sec_ctrl_p, UINT32 key[]);

extern void secReg_set_eia_key (SEC_CTRL *sec_ctrl_p, UINT32 key[]);
extern void secReg_get_eia_key (SEC_CTRL *sec_ctrl_p, UINT32 key[]);


#endif /* _SEC_REG_H_ */
