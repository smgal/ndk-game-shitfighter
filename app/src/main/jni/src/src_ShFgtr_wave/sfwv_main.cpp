
////////////////////////////////////////////////////////////////////////////////
// uses

#include "sfwv_config.h"
#include "sfwv_util.h"
#include "sfwv_sys_desc.h"
#include "sfwv_res.h"
#include "sfwv_gfx.h"
#include "sfwv_obj.h"
#include "util_render_text.h"

using namespace avej_lite;

namespace miku
{
////////////////////////////////////////////////////////////////////////////////
// type definition

TRect MAKE_RECT(int x1, int y1, int x2, int y2)
{
	TRect rect;

	rect.x = x1;
	rect.y = y1;
	rect.w = (x2-x1);
	rect.h = (y2-y1);

	return rect;
}

////////////////////////////////////////////////////////////////////////////////
// static variable

static TLayerDesc s_layer_option =
{
	1,
	{
		avej_lite::TLayerDesc::LAYER_ATTRIB_ALPHA_TEST | avej_lite::TLayerDesc::LAYER_ATTRIB_ALPHA_BLEND
	},
	avej_lite::TLayerDesc::OPTION_NONE
};

static bool   s_state_changing = false;
static TState s_required_state = STATE_EXIT;

////////////////////////////////////////////////////////////////////////////////
// global function

void g_ChangeState(TState state)
{
	s_required_state = state;
	s_state_changing = true;
}

////////////////////////////////////////////////////////////////////////////////
// main

namespace title        { extern AppCallback callback; }
namespace menu_about   { extern AppCallback callback; }
namespace menu_story   { extern AppCallback callback; }
namespace menu_option  { extern AppCallback callback; }
namespace game_play    { extern AppCallback callback; }
namespace ending_bad   { extern AppCallback callback; }
namespace ending_happy { extern AppCallback callback; }

const AppCallback* g_app_callback_list[STATE_EXIT+1] =
{
	// 	STATE_TITLE
	&title::callback,
	// 	STATE_MENU_ABOUT
	&menu_about::callback,
	// 	STATE_MENU_STORY
	&menu_story::callback,
	// 	STATE_MENU_OPTION
	&menu_option::callback,
	// 	STATE_GAME_PLAY
	&game_play::callback,
	// 	STATE_ENDING_BAD
	&ending_bad::callback,
	// 	STATE_ENDING_HAPPY
	&ending_happy::callback,
	//  STATE_EXIT
	NULL,
};

class CSystem
{
	enum TProcess
	{
		PROCESS_READY,
		PROCESS_IN
	};

	TProcess  m_process;
	TState    m_current_state;
	IAvejApp* m_p_app;

protected:
	void _Run(TState state)
	{
		while (g_app_callback_list[state])
		{
			util::ClearKeyBuffer();

			IAvejApp* p_app = IAvejApp::GetInstance(*g_app_callback_list[state]);
			SF_ASSERT(p_app);
			
			p_app->Process();
			p_app->Release();

			if (s_state_changing)
			{
				state = s_required_state;
				s_state_changing = false;
			}

			if (s_required_state == STATE_EXIT)
				break;
		}
	}

public:
	CSystem(TState state)
	: m_current_state(state), m_process(PROCESS_READY), m_p_app(0)
	{
		// initialize the graphics system
		g_p_gfx_device = IGfxDevice::GetInstance();

		gfx::Init();

		g_p_gfx_device->SetLayerDesc(s_layer_option);
		g_p_gfx_device->GetSurface(&g_p_back_buffer);

		// load resources
		#if defined(_MSC_VER)
		if (!g_p_gfx_device->CreateSurfaceFrom("shit_img_1024.tga", &g_p_res_sprite))
//		if (!g_p_gfx_device->CreateSurfaceFrom("res_ShitFighter_Wave/shit_img_1024.tga", &g_p_res_sprite))
		#endif
		{
			if (!g_p_gfx_device->CreateSurfaceFrom(p_res_shit_img, sizeof_p_res_shit_img(), &g_p_res_sprite))
			{
				SF_ASSERT(0 && "cannot load the specified image. (shit_img.tga)");
			}

			#if 0
			{
				FILE* pFile = fopen("shit_img_bak.tga", "wb");
				fwrite(p_res_shit_img, sizeof_p_res_shit_img(), 1, pFile);
				fclose(pFile);
			}
			#endif
		}

		// sprite data mapping
		g_sprite_rect[TILENAME_ITEM_SMGAL1]         = MAKE_RECT(0,0,23,23);
		g_sprite_rect[TILENAME_ITEM_SMGAL2]         = MAKE_RECT(24,0,47,23);
		g_sprite_rect[TILENAME_ITEM_SMGAL3]         = MAKE_RECT(48,0,71,23);
		g_sprite_rect[TILENAME_ITEM_NETO1]          = MAKE_RECT(72,0,95,23);
		g_sprite_rect[TILENAME_ITEM_NETO2]          = MAKE_RECT(96,0,119,23);
		g_sprite_rect[TILENAME_ITEM_NETO3]          = MAKE_RECT(120,0,143,23);
		g_sprite_rect[TILENAME_ITEM_HP]             = MAKE_RECT(144,0,167,23);
		g_sprite_rect[TILENAME_ITEM_SP]             = MAKE_RECT(168,0,199,30);
		g_sprite_rect[TILENAME_SMGAL_SP]            = MAKE_RECT(200,0,223,22);
		g_sprite_rect[TILENAME_NETO_SP]             = MAKE_RECT(376,0,391,10);
		g_sprite_rect[TILENAME_SMGAL_WEAPON1]       = MAKE_RECT(224,0,239,26);
		g_sprite_rect[TILENAME_SMGAL_WEAPON2]       = MAKE_RECT(240,0,255,38);
		g_sprite_rect[TILENAME_SMGAL_WEAPON3]       = MAKE_RECT(256,0,287,31);
		g_sprite_rect[TILENAME_NETO_WEAPON1]        = MAKE_RECT(288,0,303,14);
		g_sprite_rect[TILENAME_NETO_WEAPON2]        = MAKE_RECT(304,0,335,10);
		g_sprite_rect[TILENAME_NETO_WEAPON3]        = MAKE_RECT(336,0,375,21);
		g_sprite_rect[TILENAME_SCREW_SHIT]          = MAKE_RECT(0,51,23,69);
		g_sprite_rect[TILENAME_SPIT]                = MAKE_RECT(24,51,47,60);
		g_sprite_rect[TILENAME_WORM]                = MAKE_RECT(48,51,79,61);
		g_sprite_rect[TILENAME_NOSE_WAX]            = MAKE_RECT(80,51,95,62);
		g_sprite_rect[TILENAME_HAIR_WAX]            = MAKE_RECT(96,51,119,71);
		g_sprite_rect[TILENAME_OVEREAT]             = MAKE_RECT(120,51,159,68);
		g_sprite_rect[TILENAME_GAS]                 = MAKE_RECT(160,51,183,66);
		g_sprite_rect[TILENAME_SHIT]                = MAKE_RECT(184,51,215,63);
		g_sprite_rect[TILENAME_SMGAL1]              = MAKE_RECT(0,101,39,164);
		g_sprite_rect[TILENAME_SMGAL2]              = MAKE_RECT(40,101,79,164);
		g_sprite_rect[TILENAME_NETO1]               = MAKE_RECT(80,101,135,169);
		g_sprite_rect[TILENAME_NETO2]               = MAKE_RECT(136,101,191,169);
		g_sprite_rect[TILENAME_ZACO_BIG_SHIT]       = MAKE_RECT(192,101,239,144);
		g_sprite_rect[TILENAME_ZACO_TEETH]          = MAKE_RECT(240,101,279,132);
		g_sprite_rect[TILENAME_ZACO_ROACH]          = MAKE_RECT(280,101,327,125);
		g_sprite_rect[TILENAME_ZACO_FLY]            = MAKE_RECT(328,101,367,134);
		g_sprite_rect[TILENAME_BOSS_NOSE_WAX_MAN]   = MAKE_RECT(368,101,439,171);
		g_sprite_rect[TILENAME_BOSS_HAIR_WAX_MAN1]  = MAKE_RECT(440,101,503,172);
		g_sprite_rect[TILENAME_BOSS_HAIR_WAX_MAN2]  = MAKE_RECT(504,101,567,172);
		g_sprite_rect[TILENAME_BOSS_OVEREAT_MAN]    = MAKE_RECT(48,201,111,267);
		g_sprite_rect[TILENAME_BOSS_GAS_MAN]        = MAKE_RECT(112,201,183,273);
		g_sprite_rect[TILENAME_BOSS_SHIT_FIGHTER]   = MAKE_RECT(0,291,231,449);
		g_sprite_rect[TILENAME_TILE_SHIT_FIELD]     = MAKE_RECT(296,353,639,450);
		g_sprite_rect[TILENAME_TILE_SHIT_FIELD_SUB] = MAKE_RECT(192,267,231,273);
		g_sprite_rect[TILENAME_TILE_FINGER]         = MAKE_RECT(192,201,231,261);
//		g_sprite_rect[TILENAME_TILE_STATUE]         = MAKE_RECT(536,201,639,310);
		g_sprite_rect[TILENAME_TILE_STATUE1]        = MAKE_RECT(213*2, 101*2, (213+107)*2-1, (101+75)*2-1);
		g_sprite_rect[TILENAME_TILE_STATUE2]        = MAKE_RECT(141*2, 472*2, (141+107)*2-1, (472+40)*2-1);
		g_sprite_rect[TILENAME_TILE_GROUND]         = MAKE_RECT(0,200,48,248);
		g_sprite_rect[TILENAME_TILE_SHIT_FLY]       = MAKE_RECT(209*2, 101*2, (209+3)*2-1, (101+2)*2-1);
		g_sprite_rect[TILENAME_TILE_MAIN_TITLE]     = MAKE_RECT(248*2, 417*2, 512*2-1, 512*2-1); // exclude (248,417)-(320,472)
		g_sprite_rect[TILENAME_SMGAL_AND_NETO]      = MAKE_RECT(320*2, 393*2, (320+84)*2-1, (393+20)*2-1);
		g_sprite_rect[TILENAME_TILE_STATUS_BAR]     = MAKE_RECT(0,452,639,479);

		SetSysDesc().SetPlayerMode(PLAYERMODE_PLAYER2_ONLY);
	}
	~CSystem()
	{
		delete g_p_res_sprite;
		g_p_gfx_device->Release();
	}
	bool Process(void)
	{
		switch (m_process)
		{
		case PROCESS_READY:
			{
				if (g_app_callback_list[m_current_state] == 0)
					return false;

				util::ClearKeyBuffer();

				assert(m_p_app == 0);
				m_p_app = IAvejApp::GetInstance(*g_app_callback_list[m_current_state]);
				assert(m_p_app);

				m_process = PROCESS_IN;
			}
			// pass through
		case PROCESS_IN:
			{
				assert(m_p_app);

				if (!m_p_app->Process())
				{
					m_p_app->Release();
					m_p_app = 0;

					if (s_state_changing)
					{
						m_current_state = s_required_state;
						s_state_changing = false;
					}

					if (s_required_state == STATE_EXIT)
						return false;

					m_process = PROCESS_READY;
				}
			}
			break;
		}

		return true;
	}
};

} // namespace miku

////////////////////////////////////////////////////////////////////////////////
// main

using namespace miku;

static CSystem* p_system = 0;

static void avej_init()
{
//	p_system = new CSystem(STATE_GAME_PLAY);
	p_system = new CSystem(STATE_TITLE);
}

static void avej_done()
{
	delete p_system;
}

static bool avej_process()
{
	return p_system->Process();
}

#include "avejapp_register.h"

static bool s_Initialize(void* h_window)
{
#if (TARGET_DEVICE == TARGET_BADA)
	avej_lite::util::SetResourcePath("res_ShitFighter_Wave");
#endif

	SetTextBufferDesc(24, 1024*3/4);

	p_system = new CSystem(STATE_TITLE);
//	p_system = new CSystem(STATE_MENU_STORY);
//	p_system = new CSystem(STATE_GAME_PLAY);

	return (p_system != 0);
}

static void s_Finalize(void)
{
	delete p_system;
}

static bool s_Process(void)
{
	return p_system->Process();
}

static bool GetAppCallback(TAppCallback& out_callback)
{
	out_callback.Initialize = s_Initialize;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

REGISTER_APP_1("ShitFighter_Wave", GetAppCallback);
