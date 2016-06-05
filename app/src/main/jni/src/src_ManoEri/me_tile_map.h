
#ifndef __ME_TILE_MAP_H__
#define __ME_TILE_MAP_H__

namespace manoeri
{

extern class CTileMap* g_tile_map;

struct TMapData
{
	int tile;
};

class CTileMap
{
public:
	CTileMap(const char* mapData[]);
	~CTileMap(void);

	void Init(const char* mapData[]);
	int  XMapSize(void) const;
	int  YMapSize(void) const;

	bool IsWallMap(int x, int y) const;
	bool IsMoveableMap(int x, int y) const;

	int  GetTile(int x, int y) const;

private:
	int m_x_map_size;
	int m_y_map_size;
	int m_x_map_pitch;

	TMapData* m_p_map_data;
};

class CMapIterator
{
public:
	struct TCoord
	{
		int x, y;

		TCoord() {}
		TCoord(int _x, int _y): x(_x), y(_y) {}
	};

	struct TArea
	{
		int w, h;

		TArea() {}
		TArea(int _w, int _h): w(_w), h(_h) {}
	};

	struct TDesc
	{
		TCoord map_index;
		TCoord real_pos;
	};

	CMapIterator(TCoord offset, TArea tile_size, TArea clip_rect)
		: m_is_valid(true), m_offset(offset), m_tile_size(tile_size), m_clip_rect(clip_rect)
	{
		m_map_index      = TCoord(- offset.x / tile_size.w, - (offset.y + (tile_size.h-1)) / tile_size.h);
		m_out.map_index  = m_map_index;
		m_out.real_pos   = TCoord(m_out.map_index.x * tile_size.w + offset.x, m_out.map_index.y * tile_size.h + offset.y);
	}

	~CMapIterator()
	{
	}

	inline bool EndOfLoop(void) const
	{
		return !m_is_valid;
	}
	inline const TDesc& GetDesc(void) const
	{
		return m_out;
	}

	CMapIterator& operator++(void)
	{
		m_out.map_index.x++;
		m_out.real_pos.x += m_tile_size.w;
		if (m_out.real_pos.x > m_clip_rect.w)
		{
			m_out.map_index.x = m_map_index.x;
			m_out.real_pos.x  = m_out.map_index.x * m_tile_size.w + m_offset.x;

			m_out.map_index.y++;
			m_out.real_pos.y += m_tile_size.h;
			if (m_out.real_pos.y > m_clip_rect.h)
			{
				m_is_valid = false;
			}
		}

		return *this;
	}

private:
	bool   m_is_valid;
	TCoord m_map_index;
	TCoord m_offset;
	TArea  m_tile_size;
	TArea  m_clip_rect;

	TDesc  m_out;
};

} // namespace manoeri

#endif // #ifndef __ME_TILE_MAP_H__
