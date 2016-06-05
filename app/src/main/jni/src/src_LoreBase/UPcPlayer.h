
#ifndef __UPCPLAYER_H__
#define __UPCPLAYER_H__

#pragma warning(disable: 4786)

#include "UPcNameBase.h"
#include "USerialize.h"
#include <string>
#include <map>

struct TEnemyData;
struct TPcEnemy;

/*!
 * \file
 * \ingroup game main
 * \brief 게임에 사용되는 player 캐릭터의 class
 * \author SMgal smgal@hitel.net
 * \date 2006-12-31
*/
struct TPcPlayer: public CPcNameBase
{
	typedef enum
	{
		MALE,
		FEMALE,
		_DWORD = 0x7FFFFFFF
	} TGender;

	union
	{
		struct
		{
			int   order;
			TGender gender;
			int   class_;

			int   strength;
			int   mentality;
			int   concentration;
			int   endurance;
			int   resistance;
			int   agility;
			int   luck;

			int   ac;

			int   hp;
			int   sp;
			int   esp;
			int   experience;
			int   accuracy[3];
			int   level[3];

			int   poison;
			int   unconscious;
			int   dead;

			int   weapon;
			int   shield;
			int   armor;

			int   pow_of_weapon;
			int   pow_of_shield;
			int   pow_of_armor;
		};

		char save[30*sizeof(int)];
	};

public:
	TPcPlayer(void);
	~TPcPlayer(void);

	//! 속성 값을 kind에 맞는 디폴트 값로 채움
	void SetDefault(int kind);
	//! 현재의 상태를 점검하고 이상이 있는 경우는 보정
	void CheckCondition(void);

	//! 현재의 상태를 나타내는 문자열을 돌려줌
	const char* GetConditionString(void) const;
	//! 현재의 상태를 32-bit 컬러 값으로 돌려줌
	unsigned long GetConditionColor(void) const;
	//! 3인칭 대명사를 돌려줌
	const char* Get3rdPersonName(void) const;
	//! 성별을 나타내는 문자열을 돌려줌
	const char* GetGenderName(void) const;
	//! class를 나타내는 문자열을 돌려줌
	const char* GetClassName(void) const;
	//! 현재의 무기를 나타내는 문자열을 돌려줌
	const char* GetWeaponName(void) const;
	//! 현재의 방패를 나타내는 문자열을 돌려줌
	const char* GetShieldName(void) const;
	//! 현재의 갑옷을 나타내는 문자열을 돌려줌
	const char* GetArmorName(void) const;

	//! 문자열을 통해 현재의 속성을 변경
	bool ChangeAttribute(const char* attribute, int value);
	bool ChangeAttribute(const char* attribute, const char* data);
	bool GetAttribute(const char* attribute, int& value);
	bool GetAttribute(const char* attribute, avej_lite::util::string& data);

	//! 조합되는 속성(hp등)이 가능한 값 범위에 있는지를 확인하고, 문제가 있으면 보정한다.
	void ReviseAttribute(void);
	//! 수정한 속성값에 맞는 hp값으로 보정한다.
	void ApplyAttribute(void);

	//! 의식이 있는 상태인지 아닌지를 판별
	bool IsConscious(void) const
	{
		return Valid() && (unconscious == 0) && (dead == 0) && (hp > 0);
	}

	//! 적 데이터로부터 player를 생성
	void operator<<(const struct TEnemyData& data);

	/*
	BattleMode -+-> AttackWithWeapon -------,
				|                           |
				+-> CastSpellToAll          |
				|      |                    |
				+------+-> CastSpellToOne --+
				|                           |
				+-> UseESPForBattle --------+-> m_PlusExperience
				|
				+-> CastCureSpell -+-> m_HealAll
				|              |        |
				|              +--------+-> m_HealOne
				|              |
				|              +-> m_AntidoteAll
				|              |        |
				|              +--------+-> m_AntidoteOne
				|              |
				|              +-> m_RecoverConsciousnessAll
				|              |        |
				|              +--------+-> m_RecoverConsciousnessOne
				|              |
				|              +-> m_RevitalizeAll
				|              |        |
				|              `--------+-> m_RevitalizeOne
				|
				+-> CastSpellWithSpecialAbility
				|
				`-> TryToRunAway
	*/

	//! 공격 마법 사용
	void CastAttackSpell(void);
	//! 치료 마법 사용
	void CastCureSpell(void);
	//! 변화 마법 사용
	void CastPhenominaSpell(void);

	//! 무리를 이용한 공격
	void AttackWithWeapon(int ixObject, int ixEnemy);
	//! 적 전체에 대한 마법 공격
	void CastSpellToAll(int ixObject);
	//! 적 하나에 대한 마법 공격
	void CastSpellToOne(int ixObject, int ixEnemy);
	//! 평상용 esp 사용
	void UseESP(void);
	//! 전투용 esp 사용
	void UseESPForBattle(int ixObject, int ixEnemy);
	//! 특수 마법 공격을 사용
	void CastSpellWithSpecialAbility(int ixObject, int ixEnemy);
	//! 도망을 시도
	bool TryToRunAway(void);

private:
	//! 한 명을 치료
	void m_HealOne(TPcPlayer* pTarget);
	//! 한 명을 해독
	void m_AntidoteOne(TPcPlayer* pTarget);
	//! 한 명의 의식을 돌림
	void m_RecoverConsciousnessOne(TPcPlayer* pTarget);
	//! 한 명의 부활 시킴
	void m_RevitalizeOne(TPcPlayer* pTarget);
	//! 모두를 치료
	void m_HealAll(void);
	//! 모두를 해독
	void m_AntidoteAll(void);
	//! 모두를 의식을 돌림
	void m_RecoverConsciousnessAll(void);
	//! 모두를 부활 시킴
	void m_RevitalizeAll(void);
	//! 특수 마법의 사용 가능한지 확인
	bool m_CanUseSpecialMagic(void);
	//! ESP 사용 가능한지 확인
	bool m_CanUseESP(void);
	//! 물리친 적에 대해 경험치를 추가
	void m_PlusExperience(TPcEnemy* pEnemy);
	//! Spell point가 부족하다는 메시지 출력
	void m_PrintSpNotEnough(void);
	//! ESP point가 부족하다는 메시지 출력
	void m_PrintESPNotEnough(void);

	typedef std::map<std::string, int> TAttribMapInt;
	static TAttribMapInt s_m_attribMapIntList;

	typedef void (*TFnSet)(TPcPlayer&, const char*);
	typedef void (*TFnGet)(TPcPlayer&, avej_lite::util::string&);
	typedef std::pair<void*, void*> TFnSetGet;
	typedef std::map<std::string, TFnSetGet> TAttribMapStr;
	static TAttribMapStr s_m_attribMapStrList;

protected:
	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;
};

avej_lite::util::string GetBattleMessage(const TPcPlayer& who, int how, int what, const TPcEnemy& whom);

#endif // #ifndef __UPCPLAYER_H__
