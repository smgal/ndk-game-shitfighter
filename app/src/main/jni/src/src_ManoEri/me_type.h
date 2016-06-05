
#ifndef __ME_TYPE_H__
#define __ME_TYPE_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "avej_lite.h"


////////////////////////////////////////////////////////////////////////////////
// type definition

// ������ ��ġ�� ǥ���ϱ� ���� �ڷ���
struct TPoint
{
	int x, y;
};

// �Ǽ��� ��ġ�� ǥ���ϱ� ���� �ڷ���
struct TFPoint
{
	float x, y;
};

// ������ ������ ǥ���ϱ� ���� �ڷ���
struct TRect
{
	int x1, y1, x2, y2;
};


////////////////////////////////////////////////////////////////////////////////
// macro definition

template <typename T>
T abs(T a) 
{
	return (a > 0) ? a : -a;
}

template <typename T>
T sign(T a) 
{
	return (a != 0) ? ((a > 0) ? 1 : -1) : 0;
}


////////////////////////////////////////////////////////////////////////////////
// Exception class

class EExitGame
{
};


////////////////////////////////////////////////////////////////////////////////
// 

#define BLOCK_SIZE   24

namespace manoeri
{
	// Avej graphics instance
	extern avej_lite::IGfxDevice*  g_p_gfx_device;
	// Graphics back buffer instance
	extern avej_lite::IGfxSurface* g_p_back_buffer;
	// Graphics image instance
	extern avej_lite::IGfxSurface* g_p_tile_image;
}

#endif // #ifndef __ME_TYPE_H__
