/*
 * sysio.h
 *
 *  Created on: 06/01/2015
 *      Author: Lanhsin.lin
 */

#ifndef _SYS_IO_H_
#define _SYS_IO_H_


void write_reg(unsigned char* addr, unsigned int data, unsigned int byte_size);
void read_reg(unsigned char* addr, unsigned int* data, unsigned int byte_size);

void write_mem(unsigned char* addr, unsigned char* data, unsigned int byte_size);
void read_mem(unsigned char* addr, unsigned char* data, unsigned int byte_size);

#endif /* _SYS_IO_H_ */
