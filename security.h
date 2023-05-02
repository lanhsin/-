#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "sec_reg.h"
#include <stdint.h>

#define  MAX_DATA_LEN   (8189 + SEC_DESC_SIZE)

/******************************************************************************
                        Data Structure Definitions
******************************************************************************/
typedef enum
{
	EEA0 = 0,
	EEA1 = 1,
	EEA2 = 2,
	EEA3 = 3
}cipherAlgo;

typedef enum
{
	NO_INTEGRITY = 0,
	EIA0 = 1,
	EIA1 = 2,
	EIA2 = 3,
	EIA3 = 4
}integrityAlgo;


typedef struct
{
	uint32_t  u32Len;
	uint32_t  u32Dir;          /* The dir bit shall be 0 for uplink and 1 for downlink */
	uint32_t  u32Bearer;
	uint32_t  u32Mode;
	uint32_t  u32Count;
	uint8_t   aEeaKey[16];
	uint8_t   aEiaKey[16];
	uint8_t   aData[MAX_DATA_LEN];
	uint8_t   aMac[4];
	uint32_t  u32Offset;  //used to perform ciphering segment by segment
	uint8_t   dataOut[MAX_DATA_LEN];
} cipherPara_S;

void eia1(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *outMac);
void eia2(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *outMac);
void eia3(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *outMac);

void eea1(uint8_t *key, int32_t  count, int32_t  bearer, int32_t  dir, uint8_t *data, uint32_t  length, uint32_t  offset, uint8_t  *dataOut);
void eea2(uint8_t *key, int32_t  count, int32_t  bearer, int32_t  dir, uint8_t *data, uint32_t  length, uint32_t  offset, uint8_t  *dataOut);
void eea3(uint8_t *key, uint32_t  count, uint32_t  bearer, uint32_t  dir, uint8_t *data, uint32_t  length, uint32_t  offset, uint8_t  *dataOut);

void sec_get_dataOut(bool bMasterMode, cipherPara_S *secPara_p, SEC_INOUT *reg_inOut_p);
void sec_cipherSdu(cipherPara_S *secPara_p, SEC_CTRL *reg_ctrl_p);

#endif  /* _SECURITY_H_ */
