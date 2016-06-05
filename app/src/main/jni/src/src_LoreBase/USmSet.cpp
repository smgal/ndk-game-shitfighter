
#include "USmSet.h"

#define NULL 0

int   CSmSet::m_strlen(const char* pStr)
{
	if (pStr == NULL)
		return 0;

	char* pCh = (char*)pStr;
	int len = 0;

	while (*pCh++)
		++len;

	return len;
}

char* CSmSet::m_strchr(const char* pStr, char ch, int n)
{
	if (pStr == NULL)
		return NULL;

	char* pCh = (char*)pStr;

	while (*pCh && --n >= 0)
	{
		if (*pCh == ch)
			return pCh;
		++pCh;
	}

	return NULL;
}

int CSmSet::m_atoi(const char* pStr, int n)
{
	if (pStr == NULL)
		return 0;

	char* pCh = (char*)pStr;
	int result = 0;

	while (*pCh && --n >= 0)
	{
		if (*pCh < '0' || *pCh > '9')
			return 0;

		result = result * 10 + (*pCh++ - '0');
	}

	return result;
}

#define SET_SET(i) set[(i)/8] |= 1 << ((i)%8);

bool CSmSet::m_makeSetArray(const char* szStream, TSet& set)
{
	int i;

	for (i = 0; i < sizeof(set); i++)
		set[i] = 0;

	const char* pStr = szStream;
	char* pCh;

	while (pCh = m_strchr(pStr, ',', 100000))
	{
		char* pCh2 = m_strchr(pStr, '.', pCh - pStr);

		if (pCh2 == NULL)
		{
			i = m_atoi(pStr, pCh - pStr);
			SET_SET(i)
			pStr = pCh + 1; 
		}
		else
		{
			int iBegin = m_atoi(pStr, pCh2 - pStr);
			int iEnd   = m_atoi(pCh2+2, pCh - pCh2 - 2);
			for (int i = iBegin; i <= iEnd; i++)
				SET_SET(i)
			pStr = pCh + 1; 
		}
	}

	char* pCh2 = m_strchr(pStr, '.', 100000);

	if (pCh2 == NULL)
	{
		i = m_atoi(pStr, 100000);
		SET_SET(i)
	}
	else
	{
		int iBegin = m_atoi(pStr, pCh2 - pStr);
		int iEnd   = m_atoi(pCh2+2, 100000);
		for (int i = iBegin; i <= iEnd; i++)
			SET_SET(i)
	}

	return false;
}

CSmSet::CSmSet(void)
{
	for (int i = 0; i < sizeof(m_set); i++)
		m_set[i] = 0;
}

CSmSet::CSmSet(const char* szStream)
{
	this->Assign(szStream);
}

void CSmSet::Assign(const char* szStream)
{
	int len = m_strlen(szStream);

	if (len > 0)
	{
		char* szText = new char[len+1];
		if (szText == NULL)
			return;

		char* pDest = szText;

		for (int i = 0; i < len; i++)
		{
			if (szStream[i] != ' ')
				*pDest++ = szStream[i];
		}

		*pDest = 0;

		m_makeSetArray(szText, m_set);

		delete[] szText;
	}
}

///////////////////////////////////////

#if 0

#include <windows.h>
#include <stdio.h>

void CSet_test()
{
	char* szString[] = 
	{
		"9..12,17,13,100,180..185,255",
		"0,9..12,17,13,100,180..185,255",
		"255,0, 9..12, 17, 13,100,180..185",
	};

	for (int loop = 0; loop < sizeof(szString) / sizeof(szString[0]); loop++)
	{
		char s[256];

		sprintf(s, "+++ loop = %d\n", loop);
		OutputDebugString(s);

		OutputDebugString("source string: ");
		OutputDebugString(szString[loop]);
		OutputDebugString("\n");

		CSmSet set(szString[loop]);

		for (int i = 0; i < 256; i++)
		{
			if (set.IsSet(i))
			{
				sprintf(s, "%d, ", i);
				OutputDebugString(s);
			}
		}
		OutputDebugString("\n");
	}
}

#endif
