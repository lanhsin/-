#ifndef _SEC_TYPE_H_
#define _SEC_TYPE_H_

#ifndef QEMU_RTK_CUS
#include <stdio.h>

typedef  unsigned char   UINT8;	
typedef  unsigned short  UINT16;
typedef  unsigned int      UINT32;

typedef  char            INT8;
typedef  short           INT16;	
typedef  int              INT32;

#ifdef _WIN32
typedef unsigned __int64 UINT64;
#else
typedef unsigned long long UINT64;
#endif

typedef enum BOOLEAN_TAG
{
    TRUE  = (1 == 1)
  , FALSE = (0 == 1)
} BOOL;

#define BIT(x)			(1 << (x))

#else
#include "hw/sysbus.h"

typedef uint8_t           UINT8;
typedef uint16_t          UINT16;
typedef uint32_t          UINT32;
typedef int32_t           INT32;
typedef uint64_t          UINT64;
typedef gboolean          BOOL;

#endif

typedef unsigned long		UINT_ADDR;

typedef UINT8           u8;
typedef UINT32         u32;
typedef UINT64         u64;

#define PNULL			(void *)0x0

#endif /* _SEC_TYPE_H_ */
