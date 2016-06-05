
#ifndef __AVEJ_GFX3D_TYPE_H__
#define __AVEJ_GFX3D_TYPE_H__

#if defined(shared)
#undef shared
#endif

#include "gfx3d_config.h"
#include "avej_lite.h"

#if defined(_WIN32)
	typedef __int64   TInt64;
#else
	typedef long long TInt64;
#endif

namespace avej { namespace gfx3d
{
	typedef GLfixed TFixed;

	////////////////////////////////////////////////////////////////////////////
	// structure

	struct TFixedVec2
	{
		TFixed x, y;

		TFixedVec2() {}
		TFixedVec2(TFixed _x, TFixed _y)
			: x(_x), y(_y) {}
	};

	struct TFixedVec3
	{
		TFixed x, y, z;

		TFixedVec3() {}
		TFixedVec3(TFixed _x, TFixed _y, TFixed _z)
			: x(_x), y(_y), z(_z) {}
	};

	struct TFixedVec4
	{
		union
		{
			TFixed fixed[4];
			struct
			{
				TFixed r, g, b, a;
			};
		};

		TFixedVec4() {}
		TFixedVec4(TFixed _r, TFixed _g, TFixed _b, TFixed _a)
			: r(_r), g(_g), b(_b), a(_a) {}
	};

	////////////////////////////////////////////////////////////////////////////
	// fixed point

	inline TFixed _FXD(int i)
	{
		return TFixed((i) << 16);
	}

	inline TFixed _FXD(float f)
	{
		#define SHIFT      16
		#define CONST_1_0  (1 << SHIFT)

		return TFixed(f * (float)CONST_1_0);
	}

	inline TFixed _FXD(double d)
	{
		#define SHIFT      16
		#define CONST_1_0  (1 << SHIFT)

		return TFixed(d * (double)CONST_1_0);
	}

	inline TFixed _FXD_from8bit(unsigned char data)
	{
		TFixed temp = TFixed(data);
		temp  += (temp >> 7);
		temp <<= 8;

		return temp;
	}

	inline TFixedVec4 _FXD_color2array(unsigned long color32bit)
	{
		TFixed a = _FXD_from8bit((color32bit >> 24) & 0xFF);
		TFixed r = _FXD_from8bit((color32bit >> 16) & 0xFF);
		TFixed g = _FXD_from8bit((color32bit >>  8) & 0xFF);
		TFixed b = _FXD_from8bit((color32bit >>  0) & 0xFF);

		return TFixedVec4(r, g, b, a);
	}

	inline TFixed _FXD_mul(TFixed a, TFixed b)
	{
		return (TInt64(a) * b) >> 16;
	}

	inline int _FXD_toInt(TFixed fixed)
	{
		return TFixed(fixed >> 16);
	}

	////////////////////////////////////////////////////////////////////////////
	// enum

	enum TPrimitivesMode
	{
		PRIMITIVES_MODE_TRIANGLE_LIST,
		PRIMITIVES_MODE_TRIANGLE_STRIP
	};

	template <TPrimitivesMode mode>
	struct TTexCoord
	{
	};

	template <>
	struct TTexCoord<PRIMITIVES_MODE_TRIANGLE_STRIP>
	{
		TFixedVec2 tex_coord[4];

		TTexCoord(TFixed x1, TFixed y1, TFixed x2, TFixed y2)
		{
			tex_coord[0].x = x1;
			tex_coord[0].y = y1;
			tex_coord[1].x = x2;
			tex_coord[1].y = y1;
			tex_coord[2].x = x1;
			tex_coord[2].y = y2;
			tex_coord[3].x = x2;
			tex_coord[3].y = y2;
		}
	};

	template <>
	struct TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>
	{
		TFixedVec2 tex_coord[6];

		TTexCoord(TFixed x1, TFixed y1, TFixed x2, TFixed y2)
		{
			// 첫 번째 삼각형
			tex_coord[0].x = x1;
			tex_coord[0].y = y1;
			tex_coord[1].x = x2;
			tex_coord[1].y = y1;
			tex_coord[2].x = x1;
			tex_coord[2].y = y2;
			// 두 번째 삼각형
			tex_coord[3]   = tex_coord[2];
			tex_coord[4]   = tex_coord[1];
			tex_coord[5].x = x2;
			tex_coord[5].y = y2;
		}
	};

	struct TTexTileList
	{
		//?? 변수 이름 변경
		iu::shared_ptr<avej_lite::TSurfaceDesc> handle;
		iu::vector<TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST> > tile_info;

		TTexTileList()
			: handle(0) {}
		TTexTileList(iu::shared_ptr<avej_lite::TSurfaceDesc> _surface_desc)
			: handle(_surface_desc) {}
	};

	namespace shared
	{
		typedef iu::shared_ptr<struct ::avej::gfx3d::TTexTileList> TTexTileList;
	}

}} // namespace avej { namespace gfx3d

#endif
