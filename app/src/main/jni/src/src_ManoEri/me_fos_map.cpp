
#include "me_fos_map.h"

namespace manoeri
{

template <typename type>
type abs(type a)
{
	return (a > 0) ? a : -a;
}

struct TRect
{
	int x1, y1, x2, y2;
};

void CFosMap::operator <<(const CTileMap& map)
{
	delete[] this->_map;

	this->_w     = map.XMapSize();
	this->_h     = map.YMapSize();
	this->_pitch = this->_w;
	this->_map   = new char[this->_w * this->_h];

	for (int y = 0; y < this->_h; y++)
	for (int x = 0; x < this->_w; x++)
	{
		if (map.IsWallMap(x,y))
			this->_map[this->_pitch*y+x] = MASK_BLOCKED;
		else
			this->_map[this->_pitch*y+x] = MASK_NONE;
	}
}

#define QAZ(i,p1,p2,p3,p4) case i: v[0].x=b.p1;v[0].y=b.p2;v[3].x=b.p3;v[3].y=b.p4;break;

static bool MakeShadow(int x, int y, const TRect& b, CFosMap::TPoint v[], int& st)
{
	st  = (x > b.x2) ? 0 : (x < b.x1) ? 2 : 1;
	st += (y > b.y2) ? 0 : (y < b.y1) ? 6 : 3;

	switch (st)
	{
		QAZ(0, x1,y2,x2,y1)
		QAZ(1, x1,y2,x2,y2)
		QAZ(2, x1,y1,x2,y2)
		QAZ(3, x2,y1,x2,y2)
		QAZ(5, x1,y1,x1,y2)
		QAZ(6, x1,y1,x2,y2)
		QAZ(7, x1,y1,x2,y1)
		QAZ(8, x1,y2,x2,y1)
		default: return false;
	}

	v[1].x = x + (v[0].x - x) * 15*4;
	v[1].y = y + (v[0].y - y) * 15*4;
	v[2].x = x + (v[3].x - x) * 15*4;
	v[2].y = y + (v[3].y - y) * 15*4; 

	return true;
}

bool CFosMap::MakeFos(int x, int y, int x_on_tile, int y_on_tile, TPoint vertices[])
{
	TRect box = {x_on_tile*m_w_block, y_on_tile*m_h_block, x_on_tile*m_w_block+m_w_block, y_on_tile*m_h_block+m_h_block};
	int status;

	if (!MakeShadow(x, y, box, vertices, status))
		return false;

	IsMyEyesReached(x, y, vertices[1].x, vertices[1].y);
	IsMyEyesReached(x, y, vertices[2].x, vertices[2].y);

	return true;
}

bool CFosMap::IsMyEyesReached(int x1, int y1, int x2, int y2)
{
	if ((abs(x1-x2) <= m_w_block) && (abs(y1-y2) <= m_h_block))
	{
		SetVisibility(x1 / m_w_block, y1 / m_h_block);
		return true;
	}

	int dx = x2 - x1;
	int dy = y2 - y1;

	int x_dir = (dx < 0) ? -1 : 1;
	int y_dir = (dy < 0) ? -1 : 1;

	{
		if (x_dir < 0)
			dx = -dx;

		if (y_dir < 0)
			dy = -dy;

		bool qaz = (dx >= dy);

		if (!qaz)
		{
			dx ^= dy; dy ^= dx; dx ^= dy;
		}

		int dir       = 2 * dy - dx;
		int inc_to_R  = 2 * dy;
		int inc_to_UR = 2 * (dy - dx);
		int x         = x1;
		int y         = y1;

		int x_prev    = x / m_w_block;
		int y_prev    = y / m_h_block;

		int& major_axis = (qaz) ? x : y;
		int& minor_axis = (qaz) ? y : x;
		int& major_inc  = (qaz) ? x_dir : y_dir;
		int& minor_inc  = (qaz) ? y_dir : x_dir;

		while ((x != x2) || (y != y2))
		{
			major_axis += major_inc;

			if (dir <= 0)
				dir += inc_to_R;
			else
			{
				dir += inc_to_UR;
				minor_axis += minor_inc;
			}

			if ((x_prev != x / m_w_block) || (y_prev != y / m_h_block))
			{
				int x_new = x / m_w_block;
				int y_new = y / m_h_block;

				if ((x_new < 0) || (x_new >= _w) || (y_new < 0) || (y_new >= _h))
					return true;

				SetVisibility(x_new, y_new);

				if (IsBlocked(x_new, y_new))
					return false;

				x_prev = x_new;
				y_prev = y_new;
			}
		}
	}

	return true;
}

} // namespace manoeri
