
#ifndef __UMAP_H__
#define __UMAP_H__

#include "USmSet.h"
#include "USerialize.h"

class CGameMain;

struct TMap: public CSerialize
{
	enum
	{
		MAX_MAP_TILE = 56
	};

	typedef enum
	{
		ACT_BLOCK = 0,
		ACT_MOVE,
		ACT_EVENT,
		ACT_ENTER,
		ACT_SIGN,
		ACT_WATER,
		ACT_SWAMP,
		ACT_LAVA,
		ACT_TALK,
		ACT_MAX,
		ACT_DWORD = 0x7FFFFFFF
	} TAct;

	typedef void (CGameMain::*FnAct)(int x1, int y1, bool);

	typedef enum
	{
		TYPE_TOWN   = 0,
		TYPE_KEEP   = 1,
		TYPE_GROUND = 2,
		TYPE_DEN    = 3,
		TYPE_DWORD  = 0x7FFFFFFF
	} TType;

	typedef enum
	{
		HANDICAP_TILECHANGING = 0, // 지형 변화 
		HANDICAP_TELEPORT,         // 공간 이동
		HANDICAP_SEETHROUGH,       // 투시
		HANDICAP_CLAIRVOYANCE,     // 천리안
		HANDICAP_MAX
	} THandicap;

	// save; begin
	CSmSet jumpable;
	CSmSet teleportable;

	union
	{
		struct
		{
			TType  type;
			long   width, height;
			long   encounterOffset, encounterRange;
			long   tile4way;
			unsigned char data[256][256];
			unsigned char handicapData[HANDICAP_MAX];
		};

		char save[(256*256)*1 + sizeof(long)*10];
	};
	// save; end

	FnAct actFunc[ACT_MAX];
	FnAct actList[MAX_MAP_TILE];

	inline unsigned char operator()(int x, int y) const	{ return data[y][x] & 0x3F; }

	void ClearData(void);

	void ChangeMap(int x, int y, unsigned char tile);
	void ChangeToWay(int x, int y);
	void SetType(TType _type);

	int  GetEncounteredEnemy(void);

	bool IsJumpable(int x, int y);
	bool IsEventPos(int x, int y);
	bool IsTeleportable(int x, int y);

	void SetHandicap(THandicap handicap)
	{
		if ((handicap >= 0) && (handicap < HANDICAP_MAX))
			handicapData[handicap] = 1;
	}
	bool HasHandicap(THandicap handicap)
	{
		if ((handicap >= 0) && (handicap < HANDICAP_MAX))
			return (handicapData[handicap] > 0);
		else
			return false;
	}

	void SetLight(int x, int y);
	void ResetLight(int x, int y);
	bool HasLight(int x, int y) const;

protected:
	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;
};

#endif // #ifndef __UMAP_H__
