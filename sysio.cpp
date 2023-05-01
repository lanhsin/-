/*
 * sysio.c
 *
 *  Created on: 06/01/2015
 *      Author: Lanhsin.lin
 */
#include "sysio.h"
#include <cstring>

/*
*   Fpga    : Read register settings from real platfrom.
*   Fpga-sim: Read register settings from co-sim environment.
*           : Read IO use OCP to real platform.
*           : Write IO use OCP to real platform.
*   Qemu    : Read register settings from mainline setting.
*   Co-sim  : Read register settings from co-sim environment.
*           : Read use OCP to real platform.
*           : Write IO use memcpy to storage prepared by caller.
*   C model : Read register settings from test files.
*/

#if defined(QEMU_RTK_CUS)

void read_reg( unsigned int addr, unsigned int* data, unsigned int byte_size)
{
    memcpy(data, (unsigned char*)addr, byte_size);
}

void write_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    cpu_physical_memory_write(addr, data, byte_size);
}

void read_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    cpu_physical_memory_read(addr, data, byte_size);
}

#elif defined(FPGA_SIM_RTK)

void write_reg( unsigned int addr, unsigned int data, unsigned int byte_size)
{
    svBitVecVal sv_addr, sv_data;
    sv_addr = addr;
    sv_data = data;

    write_reg_by_sv( &sv_addr, &sv_data, byte_size);
}

void read_reg( unsigned int addr, unsigned int* data, unsigned int byte_size)
{
    svBitVecVal sv_addr, sv_data;
    sv_addr = addr;

    read_reg_by_sv( &sv_addr, data, byte_size);
}

void write_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    int i = 0, offset = 0;
    svBitVecVal sv_addr;
    svBitVecVal* sv_data = malloc(MAX_QUEUE_SIZE*sizeof(svBitVecVal));  
    sv_addr = addr;

    while(1)
    {
        for( i=0; i< byte_size; i++) 
        {
            if( (i == byte_size) || (i == MAX_QUEUE_SIZE)) break;
            else sv_data[i] = data[i+offset];
        }
        write_mem_by_sv( &sv_addr, sv_data, i);

        if( byte_size > MAX_QUEUE_SIZE)
        {
            byte_size -= MAX_QUEUE_SIZE;
            offset += MAX_QUEUE_SIZE;
            sv_addr += MAX_QUEUE_SIZE;
        }
        else break;
    }

    free(sv_data);
}

void read_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    int i = 0, offset = 0;
    svBitVecVal sv_addr;
    svBitVecVal* sv_data = malloc(MAX_QUEUE_SIZE*sizeof(svBitVecVal));

    sv_addr = addr;

    while( byte_size > MAX_QUEUE_SIZE)
    {
        read_mem_by_sv( &sv_addr, sv_data, MAX_QUEUE_SIZE);
        for( i= 0; i< MAX_QUEUE_SIZE; i++){
            data[i+offset] = sv_data[i];
        }

        offset += MAX_QUEUE_SIZE;
        sv_addr += MAX_QUEUE_SIZE;
        byte_size -= MAX_QUEUE_SIZE;
    }
    read_mem_by_sv( &sv_addr, sv_data, byte_size);
    for( i= 0; i< byte_size; i++)
        data[i+offset] = sv_data[i];

    free(sv_data);
}

#elif defined(CO_SIM_RTK)

void write_reg( unsigned int addr, unsigned int data, unsigned int byte_size)
{
    memcpy(GET_ADDR_B(addr), &data , byte_size);
}

void read_reg( unsigned int addr, unsigned int* data, unsigned int byte_size)
{
    svBitVecVal sv_addr, sv_data;
    sv_addr = addr;

    read_reg_by_sv( &sv_addr, data, byte_size);
}

void write_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    memcpy(GET_ADDR_B(addr), data, byte_size);
}

void read_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    int i = 0, offset = 0;
    svBitVecVal sv_addr;
    svBitVecVal* sv_data = malloc(MAX_QUEUE_SIZE*sizeof(svBitVecVal));

    sv_addr = addr;

    while( byte_size > MAX_QUEUE_SIZE)
    {
        read_mem_by_sv( &sv_addr, sv_data, MAX_QUEUE_SIZE);
        for( i= 0; i< MAX_QUEUE_SIZE; i++){
            data[i+offset] = sv_data[i];
     }

        offset += MAX_QUEUE_SIZE;
        sv_addr += MAX_QUEUE_SIZE;
        byte_size -= MAX_QUEUE_SIZE;
    }
    read_mem_by_sv( &sv_addr, sv_data, byte_size);
    for( i= 0; i< byte_size; i++)
        data[i+offset] = sv_data[i];

    free(sv_data);
}

#else

void write_reg( unsigned int addr, unsigned int data, unsigned int byte_size)
{
    memcpy(GET_ADDR_B(addr), &data , byte_size);
}

void read_reg( unsigned int addr, unsigned int* data, unsigned int byte_size)
{
    memcpy(data, GET_ADDR_B(addr), byte_size);
}

void write_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    memcpy(GET_ADDR_B(addr), data, byte_size);
}

void read_mem( unsigned int addr, unsigned char* data, unsigned int byte_size)
{
    memcpy(data, GET_ADDR_B(addr), byte_size);
}

#endif

void read_multi_regs(unsigned int addr, unsigned int* data, unsigned int byte_size)
{
    unsigned int i = 0, read_p = 0;
    unsigned char *data_p = (unsigned char*) data;

    for(i = 0; (i + 4) <= byte_size; i += 4)
    {
        read_reg(addr, data++,  4);
        read_p += 4;
        data_p += 4;
        addr += 4;
    }

    // Remaining bytes
    if((byte_size - read_p) == 3)
    {
        read_reg(addr++, data, 1);
        read_reg(addr, (unsigned int*)(++data_p), 2);
    }
    else if(read_p != byte_size)
        read_reg(addr, data, byte_size - read_p);
    
}


