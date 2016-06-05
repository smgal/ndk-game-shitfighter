
#ifndef __UCONFIG_H__
#define __UCONFIG_H__

namespace config
{
	struct TRect
	{
		int x, y, w, h;
	};

	extern const TRect c_mapWindowRegion;
	extern const TRect c_consoleWindowRegion;
	extern const TRect c_statusWindowRegion;

	extern const int c_wFont;
	extern const int c_hFont;

}

//?? 이 아래 부분에 대해서는 위치 고려 해야 함

namespace AvejUtil = avej_lite::util;

namespace avej_
{
	typedef unsigned short pixel;
}

#include <stdio.h>

#if defined(_WIN32)
#	define SPRINTF _snprintf
#else
#	define SPRINTF snprintf
#endif

class IntToStr
{
	char m_s[32];
public:
	IntToStr(int value)
	{
		SPRINTF(m_s, 32, "%d", value);
	}
	const char* operator()(void)
	{
		return m_s;
	}
};

#endif
