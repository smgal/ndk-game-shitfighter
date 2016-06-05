
////////////////////////////////////////////////////////////////////////////////
// uses

#include "sf_main.h"
#include "sf_gfx.h"

#include "sf_res.h"
#include "sf_util.h"

#ifdef _WIN32
#	pragma warning(disable: 4786)
#endif

#include <vector>
#include <string>
#include <algorithm>

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// callback

#define START_MAKER  "#START STORY"
#define END_MAKER    "#END"

namespace menu_story
{
	const  int  H_KEY_GUIDE     = (16*2);
	const  int  PIXELS_PER_PAGE = 240*2;

	static bool s_auto_scroll = false;
	static int  s_scroll_time_stamp = 0;
	static int  s_scroll_time_stamp_delay = 2;
	static int  s_min_scroll_time_stamp = -PIXELS_PER_PAGE * s_scroll_time_stamp_delay;
	static int  s_max_scroll_time_stamp = 0;
	static std::vector< std::pair<unsigned long, std::string> > s_string_list;

	static gui::CKeyGuide s_key_guide;

	static int  s_x = 0;
	static int  s_y = 0;
	
	static void s_PrintString(const std::pair<unsigned long, std::string>& string_pair)
	{
		if ((s_y + GLYPH_H > 0) && (s_y < PIXELS_PER_PAGE))
		{
			unsigned long real_color    = string_pair.first;
			unsigned long shading_color = ((real_color >> 1) & 0xFF000000) | (real_color & 0x00FFFFFF);

			gfx::DrawText(s_x+1, s_y+0, string_pair.second.c_str(), shading_color, true);
			gfx::DrawText(s_x+0, s_y+1, string_pair.second.c_str(), shading_color, true);
			gfx::DrawText(s_x+0, s_y+0, string_pair.second.c_str(), real_color,    true);
		}

		s_y += GLYPH_H;
	}

	bool OnCreate(void)
	{
		::util::CTextFileFromRes text_res((const char*)p_res_story, sizeof_p_res_story());

		if (text_res.IsValid())
		{
			const unsigned long COLOR_TABLE[4] =
			{
				0xFFFFFFFF, 0xFF80FFBF, 0xFFC080FF, 0xFFFF80BF
			};

			char s[256];

			while (text_res.GetString(s, sizeof(s)))
			{
				if (strncmp(s, START_MAKER, sizeof(START_MAKER)-1) == 0)
					break;
			}

			while (text_res.GetString(s, sizeof(s)))
			{
				if (strncmp(s, END_MAKER, sizeof(END_MAKER)-1) == 0)
					break;

				int len = strlen(s);
				while (len > 0 && s[len] < 27)
					s[len--] = 0;

				if (len > 0)
				{
					unsigned long color_index = s[len] - '1';
					s[len--] = 0;

					color_index = clamp(color_index, 0UL, (unsigned long)(sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0])));
					s_string_list.push_back(std::make_pair<unsigned long, std::string>(COLOR_TABLE[color_index], s));
				}
				else
				{
					s_string_list.push_back(std::make_pair<unsigned long, std::string>(0, ""));
				}
			}
		}

		s_scroll_time_stamp      = s_min_scroll_time_stamp;

		s_max_scroll_time_stamp  = (s_string_list.size()+1) * GLYPH_H - PIXELS_PER_PAGE + H_KEY_GUIDE;
		s_max_scroll_time_stamp *= s_scroll_time_stamp_delay;
		s_max_scroll_time_stamp  = max(s_max_scroll_time_stamp, 0);

		s_auto_scroll = true;

		new (&s_key_guide) gui::CKeyGuide(0, 480-H_KEY_GUIDE, 640, H_KEY_GUIDE);
		s_key_guide.SetLeftText(" B키: 게임 시작");
		s_key_guide.SetRightText("X키, Menu키: 타이틀로 ");

		return true;
	}

	bool OnDestory(void)
	{
		s_string_list.clear();

		return true;
	}

	bool OnProcess(void)
	{
		{
			CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
			input_device.UpdateInputState();

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1) ||
				input_device.WasKeyPressed(avej_lite::INPUT_KEY_B))
			{
				g_ChangeState(STATE_TITLE);
				return false;
			}

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_A))
			{
				g_ChangeState(STATE_GAME_PLAY);
				return false;
			}

			int saved_scroll_time_stamp = s_scroll_time_stamp;

			if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_UP))
			{
				if (s_scroll_time_stamp > 0)
					s_scroll_time_stamp -= s_scroll_time_stamp_delay;
			}
			if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_DOWN))
				s_scroll_time_stamp += s_scroll_time_stamp_delay;

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_L1))
			{
				if (s_scroll_time_stamp > 0)
				{
					s_scroll_time_stamp -= (PIXELS_PER_PAGE - GLYPH_H) * s_scroll_time_stamp_delay;
					s_scroll_time_stamp  = max(s_scroll_time_stamp, 0);
				}
			}
			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_R1))
				s_scroll_time_stamp += (PIXELS_PER_PAGE - GLYPH_H) * s_scroll_time_stamp_delay;

			if (s_auto_scroll)
				s_auto_scroll = (saved_scroll_time_stamp == s_scroll_time_stamp);

			s_scroll_time_stamp = clamp(s_scroll_time_stamp, s_min_scroll_time_stamp, s_max_scroll_time_stamp);
		}

		g_p_gfx_device->BeginDraw();

		{
			s_x = 6;
			s_y = 4 - s_scroll_time_stamp / s_scroll_time_stamp_delay;

			std::for_each(s_string_list.begin(), s_string_list.end(), s_PrintString);

			if (s_auto_scroll)
				++s_scroll_time_stamp;

			s_key_guide.Show();
		}

		g_p_gfx_device->EndDraw();
		g_p_gfx_device->Flip();

		return true;
	}

	AppCallback callback =
	{
		OnCreate,
		OnDestory,
		OnProcess
	};
}
