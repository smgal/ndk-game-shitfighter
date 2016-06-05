
#ifndef __UMAIN_H__
#define __UMAIN_H__

#include "avej_lite.h"
#include "UConfig.h"
#include "util_render_text.h"

////////////////////////////////////////////////////////////////////////////////
// extern definition

extern avej_lite::IGfxDevice*  pGfxDevice;
extern avej_lite::IGfxSurface* pBackBuffer;
extern avej_lite::IGfxSurface* pTileImage;
extern avej_lite::IGfxSurface* pSpriteImage;
extern avej_lite::IGfxSurface* pFontImage;

extern void _MakeFile(const char* szFileName, const char* szContents = NULL);

////////////////////////////////////////////////////////////////////////////////
// forward

unsigned long g_GetColorFromIndexedColor(unsigned long index, unsigned long default_color);

void g_DrawBltList(int x, int y, TBltParam rect[], int nRect = 0x7FFFFFFF);
void g_DrawFormatedText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long default_color, IorzRenderText::TFnBitBlt fn_bit_blt);
void g_DrawText(int x, int y, const char* szText, unsigned long color = 0xFFFFFFFF);

////////////////////////////////////////////////////////////////////////////////
// macro definition

#define CLEAR_MEMORY(var) memset(var, 0, sizeof(var));

#if defined(_WIN32)
	#if defined(_DEBUG)
		extern "C" void __stdcall OutputDebugStringA(const char* lpOutputString);
		#define ASSERT(cond) \
					if (!(cond)) \
					{ \
						char szTemp[1024]; \
						SPRINTF(szTemp, 1024, "%s(%d): Assertion Failed\n", __FILE__, __LINE__ ); \
						::OutputDebugStringA(szTemp); \
						assert(false);\
					}
	#else
		#define ASSERT
	#endif
#else
	#if defined(_DEBUG)
		#define ASSERT(cond) \
					if (!(cond)) \
					{ \
						char szTemp[1024]; \
						SPRINTF(szTemp, 1024, "%s(%d): Assertion Failed\n", __FILE__, __LINE__ ); \
						_MakeFile("./assertion.txt", szTemp); \
						assert(false);\
					}
	#else
		#define ASSERT assert
	#endif
#endif

////////////////////////////////////////////////////////////////////////////////
// type definition

enum
{
	WINDOW_MAP     = 0,
	WINDOW_CONSOLE = 1,
	WINDOW_STATUS  = 2,
	WINDOW_BATTLE  = 3,
};

////////////////////////////////////////////////////////////////////////////////
// functor definition

template <typename type>
type min(type a, type b)
{
	return (a < b) ? a : b;
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

// for_each용 functor를 단독으로 사용 가능하게
template <typename inIter, typename function>
function each(inIter iter, function func)
{
	func(*iter);
	return func;
}

////////////////////////////////////////////////////////////////////////////////
// Exception class

class EException
{
};

class EExitGame: public EException
{
};


#endif // #ifndef __UMAIN_H__
