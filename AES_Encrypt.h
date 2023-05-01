#ifndef AES_Encrypt_H_
#define AES_Encrypt_H_

#include <stdint.h>

/* use CTR type AES to caculate KS for per counter block */
void getKS(uint8_t *key, uint8_t *data, uint8_t *ret);

#endif

