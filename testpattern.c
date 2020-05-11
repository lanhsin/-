/*
 * testpattern.c
 *
 *  Created on: 20/01/2015
 *      Author: Lanhsin
 */

#include <stdio.h>
#include <string.h>
#include "security.h"

BOOL sec_log_file_load(UINT32 file_seed, FILE **file)
{
    char file_name[50] = "sec_log_";
    sprintf(file_name+8, "%d", file_seed);
    memcpy(file_name+strlen(file_name), ".txt", 5);

    *file = fopen(file_name, "w");
    if(*file == NULL)
    {
        printf("open sec_log file fail\n");
        return FALSE;
    }
    else
        return TRUE;
}

void sec_log_file_unload(FILE *file)
{
    fclose(file);
}

void sec_log_file_dataOut(FILE *file, UINT32 idx, UINT32 len, UINT32 dataOut)
{
    UINT32  i;
    UINT8   *data_p = (UINT8 *)dataOut;

    fprintf(file, "//Result %d\n", idx);

    for (i=0; i< len; i++)
    {
        if(i%8 == 0 && i!= 0)
        {
            fprintf(file, "\n");
        }
        fprintf(file, "%02X", data_p[i]);
    }
    fprintf(file, "\n");
}

BOOL sec_profile_read (UINT8* dataIn, SEC_CTRL* reg_ctrl_p)
{
    FILE    *file;
    UINT8 	key[16];
    UINT32	i, u32buf;
    UINT32	c;

    file = fopen("pattern_cmodel\\input.txt", "r");
    if(!file)
        return FALSE;

    /* read Mode */
    fscanf(file, "%X", &u32buf);
    secReg_set_cipher_mode(reg_ctrl_p, u32buf);
    
    /* read count */
    fscanf(file, "%08X", &u32buf);    
    secReg_set_count(reg_ctrl_p, u32buf);

    /* read bearer */
    fscanf(file, "%08X", &u32buf);    
    secReg_set_bearer(reg_ctrl_p, u32buf);
	
    /* read DIR */
    fscanf(file, "%X", &u32buf);
    secReg_set_direction(reg_ctrl_p, u32buf);
	
    /* read key */
    for (i=0; i<16; i++)
    {
        fscanf(file, "%02x", &c);
        key[i] = (UINT8)c;
    }

    if(secReg_get_cipher_mode(reg_ctrl_p)& 0xF)
        secReg_set_eia_key(reg_ctrl_p, (UINT32 *)key);
    else
        secReg_set_eea_key(reg_ctrl_p, (UINT32 *)key);

    /* read length */
    fscanf(file, "%08X", &u32buf);
    secReg_set_length(reg_ctrl_p, u32buf);

    /* read plaintext */
    for (i = 0; i < (u32buf / 8); i++)
    {
        fscanf(file, "%02x", &c);
        *dataIn= (UINT8)c;
        dataIn++;
    }

    fclose(file);
    return TRUE;
}


BOOL sec_master_profile_read (UINT8* dataIn, SEC_CTRL* reg_ctrl_p)
{
    FILE    *file;
    UINT8   *write_p = dataIn;
    UINT16  len;
    UINT32	i, u32buf;

    file = fopen("input_master.txt", "r");
    if(!file)
        return FALSE;

    while(fscanf(file, "%X", &u32buf) != EOF)
    {
        /* read Mode */
        len = ((u32buf & 0xffff0000) >> 16) >> 3;

       *((UINT32 *)write_p) = u32buf;
        write_p += 4;

        /* read count */
        fscanf(file, "%X", &u32buf);
        *((UINT32 *)write_p) = u32buf;
        write_p += 4;

        len = ((len + 3) >> 2) << 2;
        for(i = 8; i< SEC_DESC_SIZE+len; i+=4)
        {
            fscanf(file, "%X", &u32buf);
    
             *((UINT32 *)write_p)= ((u32buf & 0xff000000) >> 24) |
                                   ((u32buf & 0x00ff0000) >> 8) |
                                   ((u32buf & 0x0000ff00) << 8) |
                                   ((u32buf & 0x000000ff) << 24);
            write_p+=4;
        }
    }
    printf("\n");
    fclose(file);
    return TRUE;
}


