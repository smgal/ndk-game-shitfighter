
////////////////////////////////////////////////////////////////////////////////
// uses

#include "sf_util.h"
#include "sf_main.h"
#include "sf_obj.h"
#include "sf_gfx.h"
#include "sf_sys_desc.h"

using namespace avej_lite;

#define for if (0); else for

////////////////////////////////////////////////////////////////////////////////
// resource

static const int   G_MAX_MENU = 4;
static const char* G_MENU_NAME[G_MAX_MENU] =
{
   "1P: SMgal 2P: x",
   "1P: NeTo  2P: x",
   "1P: SMgal 2P: Neto",
   "1P: Neto  2P: SMgal"
};

////////////////////////////////////////////////////////////////////////////////
// callback

namespace menu_option
{
	static int s_selected_menu;

	bool OnCreate(void)
	{
		s_selected_menu = ord_(GetSysDesc().GetPlayerMode());

		return true;
	}

	bool OnDestory(void)
	{
		SetSysDesc().SetPlayerMode(TPlayerMode(s_selected_menu));

		return true;
	}

	bool OnProcess(void)
	{
		{
			CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
			input_device.UpdateInputState();

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1) ||
				input_device.WasKeyPressed(avej_lite::INPUT_KEY_A) ||
				input_device.WasKeyPressed(avej_lite::INPUT_KEY_B))
			{
				g_ChangeState(STATE_TITLE);
				return false;
			}

			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_UP))
			{
				if (--s_selected_menu < 0)
					s_selected_menu = G_MAX_MENU - 1;
			}
			if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_DOWN))
			{
				++s_selected_menu;
				s_selected_menu %= G_MAX_MENU;
			}
		}


		{
			static int s_count_step = 0;
			static int s_count = 0;

			if (++s_count_step == 5)
			{
				s_count_step = 0;
				s_count = (s_count+1) % 2;
			}
	
			g_p_gfx_device->BeginDraw();

			const TRect* p_rect[3] = { 0, 0, 0 };

			switch (s_selected_menu)
			{
			case 0: // player1
				p_rect[0] = &g_sprite_rect[TILENAME_SMGAL1+s_count];
				break;
			case 1: // player2
				p_rect[0] = &g_sprite_rect[TILENAME_NETO1+s_count];
				break;
			case 2: // dual_mode1
				p_rect[1] = &g_sprite_rect[TILENAME_SMGAL1+s_count];
				p_rect[2] = &g_sprite_rect[TILENAME_NETO1+s_count];
				break;
			case 3: // dual_mode2
				p_rect[1] = &g_sprite_rect[TILENAME_NETO1+s_count];
				p_rect[2] = &g_sprite_rect[TILENAME_SMGAL1+s_count];
				break;
			}

			const int x_center = 320;
			const int y_center = 70;
			const int x_gap    = 120;
			const int y_gap    = 0;

			const int POS_TABLE[3][2] =
			{
				{ x_center,         y_center        },
				{ x_center - x_gap, y_center - y_gap},
				{ x_center + x_gap, y_center + y_gap},
			};

			for (int i = 0; i < 3; i++)
			{
				if (p_rect[i])
				{
					int x_revised = POS_TABLE[i][0] - p_rect[i]->w / 2;
					gfx::BitBlt(x_revised, POS_TABLE[i][1], g_p_res_sprite, p_rect[i]->x, p_rect[i]->y, p_rect[i]->w, p_rect[i]->h);
				}
			}

			for (int i = 0; i < G_MAX_MENU; i++)
			{
				int x = 210;
				int color_index = (i == ord_(s_selected_menu)) ? 10 : 2;
				gfx_ix::DrawText(x+2,301+i*24,G_MENU_NAME[i],TIndexColor(color_index-1));
				gfx_ix::DrawText(x+0,300+i*24,G_MENU_NAME[i],TIndexColor(color_index));
			}

			{
				int x = 210;
				int y = 170;

				switch (s_selected_menu)
				{
				case 0: // player1
				case 1: // player2
					x = 210;
					gfx_ix::DrawText(x, y+GLYPH_H*0, "이     동: 왼쪽 십자키", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*1, "일반 공격: B키", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*2, "특수 공격: X키", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*3, "게임 종료: Menu키", TIndexColor(15));
					break;
				case 2: // dual_mode1
				case 3: // dual_mode2
					x = 40;
					gfx_ix::DrawText(x, y+GLYPH_H*0, "1P 이동: 왼쪽 십자키    2P 이동: 오른쪽 십자키", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*1, "1P 일반: auto           2P 일반: auto", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*2, "1P 특수: L키            2P 특수: R키", TIndexColor(15));
					gfx_ix::DrawText(x, y+GLYPH_H*3, "게임 종료: Menu키 + Select키", TIndexColor(15));
					break;
				}
			}

			g_p_gfx_device->EndDraw();
			g_p_gfx_device->Flip();
		}

		return true;
	}

	AppCallback callback =
	{
		OnCreate,
		OnDestory,
		OnProcess
	};
}
