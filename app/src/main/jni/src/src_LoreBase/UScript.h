
#ifndef __USCRIPT_H__
#define __USCRIPT_H__

#include "USmScript.h"

class CScript
{
	int m_mode;
	int m_position;
	int m_xPos;
	int m_yPos;

public:
	enum
	{
		MODE_MAP   = 0,
		MODE_TALK  = 1,
		MODE_SIGN  = 2,
		MODE_EVENT = 3,
		MODE_ENTER = 4,
	};

	CScript(int mode, int position, int x = 0, int y = 0);

	void NativeEqual(TSmParam* pParam);
	void NativeLess(TSmParam* pParam);
	void NativeNot(TSmParam* pParam);
	void NativeOr(TSmParam* pParam);
	void NativeAnd(TSmParam* pParam);
	void NativeRandom(TSmParam* pParam);
	void NativeAdd(TSmParam* pParam);

	void NativePushString(TSmParam* pParam);
	void NativePopString(TSmParam* pParam);

	void NativeDisplayMap(TSmParam* pParam);
	void NativeDisplayStatus(TSmParam* pParam);
	void NativeScriptMode(TSmParam* pParam);
	void NativeOn(TSmParam* pParam);
	void NativeOnArea(TSmParam* pParam);
	void NativeTalk(TSmParam* pParam);
	void NativeTextAlign(TSmParam* pParam);
	void NativeWarpPrevPos(TSmParam* pParam);
	void NativePressAnyKey(TSmParam* pParam);
	void NativeWait(TSmParam* pParam);
	void NativeLoadScript(TSmParam* pParam);
	void NativeGameOver(TSmParam* pParam);

	// map
	void NativeMapInit(TSmParam* pParam);
	void NativeMapSetType(TSmParam* pParam);
	void NativeMapSetHandicap(TSmParam* pParam);
	void NativeMapSetEncounter(TSmParam* pParam);
	void NativeMapSetStartPos(TSmParam* pParam);
	void NativeMapSetTile(TSmParam* pParam);
	void NativeMapSetRow(TSmParam* pParam);
	void NativeMapChangeTile(TSmParam* pParam);
	void NativeMapSetLightArea(TSmParam* pParam);
	void NativeMapResetLightArea(TSmParam* pParam);
	void NativeMapLoadFromFile(TSmParam* pParam);

	// tile
	void NativeTileCopyToDefaultTile(TSmParam* pParam);
	void NativeTileCopyToDefaultSprite(TSmParam* pParam);
	void NativeTileCopyTile(TSmParam* pParam);
	
	// flag / variable
	void NativeFlagSet(TSmParam* pParam);
	void NativeFlagReset(TSmParam* pParam);
	void NativeFlagIsSet(TSmParam* pParam);
	void NativeVariableSet(TSmParam* pParam);
	void NativeVariableGet(TSmParam* pParam);
	void NativeVariableAdd(TSmParam* pParam);

	// battle
	void NativeBattleInit(TSmParam* pParam);
	void NativeBattleStart(TSmParam* pParam);
	void NativeBattleRegisterEnemy(TSmParam* pParam);
	void NativeBattleShowEnemy(TSmParam* pParam);
	void NativeBattleResult(TSmParam* pParam);

	// select
	void NativeSelectInit(TSmParam* pParam);
	void NativeSelectAdd(TSmParam* pParam);
	void NativeSelectRun(TSmParam* pParam);
	void NativeSelectResult(TSmParam* pParam);

	// party
	void NativePartyPosX(TSmParam* pParam);
	void NativePartyPosY(TSmParam* pParam);
	void NativePartyPlusGold(TSmParam* pParam);
	void NativePartyMove(TSmParam* pParam);
	void NativeCheckIf(TSmParam* pParam);

	// player
	void NativePlayerIsAvailable(TSmParam* pParam);
	void NativePlayerGetName(TSmParam* pParam);
	void NativePlayerGetGenderName(TSmParam* pParam);
	void NativePlayerAssignFromEnemyData(TSmParam* pParam);
	void NativePlayerChangeAttribute(TSmParam* pParam);
	void NativePlayerGetAttribute(TSmParam* pParam);
	void NativePlayerReviseAttribute(TSmParam* pParam);
	void NativePlayerApplyAttribute(TSmParam* pParam);

	// enemy
	void NativeEnemyChangeAttribute(TSmParam* pParam);

	static void RegisterScriptFileName(const char* szFileName);
	static const char* GetScriptFileName(void);
};

#endif // __USCRIPT_H__
