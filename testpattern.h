/*
 * testpattern.h
 *
 *  Created on: 20/01/2015
 *      Author: Lanhsin
 */
#ifndef TESTPATTERN_H_
#define TESTPATTERN_H_

#include "sec_reg.h"

extern BOOL sec_log_file_load(UINT32 file_seed, FILE **file);
extern void sec_log_file_unload(FILE *file);
extern void sec_log_file_dataOut(FILE *file, UINT32 idx, UINT32 len, UINT32 dataOut);

extern BOOL sec_profile_read (UINT8* dataIn, SEC_CTRL* reg_ctrl_p);
extern BOOL sec_master_profile_read (UINT8* dataIn, SEC_CTRL* reg_ctrl_p);


#endif /* TESTPATTERN_H_ */
