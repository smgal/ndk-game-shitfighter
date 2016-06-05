
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

#define START_MAKER  "#BAD ENDING"
#define END_MAKER    "#END"

namespace ending_bad
{
	const  int  H_KEY_GUIDE     = (16*2);
	const  int  PIXELS_PER_PAGE = 240*2;

	static int  s_scroll_time_stamp = 0;
	static int  s_scroll_time_stamp_delay = 2;
	static int  s_max_scroll_time_stamp = 0;
	static int  s_finger_down = 0;
	static int  s_finger_down_delay = 4; // 1 이상
	static int  s_max_finger_down = 60 * s_finger_down_delay;
	static std::vector< std::pair<unsigned long, std::string> > s_string_list;

	static gui::CKeyGuide s_key_guide;

	static int  s_x = 0;
	static int  s_y = 0;
	
	static void s_PrintString(const std::pair<unsigned long, std::string>& string_pair)
	{
		if ((s_y + GLYPH_H > 0) && (s_y < PIXELS_PER_PAGE))
		{
			if (s_y > s_scroll_time_stamp / s_scroll_time_stamp_delay)
				return;

			int x = s_x;
			unsigned long real_color    = string_pair.first;

			if (s_y+GLYPH_H > s_scroll_time_stamp / s_scroll_time_stamp_delay)
			{
				int rate = s_y + GLYPH_H - s_scroll_time_stamp / s_scroll_time_stamp_delay;
				x += rate * 2;
				real_color = (((real_color >> 24) * (GLYPH_H-rate) / GLYPH_H) << 24) | (real_color & 0x00FFFFFF);
			}

			unsigned long shading_color = ((real_color >> 1) & 0xFF000000) | (real_color & 0x00FFFFFF);

			gfx::DrawText(x+1, s_y+0, string_pair.second.c_str(), shading_color, true);
			gfx::DrawText(x+0, s_y+1, string_pair.second.c_str(), shading_color, true);
			gfx::DrawText(x+0, s_y+0, string_pair.second.c_str(), real_color,    true);
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

					color_index = clamp(color_index, 0UL, 3UL);
					s_string_list.push_back(std::make_pair<unsigned long, std::string>(COLOR_TABLE[color_index], s));
				}
				else
				{
					s_string_list.push_back(std::make_pair<unsigned long, std::string>(0, ""));
				}
			}
		}

		s_scroll_time_stamp = 0;

		s_max_scroll_time_stamp  = (s_string_list.size()+1) * GLYPH_H;
		s_max_scroll_time_stamp *= s_scroll_time_stamp_delay;
		s_max_scroll_time_stamp  = max(s_max_scroll_time_stamp, 0);

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
		{
			CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
			input_device.UpdateInputState();

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1))
			{
				if (s_scroll_time_stamp >= s_max_scroll_time_stamp)
				{
					g_ChangeState(STATE_EXIT);
					return false;
				}
				else
				{
					s_scroll_time_stamp += GLYPH_H;
				}
			}

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_A) ||
				input_device.WasKeyPressed(avej_lite::INPUT_KEY_B))
				s_scroll_time_stamp += GLYPH_H;

			if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_UP))
				s_scroll_time_stamp -= GLYPH_H;
			if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_DOWN))
				s_scroll_time_stamp += GLYPH_H;

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_L1))
				s_scroll_time_stamp -= (PIXELS_PER_PAGE - GLYPH_H);
			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_R1))
				s_scroll_time_stamp += (PIXELS_PER_PAGE - GLYPH_H);

			s_scroll_time_stamp = clamp(++s_scroll_time_stamp, 0, s_max_scroll_time_stamp);

			if (s_scroll_time_stamp == s_max_scroll_time_stamp)
				s_finger_down = clamp(++s_finger_down, 0, s_max_finger_down);
		}

		g_p_gfx_device->BeginDraw();

		{
			const TRect& rect1 = g_sprite_rect[TILENAME_TILE_SHIT_FIELD];
			const TRect& rect2 = g_sprite_rect[TILENAME_TILE_SHIT_FIELD_SUB];
			const TRect& rect3 = g_sprite_rect[TILENAME_TILE_FINGER];

			int x = 296-30;
			int y = 353+27-30;

			gfx::BitBlt(x    , y   , g_p_res_sprite, rect1.x, rect1.y, rect1.w, rect1.h);
			gfx::BitBlt(x+196, y-2+s_finger_down/s_finger_down_delay, g_p_res_sprite, rect3.x, rect3.y, rect3.w, rect3.h-(s_finger_down/s_finger_down_delay));
			gfx::BitBlt(x+184, y+54, g_p_res_sprite, rect2.x, rect2.y, rect2.w, rect2.h);
		}
		{
			s_x = 6;
			s_y = 4;

			std::for_each(s_string_list.begin(), s_string_list.end(), s_PrintString);
		}

		s_key_guide.Show();

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
