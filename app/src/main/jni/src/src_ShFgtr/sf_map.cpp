
#include "sf_map.h"
#include "sf_res.h"
#include "sf_obj.h"
#include "sf_obj_enemy.h" // back-call
#include "sf_sys_desc.h"
#include "sf_util.h"

#define MAX_STAGE        5
#define MAP_X_MAX      400
#define MAP_Y_MAX        8

typedef unsigned char TMapArray[MAP_X_MAX][MAP_Y_MAX+1];

static struct CStageMap_TImpl
{
	TMapArray* map_data;
	TMapArray* map_data_buffer[MAX_STAGE+1];
	int        map_absolute_x, map_absolute_y;
	int        map_local_x, map_local_y;
	bool       end_of_map;

	CStageMap_TImpl()
		: map_data(0)
	{
	}

	void Reset(void)
	{
		map_data = 0;
	}
} s_impl;


CStageMap::CStageMap()
{
	SF_ASSERT(s_impl.map_data == 0);
/*
	FILE* fp;

	fp = fopen("map.dat", "rb");
	if (fp)
	{
		for (int i = 1; i <= MAX_STAGE; i++)
		{
			s_impl.map_data_buffer[i] = (TMapArray*)new unsigned char[sizeof(TMapArray)];
			fread(s_impl.map_data_buffer[i], sizeof(TMapArray), 1, fp);
		}

		fclose(fp);
	}
	else
*/
	{
		for (int i = 1; i <= MAX_STAGE; i++)
		{
			s_impl.map_data_buffer[i] = (TMapArray*)new unsigned char[sizeof(TMapArray)];
			memcpy(s_impl.map_data_buffer[i], &p_res_map[sizeof(TMapArray)*(i-1)], sizeof(TMapArray));
		}
	}

	s_impl.map_absolute_x = 0;
	s_impl.map_absolute_y = 3;
	s_impl.map_local_x    = 0;
	s_impl.map_local_y    = 0;
	s_impl.end_of_map     = false;
}

CStageMap::~CStageMap()
{
	for (int i = 1; i <= MAX_STAGE; i++)
		delete[] (unsigned char*)s_impl.map_data_buffer[i];

	s_impl.Reset();
}

void CStageMap::SetCurrentMap(int stage)
{
	s_impl.map_data       = s_impl.map_data_buffer[stage];
	s_impl.map_absolute_x = 0;
	s_impl.map_absolute_y = 1;
	s_impl.map_local_x    = 0;
	s_impl.map_local_y    = 0;
	s_impl.end_of_map     = false;
}

unsigned char CStageMap::GetTile(int a_x, int a_y)
{
	a_x = (a_x + s_impl.map_local_x + 8) / TILE_X_SIZE;
	a_y = (a_y + s_impl.map_local_y    ) / TILE_Y_SIZE;
	return (*s_impl.map_data)[s_impl.map_absolute_x+a_x][s_impl.map_absolute_y+a_y];
}

void CStageMap::Move(int x, int y)
{
	int i, result;

	//?? 상수가 들어 갔음
	if (s_impl.map_absolute_x+15 >= MAP_X_MAX)
	{
		s_impl.end_of_map = true;
		return;
	}

	s_impl.map_local_x = s_impl.map_local_x + x;
	s_impl.map_local_y = s_impl.map_local_y + y;

	while ((s_impl.map_local_x + x) < 0)
	{
		s_impl.map_local_x += TILE_X_SIZE;
		--s_impl.map_absolute_x;
	}

	while ((s_impl.map_local_x + x) >= TILE_X_SIZE)
	{
		s_impl.map_local_x -= TILE_X_SIZE;
		++s_impl.map_absolute_x;

		result = -1;
		if (s_impl.map_absolute_x+12 < MAP_X_MAX)

		switch ((*s_impl.map_data)[s_impl.map_absolute_x+12][0])
		{
			case 1:
				result = CreateEnemy(TILENAME_ZACO_FLY, 5, 550, 50, true);
				break;
			case 2:
				result = CreateEnemy(TILENAME_ZACO_ROACH, 10, 550, 150, false);
				break;
			case 3:
				result = CreateEnemy(TILENAME_ZACO_BIG_SHIT, 5, 550, 100, false);
				break;
			case 4:
				result = CreateEnemy(TILENAME_ZACO_TEETH, 10, 550, 150, false);
				break;
			case 5 :
				{
					SetSysDesc().stage.message.remained_ticks = 100;

					char stage_message[256];
					util::ComposeString(stage_message, "'STAGE @ BOSS : ", GetSysDesc().stage.current_stage);
					// -> sprintf(stage_message, "'STAGE %i BOSS : ", GetSysDesc().stage.current_stage);

					switch (GetSysDesc().stage.current_stage % 5)
					{
						case 1 :
							result = CreateEnemy(TILENAME_BOSS_GAS_MAN,100,550,10,false);
							strcat(stage_message, "방귀 맨");
							break;
						case 2 :
							result = CreateEnemy(TILENAME_BOSS_NOSE_WAX_MAN,200,550,10,false);
							strcat(stage_message, "코딱지 맨");
							break;
						case 3 :
							result = CreateEnemy(TILENAME_BOSS_HAIR_WAX_MAN2,200,550,10,false);
							strcat(stage_message, "비듬 파이터");
							break;
						case 4 :
							result = CreateEnemy(TILENAME_BOSS_OVEREAT_MAN,300,550,10,false);
							strcat(stage_message, "오바이트 맨");
							break;
						case 0 :
							result = CreateEnemy(TILENAME_BOSS_SHIT_FIGHTER,500,588,10,false);
							strcat(stage_message, "대변 파이터");
							break;
						default:
							//??
							break;
					}

					SetSysDesc().stage.message.Set(stage_message);
				}
				break;
			default:
				//??
				break;
		}

		if (result == 0)
		{
			s_impl.map_local_x += TILE_X_SIZE;
			--s_impl.map_absolute_x;

			s_impl.map_local_x = s_impl.map_local_x - x;
			s_impl.map_local_y = s_impl.map_local_y - y;

			return;
		}
		else if ((*s_impl.map_data)[s_impl.map_absolute_x+12][0] == 5)
		{
			if (GetSysDesc().use_sound)
			{
				// endTMF;
				// playTMF('Boss');
			}
		}
	}

	while ((s_impl.map_local_y + y) < 0)
	{
		s_impl.map_local_y += TILE_Y_SIZE;
		--s_impl.map_absolute_y;
	}

	while ((s_impl.map_local_y + y) >= TILE_Y_SIZE)
	{
		s_impl.map_local_y -= TILE_Y_SIZE;
		++s_impl.map_absolute_y;
	}

	for (i = MIN_ENEMY; i <= MAX_ENEMY; i++)
	{
		if (objects[i])
		{
			objects[i]->Move(x,y);
			objects[i]->Move(-x,-y);
		}
	}

	for (i = MIN_FRIEND; i <= MAX_FRIEND; i++)
	{
		if (objects[i])
		{
			objects[i]->Move(x,y);
			objects[i]->Move(-x,-y);
		}
	}
}

bool CStageMap::NotOnPlatform(int x, int y)
{
	return (GetTile(x,y) == 0);
}

int  CStageMap::GetGravity(int x, int y)
{
	int _x = x;
	int _y = y;

	if (GetTile(_x,_y) > 0)
	{
		while (GetTile(_x,_y) > 0)
			--_y;

		++_y;

		return (_y - y);
	}
	else
	{
		while (GetTile(_x,_y) == 0)
			++_y;

		return (_y - y);
	}
}

int  CStageMap::GetXOffset(void)
{
	return s_impl.map_local_x;
}

int  CStageMap::GetYOffset(void)
{
	return s_impl.map_local_y;
}

bool CStageMap::IsEndOfMap(void)
{
	return s_impl.end_of_map;
}
