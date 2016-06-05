
#ifndef __URESSTRING_H__
#define __URESSTRING_H__

namespace resource
{

////////////////////////////////////////////////////////////////////////////////
// 게임의 기본 속성 문자열 (player, weapon 등)

enum ECondition
{
	CONDITION_GOOD        = 0,
	CONDITION_POISONED    = 1,
	CONDITION_UNCONSCIOUS = 2,
	CONDITION_DEAD        = 3,
};

enum EAux
{
	AUX_RESERVED          = 0,
	AUX_MAX
};

struct THanString
{
	const char* szName;
	const char* szJosaSub1;
	const char* szJosaSub2;
	const char* szJosaObj;
	const char* szJosaWith;

	THanString(void)
		: szName(""), szJosaSub1(""), szJosaSub2(""), szJosaObj(""), szJosaWith("")
	{
	}
};

THanString GetWeaponName(int index);
THanString GetShieldName(int index);
THanString GetArmorName(int index);

THanString Get3rdPersonName(int index);
THanString GetGenderName(int index);
THanString GetClassName(int index);
THanString GetConditionName(ECondition index);

THanString GetMagicName(int index);

THanString GetAuxName(EAux index);

////////////////////////////////////////////////////////////////////////////////
// 게임의 메시지 문자열

enum EMessage
{
	MESSAGE_NOT_ENOUGH_SP,
	MESSAGE_ANONYMOUS_NAME,
	MESSAGE_CANNOT_USE_ATTACK_MAGIC_NOT_IN_BATTLE_MODE,
	MESSAGE_TO_WHOM,
	MESSAGE_TO_EVERYBODY,
	MESSAGE_SELECTION,
	MESSAGE_CANNOT_USE_POWERFUL_CURE_SPELL,
	MESSAGE_SELECT_DIRECTION,
	MESSAGE_TELEPORT_TO_NORTH,
	MESSAGE_TELEPORT_TO_SOUTH,
	MESSAGE_TELEPORT_TO_EAST,
	MESSAGE_TELEPORT_TO_WEST,
	MESSAGE_MAX
};

const char* GetMessageString(EMessage message);

////////////////////////////////////////////////////////////////////////////////
// Resource 관리 utility

unsigned short GetLastMultiByte(const char* szText);
bool HasJongsung(unsigned short code);

}

#endif // #ifndef __URESSTRING_H__
