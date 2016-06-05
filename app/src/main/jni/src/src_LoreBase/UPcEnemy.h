
#ifndef __UPCENEMY_H__
#define __UPCENEMY_H__

#pragma warning(disable: 4786)

#include "UPcNameBase.h"
#include <string>
#include <map>

struct TPcPlayer;

#pragma pack(push, 1)
struct TEnemyData
{
	char name[20];
	unsigned char strength;
	unsigned char mentality;
	unsigned char endurance;
	unsigned char resistance;
	unsigned char agility;
	unsigned char accuracy[2];
	unsigned char ac;
	unsigned char special;
	unsigned char castlevel;
	unsigned char specialcastlevel;
	unsigned char level;
};
#pragma pack(pop)

struct TPcEnemy: public CPcNameBase
{
	int  strength;
	int  mentality;
	int  endurance;
	int  resistance;
	int  agility;
	int  accuracy[2];
	int  ac;
	int  special;
	int  castlevel;
	int  specialcastlevel;
	int  level;

	int  hp;
	int  poison;
	int  unconscious;
	int  dead;

	int  E_number;

	TPcEnemy(void);
	~TPcEnemy(void);

	void operator<<(const TEnemyData& data);
	void operator<<(const TPcPlayer& player);

	bool IsConscious(void) const
	{
		return Valid() && (unconscious == 0) && (dead == 0) && (hp > 0);
	}

	void Attack(void);

	//! 문자열을 통해 현재의 속성을 변경
	bool ChangeAttribute(const char* attribute, int value);
	bool ChangeAttribute(const char* attribute, const char* data);

private:
	/* 호출 순서도
	Attack -+-> m_AttackWithWeapon
	        |
	        +-> m_CastSpell -+-> m_CastAttackSpellToAll -,
	        |                |                           |
	        |                +-> m_CastAttackSpellToOne -+-> m_CastAttackSpellSub
	        |                |
	        |                `-> m_CastCureSpell
	        |
	        +-> m_AttackWithSpecialAbility
	        |
	        `-> m_CastSpellWithSpecialAbility
	*/
	void m_AttackWithWeapon(void);
	void m_CastSpell(void);
	void m_CastAttackSpellToAll(void);
	void m_CastAttackSpellToOne(TPcPlayer* pPlayer);
	void m_CastAttackSpellSub(TPcPlayer* pPlayer, int damage);
	void m_CastCureSpell(TPcEnemy* pTarget, int recovery);
	void m_AttackWithSpecialAbility(void);
	void m_CastSpellWithSpecialAbility(void);

	typedef std::map<std::string, int> TAttribMapInt;
	static TAttribMapInt s_m_attribMapIntList;

	typedef void (*TFnSet)(TPcEnemy&, const char*);
	typedef void (*TFnGet)(TPcEnemy&, avej_lite::util::string&);
	typedef std::pair<void*, void*> TFnSetGet;
	typedef std::map<std::string, TFnSetGet> TAttribMapStr;
	static TAttribMapStr s_m_attribMapStrList;

protected:
	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;
};

// 원본 게임에서 적 데이터를 뽑아 오는 함수 1~75사이가 유효 값
const TEnemyData& GetEnemyData(int index);

#endif // #ifndef __UPCENEMY_H__
