
#include "pd_actor_the_absolute.h"
#include "pd_base_premotion.h"
#include "avej_lite.h"
#include <assert.h>

struct TTheAbsoluteData
{
	CTheAbsolute::TState state;

	TTheAbsoluteData()
		: state(CTheAbsolute::STATE_BEGING) {}
};

static TTheAbsoluteData* s_p_the_absolute_data = 0;

CTheAbsolute::CTheAbsolute()
{
	assert(s_p_the_absolute_data == 0);

	s_p_the_absolute_data = new TTheAbsoluteData;
}

CTheAbsolute::~CTheAbsolute()
{
	delete s_p_the_absolute_data;
	s_p_the_absolute_data = 0;
}

void CTheAbsolute::_Act(void* p_param) const
{
	avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();

	// 일반키 설정
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_R1))
	{
		action::set<TPremotion::TPremotionData> premotion_data(TPremotion::MESSAGE_HOLD);
		avej_lite::singleton<TPremotion>::get() << premotion_data;
	}
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_B))
	{
		action::set<TPremotion::TPremotionData> premotion_data(TPremotion::MESSAGE_RUN);
		avej_lite::singleton<TPremotion>::get() << premotion_data;
	}
	if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_A))
	{
		action::set<TPremotion::TPremotionData> premotion_data(TPremotion::MESSAGE_ACTION);
		avej_lite::singleton<TPremotion>::get() << premotion_data;
	}
	if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_C))
	{
		action::set<TPremotion::TPremotionData> premotion_data(TPremotion::MESSAGE_LOCK);
		avej_lite::singleton<TPremotion>::get() << premotion_data;
	}

	// 특수키 설정
	if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1))
	{
		// 종료
		s_p_the_absolute_data->state = STATE_GONE;
	}

	// 이동키 설정
	{
		action::set<TPremotion::TPremotionData> premotion_data;

		if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_UP))
		{
			premotion_data.data.message   = TPremotion::MESSAGE_MOVE;
			premotion_data.data.param[0] += 0;
			premotion_data.data.param[1] -= 1;
		}
		if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_DOWN))
		{
			premotion_data.data.message   = TPremotion::MESSAGE_MOVE;
			premotion_data.data.param[0] += 0;
			premotion_data.data.param[1] += 1;
		}
		if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_LEFT))
		{
			premotion_data.data.message   = TPremotion::MESSAGE_MOVE;
			premotion_data.data.param[0] -= 1;
			premotion_data.data.param[1] += 0;
		}
		if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_RIGHT))
		{
			premotion_data.data.message   = TPremotion::MESSAGE_MOVE;
			premotion_data.data.param[0] += 1;
			premotion_data.data.param[1] += 0;
		}

		avej_lite::singleton<TPremotion>::get() << premotion_data;
	}
}

// static 
CTheAbsolute::TState CTheAbsolute::GetState(void)
{
	if (s_p_the_absolute_data)
		return s_p_the_absolute_data->state;
	else
		return CTheAbsolute::STATE_COMMING;
}
