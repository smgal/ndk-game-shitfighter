
#include "me_tile_map.h"

namespace manoeri
{

CTileMap* g_tile_map = 0;

CTileMap::CTileMap(const char* map_data[])
	: m_x_map_size(0), m_y_map_size(0), m_x_map_pitch(0), m_p_map_data(0)
{
	Init(map_data);
}

CTileMap::~CTileMap(void)
{
	delete[] m_p_map_data;
}

static int strlen(const char* str1)
{
	const char* str2 = str1;
	while (*str2++)
		;
	return str2-str1-1;
}

void CTileMap::Init(const char* map_data[])
{
	int       i, j;
	int       tile;
	TMapData* p_map;

	m_x_map_size  = strlen(map_data[0]);
	m_y_map_size  = 0;

	while (map_data[m_y_map_size])
		++m_y_map_size;

	m_x_map_pitch = m_x_map_size;

	m_p_map_data  = new TMapData[m_x_map_pitch * m_y_map_size];

	for (j = 0; j < m_y_map_size; j++)
	{
		p_map  = m_p_map_data;
		p_map += j * m_x_map_pitch;

		for (i = 0; i < m_x_map_size; i++)
		{
			if      (map_data[j][i] == '>')
				p_map->tile = 16+1;
			else if (map_data[j][i] == '<')
				p_map->tile = 16+2;
			else if (map_data[j][i] != ' ')
				p_map->tile = 1;
			else
				p_map->tile = 0;

			++p_map;
		}
	}

	for (j = 0; j < m_y_map_size; j++)
	{
		p_map  = m_p_map_data;
		p_map += j * m_x_map_pitch;

		for (i = 0; i < m_x_map_size; i++)
		{
			if ((p_map->tile > 0) && (p_map->tile <= 16))
			{
				tile = 0;

				if (IsWallMap(i, j-1))
					tile += 1;
				if (IsWallMap(i+1, j))
					tile += 2;
				if (IsWallMap(i, j+1))
					tile += 4;
				if (IsWallMap(i-1, j))
					tile += 8;

				p_map->tile = tile + 1;
			}
			++p_map;
		}
	}
}

bool CTileMap::IsWallMap(int x, int y) const
{
	if ((x < 0) || (x >= m_x_map_size) || (y < 0) || (y >= m_y_map_size))
		return false;

	TMapData* p_map = m_p_map_data;

	p_map += y * m_x_map_pitch + x;

	return (p_map->tile > 0) && (p_map->tile <= 16);
}

bool CTileMap::IsMoveableMap(int x, int y) const
{
	if ((x < 0) || (x >= m_x_map_size) || (y < 0) || (y >= m_y_map_size))
		return false;

	TMapData* p_map = m_p_map_data;

	p_map += y * m_x_map_pitch + x;

	return (p_map->tile == 0) || (p_map->tile > 16);
}

int  CTileMap::GetTile(int x, int y) const
{
	if ((x < 0) || (x >= m_x_map_size) || (y < 0) || (y >= m_y_map_size))
		return false;

	TMapData* p_map = m_p_map_data;

	p_map += y * m_x_map_pitch + x;

	return p_map->tile;
}

int  CTileMap::XMapSize(void) const
{
	return m_x_map_size;
}

int  CTileMap::YMapSize(void) const
{
	return m_y_map_size;
}

} // namespace manoeri
