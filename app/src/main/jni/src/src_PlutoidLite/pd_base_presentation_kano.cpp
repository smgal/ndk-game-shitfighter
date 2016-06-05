
#include "pd_config.h"

#include "pd_actor_albireo.h"
#include "pd_base_presentation_base.h"
#include "pd_visible_actor.h"
#include "pd_class_player.h"

#include "util_fos_map.h"
#include "util_render_text.h"

#include "gfx3d_device3d.h"
//#include "gfx3d_scene.h"

//#include "util/avej_util.h"
//#include "image_decoder/avej_image_decoder.h"
//#include "device/avej_device_input.h"

#include <assert.h>
#include <algorithm>

extern void s_ComposeString(char pBuffer[], const char* szFormat, const int data...);

using namespace avej;
using namespace avej::gfx3d;

//?? config의 값을 복사한 것임. config 값의 변화에 대응이 필요 함
#define _X_MAP_RADIUS  7
#define _Y_MAP_RADIUS  5

#define NUM_OF_VERTEX  ((_X_MAP_RADIUS*2+1)*(_Y_MAP_RADIUS*2+1))

namespace
{
	class CVisibleGeometry: public CVisible3D
	{
		class CVisibleGeometrySub: public CVisible<TShared3DObject>
		{
		public:
			CVisibleGeometrySub(shared::CVertexBuffer vertex_buffer, const shared::TTexTileList tex_tile, TShared3DObject renderable_object, FnShow fn_show)
			:	CVisible<TShared3DObject>(renderable_object, fn_show),
				m_vertex_buffer(vertex_buffer),
				m_ref_tex_tile(tex_tile),
				m_vertex_object(renderable_object)
				
			{
			}

		protected:
			virtual void _Show(TShared3DObject renderable_object) const
			{
				m_Act();

				CVisible<TShared3DObject>::_Show(renderable_object);
			}

		private:
			shared::CVertexBuffer       m_vertex_buffer;
			const shared::TTexTileList  m_ref_tex_tile;
			TShared3DObject             m_vertex_object;

			void m_Act() const
			{
				action::get<CSimpleTileMap*> get_geometry;
				avej_lite::singleton<TWorld>::get() >> get_geometry;

				if (get_geometry.data == 0)
					return;

				TPosition albireos_view = avej_lite::singleton<CAlbireo>::get().GetBeholdingPosition();

				CSimpleTileMap& world_map = *get_geometry.data;

				TFixedVec2 grid_tex_coord[NUM_OF_VERTEX][6];
				{
					int x_off = albireos_view.x / X_TILE_SIZE;
					int y_off = albireos_view.y / Y_TILE_SIZE;

					for (int y = -Y_MAP_RADIUS; y <= Y_MAP_RADIUS; y++)
					for (int x = -X_MAP_RADIUS; x <= X_MAP_RADIUS; x++)
					{
						int ix_tile = world_map.GetTile(x+x_off, y+y_off);
						int index = (y+Y_MAP_RADIUS)*X_MAP_STRIDE+(x+X_MAP_RADIUS);
						memcpy(grid_tex_coord[index], m_ref_tex_tile->tile_info[ix_tile].tex_coord, sizeof(m_ref_tex_tile->tile_info[ix_tile].tex_coord));
					}
				}

				*m_vertex_buffer << TTexCoordData(&grid_tex_coord[0][0]);

				m_vertex_object->SetXY(-albireos_view.x % X_TILE_SIZE, -albireos_view.y % Y_TILE_SIZE);
				m_vertex_object->SetColor(CAlbireo::GetCurrentDreamColor());
			}
		};

	public:
		CVisibleGeometry(const shared::TTexTileList tex_tile)
			: m_ref_tex_tile(tex_tile)
		{
			TFixedVec3 vertices[NUM_OF_VERTEX][6];
			TFixedVec3 normals[NUM_OF_VERTEX][6];

			const TFixedVec3 default_normal(_FXD(0), _FXD(0), _FXD(1));

			for (int y = -Y_MAP_RADIUS; y <= Y_MAP_RADIUS; y++)
			for (int x = -X_MAP_RADIUS; x <= X_MAP_RADIUS; x++)
			{
				TFixed offset = _FXD(0.0f);

				TFixedVec3 rect[4] =
				{
					TFixedVec3(_FXD(x*X_TILE_SIZE-X_TILE_SIZE/2)-offset, _FXD(y*Y_TILE_SIZE-Y_TILE_SIZE/2)-offset, 0),
					TFixedVec3(_FXD(x*X_TILE_SIZE+X_TILE_SIZE/2)-offset, _FXD(y*Y_TILE_SIZE-Y_TILE_SIZE/2)-offset, 0),
					TFixedVec3(_FXD(x*X_TILE_SIZE-X_TILE_SIZE/2)-offset, _FXD(y*Y_TILE_SIZE+Y_TILE_SIZE/2)-offset, 0),
					TFixedVec3(_FXD(x*X_TILE_SIZE+X_TILE_SIZE/2)-offset, _FXD(y*Y_TILE_SIZE+Y_TILE_SIZE/2)-offset, 0)
				};

				int index = (y+Y_MAP_RADIUS)*X_MAP_STRIDE+(x+X_MAP_RADIUS);

				// 첫 번째 삼각형
				vertices[index][0] = rect[0];
				vertices[index][1] = rect[1];
				vertices[index][2] = rect[2];
				// 두 번째 삼각형
				vertices[index][3] = rect[2];
				vertices[index][4] = rect[1];
				vertices[index][5] = rect[3];

				for (int i = 0; i < 6; i++)
				{
					normals[index][i] = default_normal;
				}
			}

			TFixedVec2 tex_coord[NUM_OF_VERTEX][6];
			memset(tex_coord, 0, sizeof(tex_coord));

			*m_vertex_buffer << TVertexData<FVF_XYZ_NOR_TEX>(NUM_OF_VERTEX*6, &vertices[0][0], &normals[0][0], &tex_coord[0][0]);
			*m_vertex_buffer << m_ref_tex_tile;

			m_vertex_object.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, NUM_OF_VERTEX*6, false));

			m_visible.bind(new CVisibleGeometrySub(m_vertex_buffer, m_ref_tex_tile, m_vertex_object, 0));
		}

	private:
		const shared::TTexTileList m_ref_tex_tile;
		
	};

	class CVisibleChara: public CVisible3D
	{
		class CVisibleCharaSub: public CVisible<TShared3DObject>
		{
		public:
			CVisibleCharaSub(shared::CVertexBuffer vertex_buffer, const shared::TTexTileList tex_tile1, TShared3DObject renderable_object, TShared3DObject renderable_object2, FnShow fn_show)
			:	m_vertex_buffer(vertex_buffer),
				m_ref_tex_tile1(tex_tile1),
				m_vertex_object(renderable_object),
				m_vertex_object2(renderable_object2)
			{
			}

		protected:
			virtual void _Show(TShared3DObject renderable_object) const
			{
				m_Act();

				TPosition albireos_view = avej_lite::singleton<CAlbireo>::get().GetBeholdingPosition();

				const TWorld& ref_world = avej_lite::singleton<TWorld>::get();

				// focused character 찾기
				iu::shared_ptr<CPlayer> focused_chara = TWorld::GetFocusedChara();
				bool                is_focused_chara_exist = (focused_chara.get() != 0);

				// player list 얻기
				action::get<const TPlayerList*> get_player;
				ref_world >> get_player;

				// map 얻기
				action::get<CSimpleTileMap*> get_geometry;
				avej_lite::singleton<TWorld>::get() >> get_geometry;

				if (get_geometry.data == 0)
					return;

				CSimpleTileMap& world_map = *get_geometry.data;

				TPlayerList::const_iterator iter_begin = get_player.data->begin();
				TPlayerList::const_iterator iter_end   = get_player.data->end();
				TPlayerList::const_iterator iter       = iter_begin;

				for (; iter != iter_end; ++iter)
				{
					const TPlayerDesc& player_desc = (*iter)->GetDesc();

					{
						int x_on_map = player_desc.position.x / X_TILE_SIZE;
						int y_on_map = player_desc.position.y / Y_TILE_SIZE;
						if (!world_map.IsVisible(x_on_map, y_on_map))
							continue;
					}

					unsigned int opaque = player_desc.opaque.value;

					if (iter == iter_begin)
						opaque = 255;

					// 그릴 필요가 없다.
					if (opaque == 0)
						continue;

					// vertex object를 새로 안 만들고 내부 자료만 변경 시키는 편법이긴 하나 현재는 잘 동작한다. 
					TFixedVec2 tex_coord[12];

					//?? 이런 것은 CPlayer가 자동으로 해줘야 하는 것 아닌가?
					CFace::TFace dir_face = player_desc.face.GetFaceFromDirection(player_desc.internal_count/6, player_desc.sight_direction, player_desc.is_moving);
					unsigned int ix_face  = dir_face.face;

					if (!dir_face.is_horiz_flipped)
					{
						// 정방향의 texture 복사
						memcpy(&tex_coord[0], m_ref_tex_tile1->tile_info[ix_face].tex_coord, sizeof(m_ref_tex_tile1->tile_info[ix_face].tex_coord));
					}
					else
					{
						// texture coordinate를 뒤집기
						tex_coord[0] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[1];
						tex_coord[1] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[0];
						tex_coord[2] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[5];
						tex_coord[3] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[5];
						tex_coord[4] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[0];
						tex_coord[5] = m_ref_tex_tile1->tile_info[ix_face].tex_coord[2];
					}

					*m_vertex_buffer << TTexCoordData(tex_coord);

					m_vertex_object->SetColor(opaque << 24 | CAlbireo::GetCurrentDreamColor() & 0x00FFFFFF);

					m_vertex_object->SetXY(player_desc.position.x - albireos_view.x, player_desc.position.y - albireos_view.y);

					// 항상 카메라와 직각을 이루도록
					{
						CScene& scene = avej_lite::singleton<CScene>::get();
						CScene::CCamera::TCameraDesc camera_desc;
						scene.Camera() >> camera_desc;

						m_vertex_object->SetAngleX(_FXD(camera_desc.incident_angle-90));
						m_vertex_object->SetAngleZ(_FXD(-camera_desc.rotation_angle/2));
	//					m_vertex_object->SetZ((Y_TILE_SIZE * Cos_16_16(camera_desc.incident_angle-90)) >> 16);
						m_vertex_object->SetZ(230);
					}

					// 본체
					m_vertex_object->Begin(RENDER_MODE_ALPHA);
					m_vertex_object->DrawPrimitive();
					m_vertex_object->End();

					// 벽에 투영되는 것 25%
					m_vertex_object->SetColor(0x40FFFFFF);
					m_vertex_object->Begin(RENDER_MODE_ADD, RENDER_OPTION_ALWAYS_ON_TOP);
					m_vertex_object->DrawPrimitive();
					m_vertex_object->End();
					
					m_vertex_object->SetColor(0xFFFFFFFF);
				}
			}

		private:
			shared::CVertexBuffer       m_vertex_buffer;
			const shared::TTexTileList  m_ref_tex_tile1;
			TShared3DObject             m_vertex_object;
			TShared3DObject             m_vertex_object2;

			void m_Act() const
			{
				m_vertex_object->SetColor(CAlbireo::GetCurrentDreamColor());
			}
		};

	public:
		CVisibleChara(const shared::TTexTileList tex_tile)
		:	m_ref_tex_tile(tex_tile)
		{
			TFixedVec3 rect[8] =
			{
				// 본체
	#if 0
				// 바닥 대해 직각인 경우
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(0), _FXD(Y_TILE_SIZE*1.5)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(0), _FXD(Y_TILE_SIZE*1.5)),
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(0), _FXD(0)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(0), _FXD(0)),
	#else
				// 바닥에 눕는 경우
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(-Y_TILE_SIZE/2)*1.5, _FXD(1)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(-Y_TILE_SIZE/2)*1.5, _FXD(1)),
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(+Y_TILE_SIZE/2)*1.5, _FXD(1)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(+Y_TILE_SIZE/2)*1.5, _FXD(1)),
	#endif
				// 마커. 마커는 항상 바닥에
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(-Y_TILE_SIZE/2), _FXD(1)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(-Y_TILE_SIZE/2), _FXD(1)),
				TFixedVec3(_FXD(-X_TILE_SIZE/2), _FXD(+Y_TILE_SIZE/2), _FXD(1)),
				TFixedVec3(_FXD(+X_TILE_SIZE/2), _FXD(+Y_TILE_SIZE/2), _FXD(1))
			};

			TFixedVec3 vertices[12] =
			{
				// 본체의 첫 번째 삼각형
				rect[0], rect[1], rect[2],
				// 본체의 두 번째 삼각형
				rect[2], rect[1], rect[3],

				// 마커의 첫 번째 삼각형
				rect[4], rect[5], rect[6],
				// 마커의 두 번째 삼각형
				rect[6], rect[5], rect[7]
			};

			TFixedVec2 tex_coord[12];
	/*
			unsigned int m_ix_face = 90;
			memcpy(&tex_coord[0], m_ref_tex_tile->tile_info[m_ix_face].tex_coord, sizeof(m_ref_tex_tile->tile_info[m_ix_face].tex_coord));
	*/
			*m_vertex_buffer << TVertexData<FVF_XYZ_TEX>(12, vertices, tex_coord);
			*m_vertex_buffer << m_ref_tex_tile;

			m_vertex_object.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, 6, false));
			m_vertex_object2.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 6, 6, false));

			m_visible.bind(new CVisibleCharaSub(m_vertex_buffer, m_ref_tex_tile, m_vertex_object, m_vertex_object2, 0));
		}

	private:
		shared::CVertexObject      m_vertex_object2;
		const shared::TTexTileList m_ref_tex_tile;
		
	};

	// 일부러 밖으로 뺐음
	namespace
	{
		// 맵이 바뀔 때, Beholder의 시선은 바뀌지 않을 수 있기 때문에, 맵이 변경되면 항상 이 값은 초기화 되어야 한다.
		static TPosition               s_cached_position(0x80000000, 0x80000000);
		static std::vector<TFixedVec2> s_cached_triangle;
	}

	class CVisibleGeometryShadow: public CVisible3D
	{
		class CVisibleGeometryShadowSub: public CVisible<TShared3DObject>
		{
		public:
			CVisibleGeometryShadowSub()
			{
			}

		protected:
			virtual void _Show(TShared3DObject renderable_object) const
			{
				m_Act();

				// 새로 만드는 부분
				{
					int num_of_vertex = s_cached_triangle.size();

					// num_of_vertex는 항상 3의 배수여야 한다.
					assert(num_of_vertex % 3 == 0);

					shared::CVertexBuffer vertex_buffer(new CVertexBuffer);

					// std::vector<>는 순차적으로 저장된다는 것을 전제로 한 것임
					*vertex_buffer << TVertexData<FVF_XY>(num_of_vertex, &*s_cached_triangle.begin());

					CVertexObject vertex_object(vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, num_of_vertex, false);

					vertex_object.SetColor(CAlbireo::GetDreamColor(DREAM_COLOR_SHADOW));
					vertex_object.SetZ(5);

					vertex_object.Begin(RENDER_MODE_COPY);
	//				@@ cull mode 해제 해야 함
					vertex_object.DrawPrimitive();
					vertex_object.End();
				}
			}

		private:
			void m_Act() const
			{
				// 이전의 데이터를 caching 하고 있다가 변화가 없으면 그것을 내어 준다.
				TPosition albireos_view = avej_lite::singleton<CAlbireo>::get().GetBeholdingPosition();

				action::get<CSimpleTileMap*> get_geometry;
				avej_lite::singleton<TWorld>::get() >> get_geometry;

				if (get_geometry.data == 0)
					return;

				CSimpleTileMap& world_map = *get_geometry.data;

				int map_x_offset = albireos_view.x / X_TILE_SIZE - X_MAP_RADIUS;
				int map_y_offset = albireos_view.y / Y_TILE_SIZE - Y_MAP_RADIUS;

				CFosTileMap fosMap(world_map, map_x_offset, map_y_offset, X_TILE_SIZE, Y_TILE_SIZE);

				#define MAP_WSIZE (X_MAP_RADIUS*2+1)
				#define MAP_HSIZE (Y_MAP_RADIUS*2+1)

				if (albireos_view == s_cached_position)
				{
	#if defined(USE_SMOOTH_HIDING)
					for (int y = map_y_offset; y < map_y_offset+MAP_HSIZE; y++)
					for (int x = map_x_offset; x < map_x_offset+MAP_WSIZE; x++)
					{
						fosMap.ProcessVisibility(x, y);
					}
	#endif
				}
				else
				{
					std::vector<TFixedVec2>().swap(s_cached_triangle);

					// shadow map을 만들고
	#if 0 // 테스트
					s_cached_triangle.push_back(TFixedVec2(_FXD(-80), _FXD(-50)));
					s_cached_triangle.push_back(TFixedVec2(_FXD(100), _FXD(-50)));
					s_cached_triangle.push_back(TFixedVec2(_FXD(10), _FXD( 100)));
	#else
					TPosition view = albireos_view;

					view.x = albireos_view.x + X_TILE_SIZE / 2;
					view.y = albireos_view.y + Y_TILE_SIZE / 2;

					// 각 grid에 대한 기본 visibility 설정
					{
						fosMap.ClearVisibility();

						for (int y = map_y_offset; y < map_y_offset+MAP_HSIZE; y++)
						for (int x = map_x_offset; x < map_x_offset+MAP_WSIZE; x++)
						{
							fosMap.IsMyEyesReached(view.x, view.y, x*BLOCK_WSIZE,    y*BLOCK_HSIZE);
							fosMap.IsMyEyesReached(view.x, view.y, x*BLOCK_WSIZE+BLOCK_WSIZE-1, y*BLOCK_HSIZE);
							fosMap.IsMyEyesReached(view.x, view.y, x*BLOCK_WSIZE,    y*BLOCK_HSIZE+BLOCK_HSIZE-1);
							fosMap.IsMyEyesReached(view.x, view.y, x*BLOCK_WSIZE+BLOCK_WSIZE-1, y*BLOCK_HSIZE+BLOCK_HSIZE-1);
						}
					}

					// 모서리 부분에 대한 visibility 처리
					{
						#define sign(a) ((a) == 0) ? 0 : ((a) > 0) ? 1 : -1;
						int _x = (view.x / BLOCK_WSIZE);
						int _y = (view.y / BLOCK_HSIZE);

						std::vector<int> visibleList;

						for (int y = map_y_offset; y < map_y_offset+MAP_HSIZE; y++)
						for (int x = map_x_offset; x < map_x_offset+MAP_WSIZE; x++)
						{
							int dx = sign(_x-x);
							int dy = sign(_y-y);

							if (!fosMap.IsVisible(x, y))
							if (fosMap.IsOccluded(x, y))
							if ((dx) && (dy))
							{
								if (fosMap.IsVisibleAndOccluded(x, y+dy) && fosMap.IsVisibleAndOccluded(x+dx, y))
								{
									visibleList.push_back(x);
									visibleList.push_back(y);
								}
							}
						}

						for (std::vector<int>::iterator i = visibleList.begin(); i != visibleList.end();)
						{
							int x = *i++;
							int y = *i++;
							fosMap.SetVisibility(x, y);
						}
					}

					// 그림자에 대한 출력
					{
						#define X_OFFSET (-view.x)
						#define Y_OFFSET (-view.y)

						for (int y = map_y_offset; y < map_y_offset+MAP_HSIZE; y++)
						for (int x = map_x_offset; x < map_x_offset+MAP_WSIZE; x++)
						{
							if (fosMap.IsVisibleAndOccluded(x, y))
							{
								CFosTileMap::TPoint vertices[4];
								fosMap.MakeFos(view.x, view.y, x, y, vertices);

								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[0].x+X_OFFSET), _FXD(vertices[0].y+Y_OFFSET)));
								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[1].x+X_OFFSET), _FXD(vertices[1].y+Y_OFFSET)));
								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[2].x+X_OFFSET), _FXD(vertices[2].y+Y_OFFSET)));

								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[2].x+X_OFFSET), _FXD(vertices[2].y+Y_OFFSET)));
								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[3].x+X_OFFSET), _FXD(vertices[3].y+Y_OFFSET)));
								s_cached_triangle.push_back(TFixedVec2(_FXD(vertices[0].x+X_OFFSET), _FXD(vertices[0].y+Y_OFFSET)));
							}
						}
					}
	#endif

					s_cached_position = albireos_view;
				}

				// Elemental의 shy 책정
				{
					action::get<const TPlayerList*> get_player;
					avej_lite::singleton<TWorld>::get() >> get_player;

					TPlayerList::const_iterator iter_begin = get_player.data->begin();
					TPlayerList::const_iterator iter_end   = get_player.data->end();
					TPlayerList::const_iterator iter       = iter_begin;

					int main_player_sight_angle = 0;

					TPosition origin(albireos_view.x + X_TILE_SIZE / 2, albireos_view.y + Y_TILE_SIZE / 2);

					for (; (iter != iter_end); ++iter)
					{
						// main character임
						if (iter == iter_begin)
						{
							main_player_sight_angle = (*iter)->GetDesc().sight_angle;
							continue;
						}

						const TPlayerDesc& player_desc = (*iter)->GetDesc();

						bool success = false;

						//?? 원래는 main character의 ability에 따라 이 값은 바뀌어야 한다.
						if (player_desc.distance.value < CONFIG_DISTANCE_NO_WORTH)
						{
							if (((main_player_sight_angle >= player_desc.angle_range.min_value) && (main_player_sight_angle <= player_desc.angle_range.max_value))
								|| ((main_player_sight_angle+360 >= player_desc.angle_range.min_value) && (main_player_sight_angle+360 <= player_desc.angle_range.max_value)))
							//?? 원래는 이것도 해야 하지만 자동으로 shadow가 가려 준다.
							//if (fosMap.IsMyEyesReached(origin.x, origin.y, origin.x+100, origin.y))
							{
								success = true;
							}
						}

						*(*iter) << action::line_of_sight<bool>(success);
					}
				}
			}
		};

	public:
		CVisibleGeometryShadow()
		{
			m_visible.bind(new CVisibleGeometryShadowSub);
		}
		
	};

	class CVisibleGeometrySolid: public CVisible3D
	{
		class CVisibleGeometrySolidSub: public CVisible<TShared3DObject>
		{
		public:
			CVisibleGeometrySolidSub(shared::CVertexBuffer vertex_buffer, const shared::TTexTileList tex_tile, TShared3DObject renderable_object, FnShow fn_show)
			:	CVisible<TShared3DObject>(renderable_object, fn_show),
				m_vertex_buffer(vertex_buffer),
				m_ref_tex_tile(tex_tile),
				m_vertex_object(renderable_object)
				
			{
			}

		protected:
			virtual void _Show(TShared3DObject renderable_object) const
			{
				static bool       s_is_first = true;
				static TFixedVec3 s_vertices[10*3];
				static TFixedVec3 s_normals[10*3];

				if (s_is_first)
				{
					const TFixed X_EXT = _FXD(X_TILE_SIZE) / 2;
					const TFixed Y_EXT = _FXD(Y_TILE_SIZE) / 2;
					const TFixed Z_BOT = _FXD( 0.0f);
					const TFixed Z_TOP = Y_EXT * 3;

					TFixedVec3 cube[8] =
					{
						TFixedVec3(-X_EXT, -Y_EXT, Z_BOT), // 0
						TFixedVec3(+X_EXT, -Y_EXT, Z_BOT), // 1
						TFixedVec3(-X_EXT, +Y_EXT, Z_BOT), // 2
						TFixedVec3(+X_EXT, +Y_EXT, Z_BOT), // 3
						TFixedVec3(-X_EXT, -Y_EXT, Z_TOP), // 4
						TFixedVec3(+X_EXT, -Y_EXT, Z_TOP), // 5
						TFixedVec3(-X_EXT, +Y_EXT, Z_TOP), // 6
						TFixedVec3(+X_EXT, +Y_EXT, Z_TOP), // 7
					};
					
					const float LEN_OF_Z_BOT = 1.0f / sqrt(double((X_TILE_SIZE/2)*(X_TILE_SIZE/2) + (Y_TILE_SIZE/2)*(Y_TILE_SIZE/2)));
					const float LEN_OF_Z_TOP = 1.0f / sqrt(double((X_TILE_SIZE/2)*(X_TILE_SIZE/2) + (Y_TILE_SIZE/2)*(Y_TILE_SIZE/2) + (Y_TILE_SIZE/2)*(Y_TILE_SIZE/2)*9));

					TFixedVec3 cube_normal[8] =
					{
						TFixedVec3(int(-X_EXT*LEN_OF_Z_BOT), int(-Y_EXT*LEN_OF_Z_BOT), Z_BOT), // 0
						TFixedVec3(int(+X_EXT*LEN_OF_Z_BOT), int(-Y_EXT*LEN_OF_Z_BOT), Z_BOT), // 1
						TFixedVec3(int(-X_EXT*LEN_OF_Z_BOT), int(+Y_EXT*LEN_OF_Z_BOT), Z_BOT), // 2
						TFixedVec3(int(+X_EXT*LEN_OF_Z_BOT), int(+Y_EXT*LEN_OF_Z_BOT), Z_BOT), // 3
						TFixedVec3(int(-X_EXT*LEN_OF_Z_TOP), int(-Y_EXT*LEN_OF_Z_TOP), int(Z_TOP*LEN_OF_Z_TOP)), // 4
						TFixedVec3(int(+X_EXT*LEN_OF_Z_TOP), int(-Y_EXT*LEN_OF_Z_TOP), int(Z_TOP*LEN_OF_Z_TOP)), // 5
						TFixedVec3(int(-X_EXT*LEN_OF_Z_TOP), int(+Y_EXT*LEN_OF_Z_TOP), int(Z_TOP*LEN_OF_Z_TOP)), // 6
						TFixedVec3(int(+X_EXT*LEN_OF_Z_TOP), int(+Y_EXT*LEN_OF_Z_TOP), int(Z_TOP*LEN_OF_Z_TOP)), // 7
					};

					/*
						 0      0------1      1
								| back |
								|      | 
						 0------4------5------1
						 | left | top  |right |
						 |      |      |      |
						 2------6------7------3
								|front |
								|      |
						 2      2------3      3
					*/

					TFixedVec3 vertices[10*3] =
					{
						cube[4], cube[5], cube[6], // top1
						cube[6], cube[5], cube[7], // top2
						cube[6], cube[7], cube[2], // front1
						cube[2], cube[7], cube[3], // front2
						cube[7], cube[5], cube[3], // right1
						cube[3], cube[5], cube[1], // right2
						cube[5], cube[4], cube[1], // back1
						cube[1], cube[4], cube[0], // back2
						cube[4], cube[6], cube[0], // left1
						cube[0], cube[6], cube[2]  // left2
					};

					TFixedVec3 normals[10*3] =
					{
						cube_normal[4], cube_normal[5], cube_normal[6], // top1
						cube_normal[6], cube_normal[5], cube_normal[7], // top2
						cube_normal[6], cube_normal[7], cube_normal[2], // front1
						cube_normal[2], cube_normal[7], cube_normal[3], // front2
						cube_normal[7], cube_normal[5], cube_normal[3], // right1
						cube_normal[3], cube_normal[5], cube_normal[1], // right2
						cube_normal[5], cube_normal[4], cube_normal[1], // back1
						cube_normal[1], cube_normal[4], cube_normal[0], // back2
						cube_normal[4], cube_normal[6], cube_normal[0], // left1
						cube_normal[0], cube_normal[6], cube_normal[2]  // left2
					};

					assert(sizeof(s_vertices) == sizeof(vertices));
					memcpy(s_vertices, vertices, sizeof(s_vertices));
					memcpy(s_normals, normals, sizeof(s_normals));

					s_is_first = false;
				}

				action::get<CSimpleTileMap*> get_geometry;
				avej_lite::singleton<TWorld>::get() >> get_geometry;

				if (get_geometry.data == 0)
					return;

				TPosition albireos_view = avej_lite::singleton<CAlbireo>::get().GetBeholdingPosition();

				CSimpleTileMap& world_map = *get_geometry.data;

				int x_off = albireos_view.x / X_TILE_SIZE;
				int y_off = albireos_view.y / Y_TILE_SIZE;

				for (int y = -Y_MAP_RADIUS; y <= Y_MAP_RADIUS; y++)
				for (int x = -X_MAP_RADIUS; x <= X_MAP_RADIUS; x++)
				{
					int x_on_map = x+x_off;
					int y_on_map = y+y_off;

	#if defined(USE_SMOOTH_HIDING)
					unsigned long visibility = world_map.GetVisibility(x_on_map, y_on_map);

					if (visibility <= 0)
						continue;
	#else
					if (!world_map.IsVisible(x_on_map, y_on_map))
						continue;
	#endif

					int ix_tile = world_map.GetTile(x_on_map, y_on_map);

					if ((ix_tile >= 10) && (ix_tile < 20))
					{
						int x_pos = -albireos_view.x+(x_on_map)*X_TILE_SIZE;
						int y_pos = -albireos_view.y+(y_on_map)*Y_TILE_SIZE;

						static int s_ix_tile = -1;

						if (s_ix_tile != ix_tile)
						{
							TFixedVec2 tex_coord[5][6];

							for (int i = 0; i < 5; i++)
							{
								memcpy(&tex_coord[i][0], m_ref_tex_tile->tile_info[ix_tile].tex_coord, sizeof(m_ref_tex_tile->tile_info[ix_tile].tex_coord));
							}

							*m_vertex_buffer << TVertexData<FVF_XYZ_NOR_TEX>(30, s_vertices, s_normals, &tex_coord[0][0]);

							m_vertex_object.setNull();
							m_vertex_object.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, 30, false));

							m_vertex_object->Begin(RENDER_MODE_COPY);

							s_ix_tile = ix_tile;
						}

	#if defined(USE_SMOOTH_HIDING)
					#if 1
						unsigned long color = CAlbireo::GetCurrentDreamColor();
						unsigned long r = (color >> 16) & 0xFF;
						unsigned long g = (color >>  8) & 0xFF;
						unsigned long b = (color >>  0) & 0xFF;
						r = r * visibility / 255;
						g = g * visibility / 255;
						b = b * visibility / 255;
						m_vertex_object->SetColor(0xFF000000 | r << 16 | g << 8 | b);
					#else
						visibility = visibility | visibility << 8 | visibility << 16;
						m_vertex_object->SetColor((CAlbireo::GetCurrentDreamColor() & visibility) | 0xFF000000);
					#endif
	#else
						m_vertex_object->SetColor(CAlbireo::GetCurrentDreamColor());
	#endif
						m_vertex_object->SetXY(x_pos, y_pos);
						
						m_vertex_object->DrawPrimitive();
					}
				}

				// CVisible<TShared3DObject>의 _Show(renderable_object)를 부르면 절대 안된다.
			}

		private:
			shared::CVertexBuffer       m_vertex_buffer;
			const shared::TTexTileList  m_ref_tex_tile;
			mutable TShared3DObject     m_vertex_object;
		};

	public:
		CVisibleGeometrySolid(const shared::TTexTileList tex_tile)
			: m_ref_tex_tile(tex_tile)
		{
			// CVisibleGeometrySolidSub::_Show()에서 항상 재정의 되기 때문에, [m_vertex_buffer << TVertexData]는 지금 하지 않는다.

			*m_vertex_buffer << m_ref_tex_tile;
			*m_vertex_buffer << TTexParam(TTexParam::FILTER_DEFAULT, TTexParam::FILTER_LINEAR);

			m_vertex_object.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, 30, false));

			m_visible.bind(new CVisibleGeometrySolidSub(m_vertex_buffer, m_ref_tex_tile, m_vertex_object, 0));
		}

	private:
		const shared::TTexTileList m_ref_tex_tile;
		
	};

	class CVisibleTextBox: public CVisible3D
	{
		class CVisibleTextBoxSub: public CVisible<TShared3DObject>
		{
		public:
			CVisibleTextBoxSub(const shared::TTexTileList tex_font)
			:	m_ref_tex_font(tex_font),
				m_ref_render_text(*GetTextInstance())
			{
			}

		protected:
			virtual void _Show(TShared3DObject renderable_object) const
			{
				m_Act();

				// 새로 만드는 부분
				{
					int num_of_rectangle = m_blit_list_kano.size();

					if (num_of_rectangle > 0)
					{
						int ix_vertex = 0;
						int ix_index  = 0;

						TFixedVec2*     vertex_coord = new TFixedVec2[num_of_rectangle * 4];
						TFixedVec4*     vertex_color = new TFixedVec4[num_of_rectangle * 4];
						TFixedVec2*     textur_coord = new TFixedVec2[num_of_rectangle * 4];
						unsigned short* index_buffer = new unsigned short[num_of_rectangle * 6];

						std::vector<TBitBlt>::iterator iter = m_blit_list_kano.begin();
						std::vector<TBitBlt>::iterator end  = m_blit_list_kano.end();

						for (; iter != end; ++iter)
						{
							const TBitBlt& blit_param = *iter;
							TFixedVec4  diffuse_color = _FXD_color2array(blit_param.color);

							vertex_coord[ix_vertex+0] = TFixedVec2(_FXD(blit_param.dst_bound.x1), _FXD(blit_param.dst_bound.y1));
							vertex_coord[ix_vertex+1] = TFixedVec2(_FXD(blit_param.dst_bound.x2), _FXD(blit_param.dst_bound.y1));
							vertex_coord[ix_vertex+2] = TFixedVec2(_FXD(blit_param.dst_bound.x1), _FXD(blit_param.dst_bound.y2));
							vertex_coord[ix_vertex+3] = TFixedVec2(_FXD(blit_param.dst_bound.x2), _FXD(blit_param.dst_bound.y2));

							vertex_color[ix_vertex+0] = diffuse_color;
							vertex_color[ix_vertex+1] = diffuse_color;
							vertex_color[ix_vertex+2] = diffuse_color;
							vertex_color[ix_vertex+3] = diffuse_color;

							//?? 상수로 쓰면 안됨
							textur_coord[ix_vertex+0] = TFixedVec2(_FXD(blit_param.src_bound.x1 *1.0f/512.0f), _FXD(blit_param.src_bound.y1 *1.0f/512.0f));
							textur_coord[ix_vertex+1] = TFixedVec2(_FXD(blit_param.src_bound.x2 *1.0f/512.0f), _FXD(blit_param.src_bound.y1 *1.0f/512.0f));
							textur_coord[ix_vertex+2] = TFixedVec2(_FXD(blit_param.src_bound.x1 *1.0f/512.0f), _FXD(blit_param.src_bound.y2 *1.0f/512.0f));
							textur_coord[ix_vertex+3] = TFixedVec2(_FXD(blit_param.src_bound.x2 *1.0f/512.0f), _FXD(blit_param.src_bound.y2 *1.0f/512.0f));

							index_buffer[ix_index +0] = ix_vertex+0;
							index_buffer[ix_index +1] = ix_vertex+1;
							index_buffer[ix_index +2] = ix_vertex+2;
							index_buffer[ix_index +3] = ix_vertex+2;
							index_buffer[ix_index +4] = ix_vertex+1;
							index_buffer[ix_index +5] = ix_vertex+3;

							ix_vertex += 4;
							ix_index  += 6;
						}

						shared::CVertexBuffer vertex_buffer(new CVertexBuffer);

						*vertex_buffer << TVertexData<FVF_XY_DIF_TEX>(num_of_rectangle*4, vertex_coord, vertex_color, textur_coord);
						*vertex_buffer << TIndexData(num_of_rectangle*6, index_buffer);
						*vertex_buffer << m_ref_tex_font;

						CVertexObject vertex_object(vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, num_of_rectangle * 6, true);

						//?? TRenderOption() 안 쓰도록 수정 필요
						vertex_object.Begin(RENDER_MODE_ALPHA, TRenderOption(RENDER_OPTION_ALWAYS_ON_TOP | RENDER_OPTION_ORTHOGONAL));
						vertex_object.DrawPrimitive();
						vertex_object.End();

						delete[] index_buffer;
						delete[] textur_coord;
						delete[] vertex_color;
						delete[] vertex_coord;
					}
				}
			}

		private:
			const shared::TTexTileList m_ref_tex_font;
			const IorzRenderText&      m_ref_render_text;

			void m_Act() const
			{
				std::vector<TBitBlt>().swap(m_blit_list_kano);

				{
					int x_text = -160;
					int y_text = -120;
					{
						presentation_util::RenderText(x_text, y_text, presentation_util::GetFPS(), CAlbireo::GetDreamColor(DREAM_COLOR_WHITE), m_RenderText);
					}

					y_text += 12;
					{
						char s[256];
						TPosition albireos_view = avej_lite::singleton<CAlbireo>::get().GetBeholdingPosition();
						s_ComposeString(s, "(@,@)", albireos_view.x, albireos_view.y);
						presentation_util::RenderText(x_text, y_text, s, CAlbireo::GetDreamColor(DREAM_COLOR_WHITE), m_RenderText);
					}

					y_text += 12;
					{
						CScene::CCamera::TCameraDesc camera_desc;
						avej_lite::singleton<CScene>::get().Camera() >> camera_desc;

						char s[256];
						s_ComposeString(s, "Angle: @", camera_desc.incident_angle);
						presentation_util::RenderText(x_text, y_text, s, CAlbireo::GetDreamColor(DREAM_COLOR_WHITE), m_RenderText);
					}
/*??
					y_text += 12;
					{
						int x, y;

						if (avej_lite::singleton<avej_lite::CInputDevice>::get().GetTouchState(x, y))
						{
							char s[256];
							sprintf(s, "Touched at (%d, %d)", x, y);
							presentation_util::RenderText(x_text, y_text, s, CAlbireo::GetDreamColor(DREAM_COLOR_WHITE), m_RenderText);
						}
						else
						{
							unsigned long time_stamp = avej_lite::singleton<avej::CInputDevice>::get().GetTimeStamp();
							char s[256];
							sprintf(s, "time stamp: %d", time_stamp);
							presentation_util::RenderText(x_text, y_text, s, CAlbireo::GetDreamColor(DREAM_COLOR_WHITE), m_RenderText);
						}
					}
*/
					x_text              =  150;
					y_text              = -115;
					unsigned text_color = 0xFFFFC050;

					static int      updating_count = 0;
					static widechar sz_mode_name[6];
					static unsigned int sz_mode_name_max = sizeof(sz_mode_name) / sizeof(sz_mode_name[0]) - 1;

					if (updating_count++ == 0)
					{
						for (int i = 0; i < sz_mode_name_max; i++)
						{
							const int MAX_SM1 = 19;
							const int MAX_SM2 = 21;
							const int MAX_SM3 = 28;

							unsigned short SM1 = rand() % MAX_SM1;
							unsigned short SM2 = rand() % MAX_SM2;
							unsigned short SM3 = rand() % MAX_SM3;
							SM3 = 0;

							sz_mode_name[i] = widechar(0xAC00+(SM1)*(MAX_SM3*MAX_SM2)+(SM2)*MAX_SM3+(SM3));
						}
						sz_mode_name[sz_mode_name_max] = 0;
					}

					updating_count %= 15;

					presentation_util::RenderText(x_text - 12*sz_mode_name_max, y_text, sz_mode_name, text_color, m_RenderText);

					static widechar sz_test_strung[6] =
					{
						0xc65c, 0x0021, 0xb0a0, 0x0021, 0xbdc1, 0
					};

					presentation_util::RenderText(x_text - 12*sz_mode_name_max, y_text+12, sz_test_strung, text_color, m_RenderText);
				}
			}

			struct TBitBlt
			{
				TBound        dst_bound;
				TBound        src_bound;
				unsigned long color;

				TBitBlt(int xDest, int yDest, int width, int height, int xSour, int ySour, unsigned long _color)
				:	dst_bound(xDest, yDest, xDest+width, yDest+height), src_bound(xSour, ySour, xSour+width, ySour+height), color(_color) {}
			};

			static std::vector<TBitBlt> m_blit_list_kano;

			static void m_RenderText(int xDest, int yDest, int width, int height, int xSour, int ySour, unsigned long color)
			{
				m_blit_list_kano.push_back(TBitBlt(xDest, yDest, width, height, xSour, ySour, color));
			}
		};

	public:
		CVisibleTextBox(const shared::TTexTileList tex_font)
		:	m_ref_tex_font(tex_font)
		{
			m_visible.bind(new CVisibleTextBoxSub(m_ref_tex_font));
		}

	private:
		const shared::TTexTileList m_ref_tex_font;

	};

	std::vector<CVisibleTextBox::CVisibleTextBoxSub::TBitBlt> CVisibleTextBox::CVisibleTextBoxSub::m_blit_list_kano;

} // namespace

namespace presentation_kano
{
	struct TPresentationData
	{
		enum TResVisible3D
		{
			RES_VISIBLE3D_MAP,
			RES_VISIBLE3D_MAP_SHADOW,
			RES_VISIBLE3D_MAP_SOLID,
			RES_VISIBLE3D_MAP_CHARA,
			RES_VISIBLE3D_MAP_TEXT,
			RES_VISIBLE3D_MAX
		};

		// presentation의 샘플로 있는 model임. 항상 destructor에서 해제를 해줘야 함
		CVisibleCube cube;
		iu::shared_ptr<CVisibleActor<TShared3DObject> > visible_actor_cube;

		// texture 리소스
		shared::TTexTileList tex_tile;

		// model 리소스
		iu::shared_ptr<CVisible3D> res_visible_3d[RES_VISIBLE3D_MAX];

		// visible actor들. 출력 순서에 맞게 들어 가야 한다.
		std::vector<CVisibleActor<TShared3DObject> > visible_actor_list;

		// reference만 가지는 부분
		const TWorld& ref_world;

		TPresentationData()
		:	ref_world(avej_lite::singleton<TWorld>::get())
		{
			avej::gfx3d::CDevice3D& device = avej_lite::singleton<CDevice3D>::get();

			// scene 설정
			{
				CScene& scene = avej_lite::singleton<CScene>::get();

				// 조명 설정
				scene.Light()[0] << CScene::CLight::CUnitLight::Ambient(0xFF404040);
				scene.Light()[0] << CScene::CLight::CUnitLight::Diffuse(0xFFC0C0C0);
				scene.Light()[0] << CScene::CLight::CUnitLight::Pos(TFixedVec3(_FXD(0.0f), _FXD(0.0f), _FXD(1.0f)));
				scene.Light()[0] << CScene::Enable();
				scene.Light()    << CScene::Enable();

				// 공간 설정
				scene.Projection() << CScene::CProjection::CProjectionParam(_FXD(200.0f), _FXD(120.0f), _FXD(320*3/2), _FXD(320*3/2) * 9, _FXD(8));
				scene.Projection() << CScene::Enable();

				// 카메라 설정
				{
					CScene::CCamera::TCameraDesc camera_desc;

					scene.Camera() >> camera_desc;

					camera_desc.incident_angle = 35;
//					camera_desc.rotation_angle = 10;
					camera_desc.rotation_angle = 3;
					camera_desc.distance       = 1690;

					camera_desc.incident_angle = 50;
					camera_desc.distance       = 1690 * 1.7;
					scene.Camera() << camera_desc;
				}

				// Fog 설정
				{
//					CScene::CFog::TFogDesc fog_desc(_FXD(0.5f), _FXD(1000.0f*8.0f/11.0f), _FXD(4000.0f*8.0f/11.0f), 0xFF000000);
					CScene::CFog::TFogDesc fog_desc(_FXD(0.5f), _FXD(2800.0f*8.0f/11.0f), _FXD(6000.0f*8.0f/11.0f), 0xFF000000);

					scene.Fog() << fog_desc;

					scene.Fog() << CScene::Enable();
				}

				// scene 적용
				scene << CScene::Enable();
			}

			// image 읽기
			{
				avej_lite::TSurfaceDesc surface_desc;

				if (!presentation_util::LoadImage(TEXTURE_FILE_NAME1, surface_desc))
				{
					// 디코딩 실패시에는 임시로 16 * 16 크기의 texture를 만든다.
					memset(&surface_desc, 0, sizeof(surface_desc));

					surface_desc.buffer_desc.pixel_format  = avej_lite::PIXELFORMAT_RGBA4444;
					surface_desc.buffer_desc.width         = 16;
					surface_desc.buffer_desc.height        = 16;
					surface_desc.buffer_desc.depth         = 16;
					surface_desc.lock_desc.p_buffer        = new unsigned char[16*16*2];
					surface_desc.lock_desc.bytes_per_line  = 32;
					surface_desc.has_ownership             = true;
				}

				{
					typedef unsigned short TPixel;

					// 2D의 FillRect() 관련
					*((TPixel*)surface_desc.lock_desc.p_buffer) = 0xFFFF;

					// 위쪽의 384 * 180 영역만 gray level 처리
					for (int y = 0; y < 180; y++)
					{
						TPixel* p_dest = (TPixel*)((char*)surface_desc.lock_desc.p_buffer + surface_desc.lock_desc.bytes_per_line * y);

						for (int x = 0; x < 384; x++)
						{
							TPixel alpha = ((*p_dest >> 4) & 0x000F);

							// alpha를 조금 더 진하게..
							alpha = alpha * 12 / 10;
							if (alpha > 0x0F)
								alpha = 0x0F;

							*p_dest = alpha | 0xFFF0;
							++p_dest;
						}
					}
				}

				// Texture로 생성
				device.CreateTexture(surface_desc);

				// 옮겨 담기 위한 수단
				iu::shared_ptr<avej_lite::TSurfaceDesc> shared_surface_desc(new avej_lite::TSurfaceDesc(surface_desc)) ;

				// shared surface desc로 등록
				tex_tile.bind(new TTexTileList(shared_surface_desc));

				if (tex_tile->handle.get())
				{
					for (int i = 0; i < 60; i++)
					{
						TFixed x1 = _FXD(0);
						TFixed y1 = _FXD(0);
						TFixed x2 = _FXD(1.0f / tex_tile->handle->buffer_desc.width);
						TFixed y2 = _FXD(1.0f / tex_tile->handle->buffer_desc.height);

						tex_tile->tile_info.push_back(TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2));
					}

					{
						// 0 ~ 9
						{
							TFixed x1 = _FXD(398.0f / tex_tile->handle->buffer_desc.width);
							TFixed y1 = _FXD(  2.0f / tex_tile->handle->buffer_desc.height);
							TFixed x2 = _FXD(430.0f / tex_tile->handle->buffer_desc.width);
							TFixed y2 = _FXD( 34.0f / tex_tile->handle->buffer_desc.height);

							for (int x = 0; x < 10; x++)
								tex_tile->tile_info[x] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);

							x1 = _FXD(434.0f / tex_tile->handle->buffer_desc.width);
							x2 = _FXD(466.0f / tex_tile->handle->buffer_desc.width);
							tex_tile->tile_info[1] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);
						}

						// 10 ~ 19
						{
							const float f_x1 = 396.0f;
							const float f_y1 = 40.0f;
							const float f_x2 = f_x1 + 25.0f;
							const float f_y2 = f_y1 + 85.0f;

							TFixed x1 = _FXD(f_x1 / tex_tile->handle->buffer_desc.width);
							TFixed y1 = _FXD(f_y1 / tex_tile->handle->buffer_desc.height);
							TFixed x2 = _FXD(f_x2 / tex_tile->handle->buffer_desc.width);
							TFixed y2 = _FXD(f_y2 / tex_tile->handle->buffer_desc.height);

							for (int x = 0; x < 10; x++)
								tex_tile->tile_info[10+x] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);
						}

						// 30 ~ 39
						{
							const int X_START   = 0;
							const int Y_START   = 193;
							const int X_STRIDE  = 30;
							const int Y_STRIDE  = 58;
							const int X_PADDING = 0;
							const int Y_PADDING = 0;

							const float X_TILE_SIZE_FLOAT = 30 * 1.0f / tex_tile->handle->buffer_desc.width;
							const float Y_TILE_SIZE_FLOAT = 56 * 1.0f / tex_tile->handle->buffer_desc.height;

							for (int y = 0; y <  2; y++)
							for (int x = 0; x <  5; x++)
							{
								int   x_real  = X_START + X_STRIDE * x + X_PADDING;
								int   y_real  = Y_START + Y_STRIDE * y + Y_PADDING;
								float x_float = x_real * 1.0f / tex_tile->handle->buffer_desc.width;
								float y_float = y_real * 1.0f / tex_tile->handle->buffer_desc.height;

								TFixed x1 = _FXD(x_float);
								TFixed y1 = _FXD(y_float);
								TFixed x2 = x1 + _FXD(X_TILE_SIZE_FLOAT) - 1;
								TFixed y2 = y1 + _FXD(Y_TILE_SIZE_FLOAT) - 1;

								tex_tile->tile_info[30+y*5+x] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);
							}
						}
						// 40 ~ 47
						{
							const float X_START   = 155.0f;
							const float Y_START   = 193.0f;
							const float X_STRIDE  =  52.5f;
							const float Y_STRIDE  =  58.0f;
							const float X_PADDING =   0.0f;
							const float Y_PADDING =   0.0f;

							const float X_TILE_SIZE_FLOAT = X_STRIDE * 1.0f / tex_tile->handle->buffer_desc.width;
							const float Y_TILE_SIZE_FLOAT = Y_STRIDE * 1.0f / tex_tile->handle->buffer_desc.height;

							for (int y = 0; y <  2; y++)
							for (int x = 0; x <  4; x++)
							{
								int   x_real  = X_START + X_STRIDE * x + X_PADDING;
								int   y_real  = Y_START + Y_STRIDE * y + Y_PADDING;
								float x_float = x_real * 1.0f / tex_tile->handle->buffer_desc.width;
								float y_float = y_real * 1.0f / tex_tile->handle->buffer_desc.height;

								TFixed x1 = _FXD(x_float);
								TFixed y1 = _FXD(y_float);
								TFixed x2 = x1 + _FXD(X_TILE_SIZE_FLOAT) - 1;
								TFixed y2 = y1 + _FXD(Y_TILE_SIZE_FLOAT) - 1;

								tex_tile->tile_info[40+y*4+x] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);
							}
						}

						// 50 ~ 59
						{
							const int X_START   = 0;
							const int Y_START   = 319;
							const int X_STRIDE  = 35;
							const int Y_STRIDE  = 57;
							const int X_PADDING = 0;
							const int Y_PADDING = 0;

							const float X_TILE_SIZE_FLOAT = 35 * 1.0f / tex_tile->handle->buffer_desc.width;
							const float Y_TILE_SIZE_FLOAT = 57 * 1.0f / tex_tile->handle->buffer_desc.height;

							for (int y = 0; y <  2; y++)
							for (int x = 0; x <  5; x++)
							{
								int   x_real  = X_START + X_STRIDE * x + X_PADDING;
								int   y_real  = Y_START + Y_STRIDE * y + Y_PADDING;
								float x_float = x_real * 1.0f / tex_tile->handle->buffer_desc.width;
								float y_float = y_real * 1.0f / tex_tile->handle->buffer_desc.height;

								TFixed x1 = _FXD(x_float);
								TFixed y1 = _FXD(y_float);
								TFixed x2 = x1 + _FXD(X_TILE_SIZE_FLOAT) - 1;
								TFixed y2 = y1 + _FXD(Y_TILE_SIZE_FLOAT) - 1;

								tex_tile->tile_info[50+y*5+x] = TTexCoord<PRIMITIVES_MODE_TRIANGLE_LIST>(x1, y1, x2, y2);
							}
						}
					}
				}
			}

			// map 의 geometry 를 표시하기 위한 class
			res_visible_3d[RES_VISIBLE3D_MAP].bind(new CVisibleGeometry(tex_tile));
			res_visible_3d[RES_VISIBLE3D_MAP_CHARA].bind(new CVisibleChara(tex_tile));
			res_visible_3d[RES_VISIBLE3D_MAP_SHADOW].bind(new CVisibleGeometryShadow);
			res_visible_3d[RES_VISIBLE3D_MAP_SOLID].bind(new CVisibleGeometrySolid(tex_tile));
			res_visible_3d[RES_VISIBLE3D_MAP_TEXT].bind(new CVisibleTextBox(tex_tile));
		}
		~TPresentationData()
		{
			avej::gfx3d::CDevice3D& device = avej_lite::singleton<CDevice3D>::get();
			device.DestroyTexture(tex_tile->handle);
		}

		inline static void VisibleActorShow(CVisibleActor<TShared3DObject> actor)
		{
			actor.GetVisible()->Show();
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	// interface

	static TPresentationData* s_p_presentation_data = 0;

	struct TPresentationImpl: public TPresentation
	{
	public:
		TPresentationImpl();
		virtual ~TPresentationImpl();

		virtual TPresentation& operator<<(const TWorld& ref_world);
		virtual TPresentation& operator<<(action::run<unsigned int>& run);
		virtual TPresentation& operator<<(action::run<TCommunication::TCommunicationData>& communication);

	private:
		TPresentationImpl(const TPresentation&);
		TPresentationImpl& operator=(const TPresentation&);
	};

	////////////////////////////////////////////////////////////////////////////////
	// implementation

	TPresentationImpl::TPresentationImpl()
	{
		assert(s_p_presentation_data == 0);

		s_p_presentation_data = new TPresentationData;
	}

	TPresentationImpl::~TPresentationImpl()
	{
		delete s_p_presentation_data;
		s_p_presentation_data = 0;
	}

	TPresentation& TPresentationImpl::operator<<(const TWorld& ref_world)
	{
		assert(s_p_presentation_data);

		// world에 있는 tile map을 얻어 온다.
		action::get<CSimpleTileMap*> get_tile_map;
		ref_world >> get_tile_map;

		// world에 있는 actor list를 얻어 와서, 그 actor들에 대한 고유한 이름을 통해 visible을 생성한다.
		action::get<const TActorList*> get_actor_list;
		ref_world >> get_actor_list;

		const TActorList* p_actor_list = get_actor_list.data;

		const char* sz_name = (*p_actor_list)[0]->GetName();

		s_p_presentation_data->visible_actor_cube.bind(new CVisibleActor<TShared3DObject>(s_p_presentation_data->cube.GetVisible(), (*p_actor_list)[0]));

		//?? 원래는 여기서 이름을 판별하고 이름별로 분기해야 함

		// visual object: map
		s_p_presentation_data->visible_actor_list.push_back(CVisibleActor<TShared3DObject>(s_p_presentation_data->res_visible_3d[TPresentationData::RES_VISIBLE3D_MAP]->GetVisible(), (*p_actor_list)[0]));
		// visual object: map shadow
		s_p_presentation_data->visible_actor_list.push_back(CVisibleActor<TShared3DObject>(s_p_presentation_data->res_visible_3d[TPresentationData::RES_VISIBLE3D_MAP_SHADOW]->GetVisible(), (*p_actor_list)[0]));
		// visual object: map solid
		s_p_presentation_data->visible_actor_list.push_back(CVisibleActor<TShared3DObject>(s_p_presentation_data->res_visible_3d[TPresentationData::RES_VISIBLE3D_MAP_SOLID]->GetVisible(), (*p_actor_list)[0]));
		// visual object: chara
		s_p_presentation_data->visible_actor_list.push_back(CVisibleActor<TShared3DObject>(s_p_presentation_data->res_visible_3d[TPresentationData::RES_VISIBLE3D_MAP_CHARA]->GetVisible(), (*p_actor_list)[0]));
		// visual object: text
		s_p_presentation_data->visible_actor_list.push_back(CVisibleActor<TShared3DObject>(s_p_presentation_data->res_visible_3d[TPresentationData::RES_VISIBLE3D_MAP_TEXT]->GetVisible(), (*p_actor_list)[0]));

		return *this;
	}

	TPresentation& TPresentationImpl::operator<<(action::run<unsigned int>& run)
	{
		assert(s_p_presentation_data);

		avej::gfx3d::CDevice3D& device = avej_lite::singleton<CDevice3D>::get();

		device.Clear(0xFF000000);

	#if 0
		// 샘플 model 출력
		s_p_presentation_data->visible_actor_cube->GetActor()->Act();
		s_p_presentation_data->visible_actor_cube->GetVisible()->Show();

		device.Flush();
		device.ProcessMessaage(10);

		return *this;
	#endif

		// 맵과 캐릭터 출력
		std::for_each(s_p_presentation_data->visible_actor_list.begin(), s_p_presentation_data->visible_actor_list.end(), TPresentationData::VisibleActorShow);

		device.Flush();
		device.ProcessMessaage(10);

		return *this;
	}

	TPresentation& TPresentationImpl::operator<<(action::run<TCommunication::TCommunicationData>& communication)
	{
//		s_LaunchSampleUI();
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////
	// static

	namespace 
	{
		TPresentation* GetPresentationInstance(void)
		{
			static TPresentationImpl s_presentation_kano;
			return &s_presentation_kano;
		}

		class TRegisterPresentation
		{
		public:
			TRegisterPresentation()
			{
				TPresentation::Register("Kano", GetPresentationInstance);
			}
		};

		static TRegisterPresentation s_resister_agent;
	}

} // namespace presentation_kano
