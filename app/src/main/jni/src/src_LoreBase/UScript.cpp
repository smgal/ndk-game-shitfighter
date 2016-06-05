
#include "UScript.h"
#include "UExtern.h"

#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <stack>

static std::stack<avej_lite::util::string> s_stringStack;

///////////////////////////////////////////////////////////
// General

namespace
{
	int Random(int range)
	{
		static unsigned long rand_seed = 0L;

		if (range <= 0)
			return 0;

		rand_seed = rand_seed * 214013L + 2531011L;
		return ((rand_seed >> 16) & 0x7fff) % range;
	}
}

///////////////////////////////////////////////////////////
// General

void CScript::NativeEqual(TSmParam* pParam)
{
	assert(pParam);
	assert((pParam->type[1] == 'i' && pParam->type[2] == 'i') ||
	       (pParam->type[1] == 's' && pParam->type[2] == 's'));

	pParam->result.type = 'i';

	if (pParam->type[1] == 'i')
	{
		// 정수끼리 비교
		pParam->result.data = (pParam->data[1] == pParam->data[2]) ? 1 : 0;
	}
	else
	{
		// 문자열끼리 비교
		pParam->result.data = (strcmp(pParam->string[1], pParam->string[2]) == 0) ? 1 : 0;
	}
}


void CScript::NativeLess(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = (pParam->data[1] < pParam->data[2]) ? 1 : 0;
}

void CScript::NativeNot(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = (pParam->data[1] != 0) ? 0 : 1;
}

void CScript::NativeOr(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = ((pParam->data[1] != 0) || (pParam->data[2] != 0)) ? 1 : 0;
}

void CScript::NativeAnd(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = ((pParam->data[1] != 0) && (pParam->data[2] != 0)) ? 1 : 0;
}

void CScript::NativeRandom(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = Random(pParam->data[1]);
}

void CScript::NativeAdd(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = pParam->data[1] + pParam->data[2];
}


void CScript::NativePushString(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

	s_stringStack.push(pParam->string[1]);
}

void CScript::NativePopString(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	avej_lite::util::string result;

	for (int i = 0; i < pParam->data[1]; i++)
	{
		assert(!s_stringStack.empty());
		if (!s_stringStack.empty())
		{
			result.copyToFront(s_stringStack.top());
			s_stringStack.pop();
		}
	}
	pParam->result.type = 's';
	pParam->result.szStr = result;
}


void CScript::NativeDisplayMap(TSmParam* pParam)
{
	game::window::DisplayMap();
	game::UpdateScreen();
}

void CScript::NativeDisplayStatus(TSmParam* pParam)
{
	game::window::DisplayStatus();
	game::UpdateScreen();
}

void CScript::NativeScriptMode(TSmParam* pParam)
{
	assert(pParam);

	pParam->result.type = 'i';
	pParam->result.data = m_mode;
}

void CScript::NativeOn(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = 0;

	if (m_xPos == pParam->data[1] && m_yPos == pParam->data[2])
	{
		printf("NativeOn(%d, %d)\n", pParam->data[1], pParam->data[2]);
		pParam->result.data = 1;
	}
}

void CScript::NativeOnArea(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');
	assert(pParam->type[3] == 'i');
	assert(pParam->type[4] == 'i');

	int x1 = pParam->data[1];
	int y1 = pParam->data[2];
	int x2 = pParam->data[3];
	int y2 = pParam->data[4];

	pParam->result.type = 'i';
	pParam->result.data = 0;

	if ((m_xPos >= x1 && m_xPos <= x2) && (m_yPos >= y1 && m_yPos <= y2))
	{
		printf("NativeOnArea(%d, %d, %d, %d)\n", x1, y1, x2, y2);
		pParam->result.data = 1;
	}
}

void CScript::NativeTalk(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

//??	printf("NativeTalk(%s)\n", pParam->string[1]);
	game::console::WriteLine(pParam->string[1]);
}

void CScript::NativeTextAlign(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	switch (pParam->data[1])
	{
	case 0:
		game::console::SetTextAlign(game::console::ALIGN_LEFT);
		break;
	case 1:
		game::console::SetTextAlign(game::console::ALIGN_CENTER);
		break;
	case 2:
		game::console::SetTextAlign(game::console::ALIGN_RIGHT);
		break;
	default:
		assert(0);
	}
}

void CScript::NativeWarpPrevPos(TSmParam* pParam)
{
	assert(pParam);

	game::WarpPrevPos();
}

void CScript::NativePressAnyKey(TSmParam* pParam)
{
	game::PressAnyKey();
}

void CScript::NativeWait(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::Wait(pParam->data[1]);
}

void CScript::NativeLoadScript(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

	// pParam->data[2], pParam->data[3]은 deafault parameter
	game::LoadScript(pParam->string[1], pParam->data[2], pParam->data[3]);
}

void CScript::NativeGameOver(TSmParam* pParam)
{
	// pParam->data[2], pParam->data[3]은 deafault parameter
	game::GameOver();
}

///////////////////////////////////////////////////////////
// Map

struct TMapTemplate
{
	int width;
	int height;
	int row;

	typedef std::map<unsigned short, unsigned char> TConvert;
	TConvert convert;
} m_map;

void CScript::NativeMapInit(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	m_map.width  = pParam->data[1];
	m_map.height = pParam->data[2];
	m_map.row    = 0;
	m_map.convert.clear();

	game::map::Init(m_map.width, m_map.height);
}

void CScript::NativeMapSetType(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	switch (pParam->data[1])
	{
	case 0:
		game::map::SetType(TMap::TYPE_TOWN);
		break;
	case 1:
		game::map::SetType(TMap::TYPE_KEEP);
		break;
	case 2:
		game::map::SetType(TMap::TYPE_GROUND);
		break;
	case 3:
		game::map::SetType(TMap::TYPE_DEN);
		break;
	}
}

void CScript::NativeMapSetHandicap(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::map::SetHandicap(pParam->data[1]);
}

void CScript::NativeMapSetEncounter(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::map::SetEncounter(pParam->data[1], pParam->data[2]);
}

void CScript::NativeMapSetStartPos(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::map::SetStartPos(pParam->data[1], pParam->data[2]);
}

void CScript::NativeMapSetTile(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');
	assert(pParam->type[2] == 'i');

	unsigned short temp = pParam->string[1][0];
	temp <<= 8;
	temp  |= (unsigned short)pParam->string[1][1] & 0xFF;

	m_map.convert[temp] = pParam->data[2];
}

void CScript::NativeMapSetRow(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

	int len = strlen(pParam->string[1]) / 2;
	unsigned char* pChar = (unsigned char*)pParam->string[1];

	std::vector<unsigned char> row;
	row.reserve(len);

	int loop = len;
	while (--loop >= 0)
	{
		unsigned short temp = *pChar++;
		temp <<= 8;
		temp  |= (unsigned short)*pChar++ & 0xFF;

		TMapTemplate::TConvert::iterator i = m_map.convert.find(temp);
		if (i != m_map.convert.end())
		{
			row.push_back(i->second);
			continue;
		}
		assert(false);
		row.push_back(0);
	}

	game::map::Push(m_map.row++, &(*row.begin()), len);
}

void CScript::NativeMapChangeTile(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');
	assert(pParam->type[3] == 'i');

	game::map::Change(pParam->data[1], pParam->data[2], pParam->data[3]);
}


void CScript::NativeMapSetLightArea(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');
	assert(pParam->type[3] == 'i');
	assert(pParam->type[4] == 'i');

	const int& x1 = pParam->data[1];
	const int& y1 = pParam->data[2];
	const int& x2 = pParam->data[3];
	const int& y2 = pParam->data[4];

	for (int y = y1; y <= y2; y++)
	for (int x = x1; x <= x2; x++)
		game::map::SetLight(x, y);
}

void CScript::NativeMapResetLightArea(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');
	assert(pParam->type[3] == 'i');
	assert(pParam->type[4] == 'i');

	const int& x1 = pParam->data[1];
	const int& y1 = pParam->data[2];
	const int& x2 = pParam->data[3];
	const int& y2 = pParam->data[4];

	for (int y = y1; y <= y2; y++)
	for (int x = x1; x <= x2; x++)
		game::map::ResetLight(x, y);
}

void CScript::NativeMapLoadFromFile(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

	game::map::LoadFromFile(pParam->string[1]);
}

///////////////////////////////////////////////////////////
// Tile

void CScript::NativeTileCopyToDefaultTile(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::tile::CopyToDefaultTile(pParam->data[1]);
}

void CScript::NativeTileCopyToDefaultSprite(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::tile::CopyToDefaultSprite(pParam->data[1]);
}

void CScript::NativeTileCopyTile(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::tile::CopyTile(pParam->data[1], pParam->data[2]);
}

///////////////////////////////////////////////////////////
// Flag

void CScript::NativeFlagSet(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::flag::Set(pParam->data[1]);
}

void CScript::NativeFlagReset(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::flag::Reset(pParam->data[1]);
}

void CScript::NativeFlagIsSet(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = game::flag::IsSet(pParam->data[1]) ? 1 :0;
}

void CScript::NativeVariableSet(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::variable::Set(pParam->data[1], pParam->data[2]);
}

void CScript::NativeVariableAdd(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::variable::Add(pParam->data[1]);
}

void CScript::NativeVariableGet(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 'i';
	pParam->result.data = game::variable::Get(pParam->data[1]);
}

///////////////////////////////////////////////////////////
// Battle

void CScript::NativeBattleInit(TSmParam* pParam)
{
	assert(pParam);

	game::battle::Init();
}

void CScript::NativeBattleRegisterEnemy(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::battle::RegisterEnemy(pParam->data[1]);
}

void CScript::NativeBattleShowEnemy(TSmParam* pParam)
{
	assert(pParam);

	game::battle::ShowEnemy();
}

void CScript::NativeBattleStart(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::battle::Start((pParam->data[1] > 0) ? true : false);
}

void CScript::NativeBattleResult(TSmParam* pParam)
{
	assert(pParam);

	pParam->result.type = 'i';
	pParam->result.data = game::battle::Result();
}

void CScript::NativeSelectInit(TSmParam* pParam)
{
	game::select::Init();
}

void CScript::NativeSelectAdd(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 's');

	game::select::Add(pParam->string[1]);
}

void CScript::NativeSelectRun(TSmParam* pParam)
{
	game::select::Run();
}

void CScript::NativeSelectResult(TSmParam* pParam)
{
	assert(pParam);

	pParam->result.type = 'i';
	pParam->result.data = game::select::Result();
}


void CScript::NativePartyPosX(TSmParam* pParam)
{
	assert(pParam);

	pParam->result.type = 'i';
	pParam->result.data = game::party::PosX();
}

void CScript::NativePartyPosY(TSmParam* pParam)
{
	assert(pParam);

	pParam->result.type = 'i';
	pParam->result.data = game::party::PosY();
}

void CScript::NativePartyPlusGold(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::party::PlusGold(pParam->data[1]);
}

void CScript::NativePartyMove(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::party::Move(pParam->data[1], pParam->data[2]);
}

void CScript::NativeCheckIf(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	game::party::ECheckIf ixCheckIf;

	switch (pParam->data[1])
	{
	case 0:
		ixCheckIf = game::party::CHECKIF_MAGICTORCH;
		break;
	case 1:
		ixCheckIf = game::party::CHECKIF_LEVITATION;
		break;
	case 2:
		ixCheckIf = game::party::CHECKIF_WALKONWATER;
		break;
	case 3:
		ixCheckIf = game::party::CHECKIF_WALKONSWAMP;
		break;
	case 4:
		ixCheckIf = game::party::CHECKIF_MINDCONTROL;
		break;
	default:
		pParam->result.type = 'i';
		pParam->result.data = 0;
		return;
	}

	pParam->result.type = 'i';
	pParam->result.data = (game::party::CheckIf(ixCheckIf)) ? 1 : 0;
}


void CScript::NativePlayerIsAvailable(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 'i';
	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	pParam->result.data = (game::player::IsAvailable(pParam->data[1]-1)) ? 1 : 0;
}

void CScript::NativePlayerGetName(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 's';
	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	pParam->result.szStr = game::player::GetName(pParam->data[1]-1);
}

void CScript::NativePlayerGetGenderName(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	pParam->result.type = 's';
	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	pParam->result.szStr = game::player::GetGenderName(pParam->data[1]-1);
}

void CScript::NativePlayerAssignFromEnemyData(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 'i');

	game::player::AssignFromEnemyData(pParam->data[1]-1, pParam->data[2]);
}

void CScript::NativePlayerChangeAttribute(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 's');

	if (pParam->type[3] == 'i')
		game::player::ChangeAttribute(pParam->data[1]-1, pParam->string[2], pParam->data[3]);
	else
		game::player::ChangeAttribute(pParam->data[1]-1, pParam->string[2], pParam->string[3]);
}

void CScript::NativePlayerGetAttribute(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 's');

	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	int ixPerson = pParam->data[1]-1;

	pParam->result.type = 'i';
	if (!game::player::GetAttribute(ixPerson, pParam->string[2], pParam->result.data))
	{
		// integer 형에는 존재 하지 않음
		pParam->result.type  = 's';
		game::player::GetAttribute(ixPerson, pParam->string[2], pParam->result.szStr);
	}
}

void CScript::NativePlayerReviseAttribute(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	int ixPerson = pParam->data[1]-1;

	game::player::ReviseAttribute(ixPerson);
}

void CScript::NativePlayerApplyAttribute(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');

	// script는 1-base이고 C++은 0-base이기 때문에 -1을 한다.
	int ixPerson = pParam->data[1]-1;

	game::player::ApplyAttribute(ixPerson);
}

void CScript::NativeEnemyChangeAttribute(TSmParam* pParam)
{
	assert(pParam);
	assert(pParam->type[1] == 'i');
	assert(pParam->type[2] == 's');

	if (pParam->type[3] == 'i')
		game::enemy::ChangeAttribute(pParam->data[1]-1, pParam->string[2], pParam->data[3]);
	else
		game::enemy::ChangeAttribute(pParam->data[1]-1, pParam->string[2], pParam->string[3]);
}

CScript::CScript(int mode, int position, int x, int y)
	: m_mode(mode), m_position(position), m_xPos(x), m_yPos(y)

{
	// 아래의 변수가 static이기 때문에 CScript에 대한 script 벌 수는 한 개로 한정됨
	// thread safety 보장되지 않음
	static CSmScriptFunction<CScript> function;

	function.SetScript(this);

	// script 속도 향상을 위해 초기화는 단 한번만 하도록 함
	if (function.IsNotInitialized())
	{
		function.RegisterFunction("Equal", &CScript::NativeEqual);
		function.RegisterFunction("Less", &CScript::NativeLess);
		function.RegisterFunction("Not", &CScript::NativeNot);
		function.RegisterFunction("Or", &CScript::NativeOr);
		function.RegisterFunction("And", &CScript::NativeAnd);
		function.RegisterFunction("Random", &CScript::NativeRandom);
		function.RegisterFunction("Add", &CScript::NativeAdd);
		
		function.RegisterFunction("PushString", &CScript::NativePushString);
		function.RegisterFunction("PopString", &CScript::NativePopString);
		
		function.RegisterFunction("DisplayMap", &CScript::NativeDisplayMap);
		function.RegisterFunction("DisplayStatus", &CScript::NativeDisplayStatus);
		function.RegisterFunction("ScriptMode", &CScript::NativeScriptMode);
		function.RegisterFunction("On", &CScript::NativeOn);
		function.RegisterFunction("OnArea", &CScript::NativeOnArea);
		function.RegisterFunction("Talk", &CScript::NativeTalk);
		function.RegisterFunction("TextAlign", &CScript::NativeTextAlign);
		
		function.RegisterFunction("WarpPrevPos", &CScript::NativeWarpPrevPos);
		function.RegisterFunction("PressAnyKey", &CScript::NativePressAnyKey);
		function.RegisterFunction("Wait", &CScript::NativeWait);
		function.RegisterFunction("LoadScript", &CScript::NativeLoadScript);
		function.RegisterFunction("GameOver", &CScript::NativeGameOver);

		function.RegisterFunction("Map::Init", &CScript::NativeMapInit);
		function.RegisterFunction("Map::SetType", &CScript::NativeMapSetType);
		function.RegisterFunction("Map::SetHandicap", &CScript::NativeMapSetHandicap);
		function.RegisterFunction("Map::SetEncounter", &CScript::NativeMapSetEncounter);
		function.RegisterFunction("Map::SetStartPos", &CScript::NativeMapSetStartPos);
		function.RegisterFunction("Map::SetTile", &CScript::NativeMapSetTile);
		function.RegisterFunction("Map::SetRow", &CScript::NativeMapSetRow);
		function.RegisterFunction("Map::ChangeTile", &CScript::NativeMapChangeTile);
		function.RegisterFunction("Map::SetLightArea", &CScript::NativeMapSetLightArea);
		function.RegisterFunction("Map::ResetLightArea", &CScript::NativeMapResetLightArea);
		function.RegisterFunction("Map::LoadFromFile", &CScript::NativeMapLoadFromFile);
		
		function.RegisterFunction("Tile::CopyToDefaultTile", &CScript::NativeTileCopyToDefaultTile);
		function.RegisterFunction("Tile::CopyToDefaultSprite", &CScript::NativeTileCopyToDefaultSprite);
		function.RegisterFunction("Tile::CopyTile", &CScript::NativeTileCopyTile);

		function.RegisterFunction("Flag::Set", &CScript::NativeFlagSet);
		function.RegisterFunction("Flag::Reset", &CScript::NativeFlagReset);
		function.RegisterFunction("Flag::IsSet", &CScript::NativeFlagIsSet);
		function.RegisterFunction("Variable::Set", &CScript::NativeVariableSet);
		function.RegisterFunction("Variable::Add", &CScript::NativeVariableAdd);
		function.RegisterFunction("Variable::Get", &CScript::NativeVariableGet);

		function.RegisterFunction("Battle::Init", &CScript::NativeBattleInit);
		function.RegisterFunction("Battle::Start", &CScript::NativeBattleStart);
		function.RegisterFunction("Battle::RegisterEnemy", &CScript::NativeBattleRegisterEnemy);
		function.RegisterFunction("Battle::ShowEnemy", &CScript::NativeBattleShowEnemy);
		function.RegisterFunction("Battle::Result", &CScript::NativeBattleResult);

		function.RegisterFunction("Select::Init", &CScript::NativeSelectInit);
		function.RegisterFunction("Select::Add", &CScript::NativeSelectAdd);
		function.RegisterFunction("Select::Run", &CScript::NativeSelectRun);
		function.RegisterFunction("Select::Result", &CScript::NativeSelectResult);

		function.RegisterFunction("Party::PosX", &CScript::NativePartyPosX);
		function.RegisterFunction("Party::PosY", &CScript::NativePartyPosY);
		function.RegisterFunction("Party::PlusGold", &CScript::NativePartyPlusGold);
		function.RegisterFunction("Party::Move", &CScript::NativePartyMove);
		function.RegisterFunction("Party::CheckIf", &CScript::NativeCheckIf);

		function.RegisterFunction("Player::IsAvailable", &CScript::NativePlayerIsAvailable);
		function.RegisterFunction("Player::GetName", &CScript::NativePlayerGetName);
		function.RegisterFunction("Player::GetGenderName", &CScript::NativePlayerGetGenderName);
		function.RegisterFunction("Player::AssignFromEnemyData", &CScript::NativePlayerAssignFromEnemyData);
		function.RegisterFunction("Player::ChangeAttribute", &CScript::NativePlayerChangeAttribute);
		function.RegisterFunction("Player::GetAttribute", &CScript::NativePlayerGetAttribute);
		function.RegisterFunction("Player::ReviseAttribute", &CScript::NativePlayerReviseAttribute);
		function.RegisterFunction("Player::ApplyAttribute", &CScript::NativePlayerApplyAttribute);

		function.RegisterFunction("Enemy::ChangeAttribute", &CScript::NativeEnemyChangeAttribute);
	}

	// 원래는 position 값을 보고 정해야 함
	CSmScript<CScript> script(function, GetScriptFileName());
}

static avej_lite::util::string s_fileName;

void CScript::RegisterScriptFileName(const char* szFileName)
{
	s_fileName = szFileName;
}

const char* CScript::GetScriptFileName(void)
{
	return s_fileName;
}
