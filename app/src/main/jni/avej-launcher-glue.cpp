
#include "src/avejapp_register.h"
#include <avej_app.h>

#include <android/log.h>

////////////////////////////////////////////////////////////////////////////////
// macro

#define  LOG_TAG    "SMGAL"
#define  LOG_D(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOG_I(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOG_W(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define  LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// external

namespace target
{
	void setTouchPos(int id, int x, int y);
	void setKeyPressed(int id);
	void setKeyReleased(int id);
}

////////////////////////////////////////////////////////////////////////////////
// glue

namespace glue
{
	TAppCallback app_callback;

	void init(void)
	{
		if (app_callback.Process == 0)
		{
			LOG_D("glue::init(%d)", __LINE__);
//			CAppRegister::FnGetApp* fn_get_app_callback = CAppRegister::GetAppByName("AvejDream");
//			CAppRegister::FnGetApp* fn_get_app_callback = CAppRegister::GetAppByName("AvejKano");
			CAppRegister::FnGetApp* fn_get_app_callback = CAppRegister::GetAppByName("ShitFighter_Wave");
//			CAppRegister::FnGetApp* fn_get_app_callback = CAppRegister::GetAppByName("IQ Block");

			LOG_D("glue::init(%d)", __LINE__);
			if (fn_get_app_callback)
				(*fn_get_app_callback)(app_callback);

			LOG_D("glue::init(%d) %#x", __LINE__, app_callback);
			if (app_callback.Initialize)
				app_callback.Initialize(0);
			LOG_D("glue::init(%d)", __LINE__);
		}
	}

	void done(void)
	{
		if (app_callback.Finalize)
			app_callback.Finalize();
	}

	void render(long current_tick, int width, int height)
	{
		if (app_callback.Process)
			app_callback.Process();
	}

	void onTouchPress(int id, int x, int y)
	{
		target::setTouchPos(0, x, y);
	}

	void onTouchRelease(int id)
	{
		target::setTouchPos(0, -1, -1);
	}

	void onKeyPress(int key)
	{
		int avej_key = avej_lite::INPUT_KEY_MAX;

		switch ((char)key)
		{
		case '8':
		    avej_key = avej_lite::INPUT_KEY_UP;
		    break;
		case '2':
		    avej_key = avej_lite::INPUT_KEY_DOWN;
		    break;
		case '4':
		    avej_key = avej_lite::INPUT_KEY_LEFT;
		    break;
		case '6':
		    avej_key = avej_lite::INPUT_KEY_RIGHT;
		    break;
		case 'A':
		    avej_key = avej_lite::INPUT_KEY_A;
		    break;
		case 'B':
		    avej_key = avej_lite::INPUT_KEY_B;
		    break;
		}

		target::setKeyPressed(avej_key);
	}

	void onKeyRelease(int key)
	{
		target::setKeyReleased(key);
	}
}

////////////////////////////////////////////////////////////////////////////////
//

#include <stdio.h>

extern int g_fileExists(const char* fname);
extern FILE* g_fileOpen(const char* fname, unsigned int* out_start_offset, unsigned int* out_length);
extern void g_createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_p_buffer, unsigned int* out_buffer_length);

namespace target
{
	namespace file_io
	{
		bool fileExists(const char* sz_file_name)
		{
			return (g_fileExists(sz_file_name) != 0);
		}

		FILE* fopen(const char* sz_file_name, unsigned int* out_start_offset, unsigned int* out_length)
		{
			return g_fileOpen(sz_file_name, out_start_offset, out_length);
		}

		void createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_p_buffer, unsigned int* out_buffer_length)
		{
			g_createBufferFromCompressedFile(sz_file_name, out_p_p_buffer, out_buffer_length);
		}
	}
}
