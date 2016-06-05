
#include "pd_class_simple_tile_map.h"

#include <assert.h>

const char* C_MAP_DATA_STREAM[] =
{
	"#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1",
	"#1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 0 0 0 0 0 0 0 0 0#0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 0 1 1 1 1 1 1 1 1#0 0 0 0 0 0 0 0 0#3 0#3 0#1",
	"#1 0 1 1 2 1 1 3 1 1#0 0 0 0 0 0 0 0 0#3 0#3 0#1",
	"#1 0 1 1 1 1 2 1 1 1#0 0 0 0 0 0 0 0 0#3 0#3 0#1",
	"#1 0+0+0+0 0 1 1 1 0#0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 0#2#2#2#2#2 1#0#0#0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 4 4 4 4 4#2 3#0 0 0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 4 4 4 4 4#2 1#0#0#0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 4 4 4 4 4#2 1 1 1 1 1 1 0 1 1 0 1#0 0 1 1 0#1",
	"#1 4 4 4 4 4#2 1 1 2 1 1 1 0 1 1 0#0#0 0 1 1 0#1",
	"#1 4 4 4 4 4 0 1 1 2 1 1 1 0 1 1 0 1 1 0 1 1 0#1",
	"#1 4 4 4 4 4 0 1 1 2 1 1 1 0 1 1 0 1 1 0 1 1 0#1",
	"#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1 0 0 0#1#1#1#1#1#1",
	"#1 0 0 0 0#0 0 0#0 0 0 0 0#0 0 0 0 0 0 0 0 0 0#1",
	"#1 0#0 0 0#0 0 0 0 0#0 0 0#0 0 0 0 0 0 0 0 0 0#1",
	"#1 0 0 0#0#0 0#0#0#0#0 0 0#0 0 0 0#0#0 0#0 0 0#1",
	"#1 0#0#0#0 0 0 0 0 0#0 0 0#0 0 0 0#0#0 0 0 0 0#1",
	"#1 0 0#0 0 0 0 0 0 0#0 0 0 0 0 0 0 0 0 0 0 0 0#1",
	"#1 0 0#0 0 0 0 0 0 0#0 0 0 0 0#2#2 0 0 0 0 0 0#1",
	"#1 0 0#0#0#0#0#0#0 0 0 0 0 0 0#2#2 0 0#2#2+0#2#1",
	"#1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0#2 0 0 0#1",
	"#1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0#2 0 0 0#1",
	"#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1",
};

CSimpleTileMap::CSimpleTileMap()
{
	m_map_desc.width      = 24;
	m_map_desc.height     = 25;
	m_map_desc.pitch      = m_map_desc.width;
	m_map_desc.p_map_data = new CTileMapBase<TILE_DATA_TYPE_COMPRESS16>::TTileUnit[m_map_desc.pitch * m_map_desc.height];
	memset(m_map_desc.p_map_data, 0, m_map_desc.pitch * m_map_desc.height * sizeof(CTileMapBase<TILE_DATA_TYPE_COMPRESS16>::TTileUnit));

	// ~TTileMapDesc()에서 자동으로 해제됨
	m_map_desc.ownership  = true;

	for (int y = 0; y < m_map_desc.height; y++)
	for (int x = 0; x < m_map_desc.width; x++)
	{
		char group = C_MAP_DATA_STREAM[y][2*x];
		char index = C_MAP_DATA_STREAM[y][2*x+1];

		int  number = 0;

		switch (group)
		{
		case ' ':
			number += 0;
			break;
		case '#':
			number += 10;
			break;
		case '+':
			number += 20;
			break;
		default:
			assert(false);
		}

		if (index >= '0' && index <= '9')
			number += (index - '0');
		else if (index >= 'a' && index <= 'z')
			number += (index - 'a' + 10);

		this->SetTile(x, y, number);

		if (group == '#')
			this->SetOccluded(x, y);

		if (group == '#' || group == '+')
			this->SetBlocked(x, y);
	}
}
