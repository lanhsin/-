
#include "SNOW_3G.h"
#include "log.h"
#include <memory>


/* eea1.
* Input key: 128 bit Confidentiality Key.
* Input count:32-bit Count, Frame dependent input.
* Input bearer: 5-bit Bearer identity (in the LSB side).
* Input dir:1 bit, direction of transmission.
* Input data: length number of bits, input bit stream.
* Input length: 16 bit Length, i.e., the number of bits to be encrypted or
* decrypted.
* Output data: Output bit stream. Assumes data is suitably memory
* allocated.
* Encrypts/decrypts blocks of data between 1 and 20000 bits in length as
* defined in Section 3.
*/
void eea1(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint32_t offset, uint8_t *dataOut)
{
	uint32_t K[4], IV[4];
	uint32_t n = (length + offset  + 31 ) / 32, sum = 0;
	uint32_t endRes = ((length + offset) % 32) / 8;  //assume that the data is byte-aligned. 
	uint32_t startPos = offset / 32, startRes = (offset % 32) / 8;
	uint32_t h = 0, k = 0;
	uint32_t i = 0, j = 0, m = 0;

	/*Initialisation*/
	/* Load the confidentiality key for SNOW 3G initialization as in section
	3.4. */
	K[3] = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];
	K[2] = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
	K[1] = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
	K[0] = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];

	/* Prepare the initialization vector (IV) for SNOW 3G initialization as insection 3.4. */
	IV[3] = count;
	IV[2] = (bearer << 27) | ((dir & 0x1) << 26);
	IV[1] = IV[3];
	IV[0] = IV[2];

	//eea1_print add by zhangyuanliang
	for (i = 0; i < 4; i++)
	{
		sec_log(LogLevel::INFO, "K[%d] = %0x\n", i, K[i]);
		sec_log(LogLevel::INFO, "IV[%d] = %0x\n", i, IV[i]);
	}

	/* Run SNOW 3G algorithm to generate sequence of key stream bits KS*/
	Initialize(K,IV);

	std::unique_ptr<uint32_t[]> KS = std::make_unique<uint32_t[]>(n);
	GenerateKeystream(n, KS.get());
	/* Exclusive-OR the input data with keystream to generate the output bit stream */

	sec_log(LogLevel::DEBUG, "EEA1 KS:\n");
	for (i = startPos; i < n; i++)
	{
	    sec_log(LogLevel::DEBUG, "cipher block[%d] = ", i);
	    h = (i == startPos) ? startRes : 0;
	    k = (i == n - 1) ? ((endRes == 0) ? 4 : endRes) : 4;

	    for (j = h; j < k; j++, m++)
	    {
			dataOut[m] = data[m] ^ (uint8_t)(KS[i]>>((3-j)*8));
			sec_log(LogLevel::DEBUG, "%02X", dataOut[m]);

			sum += 8;
			if (sum >= length)
			    break;
	    }
	    sec_log(LogLevel::DEBUG, "\n");
	}
}
/* End of f8.c */
