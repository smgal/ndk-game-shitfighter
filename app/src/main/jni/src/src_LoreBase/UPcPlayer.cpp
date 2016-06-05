
#include "UPcPlayer.h"

#if defined(_WIN32)
#pragma warning(disable: 4786)
#endif

#include "avej_lite.h"
#include "UConfig.h"
#include "USmSet.h"
#include "USmSola.h"
#include "UPcParty.h"
#include "UPcEnemy.h"
#include "UResString.h"
#include "UConsole.h"
#include "USelect.h"
#include "USound.h"
#include "UExtern.h"
#include <assert.h>
#include <vector>
#include <map>
#include <algorithm>

using namespace avej_lite;

#define DETECT_NOT_ENOUGH_SP(consumption) \
	if (sp < consumption) \
	{ \
		game::console::WriteConsole(7, 1, resource::GetMessageString(resource::MESSAGE_NOT_ENOUGH_SP)); \
		return; \
	}

// 파티에 등록된 player의 수
template <class type>
class FnctNumOfRegistered
{
	int nPlayer;

public:
	FnctNumOfRegistered(void)
		: nPlayer(0)
	{
	}
	void operator()(type obj)
	{
		if (obj->Valid())
			++nPlayer;
	}
	int Result(void)
	{
		return nPlayer;
	}
};

////////////////////////////////////////////////////////////////////////////////
// static function

static void TPcPlayer_m_SetName(TPcPlayer& player, const char* szName)
{
	player.SetName(szName);
}

static void TPcPlayer_m_GetName(TPcPlayer& player, avej_lite::util::string& refName)
{
	const char* szName = player.GetName();
	refName = szName;
}

static resource::ECondition s_GetCondition(const TPcPlayer& player)
{
	if (player.dead > 0)
		return resource::CONDITION_DEAD;
	if (player.unconscious > 0)
		return resource::CONDITION_UNCONSCIOUS;
	if (player.poison > 0)
		return resource::CONDITION_POISONED;

	return resource::CONDITION_GOOD;
}

inline static int s_GetMaxHP(const TPcPlayer& player)
{
	return player.endurance * player.level[0];
}

inline static int s_GetMaxSP(const TPcPlayer& player)
{
	return player.mentality * player.level[1];
}

inline static int s_GetMaxESP(const TPcPlayer& player)
{
	return player.concentration * player.level[2];
}


////////////////////////////////////////////////////////////////////////////////
// class static variable

TPcPlayer::TAttribMapInt TPcPlayer::s_m_attribMapIntList;
TPcPlayer::TAttribMapStr TPcPlayer::s_m_attribMapStrList;

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool TPcPlayer::_Load(const CReadStream& stream)
{
	if (!CPcNameBase::_Load(stream))
		return false;

	return (stream.Read((void*)save, sizeof(save)) == sizeof(save));
}

bool TPcPlayer::_Save(const CWriteStream& stream) const
{
	if (!CPcNameBase::_Save(stream))
		return false;

	return (stream.Write((void*)save, sizeof(save)) == sizeof(save));
}

////////////////////////////////////////////////////////////////////////////////
// public method

TPcPlayer::TPcPlayer(void)
{
	// save data 0으로 초기화
	for (int i = 0; i < sizeof(save) / sizeof(save[0]); i++)
		save[i] = 0;

	// static data의 초기화
	if (s_m_attribMapIntList.empty())
	{
		s_m_attribMapIntList["class"]           = int(&this->class_) - int(this);
		s_m_attribMapIntList["strength"]        = int(&this->strength) - int(this);
		s_m_attribMapIntList["mentality"]       = int(&this->mentality) - int(this);
		s_m_attribMapIntList["concentration"]   = int(&this->concentration) - int(this);
		s_m_attribMapIntList["endurance"]       = int(&this->endurance) - int(this);
		s_m_attribMapIntList["resistance"]      = int(&this->resistance) - int(this);
		s_m_attribMapIntList["agility"]         = int(&this->agility) - int(this);
		s_m_attribMapIntList["luck"]            = int(&this->luck) - int(this);
		s_m_attribMapIntList["weapon"]          = int(&this->weapon) - int(this);
		s_m_attribMapIntList["shield"]          = int(&this->shield) - int(this);
		s_m_attribMapIntList["armor" ]          = int(&this->armor ) - int(this);
		s_m_attribMapIntList["pow_of_weapon"]   = int(&this->pow_of_weapon) - int(this);
		s_m_attribMapIntList["pow_of_shield"]   = int(&this->pow_of_shield) - int(this);
		s_m_attribMapIntList["pow_of_armor"]    = int(&this->pow_of_armor ) - int(this);
		s_m_attribMapIntList["ac"]              = int(&this->ac) - int(this);

		s_m_attribMapIntList["level"]           = int(&this->level[0]) - int(this);
		s_m_attribMapIntList["level(magic)"]    = int(&this->level[1]) - int(this);
		s_m_attribMapIntList["level(esp)"]      = int(&this->level[2]) - int(this);
		s_m_attribMapIntList["accuracy"]        = int(&this->accuracy[0]) - int(this);
		s_m_attribMapIntList["accuracy(magic)"] = int(&this->accuracy[1]) - int(this);
		s_m_attribMapIntList["accuracy(esp)"]   = int(&this->accuracy[2]) - int(this);
	}

	if (s_m_attribMapStrList.empty())
	{
		s_m_attribMapStrList["name"] = std::make_pair((void*)TPcPlayer_m_SetName, (void*)TPcPlayer_m_GetName);
	}
}

TPcPlayer::~TPcPlayer(void)
{
}

void TPcPlayer::SetDefault(int kind)
{
	CT_ASSERT(sizeof(*this) == 916, TPlayer_size_is_changed);

	order         = 0;
	gender        = MALE;
	class_        = 8;

	strength      = 10;
	mentality     = 10;
	concentration = 10;
	endurance     = 10;
	resistance    = 10;
	agility       = 10;
	luck          = 10;

	ac            = 0;

	experience    = 0;
	accuracy[0]   = 10;
	accuracy[1]   = 10;
	accuracy[2]   = 10;
	level[0]      = 1;
	level[1]      = 1;
	level[2]      = 1;

	poison        = 0;
	unconscious   = 0;
	dead          = 0;

	weapon        = 0;
	shield        = 0;
	armor         = 0;

	pow_of_weapon = 5;
	pow_of_shield = 0;
	pow_of_armor  = 0;

	hp  = s_GetMaxHP(*this);
	sp  = s_GetMaxSP(*this);
	esp = s_GetMaxESP(*this);

	SetName(resource::GetMessageString(resource::MESSAGE_ANONYMOUS_NAME));
}

void TPcPlayer::CheckCondition(void)
{
	if ((hp <= 0) && (unconscious == 0))
		unconscious = 1;
	if ((unconscious > endurance * level[0]) && (dead == 0))
		dead = 1;
}

const char* TPcPlayer::GetConditionString(void) const
{
	resource::ECondition ixCondition = s_GetCondition(*this);

	return resource::GetConditionName(ixCondition).szName;
}

unsigned long TPcPlayer::GetConditionColor(void) const
{
	resource::ECondition ixCondition = s_GetCondition(*this);

	switch (ixCondition)
	{
	case resource::CONDITION_GOOD:
		return 0xFFFFFFFF;
	case resource::CONDITION_POISONED:
		return 0xFFFF00FF;
	case resource::CONDITION_UNCONSCIOUS:
		return 0xFF808080;
	case resource::CONDITION_DEAD:
		return 0xFF404040;
	default:
		assert(false);
	}

	return 0xFFFFFFFF;
}

const char* TPcPlayer::Get3rdPersonName(void) const
{
	int ixGender;

	switch (gender)
	{
	case MALE:
		ixGender = 0;
		break;
	case FEMALE:
		ixGender = 1;
		break;
	default:
		ixGender = -1;
		break;
	}
	return resource::Get3rdPersonName(ixGender).szName;
}
const char* TPcPlayer::GetGenderName(void) const
{
	int ixGender;

	switch (gender)
	{
	case MALE:
		ixGender = 0;
		break;
	case FEMALE:
		ixGender = 1;
		break;
	default:
		ixGender = -1;
		break;
	}
	return resource::GetGenderName(ixGender).szName;
}
const char* TPcPlayer::GetClassName(void) const
{
	return resource::GetClassName(class_).szName;
}
const char* TPcPlayer::GetWeaponName(void) const
{
	return resource::GetWeaponName(weapon).szName;
}
const char* TPcPlayer::GetShieldName(void) const
{
	return resource::GetShieldName(shield).szName;
}
const char* TPcPlayer::GetArmorName(void) const
{
	return resource::GetArmorName(armor).szName;
}

bool TPcPlayer::ChangeAttribute(const char* attribute, int value)
{
	TAttribMapInt::iterator attrib = s_m_attribMapIntList.find(attribute);

	if (attrib == s_m_attribMapIntList.end())
		return false;

	int* pInt = reinterpret_cast<int*>(int(this) + int(attrib->second));

	*pInt = value;

	return true;
}

bool TPcPlayer::ChangeAttribute(const char* attribute, const char* data)
{
	TAttribMapStr::iterator attrib = s_m_attribMapStrList.find(attribute);

	if (attrib == s_m_attribMapStrList.end())
		return false;

	TFnSet((attrib->second).first)(*this, data);

	return true;
}

bool TPcPlayer::GetAttribute(const char* attribute, int& value)
{
	TAttribMapInt::iterator attrib = s_m_attribMapIntList.find(attribute);

	if (attrib == s_m_attribMapIntList.end())
		return false;

	int* pInt = reinterpret_cast<int*>(int(this) + int(attrib->second));

	value = *pInt;

	return true;
}

bool TPcPlayer::GetAttribute(const char* attribute, avej_lite::util::string& data)
{
	TAttribMapStr::iterator attrib = s_m_attribMapStrList.find(attribute);

	if (attrib == s_m_attribMapStrList.end())
		return false;

	TFnGet((attrib->second).second)(*this, data);

	return true;
}

void TPcPlayer::ReviseAttribute(void)
{
	hp  = sola::min(hp,  s_GetMaxHP(*this));
	sp  = sola::min(sp,  s_GetMaxSP(*this));
	esp = sola::min(esp, s_GetMaxESP(*this));
}

void TPcPlayer::ApplyAttribute(void)
{
	hp  = s_GetMaxHP(*this);
	sp  = s_GetMaxSP(*this);
	esp = s_GetMaxESP(*this);
}

void TPcPlayer::operator<<(const TEnemyData& data)
{
	CT_ASSERT(sizeof(*this) == 916, TPlayer_size_is_changed);

	// 이름 설정
	SetName(data.name);

	// 기타 속성 설정
	gender        = MALE;
	class_        = 0;
	
	strength      = data.strength;
	mentality     = data.mentality;
	concentration = 0;;
	endurance     = data.endurance;
	resistance    = data.resistance / 2;
	agility       = data.agility;
	luck          = 10;

	ac            = data.ac;

	level[0]      = data.level;
	level[1]      = data.castlevel * 3;
	if (level[1] == 0)
		level[1]  = 1;
	level[2]      = 1;

	hp  = s_GetMaxHP(*this);
	sp  = s_GetMaxSP(*this);
	esp = s_GetMaxESP(*this);

	switch (level[0])
	{
		case 0 :
		case 1 : experience = 0; break;
		case 2 : experience = 1500; break;
		case 3 : experience = 6000; break;
		case 4 : experience = 20000; break;
		case 5 : experience = 50000; break;
		case 6 : experience = 150000; break;
		case 7 : experience = 250000; break;
		case 8 : experience = 500000; break;
		case 9 : experience = 800000; break;
		case 10 : experience = 1050000; break;
		case 11 : experience = 1320000; break;
		case 12 : experience = 1620000; break;
		case 13 : experience = 1950000; break;
		case 14 : experience = 2310000; break;
		case 15 : experience = 2700000; break;
		case 16 : experience = 3120000; break;
		case 17 : experience = 3570000; break;
		case 18 : experience = 4050000; break;
		case 19 : experience = 4560000; break;
		default : experience = 5100000; break;
	}

	accuracy[0] = data.accuracy[0];
	accuracy[1] = data.accuracy[1];
	accuracy[2] = 0;

	poison      = 0;
	unconscious = 0;
	dead        = 0;

	weapon      = 10;
	shield      = 6;
	armor       = 6;

	pow_of_weapon = level[0] * 2 + 10;
	pow_of_shield = 0;
	pow_of_armor  = ac;
}

void TPcPlayer::CastAttackSpell(void)
{
	CLoreConsole& console = GetConsole();
	
	console.SetTextColorIndex(7);
	console.Write(resource::GetMessageString(resource::MESSAGE_CANNOT_USE_ATTACK_MAGIC_NOT_IN_BATTLE_MODE));
	console.Display();
}

void TPcPlayer::CastCureSpell(void)
{
	TPcPlayer* pPlayer = this;

	TMenuList menu;

	menu.push_back(resource::GetMessageString(resource::MESSAGE_TO_WHOM));

	TPcPlayer* table[10];
	int maxTable = 0;

	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->Valid())
		{
			table[maxTable++] = (*obj);
			menu.push_back((*obj)->GetName());
		}
	}

	menu.push_back(resource::GetMessageString(resource::MESSAGE_TO_EVERYBODY));

	int whom = CSelect(menu)() - 1;
	if (whom < 0)
		return;

	if (whom < maxTable)
	{
		int nEnabled = pPlayer->level[1] / 2 + 1;
		if (nEnabled > 7)
			nEnabled = 7;

		menu.clear();
		menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECTION));
		for (int i = 0; i < 7; i++)
			menu.push_back(resource::GetMagicName(19+i).szName);

		int selected = CSelect(menu, nEnabled)();
		if (selected <= 0)
			return;

		TPcPlayer* pTarget = table[whom];

		switch (selected)
		{
		case 1:
			pPlayer->m_HealOne(pTarget);
			break;
		case 2:
			pPlayer->m_AntidoteOne(pTarget);
			break;
		case 3:
			pPlayer->m_AntidoteOne(pTarget);
			pPlayer->m_HealOne(pTarget);
			break;
		case 4:
			pPlayer->m_RecoverConsciousnessOne(pTarget);
			break;
		case 5:
			pPlayer->m_RevitalizeOne(pTarget);
			break;
		case 6:
			pPlayer->m_RecoverConsciousnessOne(pTarget);
			pPlayer->m_AntidoteOne(pTarget);
			pPlayer->m_HealOne(pTarget);
			break;
		case 7:
			pPlayer->m_RevitalizeOne(pTarget);
			pPlayer->m_RecoverConsciousnessOne(pTarget);
			pPlayer->m_AntidoteOne(pTarget);
			pPlayer->m_HealOne(pTarget);
			break;
		default:
			assert(false);
		}
	}
	else
	{
		int nEnabled = pPlayer->level[1] / 2 - 3;
		if (nEnabled <= 0) //@@ 원래는 < 0 이었음 검토 필요
		{
			game::console::WriteConsole(7, 3, pPlayer->GetName(EJOSA_SUB), " ", resource::GetMessageString(resource::MESSAGE_CANNOT_USE_POWERFUL_CURE_SPELL));
			game::PressAnyKey();

			return;
		}

		if (nEnabled > 7)
			nEnabled = 7;

		menu.clear();
		menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECTION));
		for (int i = 0; i < 7; i++)
			menu.push_back(resource::GetMagicName(26+i).szName);

		int selected = CSelect(menu, nEnabled)();
		if (selected < 0)
			return;

		switch (selected+1)
		{
		case 1:
			pPlayer->m_HealAll();
			break;
		case 2:
			pPlayer->m_AntidoteAll();
			break;
		case 3:
			pPlayer->m_AntidoteAll();
			pPlayer->m_HealAll();
			break;
		case 4:
			pPlayer->m_RecoverConsciousnessAll();
			break;
		case 6:
			pPlayer->m_RevitalizeAll();
			break;
		case 5:
			pPlayer->m_RecoverConsciousnessAll();
			pPlayer->m_AntidoteAll();
			pPlayer->m_HealAll();
			break;
		case 7:
			pPlayer->m_RevitalizeAll();
			pPlayer->m_RecoverConsciousnessAll();
			pPlayer->m_AntidoteAll();
			pPlayer->m_HealAll();
			break;
		}
	}

	GetConsole().Write("");
	GetConsole().Write("");
	GetConsole().Display();

	game::window::DisplayStatus();

	game::UpdateScreen();
	game::PressAnyKey();
}

void TPcPlayer::CastPhenominaSpell(void)
{
	TPcPlayer* pPlayer = this;

	TMenuList menu;
	menu.reserve(8);

	menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECTION));
	for (int i = 0; i < 8; i++)
		menu.push_back(resource::GetMagicName(i+33).szName);

	int nEnabled = (pPlayer->level[1] > 1) ? (pPlayer->level[1] / 2 + 1) : 1;
	if (nEnabled > 8)
		nEnabled = 8;

	int selected = CSelect(menu, nEnabled)();
	if (selected <= 0)
		return;

	TPcParty& party = game::object::GetParty();
	TMap& map = game::object::GetMap();

	switch (selected)
	{
	case 1:
		{
			if (pPlayer->sp < 1)
			{
				m_PrintSpNotEnough();
				return;
			}

			pPlayer->sp -= 1;

			party.IgniteTorch();
		}
		break;
	case 2:
		{
			if (pPlayer->sp < 5)
			{
				m_PrintSpNotEnough();
				return;
			}

			pPlayer->sp -= 5;

			party.Levitate();
		}
		break;
	case 3:
		{
			if (pPlayer->sp < 10)
			{
				m_PrintSpNotEnough();
				return;
			}

			pPlayer->sp -= 10;

			party.WalkOnWater();
		}
		break;
	case 4:
		{
			if (pPlayer->sp < 20)
			{
				m_PrintSpNotEnough();
				return;
			}

			pPlayer->sp -= 20;

			party.WalkOnSwamp();
		}
		break;
	case 5:
		{
			if (pPlayer->sp < 25)
			{
				m_PrintSpNotEnough();
				return;
			}

			TMenuList menu;

			menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECT_DIRECTION));
			menu.push_back("북쪽으로 기화 이동");
			menu.push_back("남쪽으로 기화 이동");
			menu.push_back("동쪽으로 기화 이동");
			menu.push_back("서쪽으로 기화 이동");

			int selected = CSelect(menu)();

			if (selected <= 0)
				return;

			int x = party.x;
			int y = party.y;

			switch (selected)
			{
			case 1: y -= 2; break;
			case 2: y += 2; break;
			case 3: x += 2; break;
			case 4: x -= 2; break;
			default:
				assert(false);
			}

			if (!game::map::IsValidWarpPos(x, y))
				return;

			if (!map.IsJumpable(x, y))
			{
				game::console::ShowMessage(7, "기화 이동이 통하지 않습니다.");
				return;
			}

			pPlayer->sp -= 25;

			// 기화 이동 중간에 이벤트 지점이 있을 때
			if (map.IsEventPos((party.x + x) / 2, (party.y + y) / 2))
			{
				game::console::ShowMessage(13, "알 수 없는 힘이 당신의 마법을 배척합니다.");
				return;
			}

			party.xPrev = (party.x + x) / 2;
			party.yPrev = (party.y + y) / 2;
			party.x = x;
			party.y = y;

			game::console::ShowMessage(15, "기화 이동을 마쳤습니다.");

			game::window::DisplayMap();
		}
		break;
	case 6:
		{
			if (map.HasHandicap(TMap::HANDICAP_TILECHANGING))
			{
				game::console::ShowMessage(13, "이곳의 악의 힘이 이 마법을 방해합니다.");
				return;
			}

			if (pPlayer->sp < 30)
			{
				m_PrintSpNotEnough();
				return;
			}

			TMenuList menu;

			menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECT_DIRECTION));
			menu.push_back("북쪽에 지형 변화");
			menu.push_back("남쪽에 지형 변화");
			menu.push_back("동쪽에 지형 변화");
			menu.push_back("서쪽에 지형 변화");

			int selected = CSelect(menu)();

			if (selected <= 0)
				return;

			int dx = 0;
			int dy = 0;

			switch (selected)
			{
			case 1: --dy; break;
			case 2: ++dy; break;
			case 3: ++dx; break;
			case 4: --dx; break;
			default:
				assert(false);
			}

			pPlayer->sp -= 30;

			// 이벤트 지점에 대해 지형 변화를 하려할 때
			if (map.IsEventPos(party.x + dx, party.y + dy))
			{
				game::console::ShowMessage(13, "알 수 없는 힘이 당신의 마법을 배척합니다.");
				return;
			}

			map.ChangeToWay(party.x + dx, party.y + dy);

			game::console::ShowMessage(15, "지형 변화에 성공했습니다.");

			game::window::DisplayMap();
		}
		break;
	case 7:
		{
			if (map.HasHandicap(TMap::HANDICAP_TELEPORT))
			{
				game::console::ShowMessage(13, "이곳의 악의 힘이 이 마법을 방해합니다.");
				return;
			}

			if (pPlayer->sp < 50)
			{
				m_PrintSpNotEnough();
				return;
			}

			TMenuList menu;

			menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECT_DIRECTION));
			menu.push_back(resource::GetMessageString(resource::MESSAGE_TELEPORT_TO_NORTH));
			menu.push_back(resource::GetMessageString(resource::MESSAGE_TELEPORT_TO_SOUTH));
			menu.push_back(resource::GetMessageString(resource::MESSAGE_TELEPORT_TO_EAST));
			menu.push_back(resource::GetMessageString(resource::MESSAGE_TELEPORT_TO_WEST));

			int selected = CSelect(menu)();

			if (selected <= 0)
				return;

			int dx = 0;
			int dy = 0;

			switch (selected)
			{
			case 1: --dy; break;
			case 2: ++dy; break;
			case 3: ++dx; break;
			case 4: --dx; break;
			default:
				assert(false);
			}

			GetConsole().Write("");
			GetConsole().SetTextColorIndex(11);
			GetConsole().Write("당신의 공간 이동력을 지정");
			GetConsole().SetTextColorIndex(15);
			GetConsole().Write(" ## 5000 공간 이동력");
			GetConsole().Display();

			//@@ console에서 text extent 형식으로 얻어 와야 함
			int power = 5000;
			{
				int xOrigin;
				int yOrigin;

				game::window::GetRegionForConsole(&xOrigin, &yOrigin, NULL, NULL);

				power = CSelectUpDown(xOrigin + 4*6, yOrigin + 2*12, 1000, 9000, 1000, power, game::GetRealColor(10), game::GetRealColor(8))();
				if (power < 1000)
					return;

				power /= 1000;
			}

			int x = party.x + dx * power;
			int y = party.y + dy * power;

			if (!game::map::IsValidWarpPos(x, y))
			{
				game::console::ShowMessage(7, "공간 이동이 통하지 않습니다.");
				return;
			}

			// 이벤트 지점에 대해 지형 변화를 하려할 때
			if (!map.IsTeleportable(x, y))
			{
				game::console::ShowMessage(13, "공간 이동 장소로 부적합 합니다.");
				return;
			}

			pPlayer->sp -= 50;

			// 순간 이동 지점이 이벤트 지점일 때
			if (map.IsEventPos(x, y))
			{
				game::console::ShowMessage(13, "알 수 없는 힘이 당신을 배척합니다.");
				return;
			}

			party.Warp(TPcParty::POS_ABS, x, y);

			game::console::ShowMessage(15, "공간 이동 마법이 성공했습니다.");

			game::window::DisplayMap();
		}
		break;
	case 8:
		{
			if (pPlayer->sp < 30)
			{
				m_PrintSpNotEnough();
				return;
			}

			std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

			int nPlayer = std::for_each(player.begin(), player.end(), FnctNumOfRegistered<TPcPlayer*>()).Result();
			assert(nPlayer > 0);

			party.food += nPlayer;
			if (party.food > 255)
				party.food = 255;

			pPlayer->sp -= 30;

			{

				CLoreConsole& console = GetConsole();
				console.Clear();
				console.SetTextColorIndex(15);
				console.Write(" 식량 제조 마법은 성공적으로 수행되었습니다");

				{
					avej_lite::util::string s;
					s += "            ";
					s += IntToStr(nPlayer)();
					s += " 개의 식량이 증가됨";
					console.Write(s);
				}

				{
					avej_lite::util::string s;
					s += "      일행의 현재 식량은 ";
					s += IntToStr(party.food)();
					s += " 개 입니다";
					console.SetTextColorIndex(11);
					console.Write(s);
				}

				console.Display();
			}

		}
		break;
	default:
		assert(false);
	}

	game::window::DisplayStatus();
}

void TPcPlayer::AttackWithWeapon(int ixObject, int ixEnemy)
{
	TPcPlayer* pPlayer = this;

	int nEnemyAlive = game::enemy::GetNumOfConsciousEnemy();
	if (nEnemyAlive == 0)
		return;

	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();

	while (enemy[ixEnemy]->dead > 0)
	{
		if (++ixEnemy > enemy.size())
			return;
	}

	TPcEnemy* pEnemy = enemy[ixEnemy];

	assert(pPlayer != NULL);
	assert(pEnemy  != NULL);

	CLoreConsole& console = GetConsole();

	console.SetTextColorIndex(15);
	console.Write(GetBattleMessage(*pPlayer, 1, ixObject, *pEnemy));

	const char* szGender = pPlayer->Get3rdPersonName();

	if ((pEnemy->unconscious > 0) && (pEnemy->dead == 0))
	{
		switch (AvejUtil::Random(4))
		{
		case 0:
			game::console::WriteConsole(12, 4, szGender, "의 무기가 ", pEnemy->GetName(), "의 심장을 꿰뚫었다");
			break;
		case 1:
			game::console::WriteConsole(12, 4, pEnemy->GetName(), "의 머리는 ", szGender, "의 공격으로 산산 조각이 났다");
			break;
		case 2:
			game::console::WriteConsole(12, 1, "적의 피가 사방에 뿌려졌다");
			break;
		case 3:
			game::console::WriteConsole(12, 1, "적은 비명과 함께 찢겨 나갔다");
			break;
		}

		sound::PlayFx(sound::SOUND_SCREAM2);

		pPlayer->m_PlusExperience(pEnemy);

		pEnemy->hp   = 0;
		pEnemy->dead = 1;

		return;
	}

	if (AvejUtil::Random(20) > pPlayer->accuracy[0])
	{
		game::console::WriteConsole(12, 2, szGender, "의 공격은 빗나갔다 ....");
		return;
	}

	if (AvejUtil::Random(100) < pEnemy->resistance)
	{
		game::console::WriteConsole(7, 3, "적은 ", szGender, "의 공격을 저지 했다");
		return;
	}

	int damage = (pPlayer->strength * pPlayer->pow_of_weapon * pPlayer->level[0]) / 20;
	damage -= (damage * AvejUtil::Random(50) / 100);
	damage -= (pEnemy->ac * pEnemy->level * (AvejUtil::Random(10)+1) / 10);

	if (damage <= 0)
	{
		game::console::WriteConsole(7, 3, "그러나 적은 ", szGender, "의 공격을 막았다");
		return;
	}

	pEnemy->hp -= damage;

	if (pEnemy->hp <= 0)
	{
		pEnemy->hp = 0;
		pEnemy->unconscious = 0;
		pEnemy->dead = 0;

		game::console::WriteConsole(12, 3, "적은 ", szGender, "의 공격으로 의식 불명이 되었다");

		sound::PlayFx(sound::SOUND_HIT);

		pPlayer->m_PlusExperience(pEnemy);

		pEnemy->unconscious = 1;
	}
	else
	{
		game::console::WriteConsole(7, 3, "적은 ", IntToStr(damage)(), "만큼의 피해를 입었다"); // 원래는 중간이 15번 색

		sound::PlayFx(sound::SOUND_HIT);
	}

	game::window::DisplayBattle(0);
}

void TPcPlayer::CastSpellToAll(int ixObject)
{
	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();

	for (int ixEnemy = 0; ixEnemy < enemy.size(); ++ixEnemy)
	{
		if (enemy[ixEnemy]->Valid() && (enemy[ixEnemy]->dead == 0))
			CastSpellToOne(ixObject, ixEnemy);
	}
}

void TPcPlayer::CastSpellToOne(int ixObject, int ixEnemy)
{
	TPcPlayer* pPlayer = this;

	int nEnemyAlive = game::enemy::GetNumOfConsciousEnemy();
	if (nEnemyAlive == 0)
		return;

	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();

	while ((enemy[ixEnemy] == NULL) || (enemy[ixEnemy]->dead > 0))
	{
		if (++ixEnemy > enemy.size())
			return;
	}

	TPcEnemy* pEnemy = enemy[ixEnemy];

	assert(pPlayer != NULL);
	assert(pEnemy  != NULL);

	CLoreConsole& console = GetConsole();

	console.SetTextColorIndex(15);
	console.Write(GetBattleMessage(*pPlayer, 2, ixObject, *pEnemy));

	const char* szGender = pPlayer->Get3rdPersonName();

	if ((pEnemy->unconscious > 0) && (pEnemy->dead == 0))
	{
		game::console::WriteConsole(12, 2, szGender, "의 마법은 적의 시체 위에서 작열했다");

		sound::PlayFx(sound::SOUND_SCREAM1);

		pPlayer->m_PlusExperience(pEnemy);

		pEnemy->hp   = 0;
		pEnemy->dead = 1;

		return;
	}

	// 마법 소모치
	{
		int consumption = (ixObject * ixObject* pPlayer->level[1] + 1) / 2;

		DETECT_NOT_ENOUGH_SP(consumption)

		pPlayer->sp -= consumption;
	}

	if (AvejUtil::Random(20) >= pPlayer->accuracy[1])
	{
		game::console::WriteConsole(7, 3, "그러나, ", pEnemy->GetName(EJOSA_OBJ), " 빗나갔다");
		return;
	}

	if (AvejUtil::Random(100) < pEnemy->resistance)
	{
		game::console::WriteConsole(7, 4, pEnemy->GetName(EJOSA_SUB), " ", szGender, "의 마법을 저지 했다");
		return;
	}

	int damage = ixObject * ixObject* pPlayer->level[1] * 2;
	damage -= ((pEnemy->ac * pEnemy->level * (AvejUtil::Random(10)+1) + 5) / 10);
	if (damage <= 0)
	{
		game::console::WriteConsole(7, 5, "그러나, ", pEnemy->GetName(EJOSA_SUB), " ", szGender, "의 마법 공격을 막았다");
		return;
	}

	pEnemy->hp -= damage;

	if (pEnemy->hp <= 0)
	{
		pEnemy->hp = 0;
		pEnemy->unconscious = 0;
		pEnemy->dead = 0;

		game::console::WriteConsole(12, 4, pEnemy->GetName(EJOSA_SUB), " ", szGender, "의 마법에 의해 의식 불명이 되었다");

		sound::PlayFx(sound::SOUND_HIT);

		pPlayer->m_PlusExperience(pEnemy);

		pEnemy->unconscious = 1;
	}
	else
	{
		game::console::WriteConsole(7, 4, pEnemy->GetName(EJOSA_SUB), " ", IntToStr(damage)(), "만큼의 피해를 입었다"); // 원래는 중간이 15번 색

		sound::PlayFx(sound::SOUND_HIT);
	}

	game::window::DisplayBattle(0);
}

void TPcPlayer::UseESP(void)
{
	if (!m_CanUseESP())
	{
		game::console::WriteConsole(7, 1, "당신에게는 아직 능력이 없습니다.");
		return;
	}

	TMenuList menu;

	menu.clear();
	menu.push_back("사용할 초감각의 종류 ======>");
	for (int i = 1; i <= 5; i++)
		menu.push_back(resource::GetMagicName(40+i).szName);

	int selected = CSelect(menu)();
	if (selected <= 0)
		return;
	
	if (selected == 5)
	{
		avej_lite::util::string s;

		s += resource::GetMagicName(40+selected).szName;
		s += resource::GetMagicName(40+selected).szJosaSub1;
		s += " 전투 모드에서만 사용됩니다.";

		game::console::ShowMessage(7, s);
		return;
	}

	TPcPlayer* pPlayer = this;
	TPcParty& party = game::object::GetParty();

	switch (selected)
	{
	case 1:
		{
			{
				TMap& map = game::object::GetMap();

				if (map.HasHandicap(TMap::HANDICAP_SEETHROUGH))
				{
					game::console::ShowMessage(13, "이곳의 악의 힘이 이 마법을 방해합니다.");
					return;
				}
			}

			if (pPlayer->esp < 10)
			{
				m_PrintESPNotEnough();
				return;
			}

			pPlayer->esp -= 10;

			sound::MuteFx(true);

/*??
			move(font^[0],font^[55],246);

			for i = 5 to 246 do
				font^[0,i] = 0;

			if (position in [den,keep])
			{
				move(font^[52],chara^[55],246);
				for i = 5 to 246 do
					font^[52,i] = 0;
			}

			Scroll(TRUE);

			Print(15,"일행은 주위를 투시하고 있다.");

			PressAnyKey;

			move(font^[55],font^[0],246);

			if (position in [den,keep])
				move(chara^[55],font^[52],246);

			Scroll(TRUE);
*/
			sound::MuteFx(false);
		}
		break;

	case 2:
		{
			if (pPlayer->esp < 5)
			{
				m_PrintESPNotEnough();
				return;
			}

			pPlayer->esp -= 5;
/*??
			Print(7," 당신은 당신의 미래를 예언한다 ...");
			Print(7,"");

			k = ReturnPredict;

			if (k in [1..25])
				s = "당신은 " + Predict_Data[k] + " 것이다"
			else
				s = "당신은 어떤 힘에 의해 예언을 방해 받고 있다";

			cPrint(10,15," # ",s,"");

			PressAnyKey;
*/
		}
		break;

	case 3:
		{
			if (pPlayer->esp < 20)
			{
				m_PrintESPNotEnough();
				return;
			}

			pPlayer->esp -= 20;

			game::console::ShowMessage(5, "당신은 잠시동안 다른 사람의 마음을 읽을수 있다.");

			party.ability.mindControl = 3;
		}
		break;

	case 4:
		{
			{
				TMap& map = game::object::GetMap();

				if (map.HasHandicap(TMap::HANDICAP_CLAIRVOYANCE))
				{
					game::console::ShowMessage(13, "이곳의 악의 힘이 이 마법을 방해합니다.");
					return;
				}
			}

			if (pPlayer->esp < level[2] * 5)
			{
				m_PrintESPNotEnough();
				return;
			}

			menu.clear();
			menu.push_back(resource::GetMessageString(resource::MESSAGE_SELECT_DIRECTION));
			menu.push_back("북쪽으로 천리안을 사용");
			menu.push_back("남쪽으로 천리안을 사용");
			menu.push_back("동쪽으로 천리안을 사용");
			menu.push_back("서쪽으로 천리안을 사용");

			int selected = CSelect(menu)();
			if (selected <= 0)
				return;

			int x1 = 0;
			int y1 = 0;

			pPlayer->esp -= level[2] * 5;

			switch (selected)
			{
				case 1: y1 = -1; break;
				case 2: y1 =  1; break;
				case 3: x1 =  1; break;
				case 4: x1 = -1; break;
			}
/*??
			party.xaxis = x;
			party.yaxis = y;

			//@@ 사운드 끄는 부분 추가

			for i = 0 to 1 do
			{
				page = 1 - page;
				setactivepage(page);
				hany = 30;
				print(15,"천리안의 사용중 ...");
				setcolor(14);
				HPrintXY4Select(250,184,"아무키나 누르시오 ...");
			}

			for i = 1 to player[person].level[3] do
			{
				x = x + x1; y = y + y1;
				if ((x<5) or (x>==xmax-3) or (y<5) or (y>==ymax-3))
				{
					x = x - x1; y = y - y1;
				}
				else
				{
					Scroll(FALSE);
					c = readkey;

					if (c == #0)
						c = readkey;
					if (c == #27)
						goto Exit_For;
				}
			}

Exit_For:
			Clear;

			x = party.xaxis;
			y = party.yaxis;

			//@@ 사운드 설정 복원하는 부분 추가

			Scroll(TRUE);
*/
		}
		break;

	default:
		assert(0);
	}

	game::window::DisplayStatus();
}

void TPcPlayer::UseESPForBattle(int ixObject, int ixEnemy)
{
	TPcPlayer* pPlayer = this;

	assert(pPlayer != NULL);

	if (!m_CanUseESP())
	{
		game::console::WriteConsole(7, 1, "당신에게는 아직 능력이 없습니다.");
		return;
	}

	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();
	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();

	TPcEnemy* pEnemy = enemy[ixEnemy];

	assert(pEnemy  != NULL);

	if (pEnemy == NULL)
		return;

	if ((ixObject == 1) || (ixObject == 2) || (ixObject == 4))
	{
		game::console::WriteConsole(7, 3, resource::GetMagicName(ixObject+40).szName, resource::GetMagicName(ixObject+40).szJosaWith, " 전투모드에서는 사용할 수가 없습니다.");
		return;
	}

	if (ixObject == 3)
	{
		if (pPlayer->esp < 15)
		{
			game::console::WriteConsole(7, 1, "초감각 지수가 부족했다");
			return;
		}

		pPlayer->esp -= 15;

		static CSmSet s_MindControllable("6,10,20,24,27,29,33,35,40,47,53,62");

		if (!s_MindControllable.IsSet(pEnemy->E_number))
		{
			game::console::WriteConsole(7, 1, "독심술은 전혀 통하지 않았다");
			return;
		}

		int enemyLevel = pEnemy->level;
		//@@ 특수한 경우?
		if (pEnemy->E_number == 62)
			enemyLevel = 17;

		if ((enemyLevel > pPlayer->level[2]) && (AvejUtil::Random(2) == 0))
		{
			game::console::WriteConsole(7, 1, "적의 마음을 끌어들이기에는 아직 능력이 부족했다");
			return;
		}

		if (AvejUtil::Random(60) > (pPlayer->level[2]-enemyLevel)*2 + pPlayer->accuracy[2])
		{
			game::console::WriteConsole(7, 1, "적의 마음은 흔들리지 않았다");
			return;
		}

		game::console::WriteConsole(11, 1, "적은 우리의 편이 되었다");

		*player[player.size()-1] << GetEnemyData(pEnemy->E_number);

		pEnemy->dead        = 1;
		pEnemy->unconscious = 1;
		pEnemy->hp          = 0;
		pEnemy->level       = 0;

		game::window::DisplayStatus();
	}
	else
	{
		if (pPlayer->esp < 20)
		{
			game::console::WriteConsole(7, 1, "초감각 지수가 부족했다");
			return;
		}

		pPlayer->esp -= 20;

		int ixESP = AvejUtil::Random(pPlayer->level[2]) + 1;

		switch (ixESP)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			{
				switch (ixESP)
				{
				case 1:
				case 2:
					game::console::WriteConsole(7, 3, "주위의 돌들이 떠올라 ", pEnemy->GetName(EJOSA_OBJ), " 공격하기 시작한다");
					break;
				case 3:
				case 4:
					game::console::WriteConsole(7, 2, pEnemy->GetName(EJOSA_NONE), " 주위의 세균이 그에게 침투하여 해를 입히기 시작한다");
					break;
				case 5:
				case 6:
					game::console::WriteConsole(7, 4, pPlayer->GetGenderName(), "의 무기가 갑자기 ", pEnemy->GetName(EJOSA_NONE), "에게 달려들기 시작한다");
					break;
				}

				pEnemy->hp -= ixESP * 10;
				if (pEnemy->hp < 0)
					pEnemy->hp = 0;

				if ((pEnemy->unconscious > 0) && (pEnemy->dead == 0))
				{
					pEnemy->dead = 1;
					pPlayer->m_PlusExperience(pEnemy);
				}

				if ((pEnemy->hp <= 0) && (pEnemy->unconscious == 0))
				{
					pEnemy->unconscious = 1;
					pPlayer->m_PlusExperience(pEnemy);
				}
			}
			break;
		case 7:
		case 8:
		case 9:
		case 10:
			{
				switch (ixESP)
				{
				case 7:
				case 8:
					game::console::WriteConsole(7, 1, "갑자기 땅속의 우라늄이 핵분열을 일으켜 고온의 열기가 적의 주위를 감싸기 시작한다");
					break;
				case 9:
				case 10:
					game::console::WriteConsole(7, 1, "공기중의 수소가 돌연히 핵융합을 일으켜 질량 결손의 에너지를 적들에게 방출하기 시작한다");
					break;
				}

				for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
				{
					pEnemy = (*obj);

					if (!pEnemy->Valid())
						continue;

					pEnemy->hp -= ixESP * 5;
					if (pEnemy->hp < 0)
						pEnemy->hp = 0;

					if ((pEnemy->unconscious > 0) && (pEnemy->dead == 0))
					{
						pEnemy->dead = 1;
						pPlayer->m_PlusExperience(pEnemy);
					}

					if ((pEnemy->hp <= 0) && (pEnemy->unconscious == 0))
					{
						pEnemy->unconscious = 1;
						pPlayer->m_PlusExperience(pEnemy);
					}
				}
			}
			break;
		case 11:
		case 12:
			{
				game::console::WriteConsole(7, 2, pPlayer->GetGenderName(), "는 적에게 공포심을 불어 넣었다");

				if (AvejUtil::Random(40) < pEnemy->resistance)
				{
					if (pEnemy->resistance < 5)
						pEnemy->resistance = 0;
					else
						pEnemy->resistance = pEnemy->resistance - 5;
					return;
				}

				if (AvejUtil::Random(60) > pPlayer->accuracy[2])
				{
					if (pEnemy->endurance < 5)
						pEnemy->endurance = 0;
					else
						pEnemy->endurance = pEnemy->endurance - 5;
					return;
				}

				pEnemy->dead = 1;

				game::console::WriteConsole(10, 2, pEnemy->GetName(EJOSA_SUB), " 겁을 먹고는 도망 가버렸다");
			}
			break;
		case 13:
		case 14:
			{
				game::console::WriteConsole(7, 2, pPlayer->GetGenderName(), "는 적의 신진 대사를 조절하여 적의 체력을 점차 약화 시키려 한다");

				if (AvejUtil::Random(100) < pEnemy->resistance)
					return;

				if (AvejUtil::Random(40) > pPlayer->accuracy[2])
					return;

				++pEnemy->poison;
			}
			break;
		case 15:
		case 16:
		case 17:
			{
				game::console::WriteConsole(7, 2, pPlayer->GetGenderName(), "는 염력으로 적의 심장을 멈추려 한다");

				if (AvejUtil::Random(40) < pEnemy->resistance)
				{
					if (pEnemy->resistance < 5)
						pEnemy->resistance = 0;
					else
						pEnemy->resistance = pEnemy->resistance - 5;
					return;
				}

				if (AvejUtil::Random(80) > pPlayer->accuracy[2])
				{
					if (pEnemy->hp < 10)
					{
						pEnemy->hp = 0;
						pEnemy->unconscious = 1;
					}
					else
					{
						pEnemy->hp -= 10;
					}
					return;
				}

				++pEnemy->unconscious;
			}
			break;
		default:
			{
				game::console::WriteConsole(7, 2, pPlayer->GetGenderName(), "는 적을 환상속에 빠지게 하려한다");

				if (AvejUtil::Random(40) < pEnemy->resistance)
				{
					if (pEnemy->agility < 5)
						pEnemy->agility = 0;
					else
						pEnemy->agility = pEnemy->agility - 5;
					return;
				}

				if (AvejUtil::Random(30) > pPlayer->accuracy[2])
					return;

				for (int i = 0; i < 2; i++)
				{
					if (pEnemy->accuracy[i] > 0)
						--pEnemy->accuracy[i];
				}
			}
		}

		game::window::DisplayBattle(0); //@@DisplayEnemies(FALSE); 맞는지 모르겠음
	}
}

void TPcPlayer::CastSpellWithSpecialAbility(int ixObject, int ixEnemy)
{
	TPcPlayer* pPlayer = this;

	if (!pPlayer->m_CanUseSpecialMagic())
	{
		game::console::WriteConsole(7, 1, "당신에게는 아직 능력이 없습니다.");
		return;
	}

	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();

	TPcEnemy* pEnemy = enemy[ixEnemy];

	assert(pPlayer != NULL);
	assert(pEnemy  != NULL);

	if (pEnemy == NULL)
		return;

	switch (ixObject)
	{
	case 1:
		{
			DETECT_NOT_ENOUGH_SP(10)

			pPlayer->sp -= 10;

			if (AvejUtil::Random(100) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "적은 독 공격을 저지 했다");
				return;
			}

			if (AvejUtil::Random(40) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "독 공격은 빗나갔다");
				return;
			}

			game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_SUB), " 중독 되었다");
			++pEnemy->poison;
		}
		break;
	case 2:
		{
			DETECT_NOT_ENOUGH_SP(30)

			pPlayer->sp -= 30;

			if (AvejUtil::Random(100) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "기술 무력화 공격은 저지 당했다");
				return;
			}

			if (AvejUtil::Random(60) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "기술 무력화 공격은 빗나갔다");
				return;
			}

			game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_NONE), "의 특수 공격 능력이 제거되었다");

			pEnemy->special = 0;
		}
		break;
	case 3:
		{
			DETECT_NOT_ENOUGH_SP(15)

			pPlayer->sp -= 15;

			if (AvejUtil::Random(100) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "방어 무력화 공격은 저지 당했다");
				return;
			}

			int temp = (pEnemy->ac < 5) ? 40 : 25;

			if (AvejUtil::Random(temp) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "방어 무력화 공격은 빗나갔다");
				return;
			}

			game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_NONE), "의 방어 능력이 저하되었다");

			if ((pEnemy->resistance < 31) || (AvejUtil::Random(2) == 0))
				--pEnemy->ac;
			else
				pEnemy->resistance -= 10;
		}
		break;
	case 4:
		{
			DETECT_NOT_ENOUGH_SP(20)

			pPlayer->sp -= 20;

			if (AvejUtil::Random(200) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "능력 저하 공격은 저지 당했다");
				return;
			}

			if (AvejUtil::Random(30) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "능력 저하 공격은 빗나갔다");
				return;
			}

			game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_NONE), "의 전체적인 능력이 저하되었다");

			if (pEnemy->level > 1)
				--pEnemy->level;

			if (pEnemy->resistance > 0)
				pEnemy->resistance -= 10;
			else
				pEnemy->resistance = 0;
		}
		break;
	case 5:
		{
			DETECT_NOT_ENOUGH_SP(15)

			pPlayer->sp -= 15;

			if (AvejUtil::Random(100) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "마법 불능 공격은 저지 당했다");
				return;
			}

			if (AvejUtil::Random(100) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "마법 불능 공격은 빗나갔다");
				return;
			}

			if (pEnemy->castlevel > 0)
				--pEnemy->castlevel;

			if (pEnemy->castlevel > 0)
				game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_NONE), "의 마법 능력이 저하되었다");
			else
				game::console::WriteConsole(4, 2, pEnemy->GetName(EJOSA_NONE), "의 마법 능력은 사라졌다");

		}
		break;
	case 6:
		{
			DETECT_NOT_ENOUGH_SP(20)

			pPlayer->sp -= 20;

			if (AvejUtil::Random(100) < pEnemy->resistance)
			{
				game::console::WriteConsole(7, 1, "탈 초인화 공격은 저지 당했다");
				return;
			}

			if (AvejUtil::Random(100) > pPlayer->accuracy[1])
			{
				game::console::WriteConsole(7, 1, "탈 초인화 공격은 빗나갔다");
				return;
			}

			if (pEnemy->specialcastlevel > 0)
				--pEnemy->specialcastlevel;

			if (pEnemy->specialcastlevel > 0)
				game::console::WriteConsole(4, 1, pEnemy->GetName(EJOSA_NONE), "의 초자연적 능력이 저하되었다");
			else
				game::console::WriteConsole(4, 1, pEnemy->GetName(EJOSA_NONE), "의 초자연적 능력은 사라졌다");
		}
		break;
	default:
		assert(false);
	}
}

bool TPcPlayer::TryToRunAway()
{
	TPcPlayer* pPlayer = this;

	if (AvejUtil::Random(50) > pPlayer->agility)
	{
		game::console::WriteConsole(7, 1, "그러나, 일행은 성공하지 못했다");
		return false;
	}
	else
	{
		game::console::WriteConsole(11, 1, "성공적으로 도망을 갔다");
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// private method

template <typename inIter, typename function>
function for_each_param1(inIter iterBegin, inIter iterEnd, function func, int param1)
{
	while (iterBegin != iterEnd)
		func(*iterBegin++, param1);
	return func;
}

// 아군 캐릭터의 경험치를 올려주는 함수자
template <class type>
class FnctPlusExp
{
public:
	void operator()(type obj, int plus)
	{
		if (obj->Valid())
			obj->experience += plus;
	}
};

void TPcPlayer::m_HealOne(TPcPlayer* pTarget)
{
	TPcPlayer* pPlayer = this;

	assert(pPlayer);
	assert(pTarget);

	if ((pTarget->dead > 0) || (pTarget->unconscious > 0) || (pTarget->poison > 0))
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 치료될 상태가 아닙니다.");
		return;
	}

	if (pTarget->hp >= pTarget->endurance * pTarget->level[0])
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 치료할 필요가 없습니다.");
		return;
	}

	int consumption = 2 * pPlayer->level[1];

	if (pPlayer->sp < consumption)
	{
		if (!game::status::InBattle())
			m_PrintSpNotEnough();
		return;
	}

	pPlayer->sp -= consumption;
	pTarget->hp += (consumption * 3 / 2);

	if (pTarget->hp > pTarget->level[0] * pTarget->endurance)
		pTarget->hp = pTarget->level[0] * pTarget->endurance;

	game::console::WriteConsole(15, 2, pTarget->GetName(EJOSA_SUB), " 치료되어 졌습니다.");
}

void TPcPlayer::m_AntidoteOne(TPcPlayer* pTarget)
{
	TPcPlayer* pPlayer = this;

	if ((pTarget->dead > 0) || (pTarget->unconscious > 0))
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 독이 치료될 상태가 아닙니다.");
		return;
	}
	if (pTarget->poison == 0)
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 독에 걸리지 않았습니다.");
		return;
	}

	if (pPlayer->sp < 15)
	{
		if (!game::status::InBattle())
			m_PrintSpNotEnough();
		return;
	}

	pPlayer->sp -= 15;
	pTarget->poison = 0;

	game::console::WriteConsole(15, 2, pTarget->GetName(), "의 독은 제거 되었습니다.");
}

void TPcPlayer::m_RecoverConsciousnessOne(TPcPlayer* pTarget)
{
	TPcPlayer* pPlayer = this;

	if (pTarget->dead > 0)
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 의식이 돌아올 상태가 아닙니다.");
		return;
	}
	if (pTarget->unconscious == 0)
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 의식불명이 아닙니다.");
		return;
	}

	int consumption = 10 * pTarget->unconscious;

	if (pPlayer->sp < consumption)
	{
		if (!game::status::InBattle())
			m_PrintSpNotEnough();
		return;
	}

	pPlayer->sp -= consumption;
	pTarget->unconscious = 0;

	if (pTarget->hp <= 0)
		pTarget->hp = 1;

	game::console::WriteConsole(15, 2, pTarget->GetName(EJOSA_SUB), " 의식을 되찾았습니다.");
}

void TPcPlayer::m_RevitalizeOne(TPcPlayer* pTarget)
{
	TPcPlayer* pPlayer = this;

	if (pTarget->dead == 0)
	{
		if (!game::status::InBattle())
			game::console::WriteConsole(7, 2, pTarget->GetName(EJOSA_SUB), " 아직 살아 있습니다.");
		return;
	}

	int consumption = 30 * pTarget->dead;

	if (pPlayer->sp < consumption)
	{
		if (!game::status::InBattle())
			m_PrintSpNotEnough();
		return;
	}

	pPlayer->sp -= consumption;

	pTarget->dead = 0;

	if (pTarget->unconscious > pTarget->endurance * pTarget->level[0])
		pTarget->unconscious = pTarget->endurance * pTarget->level[0];

	if (pTarget->unconscious == 0)
		pTarget->unconscious = 1;

	game::console::WriteConsole(15, 2, pTarget->GetName(EJOSA_SUB), " 다시 생명을 얻었습니다.");
}

void TPcPlayer::m_HealAll(void)
{
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->Valid())
			m_HealOne(*obj);
	}
}

void TPcPlayer::m_AntidoteAll(void)
{
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->Valid())
			m_AntidoteOne(*obj);
	}
}

void TPcPlayer::m_RecoverConsciousnessAll(void)
{
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->Valid())
			m_RecoverConsciousnessOne(*obj);
	}
}

void TPcPlayer::m_RevitalizeAll(void)
{
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->Valid())
			m_RevitalizeOne(*obj);
	}
}

bool TPcPlayer::m_CanUseSpecialMagic(void)
{
	TPcParty& party = game::object::GetParty();
	return (party.ability.canUseSpecialMagic);
}

bool TPcPlayer::m_CanUseESP(void)
{
	// 아래의 조건 중 하나만 만족하면 된다

	// ESP 습득 여부
	{
		TPcParty& party = game::object::GetParty();
		if (party.ability.canUseESP)
			return true;
	}
		
	// Class 제약 확인
	{
		static const CSmSet s_ESPer("2,3,6");
		if (s_ESPer.IsSet(class_))
			return true;
	}

	return false;
}

void TPcPlayer::m_PlusExperience(TPcEnemy* pEnemy)
{
	TPcPlayer* pPlayer = this;

	assert(pPlayer != NULL);
	assert(pEnemy  != NULL);

	long plus = pEnemy->E_number;
	plus = plus * plus * plus / 8;

	if (plus == 0)
		plus = 1;

	if (pEnemy->unconscious == 0)
	{
		game::console::WriteConsole(14, 4, pPlayer->GetName(EJOSA_SUB), " ", IntToStr(plus)(), "만큼 경험치를 얻었다 !");
		pPlayer->experience += plus;
	}
	else
	{
		std::vector<TPcPlayer*>& player = game::object::GetPlayerList();
		for_each_param1(player.begin(), player.end(), FnctPlusExp<TPcPlayer*>(), plus);
	}
}

void TPcPlayer::m_PrintSpNotEnough(void)
{
	game::console::ShowMessage(7, "마법 지수가 충분하지 않습니다.");
}

void TPcPlayer::m_PrintESPNotEnough(void)
{
	game::console::ShowMessage(7, "ESP 지수가 충분하지 않습니다.");
}

////////////////////////////////////////////////////////////////////////////////
// global function

avej_lite::util::string GetBattleMessage(const TPcPlayer& who, int how, int what, const TPcEnemy& whom)
{
	avej_lite::util::string s;

	s += who.GetName(EJOSA_SUB);
	s += " ";

	switch (how)
	{
	case 1:
		s += resource::GetWeaponName(who.weapon).szName;
		s += resource::GetWeaponName(who.weapon).szJosaWith;
		s += "로 ";
		s += whom.GetName(EJOSA_OBJ);
		s += " 공격했다";
		break;
	case 2:
		s += resource::GetMagicName(what).szName;
		s += resource::GetMagicName(what).szJosaWith;
		s += "로 ";
		s += whom.GetName(EJOSA_NONE);
		s += "에게 공격했다";
		break;
	case 3:
		s += resource::GetMagicName(what+6).szName;
		s += resource::GetMagicName(what+6).szJosaWith;
		s += "로 ";
		s += whom.GetName(EJOSA_NONE);
		s += "에게 공격했다";
		break;
	case 4:
		s += whom.GetName(EJOSA_NONE);
		s += "에게 ";
		s += resource::GetMagicName(what+12).szName;
		s += resource::GetMagicName(what+12).szJosaWith;
		s += "로 특수 공격을 했다";
		break;
	case 5:
		s += whom.GetName(EJOSA_NONE);
		s += "에게 ";
		s += resource::GetMagicName(what+18).szName;
		s += resource::GetMagicName(what+18).szJosaObj;
		s += " 사용했다";
		break;
	case 6:
		s += whom.GetName(EJOSA_NONE);
		s += "에게 ";
		s += resource::GetMagicName(what+40).szName;
		s += resource::GetMagicName(what+40).szJosaObj;
		s += " 사용했다";
		break;
	case 7:
		s  = "일행은 도망을 시도했다";
		break;
	default:
		s += "잠시 주저했다";
	}

	return s;
}
