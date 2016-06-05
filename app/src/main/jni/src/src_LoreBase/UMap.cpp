
#include "UMap.h"

#include "avej_lite.h"
#include <assert.h>

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool TMap::_Load(const CReadStream& stream)
{
	if (stream.Read((void*)&jumpable, sizeof(CSmSet)) != sizeof(CSmSet))
		return false;

	if (stream.Read((void*)&teleportable, sizeof(CSmSet)) != sizeof(CSmSet))
		return false;

	if (stream.Read((void*)save, sizeof(save)) != sizeof(save))
		return false;

	// 저장되지 않는 내부 값의 변경
	SetType(type);

	return true;
}

bool TMap::_Save(const CWriteStream& stream) const
{

	if (stream.Write((void*)&jumpable, sizeof(CSmSet)) != sizeof(CSmSet))
		return false;

	if (stream.Write((void*)&teleportable, sizeof(CSmSet)) != sizeof(CSmSet))
		return false;

	if (stream.Write((void*)save, sizeof(save)) != sizeof(save))
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// public method

#define GET_TILE_DATA (data[y][x] & 0x3F)

void TMap::ClearData(void)
{
	memset(save, 0, sizeof(save));
}

void TMap::ChangeMap(int x, int y, unsigned char tile)
{
	assert(tile < 64);
	if ((x >= 0) && (x < width) && (y >= 0) && (y < height))
	{
		data[y][x] &= 0xC0;
		data[y][x] |= tile;
	}
}

void TMap::ChangeToWay(int x, int y)
{
	assert(tile4way < 64);
	if ((x >= 0) && (x < width) && (y >= 0) && (y < height))
	{
		data[y][x] &= 0xC0;
		data[y][x] |= (unsigned char)tile4way;
	}
}

void TMap::SetType(TType _type)
{
	#define ASSIGN_FUNC(value, func) if (i <= value) actList[i] = actFunc[func];

	// 각 map type에 대해 tile 번호에 따라 handler 지정
	type = _type;
	switch (_type)
	{
	case TYPE_TOWN:
		{
			for (int i = 0; i < MAX_MAP_TILE; i++)
			{
					 ASSIGN_FUNC( 0, ACT_EVENT)
				else ASSIGN_FUNC(21, ACT_BLOCK)
				else ASSIGN_FUNC(22, ACT_ENTER)
				else ASSIGN_FUNC(23, ACT_SIGN)
				else ASSIGN_FUNC(24, ACT_WATER)
				else ASSIGN_FUNC(25, ACT_SWAMP)
				else ASSIGN_FUNC(26, ACT_LAVA)
				else ASSIGN_FUNC(47, ACT_MOVE)
				else ASSIGN_FUNC(MAX_MAP_TILE-1, ACT_TALK)
			}
			jumpable.Assign("0,27..47");
			teleportable.Assign("27..47");
			tile4way = 47;
		}
		break;
	case TYPE_KEEP:
		{
			for (int i = 0; i < MAX_MAP_TILE; i++)
			{
					 ASSIGN_FUNC( 0, ACT_EVENT)
				else ASSIGN_FUNC(39, ACT_BLOCK)
				else ASSIGN_FUNC(47, ACT_MOVE)
				else ASSIGN_FUNC(48, ACT_WATER)
				else ASSIGN_FUNC(49, ACT_SWAMP)
				else ASSIGN_FUNC(50, ACT_LAVA)
				else ASSIGN_FUNC(51, ACT_BLOCK)
				else ASSIGN_FUNC(52, ACT_EVENT)
				else ASSIGN_FUNC(53, ACT_SIGN)
				else ASSIGN_FUNC(54, ACT_ENTER)
				else ASSIGN_FUNC(MAX_MAP_TILE-1, ACT_TALK)
			}
			jumpable.Assign("0,40..47");
			teleportable.Assign("27..47");
			tile4way = 43;
		}
		break;
	case TYPE_GROUND:
		{
			for (int i = 0; i < MAX_MAP_TILE; i++)
			{
					 ASSIGN_FUNC( 0, ACT_EVENT)
				else ASSIGN_FUNC(21, ACT_BLOCK)
				else ASSIGN_FUNC(22, ACT_SIGN)
				else ASSIGN_FUNC(23, ACT_SWAMP)
				else ASSIGN_FUNC(47, ACT_MOVE)
				else ASSIGN_FUNC(48, ACT_WATER)
				else ASSIGN_FUNC(49, ACT_SWAMP)
				else ASSIGN_FUNC(50, ACT_LAVA)
				else ASSIGN_FUNC(MAX_MAP_TILE-1, ACT_ENTER)
			}
			jumpable.Assign("0,24..47");
			teleportable.Assign("24..47");
			tile4way = 41;
		}
		break;
	case TYPE_DEN:
		{
			for (int i = 0; i < MAX_MAP_TILE; i++)
			{
					 ASSIGN_FUNC( 0, ACT_EVENT)
				else ASSIGN_FUNC(20, ACT_BLOCK)
				else ASSIGN_FUNC(21, ACT_TALK)
				else ASSIGN_FUNC(40, ACT_BLOCK)
				else ASSIGN_FUNC(47, ACT_MOVE)
				else ASSIGN_FUNC(48, ACT_WATER)
				else ASSIGN_FUNC(49, ACT_SWAMP)
				else ASSIGN_FUNC(50, ACT_LAVA)
				else ASSIGN_FUNC(51, ACT_BLOCK)
				else ASSIGN_FUNC(52, ACT_EVENT)
				else ASSIGN_FUNC(53, ACT_SIGN)
				else ASSIGN_FUNC(54, ACT_ENTER)
				else ASSIGN_FUNC(MAX_MAP_TILE-1, ACT_TALK)
			}
			jumpable.Assign("0,41..47");
			teleportable.Assign("41..47");
			tile4way = 43;
		}
		break;
	default:
		assert(0);
	}

	#undef ASSIGN_FUNC
}

int TMap::GetEncounteredEnemy(void)
{
	if (encounterRange == 0)
		return 0;

	return avej_lite::util::Random(encounterRange) + encounterOffset;
}

bool TMap::IsJumpable(int x, int y)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return false;

	return jumpable.IsSet(GET_TILE_DATA);
}

bool TMap::IsEventPos(int x, int y)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return false;

	return (actList[GET_TILE_DATA] == actFunc[ACT_EVENT]);
}

bool TMap::IsTeleportable(int x, int y)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return false;

	// jump 가능하면서 event 지역이면 안된다.
	return (jumpable.IsSet(GET_TILE_DATA) && (actList[GET_TILE_DATA] != actFunc[ACT_EVENT]));
}

void TMap::SetLight(int x, int y)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return;

	data[y][x] |= 0x80;
}

void TMap::ResetLight(int x, int y)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return;

	data[y][x] &= 0x7F;
}

bool TMap::HasLight(int x, int y) const
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return false;

	return (data[y][x] & 0x80) > 0;
}
