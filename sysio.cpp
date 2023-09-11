/*
 * sysio.c
 *
 *  Created on: 06/01/2015
 *      Author: Lanhsin.lin
 */
#include "sysio.h"
#include <cstring>

void write_reg(unsigned char* addr, unsigned int data, unsigned int byte_size)
{
    memcpy(addr, &data , byte_size);
}

void read_reg(unsigned char* addr, unsigned int* data, unsigned int byte_size)
{
    memcpy(data, addr, byte_size);
}

void write_mem(unsigned char* addr, unsigned char* data, unsigned int byte_size)
{
    memcpy(addr, data, byte_size);
}

void read_mem(unsigned char* addr, unsigned char* data, unsigned int byte_size)
{
    memcpy(data, addr, byte_size);
}