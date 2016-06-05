
#ifndef __PD_BASE_WORLD_H__
#define __PD_BASE_WORLD_H__

#include "pd_util.h"
#include "pd_actor.h"
#include "pd_class_simple_tile_map.h"
#include "avej_lite.h"

class CPlayer;

typedef iu::vector<iu::shared_ptr<CActor > >        TActorList;
typedef iu::vector<iu::shared_ptr<CPlayer> >        TPlayerList;

struct TTryToCommunicate
{
	CPlayer* p_from;
	CPlayer* p_to;

	TTryToCommunicate(CPlayer* _p_from, CPlayer* _p_to)
	:	p_from(_p_from), p_to(_p_to) {}
};

struct TWorld
{
	THIS_STRUCT_IS_A_SINGLETON(TWorld);

	TWorld& operator<<(action::set<CSimpleTileMap*> set);
	void    operator>>(action::get<CSimpleTileMap*>& get) const;

	TWorld& operator<<(action::set<TTryToCommunicate> set);

	void    operator>>(action::get<const TActorList* >& get) const;
	void    operator>>(action::get<const TPlayerList*>& get) const;

	static  bool CanIGoThere(TPosition origin, int x, int y, TExtent occupied_size);
	static  int  GetApproachStep(void);

	// 실제 주인공 캐릭터
	static  iu::shared_ptr<class CPlayer> GetMainChara(void);
	// 주인공이 초점을 두고 있는 캐릭터
	static  iu::shared_ptr<class CPlayer> GetFocusedChara(void);
	// Albireo가 주목해야할 캐릭터
	static  iu::shared_ptr<class CPlayer> GetNoticedChara(void);
};

#endif
