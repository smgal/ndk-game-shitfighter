
////////////////////////////////////////////////////////////////////////////////
// uses

#include "me_player.h"
#include "me_tile_map.h"

#include <stdlib.h>
#include <assert.h>
#include <math.h>

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// type definition

typedef std::vector<manoeri::CPlayer*>::iterator TIterPlayer;

////////////////////////////////////////////////////////////////////////////////
// macro definition

#define PI           3.141592

////////////////////////////////////////////////////////////////////////////////
// global variables

manoeri::CPlayer* g_p_main_player = NULL;

std::vector<manoeri::CPlayer*> g_standby_list;
std::vector<manoeri::CPlayer*> g_player_list;

////////////////////////////////////////////////////////////////////////////////
// 

namespace manoeri
{
	extern bool g_on_process;
}

namespace manoeri
{

const int BASE_MOVE = 1;

CPlayer* CreateCharacter(EPlayerType chara_type, int x_pos, int y_pos)
{
	CPlayer* p_player = NULL;

	switch (chara_type)
	{
	case PLAYERTYPE_MAIN:
		p_player = new CMainPlayer;
		p_player->m_h_texture = int(g_p_tile_image);
		p_player->m_pos.x     = x_pos;
		p_player->m_pos.y     = y_pos;
		p_player->m_face      = 1;
		p_player->m_face_dir  = 0;
		p_player->m_face_add  = 0;
		p_player->m_face_inc  = 2;
		p_player->m_space.x   = 8;
		p_player->m_space.y   = 8;
		p_player->m_move_inc  = 2 * BASE_MOVE;
		p_player->m_mass      = 60;
		break;
	case PLAYERTYPE_NONE:
		p_player = new CNonPlayer;
		p_player->m_h_texture = int(g_p_tile_image);
		p_player->m_pos.x     = x_pos;
		p_player->m_pos.y     = y_pos;
		p_player->m_face      = 3;
		p_player->m_face_dir  = 0;
		p_player->m_face_inc  = 2;
		p_player->m_face_add  = 0;
		p_player->m_space.x   = 8;
		p_player->m_space.y   = 8;
		p_player->m_move_inc  = 2 * BASE_MOVE;
		p_player->m_mass      = 40;
		break;
	case PLAYERTYPE_DUMMY:
		p_player = new CDummyPlayer;
		p_player->m_h_texture = int(g_p_tile_image);
		p_player->m_pos.x     = x_pos;
		p_player->m_pos.y     = y_pos;
		p_player->m_face      = 7;
		p_player->m_face_dir  = 0;
		p_player->m_face_inc  = 1;
		p_player->m_face_add  = 0;
		p_player->m_space.x   = 8;
		p_player->m_space.y   = 8;
		p_player->m_move_inc  = 1 * BASE_MOVE;
		p_player->m_mass      = 10;
		break;
	case PLAYERTYPE_FIRE:
		p_player = new CFirePlayer;
		p_player->m_h_texture = int(g_p_tile_image);
		p_player->m_pos.x     = x_pos;
		p_player->m_pos.y     = y_pos;
		p_player->m_face      = 7;
		p_player->m_face_dir  = 0;
		p_player->m_face_inc  = 1;
		p_player->m_face_add  = 0;
		p_player->m_space.x   = 8;
		p_player->m_space.y   = 8;
		p_player->m_move_inc  = 6 * BASE_MOVE;
		p_player->m_mass      = 0;
		break;
	default:
		assert(0);
	}

	p_player->m_type     = chara_type;
	p_player->m_is_alive = 1;

	return p_player;
}

CPlayer::~CPlayer()
{
}

void CPlayer::m_Display(int lighten)
{
	int x = m_pos.x - (BLOCK_SIZE/2) - g_p_main_player->m_pos.x + (SCREEN_WIDTH/2);
	int y = m_pos.y - (BLOCK_SIZE-m_space.y) - g_p_main_player->m_pos.y + (SCREEN_HEIGHT/2);
	g_p_back_buffer->BitBlt
	(
		x, y, (IGfxSurface*)m_h_texture,
		(m_face_dir*2+m_face_add) * BLOCK_SIZE, (m_face+0)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
	);
}

bool CPlayer::m_MoveTest(int x, int y)
{
	int i, j;

	i = (x - m_space.x) / BLOCK_SIZE;
	j = (y - m_space.y) / BLOCK_SIZE;
	if (!g_tile_map->IsMoveableMap(i, j))
		return false;

	i = (x + m_space.x) / BLOCK_SIZE;
	j = (y - m_space.y) / BLOCK_SIZE;
	if (!g_tile_map->IsMoveableMap(i, j))
		return false;

	i = (x + m_space.x) / BLOCK_SIZE;
	j = (y + m_space.y) / BLOCK_SIZE;
	if (!g_tile_map->IsMoveableMap(i, j))
		return false;

	i = (x - m_space.x) / BLOCK_SIZE;
	j = (y + m_space.y) / BLOCK_SIZE;
	if (!g_tile_map->IsMoveableMap(i, j))
		return false;

	return true;
}

bool CPlayer::IsAlive(void)
{
	return (m_is_alive > 0);
}

void CPlayer::Move(int dx, int dy, int force, bool is_self_control)
{
	if (is_self_control)
	{
		if (dy < 0)
			m_face_dir = 3;
		if (dy > 0)
			m_face_dir = 0;
		if (dx < 0)
			m_face_dir = 1;
		if (dx > 0)
			m_face_dir = 2;
	}

	force -= m_mass;

	if (force < 0)
		return;

	if ((dx != 0) || (dy != 0))
	{
		do
		{
			if (!m_MoveTest(m_pos.x + dx, m_pos.y + dy))
			{
				if ((dx == 0) || (dy == 0))
				{
					dx = 0;
					dy = 0;
					break;
				}

				if (!m_MoveTest(m_pos.x + dx, m_pos.y))
				{
					if (!m_MoveTest(m_pos.x, m_pos.y + dy))
					{
						dx = 0;
						dy = 0;
						break;
					}
					dx = 0;
					break;
				}
				dy = 0;
				break;
			}
		}
		while (false);

		if ((dx != 0) || (dy != 0))
		for (TIterPlayer obj = g_player_list.begin(); obj != g_player_list.end(); ++obj)
		{
			int nAttempt = 0;
			if (*obj != this)
			{
				while (1)
				{
					//?? 배타적인 공간을 차지해야 하는 타입에 대한 검사로 바꿔야 함
					if (m_type > 2)
						break;
					if ((*obj)->m_type > 2)
						break;

					if ((abs(m_pos.x+dx - (*obj)->m_pos.x) < (m_space.x + (*obj)->m_space.x)) &&
					   (abs(m_pos.y+dy - (*obj)->m_pos.y) < (m_space.y + (*obj)->m_space.y)))
					{
						if (nAttempt == 0)
						{
							(*obj)->Move(sign(dx), sign(dy), force, false);
							++nAttempt;
							continue;
						}
						
						dx = 0;
						dy = 0;
						return;
					}
					break;
				}
			}
		}

		m_pos.x += dx;
		m_pos.y += dy;
	}
}

void CPlayer::Display(int lighten)
{
	m_Display(lighten);
}

void CPlayer::DoAction(void)
{
}

bool CPlayer::Sort(const CPlayer* a, const CPlayer* b)
{
	return (a->m_pos.y > b->m_pos.y);
}

///////////////////////////////

CDummyPlayer::~CDummyPlayer()
{
}

bool CDummyPlayer::IsMyEyesReached(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	int x_dir = (dx < 0) ? -1 : 1;
	int y_dir = (dy < 0) ? -1 : 1;

	{
		if (x_dir < 0)
			dx = -dx;

		if (y_dir < 0)
			dy = -dy;

		bool qaz = (dx >= dy);

		if (!qaz)
		{
			dx ^= dy; dy ^= dx; dx ^= dy;
		}

		int dir       = 2 * dy - dx;
		int inc_to_R  = 2 * dy;
		int inc_to_UR = 2 * (dy - dx);
		int x         = x1;
		int y         = y1;

		int x_prev    = x / BLOCK_SIZE;
		int y_prev    = y / BLOCK_SIZE;

		int& major_axis = (qaz) ? x : y;
		int& minor_axis = (qaz) ? y : x;
		int& major_inc  = (qaz) ? x_dir : y_dir;
		int& minor_inc  = (qaz) ? y_dir : x_dir;

		while ((x != x2) || (y != y2))
		{
			major_axis += major_inc;

			if (dir <= 0)
				dir += inc_to_R;
			else
			{
				dir += inc_to_UR;
				minor_axis += minor_inc;
			}

			if ((x_prev != x / BLOCK_SIZE) || (y_prev != y / BLOCK_SIZE))
			{
				if (!g_tile_map->IsMoveableMap(x / BLOCK_SIZE, y / BLOCK_SIZE))
					return false;

				x_prev = x / BLOCK_SIZE;
				y_prev = y / BLOCK_SIZE;
			}
		}
	}

	return true;
}

bool CDummyPlayer::m_IsSeen(int x1, int y1, int x2, int y2, int& num_reached)
{
	double angle = atan2((double)y1 - (double)y2, (double)x1 - (double)x2);

	num_reached = 0;
	if (IsMyEyesReached(x1, y1, x2, y2))
		++num_reached;
	if (IsMyEyesReached(x1, y1, x2+int(cos(angle-PI/2)*m_space.x+0.5), y2+int(sin(angle-PI/2)*m_space.y+0.5)))
		++num_reached;
	if (IsMyEyesReached(x1, y1, x2+int(cos(angle+PI/2)*m_space.x+0.5), y2+int(sin(angle+PI/2)*m_space.y+0.5)))
		++num_reached;

	return (num_reached > 0);
}

void CDummyPlayer::m_Display(int lighten)
{
	if (lighten < 128)
		return;

	int x = m_pos.x - (BLOCK_SIZE/2) - g_p_main_player->m_pos.x + (SCREEN_WIDTH/2);
	int y = m_pos.y - (BLOCK_SIZE-m_space.y) - g_p_main_player->m_pos.y + (SCREEN_HEIGHT/2);

	g_p_back_buffer->BitBlt
	(
		x, y, (IGfxSurface*)m_h_texture,
		m_face_add*BLOCK_SIZE, m_face*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
	);
}
void CDummyPlayer::Display(int lighten)
{
	int num_reached;

	if (m_IsSeen(g_p_main_player->m_pos.x, g_p_main_player->m_pos.y, m_pos.x, m_pos.y, num_reached))
	{
		m_Display(num_reached * 255 / 3);
	}
}

//////////////////////////////

CNonPlayer::~CNonPlayer()
{
}

void CNonPlayer::m_Display(int lighten)
{
	CPlayer::m_Display(lighten);
}

void CNonPlayer::DoAction(void)
{
	Move(avej_lite::util::Random(3)-1, avej_lite::util::Random(3)-1, m_mass * 2, true);
}

//////////////////////////////

CFirePlayer::CFirePlayer()
	: m_life(100), m_vx(0), m_vy(0)
{
}
CFirePlayer::~CFirePlayer()
{
}

void CFirePlayer::m_Display(int lighten)
{
	int x_tile = 9;
	int y_tile = 2;
	g_p_back_buffer->BitBlt
	(
		m_pos.x - (BLOCK_SIZE/2), m_pos.y - (BLOCK_SIZE-m_space.y), (IGfxSurface*)m_h_texture,
		x_tile * BLOCK_SIZE, y_tile * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE
	);
}

void CFirePlayer::DoAction(void)
{
	if (m_life-- <= 0)
		m_is_alive = 0;

	Move(m_vx, m_vy, m_mass * 2, true);
}

void CFirePlayer::SetVelocity(int vx, int vy)
{
	m_vx = vx;
	m_vy = vy;
}

CMainPlayer::~CMainPlayer()
{
}

void CMainPlayer::DoAction(void)
{
	CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
	input_device.UpdateInputState();

	int dx = 0;
	int dy = 0;

	if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1))
	{
		g_on_process = false;
		return;
	}

	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_UP))
		dy = -m_move_inc;
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_DOWN))
		dy = m_move_inc;
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_LEFT))
		dx = -m_move_inc;
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_RIGHT))
		dx = m_move_inc;
/*
	if (input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_L1))
	{
		CFirePlayer* p_temp = (CFirePlayer*)CreateCharacter(PLAYERTYPE_FIRE, m_pos.x, m_pos.y);
		p_temp->SetVelocity(-6, 0);
		g_standby_list.push_back(p_temp);
	}
*/
	if ((dx != 0) || (dy != 0))
	{
		Move(dx, dy, m_mass * 2, true);
	}
}

} // namespace manoeri

//////////////////////////////
