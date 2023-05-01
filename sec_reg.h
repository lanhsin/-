/*
 * sec_reg.h
 *
 *  Created on: 11/03/2015
 *      Author: lanhsin
 */

#ifndef _SEC_REG_H_
#define _SEC_REG_H_

#include <cstdint>

struct SEC_CTRL
{
    /* 0x0000_0000: CIPHER_MODE */
    uint32_t    cipher_mode:1;
    uint32_t    reserved0:31;     

    /* 0x0000_0004: CIPHER_EN */
    uint32_t    cipher_en:1;
    uint32_t    reserved1:31;  

    /* 0x0000_0008: EEA_EIA_MODE */
    union
    {
        uint32_t  eea_eia_mode;
        struct
        {
            uint32_t    eia_mode:3;       /* 2:0 */
            uint32_t    reserved2:1;     /* 3 */
            uint32_t    eea_mode:2;      /* 5:4 */
            uint32_t    reserved3:26;   /* 31:6 */
        };
    };
    /* 0x0000_000C: EEA_KEY_0 */
    uint32_t    eea_Key0;

    /* 0x0000_0010: EEA_KEY_1 */
    uint32_t    eea_Key1;

    /* 0x0000_0014: EEA_KEY_2 */
    uint32_t    eea_Key2;

    /* 0x0000_0018: EEA_KEY_3 */
    uint32_t    eea_Key3;

    /* 0x0000_001C: EIA_KEY_0 */
    uint32_t    eia_Key0;

    /* 0x0000_0020: EIA_KEY_1 */
    uint32_t    eia_Key1;

    /* 0x0000_0024: EIA_KEY_2 */
    uint32_t    eia_Key2;

    /* 0x0000_0028: EIA_KEY_3 */
    uint32_t    eia_Key3;

    /* 0x0000_002C: COUNT */
    uint32_t    count;

    /* 0x0000_0030: CONFIG */
    union
    {
        uint32_t    secCfg;
        struct
        {
            uint32_t    length : 16;          /* 15:0 */
            uint32_t    direction : 1;        /* 16 */
            uint32_t    bearer : 5;           /* 21:17 */
            uint32_t    last_package : 1; /* 22 */
            uint32_t    reserved4 : 1;      /* 23 */
            uint32_t    header_type : 2;  /* 25:24 */
            uint32_t    reserved5 : 6;      /* 31:26 */
        };
    };
};

struct TX_FIFO_INT
{
    uint32_t    tx_almostempty : 1;
    uint32_t    tx_empty : 1;
    uint32_t    tx_almostfull : 1;
    uint32_t    tx_full : 1;
    uint32_t    reserved0 : 28;
};


struct SEC_INTR
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
            uint32_t                int_raw;

            /* 0x0050 */
            uint32_t                int_mask;

            /* 0x0054 */
            uint32_t                int_sts;
        };
    };
};


/* 0x0058 -- 0x006C */
struct SEC_FIFO_CTRL
{
    /* 0x0058 */
    uint32_t rx_almostfull;
    /* 0x005C */
    uint32_t rx_almostempty;
    /* 0x0060 */
    uint32_t tx_almostfull;
    /* 0x0064 */
    uint32_t tx_almostempty;
    /* 0x0068 */
    uint32_t uplink_count : 16;
    uint32_t reserved0 : 16;
    /* 0x006C */
    uint32_t downlink_count : 16;
    uint32_t reserved1 : 16;
};


struct SEC_INOUT
{
    /* 0x00F0 */
    uint32_t  rd_addr;    
    /* 0x00F4 */
    uint32_t  wr_addr;    
    /* 0x00F8 */
    uint32_t  x_mac;
};


struct SEC_REG_DESCRIPTION
{
    SEC_CTRL    *sec_ctrl_p;
    SEC_INTR    *sec_intr_p;
    SEC_FIFO_CTRL*sec_fifo_p;
    SEC_INOUT   *sec_inOut_p;
};


#define SEC_DESC_SIZE                       44

typedef struct
{
    union
    {
        uint32_t    descCfg;
        struct
        {
            uint16_t  eea_eia_mode: 5;
            uint16_t  mask: 4;
            uint16_t  last: 1;
            uint16_t  bearer: 5;
            uint16_t  direction:1;
            uint16_t  length;
        };
    };
    
    uint32_t  count;

    uint8_t  eea_Key3[4];
    uint8_t  eea_Key2[4];
    uint8_t  eea_Key1[4];
    uint8_t  eea_Key0[4];

    uint8_t  eia_Key3[4];
    uint8_t  eia_Key2[4];
    uint8_t  eia_Key1[4];
    uint8_t  eia_Key0[4];

    uint32_t  header_type: 2;
    uint32_t  reserved: 30;
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

void secReg_initialize (SEC_REG_DESCRIPTION *sec_reg_desc_p);
void secReg_deinitialize(SEC_REG_DESCRIPTION *sec_reg_desc_p);
void secReg_trigger_hw(uint32_t dataIn, uint32_t dataOut, SEC_INOUT *sec_inOut_p);

void secReg_set_eea_key (SEC_CTRL *sec_ctrl_p, uint32_t key[]);
void secReg_get_eea_key (SEC_CTRL *sec_ctrl_p, uint32_t key[]);

void secReg_set_eia_key (SEC_CTRL *sec_ctrl_p, uint32_t key[]);
void secReg_get_eia_key (SEC_CTRL *sec_ctrl_p, uint32_t key[]);


#endif /* _SEC_REG_H_ */
