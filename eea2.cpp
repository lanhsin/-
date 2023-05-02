#include "AES_Encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define EEA2_PRT    //flag of print eea2 information

/* eea2_128.
 * Input key: 128 bit Confidentiality Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input bearer: 5-bit Bearer identity (in the LSB side).
 * Input dir:1 bit, direction of transmission.
 * Input data: length number of bits, input bit stream.
 * Input length: 16 bit Length, i.e., the number of bits to be encrypted or
 * decrypted.
 * Output data: Output bit stream. Assumes data is suitably memory
 * allocated.
 * Encrypts/decrypts blocks of data between 1 and 4k bits in length as
 * defined in Section 3.
 */
static void eea2_128(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint32_t blkIdx, uint32_t offset, uint8_t *dataOut)
{
	uint32_t j, m = 0, n = 0;
    uint8_t ctrBlkT[16];
    uint8_t ksBlk[16];

 	uint32_t total_len = length + offset;
	uint32_t endPos = (total_len + 127) / 128 - 1, endRes = (total_len % 128) / 8 ;  //assume that the data is byte-aligned.
	uint32_t startPos = offset / 128, startRes = (offset % 128) / 8;
	uint32_t h = 0, k = 0;

    for(j=0;j<16;j++)
    {
	if (j<4)//count
	{
	    ctrBlkT[j] = (uint8_t)(count>>(8*(3-j)));
	}
	else if (j == 4)//bearer+dir
	{
	    ctrBlkT[j] = ((uint8_t)bearer<<3) | ((uint8_t)dir<<2);
	}
	else if(j < 14)//0x00
	{
	    ctrBlkT[j] = 0;
	}
	else
	{
	    if(blkIdx<256)
	    {
			ctrBlkT[14] = 0;
			ctrBlkT[15] = blkIdx;
			break;
	    }
	    else if (blkIdx >= 256)
	    {
			ctrBlkT[14] = blkIdx / 256;
			ctrBlkT[15] = blkIdx % 256;
			break;
	    }
	}
    }
#ifdef EEA2_PRT
	printf("\n");

	printf("EEA2 Counter block%d:\n", blkIdx);
	for (j=0; j<16; j++)
	{
		printf("%02X", ctrBlkT[j]);
	}
	printf("\n");
#endif
    getKS(key, ctrBlkT, ksBlk);

#ifdef EEA2_PRT
   printf("EEA2 Keystream block%d:\n", blkIdx);
    for (j=0; j<16; j++)
    {
        printf("%02X", ksBlk[j]);
    }
    printf("\n");
#endif

#ifdef EEA2_PRT
	printf("EEA2  Plaintext block%d:\n", blkIdx);
	for (j=0; j<16; j++)
	{
		printf("%02X", data[j]);
	}
	printf("\n");
#endif

    h = (blkIdx == startPos) ? startRes : 0;
    k = (blkIdx == endPos) ? ((endRes == 0) ? 16 : endRes) : 16;
    n = (blkIdx == startPos) ? 0:((16-startRes) + (blkIdx -startPos -1)*16);

#ifdef EEA2_PRT
	 printf("EEA2	Ciphertext block%d:\n", blkIdx);
#endif  
	for (j = h, m = n; j < k ; j++, m++)
    {
		dataOut[m] = data[m] ^ ksBlk[j];
#ifdef EEA2_PRT
		printf("%02X", dataOut[m]);
#endif
    }
#ifdef EEA2_PRT
    printf("\n");
#endif
}



// eea2_128 function only processes one block data, so we need loop call it
void eea2(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint32_t offset, uint8_t *dataOut)
{
	uint32_t i = 0;
	uint32_t start_block = offset /128;
	uint32_t end_block = ( length + offset + 127 ) / 128 -1;

	for (i= start_block; i <= end_block; i++)
	{
		eea2_128(key, count, bearer, dir, data, length, i, offset, dataOut);
	}
}

/* EEA2 */


