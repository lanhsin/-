#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "sec_type.h"
#include "sec_reg.h"

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
	UINT32  u32Len;
	UINT32  u32Dir;          /* The dir bit shall be 0 for uplink and 1 for downlink */
	UINT32  u32Bearer;
	UINT32  u32Mode;
	UINT32  u32Count;
	UINT8   aEeaKey[16];
	UINT8   aEiaKey[16];
	UINT8   aData[MAX_DATA_LEN];
	UINT8   aMac[4];
	UINT32  u32Offset;  //used to perform ciphering segment by segment
	UINT8   dataOut[MAX_DATA_LEN];
} cipherPara_S;

extern void eia1(UINT8 *key, INT32 count, INT32 bearer, INT32 dir, UINT8 *data, UINT32 length, UINT8 *outMac);
extern void eia2(UINT8 *key, INT32 count, INT32 bearer, INT32 dir, UINT8 *data, INT32 length, UINT8 *outMac);
extern void eia3(UINT8 *key, INT32 count, INT32 bearer, INT32 dir, UINT8 *data, INT32 length, UINT8 *outMac);

extern void eea1(UINT8 *key, INT32  count, INT32  bearer, INT32  dir, UINT8 *data, INT32  length, UINT32  offset, UINT8  *dataOut);
extern void eea2(UINT8 *key, INT32  count, INT32  bearer, INT32  dir, UINT8 *data, INT32  length, UINT32  offset, UINT8  *dataOut);
extern void eea3(UINT8 *key, UINT32  count, UINT32  bearer, UINT32  dir, UINT8 *data, UINT32  length, UINT32  offset, UINT8  *dataOut);

extern void sec_get_dataOut(BOOL bMasterMode, cipherPara_S *secPara_p, SEC_INOUT *reg_inOut_p);
extern void sec_cipherSdu(cipherPara_S *secPara_p, SEC_CTRL    *reg_ctrl_p);

#endif  /* _SECURITY_H_ */
