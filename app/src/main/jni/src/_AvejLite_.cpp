
#include "_AvejLite_.h"
#include "_AvejLiteLauncher_.h"
#include <math.h>

using namespace Osp::Base;
using namespace Osp::Base::Runtime;
using namespace Osp::Graphics;
using namespace Osp::Locales;
using namespace Osp::System;
using namespace Osp::App;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

#define TIME_OUT  10
#define USE_LANDSCAPE_MODE

////////////////////////////////////////////////////////////////////////////////
// Form

class AvejLiteForm:
	public Osp::Ui::Controls::Form
{
public:
	AvejLiteForm(AvejLiteApp* pApp)
	: __pApp(pApp)
	{
	}
	virtual ~AvejLiteForm(void)
	{
	}

public:
	virtual result OnDraw(void)
	{
		if (__pApp)
		{
//			__pApp->Draw();
		}
		return E_SUCCESS;
	}

private:
	AvejLiteApp* __pApp;
};

////////////////////////////////////////////////////////////////////////////////
// constant

#define ACTION_ID_BUTTON_BASE  101
#define ACTION_ID_BUTTON_1     (ACTION_ID_BUTTON_BASE+0)
#define ACTION_ID_BUTTON_2     (ACTION_ID_BUTTON_BASE+1)
#define ACTION_ID_BUTTON_3     (ACTION_ID_BUTTON_BASE+2)
#define ACTION_ID_BUTTON_4     (ACTION_ID_BUTTON_BASE+3)
#define ACTION_ID_BUTTON_5     (ACTION_ID_BUTTON_BASE+4)
#define ACTION_ID_BUTTON_6     (ACTION_ID_BUTTON_BASE+5)
#define ACTION_ID_BUTTON_7     (ACTION_ID_BUTTON_BASE+6)
#define ACTION_ID_BUTTON_8     (ACTION_ID_BUTTON_BASE+7)
#define ACTION_ID_BUTTON_9     (ACTION_ID_BUTTON_BASE+8)
#define ACTION_ID_BUTTON_A     (ACTION_ID_BUTTON_BASE+9)

////////////////////////////////////////////////////////////////////////////////
// static variables

static bool s_app_working = false;

////////////////////////////////////////////////////////////////////////////////
// AvejLiteApp

AvejLiteApp::AvejLiteApp()
: __pTimer(null), __pForm(null)
{
}

AvejLiteApp::~AvejLiteApp()
{
}


Application*
AvejLiteApp::CreateInstance(void)
{
	return new AvejLiteApp();
}

#define CHECK_COND(cond) if ((cond) == 0) throw 1;
#define CHECK_RESULT(r)  if (IsFailed(r)) throw 2;

bool
AvejLiteApp::OnAppInitializing(AppRegistry& appRegistry)
{
	Timer* tempTimer = null;
	result r = E_SUCCESS;

	try
	{
		__pForm = new AvejLiteForm(this);
		CHECK_COND(__pForm);

		r = __pForm->Construct(FORM_STYLE_NORMAL);
		CHECK_RESULT(r);

		{
			__pPanel = new Panel;
			CHECK_COND(__pPanel);

			r = __pPanel->Construct(Rectangle(0, 0, 480, 480));
			CHECK_RESULT(r);

			__pForm->AddControl(*__pPanel);
		}

		{
			const char* app_list[10];

			avej_launcher::GetAppList(app_list, 10);

			const char** p_sz_app_list     = app_list;
			const char** p_sz_app_list_end = p_sz_app_list;

			AppLog("[SMGAL]-------------------------------");
			{
				int y         = 30;
				int action_id = ACTION_ID_BUTTON_1;

				const char** p_p_string = p_sz_app_list;
				while (*p_sz_app_list_end)
				{
					Button* pButton = new Button;
					pButton->Construct(Rectangle(50, y, 300, 60), *p_sz_app_list_end);
					y += 80;

					pButton->SetActionId(action_id++);
					pButton->AddActionEventListener(*this);

					__pPanel->AddControl(*pButton);

					AppLog("[SMGAL] [Registered app] %s", *p_sz_app_list_end);

					p_sz_app_list_end++;
				}
			}

			int num_of_app = p_sz_app_list_end - p_sz_app_list;

			AppLog("[SMGAL] num of app = %d", num_of_app);
			AppLog("[SMGAL]-------------------------------");

		}

		r = GetAppFrame()->GetFrame()->AddControl(*__pForm);
		CHECK_RESULT(r);

#if defined(USE_LANDSCAPE_MODE)
		__pForm->SetOrientation(ORIENTATION_LANDSCAPE);
#endif

		tempTimer = new Timer;
		CHECK_COND(tempTimer);

		r = tempTimer->Construct(*this);
		CHECK_RESULT(r);

		__pTimer  = tempTimer;

		return true;
	}
	catch (...)
	{
		delete __pForm;
		__pForm = null;

		delete tempTimer;
		avej_launcher::Finalize();

		AppLog("[SMGAL] Initialization fails");

		return false;
	}
}

bool
AvejLiteApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	s_app_working = false;
	return true;
}


void
AvejLiteApp::OnForeground(void)
{
	if (__pTimer)
	{
		if (s_app_working)
			__pTimer->Start(TIME_OUT);
	}
}


void
AvejLiteApp::OnBackground(void)
{
	if (__pTimer)
	{
		__pTimer->Cancel();
	}
}


void
AvejLiteApp::OnLowMemory(void)
{
}


void
AvejLiteApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}


void
AvejLiteApp::OnTimerExpired(Timer& timer)
{
	if (__pTimer)
	{
#if 1
		__pTimer->Start(TIME_OUT);

		if (!avej_launcher::Process())
			this->Terminate();
#else
		if (avej_launcher::Process())
		{
			__pTimer->Start(TIME_OUT);
		}
		else
		{
			avej_launcher::Finalize();
			s_app_working = false;
		}
#endif
	}
}

void
AvejLiteApp::OnScreenOn(void)
{
}

void
AvejLiteApp::OnScreenOff(void)
{
}

void
AvejLiteApp::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	switch(actionId)
	{
	case ACTION_ID_BUTTON_1:
	case ACTION_ID_BUTTON_2:
	case ACTION_ID_BUTTON_3:
	case ACTION_ID_BUTTON_4:
	case ACTION_ID_BUTTON_5:
	case ACTION_ID_BUTTON_6:
	case ACTION_ID_BUTTON_7:
	case ACTION_ID_BUTTON_8:
	case ACTION_ID_BUTTON_9:
	case ACTION_ID_BUTTON_A:
		if (s_app_working)
		{
			avej_launcher::Finalize();
			s_app_working = false;
		}
		{
			__pPanel->SetShowState(false);
			__pForm->Draw();
			__pForm->Show();

			int app_id = actionId - ACTION_ID_BUTTON_BASE;

			int x, y, screen_width, screen_height;

			__pForm->GetBounds(x, y, screen_width, screen_height);

			if (app_id == 0)
			{
				screen_width = screen_height * 320 / 240;
			}

			AppLog("[SMGAL] resolution(%d, %d)", screen_width, screen_height);

			if (avej_launcher::Initialize(app_id, (void*)__pForm, screen_width, screen_height))
			{
				s_app_working = true;
				__pTimer->Start(TIME_OUT);
			}
		}
		break;
	default:
		break;
	}
}
