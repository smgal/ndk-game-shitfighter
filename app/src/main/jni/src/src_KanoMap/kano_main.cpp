
////////////////////////////////////////////////////////////////////////////////
// uses

#include "avej_lite.h"

namespace kanomap
{

////////////////////////////////////////////////////////////////////////////////
// forward

////////////////////////////////////////////////////////////////////////////////
// macro definition

////////////////////////////////////////////////////////////////////////////////
// constant definition

////////////////////////////////////////////////////////////////////////////////
// type definition

////////////////////////////////////////////////////////////////////////////////
// global variables

////////////////////////////////////////////////////////////////////////////////
// static variables

////////////////////////////////////////////////////////////////////////////////
// forward definition

////////////////////////////////////////////////////////////////////////////////
// class definition

////////////////////////////////////////////////////////////////////////////////
// main

class CGameMain
{
	int m_tick_count;

public:
	CGameMain(void)
		: m_tick_count(0)
	{
	}
	~CGameMain(void)
	{
	}
	bool Process(void)
	{
		return true;
	}
};

static CGameMain* s_p_game_main = 0;

static bool OnCreate(void)
{
	s_p_game_main  = new CGameMain;

	return true;
}

static bool OnDestory(void)
{
	delete s_p_game_main;

	return true;
}

static bool OnProcess(void)
{
	return s_p_game_main->Process();
}

} // namespace kanomap

////////////////////////////////////////////////////////////////////////////////
// main

#include "avejapp_register.h"

static avej_lite::IAvejApp* p_app = NULL;

static bool s_Initialize(void* h_window)
{
	avej_lite::AppCallback callBack =
	{
		kanomap::OnCreate,
		kanomap::OnDestory,
		kanomap::OnProcess
	};

	p_app = avej_lite::IAvejApp::GetInstance(callBack);

	return (p_app != 0);
}

static void s_Finalize(void)
{
	p_app->Release();
	p_app = NULL;
}

static bool s_Process(void)
{
	return (p_app) ? p_app->Process() : false;
}

static bool GetAppCallback(TAppCallback& out_callback)
{
	out_callback.Initialize = s_Initialize;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

REGISTER_APP_1("KanoMap", GetAppCallback);
