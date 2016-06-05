
#include "util_convert_to_ucs.h"

extern unsigned short UHC2UCS2_TABLE_0[];
extern unsigned short UHC2UCS2_TABLE_1[];
extern unsigned short UHC2UCS2_TABLE_2[];
extern unsigned short UHC2UCS2_TABLE_3[];

typedef struct
{
	unsigned char   minLoByte;
	unsigned char   maxLoByte;
	unsigned char   minHiByte;
	unsigned char   maxHiByte;
	unsigned short* pTable;
} CONVERT_BLOCK;

static CONVERT_BLOCK s_table[] =
{
	{ 0x41, 0x5A, 0x81, 0xC6, UHC2UCS2_TABLE_0 },
	{ 0x61, 0x7A, 0x81, 0xC5, UHC2UCS2_TABLE_1 },
	{ 0x81, 0xA0, 0x81, 0xC5, UHC2UCS2_TABLE_2 },
	{ 0xA1, 0xFE, 0x81, 0xFD, UHC2UCS2_TABLE_3 }
};

long s_FetchUHC(const char*& pSrc, const char* pEnd)
{
	unsigned long result;

	if (*pSrc & 0x80)
	{
		if (pSrc+1 >= pEnd)
			return -1;

		result = (unsigned char)(*pSrc++);
		result = (result << 8) | (unsigned char)(*pSrc++);
	}
	else
	{
		if (pSrc >= pEnd)
			return -1;

		result = *pSrc++;
	}

	return long(result);
}

TUCS2 ConvertUHC2UCS2(unsigned char byte1, unsigned char byte2, TUCS2 errChar)
{
	if (byte1 & 0x80)
	{
		for (unsigned int ixTable = 0; ixTable < sizeof(s_table) / sizeof(s_table[0]); ixTable++)
		{
			CONVERT_BLOCK& table = s_table[ixTable];

			if ((byte2 >= table.minLoByte) && (byte2 <= table.maxLoByte) &&
				(byte1 >= table.minHiByte) && (byte1 <= table.maxHiByte))
			{
				byte2  -= table.minLoByte;
				byte1  -= table.minHiByte;
				return table.pTable[byte1 * (table.maxLoByte - table.minLoByte + 1) + byte2];
			}
		}

		return errChar;
	}
	else
	{
		return TUCS2(byte1);
	}

}

TUCS2* ConvertUHC2UCS2(TUCS2* pDst, int dstSize, const char* pSrc, int srcSize, TUCS2 errChar)
{
	if ((pDst == 0) || (dstSize <= 0) || (pSrc == 0) || (srcSize <= 0))
		return 0;

	const char*  pSrcEnd = pSrc + srcSize;
	TUCS2*       pDstEnd = pDst + dstSize;

	while (pDst < pDstEnd)
	{
		long data = s_FetchUHC(pSrc, pSrcEnd);

		if (data >= 0)
		{
			if (data < 0x80)
			{
				*pDst++ = TUCS2(data);
			}
			else
			{
				unsigned char LoByte = data & 0xFF;
				unsigned char hiByte = (data >> 8) & 0xFF;

				*pDst = errChar;

				for (unsigned int ixTable = 0; ixTable < sizeof(s_table) / sizeof(s_table[0]); ixTable++)
				{
					CONVERT_BLOCK& table = s_table[ixTable];

					if ((LoByte >= table.minLoByte) && (LoByte <= table.maxLoByte) &&
					    (hiByte >= table.minHiByte) && (hiByte <= table.maxHiByte))
					{
						LoByte  -= table.minLoByte;
						hiByte  -= table.minHiByte;
						*pDst++  = table.pTable[hiByte * (table.maxLoByte - table.minLoByte + 1) + LoByte];
						break;
					}
				}
			}
			continue;
		}
		break;
	}

	return pDst;
}
