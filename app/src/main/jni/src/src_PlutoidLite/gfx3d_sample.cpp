
/*
	TODO list

	< �ֿ켱 >
	- �켱 �޴� window ����
	- ��ȭ�� � ����?

	< ���� >
	- configurable�� ��� ����� config ���Ϸ� ����
	- presentation�� class �и�
	- Dialog�� ���� window system�� �ʿ��ұ�?
	- GIF ���ڴ� �ڵ� ����

	< ��� >
	- A ��� ����� (���� ����Ʈ ������ ������ ������ ����)
	- �׸��� �ֱ�
	- �ܺ� text map �б�
	- ��ȭ ���
		- ��ȭâ �ʿ�
		- ��ũ��Ʈ�� ���� ��ȭ �ʿ�
	- audio
		- SDL�� ����Ͽ� Audio ���
		- item�� ��쿡�� ���� �ƴ� �Ҹ��� �����ȴ�.
	- player�� name �Ӽ� -> id -> ��ȭ�� id�� ���(������� ���忡���� string����)

	< ���� >
	- ��� ����ȭ
	- x2�� scaling ���� ���� �� �� ����.
	- ĳ���͵� �� ���� begin() - end() �� ����� �����ϰ�
		���� ��糢�� ��� ���
	- line of sight�� ���� �巯�� �� noise�� �ִ� ����?
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
		// �������� �����̹Ƿ� ���� ���� ����ȴ�.
		avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();
		input_device.UpdateInputState();

		// �Ϲ����� ���踦 �����̴� over mind�� ��
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
	// �������� �����̹Ƿ� ���� ���� ����ȴ�.
	avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();
	input_device.UpdateInputState();

	// �Ϲ����� ���踦 �����̴� over mind�� ��
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