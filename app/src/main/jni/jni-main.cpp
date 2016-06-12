
#include <jni.h>
#include <zip.h>
#include <zipint.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#include <android/log.h>

////////////////////////////////////////////////////////////////////////////////
// macro

#define  LOG_TAG    "[JNI]"
#define  LOG_D(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOG_I(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOG_W(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define  LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// macro

#define PACKAGE_MANGLING Java_com_avej_game

#define DECLARE_FUNC_EX(_PACKAGE_NAME_, _NAME_SPACE_, _METHOD_) \
	_PACKAGE_NAME_##_##_NAME_SPACE_##_##_METHOD_

#define DECLARE_FUNC(_PACKAGE_NAME_, _NAME_SPACE_, _METHOD_) \
	DECLARE_FUNC_EX(_PACKAGE_NAME_, _NAME_SPACE_, _METHOD_)

#define DECLARE_JNI_FUNC(_NAME_SPACE_, _METHOD_) \
	DECLARE_FUNC(PACKAGE_MANGLING, _NAME_SPACE_, _METHOD_)

////////////////////////////////////////////////////////////////////////////////
//

namespace app
{
	struct InputEvent
	{
		enum TYPE
		{
			TYPE_TOUCH_DOWN = 1,
			TYPE_TOUCH_UP = 2,
			TYPE_KEY_DOWN = 3,
			TYPE_KEY_UP = 4
		};

		TYPE type;
		int key;

		int x;
		int y;
	};
}

namespace glue
{
	void init(void);
	void done(void);
	void render(long current_tick, int width, int height);
	void onTouchPress(int id, int x, int y);
	void onTouchRelease(int id);
	void onKeyPress(int key);
	void onKeyRelease(int key);
}

////////////////////////////////////////////////////////////////////////////////
// forward

extern "C" JNIEXPORT jboolean DECLARE_JNI_FUNC(SmJNI, init)(JNIEnv* p_env, jobject obj, jstring sj_package_path, jstring sj_data_path, jstring sj_package_name, jstring sj_app_name, jobject j_system_desc);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, resize)(JNIEnv* p_env, jobject obj, jint width, jint height);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, done)(JNIEnv* p_env, jobject obj);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, process)(JNIEnv* p_env, jobject obj, jint action_type, jint param_x, jint param_y);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, pause)(JNIEnv* p_env, jobject obj);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, resume)(JNIEnv* p_env);
extern "C" JNIEXPORT void DECLARE_JNI_FUNC(SmJNI, render)(JNIEnv* p_env, jobject obj);

////////////////////////////////////////////////////////////////////////////////
//

static int  s_window_width  = 320;
static int  s_window_height = 480;

long g_getTime(void)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}

////////////////////////
// new

char g_package_name[1024] = {0};
struct zip* g_p_zip_file;

int g_fileExists(const char* sz_file_name)
{
	if (g_p_zip_file != NULL)
	{
		int result = zip_name_locate(g_p_zip_file, sz_file_name, 0);
		return (result != -1) ? 1 : 0;
	}

	return 0;
}

FILE* g_fileOpen(const char* sz_file_name, unsigned int* out_start_offset, unsigned int* out_length)
{
	struct zip_file* p_zip_file = zip_fopen(g_p_zip_file, sz_file_name, 0);

	if (p_zip_file)
	{
		FILE* p_file = NULL;

		if (out_start_offset)
			*out_start_offset = p_zip_file->fpos;

		if (out_length)
			*out_length = p_zip_file->bytes_left;

		zip_fclose(p_zip_file);

		p_file = fopen(g_package_name, "rb");
		fseek(p_file, p_zip_file->fpos, SEEK_SET);

		return p_file;
	}
	else
	{
		return NULL;
	}
}

typedef struct zip_file zip_file;

void g_createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_buffer, unsigned int* out_buffer_length)
{
	if (out_p_buffer)
	{
		*out_p_buffer = NULL;

		LOG_D("g_createBufferFromCompressedFile(%d)", __LINE__);
		zip_file* p_file = zip_fopen(g_p_zip_file, sz_file_name, 0);
		LOG_D("g_createBufferFromCompressedFile(%d)", __LINE__);

		if (p_file)
		{
			unsigned int length = p_file->bytes_left;

			unsigned char* p_buffer = (unsigned char*)malloc(length);

			if (p_buffer)
			{
				zip_fread(p_file, p_buffer, length);

				*out_p_buffer = p_buffer;

				if (out_buffer_length)
					*out_buffer_length = length;
			}

			zip_fclose(p_file);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//

namespace
{
	bool _is_not_initialized = true;
}

//void Java_com_avej_game_dreamlime_Native_init(JNIEnv* p_env, jobject obj)
jboolean DECLARE_JNI_FUNC(SmJNI, init)(JNIEnv* p_env, jobject obj, jstring sj_package_path, jstring sj_data_path, jstring sj_package_name, jstring sj_app_name, jobject j_system_desc)
//jboolean DECLARE_JNI_FUNC(SmJNI, init)(JNIEnv* p_env, jobject obj, jstring sj_package_path)
{
	if (!_is_not_initialized)
		return JNI_FALSE;

	LOG_D("[CALL] JNI.init()");

	const char* sz_package_path = p_env->GetStringUTFChars(sj_package_path, 0);
	const char* sz_data_path = p_env->GetStringUTFChars(sj_data_path, 0);
	const char* sz_package_name = p_env->GetStringUTFChars(sj_package_name, 0);
	const char* sz_app_name = p_env->GetStringUTFChars(sj_app_name, 0);

	LOG_D("\t+--------------+------------------------------");
	LOG_D("\t| Package path | %s", sz_package_path);
	LOG_D("\t| Data path    | %s", sz_data_path);
	LOG_D("\t| Package name | %s", sz_package_name);
	LOG_D("\t| App name     | %s", sz_app_name);
	LOG_D("\t+--------------+------------------------------");

	char app_name[256];
	strncpy(app_name, sz_app_name, 255);

	//target::file_io::init(sz_package_path);

	p_env->ReleaseStringUTFChars(sj_app_name, sz_app_name);
	p_env->ReleaseStringUTFChars(sj_package_name, sz_package_name);
	p_env->ReleaseStringUTFChars(sj_data_path, sz_data_path);
	p_env->ReleaseStringUTFChars(sj_package_path, sz_package_path);

#if 0
	int required_width = 720;
	int required_height = 1440;
	{
		if (strcmp(app_name, "shitfighter") == 0)
		{
			required_width = 800;
			required_height = 480;
		}
		else if (strcmp(app_name, "yunjr") == 0)
		{
			required_width = 720;
			required_height = 1280;
		}
	}

	{
		jfieldID fid;
		jclass jclass_system_desc = p_env->GetObjectClass(j_system_desc);

		DECLARE_INT(p_env, jclass_system_desc, j_system_desc, screen_width)
		DECLARE_INT(p_env, jclass_system_desc, j_system_desc, screen_height)

		SET_INT(p_env, jclass_system_desc, j_system_desc, buffer_width, required_width);
		SET_INT(p_env, jclass_system_desc, j_system_desc, buffer_height, required_height);
/*
		fid = p_env->GetFieldID(jclass_system_desc, "buffer_width", "I");
		p_env->SetIntField(j_system_desc, fid, required_width);

		fid = p_env->GetFieldID(jclass_system_desc, "buffer_height", "I");
		p_env->SetIntField(j_system_desc, fid, required_height);
*/
	}
#endif

	glue::init();

	_is_not_initialized = false;

	return JNI_TRUE;
}

void DECLARE_JNI_FUNC(SmJNI, resize)(JNIEnv* p_env, jobject obj, jint width, jint height)
{
	if (_is_not_initialized)
		return;

	LOG_D("[CALL] JNI.resize(%d x %d)", width, height);

	s_window_width  = width;
	s_window_height = height;
}

void DECLARE_JNI_FUNC(SmJNI, done)(JNIEnv* p_env, jobject obj)
{
	if (_is_not_initialized)
		return;

	LOG_D("[CALL] JNI.done()");

	glue::done();
}

void DECLARE_JNI_FUNC(SmJNI, process)(JNIEnv* p_env, jobject obj, jint action_type, jint param_x, jint param_y)
{
	if (_is_not_initialized)
		return;

	switch (action_type)
	{
	case app::InputEvent::TYPE_TOUCH_DOWN:
		LOG_D("[JNI] touched(DN) - (%d, %d)", param_x, param_y);
		glue::onTouchPress(0, param_x, param_y);
		break;
	case app::InputEvent::TYPE_TOUCH_UP:
		LOG_D("[JNI] touched(UP)");
		glue::onTouchRelease(0);
		break;
	case app::InputEvent::TYPE_KEY_DOWN:
		LOG_D("[JNI] key(DN) - ('%c')", (char)param_x);
		glue::onKeyPress(param_x);
		break;
	case app::InputEvent::TYPE_KEY_UP:
		LOG_D("[JNI] key(UP) - ('%c')", (char)param_x);
		glue::onKeyRelease(param_x);
		break;
	}
}

void DECLARE_JNI_FUNC(SmJNI, pause)(JNIEnv* p_env, jobject obj)
{
	if (_is_not_initialized)
		return;

	LOG_D("[CALL] JNI.pause()");
}

void DECLARE_JNI_FUNC(SmJNI, resume)(JNIEnv* p_env)
{
	if (_is_not_initialized)
		return;

	LOG_D("[CALL] JNI.resume()");
}

void DECLARE_JNI_FUNC(SmJNI, render)(JNIEnv* p_env, jobject obj)
{
	if (_is_not_initialized)
		return;

	static int s_is_first = 1;

	if (s_is_first)
	{
		LOG_D("[CALL] JNI.render()");
		s_is_first = 0;
	}

    long current_time = g_getTime();

	glue::render(current_time, s_window_width, s_window_height);
}
