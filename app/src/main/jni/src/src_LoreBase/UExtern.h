
#ifndef __UEXTERN_H__
#define __UEXTERN_H__

#include <vector>
#include "UConsole.h"
#include "UMap.h"
#include "UPcParty.h"
#include "UPcEnemy.h"
#include "UPcPlayer.h"

namespace game
{
	// ¹«¼Ò¼Ó
	unsigned long GetRealColor(int index);
	void WarpPrevPos(void);
	void PressAnyKey(void);
	void WaitForAnyKey(void);
	void Wait(unsigned long msec);
	void UpdateScreen(void);
	bool LoadScript(const char* szText, int xStart = 0, int yStart = 0);
	void GameOver(void);

	namespace map
	{
		void Init(int width, int height);
		void SetType(TMap::TType _type);
		void SetHandicap(int handicap);
		void SetEncounter(int offset, int range);
		void SetStartPos(int x, int y);
		void Push(int row, unsigned char* pData, int nData);
		void Change(int x, int y, int tile);
		void SetLight(int x, int y);
		void ResetLight(int x, int y);
		bool IsValidWarpPos(int x, int y);
		bool LoadFromFile(const char* szFileName);
	}
	namespace console
	{
		enum TTextAlign
		{
			ALIGN_LEFT   = CLoreConsole::ALIGN_LEFT,
			ALIGN_CENTER = CLoreConsole::ALIGN_CENTER,
			ALIGN_RIGHT  = CLoreConsole::ALIGN_RIGHT
		};

		void WriteConsole(unsigned long index, int nArg, ...);
		void WriteLine(const char* szText, unsigned long color = 0xFFFFFFFF);
		void ShowMessage(unsigned long index, const char* s);
		void SetTextAlign(TTextAlign align);
	}
	namespace tile
	{
		void CopyToDefaultTile(int ixTile);
		void CopyToDefaultSprite(int ixSprite);
		void CopyTile(int srcTile, int dstTile);
	}
	namespace window
	{
		void DisplayMap(void);
		void DisplayConsole(void);
		void DisplayStatus(void);
		void DisplayBattle(int);
		void GetRegionForConsole(int* pX, int* pY, int* pW, int* pH);
	}
	namespace status
	{
		bool InBattle(void);
	}
	namespace object
	{
		std::vector<TPcPlayer*>& GetPlayerList(void);
		std::vector<TPcEnemy*>&  GetEnemyList(void);
		TPcParty& GetParty(void);
		TMap& GetMap(void);
	}
	namespace variable
	{
		void Set(int index, int value);
		void Add(int index);
		int  Get(int index);
	}
	namespace flag
	{
		void Set(int index);
		void Reset(int index);
		bool IsSet(int index);
	}
	namespace battle
	{
		void Init(void);
		void Start(bool bAssualt);
		void RegisterEnemy(int ixEnemy);
		void ShowEnemy(void);
		int  Result(void);
	}
	namespace party
	{
		enum ECheckIf
		{
			CHECKIF_MAGICTORCH,
			CHECKIF_LEVITATION,
			CHECKIF_WALKONWATER,
			CHECKIF_WALKONSWAMP,
			CHECKIF_MINDCONTROL
		};

		int  PosX(void);
		int  PosY(void);
		void PlusGold(int plus);
		void Move(int x1, int y1);
		bool CheckIf(ECheckIf check);
	}
	namespace player
	{
		int  GetNumOfConsciousPlayer(void);
		bool IsAvailable(int ixPlayer);
		const char* GetName(int ixPlayer);
		const char* GetGenderName(int ixPlayer);
		void AssignFromEnemyData(int ixPlayer, int ixEnemy);
		void ChangeAttribute(int ixPlayer, const char* szAttrib, int value);
		void ChangeAttribute(int ixPlayer, const char* szAttrib, const char* data);
		bool GetAttribute(int ixPlayer, const char* szAttrib, int& value);
		bool GetAttribute(int ixPlayer, const char* szAttrib, avej_lite::util::string& data);
		void ReviseAttribute(int ixPlayer);
		void ApplyAttribute(int ixPlayer);
	}
	namespace enemy
	{
		int  GetNumOfConsciousEnemy(void);
		void ChangeAttribute(int ixEnemy, const char* szAttrib, int value);
		void ChangeAttribute(int ixEnemy, const char* szAttrib, const char* data);
	}
	namespace select
	{
		void Init(void);
		void Add(const char* szString);
		void Run(void);
		int  Result(void);
	}
}

#endif // #ifndef __UEXTERN_H__
