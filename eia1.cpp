#include "SNOW_3G.h"
#include "log.h"
#include <cinttypes>    // PRIx64
#include <math.h>
#include <memory>


/* go the XiaoGang's Procedure */
#define XIAOGANG 1

#ifdef XIAOGANG
uint64_t     PM[8][256];
#endif

/* MUL64x.
* Input V: a 64-bit input.
* Input c: a 64-bit input.
* Output : a 64-bit output.
* A 64-bit memory is allocated which is to be freed by the calling
* function.
* See section 4.3.2 for details.
*/
static uint64_t MUL64x(uint64_t V, uint64_t c)
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
static uint64_t MUL64xPOW(uint64_t V, uint8_t i, uint64_t c)
{
    if (i == 0)
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
static uint64_t MUL64(uint64_t V, uint64_t P, uint64_t c)
{
    uint64_t result = 0;
    for (auto i = 0; i < 64; i++)
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
static uint32_t mask32bit(int n)
{
    uint32_t mask = 0x0;
    if ( n%32 == 0 )
        return 0xffffffff;
    while (n--)
        mask = (mask>>1) ^ 0x80000000;
    return mask;
}


#ifdef XIAOGANG
static void Pre_Mul_P(uint64_t P)
{
    uint8_t      i, j, k;
    uint64_t     r = 0x1b;

    for (i = 0; i < 8; i++)
        PM[i][0] = 0;

    PM[0][1] = P;
    for (i = 1; i <= 63; i++)
    {
        PM[i >> 3][1 << (i & 0x07)] = PM[(i-1) >> 3][1 << ((i - 1) & 0x07)] << 1;
        if (PM[(i-1) >> 3][1 << ((i - 1) & 0x07)] >> 63 == 1)
            PM[i >> 3][1 << (i & 0x07)] ^= r;
    }
    for (i = 0; i <= 7; i++)
    {
        for (j = 1; j <= 7; j++)
        {
            for (k = 1; k <= (1 << j) - 1; k++)
                PM[i][(1 << j) + k] = PM[i][1 << j] ^ PM[i][k];
        }
    }
}

static uint64_t Mul_P(uint64_t X)
{
    uint8_t u8X[8];
    uint8_t i;
    uint64_t u64Out = 0;

    for (i = 0; i < 8; i++)
        u8X[i] = (uint8_t)(X >> (64 - 8*(i+1)));

    for (i = 0; i < 8; i++)
        u64Out ^= PM[i][u8X[7-i]];

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
static void f9( uint8_t* key, int count, int fresh, int dir, uint8_t *data, uint32_t length, uint8_t *retMac)
{
    uint32_t K[4], IV[4], z[5];
    uint32_t i = 0, n_msg = (length + 31) /32;
    uint64_t V, M_D_2;

    std::unique_ptr<uint32_t[]> message = std::make_unique<uint32_t[]>(n_msg);
    for(i = 0; i < n_msg; i++)
    {/* To operate 32 bit message internally. */
        message[i] = (data[i*4+0] << 24) + (data[i*4+1] << 16) + (data[i*4+2] << 8) + data[i*4+3]; 
        sec_log(LogLevel::DEBUG, "message[%d] = %x\n", i, message[i]);
    }
    /* Load the Integrity Key for SNOW3G initialization as in section 4.4. */
    K[3] = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];
    K[2] = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
    K[1] = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
    K[0] = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
    /* Prepare the Initialization Vector (IV) for SNOW3G initialization as in section 4.4. */
    IV[3] = count;
    IV[2] = fresh;
    IV[1] = count ^ ( dir << 31 ) ;
    IV[0] = fresh ^ (dir << 15);
    z[0] = z[1] = z[2] = z[3] = z[4] = 0;
    /* Run SNOW 3G to produce 5 keystream words z_1, z_2, z_3, z_4 and z_5. */
    Initialize(K,IV);
    GenerateKeystream(5,z);
    //eia1_print add by zhangyuanliang
    for (i = 0; i < 4; i++)
    {
        sec_log(LogLevel::INFO, "K[%d] = %0x\n", i, K[i]);
        sec_log(LogLevel::INFO, "IV[%d] = %0x\n", i, IV[i]);
    }
    for(i = 0; i < 5; i++)
        sec_log(LogLevel::INFO, "z[%d] = %08X\n", i, z[i]);	

    //P = (z[0] << 32) + z[1];
    uint64_t P = (uint64_t)z[0] << 32 | (uint64_t)z[1];
    uint64_t Q = (uint64_t)z[2] << 32 | (uint64_t)z[3];
    /* Calculation */
    uint32_t D = (length+63)/64 + 1;
    uint64_t EVAL = 0;
    uint64_t c = 0x1b;
#ifdef XIAOGANG
    Pre_Mul_P(P);
#endif
    sec_log(LogLevel::DEBUG, "D = %08X\n", D);
    /* for 0 <= i <= D-3 */
    for (i = 0; i < D-2; i++)
    {
        V = EVAL ^ ( (uint64_t)message[2*i] << 32 | (uint64_t)message[2*i+1] );
        sec_log(LogLevel::DEBUG, "V = 0x%016" PRIx64 "", V>>32);
        sec_log(LogLevel::DEBUG, "0x%016" PRIx64 "\n", V);
#ifndef XIAOGANG
        EVAL = MUL64(V,P,c);
#else
        EVAL = Mul_P(V);
#endif
        sec_log(LogLevel::DEBUG, "EVAL = 0x%016" PRIx64 "", EVAL>>32);
        sec_log(LogLevel::DEBUG, "0x%016" PRIx64 "\n", EVAL);
    }
    /* for D-2 */
    int rem_bits = length % 64;
    if (rem_bits == 0)
        rem_bits = 64;
    uint32_t mask = mask32bit(rem_bits%32);
    if (rem_bits > 32)
        M_D_2 = ( (uint64_t) message[2*(D-2)] << 32 ) | (uint64_t) (message[2*(D-2)+1] & mask);
    else
        M_D_2 = ( (uint64_t) message[2*(D-2)] & mask) << 32 ;

    V = EVAL ^ M_D_2;
#ifdef XIAOGANG
    EVAL = Mul_P(V);
#else
    EVAL = MUL64(V,P,c);
#endif
    sec_log(LogLevel::DEBUG, "EVAL = 0x%016" PRIx64 "", EVAL>>32);
    sec_log(LogLevel::DEBUG, "0x%016" PRIx64 "\n", EVAL);
    /* for D-1 */
    EVAL ^= length;
    sec_log(LogLevel::DEBUG, "MEVAL = 0x%016" PRIx64 "", EVAL>>32);
    sec_log(LogLevel::DEBUG, "0x%016" PRIx64 "\n", EVAL);

    /* Multiply by Q */
    EVAL = MUL64(EVAL,Q,c);
    sec_log(LogLevel::DEBUG, "QEVAL = 0x%016" PRIx64 "", EVAL>>32);
    sec_log(LogLevel::DEBUG, "0x%016" PRIx64 "\n", EVAL);

    uint32_t MAC_I = (uint32_t)(EVAL >> 32) ^ z[4];

    for(i = 0; i < 4; i++)
        retMac[i] = (uint8_t)(MAC_I >> (3-i)*8 );

    return;
}

/* EIA1 */
/* FRESH [0], ...FRESH [31] shall be replaced by BEARER[0], ...BEARER[4]  | 0^27 */
void eia1(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *retMac)
{
    int fresh = bearer<<27;
    f9(key, count, fresh, dir, data, length, retMac);
    return;
}

/* End of f9.c */
/*------------------------------------------------------------------------*/