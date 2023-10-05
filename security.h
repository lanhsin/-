#ifndef _SECURITY_H_
#define _SECURITY_H_

#include <stdint.h>

#define  MAX_DATA_LEN  2048

/******************************************************************************
                        Data Structure Definitions
******************************************************************************/
enum cipherAlgo
{
	EEA0 = 0,
	EEA1 = 1,
	EEA2 = 2,
	EEA3 = 3
};

enum integrityAlgo
{
	NO_INTEGRITY = 0,
	EIA0 = 1,
	EIA1 = 2,
	EIA2 = 3,
	EIA3 = 4
};

struct cipherPara_S
{
	uint32_t  u32Len;
	uint32_t  u32Dir;          /* The dir bit shall be 0 for uplink and 1 for downlink */
	uint32_t  u32Bearer;
	uint32_t  u32Mode;
	uint32_t  u32Count;
	unsigned char aEeaKey[16];
	unsigned char aEiaKey[16];
	unsigned char* aData = nullptr;
	uint32_t  u32Offset = 0;  //used to perform ciphering segment by segment
	unsigned char* dataOut = nullptr;
};

void eia1(unsigned char* key, int32_t count, int32_t bearer, int32_t dir, unsigned char* data, uint32_t length, unsigned char* outMac);
void eia2(unsigned char* key, int32_t count, int32_t bearer, int32_t dir, unsigned char* data, uint32_t length, unsigned char* outMac);
void eia3(unsigned char* key, int32_t count, int32_t bearer, int32_t dir, unsigned char* data, uint32_t length, unsigned char* outMac);

void eea1(unsigned char* key, int32_t  count, int32_t  bearer, int32_t  dir, unsigned char* data, uint32_t  length, uint32_t  offset, unsigned char* dataOut);
void eea2(unsigned char* key, int32_t  count, int32_t  bearer, int32_t  dir, unsigned char* data, uint32_t  length, uint32_t  offset, unsigned char* dataOut);
void eea3(unsigned char* key, uint32_t count, uint32_t  bearer, uint32_t  dir, unsigned char* data, uint32_t  length, uint32_t  offset, unsigned char* dataOut);

void sec_cipherSdu(cipherPara_S *secPara_p);

#endif  /* _SECURITY_H_ */
