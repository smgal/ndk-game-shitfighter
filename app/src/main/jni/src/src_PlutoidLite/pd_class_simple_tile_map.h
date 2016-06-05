
#ifndef __PD_CLASS_SIMPLE_TILE_MAP_H__
#define __PD_CLASS_SIMPLE_TILE_MAP_H__

#include "util_tile_map_base.h"

class CSimpleTileMap: public CTileMapBase<TILE_DATA_TYPE_COMPRESS16>
{
public:
	CSimpleTileMap();
};

#endif
