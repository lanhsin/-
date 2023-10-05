#include "AES_Encrypt.h"
#include "log.h"
#include <memory>
#include <string>

/* For CMAC Calculation */
unsigned char const_Rb[16] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87 };

unsigned char const_Zero[16] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void xor_128(unsigned char *a, unsigned char *b, unsigned char *out)
{
	for (auto i = 0; i < 16; i++)
		out[i] = a[i] ^ b[i];
}


static void print_hex(const char *s, uint8_t* buf , int len)
{
	int i;
	sec_log(LogLevel::DEBUG, "%s ", s);
	for (i = 0; i < len; i++) {
		if ( (i % 16) == 0 && i != 0 ) 
			sec_log(LogLevel::DEBUG, " ");
		sec_log(LogLevel::DEBUG, "%02x", buf[i]);
		if ( (i % 4) == 3 ) 
			sec_log(LogLevel::DEBUG, " ");
		if ( (i % 16) == 15 )
			sec_log(LogLevel::DEBUG, "\n");
	}
	if ( (i % 16) != 0 ) 
		sec_log(LogLevel::DEBUG, "\n");
}

static void print128(const char* s, unsigned char *bytes)
{
	sec_log(LogLevel::DEBUG, "%s ", s);
	for (auto j = 0; j < 16; j++) {
		sec_log(LogLevel::DEBUG, "%02x", bytes[j]);
		if ( (j%4) == 3 ) 
			sec_log(LogLevel::DEBUG, " ");
	}
	sec_log(LogLevel::DEBUG, "\n");
}

/* AES-CMAC Generation Function */
static void leftshift_onebit(unsigned char *input,unsigned char *output)
{
	unsigned char overflow = 0;
	for (auto i = 15; i >= 0; i-- ) {
		output[i] = input[i] << 1;
		output[i] |= overflow;
		overflow = (input[i] & 0x80)?1:0;
	}
}

static void generate_subkey(unsigned char *key, unsigned char *K1, unsigned char *K2)
{
	unsigned char L[16], Z[16], tmp[16];

	for (auto i = 0; i < 16; i++)
		Z[i] = 0;

	getKS(key, Z, L);
	/* If MSB(L) = 0, then K1 = L << 1 */
	if ( (L[0] & 0x80) == 0 ) 
		leftshift_onebit(L, K1);	
	else { /* Else K1 = ( L << 1 ) (+) Rb */
		leftshift_onebit(L, tmp);
		xor_128(tmp, const_Rb, K1);
	}
	if ( (K1[0] & 0x80) == 0 )
		leftshift_onebit(K1, K2);
	else {
		leftshift_onebit(K1, tmp);
		xor_128(tmp, const_Rb, K2);
	}
	
	print128("L", L);
	print128("K1", K1);
	print128("K2", K2);;
}

static void padding (unsigned char *lastb, unsigned char *pad, int length)
{
	/* original last block */
	for (auto j = 0; j < 16; j++ ) {
		if ( (j+1)*8 < length) 
			pad[j] = lastb[j];
		else if ((j+1)*8 > length && j*8 < length)
		{
			pad[j] = lastb[j];
			pad[j] |= (uint8_t)(1<<((j+1)*8 - length - 1));
		}
		else if ( (j+1)*8 == length )
		{
			pad[j] = lastb[j];
			j++;
			pad[j] = 0x80;
		} 
		else if( (j+1)*8 > length )
			pad[j] = 0x00;
	}
}

static void AES_CMAC ( unsigned char *key, uint8_t input[], int length, unsigned char *mac, int remainder128)
{
	unsigned char X[16], Y[16], M_last[16], padded[16];
	unsigned char K1[16], K2[16];
	int i, flag;

	generate_subkey(key, K1, K2);
	int n = (length + 15) / 16; /* n is number of rounds */
	if ( n == 0 ) {
		n = 1;
		flag = 0;
	} else {
		if ( (length%16) == 0 && remainder128 == 0) /* last block is a complete block */
			flag = 1;
		else /* last block is not complete block */
			flag = 0;
	}
	if ( flag ) /* last block is complete block */
		xor_128(&input[16*(n-1)], K1, M_last);
	else {
		padding(&input[16*(n-1)], padded, remainder128);
		print128("padded", padded);
		xor_128(padded, K2, M_last);
	}
	print128("M_last", M_last);

	for (i = 0; i < 16; i++) 
		X[i] = 0;
	
	std::string str;
	for (i = 0; i < n-1; i++) {
		xor_128(X, &input[16*i], Y); /* Y := Mi (+) X */
		getKS(key, Y, X); /* X := AES-128(KEY, Y); */

		str = 'X' + std::to_string(i);
		print128(str.c_str(), X);
	}

	xor_128(X, M_last, Y);
	getKS(key, Y, X);
	print128("last Y", Y);
	print128("last X", X);
	for (i = 0; i < 4; i++ )
		mac[i] = X[i];
}


void eia2(uint8_t *key, int32_t count, int32_t bearer, int32_t dir, uint8_t *data, uint32_t length, uint8_t *outMac)
{
	int mLen = (length+64+7)/8;
	int remainder128 = (length+64)%128;
	int loopi = 0, loopj = 0;

	std::unique_ptr<uint8_t[]> M = std::make_unique<uint8_t[]>(mLen);
	/* from count & bearer & dir & data generate M*/
	for(loopi = 0; loopi < mLen; loopi++) 
	{
		if (loopi < 4)//count
			M[loopi] = (uint8_t)(count>>(8*(3-loopi)));
		else if (loopi == 4)//bearer+dir
			M[loopi] = ((uint8_t)bearer<<3) | ((uint8_t)dir<<2);
		else if(loopi < 8)//0x00
			M[loopi] = 0;
		else
		{
			M[loopi] = data[loopj];
			loopj++;
		}
	}

	sec_log(LogLevel::DEBUG, "--------------------------------------------------\n");
	print128("Key", key);
	sec_log(LogLevel::DEBUG, "\nSubkey Generation\n");

	sec_log(LogLevel::DEBUG, "\nExample : len = %d\n", mLen);
	print_hex("M", M.get(), mLen);
	
	AES_CMAC(key, M.get(), mLen, outMac, remainder128);	
	print128("AES_CMAC", outMac); 
	sec_log(LogLevel::DEBUG, "--------------------------------------------------\n");
}