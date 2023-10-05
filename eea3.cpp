#include "ZUC.h"
#include "log.h"
#include <memory>

void eea3(uint8_t* key, uint32_t count, uint32_t bearer, uint32_t dir, uint8_t* data, uint32_t length, uint32_t offset, uint8_t* dataOut)
{
	uint8_t IV[16];
	uint32_t h = 0, k = 0;
	uint32_t i = 0, j = 0, m = 0;
	
	uint32_t n = (length + offset + 31) / 32;
	uint32_t endRes = ((length + offset) % 32) / 8;  //assume that the data is byte-aligned. 
	uint32_t startPos = offset / 32, startRes = (offset % 32) / 8;

	std::unique_ptr<uint32_t[]> KS = std::make_unique<uint32_t[]>(n);
	IV[0]  = (count>>24) & 0xFF;
	IV[1]  = (count>>16) & 0xFF;
	IV[2]  = (count>>8) & 0xFF;
	IV[3]  = count & 0xFF;
	IV[4]  = ((bearer << 3) | ((dir&1)<<2)) & 0xFC;
	IV[5]  = 0;
	IV[6]  = 0;
	IV[7]  = 0;
	IV[8]  = IV[0];
	IV[9]  = IV[1];
	IV[10] = IV[2];
	IV[11] = IV[3];
	IV[12] = IV[4];
	IV[13] = IV[5];
	IV[14] = IV[6];
	IV[15] = IV[7];

	sec_log(LogLevel::INFO, "EEA3 Key:\n");
	for (i = 0; i < 16; i++)
		sec_log(LogLevel::INFO, "%02X ", key[i]);

	sec_log(LogLevel::INFO, "\nEEA3 IV:\n");
	for (i = 0; i < 16; i++)
		sec_log(LogLevel::INFO, "%02X", IV[i]);

	ZUC(key, IV, KS.get(), n);

	sec_log(LogLevel::INFO, "\nEEA3 KS:\n");
	for (i = 0; i < n; i++)
		sec_log(LogLevel::INFO, "%08X\n", KS[i]);

    for (i = startPos; i < n; i++)
    {
	    sec_log(LogLevel::DEBUG, "cipher block[%d] = ", i);
		h = (i == startPos) ? startRes : 0;
		k = (i == n - 1) ? ((endRes == 0) ? 4 : endRes) : 4;

		for (j = h; j < k; j++, m++)
		{
		    dataOut[m] = data[m] ^ (uint8_t)(KS[i]>>((3-j)*8));
	    	sec_log(LogLevel::DEBUG, "%02X", dataOut[m]);
		}
		sec_log(LogLevel::DEBUG, "\n");
    }
}
