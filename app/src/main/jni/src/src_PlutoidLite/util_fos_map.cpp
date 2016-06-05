
#include "util_fos_map.h"

#include <math.h>

template <typename type>
type abs(type a)
{
	return (a > 0) ? a : -a;
}


struct TRect
{
	int x1, y1, x2, y2;
};

#define QAZ(i,p1,p2,p3,p4) case i: v[0].x=b.p1;v[0].y=b.p2;v[3].x=b.p3;v[3].y=b.p4;break;

static bool MakeShadow(int x, int y, const TRect& b, CFosTileMap::TPoint v[], int& st)
{
	st  = (x > b.x2) ? 0 : (x < b.x1) ? 2 : 1;
	st += (y > b.y2) ? 0 : (y < b.y1) ? 6 : 3;

	switch (st)
	{
		QAZ(0,x1,y2,x2,y1)
		QAZ(1,x1,y2,x2,y2)
		QAZ(2,x1,y1,x2,y2)
		QAZ(3,x2,y1,x2,y2)
		QAZ(5,x1,y1,x1,y2)
		QAZ(6,x1,y1,x2,y2)
		QAZ(7,x1,y1,x2,y1)
		QAZ(8,x1,y2,x2,y1)
		default: return false;
	}

	const int MAG_FACTOR = 15;

	v[1].x = x + (v[0].x - x) * MAG_FACTOR;
	v[1].y = y + (v[0].y - y) * MAG_FACTOR;
	v[2].x = x + (v[3].x - x) * MAG_FACTOR;
	v[2].y = y + (v[3].y - y) * MAG_FACTOR;

	return true;
}

bool CFosTileMap::MakeFos(int x, int y, int xTile, int yTile, TPoint vertices[])
{
	int status;

	{
#if 0 // Win32의 Polygon()을 사용할 때
		TRect box = {xTile*m_tile_width, yTile*m_tile_height, xTile*m_tile_width+m_tile_width-1, yTile*m_tile_height+m_tile_height-1};
#else
		TRect box = {xTile*m_tile_width, yTile*m_tile_height, xTile*m_tile_width+m_tile_width, yTile*m_tile_height+m_tile_height};
#endif
		if (MakeShadow(x, y, box, vertices, status))
		{
			IsMyEyesReached(x, y, vertices[1].x, vertices[1].y);
			IsMyEyesReached(x, y, vertices[2].x, vertices[2].y);
			return true;
		}
	}

	return false;
}

bool CFosTileMap::IsMyEyesReached(int x1, int y1, int x2, int y2)
{
	int dx, dy, xDir, yDir;
	int d, x, y, incR, incUR;
	int preX, preY;

	if ((abs(x1-x2) <= m_tile_width) && (abs(y1-y2) <= m_tile_height))
	{
		SetVisibility(x1 / m_tile_width, y1 / m_tile_height);
		return true;
	}

	dx = x2 - x1;
	dy = y2 - y1;

	yDir = (dy < 0) ? -1 : 1;
	xDir = (dx < 0) ? -1 : 1;

	{
		if (xDir < 0)
			dx = -dx;
		if (yDir < 0)
			dy = -dy;

		bool qaz = (dx >= dy);

		if (!qaz)
		{
			dx ^= dy; dy ^= dx; dx ^= dy;
		}

		d     = 2 * dy - dx;
		incR  = 2 * dy;
		incUR = 2 * (dy - dx);
		x     = x1;
		y     = y1;

		preX = x / m_tile_width;
		preY = y / m_tile_height;

		int& majorAxis = (qaz) ? x : y;
		int& minorAxis = (qaz) ? y : x;
		int& majorInc  = (qaz) ? xDir : yDir;
		int& minorInc  = (qaz) ? yDir : xDir;

		while ((x != x2) || (y != y2))
		{
			majorAxis += majorInc;

			if (d <= 0)
				d += incR;
			else
			{
				d += incUR;
				minorAxis += minorInc;
			}

			if ((preX != x / m_tile_width) || (preY != y / m_tile_height))
			{
				int newX = x / m_tile_width;
				int newY = y / m_tile_height;

				// check if it goes to out of boundary
				if ((newX < 0) || (newX >= m_map_desc.width) || (newY < 0) || (newY >= m_map_desc.height))
					return true;

				// it is a visible box
				SetVisibility(newX, newY);

				if (IsOccluded(newX, newY))
					return false;

				preX = newX;
				preY = newY;
			}
		}
	}

	return true;
}
