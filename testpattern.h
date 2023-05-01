/*
 * testpattern.h
 *
 *  Created on: 20/01/2015
 *      Author: Lanhsin
 */
#ifndef TESTPATTERN_H_
#define TESTPATTERN_H_

#include <stdint.h>
#include <cstdio>

bool sec_log_file_load(uint32_t file_seed, FILE **file);
void sec_log_file_unload(FILE *file);
void sec_log_file_dataOut(FILE *file, uint32_t idx, uint32_t len, uint32_t dataOut);

bool sec_profile_read (uint8_t* dataIn, SEC_CTRL* reg_ctrl_p);
bool sec_master_profile_read (uint8_t* dataIn, SEC_CTRL* reg_ctrl_p);


#endif /* TESTPATTERN_H_ */
