
#include "pd_config.h"
#include "pd_actor_albireo.h"
#include "pd_base_world.h"
#include "pd_base_presentation.h"
#include "pd_class_player.h"

#include "avej_lite.h"

#include <assert.h>

// World의 현상을 Albireo의 시각으로 해석하기 위한 부분
//?? 이건 view 는 아닌가 생각 해 보자.
namespace
{
	const unsigned long COLOR_BLACK      = 0xFF000000;
	const unsigned long COLOR_WHITE      = 0xFFFFFFFF;

	const unsigned long COLOR_IN_DREAM_1 = 0xFF00D827;
	const unsigned long COLOR_IN_DREAM_2 = 0xFFF08040;
	const unsigned long COLOR_FOR_SHADOW = COLOR_BLACK;

	static int s_is_first = true;

	static unsigned long s_approach_table[APPROACH_STEP_MAX+1];

	void s_MakeNecessaryTables4Albireo()
	{
		assert(s_is_first);

		// color 변환 테이블 만들기
		{
			// unsigned 로 하면 for-loop에서 underflow가 발생한다.
			long r1 = (COLOR_IN_DREAM_1 & 0x00FF0000) >> 16;
			long g1 = (COLOR_IN_DREAM_1 & 0x0000FF00) >>  8;
			long b1 = (COLOR_IN_DREAM_1 & 0x000000FF) >>  0;

			long r2 = (COLOR_IN_DREAM_2 & 0x00FF0000) >> 16;
			long g2 = (COLOR_IN_DREAM_2 & 0x0000FF00) >>  8;
			long b2 = (COLOR_IN_DREAM_2 & 0x000000FF) >>  0;

			for (int step = 0; step <= APPROACH_STEP_MAX; step++)
			{
				unsigned long r = r1 + step*(r2-r1)/APPROACH_STEP_MAX;
				unsigned long g = g1 + step*(g2-g1)/APPROACH_STEP_MAX;
				unsigned long b = b1 + step*(b2-b1)/APPROACH_STEP_MAX;

				s_approach_table[step] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
		}

		s_is_first = false;
	}

	unsigned long s_GetCurrentDreamColor(void)
	{
		if (CONFIG_TYPE == CONFIG_TYPE_DREAM)
		{
			int step = TWorld::GetApproachStep();
			return s_approach_table[step];
		}
		else
		{
			return 0xFFFFFFFF;
		}
	}
}

struct TAlbireoData
{
	TPosition      beholding_postion;
	TWorld&        ref_world;
	TPresentation& ref_presentation;

	iu::shared_ptr<CPlayer> beholding_chara;

	TAlbireoData()
	:	beholding_postion(0, 0),
		ref_world(avej_lite::singleton<TWorld>::get()),
		ref_presentation(avej_lite::singleton<TPresentation>::get()),
		beholding_chara(TWorld::GetMainChara()) {}
};

static TAlbireoData* s_p_albireo_data = 0;

CAlbireo::CAlbireo()
{
	assert(s_p_albireo_data == 0);

	s_p_albireo_data = new TAlbireoData;

	s_p_albireo_data->ref_presentation << CONFIG_NAME;

	// world에 대한 갱신
	s_p_albireo_data->ref_presentation << s_p_albireo_data->ref_world;

	s_MakeNecessaryTables4Albireo();
}

CAlbireo::~CAlbireo()
{
	delete s_p_albireo_data;
	s_p_albireo_data = 0;
}

TPosition CAlbireo::GetBeholdingPosition(void)
{
	assert(s_p_albireo_data);

	return s_p_albireo_data->beholding_postion;
}

unsigned long CAlbireo::GetDreamColor(TDreamColorAlias alias)
{
	switch (alias)
	{
	case DREAM_COLOR_BLACK:
		return COLOR_BLACK;
	case DREAM_COLOR_WHITE:
		return COLOR_WHITE;
	case DREAM_COLOR_NORMAL:
		return COLOR_IN_DREAM_1;
	case DREAM_COLOR_APPROCH:
		return COLOR_IN_DREAM_2;
	case DREAM_COLOR_SHADOW:
		return COLOR_FOR_SHADOW;
	default:
		assert(false);
		return 0;
	}
}

unsigned long CAlbireo::GetCurrentDreamColor(void)
{
	return s_GetCurrentDreamColor();
}

void CAlbireo::_Act(void* p_param) const
{
	//?? config 부분?
	const  int  S_MAX_STEP = 10;
	static bool s_is_sight_moving = false;
	static int  s_step = 0; // (0 ~ S_MAX_STEP)

	assert(s_p_albireo_data);

	// world에 있는 player list를 얻어 와서, main chara의 위치를 변경한다.
	{
		iu::shared_ptr<class CPlayer> current_noticed_chara = TWorld::GetNoticedChara();

		if (current_noticed_chara.get())
		{
			if (!s_is_sight_moving && (s_p_albireo_data->beholding_chara == current_noticed_chara))
			{
				// 시선의 변화 없음
				s_p_albireo_data->beholding_postion = s_p_albireo_data->beholding_chara->GetDesc().position;
			}
			else
			{
				// 시선 이동
				if (!s_is_sight_moving)
				{
					s_step = 0;
					s_is_sight_moving = true;
				}

				++s_step;

				const TPosition& src_pos = s_p_albireo_data->beholding_chara->GetDesc().position;
				const TPosition& dst_pos = current_noticed_chara->GetDesc().position;

				s_p_albireo_data->beholding_postion.x = src_pos.x + (dst_pos.x - src_pos.x) * s_step / S_MAX_STEP;
				s_p_albireo_data->beholding_postion.y = src_pos.y + (dst_pos.y - src_pos.y) * s_step / S_MAX_STEP;

				if (s_step >= S_MAX_STEP)
				{
					// 시선 이동이 모두 끝남
					s_p_albireo_data->beholding_chara = current_noticed_chara;
					s_is_sight_moving = false;
				}
			}
		}
	}

	static bool s_is_first = true;

	if (!s_is_first)
	{
		// presentation 실행
		static unsigned int s_ref_count = 0;

		action::run<unsigned int> run_command(s_ref_count);
		s_p_albireo_data->ref_presentation << run_command;
	}
	else
	{
		// window manager 실행
		action::get<TCommunication::TCommunicationData> get;
		action::run<TCommunication::TCommunicationData> communication_command(get.data);
		s_p_albireo_data->ref_presentation << communication_command;

		s_is_first = false;
	}

	if (!s_is_sight_moving)
	{
		// Albireo만 communication을 열람하므로 peek가 아닌 get을 바로 해도 된다.
		action::get<TCommunication::TCommunicationData> get;

		TCommunication& communication = avej_lite::singleton<TCommunication>::get();

		communication >> get;

		if (get.data.try_to != TCommunication::TRY_TO_NONE)
		{
			action::run<TCommunication::TCommunicationData> communication_command(get.data);
			s_p_albireo_data->ref_presentation << communication_command;

			//?? 임시.. 원래 캐릭터로 복귀
			action::set<TTryToCommunicate> set(TTryToCommunicate(NULL, NULL));
			avej_lite::singleton<TWorld>::get() << set;
		}
	}

}
