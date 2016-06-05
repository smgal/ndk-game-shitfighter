
////////////////////////////////////////////////////////////////////////////////
// Application list

static const char* APP_LIST[] =
{
	"KanoMap",
	"LoreBase",
	"IQ Block",
	"AvejDream",
	"AvejKano",
	"ManoEri",
	"ShitFighter_Wave",
	"ShitFighter",
	0
};

static const char** s_GetAppList(void)
{
	return APP_LIST;
}

void GetAppList(const char* app_list[], int num_alloc)
{
	const char** p_sz_app_list = s_GetAppList();
	const char** p_sz_app_list_end = p_sz_app_list;

	const char** p_sz_out_list = app_list;

	{
		const char** p_p_string = p_sz_app_list;
		while (*p_sz_app_list_end && num_alloc-- > 0)
		{
			*p_sz_out_list++ = *p_sz_app_list_end++;
		}

		if (num_alloc > 0)
			*p_sz_out_list++ = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////
// uses

#include "avejapp_register.h"
#include "avej_lite.h"

#if (TARGET_DEVICE == TARGET_GP2XWIZ)
#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// AvejMain()

int QAZ();

int AvejMain(void)
{
	//return QAZ();
	CAppRegister::FnGetApp* FnGetAppCallback = CAppRegister::GetAppByName(APP_LIST[0+4]);

	if (FnGetAppCallback)
	{
		TAppCallback app_callback;

		if ((*FnGetAppCallback)(app_callback))
		{
			app_callback.Initialize(0);

			while (app_callback.Process())
				;

			app_callback.Finalize();
		}
	}

#if (TARGET_DEVICE == TARGET_GP2XWIZ)
	chdir("/usr/gp2x");
#if defined(_CAANOO_)
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", "--view-main", NULL);
#else
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif

#endif

	return 0;
}




#include <stdio.h>

int QAZ()
{
	avej_lite::util::CFileReadStream file_stream("lore_ep1.cm2");

	if (file_stream.IsValid())
	{
		int   file_size = file_stream.GetSize();
		char* p_buffer  = new char[file_size];

		file_stream.Read(p_buffer, file_size);

		avej_lite::util::CTextHolder text_holder(p_buffer, file_size);

		delete[] p_buffer;

		int count = text_holder.GetLineCount();

		for (int i = 0; i < count; i++)
		{
			char str[1024];
			text_holder.GetLine(i, str, 1024);
			printf("%3d(%3d): %s\n", i, text_holder.GetLineLength(i), str);
		}

	}

	return 0;
}
