/*********************************************************************/
// Plat_security.h 
/*********************************************************************/

#ifndef _PLAT_SECURITY_H_
#define _PLAT_SECURITY_H_

#include "secReg.h"
#include "sec_type.h"

typedef enum
{
    SECURITY_MODE_NONE = 0x00
  , SECURITY_MODE_EIA1 = 0x02
  , SECURITY_MODE_EIA2 = 0x03
  , SECURITY_MODE_EIA3 = 0x04
  , SECURITY_MODE_EEA1 = 0x10
  , SECURITY_MODE_EEA2 = 0x20
  , SECURITY_MODE_EEA3 = 0x30
} SECURITY_Mode_t;

typedef struct {
    SECURITY_Mode_t sec_mode;
    unsigned char key[16];            // key
    unsigned int count;               // count
    /* 0x0000_0030: CONFIG */
    union
    {
        unsigned int    secCfg;
        struct
        {
            unsigned int    length : 16;          /* 15:0 */
            unsigned int    direction : 1;        /* 16 */
            unsigned int    bearer : 5;           /* 21:17 */
            unsigned int    last_package : 1; /* 22 */
            unsigned int    reserved4 : 1;      /* 23 */
            unsigned int    header_type : 2;  /* 25:24 */
            unsigned int    reserved5 : 6;      /* 31:26 */
        };
    };
    unsigned char *src_data;
    unsigned char *dest_data;
} SECURITY_Config_t;

extern void Plat_Security_SetDma_Mode(BOOL value);
extern void Plat_Security_SetCfg(SECURITY_Config_t *pCipher);
extern void Plat_Security_SetMst(unsigned int RdAddr, unsigned int WrAddr);
extern void Plat_Security_GetData(unsigned int* retMac);
extern void Plat_Security_ClearPollingBit(void);

extern void Plat_Security_SetRxFIFO_AlmostEmpty(unsigned char value);
extern void Plat_Security_SetTxFIFO_AlmostFull(unsigned char value);


#endif /* _PLAT_SECURITY_H_ */


