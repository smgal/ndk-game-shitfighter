
#include "pd_config.h"
#include "pd_actor_oracle.h"
#include "pd_base_premotion.h"
#include "pd_base_world.h"
#include "pd_class_player.h"
#include <assert.h>

struct TOracleData
{
	TPremotion& ref_premotion;
	TWorld&     ref_world;
	iu::shared_ptr<CPlayer> ref_main_chara;

	TOracleData()
	:	ref_premotion(avej_lite::singleton<TPremotion>::get()),
		ref_world(avej_lite::singleton<TWorld>::get())
	{
	}
};

static TOracleData* s_p_oracle_data = 0;

COracle::COracle()
{
	assert(s_p_oracle_data == 0);

	s_p_oracle_data = new TOracleData;

	// main character를 찾는다. 만약 여기에 문제가 있다면 world보다 oracle이 먼저 생기려했을 때다.
	{
		action::get<const TPlayerList*> get_player_list;
		s_p_oracle_data->ref_world >> get_player_list;

		const TPlayerList* p_player_list = get_player_list.data;

		if (p_player_list)
		{
			if (!(*p_player_list).empty())
			{
				s_p_oracle_data->ref_main_chara = *(p_player_list->begin());
			}
		}
	}
}

COracle::~COracle()
{
	delete s_p_oracle_data;
	s_p_oracle_data = 0;
}

void COracle::_Act(void* p_param) const
{
	assert(s_p_oracle_data);

	action::get<TPremotion::TPremotionData> premotion_data;

	// hold state와 run state는 move 보다 먼저 이벤트를 알려 준다.
	bool is_holded  = false;
	bool is_running = false;

	bool can_main_chara_get_a_message = (TWorld::GetNoticedChara() == TWorld::GetMainChara());

	do
	{
		s_p_oracle_data->ref_premotion >> premotion_data;

		switch (premotion_data.data.message)
		{
		case TPremotion::MESSAGE_NONE:
			break;
		case TPremotion::MESSAGE_HOLD:
			is_holded = true;
			break;
		case TPremotion::MESSAGE_RUN:
			is_running = true;
			break;
		case TPremotion::MESSAGE_MOVE:
			if (can_main_chara_get_a_message && s_p_oracle_data->ref_main_chara.get())
			{
				const int STEP = (is_running) ? PLAYER_MOVE_RUNNING : PLAYER_MOVE_WALKING;

				const TPlayerDesc& player_desc = s_p_oracle_data->ref_main_chara->GetDesc();

				TPosition position(player_desc.position.x + STEP * premotion_data.data.param[0], player_desc.position.y + STEP * premotion_data.data.param[1]);

				position.x = (position.x / STEP) * STEP;
				position.y = (position.y / STEP) * STEP;

				*s_p_oracle_data->ref_main_chara << TDirectionHolded(is_holded) << position;
			}
			break;
		case TPremotion::MESSAGE_ACTION:
			if (can_main_chara_get_a_message)
			{
				*s_p_oracle_data->ref_main_chara << action::act<int>();
			}
			break;
		default:
			break;
		}

	} while (premotion_data.data.message != TPremotion::MESSAGE_NONE);
}
