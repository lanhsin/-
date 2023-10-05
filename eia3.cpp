#include "ZUC.h"
#include "log.h"
#include <memory>

static uint32_t GET_WORD(uint32_t * DATA, uint32_t i)
{
	uint32_t WORD;
	uint32_t ti = i % 32;
	if (ti == 0)
		WORD = DATA[i/32];
	else
		WORD = (DATA[i/32]<<ti) | (DATA[i/32+1]>>(32-ti));
	return WORD;
}

static uint8_t GET_BIT(uint8_t * DATA, uint32_t i)
{
	return (DATA[i/8] & (1<<(7-(i%8)))) ? 1 : 0;
}

void eia3(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *outMac)
{
	uint32_t L = (length+31) / 32 + 2; 
	uint32_t T = 0, retMac = 0;
	uint32_t i;
	uint8_t IV[16];
	
	std::unique_ptr<uint32_t[]> KS = std::make_unique<uint32_t[]>(L);
	IV[0]  = (count>>24) & 0xFF;
	IV[1]  = (count>>16) & 0xFF;
	IV[2]  = (count>>8) & 0xFF;
	IV[3]  = count & 0xFF;
	IV[4]  = (bearer << 3) & 0xF8;
	IV[5]  = 0;
	IV[6]  = 0;
	IV[7]  = 0;
	IV[8]  = IV[0] ^ ((dir&1) << 7);
	IV[9]  = IV[1];
	IV[10] = IV[2];
	IV[11] = IV[3];
	IV[12] = IV[4];
	IV[13] = IV[5];
	IV[14] = IV[6] ^((dir&1) << 7);
	IV[15] = IV[7];

	sec_log(LogLevel::INFO, "EIA3 Key:\n");
	for (i = 0; i < 16; i++)
		sec_log(LogLevel::INFO, "%02X ", key[i]);
	sec_log(LogLevel::INFO, "\nEIA3 IV:\n");
	for (i = 0; i < 16; i++)
		sec_log(LogLevel::INFO, "%02X", IV[i]);

	ZUC(key, IV, KS.get(), L);

	sec_log(LogLevel::INFO, "\nEIA3 KS:\n");
	for (i = 0; i < L; i++)
		sec_log(LogLevel::INFO, "%08X\n", KS[i]);

	for (i = 0; i < length; i++)
	{	
		if (GET_BIT(data, i)) {
			sec_log(LogLevel::DEBUG, "T_0x%08x ^ GET_WORD(KS,%d)_0x%08x = 0x%08x\n", T, i, GET_WORD(KS.get(), i), T^GET_WORD(KS.get(), i));
			T ^= GET_WORD(KS.get(), i);			
		}
	}
		
	T ^= GET_WORD(KS.get(), length);
	retMac = T ^ KS[L-1];

	for(i = 0; i < 4; i++)
		outMac[i] = (uint8_t)(retMac >> (3-i)*8 );

	sec_log(LogLevel::DEBUG, "\n");
}

