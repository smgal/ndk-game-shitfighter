
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
 * \brief ���ӿ� ���Ǵ� player ĳ������ class
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

	//! �Ӽ� ���� kind�� �´� ����Ʈ ���� ä��
	void SetDefault(int kind);
	//! ������ ���¸� �����ϰ� �̻��� �ִ� ���� ����
	void CheckCondition(void);

	//! ������ ���¸� ��Ÿ���� ���ڿ��� ������
	const char* GetConditionString(void) const;
	//! ������ ���¸� 32-bit �÷� ������ ������
	unsigned long GetConditionColor(void) const;
	//! 3��Ī ���縦 ������
	const char* Get3rdPersonName(void) const;
	//! ������ ��Ÿ���� ���ڿ��� ������
	const char* GetGenderName(void) const;
	//! class�� ��Ÿ���� ���ڿ��� ������
	const char* GetClassName(void) const;
	//! ������ ���⸦ ��Ÿ���� ���ڿ��� ������
	const char* GetWeaponName(void) const;
	//! ������ ���и� ��Ÿ���� ���ڿ��� ������
	const char* GetShieldName(void) const;
	//! ������ ������ ��Ÿ���� ���ڿ��� ������
	const char* GetArmorName(void) const;

	//! ���ڿ��� ���� ������ �Ӽ��� ����
	bool ChangeAttribute(const char* attribute, int value);
	bool ChangeAttribute(const char* attribute, const char* data);
	bool GetAttribute(const char* attribute, int& value);
	bool GetAttribute(const char* attribute, avej_lite::util::string& data);

	//! ���յǴ� �Ӽ�(hp��)�� ������ �� ������ �ִ����� Ȯ���ϰ�, ������ ������ �����Ѵ�.
	void ReviseAttribute(void);
	//! ������ �Ӽ����� �´� hp������ �����Ѵ�.
	void ApplyAttribute(void);

	//! �ǽ��� �ִ� �������� �ƴ����� �Ǻ�
	bool IsConscious(void) const
	{
		return Valid() && (unconscious == 0) && (dead == 0) && (hp > 0);
	}

	//! �� �����ͷκ��� player�� ����
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

	//! ���� ���� ���
	void CastAttackSpell(void);
	//! ġ�� ���� ���
	void CastCureSpell(void);
	//! ��ȭ ���� ���
	void CastPhenominaSpell(void);

	//! ������ �̿��� ����
	void AttackWithWeapon(int ixObject, int ixEnemy);
	//! �� ��ü�� ���� ���� ����
	void CastSpellToAll(int ixObject);
	//! �� �ϳ��� ���� ���� ����
	void CastSpellToOne(int ixObject, int ixEnemy);
	//! ���� esp ���
	void UseESP(void);
	//! ������ esp ���
	void UseESPForBattle(int ixObject, int ixEnemy);
	//! Ư�� ���� ������ ���
	void CastSpellWithSpecialAbility(int ixObject, int ixEnemy);
	//! ������ �õ�
	bool TryToRunAway(void);

private:
	//! �� ���� ġ��
	void m_HealOne(TPcPlayer* pTarget);
	//! �� ���� �ص�
	void m_AntidoteOne(TPcPlayer* pTarget);
	//! �� ���� �ǽ��� ����
	void m_RecoverConsciousnessOne(TPcPlayer* pTarget);
	//! �� ���� ��Ȱ ��Ŵ
	void m_RevitalizeOne(TPcPlayer* pTarget);
	//! ��θ� ġ��
	void m_HealAll(void);
	//! ��θ� �ص�
	void m_AntidoteAll(void);
	//! ��θ� �ǽ��� ����
	void m_RecoverConsciousnessAll(void);
	//! ��θ� ��Ȱ ��Ŵ
	void m_RevitalizeAll(void);
	//! Ư�� ������ ��� �������� Ȯ��
	bool m_CanUseSpecialMagic(void);
	//! ESP ��� �������� Ȯ��
	bool m_CanUseESP(void);
	//! ����ģ ���� ���� ����ġ�� �߰�
	void m_PlusExperience(TPcEnemy* pEnemy);
	//! Spell point�� �����ϴٴ� �޽��� ���
	void m_PrintSpNotEnough(void);
	//! ESP point�� �����ϴٴ� �޽��� ���
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
