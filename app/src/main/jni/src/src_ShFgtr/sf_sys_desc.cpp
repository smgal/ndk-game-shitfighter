
#include "sf_sys_desc.h"

const TSysDesc& GetSysDesc(void)
{
	return avej_lite::singleton<TSysDesc>::get();
}

TSysDesc& SetSysDesc(void)
{
	return avej_lite::singleton<TSysDesc>::get();
}

////////////////////////////////////////////////////////////////////////////////
//

TSysDesc::TSysDesc()
: language(LANGUAGE_KOREAN), use_sound(true)
{
}

TSysDesc::~TSysDesc()
{
}

TSysDesc::TStage::TMesaage::TMesaage()
	: remained_ticks(0)
{
	stage_message[0] = 0;
}

const char* TSysDesc::TStage::TMesaage::Get(void) const
{
	return stage_message;
}

void TSysDesc::TStage::TMesaage::Set(const char* sz_message)
{
	strncpy(stage_message, sz_message, sizeof(stage_message));
}


static TPlayerMode s_player_mode = PLAYERMODE_DOUBLE_MODE1;
static bool s_auto_shot = true;

TPlayerMode TSysDesc::GetPlayerMode(void) const
{
	return s_player_mode;
}

void TSysDesc::SetPlayerMode(TPlayerMode player_mode)
{
	s_player_mode = player_mode;

	switch (player_mode)
	{
		case PLAYERMODE_PLAYER1_ONLY:
		case PLAYERMODE_PLAYER2_ONLY:
			s_auto_shot = false;
			break;
		case PLAYERMODE_DOUBLE_MODE1:
		case PLAYERMODE_DOUBLE_MODE2:
			s_auto_shot = true;
			break;
	}
}

bool TSysDesc::IsAutoShot(void) const
{
	return s_auto_shot;
}
