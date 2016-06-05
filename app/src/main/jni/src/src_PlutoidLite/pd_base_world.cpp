
#include "pd_config.h"
#include "pd_actor.h"
#include "pd_base_world.h"
#include "pd_class_player.h"
#include "pd_data_communication.h"
#include <assert.h>

namespace
{
	static int s_is_first = true;

	////////////////////////////////////////////////////////////////////////////
	// distance table 관련

	// grid의 단위는 10, 전체 크기는 300*300
	const unsigned int DISTANCE_TABLE_RES = 10;
	const unsigned int DISTANCE_TABLE_MAX = 15;

	// 이 값은 DISTANCE_TABLE_RES과 DISTANCE_TABLE_MAX이 바뀌면 수정되어야 함
	const unsigned int DISTANCE_NO_WORTH  = 200;

	static int s_distance_table[DISTANCE_TABLE_MAX][DISTANCE_TABLE_MAX];
	static int s_distance_angle_min[DISTANCE_TABLE_MAX][DISTANCE_TABLE_MAX];
	static int s_distance_angle_max[DISTANCE_TABLE_MAX][DISTANCE_TABLE_MAX];

	// 대화 가능한 최대 거리
	const unsigned int DISTANCE_TALKABLE  = 80;

	static iu::shared_ptr<class CPlayer> s_main_chara;
	static const iu::shared_ptr<class CPlayer>* s_focused_chara = 0;

	////////////////////////////////////////////////////////////////////////////
	// approach table 관련

	static int s_approach_step = 0;

	static void s_SetApproachStep(int step)
	{
		if (step < 0)
			step = 0;
		if (step > APPROACH_STEP_MAX)
			step = APPROACH_STEP_MAX;

		s_approach_step = step;
	}

	static int s_GetApproachStep(void)
	{
		return s_approach_step;
	}

	void s_MakeNecessaryTables()
	{
		assert(s_is_first);

		// 거리 테이블 만들기
		{
			for (int y = 0; y < 15; y++)
			for (int x = 0; x < 15; x++)
			{
				//?? 이것도 미리 계산 가능함
				s_distance_table[x][y] = int(sqrtf(x * x + y * y) * DISTANCE_TABLE_RES + 0.5f);

				int x1 = x  * DISTANCE_TABLE_RES - DISTANCE_TABLE_RES / 2;
				int x2 = x1 + DISTANCE_TABLE_RES;
				int y1 = y  * DISTANCE_TABLE_RES - DISTANCE_TABLE_RES / 2;
				int y2 = y1 + DISTANCE_TABLE_RES;

				if ((x == 0 && y == 0))
				{
					s_distance_angle_min[x][y] = 0;
					s_distance_angle_max[x][y] = 359;
				}
				else if (x == 0)
				{
					s_distance_angle_min[x][y] = int(atan2f(y1, x2) * 180 / 3.141592f + 0.5f);
					s_distance_angle_max[x][y] = int(atan2f(y1, x1) * 180 / 3.141592f + 0.5f);
				}
				else if (y == 0)
				{
					s_distance_angle_min[x][y] = int(atan2f(y1, x1) * 180 / 3.141592f + 0.5f);
					s_distance_angle_max[x][y] = int(atan2f(y2, x1) * 180 / 3.141592f + 0.5f);
				}
				else
				{
					s_distance_angle_min[x][y] = int(atan2f(y1, x2) * 180 / 3.141592f + 0.5f);
					s_distance_angle_max[x][y] = int(atan2f(y2, x1) * 180 / 3.141592f + 0.5f);
				}
			}
		}

		s_is_first = false;
	}

	void s_AnalyzePhenomenon(void)
	{
		TPosition albireos_view = TWorld::GetMainChara()->GetDesc().position;

		{
			const TWorld& ref_world = avej_lite::singleton<TWorld>::get();

			action::get<const TPlayerList*> get_player;
			ref_world >> get_player;

			TPlayerList::const_iterator iter_begin = get_player.data->begin();
			TPlayerList::const_iterator iter_end   = get_player.data->end();
			TPlayerList::const_iterator iter       = iter_begin;

			// 가장 가까이에 있는 유효한 캐릭터 찾기
			struct
			{
				const iu::shared_ptr<class CPlayer>* player;
				int                                    distance;
			} nearest = { 0, DISTANCE_NO_WORTH };

			int step = -1000000;

			for (; (iter != iter_end); ++iter)
			{
				// main character임
				if (iter == iter_begin)
					continue;

				const TPlayerDesc& player_desc = (*iter)->GetDesc();

				int x_abs_distance = abs(albireos_view.x - player_desc.position.x) / DISTANCE_TABLE_RES;
				int y_abs_distance = abs(albireos_view.y - player_desc.position.y) / DISTANCE_TABLE_RES;

				if (max(x_abs_distance, y_abs_distance) < int(DISTANCE_TABLE_MAX))
				{
					int distance = s_distance_table[x_abs_distance][y_abs_distance];
					int revert_distance = 150 - distance;

					//?? 원래는 main character의 conflict과 xor을 해야 하지만.. 그냥 
					// conflict인 것일 때만 step 갱신
					if (player_desc.trait_flag & PLAYER_TRAIT_CONFLICT)
					{
						if (step < revert_distance)
							step = revert_distance;
					}

					// 거리와 각도 기록
					{
						int x_sign = sign(player_desc.position.x - albireos_view.x);
						int y_sign = sign(player_desc.position.y - albireos_view.y);

						int angle_min;
						int angle_max;

						if (x_sign * y_sign >= 0)
						{
							if ((x_sign >= 0) && (y_sign > 0))
							{
								// 1사분면
								angle_min = s_distance_angle_min[x_abs_distance][y_abs_distance];
								angle_max = s_distance_angle_max[x_abs_distance][y_abs_distance];
							}
							else
							{
								// 3사분면
								angle_min = s_distance_angle_min[x_abs_distance][y_abs_distance] + 180;
								angle_max = s_distance_angle_max[x_abs_distance][y_abs_distance] + 180;
							}
						}
						else
						{
							if ((x_sign < 0) && (y_sign >= 0))
							{
								// 2사분면
								angle_min = s_distance_angle_min[y_abs_distance][x_abs_distance] + 90;
								angle_max = s_distance_angle_max[y_abs_distance][x_abs_distance] + 90;
							}
							else
							{
								// 4사분면
								angle_min = s_distance_angle_min[y_abs_distance][x_abs_distance] + 270;
								angle_max = s_distance_angle_max[y_abs_distance][x_abs_distance] + 270;
							}
						}

						while (angle_min < 0)
						{
							angle_min += 360;
							angle_max += 360;
						}

						assert(angle_min <= angle_max);

						*(*iter) << TDistance(distance) << TAngleRange(angle_min, angle_max);
					}

					// 가장 가까운 캐릭터인지 판별
					if (nearest.distance > distance)
					{
						if ((player_desc.trait_flag & PLAYER_TRAIT_TALKABLE) && (distance < int(DISTANCE_TALKABLE)))
						{
							nearest.distance = distance;
							nearest.player   = &(*iter);
						}
					}
				}
				else
				{
					*(*iter) << TDistance(DISTANCE_NO_WORTH);
				}

			}

			s_SetApproachStep(step);

			// 가장 가까운 캐릭터 기록
			s_focused_chara = nearest.player;
		}
	}
}

// Caina Crena를 위해 약속되어진 통로
void TWorld_AnalyzePhenomenon()
{
	s_AnalyzePhenomenon();
}

struct TWorldData
{
	TActorList       actor_list;
	TPlayerList      player_list;

	// geometry
	CSimpleTileMap*  p_ref_tile_map;

	//?? 이것도 s_main_chara의 위치로 가거나, 그것이 이 위치로 와야 한다.
	iu::shared_ptr<CPlayer> noticed_chara;

	// 초기에는 default map를 대입해서, p_ref_tile_map 이 NULL인 경우는 없어야 한다.
	TWorldData()
		: p_ref_tile_map(0) {}
};

static TWorldData* s_p_world_data = 0;

TWorld::TWorld()
{
	assert(s_p_world_data == 0);

	s_p_world_data = new TWorldData;

	//?? 임시로 actor 넣기
	iu::shared_ptr<CActor> actor(new CActor);
	actor->SetName("cube");

	s_p_world_data->actor_list.push_back(actor);

	CFace* p_face = 0;
	CFace* p_face_skeleton = 0;

	switch (CONFIG_TYPE)
	{
	case CONFIG_TYPE_DREAM:
		{
			p_face          = new CFace(30);
			p_face_skeleton = new CFace(31);
		}
		break;
	case CONFIG_TYPE_KANO:
		{
			int face_list[] = { 30, 31, 32, 33, 34,-30,-31,-32,-33,-34, 35, 36, 37, 38, 39,-35,-36,-37,-38,-39 };

			p_face = new CFace(face_list, sizeof(face_list) / sizeof(face_list[0]));
			
			if (p_face)
			{
				p_face->SetFace(-1, +1, "0", "1234");
				p_face->SetFace( 0, +1, "0", "1234");
				p_face->SetFace(+1, +1, "5", "6789");
				p_face->SetFace(+1,  0, "5", "6789");
				p_face->SetFace(-1,  0, "A", "BCDE");
				p_face->SetFace(-1, -1, "A", "BCDE");
				p_face->SetFace( 0, -1, "F", "GHIJ");
				p_face->SetFace(+1, -1, "F", "GHIJ");
			}

			int face_list_skeleton[] = { 50, 51, 52, 53, 54,-50,-51,-52,-53,-54, 55, 56, 57, 58, 59,-55,-56,-57,-58,-59 };

			p_face_skeleton = new CFace(face_list_skeleton, sizeof(face_list_skeleton) / sizeof(face_list_skeleton[0]));

			if (p_face_skeleton)
			{
				p_face_skeleton->SetFace(-1, +1, "0", "1234");
				p_face_skeleton->SetFace( 0, +1, "0", "1234");
				p_face_skeleton->SetFace(+1, +1, "5", "6789");
				p_face_skeleton->SetFace(+1,  0, "5", "6789");
				p_face_skeleton->SetFace(-1,  0, "A", "BCDE");
				p_face_skeleton->SetFace(-1, -1, "A", "BCDE");
				p_face_skeleton->SetFace( 0, -1, "F", "GHIJ");
				p_face_skeleton->SetFace(+1, -1, "F", "GHIJ");
			}
		}
		break;
	}

	if (p_face == 0 || p_face_skeleton == 0)
	{
		assert(0);
		return;
	}

	iu::shared_ptr<CFace> face_auto_del_1(p_face);
	iu::shared_ptr<CFace> face_auto_del_2(p_face_skeleton);

	CFace& face = *p_face;
	CFace& face_skeleton = *p_face_skeleton;

	//?? 임시로 player 넣기
	{
		iu::shared_ptr<CPlayer> player(new CMainPlayer);

		*player << "슴갈" << TPlayerDesc(TPosition(140, 60), face);

		s_p_world_data->player_list.push_back(player);
	}

	//?? 두 번 째 플레이어
	{
		iu::shared_ptr<CPlayer> player(new CNpcPlayerWanderer);

		*player << "Orz" << TPlayerDesc(TPosition(200, 100), face_skeleton);

		s_p_world_data->player_list.push_back(player);
	}

	//?? 추가 NPC
	{
		iu::shared_ptr<CPlayer> player(new CNpcPlayerWanderer);
		*player << "Orz" << TPlayerDesc(TPosition(450, 200), face_skeleton);
		s_p_world_data->player_list.push_back(player);
	}
	{
		iu::shared_ptr<CPlayer> player(new CNpcPlayerWanderer);
		*player << "Orz" << TPlayerDesc(TPosition(112, 640), face_skeleton);
		s_p_world_data->player_list.push_back(player);
	}
	{
		iu::shared_ptr<CPlayer> player(new CNpcPlayerWanderer);
		*player << "Orz" << TPlayerDesc(TPosition(450, 300), face_skeleton);
		s_p_world_data->player_list.push_back(player);
	}
	{
		iu::shared_ptr<CPlayer> player(new CNpcPlayerWanderer);
		*player << "Orz" << TPlayerDesc(TPosition(316, 642), face_skeleton);
		s_p_world_data->player_list.push_back(player);
	}

	//?? 세 번 째 플레이어
	{
		iu::shared_ptr<CPlayer> player(new CFloater);
     
		*player << "Alien00" << TPlayerDesc(TPosition(422, 224), face_skeleton);

		s_p_world_data->player_list.push_back(player);
	}

	//?? 네 번 째 플레이어
	{
		iu::shared_ptr<CPlayer> player(new CFloater);
     
		*player << "Alien01" << TPlayerDesc(TPosition(112, 640), face_skeleton);

		s_p_world_data->player_list.push_back(player);
	}

	s_main_chara = *s_p_world_data->player_list.begin();
	s_p_world_data->noticed_chara = s_main_chara;

	s_MakeNecessaryTables();
}

TWorld::~TWorld()
{
	delete s_p_world_data;
	s_p_world_data = 0;
}

TWorld& TWorld::operator<<(action::set<CSimpleTileMap*> set)
{
	assert(s_p_world_data);

	s_p_world_data->p_ref_tile_map = set.data;

	return *this;
}

void TWorld::operator>>(action::get<CSimpleTileMap*>& get) const
{
	assert(s_p_world_data);

	get.data = s_p_world_data->p_ref_tile_map;
}

TWorld& TWorld::operator<<(action::set<TTryToCommunicate> set)
{
	assert(s_p_world_data);

	if (set.data.p_to)
	{
		TPlayerList::iterator iter_begin = s_p_world_data->player_list.begin();
		TPlayerList::iterator iter_end   = s_p_world_data->player_list.end();
		TPlayerList::iterator iter       = iter_begin;

		for (; iter != iter_end; ++iter)
		{
			// 동일인인가?
			if ((*iter).get() == set.data.p_to)
			{
				s_p_world_data->noticed_chara = (*iter);

				// communication 등록
				TCommunication& communication = avej_lite::singleton<TCommunication>::get();

				communication << action::set<TCommunication::TCommunicationData>(TCommunication::TCommunicationData(TCommunication::TRY_TO_TALK, set.data.p_from, set.data.p_to));

				break;
			}
		}

		// 만약 여기에 걸리면 player list에 없는 player가 들어 왔다는 의미가 된다.
		assert(iter != iter_end);
	}
	else
	{
		s_p_world_data->noticed_chara = *(s_p_world_data->player_list.begin());
	}
	
	return *this;
}

void TWorld::operator>>(action::get<const TActorList*>& get) const
{
	assert(s_p_world_data);

	// Abunai code
	get.data = &s_p_world_data->actor_list;
}

void TWorld::operator>>(action::get<const TPlayerList*>& get) const
{
	assert(s_p_world_data);

	// Abunai code
	get.data = &s_p_world_data->player_list;
}

bool TWorld::CanIGoThere(TPosition origin, int x, int y, TExtent occupied_size)
{
	if (s_p_world_data == 0)
		return false;

	if (s_p_world_data->p_ref_tile_map == 0)
		return false;

	//?? 이러면 안되는데...
	int x1 = x;// - occupied_size.w / 2;
	int y1 = y;// - occupied_size.h / 2;
	//?? 원래는 0.00001을 빼야 한다.
	int x2 = x1 + occupied_size.w - 1;
	int y2 = y1 + occupied_size.h - 1;

	// normalize
	//?? world의 도량법을 받아서 넣어야 함
	x1 /= 28;
	x2 /= 28;
	y1 /= 32;
	y2 /= 32;

	// 지형과의 충돌 검사
	if (s_p_world_data->p_ref_tile_map->IsBlocked(x1, y1) ||
	    s_p_world_data->p_ref_tile_map->IsBlocked(x2, y1) ||
	    s_p_world_data->p_ref_tile_map->IsBlocked(x1, y2) ||
	    s_p_world_data->p_ref_tile_map->IsBlocked(x2, y2))
		return false;

	// player간의 충돌 검사
	TPlayerList::iterator iter_begin = s_p_world_data->player_list.begin();
	TPlayerList::iterator iter_end   = s_p_world_data->player_list.end();

	for (TPlayerList::iterator iter = iter_begin; iter != iter_end; ++iter)
	{
		const TPlayerDesc& player_desc = (*iter)->GetDesc();

		// 물질로 존재하지 않을 때는 통과
		if ((player_desc.trait_flag & PLAYER_TRAIT_HAS_MASS) == 0)
			continue;

		// 자기 자신인지 검사
		if ((player_desc.position.x == origin.x) && (player_desc.position.y == origin.y))
			continue;

		// 크기에 대한 절대 값 비교
		//?? 현재는 모두 크기가 같다고 보지만, 서로 크기가 다를 수 있으므로 그때가 되면 수정되어야 함. (my_occupied_size.w / 2 + your_occupied_size.w / 2)가 됨
		if ((abs(player_desc.position.x - x) < occupied_size.w) && (abs(player_desc.position.y - y) < occupied_size.h))
			return false;
	}

	return true;
}

int  TWorld::GetApproachStep(void)
{
	return s_GetApproachStep();
}

iu::shared_ptr<class CPlayer> TWorld::GetMainChara(void)
{
	return s_main_chara;
}

iu::shared_ptr<class CPlayer> TWorld::GetFocusedChara(void)
{
	if (s_focused_chara != 0)
	{
		return *s_focused_chara;
	}
	else
	{
		iu::shared_ptr<CPlayer> null_chara(0);
		return null_chara;
	}
}

iu::shared_ptr<class CPlayer> TWorld::GetNoticedChara(void)
{
	if (s_p_world_data && s_p_world_data->noticed_chara.get())
	{
		return s_p_world_data->noticed_chara;
	}
	else
	{
		iu::shared_ptr<CPlayer> null_chara(0);
		return null_chara;
	}
}
