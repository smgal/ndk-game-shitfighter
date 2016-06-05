
#ifndef __UTIL_FOS_MAP_H__
#define __UTIL_FOS_MAP_H__

#include "util_tile_map_base.h"

class CFosTileMap: public CTileMapBase<TILE_DATA_TYPE_COMPRESS16>
{
public:
	struct TPoint
	{
		int x, y;
	};

	CFosTileMap(const CTileMapBase<TILE_DATA_TYPE_COMPRESS16>& ref, int map_x_offset, int map_y_offset, int tile_width, int tile_height)
	:	m_map_x_offset(map_x_offset), m_map_y_offset(map_y_offset),
		m_tile_width(tile_width), m_tile_height(tile_height)
	{
		// binary copy
		memcpy(&this->m_map_desc, &ref.GetMapDesc(), sizeof(this->m_map_desc));
		this->m_map_desc.ownership = false;
	}

	void ClearVisibility(void)
	{
		for (int y = 0; y < m_map_desc.height; y++)
		for (int x = 0; x < m_map_desc.width; x++)
		{
			ResetVisibility(x, y);
		}
	}
	bool MakeFos(int x, int y, int xTile, int yTile, TPoint vertices[]);
	bool IsMyEyesReached(int x1, int y1, int x2, int y2);

private:
	int   m_map_x_offset;
	int   m_map_y_offset;
	int   m_tile_width;
	int   m_tile_height;
};

#endif
