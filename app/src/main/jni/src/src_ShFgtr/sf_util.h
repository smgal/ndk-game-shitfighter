
#ifndef __SF_UTIL_H__
#define __SF_UTIL_H__

#include <stdlib.h>

#if 1
	#include <cassert>
	#include <cmath>
	#define SF_ASSERT(cond) assert(cond)
	#define SF_SQRT(a)      sqrt(a)
	#define SF_SIN(a)       sin(a)
#else
	#define SF_ASSERT(cond)
	#define SF_SQRT(a)      0
	#define SF_SIN(a)       0
#endif

#define assigned_(cond) (cond)

#define round_(val)  int(val + 0.5)

template <typename T>
int ord_(T value)
{
	return (int)value;
}

template <typename T>
void inc_(T& value)
{
	int temp = (int)value;
	value = (T)(temp+1);
}

template <typename T>
void inc_(T& value, int amount)
{
	value += amount ;
}

template <typename T>
void dec_(T& value)
{
	--value;
}

template <typename T>
void dec_(T& value, int amount)
{
	value -= amount ;
}

template <typename T>
T succ_(T value)
{
	return T(int(value)+1);
}

template <typename T>
T pred_(T value)
{
	return T(int(value)-1);
}

#define random(max_val) (rand() % (max_val))

template <typename T>
T max(T a, T b)
{
	return (a > b) ? a : b;
}

template <typename T>
T min(T a, T b)
{
	return (a < b) ? a : b;
}

template <typename T>
T clamp(T a, T min, T max)
{
	return (a < min) ? min : (a > max) ? max : a;
}

namespace util
{
	void ClearKeyBuffer(void);
	void ComposeString(char pBuffer[], const char* szFormat, const int data ...);

	class CTextFileFromRes
	{
	public:
		CTextFileFromRes(const char* sz_text_stream, int size);
		~CTextFileFromRes();

		bool IsValid(void);
		bool GetString(char* out_str, int str_len);

	private:
		struct TImpl;
		TImpl* m_p_impl;
	};
}

#endif // #ifndef __SF_UTIL_H__
