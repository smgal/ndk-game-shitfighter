
#include "sf_res.h"
#include "sf_sys_desc.h"

TTile* tile_data[TILENAME_MAX];

TTile::TTile()
:	m_x_len(0), m_y_len(0), m_size(0), m_p_sprite(0)
{
}

TTile::TTile(int x1, int y1, int x2, int y2)
:	m_x_len(0), m_y_len(0), m_size(0), m_p_sprite(0)
{
}

TTile::TTile(TTileName number)
{
    m_x_len    = tile_data[number]->m_x_len;
    m_y_len    = tile_data[number]->m_y_len;
    m_size     = tile_data[number]->m_size;
    m_p_sprite = tile_data[number]->m_p_sprite;
}

TTile::~TTile()
{
}

////////////////////////////////////////////////////////////////////////////////
//

static const char* s_current_res_string[RESSTRING_MAX] = { "@", };

static bool s_UpdateResString(void)
{
	switch (GetSysDesc().language)
	{
	case LANGUAGE_KOREAN:
		s_current_res_string[RESSTRING_TITLE      ] = "초 극상 / 궁극 변태 / 대변 파이터";
		s_current_res_string[RESSTRING_TITLE_MENU1] = "이 게임은 뭐지 ???";
		s_current_res_string[RESSTRING_TITLE_MENU2] = "스토리를 알고 싶군";
		s_current_res_string[RESSTRING_TITLE_MENU3] = "게임을 시작해 볼까";
		s_current_res_string[RESSTRING_TITLE_MENU4] = "옵션도 조정해 보자";
		break;
	case LANGUAGE_ENGLISH:
		s_current_res_string[RESSTRING_TITLE      ] = "Superb / Ultra Pervert / Shit Fighter";
		s_current_res_string[RESSTRING_TITLE_MENU1] = "What is this???";
		s_current_res_string[RESSTRING_TITLE_MENU2] = "I wanna know the story of this game";
		s_current_res_string[RESSTRING_TITLE_MENU3] = "Let's go into the game";
		s_current_res_string[RESSTRING_TITLE_MENU4] = "Is there options";
		break;
	default:
		return false;
	}

	return true;
}

void UpdateResString(void)
{
	s_UpdateResString();
}

const char* GetResString(TResString res_string)
{
	if (s_current_res_string[0][0] == '@')
	{
		if (!s_UpdateResString())
			return "";
	}

	return s_current_res_string[res_string];
}
