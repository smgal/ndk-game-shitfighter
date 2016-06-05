
#ifndef __UTIL_TILE_MAP_BASE_H__
#define __UTIL_TILE_MAP_BASE_H__

/*
데이터는 unsigned long으로 규정하고, 그것이 어떻게 나눠져 있는가는
Tile 관리자에게 위임한다.
*/

#include <new>
#include <string.h>


/* 용어 설명

tile
	맵을 이루는 하나의 기본 단위.
tile data
	tile에 대한 데이터 구조
tile map
	tile로 이루어진 2차원적인 맵
*/

// tile data의 종류
enum TTileDataType
{
	TILE_DATA_TYPE_COMPRESS16,
	TILE_DATA_TYPE_COMPRESS08
};

// tile data의 특성 구조체
template <TTileDataType tile_data_type>
struct TTileDataTraits
{
};

template <>
struct TTileDataTraits<TILE_DATA_TYPE_COMPRESS16>
{
	typedef unsigned short TTileUnit;

	struct tile
	{
		enum { mask  = 0x00FF };
		enum { shift = 0 };
	};
	struct attrib
	{
		enum { mask  = 0xFF00 };
		enum { shift = 8 };
	};
	struct blocked
	{
		enum { mask  = 0x1000 };
		enum { shift = 12 };
	};
	struct occluded
	{
		enum { mask  = 0x2000 };
		enum { shift = 13 };
	};
	struct visible
	{
		enum { mask  = 0x4000 };
		enum { shift = 14 };
	};
	struct visibility
	{
		enum { mask  = 0x0F00 };
		enum { shift = 8 };
	};
};

template <>
struct TTileDataTraits<TILE_DATA_TYPE_COMPRESS08>
{
	typedef unsigned char TTileUnit;

	struct tile
	{
		enum { mask  = 0x3F };
		enum { shift = 0 };
	};
	struct attrib
	{
		enum { mask  = 0xC0 };
		enum { shift = 6 };
	};
	struct blocked
	{
		enum { mask  = 0x40 };
		enum { shift = 6 };
	};
	struct visible
	{
		enum { mask  = 0x80 };
		enum { shift = 7 };
	};
	struct visibility
	{
		enum { mask  = 0x00 };
		enum { shift = 0 };
	};
};

// tile data의 각 속성 별 input/output을 정의
template <TTileDataType tile_data_type>
struct TTileDataIO
{
	#define SET_(name) \
		m_data = (m_data & ~TTileDataTraits<tile_data_type>::name::mask) \
			   | ((name << TTileDataTraits<tile_data_type>::name::shift) & TTileDataTraits<tile_data_type>::name::mask)

	#define GET_(name) \
		((m_data & TTileDataTraits<tile_data_type>::name::mask) >> TTileDataTraits<tile_data_type>::name::shift)

	#define RETURN_(name) \
		return GET_(name)

	typedef typename TTileDataTraits<tile_data_type>::TTileUnit TTileUnit;

public:
	TTileDataIO(TTileUnit& data)
		: m_data(data) {}

	void SetTile(TTileUnit tile)
	{
		SET_(tile);
	}
	TTileUnit GetTile() const
	{
		RETURN_(tile);
	}
	void ResetAttrib()
	{
		m_data = (m_data & ~TTileDataTraits<tile_data_type>::attrib::mask);
	}
	void SetAttribBlocked(TTileUnit blocked)
	{
		SET_(blocked);
	}
	TTileUnit GetAttribBlocked() const
	{
		RETURN_(blocked);
	}
	void SetAttribOcculuded(TTileUnit occluded)
	{
		SET_(occluded);
	}
	TTileUnit GetAttribOccluded() const
	{
		RETURN_(occluded);
	}
	void SetAttribVisible(TTileUnit visible)
	{
		SET_(visible);
		if (visible)
		{
			int visibility = (TTileDataTraits<tile_data_type>::visibility::mask >> TTileDataTraits<tile_data_type>::visibility::shift);
			SET_(visibility);
		}
		else
		{
			// visible이 아니므로 점점 투명하게 만든다.
			ProcessAttribVisibilityForce();
		}
	}
	TTileUnit GetAttribVisible() const
	{
		RETURN_(visible);
	}
	unsigned long GetAttribVisibility() const
	{
		int visibility = (m_data & TTileDataTraits<tile_data_type>::visibility::mask);
		visibility >>= TTileDataTraits<tile_data_type>::visibility::shift;
		// 0~15 -> 0~255 로 변경
		visibility |= (visibility << 4);
		return visibility;
	}
	void ProcessAttribVisibility()
	{
		// visible이 아닐 때 점점 투명하게 만든다.
		if (GET_(visible) == 0)
			ProcessAttribVisibilityForce();
	}
	void ProcessAttribVisibilityForce()
	{
		if (m_data & TTileDataTraits<tile_data_type>::visibility::mask)
		{
			// 강제로 visibility를 1씩 떨어 뜨린다.
			int visibility = (m_data & TTileDataTraits<tile_data_type>::visibility::mask);
			visibility >>= TTileDataTraits<tile_data_type>::visibility::shift;
			--visibility;
			SET_(visibility);
		}
	}

private:
	TTileUnit& m_data;

	#undef SET_
	#undef RETURN_
};

// tile map을 표현하기 위한 자료 기술(記述)
template <TTileDataType tile_data_type>
struct TTileMapDesc
{
	typedef typename TTileDataTraits<tile_data_type>::TTileUnit TTileUnit;

	TTileUnit* p_map_data;
	int        width;
	int        height;
	int        pitch;
	bool       ownership;

	TTileMapDesc()
		: p_map_data(0), width(0), height(0), pitch(0), ownership(false)
	{
	}
	TTileMapDesc(TTileUnit* _p_map_data, int _width, int _height, int _pitch)
		: p_map_data(_p_map_data), width(_width), height(_height), pitch(_pitch), ownership(false)
	{
	}
	TTileMapDesc(int _width, int _height)
		: p_map_data(new TTileUnit[_width * _height]), width(_width), height(_height), pitch(_width), ownership(true)
	{
		memset(p_map_data, 0, _width * _height * sizeof(TTileUnit));
	}
	~TTileMapDesc()
	{
		if (ownership)
			delete[] p_map_data;
	}
	void Reset()
	{
		if (ownership)
			delete[] p_map_data;

		p_map_data = 0;
		width      = 0;
		height     = 0;
		pitch      = 0;
		ownership  = false;
	}
};

// tile map을 표현하기 위한 기반 class
template <TTileDataType tile_data_type>
class CTileMapBase
{
public:
	typedef typename TTileDataTraits<tile_data_type>::TTileUnit TTileUnit;

	inline bool IsAvaliable(void) const
	{
		return (m_map_desc.p_map_data != 0);
	}

	inline int  GetWidth() const
	{
		return m_map_desc.width;
	}
	inline int  GetHeight() const
	{
		return m_map_desc.height;
	}

	inline bool IsBlocked(int x, int y) const
	{
		return m_InRange(x, y) ? (TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).GetAttribBlocked() > 0) : false;
	}
	inline bool IsOccluded(int x, int y) const
	{
		return m_InRange(x, y) ? (TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).GetAttribOccluded() > 0) : false;
	}
	inline bool IsVisible(int x, int y) const
	{
		return m_InRange(x, y) ? (TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).GetAttribVisible() > 0) : false;
	}
	inline bool IsVisibleAndOccluded(int x, int y) const
	{
		return (IsOccluded(x, y) && IsVisible(x, y));
	}
	inline void SetTile(int x, int y, int tile) const
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetTile(tile);
	}
	inline int  GetTile(int x, int y) const
	{
		return m_InRange(x, y) ? TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).GetTile() : 0;
	}
	inline void SetVisibility(int x, int y)
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribVisible(1);
	}
	inline void ResetVisibility(int x, int y)
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribVisible(0);
	}
	inline unsigned long GetVisibility(int x, int y)
	{
		return m_InRange(x, y) ? TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).GetAttribVisibility() : 0;
	}
	inline void ProcessVisibility(int x, int y)
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).ProcessAttribVisibility();
	}
	inline void SetBlocked(int x, int y)
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribBlocked(1);
	}
	inline void SetOccluded(int x, int y)
	{
		if (m_InRange(x, y))
			TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribOcculuded(1);
	}

	inline const TTileMapDesc<tile_data_type>& GetMapDesc(void) const
	{
		return m_map_desc;
	}

	inline void operator <<(const char** map)
	{
		m_map_desc.Reset();

		{
			int width  = m_strlen(map[0]);
			int height = 0;

			while (map[height])
				++height;

			new (&m_map_desc) TTileMapDesc<tile_data_type>(width, height);
		}

		for (int y = 0; y < m_map_desc.height; y++)
		for (int x = 0; x < m_map_desc.width; x++)
		{
			switch (map[y][x])
			{
				case ' ':
					TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).ResetAttrib();
					break;
				case '#':
					TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribBlocked(1);
					TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetTile(1);
					break;
				default :
					TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetAttribBlocked(1);
					TTileDataIO<tile_data_type>(m_map_desc.p_map_data[m_map_desc.pitch*y+x]).SetTile(0);
					break;
			}
		}
	}

protected:
	TTileMapDesc<tile_data_type> m_map_desc;

private:

	inline int m_strlen(const char* str1) const
	{
		const char* str2 = str1;
		while (*str2++)
			;
		return str2-str1-1;
	}

	inline bool m_InRange(int x, int y) const
	{
		return ((x >= 0) && (x < m_map_desc.width) && (y >= 0) && (y < m_map_desc.height));
	}
};

#endif
