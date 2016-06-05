
#include "pd_actor_caina_crena.h"
#include "pd_actor_oracle.h"
#include "pd_base_world.h"
#include "pd_class_player.h"

//?? 임시
//#include "device/avej_device_input.h"

#include <assert.h>
#include <algorithm>

struct TCainaCrenaData
{
	TWorld&  ref_world;
	COracle& ref_oracle;
	// simple map의 ownership은 Caina Crena만 가지며 world는 reference만 가진다.
	CSimpleTileMap tile_map;

	// world가 생성된 뒤에 oracle이 생긴다.
	TCainaCrenaData()
	: ref_world(avej_lite::singleton<TWorld>::get()), ref_oracle(avej_lite::singleton<COracle>::get())
	{
	}
};

static TCainaCrenaData* s_p_caina_crena_data = 0;

CCainaCrena::CCainaCrena()
{
	assert(s_p_caina_crena_data == 0);

	s_p_caina_crena_data = new TCainaCrenaData;

	// tile map을 world에 대입
	//?? 좀 더 일반화된 tile map desc가 들어 가야...
	action::set<CSimpleTileMap*> tile_map(&s_p_caina_crena_data->tile_map);
	s_p_caina_crena_data->ref_world << tile_map;
}

CCainaCrena::~CCainaCrena()
{
	delete s_p_caina_crena_data;
	s_p_caina_crena_data = 0;
}

void CCainaCrena::_Act(void* p_param) const
{
	assert(s_p_caina_crena_data);

	// world의 자연 현상을 해석한다. 이때 caina crena에게만 공개된 방법으로 world에 접근 한다.
	{
		extern void TWorld_AnalyzePhenomenon(void);
		TWorld_AnalyzePhenomenon();
	}

	// 먼저 oracle이 행동한다.
	{
		s_p_caina_crena_data->ref_oracle.Act();
	}

	// Actor에 대한 Act()
	{
		action::get<const TActorList*> get_actor_list;
		s_p_caina_crena_data->ref_world >> get_actor_list;

		const TActorList* p_actor_list = get_actor_list.data;

		if (p_actor_list)
		{
			std::for_each(p_actor_list->begin(), p_actor_list->end(), CActor::SharedActorAct);
		}
	}

	// Player에 대한 Act()
	{
		action::get<const TPlayerList*> get_player_list;
		s_p_caina_crena_data->ref_world >> get_player_list;

		const TPlayerList* p_player_list = get_player_list.data;

		if (p_player_list)
		{
			std::for_each(p_player_list->begin(), p_player_list->end(), CPlayer::SharedPlayerAct);
		}
	}
}
