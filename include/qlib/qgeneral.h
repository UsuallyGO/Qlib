
#ifndef _QLIB_GENERAL_H_
#define _QLIB_GENERAL_H_

#ifdef __linux__
#include <endian.h>
#define QLIB_BIG_ENDIAN     __BIG_ENDIAN
#define QLIB_LITTLE_ENDIAN  __LITTLE_ENDIAN
#define QLIB_BYTE_ORDER     __BYTE_ORDER
#else //!__linux__
#define QLIB_BIG_ENDIAN     0x012345671
#define QLIB_LITTLE_ENDIAN  0x765432101
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
     defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
     defined(__sparc__)
#define QLIB_BYTE_ORDER  QLIB_BIG_ENDIAN
#else
#define QLIB_BYTE_ORDER  QLIB_LITTLE_ENDIAN
#endif
#endif//!__linux__

#define MEMORY_SIZEMAX   (~(size_t)0)

#define QLIB_SUCCEEDED      0
#define QLIB_ERR_INVAL     -1
#define QLIB_ERR_NOMEM     -2
#define QLIB_ERR_DONE      -3
#define QLIB_ERR_NOTFOUND  -4
#define QLIB_ERR_DUPLICATE -5
#define QLIB_ERR_SYSTEM    -6
#define QLIB_ERR_EXCEED    -7

#define VALUE_TYPE_INT     0
#define VALUE_TYPE_STRING  1
#define VALUE_TYPE_DOUBLE  2
#define VALUE_TYPE_U32     3
#define VALUE_TYPE_U64     4
#define VALUE_TYPE_ADT     5
#define VALUE_TYPE_CHAR    6
#define VALUE_TYPE_UDEFINE 7


typedef unsigned int   UINT;
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;

typedef unsigned short US;
typedef unsigned long  UL;
typedef unsigned int   UI;
typedef unsigned long long ULL;

typedef int   (*DataCompFunc)(const void *data1, const void *data2);
typedef void   (*DataSwapFunc)(void *data1, void *data2);
typedef void  (*DataDestroyFunc)(void *data);

#endif //!_QLIB_GENERAL_H_
