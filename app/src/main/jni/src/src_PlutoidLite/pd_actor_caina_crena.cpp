
#include "pd_actor_caina_crena.h"
#include "pd_actor_oracle.h"
#include "pd_base_world.h"
#include "pd_class_player.h"

//?? �ӽ�
//#include "device/avej_device_input.h"

#include <assert.h>
#include <algorithm>

struct TCainaCrenaData
{
	TWorld&  ref_world;
	COracle& ref_oracle;
	// simple map�� ownership�� Caina Crena�� ������ world�� reference�� ������.
	CSimpleTileMap tile_map;

	// world�� ������ �ڿ� oracle�� �����.
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

	// tile map�� world�� ����
	//?? �� �� �Ϲ�ȭ�� tile map desc�� ��� ����...
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

	// world�� �ڿ� ������ �ؼ��Ѵ�. �̶� caina crena���Ը� ������ ������� world�� ���� �Ѵ�.
	{
		extern void TWorld_AnalyzePhenomenon(void);
		TWorld_AnalyzePhenomenon();
	}

	// ���� oracle�� �ൿ�Ѵ�.
	{
		s_p_caina_crena_data->ref_oracle.Act();
	}

	// Actor�� ���� Act()
	{
		action::get<const TActorList*> get_actor_list;
		s_p_caina_crena_data->ref_world >> get_actor_list;

		const TActorList* p_actor_list = get_actor_list.data;

		if (p_actor_list)
		{
			std::for_each(p_actor_list->begin(), p_actor_list->end(), CActor::SharedActorAct);
		}
	}

	// Player�� ���� Act()
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
