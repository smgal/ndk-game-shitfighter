
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

#define START_MAKER  "#HAPPY ENDING"
#define END_MAKER    "#END"

unsigned long Hue2RGB(double hue)
{
	typedef unsigned long UL32;

	const double PI  = 3.141592;
	const double MUL = 255.9999 * 1.0 / 2.0;

	double r = 1.0 + SF_SIN(hue - 2.0 * PI / 3.0);
	double g = 1.0 + SF_SIN(hue);
	double b = 1.0 + SF_SIN(hue + 2.0 * PI / 3.0);

	return 0xFF000000 | UL32(r*MUL) << 16 | UL32(g*MUL) << 8 | UL32(b*MUL);
}

namespace ending_happy
{
	const  int  H_KEY_GUIDE     = (16*2);
	const  int  PIXELS_PER_PAGE = 240*2;

	static int  s_stage = 0;

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

			if (real_color == 0x0BADF00D)
			{
				static double s_angle = 0;

				real_color = Hue2RGB(s_angle);

				s_angle += 0.03;
			}

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
			const unsigned long COLOR_TABLE[6] =
			{
				0xFFFFFFFF, 0xFF80FFBF, 0xFFC080FF, 0xFFFF80BF, 0xFFFFDF60, 0x0BADF00D
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
		s_stage       = 0;

		new (&s_key_guide) gui::CKeyGuide(0, 480-H_KEY_GUIDE, 640, H_KEY_GUIDE);
		s_key_guide.SetLeftText(" Menu키: 게임 종료");

		return true;
	}

	bool OnDestory(void)
	{
		s_string_list.clear();

		return true;
	}

	bool OnProcess(void)
	{
		CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
		input_device.UpdateInputState();

		switch (s_stage)
		{
		case 0:
			{
				if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1))
				{
					++s_stage;
					::util::ClearKeyBuffer();
					return true;
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
		default:
			{
				if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1) ||
					input_device.WasKeyPressed(avej_lite::INPUT_KEY_A) ||
					input_device.WasKeyPressed(avej_lite::INPUT_KEY_B))
				{
					g_ChangeState(STATE_EXIT);
					return false;
				}
			}

			g_p_gfx_device->BeginDraw();

			{
				const TRect& rect1 = g_sprite_rect[TILENAME_TILE_STATUE1];
				const TRect& rect2 = g_sprite_rect[TILENAME_TILE_STATUE2];

				int x = (640 - rect1.w) / 2;
				int y = (480 - rect1.h - rect2.h) / 2;

				gfx::BitBlt(x, y, g_p_res_sprite, rect1.x, rect1.y, rect1.w, rect1.h);

				y += rect1.h;
				gfx::BitBlt(x, y, g_p_res_sprite, rect2.x, rect2.y, rect2.w, rect2.h);
			}

			s_key_guide.Show();

			g_p_gfx_device->EndDraw();
			g_p_gfx_device->Flip();

			return true;
		}
	}

	AppCallback callback =
	{
		OnCreate,
		OnDestory,
		OnProcess
	};
}
