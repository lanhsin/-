#ifndef AES_Encrypt_H_
#define AES_Encrypt_H_

/* use CTR type AES to caculate KS for per counter block */
void getKS(UINT8 *key, UINT8 *data, UINT8 *ret);

#endif

