
#ifndef __UTIL_CONVERT_TO_UCS_H__
#define __UTIL_CONVERT_TO_UCS_H__

typedef unsigned short TUCS2;

TUCS2* ConvertUHC2UCS2(TUCS2* pDst, int dstSize, const char* pSrc, int srcSize, TUCS2 errChar = 0x20);

#endif
