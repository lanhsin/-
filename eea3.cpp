#include <stdlib.h>
#include <stdio.h>
#include "sec_type.h"
#include "ZUC.h"

//#define EEA3_PRINT
//#define EEA3_PRINT_2

void eea3(u8* key, u32 count, u32 bearer, u32 dir, u8* data, u32 length, u32 offset, u8* dataOut)
{
	u32 *KS;
	u8 IV[16];
	int h = 0, k = 0;
	int i=0, j=0, m = 0;
	
	int n = (length + offset + 31)/32;
	u32 endRes = ((length + offset) % 32) / 8;  //assume that the data is byte-aligned. 
	u32 startPos = offset / 32, startRes = (offset % 32) / 8;

	KS      = (u32 *) malloc(n*sizeof(u32));
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

	#ifdef EEA3_PRINT
		printf("EEA3 Key:\n");
		for (i = 0;i < 16;i++)
		{
			printf("%02X ", key[i]);
		}
		printf("\n");
	#endif

#ifdef EEA3_PRINT
		printf("EEA3 IV:\n");
		for (i = 0;i < 16;i++)
		{
			printf("%02X", IV[i]);
		}
		printf("\n");
#endif	

	ZUC(key,IV,KS,n);

#ifdef EEA3_PRINT
	printf("EEA3 KS:\n");
	for (i = 0; i < n;i++)
	{
		printf("%08X\n", KS[i]);
	}
#endif

    for (i = startPos; i < n; i++)
    {
#ifdef EEA3_PRINT_2
    printf("cipher block[%d] = ", i);
#endif
	h = (i == startPos) ? startRes : 0;
	k = (i == n - 1) ? ((endRes == 0) ? 4 : endRes) : 4;

	for (j = h; j < k; j++, m++)
	{
	    dataOut[m] = data[m] ^ (u8)(*(KS+i)>>((3-j)*8));
#ifdef EEA3_PRINT_2
	    printf("%02X", dataOut[m]);
#endif
	}
#ifdef EEA3_PRINT_2
	printf("\n");
#endif
    }

    free(KS);
}
