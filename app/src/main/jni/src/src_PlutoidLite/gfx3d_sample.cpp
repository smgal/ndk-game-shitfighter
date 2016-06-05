
/*
	TODO list

	< 최우선 >
	- 우선 메뉴 window 부터
	- 대화는 어떤 구조?

	< 구조 >
	- configurable한 모든 상수를 config 파일로 빼기
	- presentation의 class 분리
	- Dialog를 위해 window system이 필요할까?
	- GIF 디코더 코드 정리

	< 기능 >
	- A 모션 만들기 (현재 디폴트 사이즈 때문에 문제가 있음)
	- 그림자 넣기
	- 외부 text map 읽기
	- 대화 기능
		- 대화창 필요
		- 스크립트를 통한 대화 필요
	- audio
		- SDL을 사용하여 Audio 재생
		- item의 경우에는 색이 아닌 소리로 반응된다.
	- player에 name 속성 -> id -> 대화에 id로 사용(사용자의 입장에서는 string으로)

	< 개선 >
	- 출력 최적화
	- x2의 scaling 없이 가능 할 것 같다.
	- 캐릭터도 한 번의 begin() - end() 로 출력이 가능하게
		같은 모양끼리 모아 찍기
	- line of sight에 의해 드러날 때 noise를 넣는 것은?
*/

#include "pd_actor_the_absolute.h"
#include "pd_actor_caina_crena.h"
#include "pd_actor_albireo.h"

#include <vector>
#include <algorithm>

/*
#if defined(_MSC_VER) || (TARGET_DEVICE == TARGET_GP2XWIZ)
int AvejMain(void)
#else
int _main(void)
#endif
{
	CTheAbsolute& the_absolute = avej_lite::singleton<CTheAbsolute>::get();
	CCainaCrena&  caina_crena  = avej_lite::singleton<CCainaCrena>::get();
	CAlbireo&     albireo      = avej_lite::singleton<CAlbireo>::get();

	std::vector<CActor*> actor_overmind;

	actor_overmind.push_back(&the_absolute);
	actor_overmind.push_back(&caina_crena);
	actor_overmind.push_back(&albireo);

	while (1)
	{
		// 절대적인 행위이므로 가장 먼저 수행된다.
		avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();
		input_device.UpdateInputState();

		// 일반적인 세계를 움직이는 over mind의 턴
		std::for_each(actor_overmind.begin(), actor_overmind.end(), CActor::ActorAct);

		CTheAbsolute::TState state = CTheAbsolute::GetState();

		if (state == CTheAbsolute::STATE_GONE)
			break;
	}

	return 0;
}
*/

std::vector<CActor*> actor_overmind;

static void avej_init()
{
	CTheAbsolute& the_absolute = avej_lite::singleton<CTheAbsolute>::get();
	CCainaCrena&  caina_crena  = avej_lite::singleton<CCainaCrena>::get();
	CAlbireo&     albireo      = avej_lite::singleton<CAlbireo>::get();

	actor_overmind.push_back(&the_absolute);
	actor_overmind.push_back(&caina_crena);
	actor_overmind.push_back(&albireo);
}

static void avej_done()
{
}

static bool avej_process()
{
	// 절대적인 행위이므로 가장 먼저 수행된다.
	avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();
	input_device.UpdateInputState();

	// 일반적인 세계를 움직이는 over mind의 턴
	std::for_each(actor_overmind.begin(), actor_overmind.end(), CActor::ActorAct);

	CTheAbsolute::TState state = CTheAbsolute::GetState();

	return (state != CTheAbsolute::STATE_GONE);
}

#include "avejapp_register.h"

#include "pd_config.h"

static bool s_InitializeDream(void* h_window)
{
	SetConfig(CONFIG_TYPE_DREAM);

	avej_init();
	return true;
}

static bool s_InitializeKano(void* h_window)
{
	SetConfig(CONFIG_TYPE_KANO);
	
	avej_init();
	return true;
}

static void s_Finalize(void)
{
	avej_done();
}

static bool s_Process(void)
{
	return avej_process();
}

static bool GetAppCallbackDream(TAppCallback& out_callback)
{
	out_callback.Initialize = s_InitializeDream;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

static bool GetAppCallbackKano(TAppCallback& out_callback)
{
	out_callback.Initialize = s_InitializeKano;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

REGISTER_APP_1("AvejDream", GetAppCallbackDream);
REGISTER_APP_2("AvejKano",  GetAppCallbackKano);

/*
////////////////////////////////////////////////////////////////////////////////
// internal

namespace
{
	unsigned int _get_touch_region(int x, int y)
	{
	}
}

namespace target
{
	void setTouchRegionCallback(unsigned int (*fn_callback)(int x, int y));
}
target::setTouchRegionCallback(_get_touch_region);


*/