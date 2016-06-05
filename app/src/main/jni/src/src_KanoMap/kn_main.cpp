
#include "flat_board/flat_board.h"
#include "target/target_dep.h"
#include "kn_sprite_decoder.h"
#include "kn_util.h"
/*
#include <FBase.h>
#include <FGraphics.h>
#include <FMedia.h>
*/
#include <new>
#include <queue>

////////////////////////////////////////////////////////////////////////////////
// local

namespace kano
{
	struct TBufferDesc
	{
		void* p_start_address;
		int   width;
		int   height;
		int   bytes_per_line;
		int   bits_per_pixel;
	};

	void Init();
	void Done();
	bool Do(const TBufferDesc& buffer_desc);
}

////////////////////////////////////////////////////////////////////////////////
// type definition

namespace kano
{
	typedef flat_board::CFlatBoard<flat_board::PIXELFORMAT_RGB565>   CFlatBoard16;
	typedef flat_board::CFlatBoard<flat_board::PIXELFORMAT_ARGB8888> CFlatBoard32;
	typedef unsigned long TTargetPixel;
}

////////////////////////////////////////////////////////////////////////////////
// external

////////////////////////////////////////////////////////////////////////////////
// resource

namespace kano
{
	template <class TBaseClass>
	class auto_deletor
	{
	public:
		auto_deletor(TBaseClass* object = 0)
			: m_object(object)
		{
		}
		~auto_deletor(void)
		{
			delete[] m_object;
		}
		void operator()(TBaseClass* object)
		{
			m_object = object;
		}

	private:
		TBaseClass* m_object;
	};

	struct TPoint
	{
		int x, y;
	};

	struct TSize
	{
		int w, h;
	};

	struct TRect
	{
		int x, y, w, h;

		TRect()
			{}
		TRect(int _x, int _y, int _w, int _h)
			: x(_x),  y(_y),  w(_w),  h(_h) {}
	};
}

#define Bounds TRect

namespace kano
{
	struct TFloodFillDesc
	{
		int   x_start;
		int   y_start;
		int   seed;
		bool  (*fn_IsMarked)(void*, int x, int y);
		void  (*fn_SetMark)(void*, int x, int y, int val);
		void* p_private_data;

		TFloodFillDesc()
			: x_start(0), y_start(0), seed(0), fn_IsMarked(0), fn_SetMark(0), p_private_data(0)
		{
		}
	};

	void FloodFill4(const TFloodFillDesc& flood_fill_desc)
	{
		typedef std::pair<int, int> TPos;

		std::queue<TPos> q;

		q.push(std::make_pair(flood_fill_desc.x_start, flood_fill_desc.y_start));

		do
		{
			int x = q.front().first;
			int y = q.front().second;

			q.pop();

			if (flood_fill_desc.fn_IsMarked(flood_fill_desc.p_private_data, x, y))
				continue;

			flood_fill_desc.fn_SetMark(flood_fill_desc.p_private_data, x, y, flood_fill_desc.seed);

			q.push(std::make_pair(x-1, y));
			q.push(std::make_pair(x, y-1));
			q.push(std::make_pair(x, y+1));
			q.push(std::make_pair(x+1, y));
		} while (!q.empty());
	}

	struct TMap
	{
		struct TMapHeaderData
		{
			char   file_name[9];
			TPoint map_start;
			TPoint map_exit;
		};

		struct TMapHeader
		{
			enum { MAX_MAP_EXIT_POINT = 10 };

			char           id[7];
			char           map_code[5];
			TMapHeaderData map_header_data;
			bool           boundary_exit;
			short          handicap_flag;
			unsigned char  num_exit_point;
			unsigned char  friendship_level;
			bool           is_map_recording;
			bool           is_person_recording;
			TMapHeaderData map_exit_point[MAX_MAP_EXIT_POINT];
			TSize          map_size;
			char           midi[9];
			char           bmp[9];
			char           reserved[50-18];
		};

		struct TMapField
		{
			short          tile;
			short          wall[2];
			unsigned char  object;
			unsigned char  attribute;
			unsigned char  height;
			unsigned char  person;
		};

		TMapHeader     map_header;
		TMapField*     p_map_data;
		unsigned char* p_map_aux;

		TMap()
			: p_map_data(0), p_map_aux(0)
		{
		}
		~TMap()
		{
			delete   p_map_data;
			delete[] p_map_aux;
		}

		static bool IsInDoor(int tile)
		{
			return (tile >= 197 && tile < 277) || (tile >= 307 && tile < 347);
		}
		static bool IsInDoorWall(int tile)
		{
			return (tile >= 307 && tile < 307+19);
		}
		static bool IsInDoorFloor(int tile)
		{
			return IsInDoor(tile) && !IsInDoorWall(tile);
		}

		static bool IsMarked(void* private_data, int x, int y)
		{
			TMap* p_map = static_cast<TMap*>(private_data);

			if (x < 0 || x >= p_map->map_header.map_size.w || y < 0 || y >= p_map->map_header.map_size.h)
				return false;

			int  tile = p_map->GetMapTile(x, y);

			return !((IsInDoorFloor(tile)) && (p_map->GetMapAux(x, y) == 0));
		}

		static void SetMark(void* private_data, int x, int y, int val)
		{
			TMap* p_map = static_cast<TMap*>(private_data);

			if (x >= 0 && x < p_map->map_header.map_size.w && y >= 0 && y < p_map->map_header.map_size.h)
				p_map->p_map_aux[p_map->map_header.map_size.w * y + x] = val;
		}

		void LoadFromFile(const char* p_file_name)
		{
			target::file_io::CStreamReadFile file(p_file_name);

			// sizeof(map_header) -> 388
			// sizeof(TMapField)  ->  10

			file.Read(&map_header, sizeof(map_header));

			p_map_data = new TMapField[map_header.map_size.w * map_header.map_size.h];

			if (p_map_data)
			{
				file.Read(p_map_data, sizeof(TMapField) * map_header.map_size.w * map_header.map_size.h);
			}

			p_map_aux = new unsigned char[map_header.map_size.w * map_header.map_size.h];

			if (p_map_aux)
			{
				memset(p_map_aux, 0, map_header.map_size.w * map_header.map_size.h);
			}

			// process in-door floor
			{
				int index = 1;

				TFloodFillDesc flood_fill_desc;

				flood_fill_desc.fn_IsMarked    = IsMarked;
				flood_fill_desc.fn_SetMark     = SetMark;
				flood_fill_desc.p_private_data = (void*)this;

				for (int y = 0; y < map_header.map_size.h; y++)
				for (int x = 0; x < map_header.map_size.w; x++)
				{
					int  tile = this->GetMapTile(x, y);

					if (IsInDoorFloor(tile) && (GetMapAux(x, y) == 0))
					{
						flood_fill_desc.x_start = x;
						flood_fill_desc.y_start = y;
						flood_fill_desc.seed    = index++;

						FloodFill4(flood_fill_desc);
					}
				}
			}

			// process in-door wall
			{
				for (int y = 0; y < map_header.map_size.h; y++)
				for (int x = 0; x < map_header.map_size.w; x++)
				{
					int  tile = this->GetMapTile(x, y);

					if (IsInDoorWall(tile) && (GetMapAux(x, y) == 0))
					{
						int tile1 = this->GetMapTile(x, y+1);
						if (IsInDoorFloor(tile1))
						{
							SetMapAux(x, y, GetMapAux(x, y+1));
						}
						else
						{
							int tile2 = this->GetMapTile(x-1, y);
							if (IsInDoorFloor(tile2))
							{
								SetMapAux(x, y, GetMapAux(x-1, y));
							}
							else
							{
								int tile3 = this->GetMapTile(x-1, y+1);
								if (IsInDoorFloor(tile3))
									SetMapAux(x, y, GetMapAux(x-1, y+1));
							}
						}
					}
				}
			}
		}

		int GetMapTile(int x, int y)
		{
			if (x < 0 || x >= map_header.map_size.w)
				return 0;
			if (y < 0 || y >= map_header.map_size.h)
				return 0;

			return p_map_data[map_header.map_size.w * y + x].tile;
		}

		int GetMapHeight(int x, int y)
		{
			if (x < 0 || x >= map_header.map_size.w)
				return 0;
			if (y < 0 || y >= map_header.map_size.h)
				return 0;

			return p_map_data[map_header.map_size.w * y + x].height;
		}

		int GetMapWall1(int x, int y)
		{
			if (x < 0 || x >= map_header.map_size.w)
				return 0;
			if (y < 0 || y >= map_header.map_size.h)
				return 0;

			return p_map_data[map_header.map_size.w * y + x].wall[0];
		}

		int GetMapAux(int x, int y)
		{
			if (x < 0 || x >= map_header.map_size.w)
				return 0;
			if (y < 0 || y >= map_header.map_size.h)
				return 0;

			return p_map_aux[map_header.map_size.w * y + x];
		}

		void SetMapAux(int x, int y, int val)
		{
			if (x < 0 || x >= map_header.map_size.w)
				return;
			if (y < 0 || y >= map_header.map_size.h)
				return;

			p_map_aux[map_header.map_size.w * y + x] = val;
		}
	};
}

namespace kano
{
	const int MAX_BITMAP_TILE = 100 + 50 + 7 + 40 + 80 + 30 + 40 + 40; // 387

	enum
	{
	   BMP_NONE      = 0,
	   BMP_GENERAL   = 1,
	   BMP_WALL1     = 2,
	   BMP_TREE      = 3,
	   BMP_WALL2     = 4,
	   BMP_INTERIOR1 = 5,
	   BMP_CASTLE    = 6,
	   BMP_INTERIOR2 = 7,
	   BMP_WALL3     = 8
	};

	struct TTileInfo
	{
		long  texture_id;
		long  height;
		TRect src_rect;
	};

	struct TGameResource
	{
		TMap                               map;
		TTileInfo                          tile_info[MAX_BITMAP_TILE];
		CFlatBoard32                       pTileBitmap[8];
		auto_deletor<CFlatBoard32::TPixel> auto_relase_tile;

		CFlatBoard32                       wall_paper;
		auto_deletor<CFlatBoard32::TPixel> auto_relase_wall_paper;

		CKanoSpriteData                    sprite_data;

		TGameResource()
		{
			int i, j;
			int index = 0;

			for (j = 0; j < 5; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_GENERAL;
				tile_info[index].src_rect = Bounds(i*64,j*28,64,28);
				++index;
			}
			for (j = 0; j < 2; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_GENERAL;
				tile_info[index].src_rect = Bounds(i*64,j*50+28*5,64,50);
				++index;
			}
			for (j = 0; j < 1; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_GENERAL;
				tile_info[index].src_rect = Bounds(i*64,j*60+28*5+50*2,64,60);
				++index;
			}
			for (j = 0; j < 2; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_GENERAL;
				tile_info[index].src_rect = Bounds(i*64,j*70+28*5+50*2+60*1,64,70);
				++index;
			}

			// 100
			for (j = 0; j < 2; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL1;
				tile_info[index].src_rect = Bounds(i*64,j*130,64,130);
				++index;
			}
			for (j = 0; j < 3; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL1;
				tile_info[index].src_rect = Bounds(i*64,j*60+130*2,64,60);
				++index;
			}
			// 150
			for (i = 0; i < 3; i++)
			{
				tile_info[index].texture_id = BMP_TREE;
				tile_info[index].src_rect = Bounds(i*200,0,200,220);
				++index;
			}
			for (i = 0; i < 4; i++)
			{
				tile_info[index].texture_id = BMP_TREE;
				tile_info[index].src_rect = Bounds(i*150,220,150,150);
				++index;
			}

			// 157
			for (j = 0; j < 3; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL2;
				tile_info[index].src_rect = Bounds(i*64,j*130,64,130);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL2;
				tile_info[index].src_rect = Bounds(i*64,390,64,80);
				++index;
			}
			// 197
			for (j = 0; j < 5; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR1;
				tile_info[index].src_rect = Bounds(i*64,j*28,64,28);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR1;
				tile_info[index].src_rect = Bounds(i*64,28*5,64,60);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR1;
				tile_info[index].src_rect = Bounds(i*64,28*5+60,64,80);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR1;
				tile_info[index].src_rect = Bounds(i*64,28*5+60+80,64,130);
				++index;
			}

			// 277
			for (j = 0; j < 3; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_CASTLE;
				tile_info[index].src_rect = Bounds(i*64,j*140,64,140);
				++index;
			}

			// 307
			for (j = 0; j < 3; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR2;
				tile_info[index].src_rect = Bounds(i*64,j*130,64,130);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_INTERIOR2;
				tile_info[index].src_rect = Bounds(i*64,390,64,80);
				++index;
			}

			// 347
			for (j = 0; j < 3; j++)
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL3;
				tile_info[index].src_rect = Bounds(i*64,j*130,64,130);
				++index;
			}
			for (i = 0; i < 10; i++)
			{
				tile_info[index].texture_id = BMP_WALL3;
				tile_info[index].src_rect = Bounds(i*64,390,64,80);
				++index;
			}

			// 387
			tile_info[MAX_BITMAP_TILE].texture_id  = BMP_TREE;
			tile_info[MAX_BITMAP_TILE].src_rect  = Bounds(36,437,64,28);
			tile_info[MAX_BITMAP_TILE].height = 5;

			for (i =   0; i <= 64; i++)
				tile_info[i].height = 5;
			for (i =  65; i <= 69; i++)
				tile_info[i].height = 20;
			for (i =  70; i <= 74; i++)
				tile_info[i].height = 5;
			for (i =  75; i <= 79; i++)
				tile_info[i].height = 30;
			for (i =  80; i <= 99; i++)
				tile_info[i].height = 40;
			for (i = 100; i <=149; i++)
				tile_info[i].height = 5;
			for (i = 150; i <=152; i++)
				tile_info[i].height = 220;
			for (i = 153; i <=156; i++)
				tile_info[i].height = 150;
			for (i = 157; i <=196; i++)
				tile_info[i].height = 5;
			for (i = 197; i <=246; i++)
				tile_info[i].height = 5;
			for (i = 247; i <=256; i++)
				tile_info[i].height = 5;
			for (i = 257; i <=266; i++)
				tile_info[i].height = 5;
			for (i = 267; i <=276; i++)
				tile_info[i].height = 5;
			for (i = 277; i <=306; i++)
				tile_info[i].height = 5;
			for (i = 307; i <=346; i++)
				tile_info[i].height = 5;
			for (i = 347; i <=386; i++)
				tile_info[i].height = 5;

			////////////////////////////////////////////////////////////////////

			{
				Osp::Media::Image image;
				image.Construct();

				char fileName[] = "/Res/neto.jpg";

				Osp::Graphics::Bitmap* pTempBitmap = image.DecodeN(fileName, Osp::Graphics::BITMAP_PIXEL_FORMAT_ARGB8888);

				if (pTempBitmap)
				{
					Osp::Graphics::BufferInfo buffer_info;

					if (pTempBitmap->Lock(buffer_info) == E_SUCCESS)
					{
						int num_pixel = buffer_info.width * buffer_info.height;

						CFlatBoard32::TPixel* p_buffer = new CFlatBoard32::TPixel[num_pixel];
						auto_relase_wall_paper(p_buffer);

						memcpy(p_buffer, buffer_info.pPixels, num_pixel * sizeof(CFlatBoard32::TPixel));

						new (&wall_paper) CFlatBoard32(p_buffer, buffer_info.width, buffer_info.height, buffer_info.width, 0xFF000000);

						pTempBitmap->Unlock();
					}

					delete pTempBitmap;
				}
			}
		}

		~TGameResource()
		{
		}

		void LoadSprite(const char* sz_sprite_name)
		{
			new (&sprite_data) CKanoSpriteData(sz_sprite_name);
		}

		void LoadScript(const char* sz_script_name)
		{
			STD_STRING file_name_map(sz_script_name);
			file_name_map += ".hmp";

			STD_STRING file_name_image(sz_script_name);
			file_name_image.copy_to_front("/Res/");
			file_name_image += "@.bmp";

			map.LoadFromFile(file_name_map);

			{
				CFlatBoard32::TPixel* p_buffer = new CFlatBoard32::TPixel[640 * 480 * 8];
				auto_relase_tile(p_buffer);

				Osp::Media::Image image;
				image.Construct();

				for (int i = 0; i < int(sizeof(pTileBitmap) / sizeof(pTileBitmap[0])); i++)
				{
					char fileName[256];
					SPRINTF(fileName, file_name_image.c_str(), i+1);

					Osp::Graphics::Bitmap* pTempBitmap = image.DecodeN(fileName, Osp::Graphics::BITMAP_PIXEL_FORMAT_ARGB8888);

					if (pTempBitmap)
					{
						Osp::Graphics::BufferInfo buffer_info;

						if (pTempBitmap->Lock(buffer_info) == E_SUCCESS)
						{
							//??
							int num_pixel = 640*480;
							CFlatBoard32::TPixel* p = p_buffer + (i * num_pixel);
							memcpy(p, buffer_info.pPixels, num_pixel * sizeof(CFlatBoard32::TPixel));

							new (&pTileBitmap[i]) CFlatBoard32(p, 640, 480, 640, 0xFF000000);

							pTempBitmap->Unlock();
						}

						delete pTempBitmap;
					}
				}
			}
		}
	};

	static TGameResource* s_p_game_resource = 0;
}

////////////////////////////////////////////////////////////////////////////////
// object

////////////////////////////////////////////////////////////////////////////////
// local

namespace kano
{
	const int MAX_X_LINE    = 480;
	const int MAX_Y_LINE    = 480;

	const int TILE_X_SIZE   = 64;
	const int TILE_X_HALF   = TILE_X_SIZE / 2;
	const int TILE_Y_SIZE   = 24;
	const int TILE_Y_HEIGHT = 5;
	const int TILE_Y_HALF   = TILE_Y_SIZE / 2;

	const int WALL_HEIGHT   = 96;

	void DrawSprite(CFlatBoard32& dest_board, int x, int y, int z, int tile_index)
	{
		// assert(s_p_game_resource);

		const TRect& rect = s_p_game_resource->tile_info[tile_index].src_rect;

		int texture_id = s_p_game_resource->tile_info[tile_index].texture_id - 1;
		int revised_x = x - rect.w / 2;
		int revised_y = y - z - rect.h;

		dest_board.BitBlt(revised_x, revised_y, &s_p_game_resource->pTileBitmap[texture_id], rect.x, rect.y, rect.w, rect.h);
	}

	void DrawSprite(CFlatBoard32& dest_board, int x, int y, const TKanoSprite<unsigned short>& kano_sprite)
	{
		unsigned long* p_buffer_32 = new unsigned long[kano_sprite.width * kano_sprite.height];
		auto_deletor<CFlatBoard32::TPixel> auto_relase_buffer(p_buffer_32);

		{
			unsigned long* p_dest = p_buffer_32;

			for (int y = 0; y < kano_sprite.height; y++)
			for (int x = 0; x < kano_sprite.width; x++)
			{
				if (kano_sprite.p_buffer[y*kano_sprite.width + x] != 0xFFFF)
				{
					unsigned long r = (kano_sprite.p_buffer[y*kano_sprite.width + x] >> 8) & 0xF8;
					unsigned long g = (kano_sprite.p_buffer[y*kano_sprite.width + x] >> 3) & 0xFC;
					unsigned long b = (kano_sprite.p_buffer[y*kano_sprite.width + x] << 3) & 0xF8;

					r |= (r >> 5);
					g |= (g >> 6);
					b |= (b >> 5);

					*p_dest++ = (r << 16) | (g << 8) | b;;
				}
				else
				{
					*p_dest++ = 0;
				}
			}
		}

		CFlatBoard32 sour_board(p_buffer_32, kano_sprite.width, kano_sprite.height, kano_sprite.width, 0x0);

		int revised_x = x - kano_sprite.width / 2 - TILE_X_HALF;
		int revised_y = y - kano_sprite.height - TILE_Y_HEIGHT;

		dest_board.BitBlt(revised_x, revised_y, &sour_board, 0, 0, kano_sprite.width, kano_sprite.height);
	}

	int Abs2MapCoordX(int xAbs, int yAbs)
	{
		// assert(s_p_game_resource);
		return ((xAbs / TILE_X_HALF + (s_p_game_resource->map.map_header.map_size.w - yAbs / TILE_Y_HALF)) / 2);
	}

	int Abs2MapCoordY(int xAbs, int yAbs)
	{
		// assert(s_p_game_resource);
		return ((xAbs / TILE_X_HALF - (s_p_game_resource->map.map_header.map_size.w - yAbs / TILE_Y_HALF)) / 2);
	}

	int Map2AbsCoordX(int xMap, int yMap)
	{
		return ((xMap + yMap) * TILE_X_HALF);
	}

	int Map2AbsCoordY(int xMap, int yMap)
	{
		// assert(s_p_game_resource);
	   return (((s_p_game_resource->map.map_header.map_size.w - xMap) + yMap) * TILE_Y_HALF);
	}

	void DrawQuaterViewEx(CFlatBoard32& dest_board, int xViewPos, int yViewPos, bool is_outdoor)
	{
		if (s_p_game_resource == 0)
			return;

		static int s_toggle_coll_time = 2;
		static int s_toggle = 1;

		if (s_toggle_coll_time-- <= 0)
		{
			s_toggle_coll_time = 1 + rand()%3;
			s_toggle = 1 - s_toggle;
		}

		int xMapCenter = Abs2MapCoordX(xViewPos, yViewPos);
		int yMapCenter = Abs2MapCoordY(xViewPos, yViewPos);
		int xPitchCenter = Map2AbsCoordX(xMapCenter, yMapCenter) - xViewPos;
		int yPitchCenter = Map2AbsCoordY(xMapCenter, yMapCenter) - yViewPos;
		int matched_aux;
		{
			int tile    = s_p_game_resource->map.GetMapTile(xMapCenter, yMapCenter);
			int id      = s_p_game_resource->tile_info[tile].texture_id;

			is_outdoor  = !((id == BMP_INTERIOR1) || (id == BMP_INTERIOR2));

			matched_aux = s_p_game_resource->map.GetMapAux(xMapCenter, yMapCenter);
		}

		xViewPos -= (MAX_X_LINE / 2) + TILE_X_SIZE;
		yViewPos -= (MAX_Y_LINE / 2) + TILE_Y_SIZE;

		int xMapOrg = Abs2MapCoordX(xViewPos, yViewPos);
		int yMapOrg = Abs2MapCoordY(xViewPos, yViewPos);

		int xPosOrg = -TILE_X_HALF;
		int yPosOrg = -TILE_Y_HALF;

		int xPitch = Map2AbsCoordX(xMapOrg, yMapOrg) - xViewPos;
		int yPitch = Map2AbsCoordY(xMapOrg, yMapOrg) - yViewPos;

		for (int y = 0; y <= (MAX_Y_LINE / TILE_Y_HALF) + 20; y++)
		{
			int xTilePos = xPosOrg + 0;
			int yTilePos = yPosOrg + y * TILE_Y_HALF - TILE_Y_HALF;

			xTilePos += xPitch;
			yTilePos += yPitch;

			int xMapPos  = xMapOrg - y / 2;
			int yMapPos  = yMapOrg + (y + 1) / 2;

			if (y % 2)
				xTilePos += TILE_X_HALF;

			for (int x = 0; x <= (MAX_X_LINE / TILE_X_HALF) + 1; x++)
			{
				int aux    = s_p_game_resource->map.GetMapAux(xMapPos, yMapPos);

				if (is_outdoor || (aux == matched_aux))
				{
					int tile   = s_p_game_resource->map.GetMapTile(xMapPos, yMapPos);
					int height = s_p_game_resource->map.GetMapHeight(xMapPos, yMapPos);

					if (is_outdoor)
					{
						switch (s_p_game_resource->tile_info[tile].texture_id)
						{
						case BMP_WALL1:
						case BMP_WALL2:
						case BMP_WALL3:
						case BMP_CASTLE:
							DrawSprite(dest_board, xTilePos, yTilePos, 0, tile);
							break;
						case BMP_GENERAL:
						case BMP_TREE:
							DrawSprite(dest_board, xTilePos, yTilePos, height * s_p_game_resource->tile_info[tile].height, tile);
							break;
						case BMP_INTERIOR1:
						case BMP_INTERIOR2:
						default:
							break;
						}

						{
							int wall1 = s_p_game_resource->map.GetMapWall1(xMapPos, yMapPos);

							if (wall1 > 0)
							{
								if (s_p_game_resource->tile_info[wall1].texture_id == BMP_TREE)
									DrawSprite(dest_board, xTilePos, yTilePos - 7, height * s_p_game_resource->tile_info[tile].height, wall1);
								else
									DrawSprite(dest_board, xTilePos, yTilePos - WALL_HEIGHT, height * s_p_game_resource->tile_info[tile].height, wall1);
							}
						}
					}
					else
					{
						if (s_toggle)
						{
							if (tile == 270+0)
								tile++;
							else if (tile == 271+0)
								tile--;
							else if (tile == 273+0)
								tile++;
							else if (tile == 274+0)
								tile--;
						}

						switch (s_p_game_resource->tile_info[tile].texture_id)
						{
						case BMP_INTERIOR1:
						case BMP_INTERIOR2:
							DrawSprite(dest_board, xTilePos, yTilePos, 0, tile);
							break;
						case BMP_WALL1:
						case BMP_WALL2:
						case BMP_WALL3:
						case BMP_CASTLE:
						case BMP_GENERAL:
						case BMP_TREE:
						default:
							break;
						}
					}
				}

				if (xMapPos == xMapCenter && yMapPos == yMapCenter)
				{
					DrawSprite(dest_board, xTilePos, yTilePos, 0, 1);

					const TKanoSprite<unsigned short>& kano_sprite = s_p_game_resource->sprite_data.GetData16(50);
					DrawSprite(dest_board, xTilePos-xPitchCenter, yTilePos-yPitchCenter, kano_sprite);

					goto LOOP_EXIT;
				}

				xTilePos += TILE_X_SIZE;

				++xMapPos;
				++yMapPos;
			}
		}
LOOP_EXIT:
		;
/*
		{
			int x = 50;
			for (int i = 50; i < 60; i++)
			{
				const TKanoSprite& kano_sprite = s_p_game_resource->sprite_data.GetData(i);
				DrawSprite(dest_board, x, 100, kano_sprite);
				x += (kano_sprite.width + 10);
			}
			x = 60;
			for (int i = 60; i < 70; i++)
			{
				const TKanoSprite& kano_sprite = s_p_game_resource->sprite_data.GetData(i);
				DrawSprite(dest_board, x, 200, kano_sprite);
				x += (kano_sprite.width + 10);
			}
			x = 70;
			for (int i = 70; i < 80; i++)
			{
				const TKanoSprite& kano_sprite = s_p_game_resource->sprite_data.GetData(i);
				DrawSprite(dest_board, x, 300, kano_sprite);
				x += (kano_sprite.width + 10);
			}
			x = 80;
			for (int i = 80; i < 90; i++)
			{
				const TKanoSprite& kano_sprite = s_p_game_resource->sprite_data.GetData(i);
				DrawSprite(dest_board, x, 400, kano_sprite);
				x += (kano_sprite.width + 10);
			}
		}
*/
	}
}

////////////////////////////////////////////////////////////////////////////////
// main

void kano::Init()
{
	s_p_game_resource = new TGameResource;
	s_p_game_resource->LoadScript("parola");
	s_p_game_resource->LoadSprite("AIAS.DAT");
/*
	{
		HDC dc = GetDC(0);

		for (int y = 0; y < data.height; y++)
		for (int x = 0; x < data.width; x++)
		{
			if (data.p_buffer[y*data.width + x] != 0xFFFF)
			{
				COLORREF r = (data.p_buffer[y*data.width + x] >> 8) & 0xF8;
				COLORREF g = (data.p_buffer[y*data.width + x] >> 3) & 0xFC;
				COLORREF b = (data.p_buffer[y*data.width + x] << 3) & 0xF8;

				r |= (r >> 5);
				g |= (g >> 6);
				b |= (b >> 5);

				COLORREF color = (b << 16) | (g << 8) | r;

				SetPixel(dc, x, y, color);
			}
		}

		ReleaseDC(0, dc);
	}
*/
}

void kano::Done()
{
	delete s_p_game_resource;
	s_p_game_resource = 0;
}

bool kano::Do(const TBufferDesc& buffer_desc)
{
	if (buffer_desc.bits_per_pixel != 32)
		return false;

	static int s_x_view = 1862;
	static int s_y_view = 1276;

	static bool s_is_outdoor = true;

	// object control
	{
		target::IInputDevice* p_input_device = target::GetInputDevice();

		if (p_input_device)
		{
#if 0
			const int MOVE_PIXEL = 8;

			p_input_device->Update();

			if (p_input_device->CheckKeyPressed(target::KEY_LEFT))
				s_x_view -= MOVE_PIXEL;
			if (p_input_device->CheckKeyPressed(target::KEY_RIGHT))
				s_x_view += MOVE_PIXEL;
			if (p_input_device->CheckKeyPressed(target::KEY_UP))
				s_y_view -= MOVE_PIXEL;
			if (p_input_device->CheckKeyPressed(target::KEY_DOWN))
				s_y_view += MOVE_PIXEL;
#else
			static bool s_is_pressing = false;
			static int  s_pressing_x  = 0;
			static int  s_pressing_y  = 0;
			static int  s_pressing_x_view = 0;
			static int  s_pressing_y_view = 0;

			target::TInputUpdateInfo input_info = p_input_device->Update();

			if (input_info.is_touched)
			{
				if (s_is_pressing)
				{
					s_x_view = s_pressing_x_view + (s_pressing_x - input_info.touch_pos.x);
					s_y_view = s_pressing_y_view + (s_pressing_y - input_info.touch_pos.y);
				}
				else
				{
					s_is_pressing = true;
					s_pressing_x = input_info.touch_pos.x;
					s_pressing_y = input_info.touch_pos.y;
					s_pressing_x_view = s_x_view;
					s_pressing_y_view = s_y_view;
				}
			}
			else
			{
				s_is_pressing = false;
			}

			if (p_input_device->CheckKeyPressed(target::KEY_LEFT) &&
				p_input_device->CheckKeyPressed(target::KEY_RIGHT) &&
				p_input_device->CheckKeyPressed(target::KEY_UP) &&
				p_input_device->CheckKeyPressed(target::KEY_DOWN))
			{
				s_is_outdoor = !s_is_outdoor;
			}
#endif
		}
	}

	// display
	{
		const int GUIDE_WIDTH  = 480;
		const int GUIDE_HEIGHT = 480;

		int game_width  = (buffer_desc.width  > GUIDE_WIDTH ) ? GUIDE_WIDTH  : buffer_desc.width;
		int game_height = (buffer_desc.height > GUIDE_HEIGHT) ? GUIDE_HEIGHT : buffer_desc.height;
		int game_x      = (buffer_desc.width  - game_width ) >> 1;
		int game_y      = (buffer_desc.height - game_height) >> 1;
		int game_pitch  = (buffer_desc.bytes_per_line << 3) / buffer_desc.bits_per_pixel;

		// top-align
		game_y = 0;

		CFlatBoard32::TPixel* p_game_buffer = static_cast<CFlatBoard32::TPixel*>(buffer_desc.p_start_address);
		p_game_buffer += ((game_pitch * game_y) + game_x);

		CFlatBoard32 dest_board(p_game_buffer, game_width, game_height, game_pitch);

		dest_board.BitBlt(0, 0, &s_p_game_resource->wall_paper, 0, 0, 480, 480);

		DrawQuaterViewEx(dest_board, s_x_view, s_y_view, s_is_outdoor);

		// AppLog("[KANO] map_x_view = %d, map_y_view = %d", s_x_view, s_y_view);
	}

	return true;
}
