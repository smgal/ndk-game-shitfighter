
////////////////////////////////////////////////////////////////////////////////
// uses

#include <vector>
#include <algorithm>

#include "avej_lite.h"

#include "me_main.h"
#include "me_tile_map.h"
#include "me_fos_map.h"
#include "me_player.h"
#include "me_res.h"

#include <assert.h>

using namespace avej_lite;
using namespace manoeri;

////////////////////////////////////////////////////////////////////////////////
// forward

////////////////////////////////////////////////////////////////////////////////
// macro definition

#define MAX_PLAYER   6

////////////////////////////////////////////////////////////////////////////////
// constant definition

const CMapIterator::TArea TILE_AREA(BLOCK_SIZE, BLOCK_SIZE);
const CMapIterator::TArea CLIPPING_AREA(SCREEN_WIDTH, SCREEN_HEIGHT + BLOCK_SIZE);

////////////////////////////////////////////////////////////////////////////////
// type definition

typedef std::vector<CPlayer*>::iterator TIterPlayer;

////////////////////////////////////////////////////////////////////////////////
// global variables

namespace manoeri
{
	// Avej graphics instance
	IGfxDevice*  g_p_gfx_device   = NULL;
	// Graphics back buffer instance
	IGfxSurface* g_p_back_buffer  = NULL;
	// Graphics image instance
	IGfxSurface* g_p_tile_image   = NULL;

	bool g_on_process = true;

////////////////////////////////////////////////////////////////////////////////
// static variables

////////////////////////////////////////////////////////////////////////////////
// forward definition

////////////////////////////////////////////////////////////////////////////////
// class definition

////////////////////////////////////////////////////////////////////////////////
// main

class CGameMain
{
	int m_tick_count;

public:
	CGameMain(void)
		: m_tick_count(0)
	{
	}
	~CGameMain(void)
	{
	}
	bool Process(void)
	{
		{
			for (TIterPlayer obj = g_player_list.begin(); obj != g_player_list.end(); )
			{
				if ((*obj)->IsAlive())
				{
					++obj;
				}
				else
				{
					delete *obj;
					g_player_list.erase(obj);
				}
			
			}
		}

		if (!g_standby_list.empty())
		{
			for (TIterPlayer obj = g_standby_list.begin(); obj != g_standby_list.end(); ++obj)
				g_player_list.push_back(*obj);

			g_standby_list.clear();
		}

		std::sort(g_player_list.begin(), g_player_list.end(), CPlayer::Sort);

		{
			for (TIterPlayer obj = g_player_list.begin(); obj != g_player_list.end(); ++obj)
			{
				(*obj)->DoAction();
				(*obj)->m_face_add = (m_tick_count / 20) % (*obj)->m_face_inc;
			}
			++m_tick_count;
		}

		g_p_gfx_device->BeginDraw();

		const int g_x = g_p_main_player->m_pos.x;
		const int g_y = g_p_main_player->m_pos.y;

		const int X_OFFSET = - g_x + (SCREEN_WIDTH/2);
		const int Y_OFFSET = - g_y + (SCREEN_HEIGHT/2);

		const CMapIterator::TCoord OFFSET_COORD(X_OFFSET, Y_OFFSET);


		static CFosMap fos_map(BLOCK_SIZE, BLOCK_SIZE);

		if (fos_map.IsAvaliable())
			fos_map.Reset();
		else
			fos_map << *g_tile_map;

		{
			const int BLOCK_WSIZE = BLOCK_SIZE;
			const int BLOCK_HSIZE = BLOCK_SIZE;

			// Set the visibility on each grid
			{
				for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
				{
					const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

					fos_map.IsMyEyesReached(g_x, g_y, out_desc.map_index.x*BLOCK_WSIZE,               out_desc.map_index.y*BLOCK_HSIZE);
					fos_map.IsMyEyesReached(g_x, g_y, out_desc.map_index.x*BLOCK_WSIZE+BLOCK_WSIZE-1, out_desc.map_index.y*BLOCK_HSIZE);
					fos_map.IsMyEyesReached(g_x, g_y, out_desc.map_index.x*BLOCK_WSIZE,               out_desc.map_index.y*BLOCK_HSIZE+BLOCK_HSIZE-1);
					fos_map.IsMyEyesReached(g_x, g_y, out_desc.map_index.x*BLOCK_WSIZE+BLOCK_WSIZE-1, out_desc.map_index.y*BLOCK_HSIZE+BLOCK_HSIZE-1);
				}
			}

			// Recover the visibility of the corner
			{
				#define sign(a) ((a) == 0) ? 0 : ((a) > 0) ? 1 : -1;
				int _x = (g_x / BLOCK_WSIZE);
				int _y = (g_y / BLOCK_HSIZE);

				std::vector<int> visible_list;

				for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
				{
					const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

					int dx = sign(_x - out_desc.map_index.x);
					int dy = sign(_y - out_desc.map_index.y);

					if (!fos_map.IsVisible(out_desc.map_index.x, out_desc.map_index.y))
					if ( fos_map.IsBlocked(out_desc.map_index.x, out_desc.map_index.y))
					if ((dx) && (dy))
					{
						if (fos_map.IsVisibleAndBlocked(out_desc.map_index.x, out_desc.map_index.y+dy) && fos_map.IsVisibleAndBlocked(out_desc.map_index.x+dx, out_desc.map_index.y))
						{
							visible_list.push_back(out_desc.map_index.x);
							visible_list.push_back(out_desc.map_index.y);
						}
					}
				}

				for (std::vector<int>::iterator i = visible_list.begin(); i != visible_list.end();)
				{
					int x = *i++;
					int y = *i++;
					fos_map.SetVisibility(x, y);
				}
			}
		}

		g_p_back_buffer->SetActiveLayer(0);

		// top of the wall
		{
			int x_tile = 0;
			int y_tile = 8;

			for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
			{
				const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

				if (!fos_map.IsVisibleAndBlocked(out_desc.map_index.x, out_desc.map_index.y))
					continue;

				int tile = g_tile_map->GetTile(out_desc.map_index.x, out_desc.map_index.y)-1;

				g_p_back_buffer->BitBlt
				(
					out_desc.real_pos.x, out_desc.real_pos.y - BLOCK_SIZE, g_p_tile_image,
					(x_tile + tile % 10) * BLOCK_SIZE, (y_tile + tile / 10) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
				);
			}
		}

		g_p_back_buffer->SetActiveLayer(1);

		// player
		{
			for (std::vector<CPlayer*>::iterator obj = g_player_list.begin(); obj != g_player_list.end(); ++obj) 
				(*obj)->Display();
		}

		// bottom of the wall
		{
			for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
			{
				int x_tile = 6;
				int y_tile = 9;

				const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

				if (!fos_map.IsVisibleAndBlocked(out_desc.map_index.x, out_desc.map_index.y))
					continue;

				int tile = g_tile_map->GetTile(out_desc.map_index.x, out_desc.map_index.y) - 1;

				switch (g_tile_map->GetTile(out_desc.map_index.x+1, out_desc.map_index.y) - 1)
				{
				case 12:
				case 13:
				case 14:
				case 15:
					++x_tile;
				}

				g_p_back_buffer->BitBlt
				(
					out_desc.real_pos.x, out_desc.real_pos.y, g_p_tile_image,
					x_tile * BLOCK_SIZE, y_tile*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
				);
			}
		}

		g_p_back_buffer->SetActiveLayer(2);

		// shadow
		{
			CFosMap::TPoint vertices[4];

			for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
			{
				const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

				if (fos_map.IsVisibleAndBlocked(out_desc.map_index.x, out_desc.map_index.y))
				{
					if (fos_map.MakeFos(g_x, g_y, out_desc.map_index.x, out_desc.map_index.y, vertices))
					{
						IGfxSurface::TPoint<int> point[4];

						for (int i = 0; i < 4; i++)
						{
							point[i].x = vertices[i].x + X_OFFSET;
							point[i].y = vertices[i].y + Y_OFFSET;
						}

						g_p_back_buffer->FillRect(0xFF000000, point);
					}
				}
			}
		}

		g_p_back_buffer->SetActiveLayer(3);

		// floor
		{
			for (CMapIterator map_iterator(OFFSET_COORD, TILE_AREA, CLIPPING_AREA); !map_iterator.EndOfLoop(); ++map_iterator)
			{
				const CMapIterator::TDesc& out_desc = map_iterator.GetDesc();

				if (fos_map.IsVisible(out_desc.map_index.x, out_desc.map_index.y))
				if (!fos_map.IsBlocked(out_desc.map_index.x, out_desc.map_index.y))
				{
					int x_tile = 8;
					int y_tile = 7;
					int tile = g_tile_map->GetTile(out_desc.map_index.x, out_desc.map_index.y);

					switch (tile)
					{
					case 0:
						x_tile = 8;
						y_tile = 7;
						if (g_tile_map->IsWallMap(out_desc.map_index.x+1, out_desc.map_index.y))
							++x_tile;
						break;
					case 17:
						x_tile = 8;
						y_tile = 9;
						break;
					case 18:
						x_tile = 9;
						y_tile = 9;
						break;
					default:
						continue;
					}

					g_p_back_buffer->BitBlt
					(
						out_desc.real_pos.x, out_desc.real_pos.y, g_p_tile_image,
						x_tile*BLOCK_SIZE, y_tile*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
					);
				}
			}
		}

		g_p_gfx_device->EndDraw();
		g_p_gfx_device->Flip();
		return true;
	}
};

const char* BLOCK_MAP1[] =
{
	"+------------------+",
	"|        >      o  |",
	"|  ooo o        o  |",
	"|  o       o    o  |",
	"|    o     o       |",
	"|  o    o    ooo   |",
	"|  oo  oo  o       |",
	"|          o       |",
	"|     o    o       |",
	"|     o       oo   |",
	"|              o   |",
	"|                  |",
	"|   oo oo oooo   > |",
	"|          o       |",
	"|          o       |",
	"|   oo oo  o    o  |",
	"|               o  |",
	"|<    oo   o       |",
	"|     o            |",
	"+------------------+",
/*
	"+------------------+",
	"|             o    |",
	"|             o    |",
	"|     o          o |",
	"|          ooooooo |",
	"|          o       |",
	"|  oooooo  oooooo  |",
	"|ooo            o  |",
	"|  o            o  |",
	"|  o  ooo  ooo  o  |",
	"|  o  o      o  o  |",
	"|  o  o      o  o  |",
	"|  o  oooooooo  o  |",
	"|  o            o  |",
	"|  ooo      ooooo  |",
	"|    oo    oo      |",
	"|     oo   o  >    |",
	"|      oo  o       |",
	"|       o<<o       |",
	"+--------  --------+",*/
	0
};

static CGameMain* s_p_game_main = NULL;

static avej_lite::TLayerDesc s_layer_option =
{
	4,
	{
		avej_lite::TLayerDesc::LAYER_ATTRIB_OPAQUE, // top of the wall
		avej_lite::TLayerDesc::LAYER_ATTRIB_ALPHA_TEST, // player, bottom of the wall
		avej_lite::TLayerDesc::LAYER_ATTRIB_OPAQUE, // shadow
		avej_lite::TLayerDesc::LAYER_ATTRIB_OPAQUE, // floor
	},
	avej_lite::TLayerDesc::OPTION_DEPTH_TEST
};

static bool OnCreate(void)
{
	s_p_game_main  = new CGameMain;

	g_p_gfx_device = IGfxDevice::GetInstance();

	g_p_gfx_device->SetLayerDesc(s_layer_option);
	g_p_gfx_device->GetSurface(&g_p_back_buffer);

	// The resources loading
//	if (!g_p_gfx_device->CreateSurfaceFrom("./deja_dun.tga", &g_p_tile_image))
	{
		if (!g_p_gfx_device->CreateSurfaceFrom(p_res_img0, sizeof_p_res_img0(), &g_p_tile_image))
//		if (!g_p_gfx_device->CreateSurfaceFrom("bin/deja_dun.tga", &g_p_tile_image))
		{
			assert(0 && "cannot load the image.");
		}
	}

	g_p_main_player = CreateCharacter(PLAYERTYPE_MAIN, 24*10, 24*17);

	g_player_list.push_back(g_p_main_player);
	g_player_list.push_back(CreateCharacter(PLAYERTYPE_NONE, 180, 100));
	g_player_list.push_back(CreateCharacter(PLAYERTYPE_NONE,  40, 200));
	g_player_list.push_back(CreateCharacter(PLAYERTYPE_NONE, 440, 130));
	g_player_list.push_back(CreateCharacter(PLAYERTYPE_NONE, 24*10,  24*14));

//	g_player_list.push_back(CreateCharacter(PLAYERTYPE_DUMMY, 24*10, 24*16));
	g_player_list.push_back(CreateCharacter(PLAYERTYPE_DUMMY, 24*10, 24*15));
//	g_player_list.push_back(CreateCharacter(PLAYERTYPE_DUMMY, 24*10, 24*14));

	g_tile_map = new CTileMap(BLOCK_MAP1);
//	CScript script(CScript::MODE_MAP, 0);

	return true;
}

static bool OnDestory(void)
{
	delete g_tile_map;

	for (TIterPlayer obj = g_player_list.begin(); obj != g_player_list.end(); ++obj)
		delete (*obj);

	g_player_list.clear();

	delete g_p_tile_image;
	
	g_p_gfx_device->Release();

	delete s_p_game_main;

	return true;
}

static bool OnProcess(void)
{
	return s_p_game_main->Process();
}

} // namespace manoeri

////////////////////////////////////////////////////////////////////////////////
// main

#include "avejapp_register.h"

static IAvejApp* p_app = NULL;

static bool s_Initialize(void* h_window)
{
	AppCallback callBack =
	{
		OnCreate,
		OnDestory,
		OnProcess
	};

	p_app = IAvejApp::GetInstance(callBack);

	return (p_app != 0);
}

static void s_Finalize(void)
{
	p_app->Release();
	p_app = NULL;
}

static bool s_Process(void)
{
	return (p_app && g_on_process) ? p_app->Process() : false;
}

static bool GetAppCallback(TAppCallback& out_callback)
{
	out_callback.Initialize = s_Initialize;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

REGISTER_APP_1("ManoEri", GetAppCallback);

/*
#if (TARGET_DEVICE == TARGET_GP2XWIZ)
#include <unistd.h>
#endif

#if defined(_MSC_VER) || (TARGET_DEVICE == TARGET_GP2XWIZ)
int AvejMain(void)
#else
int _main(void)
#endif
{
	avej_init();

	while (avej_process())
		;

	avej_done();

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
*/