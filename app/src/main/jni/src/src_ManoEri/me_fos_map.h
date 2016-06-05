
#ifndef __ME_FOS_MAP_H__
#define __ME_FOS_MAP_H__

#include "me_visible_map.h"
#include "me_tile_map.h"

namespace manoeri
{

class CFosMap: public CVisibleMap
{
public:
	struct TPoint
	{
		int x, y;
	};

	CFosMap(int w_block, int h_block)
		: m_w_block(w_block), m_h_block(h_block)
	{
	}
	~CFosMap(void)
	{
	}

	void operator <<(const CTileMap& map);
	bool MakeFos(int x, int y, int x_on_tile, int y_on_tile, TPoint vertices[]);
	bool IsMyEyesReached(int x1, int y1, int x2, int y2);

protected:
	int  m_w_block;
	int  m_h_block;
};

} // namespace manoeri

#endif // #ifndef __ME_FOS_MAP_H__
