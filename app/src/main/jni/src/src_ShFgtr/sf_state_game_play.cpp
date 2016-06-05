
////////////////////////////////////////////////////////////////////////////////
// uses

#include "sf_util.h"
#include "sf_map.h"
#include "sf_obj.h"
#include "sf_obj_friend.h"
#include "sf_main.h"
#include "sf_sys_desc.h"
#include "sf_gfx.h"
#include "sf_util.h"

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// callback

namespace game_play
{
	bool OnCreate(void)
	{
		switch (SetSysDesc().GetPlayerMode())
		{
		case PLAYERMODE_PLAYER1_ONLY:
		case PLAYERMODE_PLAYER2_ONLY:
			SetPlayerKeyType(PLAYERKEYTYPE_SINGLE);
			break;
		case PLAYERMODE_DOUBLE_MODE1:
		case PLAYERMODE_DOUBLE_MODE2:
			SetPlayerKeyType(PLAYERKEYTYPE_DOUBLE);
			break;
		}

		SetSysDesc().stage.has_cleared = true;
		SetSysDesc().stage.current_stage = 0;

		avej_lite::singleton<gui::CFloatMessage>::get();

		CStageMap& map = avej_lite::singleton<CStageMap>::get();

		map.SetCurrentMap(GetSysDesc().stage.current_stage);

		{
			tile_data[TILENAME_MIN] = 0;

			for (int i = TILENAME_MIN+1; i < TILENAME_MAX; i++)
			{
				TTile* p_temp = new TTile;

				p_temp->m_x_len    = g_sprite_rect[i].w;
				p_temp->m_y_len    = g_sprite_rect[i].h;
				p_temp->m_size     = g_sprite_rect[i].w * g_sprite_rect[i].h;
				p_temp->m_p_sprite = (TImage*)&g_sprite_rect[i];

				tile_data[i] = p_temp;
			}
		}

		{
			TTileName TN1 = ::TILENAME_MIN;
			TTileName TN2 = ::TILENAME_MIN;

			switch (GetSysDesc().GetPlayerMode())
			{
				case PLAYERMODE_PLAYER1_ONLY:
					TN1 = ::TILENAME_SMGAL1;
					break;
				case PLAYERMODE_PLAYER2_ONLY:
					TN1 = ::TILENAME_NETO1;
					break;
				case PLAYERMODE_DOUBLE_MODE1:
					TN1 = ::TILENAME_SMGAL1;
					TN2 = ::TILENAME_NETO1;
					break;
				case PLAYERMODE_DOUBLE_MODE2:
					TN1 = ::TILENAME_NETO1;
					TN2 = ::TILENAME_SMGAL1;
					break;
			}

			if (TN1 > TILENAME_MIN)
			{
				if ((TN1 == ::TILENAME_NETO1))
					objects[MIN_FRIEND] = new TFriendNeto(104, MIN_FRIEND);
				else
					objects[MIN_FRIEND] = new TFriendSMgal(104, MIN_FRIEND);

				objects[MIN_FRIEND]->Warp(50,100);
			}
			if (TN2 > TILENAME_MIN)
			{
				if ((TN2 == ::TILENAME_NETO1))
					objects[MIN_FRIEND+1] = new TFriendNeto(104, MAX_FRIEND);
				else
					objects[MIN_FRIEND+1] = new TFriendSMgal(104, MAX_FRIEND);

				objects[MIN_FRIEND+1]->Warp(100,10);
			}
		}

		return true;
	}

	bool OnDestory(void)
	{
		int i;

		for (i = 0; i <= MAX_ENEMY; i++)
		{
			delete objects[i];
			objects[i] = 0;
		}
		// TEnemy의 소멸자에서 생성된 object를 다시 제거
		for (i = 0; i <= MAX_ENEMY; i++)
		{
			delete objects[i];
			objects[i] = 0;
		}

		for (i = TILENAME_MIN+1; i < TILENAME_MAX; i++)
		{
			delete tile_data[i];
			tile_data[i] = NULL;
		}

		avej_lite::singleton<gui::CFloatMessage>::release();

		avej_lite::singleton<CStageMap>::release();

		return true;
	}

	bool OnProcess(void)
	{
		CStageMap& map = avej_lite::singleton<CStageMap>::get();

		if (GetSysDesc().stage.has_cleared)
		{
			SetSysDesc().stage.has_cleared = false;
			SetSysDesc().stage.current_stage = GetSysDesc().stage.current_stage + 1;

			if (GetSysDesc().stage.current_stage > 5)
			{
				g_ChangeState(STATE_ENDING_HAPPY);
				return false;
			}

			map.SetCurrentMap(GetSysDesc().stage.current_stage);

			SetSysDesc().stage.message.remained_ticks = SCROLL_DELAY_CALC(100);

			char stage_message[256];
			::util::ComposeString(stage_message, "STAGE @", GetSysDesc().stage.current_stage);
			// -> sprintf(stage_message, "STAGE %d", GetSysDesc().stage.current_stage);

			SetSysDesc().stage.message.Set(stage_message);
		}

		CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();
		input_device.UpdateInputState();

		{
			bool has_pressed_exit_key;
			
			if (GetSysDesc().GetPlayerMode() == PLAYERMODE_DOUBLE_MODE1 || GetSysDesc().GetPlayerMode() == PLAYERMODE_DOUBLE_MODE2)
				has_pressed_exit_key = input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_SYS1) && input_device.IsKeyHeldDown(avej_lite::INPUT_KEY_SYS2);
			else
				has_pressed_exit_key = input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1);

			if (has_pressed_exit_key)
			{
				if ((assigned_(objects[MIN_FRIEND]) && objects[MIN_FRIEND]->IsValid()) ||
					(assigned_(objects[MAX_FRIEND]) && objects[MAX_FRIEND]->IsValid()))
					g_ChangeState(STATE_EXIT);
				else
					g_ChangeState(STATE_ENDING_BAD);

				return false;
			}
		}

		// process objects
		map.Move(SCROLL_SPEED, 0);

		{
			IGfxDevice::TDeviceDesc screenDesc;

			g_p_gfx_device->GetDesc(screenDesc);

			g_p_gfx_device->BeginDraw();

			// clear the screen
			g_p_back_buffer->BitBlt(0, 0, g_p_res_sprite, 0, 512/2, 640/2, 216);

			// display background
			for (int j = 0; j <= 7; j++)
			for (int i = 1-1; i <= 12+1; i++)
			{
				int x_offset = map.GetXOffset();
				int y_offset = map.GetYOffset();

				if (map.GetTile(i*TILE_X_SIZE-x_offset, j*TILE_Y_SIZE-y_offset) > 0)
				{
//					gfx::DrawImage(i*TILE_X_SIZE-x_offset-1, j*TILE_Y_SIZE-y_offset+Y_DOWN,tile_data[TILENAME_TILE_GROUND]->m_p_sprite);
					gfx::DrawImage(i*TILE_X_SIZE-x_offset, j*TILE_Y_SIZE-y_offset+Y_DOWN+1,tile_data[TILENAME_TILE_GROUND]->m_p_sprite);
				}
			}

			for (int i = MAX_ENEMY; i >= MIN_OBJECTS; i--)
			{
				if (assigned_(objects[i]))
				{
					if (objects[i]->ProcessByAI())
					{
						if (objects[i]->IsValid())
						{
							objects[i]->ProcessByGravity();
							objects[i]->Display();
						}
						else
						{
							delete objects[i];
							objects[i] = NULL;
						}
					}
					else
					{
						delete objects[i];
						objects[i] = NULL;
					}
				}
			}

			{
				gui::CFloatMessage& message = avej_lite::singleton<gui::CFloatMessage>::get();
				message.Show();
			}

			gfx::DrawImage(0, 0,tile_data[TILENAME_TILE_STATUS_BAR]->m_p_sprite);

			{
				int i = 0;
				int j = 0;
				char s1[64] = {0, };
				char s2[64] = {0, };
				int number1 = 0;
				int number2 = 0;

				if (assigned_(objects[MIN_FRIEND]))
				{
					TFriend* p_friend = (TFriend*)objects[MIN_FRIEND];
					i = p_friend->m_hit_point;
					number1 = p_friend->m_special_weapon;
					::util::ComposeString(s1, "@", p_friend->GetLevel());
					// -> sprintf(s1, "%d", p_friend->GetLevel());
				}
				if (assigned_(objects[MAX_FRIEND]))
				{
					TFriend* p_friend = (TFriend*)objects[MAX_FRIEND];
					j = p_friend->m_hit_point;
					number2 = p_friend->m_special_weapon;
					::util::ComposeString(s2, "@", p_friend->GetLevel());
					// -> sprintf(s2, "%d", p_friend->GetLevel());
				}

				switch (GetSysDesc().GetPlayerMode())
				{
					case PLAYERMODE_PLAYER1_ONLY:
						i = 104 + i;
						j = 424;
						break;
					case PLAYERMODE_PLAYER2_ONLY:
						j = 424 + i;
						i = 104;
						strcpy(s2, s1);
						strcpy(s1, "");
						number2 = number1;
						number1 = 0;
						break;
					case PLAYERMODE_DOUBLE_MODE1:
						i = 104 + i;
						j = 424 + j;
						break;
					case PLAYERMODE_DOUBLE_MODE2:
						{
							int k;
							k = i; i = 104 + j; j = 424 + k;
							k = number1; number1 = number2; number2 = k;

							char s3[64];
							strcpy(s3, s2);
							strcpy(s2, s1);
							strcpy(s1, s3);
						}
						break;
				}

				if (i < 208)
					gfx::FillRect(0xFF000000, i, 10, 208-i, 6);
				if (j < 528)
					gfx::FillRect(0xFF000000, j, 10, 528-j, 6);

				if (s1[0])
				{
					gfx_ix::DrawText(273,4,s1,TIndexColor(3));
					gfx_ix::DrawText(272,3,s1,TIndexColor(4));
				}
				if (s2[0])
				{
					gfx_ix::DrawText(593,4,s2,TIndexColor(3));
					gfx_ix::DrawText(592,3,s2,TIndexColor(4));
				}
				if (number1 > 0)
				{
					if (number1 > 10) number1 = 10;
					for (int i = 0; i < number1; i++)
					{
						gfx::DrawSprite(i*24+TILE_X_SIZE,30,tile_data[TILENAME_SMGAL_SP]->m_p_sprite,true);
					}
				}
				if (number2 > 0)
				{
					if (number2 > 10) number2 = 10;
					for (int i = 0; i < number2; i++)
					{
						gfx::DrawSprite(i*24+TILE_X_SIZE+330,36,tile_data[TILENAME_NETO_SP]->m_p_sprite,true);
					}
				}
			}

			if (GetSysDesc().stage.message.remained_ticks > 0)
			{
				SetSysDesc().stage.message.remained_ticks = GetSysDesc().stage.message.remained_ticks - 1;
				int i = (640/24 - strlen(GetSysDesc().stage.message.Get())) / 2;
				gfx_ix::DrawText_x2(i*12, 85, GetSysDesc().stage.message.Get(), TIndexColor(11));
			}

			// flip the screen
			g_p_gfx_device->EndDraw();
			g_p_gfx_device->Flip();
		}

		if (map.IsEndOfMap())
		{
			int k = 0;
			for (int i = MIN_ENEMY; i <= MAX_ENEMY; i++)
			{
				if (assigned_(objects[i]))
					k++;
			}
			if (k == 0)
			{
				SetSysDesc().stage.has_cleared = true;
//				ascii_code  = ascii_code + [_ESC_KEY];
			}
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
