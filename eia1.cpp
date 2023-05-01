#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "sec_type.h"
#include "SNOW_3G.h"

//#define EIA1_PRINT
/* print out switch */
//#define PRT 1

/* go the XiaoGang's Procedure */
#define XIAOGANG 1

#ifdef XIAOGANG
u64     PM[8][256];
#endif

/* MUL64x.
* Input V: a 64-bit input.
* Input c: a 64-bit input.
* Output : a 64-bit output.
* A 64-bit memory is allocated which is to be freed by the calling
* function.
* See section 4.3.2 for details.
*/
static u64 MUL64x(u64 V, u64 c)
{
	if ( V & 0x8000000000000000 )
		return (V << 1) ^ c;
	else
		return V << 1;
}
/* MUL64xPOW.
* Input V: a 64-bit input.
* Input i: a positive integer.
* Input c: a 64-bit input.
* Output : a 64-bit output.
* A 64-bit memory is allocated which is to be freed by the calling
function.
* See section 4.3.3 for details.
*/
static u64 MUL64xPOW(u64 V, u8 i, u64 c)
{
	if ( i == 0)
		return V;
	else
		return MUL64x( MUL64xPOW(V,i-1,c) , c);
}
/* MUL64.
* Input V: a 64-bit input.
* Input P: a 64-bit input.
* Input c: a 64-bit input.
* Output : a 64-bit output.
* A 64-bit memory is allocated which is to be freed by the calling
* function.
* See section 4.3.4 for details.
*/
static u64 MUL64(u64 V, u64 P, u64 c)
{
	u64 result = 0;
	int i = 0;
	for ( i=0; i<64; i++)
	{
		if( ( P>>i ) & 0x1 )
			result ^= MUL64xPOW(V,i,c);
	}
	return result;
}
/* mask32bit.
* Input n: an integer in 1-32.
* Output : a 32 bit mask.
* Prepares a 32 bit mask with required number of 1 bits on the MSB side.
*/
static u32 mask32bit(int n)
{
	u32 mask=0x0;
	if ( n%32 == 0 )
		return 0xffffffff;
	while (n--)
		mask = (mask>>1) ^ 0x80000000;
	return mask;
}


#ifdef XIAOGANG
static void Pre_Mul_P(u64 P)
{
    u8      i,j,k;
    u64     r = 0x1b;

    for (i = 0; i < 8; i++)
    {
        PM[i][0] = 0;
    }
    PM[0][1] = P;
    for (i=1; i<=63; i++)
    {
        PM[i >> 3][1 << (i & 0x07)] = PM[(i-1) >> 3][1 << ((i - 1) & 0x07)] << 1;
        if (PM[(i-1) >> 3][1 << ((i - 1) & 0x07)] >> 63 == 1)
        {
            PM[i >> 3][1 << (i & 0x07)] ^= r;
        }
    }
    for (i=0; i<=7; i++)
    {
        for (j=1; j<=7; j++)
        {
            for (k=1; k <= (1 << j) - 1; k++)
            {
                PM[i][(1 << j) + k] = PM[i][1 << j] ^ PM[i][k];
            }
        }
    }
}

static u64 Mul_P(u64 X)
{
    u8      u8X[8];
    u8      i;
    u64     u64Out = 0;

    for (i=0; i<8; i++)
    {
        u8X[i] = (u8)(X >> (64 - 8*(i+1)));
    }
    for (i=0; i<8; i++)
    {
//	printf("PM[%d][%d] = %08X", i, u8X[7-i], PM[i][u8X[7-i]] >> 32);
//	printf("%08X\n", (u32)PM[i][u8X[7-i]]);
        u64Out ^= PM[i][u8X[7-i]];
    }

    return u64Out;
}
#endif


/* f9.
* Input key: 128 bit Integrity Key.
* Input count:32-bit Count, Frame dependent input.
* Input fresh: 32-bit Random number.
* Input dir:1 bit, direction of transmission (in the LSB).
* Input data: length number of bits, input bit stream.
* Input length: 64 bit Length, i.e., the number of bits to be MAC'd.
* Output : 32 bit block used as MAC
* Generates 32-bit MAC using UIA2 algorithm as defined in Section 4.
*/
static void f9( u8* key, int count, int fresh, int dir, u8 *data, u32 length, u8 *retMac)
{
	u32 K[4],IV[4], z[5];
	u32 i=0,D,n_msg = (length+31)/32;
	u32 MAC_I = 0; /* memory for the result */
	u64 EVAL;
	u64 V;
	u64 P;
	u64 Q;
	u64 c;
	u64 M_D_2;
	int rem_bits = 0;
	u32 mask = 0;
	u32 *message;

	message = (u32*)malloc(sizeof(u32)*n_msg);
	for(i=0; i<n_msg; i++)
	{/* To operate 32 bit message internally. */
		*(message+i) = (data[i*4+0] << 24) + (data[i*4+1] << 16) + (data[i*4+2] << 8) + data[i*4+3]; 
#ifdef PRT
		printf("message[%d] = %x\n", i, message[i]);
#endif
	}
	/* Load the Integrity Key for SNOW3G initialization as in section 4.4. */
	K[3] = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];
	K[2] = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
	K[1] = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
	K[0] = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
#ifdef PRT
	for(i=0; i<4; i++)
	{
		printf("K[%d] = %0x\n", i, K[i]);
	}
#endif
	/* Prepare the Initialization Vector (IV) for SNOW3G initialization as in section 4.4. */
	IV[3] = count;
	IV[2] = fresh;
	IV[1] = count ^ ( dir << 31 ) ;
	IV[0] = fresh ^ (dir << 15);
#ifdef PRT
	for(i=0; i<4; i++)
	{
		printf("IV[%d] = %0x\n", i, IV[i]);
	}
#endif
	z[0] = z[1] = z[2] = z[3] = z[4] = 0;
	/* Run SNOW 3G to produce 5 keystream words z_1, z_2, z_3, z_4 and z_5. */
	Initialize(K,IV);
	//eia1_print add by zhangyuanliang
	#ifdef EIA1_PRINT
		for (i=0;i<4;i++)
		{
			printf("K%d = %0x\n", i, K[i]);
			printf("IV%d = %0x\n", i, IV[i]);
		}
	#endif
	GenerateKeystream(5,z);
	//eia1_print add by zhangyuanliang
	#ifdef EIA1_PRINT
		for(i=0; i<5; i++)	
		{		
			printf("z[%d] = %08X\n", i, z[i]);	
		}
	#endif
	//P = (z[0] << 32) + z[1];
	P = (u64)z[0] << 32 | (u64)z[1];
	Q = (u64)z[2] << 32 | (u64)z[3];
	/* Calculation */
	D = (length+63)/64 + 1;
	EVAL = 0;
	c = 0x1b;
#ifdef XIAOGANG
        Pre_Mul_P(P);
#endif
#ifdef PRT
	printf("D = %08X\n", D);
#endif
	/* for 0 <= i <= D-3 */
	for (i=0;i<D-2;i++)
	{
		V = EVAL ^ ( (u64)message[2*i] << 32 | (u64)message[2*i+1] );
#ifdef PRT
		printf("V = 0x%016"PRIx64"", V>>32);
		printf("0x%016"PRIx64"\n", V);
#endif
#ifndef XIAOGANG
		EVAL = MUL64(V,P,c);
#else
		EVAL = Mul_P(V);
#endif
#ifdef PRT
	printf("EVAL = 0x%016"PRIx64"", EVAL>>32);
	printf("0x%016"PRIx64"\n", EVAL);
#endif
	}
	/* for D-2 */
	rem_bits = length % 64;
	if (rem_bits == 0)
		rem_bits = 64;
	mask = mask32bit(rem_bits%32);
	if (rem_bits > 32)
	{
		M_D_2 = ( (u64) message[2*(D-2)] << 32 ) | (u64) (message[2*(D-2)+1] & mask);
	} 
	else
	{
		M_D_2 = ( (u64) message[2*(D-2)] & mask) << 32 ;
	}
//#ifdef PRT
//	printf("M_D_2 = %x", M_D_2>>32);
//	printf("%x\n", M_D_2);
//#endif
	V = EVAL ^ M_D_2;
#ifdef XIAOGANG
	EVAL = Mul_P(V);
#else
	EVAL = MUL64(V,P,c);
#endif
#ifdef PRT
	printf("EVAL = 0x%016"PRIx64"", EVAL>>32);
	printf("0x%016"PRIx64"\n", EVAL);
#endif
	/* for D-1 */
	EVAL ^= length;
#ifdef PRT
	printf("MEVAL = 0x%016"PRIx64"", EVAL>>32);
	printf("0x%016"PRIx64"\n", EVAL);
#endif
	/* Multiply by Q */
	EVAL = MUL64(EVAL,Q,c);
#ifdef PRT
	printf("QEVAL = 0x%016"PRIx64"", EVAL>>32);
	printf("0x%016"PRIx64"\n", EVAL);
#endif
	MAC_I = (u32)(EVAL >> 32) ^ z[4];
//#ifdef PRT
//	for(i=0; i<5; i++)
//	{
//		printf("z[%d] = %0x\n", i, z[i]);
//	}
//	printf("P = %x", P>>32);
//	printf("%x\n", P);
//	printf("Q = %x", Q>>32);
//	printf("%x\n", Q);
//	printf("EVAL = %x", EVAL>>32);
//	printf("%x\n", EVAL);
//#endif
	free(message);
	for(i=0; i<4; i++)
	{
		retMac[i] = (u8)(MAC_I >> (3-i)*8 );
	}
	return;
}

/* EIA1 */
/* FRESH [0], ...FRESH [31] shall be replaced by BEARER[0], ...BEARER[4]  | 0^27 */
void eia1(UINT8 *key, INT32 count, INT32 bearer, INT32 dir, UINT8 *data, UINT32 length, UINT8 *retMac)
{
    int fresh=0;

    fresh = bearer<<27;
    f9(key, count, fresh, dir, data, length, retMac);
    return;
}

/* End of f9.c */
/*------------------------------------------------------------------------*/


