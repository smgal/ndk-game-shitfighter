
#ifndef __PD_UTIL_H__
#define __PD_UTIL_H__

#include "avej_lite.h"

////////////////////////////////////////////////////////////////////////////////
// macro

//#define CT_ASSERT(x)    switch(0) {case 0: case x:;}
#define CT_ASSERT2(x)   typedef int __acc_cta_t[1-!(x)];

void    _WriteLog(const char* sz_str, int line);
#define WRITE_LOG(s) _WriteLog(s, __LINE__);


////////////////////////////////////////////////////////////////////////////////
// type definition

struct TPosition
{
	int x;
	int y;

	TPosition() {}
	TPosition(int _x, int _y)
	:	x(_x), y(_y) {}

	TPosition& operator=(const TPosition& ref)
	{
		this->x = ref.x;
		this->y = ref.y;

		return *this;
	}
};

struct TExtent
{
	int w, h;

	TExtent() {}
	TExtent(int _w, int _h)
	:	w(_w), h(_h) {}
};

struct TBound
{
	int x1, y1;
	int x2, y2;

	TBound() {}
	TBound(int _x1, int _y1, int _x2, int _y2)
	:	x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
};

struct TDirection
{
	int dx;
	int dy;

	TDirection()
	:	dx(0), dy(0) {}
	TDirection(int _dx, int _dy)
	:	dx(_dx), dy(_dy) {}
	TDirection(int x1, int y1, int x2, int y2)
	:	dx(x2-x1), dy(y2-y1) {}
	TDirection(TPosition pos1, TPosition pos2)
	:	dx(pos2.x-pos1.x), dy(pos2.y-pos1.y) {}
};

inline bool operator==(const TPosition& ref1, const TPosition& ref2)
{
	return (ref1.x == ref2.x) && (ref1.y == ref2.y);
}

template <typename type>
type abs(type a)
{
	return (a > 0) ? a : -a;
}

template <typename type>
type sign(type a)
{
	return (a > 0) ? 1 : ((a < 0) ? -1 : 0);
}

template <typename T>
inline T min(T a, T b)
{
	return (a < b) ? a : b;
}

template <typename T>
inline T max(T a, T b)
{
	return (a > b) ? a : b;
}

// vector 내의 객체 소거
template <class type>
class FnctDelete
{
public:
	void operator()(type obj)
	{
		delete obj;
	}
};

// vector 내의 배열 객체 소거
template <class type>
class FnctDeleteArray
{
public:
	void operator()(type obj)
	{
		delete[] obj;
	}
};

inline unsigned long ConvertStringToId(const char* sz_str, int str_len)
{
	++str_len;
	return
	(
		(((sz_str)[str_len*8/9] << 24) | ((sz_str)[str_len*7/9] << 16) | ((sz_str)[str_len*6/9] <<  8) | ((sz_str)[str_len*5/9] <<  0)) ^
		(((sz_str)[str_len*4/9] << 24) | ((sz_str)[str_len*3/9] << 16) | ((sz_str)[str_len*2/9] <<  8) | ((sz_str)[str_len*1/9] <<  0))
	);
}

// 각 class에 대한 I/O stream에 사용되는 action이다.
// 같은 이름의 namespace를 사용해서 다른 곳에서 확장도 가능하다.
namespace action
{
	#define DECLARE_ACTION(action_name)           \
		template <typename T>                     \
		struct action_name                        \
		{                                         \
			T data;                               \
			action_name() {}                      \
			action_name(T _data): data(_data) {}  \
		}

	DECLARE_ACTION(peek);
	DECLARE_ACTION(get);
	DECLARE_ACTION(set);
	DECLARE_ACTION(run);
	DECLARE_ACTION(act);
	DECLARE_ACTION(talk);
	DECLARE_ACTION(line_of_sight);

	#undef DECLARE_ACTION
}

// TFixed에 맞는 sin() / cos() 함수
int Sin_16_16(int degree);
int Cos_16_16(int degree);

#endif
