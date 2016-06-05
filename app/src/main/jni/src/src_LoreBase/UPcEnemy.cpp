
#include "UPcEnemy.h"

#include "avej_lite.h"
#include "UConfig.h"
#include "UPcPlayer.h"
#include "UExtern.h"
#include <assert.h>
#include <algorithm>

using namespace avej_lite;
namespace AvejUtil = ::avej_lite::util;

enum EPlayerStatus
{
	EPLAYERSTATUS_CONSCIOUS, // 의식이 있는 상태
	EPLAYERSTATUS_NOT_DEAD,  // 죽지 않은 상태
	EPLAYERSTATUS_ALL,       // 유효한 모든 상태
};

TPcPlayer* GetRandomPlayer(EPlayerStatus status);

////////////////////////////////////////////////////////////////////////////////
// static function

static void TPcEnemy_m_SetName(TPcEnemy& enemy, const char* szName)
{
	enemy.SetName(szName);
}

static void TPcEnemy_m_GetName(TPcEnemy& enemy, avej_lite::util::string& refName)
{
	const char* szName = enemy.GetName();
	refName = szName;
}

////////////////////////////////////////////////////////////////////////////////
// class static variable

TPcEnemy::TAttribMapInt TPcEnemy::s_m_attribMapIntList;
TPcEnemy::TAttribMapStr TPcEnemy::s_m_attribMapStrList;

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool TPcEnemy::_Load(const CReadStream& stream)
{
	return false;
}

bool TPcEnemy::_Save(const CWriteStream& stream) const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// public method

TPcEnemy::TPcEnemy(void)
{
	// static data의 초기화
	if (s_m_attribMapIntList.empty())
	{
		s_m_attribMapIntList["E_number" ]        = int(&this->E_number) - int(this);
		s_m_attribMapIntList["strength"]         = int(&this->strength) - int(this);
		s_m_attribMapIntList["mentality"]        = int(&this->mentality) - int(this);
		s_m_attribMapIntList["endurance"]        = int(&this->endurance) - int(this);
		s_m_attribMapIntList["resistance"]       = int(&this->resistance) - int(this);
		s_m_attribMapIntList["agility"]          = int(&this->agility) - int(this);
		s_m_attribMapIntList["accuracy[0]"]      = int(&this->accuracy[0]) - int(this);
		s_m_attribMapIntList["accuracy[1]"]      = int(&this->accuracy[1]) - int(this);
		s_m_attribMapIntList["ac"]               = int(&this->ac) - int(this);
		s_m_attribMapIntList["special"]          = int(&this->special) - int(this);
		s_m_attribMapIntList["castlevel"]        = int(&this->castlevel) - int(this);
		s_m_attribMapIntList["specialcastlevel"] = int(&this->specialcastlevel) - int(this);
		s_m_attribMapIntList["level"]            = int(&this->level) - int(this);
	}

	if (s_m_attribMapStrList.empty())
	{
		s_m_attribMapStrList["name"] = std::make_pair((void*)TPcEnemy_m_SetName, (void*)TPcEnemy_m_GetName);
	}
}

TPcEnemy::~TPcEnemy(void)
{
}

void TPcEnemy::operator<<(const TEnemyData& data)
{
	CT_ASSERT(sizeof(name) >= sizeof(data.name), name_is_too_small);

	// 이름 설정
	SetName(data.name);

	// 기타 속성 설정
	strength         = data.strength;
	mentality        = data.mentality;
	endurance        = data.endurance;
	resistance       = data.resistance;
	agility          = data.agility;
	accuracy[0]      = data.accuracy[0];
	accuracy[1]      = data.accuracy[1];
	ac               = data.ac;
	special          = data.special;
	castlevel        = data.castlevel;
	specialcastlevel = data.specialcastlevel;
	level            = data.level;

	// 속성에서 계산 가능한 값
	hp               = endurance * level;
	poison           = 0;
	unconscious      = 0;
	dead             = 0;

	E_number         = 1;
}

void TPcEnemy::operator<<(const TPcPlayer& player)
{
	SetName(player.GetName(EJOSA_NONE));

	strength         = player.strength;
	mentality        = player.mentality;
	endurance        = player.endurance;
	resistance       = player.resistance;
	agility          = player.agility;
	accuracy[0]      = player.accuracy[0];
	accuracy[1]      = player.accuracy[1];
	ac               = player.ac;
	if (player.class_ == 7)
		special = 2;
	else
		special = 0;
	castlevel        = player.level[1] / 4;
	specialcastlevel = 0;
	level            = player.level[0];

	hp               = endurance * level;
	poison           = 0;
	unconscious      = 0;
	dead             = 0;

	E_number         = 1;
}

void TPcEnemy::Attack(void)
{
	TPcEnemy* pEnemy = this;

	if (pEnemy->specialcastlevel > 0)
		pEnemy->m_CastSpellWithSpecialAbility();

	int agility = pEnemy->agility;
	if (agility > 20)
		agility = 20;

	if ((pEnemy->special > 0) && (AvejUtil::Random(50) < agility))
	{
		int nAlive = game::player::GetNumOfConsciousPlayer();
		if (nAlive > 3)
		{
			pEnemy->m_AttackWithSpecialAbility();
			return;
		}
	}

	if ((AvejUtil::Random(pEnemy->accuracy[0]*1000) > AvejUtil::Random(pEnemy->accuracy[1]*1000)) && (pEnemy->strength > 0))
	{
		pEnemy->m_AttackWithWeapon();
	}
	else
	{
		if (pEnemy->castlevel > 0)
			pEnemy->m_CastSpell();
		else
			pEnemy->m_AttackWithWeapon();
	}
}

////////////////////////////////////////////////////////////////////////////////
// private method

void TPcEnemy::m_AttackWithWeapon(void)
{
	TPcEnemy* pEnemy = this;

	if (AvejUtil::Random(20) >= pEnemy->accuracy[0])
	{
		game::console::WriteConsole(7, 2, pEnemy->GetName(EJOSA_SUB), " 빗맞추었다");
		return;
	}

	TPcPlayer* pPlayer = GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS);

	if (pPlayer == NULL)
		return;

	int damage = pEnemy->strength * pEnemy->level * (AvejUtil::Random(10)+1) / 10;

	if (pPlayer->IsConscious())
	{
		if (AvejUtil::Random(50) < pPlayer->resistance)
		{
			game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_OBJ), " 공격했다");
			game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 적의 공격을 저지했다");
			return;
		}
		damage -= (pPlayer->ac * pPlayer->level[0] * (AvejUtil::Random(10)+1) / 10);
	}

	if (damage <= 0)
	{
		game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_OBJ), " 공격했다");
		game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 적의 공격을 방어했다");
		return;
	}

	if (pPlayer->dead > 0)
		pPlayer->dead += damage;
	if ((pPlayer->unconscious > 0) && (pPlayer->dead == 0))
		pPlayer->unconscious += damage;
	if (pPlayer->hp > 0)
		pPlayer->hp -= damage;

	game::console::WriteConsole(13, 4, pPlayer->GetName(EJOSA_SUB), " ", pEnemy->GetName(EJOSA_NONE), "에게 공격 받았다");
	game::console::WriteConsole(5, 4, pPlayer->GetName(EJOSA_SUB), " @D", IntToStr(damage)(), "@@만큼의 피해를 입었다");
}

void TPcEnemy::m_CastSpell(void)
{
	TPcEnemy*  pEnemy  = this;
	TPcPlayer* pPlayer = NULL;

	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	switch (pEnemy->castlevel)
	{
	case 0:
		break;
	case 1:
		{
			pPlayer = player[AvejUtil::Random(player.size())];
			if (!pPlayer->IsConscious())
				pPlayer = player[AvejUtil::Random(player.size())];

			pEnemy->m_CastAttackSpellToOne(pPlayer);
		}
		break;
	case 2:
		{
			pEnemy->m_CastAttackSpellToOne(GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS));
		}
		break;
	case 3:
		{
			if (AvejUtil::Random(game::player::GetNumOfConsciousPlayer()) < 2)
				pEnemy->m_CastAttackSpellToOne(GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS));
			else
				pEnemy->m_CastAttackSpellToAll();
		}
		break;
	case 4:
		{
			if ((pEnemy->hp < pEnemy->endurance * pEnemy->level / 3) && (AvejUtil::Random(2) == 0))
			{
				pEnemy->m_CastCureSpell(pEnemy, pEnemy->level * pEnemy->mentality / 4);
			}
			else
			{
				if (AvejUtil::Random(game::player::GetNumOfConsciousPlayer()) < 2)
					pEnemy->m_CastAttackSpellToOne(GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS));
				else
					pEnemy->m_CastAttackSpellToAll();
			}
		}
		break;
	case 5:
		{
			if ((pEnemy->hp < pEnemy->endurance * pEnemy->level / 3) && (AvejUtil::Random(3) == 0))
			{
				pEnemy->m_CastCureSpell(pEnemy, pEnemy->level * pEnemy->mentality / 4);
			}
			else
			{
				if (AvejUtil::Random(game::player::GetNumOfConsciousPlayer()) < 2)
				{
					int totalHP = 0;
					int totalMaxHP = 0;

					for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
					{
						if ((*obj)->Valid())
						{
							totalHP += (*obj)->hp;
							totalMaxHP += (*obj)->endurance * (*obj)->level;
						}
					}

					if ((enemy.size() > 2) && (totalHP*3 < totalMaxHP) && (AvejUtil::Random(2) == 0))
					{
						for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
						{
							if ((*obj)->Valid())
								pEnemy->m_CastCureSpell((*obj), pEnemy->level * pEnemy->mentality / 6);
						}
					}
					else
					{
						// party에서 의식이 있는 사람 중에 가장 hp가 낮은 사람을 공격
						pPlayer = *player.begin();
						for (std::vector<TPcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
						{
							if ((*obj)->Valid())
							{
								if ((*obj)->hp < pPlayer->hp)
									pPlayer = (*obj);
							}
						}

						if (pPlayer->Valid())
							pEnemy->m_CastAttackSpellToOne(pPlayer);
					}
				}
				else
				{
					pEnemy->m_CastAttackSpellToAll();
				}
			}
		}
		break;
	case 6:
		{
			if ((pEnemy->hp < pEnemy->endurance * pEnemy->level / 3) && (AvejUtil::Random(3) == 0))
			{
				pEnemy->m_CastCureSpell(pEnemy, pEnemy->level * pEnemy->mentality / 4);
				return;
			}

			int avgAC = 0;

			{
				int accAC = 0;
				int nPlayer = 0;;
				for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
				{
					if ((*obj) && ((*obj)->Valid()))
					{
						++nPlayer;
						accAC += ac;
					}
				}
				avgAC = (nPlayer > 0) ? (accAC / nPlayer) : 0;
			}

			if ((avgAC > 4) && (AvejUtil::Random(5) == 0))
			{
				for (std::vector<TPcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
				{
					if ((*obj)->Valid())
					{
						game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", (*obj)->GetName(EJOSA_NONE), "의 갑옷파괴를 시도했다");
						if ((*obj)->luck > AvejUtil::Random(21))
						{
							game::console::WriteConsole(7, 3, "그러나, ", pEnemy->GetName(EJOSA_SUB), " 성공하지 못했다");
						}
						else
						{
							game::console::WriteConsole(5, 2, (*obj)->GetName(EJOSA_NONE), "의 갑옷은 파괴되었다");
							if ((*obj)->ac > 0)
								--(*obj)->ac;
						}
					}
				}

				game::window::DisplayStatus();
			}
			else
			{
				int totalHP = 0;
				int totalMaxHP = 0;

				for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
				{
					if ((*obj)->Valid())
					{
						totalHP += (*obj)->hp;
						totalMaxHP += (*obj)->endurance * (*obj)->level;
					}
				}

				if ((enemy.size() > 2) && (totalHP*3 < totalMaxHP) && (AvejUtil::Random(3) == 0))
				{
					for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
					{
						if ((*obj)->Valid())
							pEnemy->m_CastCureSpell((*obj), pEnemy->level * pEnemy->mentality / 6);
					}
				}
				else
				{
					if (AvejUtil::Random(game::player::GetNumOfConsciousPlayer()) < 2)
					{
						// party에서 의식이 있는 사람 중에 가장 hp가 낮은 사람을 공격
						pPlayer = *player.begin();
						int hp = (pPlayer->IsConscious()) ? pPlayer->hp : 0x7FFFFFFFL;
						for (std::vector<TPcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
						{
							if ((*obj)->Valid())
							{
								if ((*obj)->IsConscious())
								{
									if ((*obj)->hp < hp)
									{
										pPlayer = (*obj);
										hp = pPlayer->hp;
									}
								}
							}
						}

						if (pPlayer->Valid())
							pEnemy->m_CastAttackSpellToOne(pPlayer);
					}
					else
					{
						pEnemy->m_CastAttackSpellToAll();
					}
				}
			}
		}
		break;
	default:
		assert(false);
	}
}

void TPcEnemy::m_AttackWithSpecialAbility(void)
{
	TPcEnemy*  pEnemy  = this;
	TPcPlayer* pPlayer = NULL;

	switch (pEnemy->special)
	{
	case 1:
		{
			for (int i = 0; i < 5; i++)
			{
				pPlayer = GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS);
				if ((pPlayer != NULL) && (pPlayer->poison == 0))
					break;
			}

			if (pPlayer == NULL)
				return;

			game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_NONE), "에게 독 공격을 시도했다");

			if (AvejUtil::Random(40) > pEnemy->agility)
			{
				game::console::WriteConsole(7, 1, "독 공격은 실패했다");
				return;
			}

			if (AvejUtil::Random(20) < pPlayer->luck)
			{
				game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 독 공격을 피했다");
				return;
			}

			game::console::WriteConsole(4, 2, pPlayer->GetName(EJOSA_SUB), " 중독 되었다 !!");

			++pPlayer->poison;
		}
		break;
	case 2:
		{
			pPlayer = GetRandomPlayer(EPLAYERSTATUS_CONSCIOUS);

			if (pPlayer == NULL)
				return;

			game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_NONE), "에게 치명적 공격을 시도했다");

			if (AvejUtil::Random(50) > pEnemy->agility)
			{
				game::console::WriteConsole(7, 1, "치명적 공격은 실패했다");
				return;
			}

			if (AvejUtil::Random(20) < pPlayer->luck)
			{
				game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 치명적 공격을 피했다");
				return;
			}

			game::console::WriteConsole(4, 2, pPlayer->GetName(EJOSA_SUB), " 의식불명이 되었다 !!");

			if (pPlayer->unconscious == 0)
			{
				pPlayer->unconscious = 1;
				if (pPlayer->hp > 0)
					pPlayer->hp = 0;
			}
		}
		break;
	case 3:
		{
			pPlayer = GetRandomPlayer(EPLAYERSTATUS_NOT_DEAD);

			if (pPlayer == NULL)
				return;

			game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_NONE), "에게 죽음의 공격을 시도했다");

			if (AvejUtil::Random(60) > pEnemy->agility)
			{
				game::console::WriteConsole(7, 1, "죽음의 공격은 실패했다");
				return;
			}

			if (AvejUtil::Random(20) < pPlayer->luck)
			{
				game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 죽음의 공격을 피했다");
				return;
			}

			game::console::WriteConsole(4, 2, pPlayer->GetName(EJOSA_SUB), " 죽었다 !!");

			if (pPlayer->dead == 0)
			{
				pPlayer->dead = 1;
				if (pPlayer->hp > 0)
					pPlayer->hp = 0;
			}
		}
		break;
	}
}

void TPcEnemy::m_CastSpellWithSpecialAbility(void)
{
	//?? 전체 코드 다시 봐야 함
	TPcEnemy* pEnemy = this;

	if (pEnemy->specialcastlevel <= 0)
		return;

	// 특수한 경우임, 보통은 코드로 조작된 적
	if (pEnemy->E_number <= 1)
		return;

	std::vector<TPcEnemy*>& enemy = game::object::GetEnemyList();
	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	int nEnemy = enemy.size();

	{
		// 죽지 않은 적의 수
		int nNotDead = 0;
		// 죽은 적 중에서 제일 앞에 있는 적
		int ixEnemy = nEnemy - 1;

		for (int i = nEnemy - 1; i >= 0; --i)
		{
			if (enemy[i]->dead == 0)
				++nNotDead;
			else
				ixEnemy = i;
		}

		if ((nNotDead < AvejUtil::Random(3)+2) && (AvejUtil::Random(3) == 0))
		{
			if (nEnemy < enemy.capacity())
			{
				game::battle::RegisterEnemy(pEnemy->E_number + AvejUtil::Random(4) - 20);
				nEnemy = enemy.size();
			}
			else
			{
				int index = pEnemy->E_number + AvejUtil::Random(4) - 20;
				//@@ enemy[ixEnemy]의 유효성이 검증되었나?
				*enemy[ixEnemy] << GetEnemyData(index);
				enemy[ixEnemy]->E_number = index;
			}

			game::window::DisplayBattle(0); //@@ 0 맞나?
			game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", enemy[ixEnemy]->GetName(EJOSA_OBJ), " 생성시켰다");
		}

		if (pEnemy->specialcastlevel > 1)
		{
			TPcPlayer* pLastPlayer = *(player.end() - 1);
			if ((pLastPlayer->Valid()) && (nNotDead < 7) && (AvejUtil::Random(5) == 0))
			{
				if (nEnemy < enemy.capacity())
					ixEnemy = nEnemy++;

				*enemy[ixEnemy] << *pLastPlayer;
				pLastPlayer->SetName("");

				game::window::DisplayStatus();
				game::window::DisplayBattle(0); //@@ 0이 맞는지 확인 필요

				game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB2), " 독심술을 사용하여 ", enemy[ixEnemy]->GetName(EJOSA_OBJ), " 자기편으로 끌어들였다");
			}
		}

		if (pEnemy->specialcastlevel > 2)
		{
			int ixPlayer;

			if (pEnemy->special == 0)
				return;

			if (AvejUtil::Random(5) == 0)
			for (ixPlayer = 0; ixPlayer < 6; ixPlayer++)
			if ((player[ixPlayer]->dead == 0) && (player[ixPlayer]->Valid()))
			{
				game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", player[ixPlayer]->GetName(EJOSA_NONE), "에게 죽음의 공격을 시도했다");

				if (AvejUtil::Random(60) > pEnemy->agility)
					game::console::WriteConsole(7, 1, "죽음의 공격은 실패했다");
				else if (AvejUtil::Random(20) < player[ixPlayer]->luck)
					game::console::WriteConsole(7, 3, "그러나, ", player[ixPlayer]->GetName(EJOSA_SUB), "죽음의 공격을 피했다");
				else
				{
					game::console::WriteConsole(4, 2, player[ixPlayer]->GetName(EJOSA_SUB), " 죽었다!!");
					if (player[ixPlayer]->dead == 0)
					{
						player[ixPlayer]->dead = 1;
						if (player[ixPlayer]->hp > 0)
							player[ixPlayer]->hp = 0;
					}
				}
			}
		}
		
	}
}

void TPcEnemy::m_CastAttackSpellToAll(void)
{
	TPcEnemy* pEnemy = this;

	//@@ 원래는 리소스 관리 쪽으로 넘어 가야 함
	struct TMagicTable
	{
		const char* szMagicName;
		int damage;
	};
	
	static const TMagicTable C_MAGIC_TABLE[] = 
	{
		{ "열파",     1},  // 0
		{ "에너지",   2},  // 2
		{ "초음파",   3},  // 3
		{ "혹한기",   5},  // 4
		{ "화염폭풍", 8},  // 5
	};

	const int MAX_CONVERT_TABLE = 21;
	static const int C_CONVERT_TABLE[MAX_CONVERT_TABLE+1] =
	{
	//  0  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
		1, 1, 1, 2, 2, 2, 2, 3, 3, 3,
		3, 4
	};

	int mentality = pEnemy->mentality;
	if (mentality < 0)
		mentality = 0;
	if (mentality > MAX_CONVERT_TABLE)
		mentality = MAX_CONVERT_TABLE;

	const TMagicTable& magic = C_MAGIC_TABLE[C_CONVERT_TABLE[mentality]];

	game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " 일행 모두에게 '", magic.szMagicName, "'마법을 사용했다");

	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
		pEnemy->m_CastAttackSpellSub(*obj, magic.damage * pEnemy->level);
}

void TPcEnemy::m_CastAttackSpellToOne(TPcPlayer* pPlayer)
{
	TPcEnemy* pEnemy = this;

	//@@ 원래는 리소스 관리 쪽으로 넘어 가야 함
	struct TMagicTable
	{
		const char* szMagicName;
		int damage;
	};
	
	static const TMagicTable C_MAGIC_TABLE[] = 
	{
		{ "충격", 1},  // 0
		{ "냉기", 2},  // 2
		{ "고통", 4},  // 3
		{ "혹한", 6},  // 4
		{ "화염", 7},  // 5
		{ "번개", 10}, // 6
	};

	const int MAX_CONVERT_TABLE = 20;
	static const int C_CONVERT_TABLE[MAX_CONVERT_TABLE+1] =
	{
	//  0  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 1, 1, 1, 1, 1, 2,
		2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
		5
	};

	if (pPlayer == NULL)
		return;

	int mentality = pEnemy->mentality;
	if (mentality < 0)
		mentality = 0;
	if (mentality > MAX_CONVERT_TABLE)
		mentality = MAX_CONVERT_TABLE;

	const TMagicTable& magic = C_MAGIC_TABLE[C_CONVERT_TABLE[mentality]];

	game::console::WriteConsole(13, 6, pEnemy->GetName(EJOSA_SUB), " ", pPlayer->GetName(EJOSA_NONE), "에게 '", magic.szMagicName, "'마법을 사용했다");

	pEnemy->m_CastAttackSpellSub(pPlayer, magic.damage * pEnemy->level);
}

void TPcEnemy::m_CastCureSpell(TPcEnemy* pTarget, int recovery)
{
	TPcEnemy* pEnemy = this;

	if (pEnemy == pTarget)
		game::console::WriteConsole(13, 2, pEnemy->GetName(EJOSA_SUB), " 자신을 치료했다");
	else
		game::console::WriteConsole(13, 4, pEnemy->GetName(EJOSA_SUB), " ", pTarget->GetName(EJOSA_OBJ), " 치료했다");

	if (pTarget->dead > 0)
	{
		pTarget->dead = 0;
	}
	else if (pTarget->unconscious > 0)
	{
		pTarget->unconscious = 0;
		if (pTarget->hp <= 0)
			pTarget->hp = 1;
	}
	else
	{
		pTarget->hp += recovery;
		if (pTarget->hp > pTarget->endurance * pTarget->level)
			pTarget->hp = pTarget->endurance * pTarget->level;
	}
}

void TPcEnemy::m_CastAttackSpellSub(TPcPlayer* pPlayer, int damage)
{
	TPcEnemy* pEnemy = this;

	{
		if (AvejUtil::Random(20) >= pEnemy->accuracy[1])
		{
			game::console::WriteConsole(7, 2, pEnemy->GetName(EJOSA_NONE), "의 마법공격은 빗나갔다");
			return;
		}

		if (pPlayer->IsConscious())
		if (AvejUtil::Random(50) < pPlayer->resistance)
		{
			game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 적의 마법을 저지했다");
			return;
		}

		damage -= AvejUtil::Random(damage / 2);

		if (pPlayer->IsConscious())
			damage -= (pPlayer->ac * pPlayer->level[0] * (AvejUtil::Random(10)+1) / 10);

		if (damage <= 0)
		{
			game::console::WriteConsole(7, 3, "그러나, ", pPlayer->GetName(EJOSA_SUB), " 적의 마법을 막아냈다");
			return;
		}

		if (pPlayer->dead > 0)
			pPlayer->dead += damage;

		if ((pPlayer->unconscious > 0) && (pPlayer->dead == 0))
			pPlayer->unconscious += damage;

		if (pPlayer->hp > 0)
			pPlayer->hp -= damage;

		game::console::WriteConsole(5, 4, pPlayer->GetName(EJOSA_SUB), " @D", IntToStr(damage)(), "@@만큼의 피해를 입었다");
	}
}

bool TPcEnemy::ChangeAttribute(const char* attribute, int value)
{
	TAttribMapInt::iterator attrib = s_m_attribMapIntList.find(attribute);

	if (attrib == s_m_attribMapIntList.end())
		return false;

	int* pInt = reinterpret_cast<int*>(int(this) + int(attrib->second));

	*pInt = value;

	return true;
}

bool TPcEnemy::ChangeAttribute(const char* attribute, const char* data)
{
	TAttribMapStr::iterator attrib = s_m_attribMapStrList.find(attribute);

	if (attrib == s_m_attribMapStrList.end())
		return false;

	TFnSet((attrib->second).first)(*this, data);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// global function

const TEnemyData& GetEnemyData(int index)
{
	static bool s_isFirst = true;
	static TEnemyData s_enemyData[75] =
	{	// name                 str men end res agi accuracy   ac spe clv scl  lv
		{"Orc",                   8,  0,  8,  0,  8,{  8,  0},  1,  0,  0,  0,  1},
		{"Troll",                 9,  0,  6,  0,  9,{  9,  0},  1,  0,  0,  0,  1},
		{"Serpent",               7,  3,  7,  0, 11,{ 11,  6},  1,  1,  1,  0,  1},
		{"Earth Worm",            3,  5,  5,  0,  6,{ 11,  7},  1,  0,  1,  0,  1},
		{"Dwarf",                10,  0, 10,  0, 10,{ 10,  0},  2,  0,  0,  0,  2},
		{"Giant",                15,  0, 13,  0,  8,{  8,  0},  2,  0,  0,  0,  2},
		{"Phantom",               0, 12, 12,  0,  0,{  0, 13},  0,  0,  2,  0,  2},
		{"Wolf",                  7,  0, 11,  0, 15,{ 15,  0},  1,  0,  0,  0,  2},
		{"Imp",                   8,  8, 10, 20, 18,{ 18, 10},  2,  0,  2,  0,  3},
		{"Goblin",               11,  0, 13,  0, 13,{ 13,  0},  3,  0,  0,  0,  3},
		{"Python",                9,  5, 10,  0, 13,{ 13,  6},  1,  1,  1,  0,  3},
		{"Insects",               6,  4,  8,  0, 14,{ 14, 15},  2,  1,  1,  0,  3},
		{"Giant Spider",         10,  0,  9,  0, 20,{ 13,  0},  2,  1,  0,  0,  4},
		{"Gremlin",              10,  0, 10,  0, 20,{ 20,  0},  2,  0,  0,  0,  4},
		{"Buzz Bug",             13,  0, 11,  0, 15,{ 15,  0},  1,  1,  0,  0,  4},
		{"Salamander",           12,  2, 13,  0, 12,{ 12, 10},  3,  1,  1,  0,  4},
		{"Blood Bat",            11,  0, 10,  0,  5,{ 15,  0},  1,  0,  0,  0,  5},
		{"Giant Rat",            13,  0, 18,  0, 10,{ 10,  0},  2,  0,  0,  0,  5},
		{"Skeleton",             10,  0, 19,  0, 12,{ 12,  0},  3,  0,  0,  0,  5},
		{"Kelpie",                8, 13,  8,  0, 14,{ 15, 17},  2,  0,  3,  0,  5},
		{"Gazer",                15,  8, 11,  0, 20,{ 15, 15},  3,  0,  2,  0,  6},
		{"Ghost",                 0, 15, 10,  0,  0,{  0, 15},  0,  0,  3,  0,  6},
		{"Slime",                 5, 13,  5,  0, 19,{ 19, 19},  2,  0,  2,  0,  6},
		{"Rock-Man",             19,  0, 15,  0, 10,{ 10,  0},  5,  0,  0,  0,  6},
		{"Kobold",                9,  9,  9,  0,  9,{  9,  9},  2,  0,  3,  0,  7},
		{"Mummy",                10, 10, 10,  0, 10,{ 10, 10},  3,  1,  2,  0,  7},
		{"Devil Hunter",         13, 10, 10,  0, 10,{ 10, 18},  3,  2,  2,  0,  7},
		{"Crazy One",             9,  9, 10,  0,  5,{  5, 13},  1,  0,  3,  0,  7},
		{"Ogre",                 19,  0, 19,  0,  9,{ 12,  0},  4,  0,  0,  0,  8},
		{"Headless",             10,  0, 15,  0, 10,{ 10,  0},  3,  2,  0,  0,  8},
		{"Mud-Man",              10,  0, 15,  0, 10,{ 10,  0},  7,  0,  0,  0,  8},
		{"Hell Cat",             10, 15, 11,  0, 18,{ 18, 16},  2,  2,  3,  0,  8},
		{"Wisp",                  5, 16, 10,  0, 20,{ 20, 20},  2,  0,  4,  0,  9},
		{"Basilisk",             10, 15, 12,  0, 20,{ 20, 10},  2,  1,  2,  0,  9},
		{"Sprite",                0, 20,  2, 80, 20,{ 20, 20},  0,  3,  5,  0,  9},
		{"Vampire",              15, 13, 14, 20, 17,{ 17, 15},  3,  1,  2,  0,  9},
		{"Molten Monster",        8,  0, 20, 50,  8,{ 16,  0},  3,  0,  0,  0, 10},
		{"Great Lich",           10, 10, 11, 10, 18,{ 10, 10},  4,  2,  3,  0, 10},
		{"Rampager",             20,  0, 19,  0, 19,{ 19,  0},  3,  0,  0,  0, 10},
		{"Mutant",                0, 10, 15,  0,  0,{  0, 20},  3,  0,  3,  0, 10},
		{"Rotten Corpse",        15, 15, 15, 60, 15,{ 15, 15},  2,  2,  3,  0, 11},
		{"Gagoyle",              10,  0, 20, 10, 10,{ 10,  0},  6,  0,  0,  0, 11},
		{"Wivern",               10, 10,  9, 30, 20,{ 20,  9},  3,  2,  3,  0, 11},
		{"Grim Death",           16, 16, 16, 50, 16,{ 16, 16},  2,  2,  3,  0, 12},
		{"Griffin",              15, 15, 15,  0, 15,{ 14, 14},  3,  2,  3,  0, 12},
		{"Evil Soul",             0, 20, 10,  0,  0,{  0, 15},  0,  3,  4,  0, 12},
		{"Cyclops",              20,  0, 20, 10, 20,{ 20,  0},  4,  0,  0,  0, 13},
		{"Dancing-Swd",          15, 20,  6, 20, 20,{ 20, 20},  0,  2,  4,  0, 13},
		{"Hydra",                15, 10, 20, 40, 18,{ 18, 12},  8,  1,  3,  0, 13},
		{"Stheno",               20, 20, 20,255, 10,{ 10, 10},255,  1,  3,  0, 14},
		{"Euryale",              20, 20, 15,255, 10,{ 15, 10},255,  2,  3,  0, 14},
		{"Medusa",               15, 10, 16, 50, 15,{ 15, 10},  4,  3,  3,  0, 14},
		{"Minotaur",             15,  7, 20, 40, 20,{ 20, 15}, 10,  0,  3,  0, 15},
		{"Dragon",               15,  7, 20, 50, 18,{ 20, 15},  9,  2,  4,  0, 15},
		{"Dark Soul",             0, 20, 40, 60,  0,{  0, 20},  0,  0,  5,  0, 15},
		{"Hell Fire",            15, 20, 30, 30, 15,{ 15, 15},  0,  3,  5,  0, 16},
		{"Astral Mud",           13, 20, 25, 40, 19,{ 19, 10},  9,  3,  4,  0, 16},
		{"Reaper",               15, 20, 33, 70, 20,{ 20, 20},  5,  1,  3,  0, 17},
		{"Crab God",             20, 20, 30, 20, 18,{ 18, 19},  7,  2,  4,  0, 17},
		{"Wraith",                0, 24, 35, 50, 15,{  0, 20},  2,  3,  4,  0, 18},
		{"Death Skull",           0, 20, 40, 80,  0,{  0, 20},  0,  2,  5,  0, 18},
		{"Draconian",            30, 20, 30, 60, 18,{ 18, 18},  7,  2,  5,  1, 19},
		{"Death Knight",         35,  0, 35, 50, 20,{ 20,  0},  6,  3,  0,  1, 19},
		{"Guardian-Lft",         25,  0, 40, 70, 20,{ 18,  0},  5,  2,  0,  0, 20},
		{"Guardian-Rgt",         25,  0, 40, 40, 20,{ 20,  0},  7,  2,  0,  0, 20},
		{"Mega-Robo",            40,  0, 50,  0, 19,{ 19,  0}, 10,  0,  0,  0, 21},
		{"Ancient Evil",          0, 20, 60,100, 18,{  0, 20},  5,  0,  6,  2, 22},
		{"Lord Ahn",             40, 20, 60,100, 35,{ 20, 20}, 10,  3,  5,  3, 23},
		{"Frost Dragon",         15,  7, 20, 50, 18,{ 20, 15},  9,  2,  4,  0, 24},
		{"ArchiDraconian",       30, 20, 30, 60, 18,{ 18, 18},  7,  2,  5,  1, 25},
		{"Panzer Viper",         35,  0, 40, 80, 20,{ 20,  0},  9,  1,  0,  0, 26},
		{"Black Knight",         35,  0, 35, 50, 20,{ 20,  0},  6,  3,  0,  1, 27},
		{"ArchiMonk",            20,  0, 50, 70, 20,{ 20,  0},  5,  0,  0,  0, 28},
		{"ArchiMage",            10, 19, 30, 70, 10,{ 10, 19},  4,  0,  6,  0, 29},
		{"Neo-Necromancer",      40, 20, 60,100, 30,{ 20, 20}, 10,  3,  6,  3, 30}
	};

	CT_ASSERT(sizeof(s_enemyData[0]) == 32, TEnemyData_size_is_changed);

	if (s_isFirst)
	{
		// 실행 디렉토리에 foedata0.dat 파일이 있는 경우는 그 파일을 이용하고,
		// 그렇지 않을 경우에는 빌드시 미리 지정된 디폴트 데이터를 사용한다.
		FILE* fp = fopen("./foedata0.dat", "rb");
		if (fp)
		{
			for (int i = 0; i < sizeof(s_enemyData) / sizeof(s_enemyData[0]); i++)
			{
				int read = fread(&s_enemyData[i], sizeof(TEnemyData), 1, fp);
				assert(read == 1);
				if (read != 1)
					break;

				// pascal형 string을 C형 string으로 변환
				{
					unsigned char strLength = (unsigned char)s_enemyData[i].name[0];
					memcpy(&s_enemyData[i].name[0], &s_enemyData[i].name[1], strLength);
					s_enemyData[i].name[strLength] = 0;
				}
			}

			fclose(fp);

#if 0 // 데이터 저장에 사용된 code
			{
				FILE* fp = fopen("./foedata0.cpp", "wt");
				if (fp)
				{
					int nEnemy = sizeof(s_enemyData) / sizeof(s_enemyData[0]);

					fprintf(fp, "static TEnemyData s_enemyData[%d] =\n", nEnemy);
					fprintf(fp, "{\t// name                 str men end res agi accuracy   ac spe clv scl  lv\n");
	
					for (int i = 0; i < nEnemy; i++)
					{
						const TEnemyData& data = s_enemyData[i];
						fprintf(fp, "\t{");
						fprintf(fp, "\"%s\",", data.name);
						for (int j = 0; j < sizeof(data.name)-strlen(data.name); j++)
							fprintf(fp, " ");
						fprintf(fp, "%3d,", data.strength);
						fprintf(fp, "%3d,", data.mentality);
						fprintf(fp, "%3d,", data.endurance);
						fprintf(fp, "%3d,", data.resistance);
						fprintf(fp, "%3d,", data.agility);
						fprintf(fp, "{%3d,%3d},", data.accuracy[0], data.accuracy[1]);
						fprintf(fp, "%3d,", data.ac);
						fprintf(fp, "%3d,", data.special);
						fprintf(fp, "%3d,", data.castlevel);
						fprintf(fp, "%3d,", data.specialcastlevel);
						fprintf(fp, "%3d},\n", data.level);
					}

					fprintf(fp, "};\n");

					fclose(fp);
				}
			}
#endif
		}
		else
		{
			// 디폴트 데이터를 사용
		}

		s_isFirst = false;
	}

	// 1-base를 0-base로
	--index;

	if ((index < 0) || (index >= sizeof(s_enemyData) / sizeof(s_enemyData[0])))
		index = 0;

	return s_enemyData[index];
}

////////////////////////////////////////////////////////////////////////////////
// static function

// 이 함수는 경우에 따라 NULL을 돌려 줄 수 있으로 받는 쪽에서 NULL을 검사 해야 한다.
TPcPlayer* GetRandomPlayer(EPlayerStatus status)
{
	TPcPlayer* pPlayer = NULL;
	int nCount = 0;

	std::vector<TPcPlayer*>& player = game::object::GetPlayerList();

	for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if (!(*obj)->Valid())
			continue;

		if (status == EPLAYERSTATUS_CONSCIOUS)
			if (((*obj)->unconscious > 0) || ((*obj)->dead > 0) || ((*obj)->hp == 0))
				continue;

		if (status == EPLAYERSTATUS_NOT_DEAD)
			if ((*obj)->dead > 0)
				continue;

		++nCount;
	}

	if (nCount > 0)
	{
		int order = AvejUtil::Random(nCount);

		for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
		{
			if (!(*obj)->Valid())
				continue;

			if (status == EPLAYERSTATUS_CONSCIOUS)
				if (((*obj)->unconscious > 0) || ((*obj)->dead > 0) || ((*obj)->hp == 0))
					continue;

			if (status == EPLAYERSTATUS_NOT_DEAD)
				if ((*obj)->dead > 0)
					continue;

			if (order-- == 0)
			{
				pPlayer = *obj;
				break;
			}
		}
	}
	else
	{
		// 만약 해당되는 조건에 맞는 캐릭터가 없다면 임의로 추출한다.
		pPlayer = player[AvejUtil::Random(player.size())];

		if (!pPlayer->Valid())
			pPlayer = NULL;
	}

	return pPlayer;
}
