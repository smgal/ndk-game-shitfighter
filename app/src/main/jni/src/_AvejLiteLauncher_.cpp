
////////////////////////////////////////////////////////////////////////////////
// uses

#include "_AvejLiteLauncher_.h"
#include "avejapp_register.h"

////////////////////////////////////////////////////////////////////////////////
// grobal variables

void* g_hNativeWindow      = 0;
int   g_nativeWindowWidth  = 0;
int   g_nativeWindowHeight = 0;

////////////////////////////////////////////////////////////////////////////////
// static variables

static TAppCallback app_callback;

////////////////////////////////////////////////////////////////////////////////
// namespace avej_launcher

extern void GetAppList(const char* app_list[], int num_alloc);

void avej_launcher::GetAppList(const char* app_list[], int num_alloc)
{
	::GetAppList(app_list, num_alloc);
}

bool avej_launcher::Initialize(int app_id, void* handle, int screen_width, int screen_height)
{
	g_hNativeWindow      = handle;
	g_nativeWindowWidth  = screen_width;
	g_nativeWindowHeight = screen_height;

	const char* app_list[10];

	GetAppList(app_list, 10);

	const char** p_sz_app_list     = app_list;
	const char** p_sz_app_list_end = p_sz_app_list;

	{
		const char** p_p_string = p_sz_app_list;
		while (*p_sz_app_list_end)
			p_sz_app_list_end++;
	}

	int num_of_app = p_sz_app_list_end - p_sz_app_list;

	if (app_id < 0)
		app_id = 0;

	if (app_id >= num_of_app)
		app_id = num_of_app - 1;

	if (app_id < 0)
		return false;

	{
		const char* sz_app_list = *(p_sz_app_list + app_id);

		CAppRegister::FnGetApp* fn_get_app_callback = CAppRegister::GetAppByName(sz_app_list);
		if (fn_get_app_callback)
		{
			(*fn_get_app_callback)(app_callback);
			app_callback.Initialize(0);
		}
	}

	return true;
}

void avej_launcher::Finalize(void)
{
	if (app_callback.Finalize)
		app_callback.Finalize();
}

bool avej_launcher::Process(void)
{
	return (app_callback.Process) ? app_callback.Process() : false;
}
