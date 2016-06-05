
/*

  물의 정령 조인했을 때 자동을 status 맞춰줘야 함
  여관에 돈이 없을 경우...
  음식파는 장사꾼 추가

*/

/*

DEPAPEPE
코레 하마리마스 (이거 중독성이 있어요)


BUG list

저장한 것을 부르면 default tile이 사라짐
예> ground1에서 save를 했는데도 0번 이 나무 모양이 아니라 까만색이 됨


TODO list

 - CONSOLE_WRITE 사용 법 재고, 적어도 szTemp를 외부 선언 해서는 안됨

- 동굴에 들어가면 불이 꺼지기
	시야가 좁아지거나 색이 바뀌는 것은?

- 문자열을 resource 化
	UPcPlayer 하던 중

- resource file을 지정하여 모든 파일을 하나의 파일에 저장
	실제로는 file과 resource 형태 둘 다 지원해야 함
	또는, code로 resouce 지정 가능한 형태

- 한글 입력 방법 만들기

- character 생성하는 것 만들기

- press any key 가 아니라 press enter key 같은 것이 되어야 하지않는가?

- g_?? 함수들은 모두 global을 뜻하는 namespace로 정리
	main.h 에 있는 아래의 두 함수가 대상임
	void g_DrawBltList(int x, int y, CSmFont12x12::TBltParam rect[], int nRect = 0x7FFFFFFF);
	void g_DrawText(int x, int y, const char* szText, unsigned long color = 0xFFFFFFFF);

- event에 사용되는 함수들을 매뉴얼로 정리
- 스크립트 전투시 전투 결과를 받는 방법
  - 전투에서 도망쳤을 때 회피가 불가능 하게 하는 시나리오
  - 전투에서 도망쳤을 때 남은 수 만큼만 다시 나타나는 시나리오

- Doxygen 문서 만들기



개선 사항

- resource 암호화 seed: byte; adder: byte 의 xor만으로 암호화

- 헤더 딸린 자체 map file 구조 필요, 가능하다면 3탄의 map 구조도 사용 가능

- script를 파일 단위로 미리 읽어 놓는 방식으로 속도 개선
- 게임 내의 singleton 정리
	Script 객체
	GameMain 객체
- factory 정리


CODING GUIDE

- localization 고려하여 문자열을 따로 관리
- 16-bit color에 대한 conversion이 항상 필요
- strcpy -> strncpy
- sprintf -> snprintf
- class 정의
  - 저장하는 변수는 protected로
  - private 변수/함수는 m_*
  - protected 변수는 원래 이름, 함수는 _*
  - public 변수는 허용하지 않음, 함수는 원래 이름
- ??(구현 해야 하는 부분) @@(개선 사항)

- singleton 목록
	CLoreConsole


  원작의 버그 목록

- 특수 기술을 얻기만 해도 일반 ESP를 사용할 수 있음
- 독심술을 사용해도 ESP 지수가 30이 빠지지 않음
- 적을 아군으로 만들 때 20레벨인 적의 경험치가 5100000가 아닌 510000만 할당됨
- 적이 독심술을 걸어 6번째 아군을 데려갈 수 있는데, 6번째 아군이 적의 n번이 되는 것이 아니라 n번째 아군이 적의 6번이 된다. 하지만 사라지는 것은 6번째 아군이다.
- 예언는/천리안는 전투모드에서는 사용할 수가 없습니다. <- 조사가 틀렸음

*/

////////////////////////////////////////////////////////////////////////////////
// uses

#pragma warning( disable: 4786 )

#include <vector>
#include <algorithm>

#include <assert.h>
#include <string.h>

#include "USmStream.h"
#include "USmSet.h"
#include "USmSola.h"

#include "UExtern.h"
#include "UScript.h"
#include "UConsole.h"
#include "UKeyBuffer.h"
#include "USelect.h"
#include "UResString.h"
#include "USound.h"
#include "UMap.h"
#include "UGameOption.h"
#include "UMain.h"
#include "USerialize.h"

#include "UPcParty.h"
#include "UPcNameBase.h"
#include "UPcEnemy.h"
#include "UPcPlayer.h"

#include "avej_lite.h"

#include "util_convert_to_ucs.h"


////////////////////////////////////////////////////////////////////////////////
// type definition

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// global variables

IGfxDevice*  pGfxDevice   = NULL;
IGfxSurface* pBackBuffer  = NULL;
IGfxSurface* pTileImage   = NULL;
IGfxSurface* pSpriteImage = NULL;
IGfxSurface* pFontImage   = NULL;

static avej_lite::TLayerDesc s_layer_option =
{
	1,
	{
		avej_lite::TLayerDesc::LAYER_ATTRIB_ALPHA_TEST | avej_lite::TLayerDesc::LAYER_ATTRIB_ALPHA_BLEND,
	},
	avej_lite::TLayerDesc::OPTION_NONE
};

////////////////////////////////////////////////////////////////////////////////
//

enum { MAX_COLOR_TABLE = 16 };

static const unsigned long COLOR_TABLE[MAX_COLOR_TABLE] =
{
	0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
	0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
	0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
	0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
};

unsigned long g_GetColorFromIndexedColor(unsigned long index, unsigned long default_color)
{
	static bool s_is_first = true;
	static unsigned long s_localTable[MAX_COLOR_TABLE];

	if (s_is_first)
	{
		for (int index = 0; index < sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]); index++)
			s_localTable[index] = pBackBuffer->Color(COLOR_TABLE[index]);

		s_is_first = false;
	}

	return (index < sizeof(s_localTable) / sizeof(s_localTable[0])) ? s_localTable[index] : default_color;
}

////////////////////////////////////////////////////////////////////////////////
//

template <typename pixel>
static void s_DrawBltList(int x, int y, TBltParam rect[], int nRect)
{
	static bool s_is_first = true;
	static unsigned long s_white;

	if (s_is_first)
	{
		ASSERT(pBackBuffer);
		s_white = pBackBuffer->Color(0xFF, 0xFF, 0xFF, 0xFF);
		s_is_first = false;
	}

	for (int i = 0; i < nRect; i++)
	{
		if (!rect[i].is_available)
			break;

		unsigned long color[4] = { rect[i].color, rect[i].color, rect[i].color, rect[i].color };

		pBackBuffer->BitBlt(x + rect[i].dst_bound.x1, y + rect[i].dst_bound.y1, pFontImage, rect[i].src_bound.x1, rect[i].src_bound.y1, rect[i].src_bound.x2 - rect[i].src_bound.x1, rect[i].src_bound.y2 - rect[i].src_bound.y1, color);
	}
}

void g_DrawBltList(int x, int y, TBltParam rect[], int nRect)
{
	s_DrawBltList<avej_::pixel>(x, y, rect, nRect);
}

void g_DrawFormatedText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long default_color, IorzRenderText::TFnBitBlt fn_bit_blt)
{
	// 항상 UCS-2 형식의 wchar_t로 컴파일 되어야 한다.
	CT_ASSERT(sizeof(widechar) == sizeof(wchar_t), QAZ);

	static const IorzRenderText& ref_render_text = *GetTextInstance();

	if (sz_text_begin == NULL || sz_text_end == NULL || sz_text_begin >= sz_text_end )
		return;

	//?? 현재는 이 사이즈로 고정
	static widechar w_str[256];

	// KSC-5601을 UCS-2로 변경
	{
		widechar* w_str_end = ConvertUHC2UCS2(w_str, 256, sz_text_begin, sz_text_end - sz_text_begin);
		*w_str_end = 0;
	}

	{
		const int DEFAULT_INDEX_COLOR = 15;

		widechar* p_str_begin  = w_str;
		widechar* p_str_end    = w_str;
		int       x_end        = x;

		unsigned long   current_color = default_color;

		while (*p_str_end)
		{
			if (*p_str_end == widechar('@'))
			{
				// NULL 문자로 끝내기, '@'가 NULL로 치환되는 것이기 때문에 조작해도 문제 없음
				*p_str_end = 0;

				// blitting list 만들기
				ref_render_text.RenderText(x, y, p_str_begin, current_color, fn_bit_blt);

				// x 시작값 갱신
				x = x_end;

				// current_color 갱신
				{
					widechar index_char = *(++p_str_end);
					int      index_color = -1;

					if      (index_char >= widechar('0') && index_char <= widechar('9'))
						index_color = index_char - '0';
					else if (index_char >= widechar('A') && index_char <= widechar('Z'))
						index_color = index_char - 'A' + 10;
					else if (index_char >= widechar('a') && index_char <= widechar('z'))
						index_color = index_char - widechar('A') + 10;
					else if (index_char == widechar('@'))
						current_color = default_color; // 복구

					if (index_color >= 0)
						current_color = g_GetColorFromIndexedColor(index_color, default_color);
				}

				// 문자열의 시작을 갱신
				p_str_begin = ++p_str_end;
			}
			else
			{
				//?? 상수가 들어 가면 안됨
				x_end += (*p_str_end > 0x00FF) ? 12 : 6;
				++p_str_end;
			}
		}

		// 남은 문자열에 대해 blitting list 만들기
		ref_render_text.RenderText(x, y, p_str_begin, current_color, fn_bit_blt);
	}
}

static void s_RenderText(int x_dest, int y_dest, int width, int height, int x_sour, int y_sour, unsigned long color)
{
	unsigned long color_list[4] = { color, color, color, color };
	pBackBuffer->BitBlt(x_dest, y_dest, pFontImage, x_sour, y_sour, width, height, color_list);
}

void g_DrawText(int x, int y, const char* sz_text, unsigned long color)
{
	const char* sz_text_end = sz_text + strlen(sz_text);
	g_DrawFormatedText(x, y, sz_text, sz_text_end, color, s_RenderText);
/*
	CSmFont12x12 s_font;
	CSmFont12x12::TBltParam rect[20];

	if (color == 0xFFFFFFFF)
		color = pBackBuffer->Color(color);

	if (s_font.RenderText(x, y, szText, color, rect, 20))
		s_DrawBltList<avej_::pixel>(0, 0, rect, 20);
*/
}

////////////////////////////////////////////////////////////////////////////////
// macro

#define CONSOLE_WRITE(format, var) \
	SPRINTF(szTemp, 256, format, var); \
	console.Write(szTemp);

#define CONSOLE_WRITE2(format, var1, var2) \
	SPRINTF(szTemp, 256, format, var1, var2); \
	console.Write(szTemp);

#define CONSOLE_WRITE4(format, var1, var2, var3, var4) \
	SPRINTF(szTemp, 256, format, var1, var2, var3, var4); \
	console.Write(szTemp);

#define NUM_OF_CONSCIOUS_PLAYER(player) std::for_each(player.begin(), player.end(), FnctNumOfConscious<TPcPlayer*>()).Result()
#define NUM_OF_CONSCIOUS_ENEMY(enemy) std::for_each(enemy.begin(), enemy.end(), FnctNumOfConscious<TPcEnemy*>()).Result()

////////////////////////////////////////////////////////////////////////////////
// GetSaveFileName class;

#define SAVE_FILE_NAME "gamedat"
#define SAVE_FILE_EXT  ".sav"

class GetSaveFileName
{
private:
	avej_lite::util::string m_name;
public:
	GetSaveFileName(int index)
	{
		m_name += "./";
		m_name += SAVE_FILE_NAME;
		m_name += IntToStr(index)();
		m_name += SAVE_FILE_EXT;
	}
	~GetSaveFileName(void)
	{
		m_name += "./";
	}
	operator const char*(void)
	{
		return m_name;
	}
};

////////////////////////////////////////////////////////////////////////////////
// GameMain class;

extern CGameMain* s_pGameMain;

class CGameMain
{
private:
	/* functor */

	// window의 갱신
	template <class type>
	class FnctDisplay
	{
	public:
		void operator()(type obj)
		{
			obj->Display();
		}
	};

	// 아군의 상태 점검
	template <class type>
	class FnctCheckCondition
	{
	public:
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				obj->CheckCondition();
			}
		}
	};

	// 의식이 남아 있는 player의 수
	template <class type>
	class FnctNumOfConscious
	{
		int nAlive;

	public:
		FnctNumOfConscious(void)
			: nAlive(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->IsConscious())
				++nAlive;
		}
		int Result(void)
		{
			// 살아 있는 player의 수
			return nAlive;
		}
	};

	// 파티가 swamp로 이동했을 때 적용되는 규칙
	template <class type>
	class FnctEnterSwamp
	{
		int nUpdate;

	public:
		FnctEnterSwamp(void)
			: nUpdate(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				if (AvejUtil::Random(20) + 1 >= obj->luck)
				{
					game::console::WriteConsole(13, 2, obj->GetName(EJOSA_SUB), " 중독되었다.");

					if (obj->poison == 0)
						obj->poison = 1;

					++nUpdate;
				}
			}
		}
		int Result(void)
		{
			// 데이터가 갱신된 player의 수
			return nUpdate;
		}
	};

	// 파티가 lava로 이동했을 때 적용되는 규칙
	template <class type>
	class FnctEnterLava
	{
		int nUpdate;

	public:
		FnctEnterLava(void)
			: nUpdate(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				int damage = AvejUtil::Random(40) + 40;

				if (obj->luck > 0)
					damage -= (2 * AvejUtil::Random(obj->luck));

				if (damage > 0)
				{
					game::console::WriteConsole(13, 4, obj->GetName(EJOSA_SUB), " ", IntToStr(damage)(), "의 피해를 입었다 !");

					if ((obj->hp > 0) && (obj->unconscious == 0))
					{
						obj->hp -= damage;
						if (obj->hp <= 0)
							obj->unconscious = 1;
					}
					else if ((obj->hp > 0) && (obj->unconscious > 0))
					{
						obj->hp -= damage;
					}
					else if ((obj->unconscious > 0) && (obj->dead == 0))
					{
						obj->unconscious += damage;
						if (obj->unconscious > obj->endurance * obj->level[0])
							obj->dead = 1;
					}
					else if (obj->dead > 0)
					{
						if (obj->dead + damage > 30000)
							obj->dead = 30000;
						else
							obj->dead += damage;
					}

					++nUpdate;
				}
			}
		}
		int Result(void)
		{
			// 데이터가 갱신된 player의 수
			return nUpdate;
		}
	};

	// 시간이 가거나 파티가 이동했을 때 적용되는 규칙
	template <class type>
	class FnctTimeGoes
	{
		int nUpdate;

	public:
		FnctTimeGoes(void)
			: nUpdate(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				if (obj->poison > 0)
					++obj->poison;

				if (obj->poison > 10)
				{
					obj->poison = 1;
					if ((obj->dead > 0) && (obj->dead < 100))
					{
						++obj->dead;
					}
					else if (obj->unconscious > 0)
					{
						++obj->unconscious;
						if (obj->unconscious > obj->endurance * obj->level[0])
							obj->dead = 1;
					}
					else
					{
						--obj->hp;
						if (obj->hp <= 0)
							obj->unconscious = 1;
					}
					++nUpdate;
				}
			}
		}
		int Result(void)
		{
			// 데이터가 갱신된 player의 수
			return nUpdate;
		}
	};

	// 파티가 캠프를 할 때 적용되는 규칙
	template <class type>
	class FnctRestHere
	{
	public:
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				if (s_pGameMain->party.food <= 0)
				{
					game::console::WriteConsole(4, 1, "일행은 식량이 바닥났다");
				}
				else if (obj->dead > 0)
				{
					game::console::WriteConsole(7, 2, obj->GetName(EJOSA_SUB), " 죽었다");
				}
				else if ((obj->unconscious > 0) && (obj->poison == 0))
				{
					obj->unconscious -= (obj->level[0] + obj->level[1] + obj->level[2]);
					if (obj->unconscious <= 0)
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " 의식이 회복되었다");

						obj->unconscious = 0;
						if (obj->hp <= 0)
							obj->hp = 1;

						--s_pGameMain->party.food;
					}
					else
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " 여전히 의식 불명이다");
					}
				}
				else if ((obj->unconscious > 0) && (obj->poison > 0))
				{
					game::console::WriteConsole(7, 3, "독 때문에 ", obj->GetName(EJOSA_NONE), "의 의식은 회복되지 않았다");
				}
				else if (obj->poison > 0)
				{
					game::console::WriteConsole(7, 3, "독 때문에 ", obj->GetName(EJOSA_NONE), "의 건강은 회복되지 않았다");
				}
				else
				{
					int i = (obj->level[0] + obj->level[1] + obj->level[2]) * 2;
					if (obj->hp >= obj->endurance * obj->level[0])
					{
						if (s_pGameMain->party.food < 255)
							++s_pGameMain->party.food;
					}

					obj->hp += i;
					if (obj->hp >= obj->endurance * obj->level[0])
					{
						obj->hp = obj->endurance * obj->level[0];
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " 모든 건강이 회복되었다");
					}
					else
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " 치료되었다");
					}
					--s_pGameMain->party.food;
				}

				obj->sp  = obj->mentality * obj->level[1];
				obj->esp = obj->concentration * obj->level[2];
			}
		}
	};

	// 캐릭터 리스트의 평균 행운을 돌려주는 함수자  
	template <class type>
	class FnctAvgLuck
	{
		int accLuck;
		int nPlayer;

	public:
		FnctAvgLuck(void)
			: accLuck(0), nPlayer(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				++nPlayer;
				accLuck += obj->luck;
			}
		}
		int Result(void)
		{
			return (nPlayer > 0) ? (accLuck / nPlayer) : 0;
		}
	};

	// 캐릭터 리스트의 평균 민첩성을 돌려주는 함수자  
	template <class type>
	class FnctAvgAgility
	{
		int accAgility;
		int nPlayer;

	public:
		FnctAvgAgility(void)
			: accAgility(0), nPlayer(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				++nPlayer;
				accAgility += obj->agility;
			}
		}
		int Result(void)
		{
			return (nPlayer > 0) ? (accAgility / nPlayer) : 0;
		}
	};

	// 캐릭터 리스트의 평균 AC를 돌려주는 함수자  
	template <class type>
	class FnctAvgAC
	{
		int accAC;
		int nPlayer;

	public:
		FnctAvgAC(void)
			: accAC(0), nPlayer(0)
		{
		}
		void operator()(type obj)
		{
			if (obj->Valid())
			{
				++nPlayer;
				accAC += obj->ac;
			}
		}
		int Result(void)
		{
			return (nPlayer > 0) ? (accAC / nPlayer) : 0;
		}
	};

	// 적들이 제공할 수 있는 황금의 개수를 계산하는 함수자
	template <class type>
	class FnctPlusGold
	{
		int nGold;
	public:
		FnctPlusGold(void)
			: nGold(0)
		{
		}
		void operator()(type obj)
		{
			const TEnemyData& enemyData = GetEnemyData(obj->E_number);

			int ac = enemyData.ac;
			if (ac < 0)
				ac = 1;

			int level = enemyData.level;

			nGold += level * level * level * ac;
		}
		int Result(void)
		{
			return nGold;
		}
	};

private:
	/* action */

	void m_ActBlock(int x1, int y1, bool bUseless)
	{
		return;
	}
	void m_ActMove(int x1, int y1, bool bEncounter)
	{
		party.Move(x1, y1);

		window[WINDOW_MAP]->SetUpdateFlag();

		int nUpdate;

		nUpdate = std::for_each(player.begin(), player.end(), FnctTimeGoes<TPcPlayer*>()).Result();

		// player의 디스플레이 된 수치에 변화가 생겼다면
		if (nUpdate > 0)
		{
			window[WINDOW_STATUS]->SetUpdateFlag();
		}

		DetectGameOver();
		
		// 독심술을 사용 중이라면 사용 시간을 감소 시킴
		if (party.ability.mindControl > 0)
			--party.ability.mindControl;

		if (bEncounter)
		{
			if (AvejUtil::Random(party.encounter*20) == 0)
				EncounterEnemy();
		}
	}
	void m_ActEvent(int x1, int y1, bool bUseless)
	{
		party.Move(x1, y1);

		window[WINDOW_MAP]->SetUpdateFlag();

		GetConsole().Clear();

		CScript script(CScript::MODE_EVENT, 0, party.x, party.y);

		return;
	}
	void m_ActEnter(int x1, int y1, bool bUseless)
	{
		GetConsole().Clear();
		CScript script(CScript::MODE_ENTER, 0, party.x + x1, party.y + y1);
	}
	void m_ActSign(int x1, int y1, bool bUseless)
	{
		CLoreConsole& console = GetConsole();

		console.Clear();
		console.SetTextColorIndex(7);
		console.Write("표지판에는 다음과 같이 적혀 있었다.");
		console.Write("");

		CScript script(CScript::MODE_SIGN, 0, party.x + x1, party.y + y1);
	}
	void m_ActWater(int x1, int y1, bool bUseless)
	{
		if (party.ability.walkOnWater > 0)
		{
			--party.ability.walkOnWater;
			party.Move(x1, y1);
			window[WINDOW_MAP]->SetUpdateFlag();

			if (AvejUtil::Random(party.encounter*30) == 0)
				EncounterEnemy();
		}
	}
	void m_ActSwamp(int x1, int y1, bool bUseless)
	{
		m_ActMove(x1, y1, false);

		// walkOnSwamp에 대한 처리
		if (party.ability.walkOnSwamp > 0)
		{
			--party.ability.walkOnSwamp;
		}
		else
		{
			CLoreConsole& console = GetConsole();
			console.Clear();
			console.SetTextColorIndex(13);
			console.Write("일행은 독이 있는 늪에 들어갔다 !!!");
			console.Write("");

			int nUpdate;

			nUpdate = std::for_each(player.begin(), player.end(), FnctEnterSwamp<TPcPlayer*>()).Result();

			console.Display();

			// player의 디스플레이 된 수치에 변화가 생겼다면
			if (nUpdate > 0)
			{
				window[WINDOW_STATUS]->SetUpdateFlag();
			}
		}

		DetectGameOver();
	}
	void m_ActLava(int x1, int y1, bool bUseless)
	{
		m_ActMove(x1, y1, false);

		{
			CLoreConsole& console = GetConsole();
			console.Clear();
			console.SetTextColorIndex(12);
			console.Write("일행은 용암지대로 들어섰다 !!!");
			console.Write("");

			int nUpdate;

			nUpdate = std::for_each(player.begin(), player.end(), FnctEnterLava<TPcPlayer*>()).Result();

			console.Display();

			window[WINDOW_STATUS]->SetUpdateFlag();
		}

		DetectGameOver();

		return;
	}
	void m_ActTalk(int x1, int y1, bool bUseless)
	{
		GetConsole().Clear();
		CScript script(CScript::MODE_TALK, 0, party.x + x1, party.y + y1);
		return;
	}

private:
	enum EEndBattle
	{
		END_BATTLE_NONE,
		END_BATTLE_LOSE,
		END_BATTLE_WIN,
		END_BATTLE_RUN_AWAY,
	};

	/* window */
	class CWindow
	{
	private:
		// 현재 이 window는 update 되어야 하는 상태인가?
		bool m_bMustUpdate;
		// 현재 이 window는 눈에 보이는 상태인가?
		bool m_bVisible;

		const CGameMain* m_pGameMain;

	protected:
		// window의 위치와 크기
		int  m_x, m_y, m_w, m_h;

		inline const CGameMain* _GetMainInst(void) { return m_pGameMain; };
		virtual void _OnDisplay(int param1, int param2) = 0;
		virtual void _OnSetRegion() {}
	
	public:
		CWindow(CGameMain* pGameMain)
			: m_bMustUpdate(true), m_bVisible(true),
			m_pGameMain(pGameMain),
			m_x(0), m_y(0), m_w(0), m_h(0)
		{
		}
		virtual ~CWindow(void)
		{
		}

		void SetUpdateFlag(void)
		{
			m_bMustUpdate = true;
		};
		void Display(int param1 = -1, int param2 = -1)
		{
			if (!m_bVisible)
				return;

			bool bForceUpdate = (param1 != -1) || (param2 != -1);

//??			if (bForceUpdate || m_bMustUpdate)
				_OnDisplay(param1, param2);

			m_bMustUpdate = false;
		}
		void SetRegion(int x, int y, int w, int h)
		{
			m_x = x;
			m_y = y;
			m_w = w;
			m_h = h;

			_OnSetRegion();
		}
		void GetRegion(int* pX, int* pY, int* pW, int* pH)
		{
			if (pX)
				*pX = m_x;
			if (pY)
				*pY = m_y;
			if (pW)
				*pW = m_w;
			if (pH)
				*pH = m_h;
		}
		bool IsVisible(void)
		{
			return m_bVisible;
		}
		void Show(void)
		{
			if (!m_bVisible)
				m_bMustUpdate = true;

			m_bVisible = true;

			this->Display();
		}
		void Hide(void)
		{
			m_bVisible = false;
			//@@ 추가적인 뭔가가 더 필요한가?
		}
	};

	class CWindowMap: public CWindow
	{
	public:
		enum
		{
			// 기준 반지름
			_X_RADIUS = 5,
			_Y_RADIUS = 5,
			// 타일의 실제 크기
			TILE_X_SIZE = 24,
			TILE_Y_SIZE = 24
		};

	private:
		// 타일 출력을 위한 시작 옵셋
		int m_xDisplayOffset;
		int m_yDisplayOffset;
		// 가로 세로 출력 반지름
		int m_wRadDisplay;
		int m_hRadDisplay;

	protected:
		void _OnDisplay(int param1, int param2)
		{
			bool bDisplayChara = true;
			const CGameMain& gameMain = *_GetMainInst();

			pBackBuffer->SetClipRect(m_x, m_y, m_w, m_h);

			// 동굴이 아니거나 마법의 횟불이 켜져 있는 상태라면,
			bool bDaylight = (s_pGameMain->map.type != TMap::TYPE_DEN) || (s_pGameMain->party.ability.magicTorch > 0);

			// 어두운 상태라면 일단 화면을 검게 만든다.
			if (!bDaylight)
				pBackBuffer->FillRect(0xFF000000, m_x, m_y, m_w, m_h);

			{
				int xOrigin = gameMain.party.x;
				int yOrigin = gameMain.party.y;

				int x;
				int y = m_y + m_yDisplayOffset;

				for (int j = -(m_hRadDisplay-1); j <= (m_hRadDisplay-1); j++)
				{
					x = m_x + m_xDisplayOffset;
					for (int i = -(m_wRadDisplay-1); i <= (m_wRadDisplay-1); i++)
					{
						// (bDaylight == true)라면 루프를 최적화 할 수 있지만...
						if (bDaylight)
						{
							int x_src = gameMain.map(xOrigin+i,yOrigin+j)*TILE_X_SIZE;
							int y_src = gameMain.map.type*TILE_Y_SIZE;

							y_src += 224;
							while (x_src >= 504 - 24*2)
							{
								x_src -= (504 - 24*2);
								y_src += 24*4;
							}

							pBackBuffer->BitBlt(x, y, pTileImage, x_src, y_src, TILE_X_SIZE, TILE_Y_SIZE);
						}
						else
						{
							if (gameMain.map.HasLight(xOrigin+i,yOrigin+j))
								pBackBuffer->BitBlt(x, y, pTileImage, gameMain.map(xOrigin+i,yOrigin+j)*TILE_X_SIZE, gameMain.map.type*TILE_Y_SIZE, TILE_X_SIZE, TILE_Y_SIZE);
						}
						x += TILE_X_SIZE;
					}
					y += TILE_Y_SIZE;
				}
			}

			// 주인공 출력
			if (bDisplayChara)
			{
				int lookOffset = (gameMain.map.encounterRange == 0) ? 0 : 4;
				int x_src = 24*19;
				int y_src = (gameMain.party.face+lookOffset)*TILE_Y_SIZE;
				pBackBuffer->BitBlt((m_wRadDisplay-1)*TILE_X_SIZE+m_x+m_xDisplayOffset, (m_hRadDisplay-1)*TILE_Y_SIZE+m_y+m_yDisplayOffset, pSpriteImage, x_src, y_src, TILE_X_SIZE, TILE_Y_SIZE);
			}

			pBackBuffer->SetClipRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

#if defined(_DEBUG)
			// 현재 좌표 출력 (임시)
			{
				char szText[256];
				SPRINTF(szText, 256, "(%3d,%3d)", gameMain.party.x, gameMain.party.y);
				pBackBuffer->FillRect(0, 0, 0, 9*config::c_wFont, config::c_hFont);
				g_DrawText(0, 0, szText);
			}
#endif
		}
		void _OnSetRegion()
		{
			if ((config::c_mapWindowRegion.w > 0) && (config::c_mapWindowRegion.h > 0))
			{
				m_wRadDisplay    = (config::c_mapWindowRegion.w+3*TILE_X_SIZE-1) / (2*TILE_X_SIZE);
				m_hRadDisplay    = (config::c_mapWindowRegion.h+3*TILE_Y_SIZE-1) / (2*TILE_Y_SIZE);
				m_xDisplayOffset = (config::c_mapWindowRegion.w - (2*m_wRadDisplay-1) * TILE_X_SIZE) / 2;
				m_yDisplayOffset = (config::c_mapWindowRegion.h - (2*m_hRadDisplay-1) * TILE_Y_SIZE) / 2;
			}
			else
			{
				// 만약 유효하지 않은 영역을 설정한 경우에는 실제 출력은 하지 않는다.
				m_xDisplayOffset = 0;
				m_yDisplayOffset = 0;
				m_wRadDisplay    = 0;
				m_hRadDisplay    = 0;
			}
		}

	public:
		CWindowMap(CGameMain* pGameMain)
			: CWindow(pGameMain)
		{
			m_xDisplayOffset = 0;
			m_yDisplayOffset = 0;
			m_wRadDisplay    = _X_RADIUS;
			m_hRadDisplay    = _Y_RADIUS;
		}
		virtual ~CWindowMap(void)
		{
		}
	};

	class CWindowConsole: public CWindow
	{
	protected:
		void _OnDisplay(int param1, int param2)
		{
			static bool s_is_first = true;
			if (s_is_first)
			{
				_OnSetRegion();

				// 전체 영역을 background color 로 채운 뒤, CLoreConsole을 통해 client 영역을 채움
				unsigned long bgColor = pBackBuffer->Color(0xFF, 0x40, 0x40, 0x40);
				pBackBuffer->FillRect(bgColor, m_x, m_y, m_w, m_h);

 				// 시작시 console 화면을 bg color로 채움
				CLoreConsole& console = GetConsole();
				console.SetBgColor(bgColor);
				console.Clear();

				s_is_first = false;
			}
			return;
		}

		void _OnSetRegion()
		{
			int wReal = m_w / config::c_wFont * config::c_wFont;
			int hReal = m_h / config::c_hFont * config::c_hFont;
			GetConsole().SetRegion(m_x+(m_w-wReal)/2, m_y+(m_h-hReal)/2, wReal, hReal);
		}

	public:
		CWindowConsole(CGameMain* pGameMain)
			: CWindow(pGameMain)
		{
		}
		virtual ~CWindowConsole(void)
		{
		}
	};

	class CWindowStatus: public CWindow
	{
	protected:
		void m_displayStatus(TPcPlayer* obj)
		{
			// 시작 전에는 항상 player의 값을 보정한다.
			obj->CheckCondition();

			unsigned long nameColor = pBackBuffer->Color(obj->GetConditionColor());
			unsigned long color0 = pBackBuffer->Color(0xFF, 0x00, 0x00, 0x00);
			unsigned long color1 = pBackBuffer->Color(0xFF, 0x40, 0x20, 0xC0);
			unsigned long color2 = pBackBuffer->Color(0xFF, 0x20, 0x10, 0x80);
			unsigned long color3 = pBackBuffer->Color(0xFF, 0x10, 0x00, 0x40);

			// Name HP SP ESP Condition
			static const int FILELD_WIDTH[4] = {70, 24+4, 18+4, 18+4};

			int i;
			int y = (obj->order+0)+1;
			int x = 0;

			g_DrawText(m_x+3, m_y, "name         hp  sp esp");

			for (i = 0; i < sizeof(FILELD_WIDTH) / sizeof(FILELD_WIDTH[0]); i++)
			{
				pBackBuffer->FillRect(color1, m_x+x, m_y+y*config::c_hFont+0, FILELD_WIDTH[i]-2, 2);
				pBackBuffer->FillRect(color2, m_x+x, m_y+y*config::c_hFont+2, FILELD_WIDTH[i]-2, 2);
				pBackBuffer->FillRect(color3, m_x+x, m_y+y*config::c_hFont+4, FILELD_WIDTH[i]-2, 3);
				pBackBuffer->FillRect(color0, m_x+x, m_y+y*config::c_hFont+7, FILELD_WIDTH[i]-2, 5);
				x += FILELD_WIDTH[i]+1;
			}

			x = 0;
			if (obj->Valid())
			{
				g_DrawText(m_x+x, m_y+config::c_hFont*y, obj->GetName(), nameColor);
				x += FILELD_WIDTH[0]+1;

				char szTemp[32];

				SPRINTF(szTemp, 32, "%4d", obj->hp);
				g_DrawText(m_x+x, m_y+config::c_hFont*y, szTemp);
				x += FILELD_WIDTH[1]+1;

				SPRINTF(szTemp, 32, "%3d", obj->sp);
				g_DrawText(m_x+x, m_y+config::c_hFont*y, szTemp);
				x += FILELD_WIDTH[2]+1;

				SPRINTF(szTemp, 32, "%3d", obj->esp);
				g_DrawText(m_x+x, m_y+config::c_hFont*y, szTemp);
				x += FILELD_WIDTH[3]+1;

				// 320*240 에서는 condition을 이름으로 출력하지 않고 이름 출력시 색깔로 정의됨
				//g_DrawText(m_x+x, m_y+config::c_hFont*y, obj->GetConditionString());
			}
			else
			{
				g_DrawText(m_x+x, m_y+config::c_hFont*y, resource::GetAuxName(resource::AUX_RESERVED).szName, game::GetRealColor(4));
			}
		}

		void _OnDisplay(int param1, int param2)
		{
			const CGameMain& gameMain = *_GetMainInst();

			std::vector<TPcPlayer*>::const_iterator obj = gameMain.player.begin();
			while (obj != gameMain.player.end())
			{
				m_displayStatus(*obj++);
			}
		}
	public:
		CWindowStatus(CGameMain* pGameMain)
			: CWindow(pGameMain)
		{
		}
		virtual ~CWindowStatus(void)
		{
		}
	};
	
	class CWindowBattle: public CWindow
	{
		void m_displayWithColor(TPcEnemy* obj, int row)
		{
			if (obj->Valid())
			{
				int ixColor = 10;

				if (obj->hp <= 0)
					ixColor = 8;
				else if (obj->hp <= 20)
					ixColor = 12;
				else if (obj->hp <= 50)
					ixColor = 4;
				else if (obj->hp <= 100)
					ixColor = 6;
				else if (obj->hp <= 200)
					ixColor = 14;
				else if (obj->hp <= 300)
					ixColor = 2;

				if (obj->unconscious)
					ixColor = 8;
				if (obj->dead)
					ixColor = 0;

				ASSERT(ixColor >= 0 && ixColor < 16);

				g_DrawText(m_x+26, 20*(row++)+(m_y+6), obj->GetName(), game::GetRealColor(ixColor));
			}
		}

	protected:
		void m_DisplayEnemies(bool bClean, int inverted = -1)
		{
			if (bClean)
			{
				unsigned long color = pBackBuffer->Color(0xFF, 0x00, 0x00, 0x00);
				pBackBuffer->FillRect(color, m_x, m_y, m_w, m_h);
			}

			if (inverted >= 0)
			{
				unsigned long color = pBackBuffer->Color(0xFF, 0x80, 0x80, 0x80);
				pBackBuffer->FillRect(color, m_x, 20*(inverted)+m_y+6, m_w, 16);
			}

			CGameMain* pGameMain = (CGameMain*)(_GetMainInst());

			int row = 0;
			std::vector<TPcEnemy*>::iterator obj = pGameMain->enemy.begin();
			while (obj != pGameMain->enemy.end())
			{
				m_displayWithColor(*obj++, row++);
			}

			game::UpdateScreen();
		}

		void _OnDisplay(int param1, int param2)
		{
			m_DisplayEnemies((param1 != 0) ? true : false, param2);
		}
	public:
		CWindowBattle(CGameMain* pGameMain)
			: CWindow(pGameMain)
		{
		}
		virtual ~CWindowBattle(void)
		{
		}
	};

private:
	enum
	{
		MAX_PLAYER = 6,
		MAX_ENEMY  = 7,
	};

	TPcPlayer  m_player[MAX_PLAYER];
	TPcEnemy   m_enemy[MAX_ENEMY];

public:
	enum EGameState
	{
		GAME_STATE_MAP,
		GAME_STATE_BATTLE,
	};

	enum EExitCode
	{
		EXIT_BY_USER,
		EXIT_BY_ACCIDENT,
		EXIT_BY_ENEMY,
		EXIT_BY_FORCE,
	};

	enum EBattleResult
	{
		BATTLE_RESULT_EVADE,
		BATTLE_RESULT_WIN,
		BATTLE_RESULT_LOSE,
	};
	
	EGameState  gameState;
	TPcParty    party;
	TMap        map;
	TGameOption gameOption;

	std::vector<TPcPlayer*> player;
	std::vector<TPcEnemy* > enemy;
	std::vector<CWindow*> window;
	std::vector<CSerialize*> saveList;

	CGameMain(void)
		: gameState(GAME_STATE_MAP)
	{
		player.reserve(MAX_PLAYER);
		enemy.reserve(MAX_ENEMY);

		ASSERT(player.capacity() == MAX_PLAYER);
		ASSERT(enemy.capacity() == MAX_ENEMY);

		// save를 해야 하는 object의 목록 등록
		{
			//?? 헤더 파일을 만들어야 함
			saveList.push_back(&party);

			for (int i = 0; i < MAX_PLAYER; i++)
				saveList.push_back(&m_player[i]);

			saveList.push_back(&map);
			saveList.push_back(&gameOption);
		}

		// party 초기화
		{
			// 외부에서 이 값이 수정되어져야 함
			party.x = -1;
			party.y = -1;
			//??
			party.ability.canUseESP = false;
			party.ability.canUseSpecialMagic = false;
		}

		// player 초기화
		{
			m_player[0].SetDefault(0);

			m_player[0].SetName("슴갈");
			m_player[0].class_ = 8;
			m_player[0].level[0] = 3;
			m_player[0].level[1] = 1;
			m_player[0].level[2] = 0;
			m_player[0].ReviseAttribute();

/*@@ 이전 임시 party 멤버임

			m_player[4].SetDefault(0);
			m_player[4].SetName("Becrux");
			m_player[4].hp = 100;

			m_player[2].SetDefault(0);
			m_player[2].SetName("슴가");
			m_player[2].gender = TPcPlayer::FEMALE;
			m_player[2].hp = 100;

			m_player[3].SetDefault(0);
			m_player[3].SetName("Lore Hunter");
			m_player[3].hp = 100;
			m_player[3].class_ = 2;

/* */
			for (int i = 0; i < sizeof(m_player) / sizeof(m_player[0]); i++)
			{
				m_player[i].order = i;
				player.push_back(&m_player[i]);
			}
		}
		
		// map 초기화
		{
			// map에게 default handler를 알려줌
			map.actFunc[TMap::ACT_BLOCK] = &CGameMain::m_ActBlock;
			map.actFunc[TMap::ACT_MOVE]  = &CGameMain::m_ActMove;
			map.actFunc[TMap::ACT_EVENT] = &CGameMain::m_ActEvent;
			map.actFunc[TMap::ACT_ENTER] = &CGameMain::m_ActEnter;
			map.actFunc[TMap::ACT_SIGN]  = &CGameMain::m_ActSign;
			map.actFunc[TMap::ACT_WATER] = &CGameMain::m_ActWater;
			map.actFunc[TMap::ACT_SWAMP] = &CGameMain::m_ActSwamp;
			map.actFunc[TMap::ACT_LAVA]  = &CGameMain::m_ActLava;
			map.actFunc[TMap::ACT_TALK]  = &CGameMain::m_ActTalk;
		}

		// window 초기화
		{
			// 아래 window들의 생성 순서는 정해져 있음

			// Map용 window
			{
				CWindow* pWindow = new CWindowMap(this);
				const config::TRect& region = config::c_mapWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Console용 window
			{
				CWindow* pWindow = new CWindowConsole(this);
				const config::TRect& region = config::c_consoleWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Status용 window
			{
				CWindow* pWindow = new CWindowStatus(this);
				const config::TRect& region = config::c_statusWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Battle용 window
			{
				CWindow* pWindow = new CWindowBattle(this);
				const config::TRect& region = config::c_mapWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				pWindow->Hide();
				window.push_back(pWindow);
			}
		}

	}
	~CGameMain(void)
	{
		std::for_each(window.begin(), window.end(), FnctDelete<CWindow*>());
	}

	bool LoadMapFromFile(const char* szFileName)
	{
		CFileReadStream stream(szFileName);

		if (!stream.IsVaild())
			return false;

		unsigned char byte;

		map.ClearData();

		// 가장 먼저 map type의 판별이 있어야 함
		map.SetType(TMap::TYPE_TOWN);

		stream.Read(&byte, 1);
		map.width  = byte;

		stream.Read(&byte, 1);
		map.height = byte;

		for (int y = 0; y < map.height; y++)
		{
			stream.Read(&map.data[y][0], map.width);
		}

		return true;
	}

	bool LoadScript(const char* szFileName)
	{
		CScript::RegisterScriptFileName(szFileName);
		gameOption.scriptFile = szFileName;

		CScript script(CScript::MODE_MAP, 0);

		return true;
	}

	// 0~(n-1) 까지의 유효 숫자 또는 -1의 무효 숫자를 돌려 줌
	int SelectPlayer(const char* szTitle = NULL)
	{
		if (szTitle == NULL)
			szTitle = "한명을 고르시오 ---";

		TMenuList menu;

		menu.reserve(MAX_PLAYER+1);

		menu.push_back(szTitle);

		int realPlayer[MAX_PLAYER];
		int nReal = 0;
		int count = 0;
		for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj, ++count)
		{
			ASSERT(nReal < MAX_PLAYER);

			if ((*obj)->Valid())
			{
				menu.push_back((*obj)->GetName());
				realPlayer[nReal++] = count;
			}
		}

		ASSERT(nReal > 0);

		int selected = CSelect(menu)();

		--selected;

		return (selected >= 0) ? realPlayer[selected] : -1;
	}

	void ChangeWindowForBattle(void)
	{
		window[WINDOW_MAP]->Hide();
		window[WINDOW_BATTLE]->Show();
	}

	void ChangeWindowForField(void)
	{
		window[WINDOW_BATTLE]->Hide();
		window[WINDOW_MAP]->Show();
	}

	bool IsValidWarpPos(int x, int y)
	{
		return ((x >= CWindowMap::_X_RADIUS) && (x < map.width-CWindowMap::_X_RADIUS) && (y >= CWindowMap::_Y_RADIUS) && (y < map.height-CWindowMap::_Y_RADIUS));
	}

	// 0~(n-1) 까지의 유효 숫자
	// 절대 -1의 무효 숫자를 돌려 주지 않음
	int SelectEnemy(void)
	{
		int nEnabled = enemy.size();
		int selected = 0;
		
		do
		{
			window[WINDOW_BATTLE]->Display(1, selected);

			bool bUpdate = false;

			do
			{
				unsigned short key;
				while ((key = GetKeyBuffer().GetKey()) < 0)
					;

				switch (key)
				{
				case avej_lite::INPUT_KEY_UP:
				case avej_lite::INPUT_KEY_DOWN:
					{
						int dy = (key == avej_lite::INPUT_KEY_UP) ? -1 : +1;
						selected += dy;

						if (selected < 0)
							selected = nEnabled - 1;
						if (selected >= nEnabled)
							selected = 0;

						bUpdate = true;
					}
					break;
/* 무효 숫자를 돌려주지 않게 함
				case avej::KEY_BUTTON_A:
					selected = -1;
					// pass through
*/
				case avej_lite::INPUT_KEY_B:
					window[WINDOW_BATTLE]->Display(1);
					return selected;
				}
			} while (!bUpdate);
			
		} while (1);
	}
	
	void DetectGameOver(void)
	{
		// 전처리
		std::for_each(player.begin(), player.end(), FnctCheckCondition<TPcPlayer*>());

		// 실제 생존해 있는 숫자를 확인
		int nAlive = NUM_OF_CONSCIOUS_PLAYER(player);

		if (nAlive == 0)
			GameOver(EXIT_BY_ACCIDENT);
	}

	bool CheckEndOfBattle(EEndBattle& exitCode)
	{
		int nAlive = NUM_OF_CONSCIOUS_PLAYER(player);

		if (nAlive == 0)
		{
			exitCode = END_BATTLE_LOSE;
			return true;
		}

		nAlive = NUM_OF_CONSCIOUS_ENEMY(enemy);

		if (nAlive == 0)
		{
			exitCode = END_BATTLE_WIN;
			return true;
		}

		exitCode = END_BATTLE_NONE;

		return false;
	}

	void PlusGold(void)
	{
		int nGold = std::for_each(enemy.begin(), enemy.end(), FnctPlusGold<TPcEnemy*>()).Result();
		
		party.gold += nGold;

		game::console::WriteConsole(15, 3, "일행은 ", IntToStr(nGold)(), "개의 금을 얻었다.");
	}

	int RegisterEnemy(int index)
	{
		int ixEnemy = enemy.size();

		m_enemy[ixEnemy] << GetEnemyData(index);
		m_enemy[ixEnemy].E_number = index;
		enemy.push_back(&m_enemy[ixEnemy]);

		return ixEnemy;
	}

	// 정상적인 승부로 끝이나면 true, 도망을 치면 false
	EBattleResult BattleMode(bool bAssualt)
	{
		EBattleResult result = BATTLE_RESULT_WIN;
		gameState = GAME_STATE_BATTLE;

		EEndBattle exitCode = END_BATTLE_NONE;
		int battle[MAX_PLAYER][4];

		do
		{
			if (!bAssualt)
				goto QAZ;

			CLEAR_MEMORY(battle);

			window[WINDOW_BATTLE]->Display(0);

			// 명령 등록
			{
				bool bAutoBattle = false;

				for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
				{
					int order = (*obj)->order;

					if ((*obj)->IsConscious())
					{
						if (!bAutoBattle)
						{
							TMenuList menu;

							avej_lite::util::string str1;
							str1 += (*obj)->GetName();
							str1 += "의 전투 모드 ===>";

							avej_lite::util::string str2;
							str2 += "한 명의 적을 ";
							str2 += resource::GetWeaponName((*obj)->weapon).szName;
							str2 += resource::GetWeaponName((*obj)->weapon).szJosaWith;
							str2 += "로 공격";

							menu.reserve(8);
							menu.push_back(str1);
							menu.push_back(str2);
							menu.push_back("한 명의 적에게 마법 공격");
							menu.push_back("모든 적에게 마법 공격");
							menu.push_back("적에게 특수 마법 공격");
							menu.push_back("일행을 치료");
							menu.push_back("적에게 초능력 사용");

							if (order == 0)
								menu.push_back("일행에게 무조건 공격 할 것을 지시");
							else
								menu.push_back("도망을 시도함");

							int selected = CSelect(menu)();

							if (selected != 1)
								GetConsole().Clear();

							if ((selected == 7) && (order == 0))
							{
								selected = 8;
								bAutoBattle = true;
							}

							battle[order][1] = selected;
						}
						else
						{
							battle[order][1] = 8;
						}

						// 메뉴 선택에 따른 동작 방식 결정
						switch (battle[order][1])
						{
						case 1:
							{
								int selected = SelectEnemy();
								GetConsole().Clear();
								if (selected == -1)
									battle[order][1] = 0;
								battle[order][2] = (*obj)->weapon;
								battle[order][3] = selected;
							}
							break;
						case 2:
						case 3:
						case 4:
							{
								int  ixMagicOffset = 0;
								int  nEnabled = 7;
								bool isSingleAttack = true;

								switch (battle[order][1])
								{
								case 2:
									ixMagicOffset = 0;
									{
										int level2 = (*obj)->level[1];
										if      (level2 <= 1)
											nEnabled = 2;
										else if (level2 <= 3)
											nEnabled = 3;
										else if (level2 <= 7)
											nEnabled = 4;
										else if (level2 <= 11)
											nEnabled = 5;
										else if (level2 <= 15)
											nEnabled = 6;
									}
									break;
								case 3:
									ixMagicOffset = 6;
									{
										int level2 = (*obj)->level[1];
										if      (level2 <= 1)
											nEnabled = 1;
										else if (level2 <= 2)
											nEnabled = 2;
										else if (level2 <= 5)
											nEnabled = 3;
										else if (level2 <= 9)
											nEnabled = 4;
										else if (level2 <= 13)
											nEnabled = 5;
										else if (level2 <= 17)
											nEnabled = 6;
									}
									isSingleAttack = false;
									break;
								case 4:
									ixMagicOffset = 12;
									{
										int level2 = (*obj)->level[1];
										if      (level2 <= 4)
											nEnabled = 1;
										else if (level2 <= 9)
											nEnabled = 2;
										else if (level2 <= 11)
											nEnabled = 3;
										else if (level2 <= 13)
											nEnabled = 4;
										else if (level2 <= 15)
											nEnabled = 5;
										else if (level2 <= 17)
											nEnabled = 6;
									}
									break;
								default:
									ASSERT(false);
								}

								TMenuList menu;

								menu.reserve(8);
								menu.push_back("");
								menu.push_back("없음");
								for (int i = 0; i < 6; i++)
									menu.push_back(resource::GetMagicName(i+1+ixMagicOffset).szName);

								int selected = CSelect(menu, nEnabled)();
								int ixEnemy  = -1;

								if ((selected == 0) || (selected == 1))
									battle[order][1] = 0;
								else if (isSingleAttack)
									ixEnemy = SelectEnemy();

								GetConsole().Clear();

								battle[order][2] = selected - 1;
								battle[order][3] = ixEnemy;
							}
							break;
						case 5:
							player[order]->CastCureSpell();
							break;
						case 6:
							{
								TMenuList menu;

								menu.reserve(8);
								menu.push_back("");
								for (int i = 0; i < 5; i++)
									menu.push_back(resource::GetMagicName(i+1+40).szName);

								int selected = CSelect(menu)();
								int ixEnemy  = -1;

								if (selected == 0)
									battle[order][1] = 0;
								else
									ixEnemy = SelectEnemy();

								GetConsole().Clear();

								battle[order][2] = selected;
								battle[order][3] = ixEnemy;

								if ((ixEnemy < 0) || (enemy[ixEnemy]->unconscious > 0) || (enemy[ixEnemy]->dead > 0))
									battle[order][1] = 0;
							}
							break;
						case 8:
							switch ((*obj)->class_)
							{
							case  0:
							case  1:
							case  4:
							case  5:
							case  6:
							case  7:
							case  8:
							case 10:
								battle[order][1] = 1;
								battle[order][2] = (*obj)->weapon;
								battle[order][3] = 0;
								break;
							case  2:
							case  9:
								battle[order][1] = 2;

								switch ((*obj)->level[1])
								{
								case 0:
								case 1:
								case 2:
								case 3:
									battle[order][2] = 1;
									break;
								case 4:
								case 5:
								case 6:
								case 7:
								case 8:
								case 9:
								case 10:
								case 11:
									battle[order][2] = 2;
									break;
								default:
									battle[order][2] = 3;
									break;
								}
								battle[order][3] = 0;
								break;
							case  3:
								battle[order][1] = 6;
								battle[order][2] = 5;
								{
									int target = 0;
									for (int ixEnemy = 1; ixEnemy < enemy.size(); ++ixEnemy)
									{ // @@ dead 맞나?
										if ((enemy[ixEnemy]->unconscious == 0) && (enemy[ixEnemy]->dead))
											target = ixEnemy;
									}
									battle[order][3] = target;
								}

								break;
							default:
								ASSERT(false);
							}
							break;
						}
					}
				}
			}

			GetConsole().Clear();

			// 명령 해석
			{
				for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
				{
					if ((*obj)->IsConscious())
					{
						int order = (*obj)->order;

						static CSmSet s_printMessage("0,4,6..8");

						if (s_printMessage.IsSet(battle[order][1]))
						{
							avej_lite::util::string s = GetBattleMessage(**obj, battle[order][1], battle[order][2], *enemy[battle[order][3]]);
							GetConsole().SetTextColorIndex(15);
							GetConsole().Write(s);
						}

						switch (battle[order][1])
						{
							case 1:
								player[order]->AttackWithWeapon(battle[order][2], battle[order][3]);
								break;
							case 2:
								player[order]->CastSpellToOne(battle[order][2], battle[order][3]);
								break;
							case 3:
								player[order]->CastSpellToAll(battle[order][2]);
								break;
							case 4:
								player[order]->CastSpellWithSpecialAbility(battle[order][2], battle[order][3]);
								break;
							case 6:
								player[order]->UseESPForBattle(battle[order][2], battle[order][3]);
								break;
							case 7:
								{
									if ((*obj)->TryToRunAway())
									{
										game::PressAnyKey();

										GetConsole().Clear();

										exitCode = END_BATTLE_RUN_AWAY;

										goto END_OF_BATTLE;
									}
								}
						}
						
						GetConsole().Write("");
					}
				}
				
				window[WINDOW_BATTLE]->Display(1);

				GetConsole().Write("");
				GetConsole().Display();
				game::UpdateScreen();
				game::PressAnyKey();
			}
QAZ:
			GetConsole().Clear();

			{
				for (std::vector<TPcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
				{
					if ((*obj)->poison > 0)
					{
						if ((*obj)->unconscious > 0)
						{
							(*obj)->dead = 1;
						}
						else
						{
							(*obj)->hp -= (*obj)->poison;
							if ((*obj)->hp <= 0)
								(*obj)->unconscious = 1;
						}
					}
					if (((*obj)->unconscious == 0) && ((*obj)->dead == 0))
					{
						(*obj)->Attack();
					}
				}
			}

			window[WINDOW_STATUS]->SetUpdateFlag();
			window[WINDOW_STATUS]->Display();

			game::UpdateScreen();

			if (CheckEndOfBattle(exitCode))
				break;

			bAssualt = true;

			// 원작에서는 적 공격 후에 Press Any Key 문자 출력이 없었음
			game::PressAnyKey();

		} while (1);

END_OF_BATTLE:
		switch (exitCode)
		{
		case END_BATTLE_LOSE:
			GameOver(EXIT_BY_ENEMY);
			return BATTLE_RESULT_LOSE;
		case END_BATTLE_WIN:
			GetConsole().Clear();
			PlusGold();
			break;
		case END_BATTLE_RUN_AWAY:
			result = BATTLE_RESULT_EVADE;
			break;
		default:
			ASSERT(false);
		}

		window[WINDOW_MAP]->SetUpdateFlag();
		window[WINDOW_MAP]->Display();

		gameState = GAME_STATE_MAP;

		return result;
	}

	void EncounterEnemy(void)
	{
		if (map.GetEncounteredEnemy() == 0)
			return;

		enemy.clear();

		int nEnemy = (AvejUtil::Random(party.maxEnemy) + 1);

		int enemyAgility = 0;
		for (int i = 0; i < nEnemy; i++)
		{
			int ixEnemy = map.GetEncounteredEnemy();
			ASSERT(ixEnemy > 0);

			int index = RegisterEnemy(ixEnemy);
			enemyAgility += enemy[index]->agility;
		}

		ASSERT(nEnemy > 0);
		enemyAgility /= nEnemy;

		CLoreConsole& console = GetConsole();

		console.Clear();

		console.SetTextColorIndex(12);
		console.Write("적이 출현했다 !!!");
		console.Write("");

		console.SetTextColorIndex(11);
		console.Write("적의 평균 민첩성");
		{
			char szTemp[256];
			int avgAgility = std::for_each(enemy.begin(), enemy.end(), FnctAvgAgility<TPcEnemy*>()).Result();
			CONSOLE_WRITE(" : %d", avgAgility);
		}

		console.Display();

		ChangeWindowForBattle();

		game::UpdateScreen();

		// 위의 글자를 출력하기 위해서 임시로 console 창의 사이즈를 줄인다.
		config::TRect savedRect;
		int reducedSize = 4*config::c_hFont;

		window[WINDOW_CONSOLE]->GetRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
		window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

		TMenuList menu;

		menu.reserve(3);
		menu.push_back("");
		menu.push_back("적과 교전한다");
		menu.push_back("도망간다");

		bool willingToAvoidBattle = (CSelect(menu)() != 1);

		// console 창의 사이즈를 복귀시킨다.
		window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y, savedRect.w, savedRect.h);

		// '도망간다'를 선택했을 때 전 영역을 bg color로 채우기 위함
		console.Clear();
		console.Display();

		do
		{
			bool bAssualt;

			if (willingToAvoidBattle)
			{
				int avgLuck    = std::for_each(player.begin(), player.end(), FnctAvgLuck<TPcPlayer*>()).Result();
				int avgAgility = std::for_each(enemy.begin(), enemy.end(), FnctAvgAgility<TPcEnemy*>()).Result();

				if (avgLuck > avgAgility)
					break; // 전투를 회피

				bAssualt = false;
			}
			else
			{
				int avgAgility1 = std::for_each(player.begin(), player.end(), FnctAvgAgility<TPcPlayer*>()).Result();
				int avgAgility2 = std::for_each(enemy.begin(), enemy.end(), FnctAvgAgility<TPcEnemy*>()).Result();

				bAssualt = (avgAgility1 > avgAgility2);
			}

			BattleMode(bAssualt);
		} while(0);

		ChangeWindowForField();
	}

	void ShowPartyStatus()
	{
		char szTemp[256];

		CLoreConsole& console = GetConsole();

		console.Clear();
		console.SetTextColorIndex(7);

		CONSOLE_WRITE("X 축 = %d", party.x);
		CONSOLE_WRITE("Y 축 = %d", party.y);
		CONSOLE_WRITE("남은 식량 = %d", party.food);
		CONSOLE_WRITE("남은 황금 = %d", party.gold);

		console.Write("");

		CONSOLE_WRITE("마법의 횃불 : %d", party.ability.magicTorch);
		CONSOLE_WRITE("공중 부상   : %d", party.ability.levitation);
		CONSOLE_WRITE("물위를 걸음 : %d", party.ability.walkOnWater);
		CONSOLE_WRITE("늪위를 걸음 : %d", party.ability.walkOnSwamp);

		console.Display();
	}

	void ShowCharacterStatus(void)
	{
		int selected = SelectPlayer("능력을 보고싶은 인물을 선택하시오");
		if (selected >= 0)
		{
			ASSERT(selected < MAX_PLAYER);
			ShowCharacterStatus(*player[selected]);
		}
	}

	void ShowCharacterStatus(const TPcPlayer& selected)
	{
		ASSERT(selected.Valid());

		if (!selected.Valid())
			return;

		char szTemp[256];

		CLoreConsole& console = GetConsole();

		console.Clear();

		console.SetTextColorIndex(11);
		CONSOLE_WRITE("# 이름 : %s", selected.GetName())
		CONSOLE_WRITE("# 성별 : %s", selected.GetGenderName())
		CONSOLE_WRITE("# 계급 : %s", selected.GetClassName())

		console.Write("");

		console.SetTextColorIndex(3);
		CONSOLE_WRITE("체력   : %d", selected.strength)
		CONSOLE_WRITE("정신력 : %d", selected.mentality)
		CONSOLE_WRITE("집중력 : %d", selected.concentration)
		CONSOLE_WRITE("인내력 : %d", selected.endurance)
		CONSOLE_WRITE("저항력 : %d", selected.resistance)
		CONSOLE_WRITE("민첩성 : %d", selected.agility)
		CONSOLE_WRITE("행운   : %d", selected.luck)

		console.Display();
		game::UpdateScreen();

		game::PressAnyKey();

		console.Clear();

		// 이 부분은 한 번 더 반복
		console.SetTextColorIndex(11);
		CONSOLE_WRITE("# 이름 : %s", selected.GetName())
		CONSOLE_WRITE("# 성별 : %s", selected.GetGenderName())
		CONSOLE_WRITE("# 계급 : %s", selected.GetClassName())

		console.Write("");

		console.SetTextColorIndex(3);
		CONSOLE_WRITE2("무기의 정확성   : %2d    전투 레벨   : %2d", selected.accuracy[0], selected.level[0])
		CONSOLE_WRITE2("정신력의 정확성 : %2d    마법 레벨   : %2d", selected.accuracy[1], selected.level[1])
		CONSOLE_WRITE2("초감각의 정확성 : %2d    초감각 레벨 : %2d", selected.accuracy[2], selected.level[2])
		CONSOLE_WRITE("## 경험치   : %d", selected.experience)

		console.Write("");

		console.SetTextColorIndex(2);
		CONSOLE_WRITE("사용 무기 - %s", selected.GetWeaponName())
		CONSOLE_WRITE2("방패 - %s           갑옷 - %s", selected.GetShieldName(), selected.GetArmorName())

		console.Display();
	}

	void ShowQuickView(void)
	{
		char szTemp[256];

		CLoreConsole& console = GetConsole();

		console.Clear();

		console.SetTextColorIndex(15);
		console.Write("                이름    중독  의식불명    죽음");
		console.Write("");

		console.SetTextColorIndex(7);
		for (std::vector<TPcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
		{
			if ((*obj)->Valid())
			{
				CONSOLE_WRITE4("%20s   %5d %9d %7d", (*obj)->GetName(), (*obj)->poison, (*obj)->unconscious, (*obj)->dead)
			}
		}

		console.Display();
	}

	void CastSpell(void)
	{
		int selected = SelectPlayer();
		if (selected < 0)
			return;

		if (!player[selected]->IsConscious())
		{
			game::console::WriteConsole(7, 2, player[selected]->Get3rdPersonName(), "는 마법을 사용할 수 있는 상태가 아닙니다.");
			return;
		}

		TMenuList menu;

		menu.reserve(4);
		menu.push_back("사용할 마법의 종류 ===>");
		menu.push_back("공격 마법");
		menu.push_back("치료 마법");
		menu.push_back("변화 마법");

		switch (CSelect(menu)())
		{
		case 1:
			player[selected]->CastAttackSpell();
			break;
		case 2:
			player[selected]->CastCureSpell();
			break;
		case 3:
			player[selected]->CastPhenominaSpell();
			break;
		default:
			break;
		}
	}

	void UseExtrasense(void)
	{
		GetConsole().Clear();

		int selected = SelectPlayer();
		if (selected < 0)
			return;

		if (!player[selected]->IsConscious())
		{
			game::console::WriteConsole(7, 2, player[selected]->Get3rdPersonName(), "는 초감각을 사용할수있는 상태가 아닙니다.");
			return;
		}

		player[selected]->UseESP();
	}

	void RestHere(void)
	{
		CLoreConsole& console = GetConsole();

		console.Clear();

		std::for_each(player.begin(), player.end(), FnctRestHere<TPcPlayer*>());

		if (party.ability.magicTorch > 0)
			--party.ability.magicTorch;

		party.ability.levitation  = 0;
		party.ability.walkOnWater = 0;
		party.ability.walkOnSwamp = 0;
		party.ability.mindControl = 0;

		console.Display();

		window[WINDOW_STATUS]->SetUpdateFlag();
		window[WINDOW_STATUS]->Display();

		game::UpdateScreen();
		//@@ 필요한가?
		game::PressAnyKey();
	}

	bool LoadGame(int index)
	{
		CSerializeStream stream(GetSaveFileName(index), CSerializeStream::STREAM_READ);

		for (std::vector<CSerialize*>::iterator obj = saveList.begin(); obj != saveList.end(); ++obj)
		{
			stream >> *(*obj);
		}

		CScript::RegisterScriptFileName(gameOption.scriptFile);

		window[WINDOW_MAP]->SetUpdateFlag();
		window[WINDOW_STATUS]->SetUpdateFlag();

		return true;
	}

	bool SaveGame(int index)
	{
		CSerializeStream stream(GetSaveFileName(index), CSerializeStream::STREAM_WRITE);

		for (std::vector<CSerialize*>::iterator obj = saveList.begin(); obj != saveList.end(); ++obj)
		{
			stream << *(*obj);
		}

		return true;
	}

	bool SelectLoadMenu(void)
	{
		TMenuList menu;

		menu.push_back("불러 내고 싶은 게임을 선택하십시오.");
		menu.push_back("없습니다");
		menu.push_back("본 게임 데이타");
		menu.push_back("게임 데이타 1 (부)");
		menu.push_back("게임 데이타 2 (부)");
		menu.push_back("게임 데이타 3 (부)");

		int selected = CSelect(menu)();

		if (selected <= 1)
			return false;

		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(11);
			console.Write("저장했던 게임을 다시 불러옵니다");
			console.Display();

			// 0~3 사의 값
			if (!LoadGame(selected-2))
			{
				// 파일이 존재 하지 않음
				//?? 관련된 메시지를 보내고 다시 메뉴를 선택하게 해야 함
				//?? 또는 자동으로 데이터가 있는지를 확인하여 메뉴를 하이라이트를 시켜야 함
				return false;
			}

			window[WINDOW_MAP]->SetUpdateFlag();
			window[WINDOW_STATUS]->SetUpdateFlag();
			console.Clear();
			console.Display();
		}

		return true;
	}

	bool SelectSaveMenu(void)
	{
		TMenuList menu;

		menu.push_back("게임의 저장 장소를 선택하십시오.");
		menu.push_back("없습니다");
		menu.push_back("본 게임 데이타");
		menu.push_back("게임 데이타 1 (부)");
		menu.push_back("게임 데이타 2 (부)");
		menu.push_back("게임 데이타 3 (부)");

		int selected = CSelect(menu)();

		if (selected <= 1)
			return false;

		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(12);
			console.Write("현재의 게임을 저장합니다");
			console.Display();

			// 0~3 사의 값
			if (!SaveGame(selected-2))
			{
				//?? 실패 했음을 알려 줘야 하나?
				return false;
			}

			console.SetTextColorIndex(7);
			console.Write("");
			console.Write("성공했습니다");
			console.Display();

			game::PressAnyKey();
		}

		return true;
	}

	void SelectGameOption(void)
	{
		TMenuList menu;

		menu.reserve(7);
		menu.push_back("게임 선택 상황");
		menu.push_back("난이도 조절");
		menu.push_back("정식 일행의 순서 정렬");
		menu.push_back("일행에서 제외 시킴");
		menu.push_back("이전의 게임을 재개");
		menu.push_back("현재의 게임을 저장");
		menu.push_back("게임을 마침");

		int selected = CSelect(menu)();

		switch (selected)
		{
		case 0:
			break;
		case 1:
			// 전투시 출현하는 적의 최대치 기입
			{
				menu.clear();
				menu.push_back("한번에 출현하는 적들의 최대치를 기입하십시오");
				menu.push_back("3명의 적들");
				menu.push_back("4명의 적들");
				menu.push_back("5명의 적들");
				menu.push_back("6명의 적들");
				menu.push_back("7명의 적들");

				int selected = CSelect(menu, -1, party.maxEnemy - 2)();

				if (selected == 0)
					break;

				party.maxEnemy = selected + 2;
			}

			// 전투시 출현하는 적의 최대치 기입
			{
				menu.clear();
				menu.push_back("일행들의 지금 성격은 어떻습니까 ?");
				menu.push_back("일부러 전투를 피하고 싶다");
				menu.push_back("너무 잦은 전투는 원하지 않는다");
				menu.push_back("마주친 적과는 전투를 하겠다");
				menu.push_back("보이는 적들과는 모두 전투하겠다");
				menu.push_back("그들은 피에 굶주려 있다");

				int selected = CSelect(menu, -1, 6 - party.encounter)();

				if (selected == 0)
					break;

				party.encounter = 6 - selected;
			}
			break;
		case 2:
			{
				int  ixPlayer;
				int  indexTable[6] = {0, };
				int* pIndex = &indexTable[1];

				CLoreConsole& console = GetConsole();

				console.Clear();

				console.SetTextColorIndex(12);
				console.Write("현재의 일원의 전투 순서를 정렬 하십시오.");
				console.Write("");
				console.Display();

				menu.clear();
				menu.push_back("@B순서를 바꿀 일원 1@@");

				for (ixPlayer = 1; ixPlayer < 5; ixPlayer++)
				{
					if (player[ixPlayer]->Valid())
					{
						menu.push_back(player[ixPlayer]->GetName());
						*pIndex++ = ixPlayer;
					}
				}

				// 주인공을 제외한 일행이 1명 이하라면 이 기능을 수행할 필요가 없다.
				if (menu.size() < 3)
				{
					CLoreConsole& console = GetConsole();

					console.Clear();
					console.SetTextColorIndex(7);
					console.Write("전투 순서를 정렬 할 필요가 없습니다.");
					console.Display();

					game::PressAnyKey();

					break;
				}

				// 위의 글자를 출력하기 위해서 임시로 console 창의 사이즈를 줄인다.
				config::TRect savedRect;
				int reducedSize = 3*config::c_hFont;

				window[WINDOW_CONSOLE]->GetRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
				window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

				do
				{
					int selected1 = CSelect(menu)();
					if (selected1 == 0)
						break;

					menu[0] = "@B순서를 바꿀 일원 2@@";

					int selected2 = CSelect(menu)();
					if (selected2 == 0)
						break;

					if (selected1 != selected2)
					{
						int ixPlayer1 = indexTable[selected1];
						int ixPlayer2 = indexTable[selected2];
						assert(ixPlayer1 > 0 && ixPlayer1 < 6);
						assert(ixPlayer2 > 0 && ixPlayer2 < 6);

						if (ixPlayer1 > ixPlayer2)
							std::swap(ixPlayer1, ixPlayer2);

						assert(ixPlayer1 < ixPlayer2);

						// 실제 등록 순서를 바꾸기
						std::swap(player[ixPlayer1], player[ixPlayer2]);
						// order는 등록 번호이므로 이전과 같아야 한다
						std::swap(player[ixPlayer1]->order, player[ixPlayer2]->order);
					}

					window[WINDOW_STATUS]->SetUpdateFlag();
					window[WINDOW_STATUS]->Display();
				} while(0);

				// console 창의 사이즈를 복귀시킨다.
				window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y, savedRect.w, savedRect.h);

				console.Clear();
			}
				
			break;
		case 3:
			{
				int  ixPlayer;
				int  indexTable[6] = {0, };
				int* pIndex = &indexTable[1];

				menu.clear();
				menu.push_back("@C일행에서 제외 시키고 싶은 사람을 고르십시오.@@");

				for (ixPlayer = 1; ixPlayer < 6; ixPlayer++)
				{
					if (player[ixPlayer]->Valid())
					{
						menu.push_back(player[ixPlayer]->GetName());
						*pIndex++ = ixPlayer;
					}
				}

				// 주인공뿐이라면 이 기능을 수행할 필요가 없다.
				if (menu.size() < 2)
				{
					CLoreConsole& console = GetConsole();

					console.Clear();
					console.SetTextColorIndex(7);
					console.Write("제외시킬 일행이 없습니다.");
					console.Display();

					game::PressAnyKey();

					break;
				}

				int selected1 = CSelect(menu)();
				if (selected1 == 0)
					break;

				ixPlayer = indexTable[selected1];
				assert(ixPlayer > 0 && ixPlayer < 6);

				player[ixPlayer]->SetName("");

				window[WINDOW_STATUS]->SetUpdateFlag();
				window[WINDOW_STATUS]->Display();
			}
			break;
		case 4:
			SelectLoadMenu();
			break;
		case 5:
			SelectSaveMenu();
			break;
		case 6:
			GameOver(EXIT_BY_USER);
			break;
		}
	}

	void SelectMainMenu(void)
	{
		TMenuList menu;

		menu.reserve(8);
		menu.push_back("당신의 명령을 고르시오 ===>");
		menu.push_back("일행의 상황을 본다");
		menu.push_back("개인의 상황을 본다");
		menu.push_back("일행의 건강 상태를 본다");
		menu.push_back("마법을 사용한다");
		menu.push_back("초능력을 사용한다");
		menu.push_back("여기서 쉰다");
		menu.push_back("게임 선택 상황");

		int selected = CSelect(menu)();

		switch (selected)
		{
		case 0:
			break;
		case 1:
			ShowPartyStatus();
			break;
		case 2:
			ShowCharacterStatus();
			break;
		case 3:
			ShowQuickView();
			break;
		case 4:
			CastSpell();
			break;
		case 5:
			UseExtrasense();
			break;
		case 6:
			RestHere();
			break;
		case 7:
			SelectGameOption();
			break;
		default:
			ASSERT(0);
		}
	}

	void GameOver(EExitCode code)
	{
		switch (code)
		{
		case EXIT_BY_USER:
			{
				TMenuList menu;

				menu.push_back("정말로 끝내겠습니까 ?"); // 원래는 10번 색
				menu.push_back("       << 아니오 >>");
				menu.push_back("       <<   예   >>");

				if (CSelect(menu)() != 2)
					return;
			}
			break;
		case EXIT_BY_ACCIDENT:
			{
				CLoreConsole& console = GetConsole();

				console.Clear();
				console.SetTextColorIndex(13);
				console.Write("일행은 모험중에 모두 목숨을 잃었다.");
				console.Display();

				window[WINDOW_MAP]->Display();

				window[WINDOW_STATUS]->SetUpdateFlag();
				window[WINDOW_STATUS]->Display();

				game::UpdateScreen();

				game::PressAnyKey();

				if (SelectLoadMenu())
					return;
			}
			break;
		case EXIT_BY_ENEMY:
			{
				CLoreConsole& console = GetConsole();

				console.Clear();
				console.SetTextColorIndex(13);
				console.Write("일행은 모두 전투에서 패했다 !!");
				console.Display();

				game::PressAnyKey();

				console.Clear();

				TMenuList menu;
				menu.reserve(8);
				menu.push_back("    @A어떻게 하시겠습니까 ?@@");
				menu.push_back("   이전의 게임을 재개한다");
				menu.push_back("       게임을 끝낸다");

				int selected = CSelect(menu)();

				if (selected == 1)
				{
					if (SelectLoadMenu())
						return;
				}
			}
			break;
		case EXIT_BY_FORCE:
			// 특별하게 취해야할 동작이 없음
			break;
		default:
			ASSERT(false);
		}

		throw EExitGame();
	}

	bool Process(void)
	{
		pGfxDevice->BeginDraw(false);

		if (GetKeyBuffer().IsKeyPressed())
		{
			int  key = GetKeyBuffer().GetKey();
			bool bTimePassed = true;

			assert(key >= 0);

			// command 처리
			switch (key)
			{
			case 'P':
			case 'p':
				ShowPartyStatus();
				break;
			case 'V':
			case 'v':
				ShowCharacterStatus();
				break;
			case 'Q':
			case 'q':
				ShowQuickView();
				break;
			case 'C':
			case 'c':
				CastSpell();
				break;
			case 'E':
			case 'e':
				UseExtrasense();
				break;
			case 'R':
			case 'r':
				RestHere();
				break;
			case 'G':
			case 'g':
				SelectGameOption();
				break;
			case avej_lite::INPUT_KEY_SYS2:
				// space 처리
				SelectMainMenu();
				break;
			default:
				bTimePassed = false;
			}
			
			{
				// 화살표 키처리
				int x1 = 0, y1 = 0;
				bool keyPressed = true;

				switch (key)
				{
					case avej_lite::INPUT_KEY_UP:    y1 = -1; break;
					case avej_lite::INPUT_KEY_DOWN:  y1 =  1; break;
					case avej_lite::INPUT_KEY_LEFT:  x1 = -1; break;
					case avej_lite::INPUT_KEY_RIGHT: x1 =  1; break;
					default:
						keyPressed = false;
				}

				if (keyPressed || bTimePassed)
				{
					unsigned char tile = map(party.x + x1, party.y + y1);
					(this->*map.actList[tile])(x1, y1, true);
				}

			}
		}

		std::for_each(window.begin(), window.end(), FnctDisplay<CWindow*>());

		pGfxDevice->EndDraw();

		pGfxDevice->Flip();

		return true;
	}
};

CGameMain* s_pGameMain;

////////////////////////////////////////////////////////////////////////////////
// extern namespace

#include <stdarg.h>

namespace game
{
	//! 인덱스 컬러에 대해 실제 컬러 값을 돌려 준다.
	//! 인덱스 컬러는 미리 설정된 16개의 컬러이다.
	unsigned long GetRealColor(int index)
	{
		static const unsigned long s_COLOR_TABLE[16] =
		{
			0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
			0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
			0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
			0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
		};

		if ((index < 0) || (index >= 16))
			index = 15;

		return pBackBuffer->Color(s_COLOR_TABLE[index]);
	}

	//! 이전의 맵 상의 위치로 복귀한다.
	void WarpPrevPos(void)
	{
		s_pGameMain->party.Warp(TPcParty::POS_PREV);
	}

	//! 아무 키나 누를 때까지 메시지를 출력하고 대기한다.
	void PressAnyKey(void)
	{
		//?? 만약 글자가 끝까지 진행했다면 press any key를 표현하기 위해 스크롤이 되어야 한다.
		int xRegion, yRegion, wRegion, hRegion;
		s_pGameMain->window[WINDOW_CONSOLE]->GetRegion(&xRegion, &yRegion, &wRegion, &hRegion);

		g_DrawText(xRegion, yRegion+hRegion-15, "아무키나 누르십시오 ...", game::GetRealColor(14));
		game::UpdateScreen();

		CKeyBuffer& keyBuffer = GetKeyBuffer();

		while (keyBuffer.IsKeyPressed())
			keyBuffer.GetKey();

		while (!keyBuffer.IsKeyPressed())
			AvejUtil::Delay(20);

		keyBuffer.GetKey();

		// 화면 clear
		CLoreConsole& console = GetConsole();

		console.Clear();
		console.Display();
		game::UpdateScreen();

	}

	//! 아무 키나 누를 때까지 메시지를 출력없이 대기한다.
	void WaitForAnyKey(void)
	{
		CKeyBuffer& keyBuffer = GetKeyBuffer();

		while (keyBuffer.IsKeyPressed())
			keyBuffer.GetKey();

		while (!keyBuffer.IsKeyPressed())
			AvejUtil::Delay(20);

		keyBuffer.GetKey();
	}

	//! 지정한 시간만큼 대기한다.
	void Wait(unsigned long msec)
	{
		AvejUtil::Delay(msec);
	}

	//! 백버퍼의 내용을 실제 화면에 반영한다.
	void UpdateScreen(void)
	{
		pGfxDevice->EndDraw();
		pGfxDevice->Flip();
		pGfxDevice->BeginDraw(false);
	}

	//! 명시한 스크립트 파일로부터 새로 스크립트를 시작한다.
	bool LoadScript(const char* szFileName, int xStart, int yStart)
	{
		if (!s_pGameMain->LoadScript(szFileName))
			return false;

		if (xStart > 0 || yStart > 0)
		{
			s_pGameMain->party.x = xStart;
			s_pGameMain->party.y = yStart;
		}

		// 시작시 바라보는 방향을 정함
		if (s_pGameMain->party.y > s_pGameMain->map.height / 2)
			s_pGameMain->party.Face(0, -1);
		else
			s_pGameMain->party.Face(0, 1);

		game::window::DisplayMap();

		return true;
	}

	void GameOver(void)
	{
		s_pGameMain->GameOver(CGameMain::EXIT_BY_FORCE);
	}

	namespace map
	{
		//! 맵을 초기화하고 그 크기를 지정한다.
		void Init(int width, int height)
		{
			s_pGameMain->map.SetType(TMap::TYPE_TOWN);
			s_pGameMain->map.width  = width;
			s_pGameMain->map.height = height;
			s_pGameMain->map.encounterOffset = 0;
			s_pGameMain->map.encounterRange  = 0;
			CLEAR_MEMORY(s_pGameMain->map.data);
			CLEAR_MEMORY(s_pGameMain->map.handicapData);
		}

		//! 현재 맵에서 적의 출현 범위를 지정한다.
		void SetType(TMap::TType _type)
		{
			s_pGameMain->map.SetType(_type);
		}

		//! 현재 맵에서의 핸디캡을 설정한다.
		void SetHandicap(int handicap)
		{
			s_pGameMain->map.SetHandicap(TMap::THandicap(handicap));
		}

		//! 현재 맵에서 적의 출현 범위를 지정한다.
		void SetEncounter(int offset, int range)
		{
			ASSERT(offset > 0 && range >= 0);
			s_pGameMain->map.encounterOffset = offset;
			s_pGameMain->map.encounterRange  = range;
		}

		//! 현재 맵에서 시작 지점을 지정한다.
		void SetStartPos(int x, int y)
		{
			s_pGameMain->party.x = x;
			s_pGameMain->party.y = y;
		}

		//! 맵 데이터를 행 단위로 입력한다.
		void Push(int row, unsigned char* pData, int nData)
		{
			if (row >= 0 && row < s_pGameMain->map.height)
			{
				if (nData <= 0)
					return;
				if (nData > s_pGameMain->map.width)
					nData = s_pGameMain->map.width;

				memcpy(&s_pGameMain->map.data[row][0], pData, nData);
			}
		}
		//! 맵의 (x,y) 지점의 타일 값을 변경한다.
		void Change(int x, int y, int tile)
		{
			s_pGameMain->map.ChangeMap(x, y, tile);
		}
		void SetLight(int x, int y)
		{
			s_pGameMain->map.SetLight(x, y);
		}
		void ResetLight(int x, int y)
		{
			s_pGameMain->map.ResetLight(x, y);
		}
		//! 맵의 (x,y) 지점이 올바른 이동 지점인지를 알려 준다.
		bool IsValidWarpPos(int x, int y)
		{
			return s_pGameMain->IsValidWarpPos(x, y);
		}
		//! 파일로부터 맵을 읽는다.
		bool LoadFromFile(const char* szFileName)
		{
			if (!s_pGameMain->LoadMapFromFile(szFileName))
				return false;

			return true;
		}
	}

	namespace console
	{
		//! 콘솔 창에, 명시한 컬러의 명시한 가변 파라미터의 문자열 집합을 출력한다.
		void WriteConsole(unsigned long index, int nArg, ...)
		{
			CLoreConsole& console = GetConsole();

			console.SetTextColorIndex(index);
			avej_lite::util::string s;
			{
				va_list argptr;
				va_start(argptr, nArg);
				for (int i = 0; i < nArg; i++)
					s += va_arg(argptr, char*);
				va_end(argptr);
			}

			console.Write(s);
			//@@ 성능 문제
			console.Display();
		}

		//! 콘솔 창에, 명시한 컬러의 명시한 문자열을 출력한다.
		void WriteLine(const char* szText, unsigned long color)
		{
			CLoreConsole& console = GetConsole();

			console.SetTextColor(color);
			console.Write(szText);
			console.Display();
			//@@ 속도 저하가 생기지는 않을까?
			game::UpdateScreen();
		}

		//! 콘솔 창에, 명시한 컬러의 명시한 문자열을 새로 출력한다.
		void ShowMessage(unsigned long index, const char* s)
		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(index);
			console.Write(s);
			console.Display();
		}

		//! 콘솔 창에 출력되는 텍스트의 alignment 속성을 설정한다.
		void SetTextAlign(TTextAlign align)
		{
			GetConsole().SetTextAlign(CLoreConsole::TTextAlign(align));
		}
	}

	namespace tile
	{
		//?? 임시
		const int TILE_X_SIZE = 24;
		const int TILE_Y_SIZE = 24;
		//! 지정한 타일을 임시 타일(55번)에 복사한다.
		void CopyToDefaultTile(int ixTile)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(55*TILE_X_SIZE, yDest, pTileImage, ixTile*TILE_X_SIZE, yDest, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! 지정한 타일을 지정한 타일에 복사한다.
		void CopyTile(int srcTile, int dstTile)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(dstTile*TILE_X_SIZE, yDest, pTileImage, srcTile*TILE_X_SIZE, yDest, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! 지정한 스프라이트를 임시 타일(55번)에 복사한다.
		void CopyToDefaultSprite(int ixSprite)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(55*TILE_X_SIZE, yDest, pSpriteImage, ixSprite*TILE_X_SIZE, 0, TILE_X_SIZE, TILE_Y_SIZE);
		}
	}

	namespace window
	{
		//! 맵을 화면에 출력한다.
		void DisplayMap(void)
		{
			s_pGameMain->window[WINDOW_MAP]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_MAP]->Display((s_pGameMain->map.type == TMap::TYPE_DEN) ? 1 : 0, s_pGameMain->party.ability.magicTorch);
		}
		//! 콘솔 창을 화면에 출력한다.
		void DisplayConsole(void)
		{
			s_pGameMain->window[WINDOW_CONSOLE]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_CONSOLE]->Display();
		}
		//! 상태 창을 화면에 출력한다.
		void DisplayStatus(void)
		{
			s_pGameMain->window[WINDOW_STATUS]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_STATUS]->Display();
		}
		//! 전투 창을 화면에 출력한다.
		void DisplayBattle(int param1)
		{
			s_pGameMain->window[WINDOW_BATTLE]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_BATTLE]->Display(param1);
		}
		//! 콘솔 창의 크기를 돌려 준다.
		void GetRegionForConsole(int* pX, int* pY, int* pW, int* pH)
		{
			GetConsole().GetRegion(pX, pY, pW, pH);
			// 아래처럼 하면 안됨. console window 내에 client영역을 얻어야 하기 때문임.
			// s_pGameMain->window[WINDOW_CONSOLE]->GetRegion(pX, pY, pW, pH);
		}
	}

	namespace status
	{
		//! 현재 전투 상태인지 아닌지를 알려 준다.
		bool InBattle(void)
		{
			return (s_pGameMain->gameState == CGameMain::GAME_STATE_BATTLE);
		}
	}

	namespace object
	{
		//! 아군 리스트를 vector 형식으로 돌려 준다.
		std::vector<TPcPlayer*>& GetPlayerList(void)
		{
			return s_pGameMain->player;
		}
		//! 적의 리스트를 vector 형식으로 돌려 준다.
		std::vector<TPcEnemy*>&  GetEnemyList(void)
		{
			return s_pGameMain->enemy;
		}
		//! 파티의 정보를 돌려 준다.
		TPcParty& GetParty(void)
		{
			return s_pGameMain->party;
		}
		//! 현재 진행 중인 맵의 정보를 돌려 준다.
		TMap& GetMap(void)
		{
			return s_pGameMain->map;
		}
	}

	namespace variable
	{
		//! 명시한 게임 변수를 설정한다.
		void Set(int index, int value)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			s_pGameMain->gameOption.variable[index] = value;
		}
		//! 명시한 게임 변수의 값을 1증가 시킨다.
		void Add(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			++s_pGameMain->gameOption.variable[index];
		}
		//! 명시한 게임 변수의 값을 돌려 준다.
		int Get(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return false;
			}

			return s_pGameMain->gameOption.variable[index];
		}
	}
	
	namespace flag
	{
		//! 명시한 게임 플래그를 설정한다.
		void Set(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_pGameMain->gameOption.flag[index] = true;
		}
		//! 명시한 게임 플래그를 해제한다.
		void Reset(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_pGameMain->gameOption.flag[index] = false;
		}
		//! 명시한 게임 플래그의 설정 여부를 돌려 준다.
		bool IsSet(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return false;
			}

			return s_pGameMain->gameOption.flag[index];
		}
	}

	namespace battle
	{
		//?? 여기에 전투 최종 결과 값을 넣어야 한다.
		static CGameMain::EBattleResult s_result = CGameMain::BATTLE_RESULT_EVADE;

		//! 전투 상황에 돌입했음을 알려 준다.
		void Init(void)
		{
			s_pGameMain->enemy.clear();
		}
		//! 현재의 데이터로 전투를 한다.
		void Start(bool bAssualt)
		{
			s_result = s_pGameMain->BattleMode(bAssualt);
			s_pGameMain->ChangeWindowForField();
		}
		//! 전투에 참가하는 적을 추가한다. 입력 방식은 적의 index 번호를 넘겨 주는 것이다.
		void RegisterEnemy(int ixEnemy)
		{
			s_pGameMain->RegisterEnemy(ixEnemy);
		}
		//! 전투에 참가하는 적을 화면에 표시한다.
		void ShowEnemy(void)
		{
			s_pGameMain->ChangeWindowForBattle();
			game::UpdateScreen();
		}
		//! 전투의 결과를 돌려준다.
		int  Result(void)
		{
			switch (s_result)
			{
			case CGameMain::BATTLE_RESULT_EVADE:
				return 0;
			case CGameMain::BATTLE_RESULT_WIN:
				return 1;
			case CGameMain::BATTLE_RESULT_LOSE:
				return 2;
			default:
				ASSERT(0);
				return 1;
			}
		}
	}

	namespace party
	{
		//! 
		int PosX(void)
		{
			return s_pGameMain->party.x;
		}
		//! 
		int PosY(void)
		{
			return s_pGameMain->party.y;
		}
		//! 
		void PlusGold(int plus)
		{
			s_pGameMain->party.gold += plus;
		}
		//!
		void Move(int x1, int y1)
		{
			s_pGameMain->party.Move(x1, y1);
		}
		//!
		bool CheckIf(ECheckIf check)
		{
			switch (check)
			{
			case CHECKIF_MAGICTORCH:
				return (s_pGameMain->party.ability.magicTorch > 0);
			case CHECKIF_LEVITATION:
				return (s_pGameMain->party.ability.levitation > 0);
			case CHECKIF_WALKONWATER:
				return (s_pGameMain->party.ability.walkOnWater > 0);
			case CHECKIF_WALKONSWAMP:
				return (s_pGameMain->party.ability.walkOnSwamp > 0);
			case CHECKIF_MINDCONTROL:
				return (s_pGameMain->party.ability.mindControl > 0);
			default:
				ASSERT(0);
				return false;
			}
		}
	}
	
	namespace player
	{
		//! 현재 의식이 있는 아군의 수를 돌려 준다.
		int GetNumOfConsciousPlayer(void)
		{
			int nAlive = 0;
			for (std::vector<TPcPlayer*>::iterator obj = s_pGameMain->player.begin(); obj != s_pGameMain->player.end(); ++obj)
				nAlive += ((*obj)->IsConscious()) ? 1 :0;

			return nAlive;
		}

		bool IsAvailable(int ixPlayer)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return false;

			return s_pGameMain->player[ixPlayer]->Valid();
		}
		const char* GetName(int ixPlayer)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return static_cast<const char*>(0);

			return s_pGameMain->player[ixPlayer]->GetName();
		}

		const char* GetGenderName(int ixPlayer)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return static_cast<const char*>(0);

			return s_pGameMain->player[ixPlayer]->GetGenderName();
		}
		void AssignFromEnemyData(int ixPlayer, int ixEnemy)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return;

			*s_pGameMain->player[ixPlayer] << GetEnemyData(ixEnemy);
		}
		void ChangeAttribute(int ixPlayer, const char* szAttrib, int value)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return;

			s_pGameMain->player[ixPlayer]->ChangeAttribute(szAttrib, value);
		}
		void ChangeAttribute(int ixPlayer, const char* szAttrib, const char* data)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return;

			s_pGameMain->player[ixPlayer]->ChangeAttribute(szAttrib, data);
		}
		bool GetAttribute(int ixPlayer, const char* szAttrib, int& value)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return false;

			return s_pGameMain->player[ixPlayer]->GetAttribute(szAttrib, value);
		}
		bool GetAttribute(int ixPlayer, const char* szAttrib, avej_lite::util::string& data)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return false;

			return s_pGameMain->player[ixPlayer]->GetAttribute(szAttrib, data);
		}
		void ReviseAttribute(int ixPlayer)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return;

			s_pGameMain->player[ixPlayer]->ReviseAttribute();
		}
		void ApplyAttribute(int ixPlayer)
		{
			if ((ixPlayer < 0) || (ixPlayer >= s_pGameMain->player.size()))
				return;

			s_pGameMain->player[ixPlayer]->ApplyAttribute();
		}
	}

	namespace enemy
	{
		//! 현재 의식이 있는 적의 수를 돌려 준다.
		int GetNumOfConsciousEnemy(void)
		{
			int nAlive = 0;
			for (std::vector<TPcEnemy*>::iterator obj = s_pGameMain->enemy.begin(); obj != s_pGameMain->enemy.end(); ++obj)
				nAlive += ((*obj)->IsConscious()) ? 1 :0;

			return nAlive;
		}

		void ChangeAttribute(int ixEnemy, const char* szAttrib, int value)
		{
			if ((ixEnemy < 0) || (ixEnemy >= s_pGameMain->enemy.size()))
				return;

			s_pGameMain->enemy[ixEnemy]->ChangeAttribute(szAttrib, value);
		}
		void ChangeAttribute(int ixEnemy, const char* szAttrib, const char* data)
		{
			if ((ixEnemy < 0) || (ixEnemy >= s_pGameMain->enemy.size()))
				return;

			s_pGameMain->enemy[ixEnemy]->ChangeAttribute(szAttrib, data);
		}
	}

	namespace select
	{
		//?? CSelect의 default parameter에 대한 변경을 하는 것이 필요할지도 모름
		static TMenuList s_menu;
		static int s_result = 0;

		//! 선택 아이템을 초기화 한다.
		void Init(void)
		{
			s_menu.clear();
			s_result = 0;
		}
		//! 선택 아이템을 추가 한다.
		void Add(const char* szString)
		{
			s_menu.push_back(szString);
		}
		//! 현재 선택된 아이템으로 선택을 한다.
		void Run(void)
		{
			s_result = CSelect(s_menu)();
		}
		//! 선택된 결과를 가져 온다.
		int  Result(void)
		{
			return s_result;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// main

void _MakeFile(const char* szFileName, const char* szContents)
{
	FILE* fp = fopen(szFileName, "wt");
	if (szContents)
	{
		fprintf(fp, szContents);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static bool OnCreate(void)
{
	// 그래픽 객체 생성
	pGfxDevice = IGfxDevice::GetInstance();

	pGfxDevice->SetLayerDesc(s_layer_option);
	pGfxDevice->GetSurface(&pBackBuffer);

	// 리소스 로딩
	pGfxDevice->CreateSurfaceFrom("lore_tile.tga", &pTileImage);

	pSpriteImage = pFontImage = pTileImage;

	// 게임 객체 초기화
	s_pGameMain = new CGameMain;

	// 초기 디폴트 스크립트 로딩
	s_pGameMain->LoadScript("startup.cm2");

	ASSERT(s_pGameMain->party.x >= 0 && s_pGameMain->party.y >= 0);

	return true;
}

static bool OnDestory(void)
{
	delete pTileImage;

	pSpriteImage = NULL;
	pFontImage   = NULL;
	pTileImage   = NULL;
	
	pGfxDevice->Release();

	delete s_pGameMain;

	return true;
}


//?? 정확한 위치는?
static bool           s_isKeyPressed = false;
static unsigned short s_pressedKey;
static unsigned long  s_repeatTime;
const  unsigned long  c_delayTime = 75;

static bool OnJoyDown(unsigned short button)
{
	// auto pressed key 구현
	if (!s_isKeyPressed)
	{
		s_isKeyPressed = true;
		s_pressedKey   = button;
		s_repeatTime   = AvejUtil::GetTicks() + c_delayTime*3;
	}

	return GetKeyBuffer().SetKeyDown(button);
}

static bool OnJoyUp(unsigned short button)
{
	// auto pressed key 구현
	if (s_isKeyPressed)
	{
		s_isKeyPressed = false;
	}

	return GetKeyBuffer().SetKeyUp(button);
}

static bool OnKeyDown(unsigned short key, unsigned long state)
{
	return OnJoyDown(key);
}

static bool OnKeyUp(unsigned short key, unsigned long state)
{
	return OnJoyUp(key);
}

namespace lorebase
{

void OnProcessKey(void)
{
	static bool s_is_first = true;
	static bool S_KEY_STATE_TABLE[INPUT_KEY_MAX];

	if (s_is_first)
	{
		memset(S_KEY_STATE_TABLE, 0, sizeof(S_KEY_STATE_TABLE));
		s_is_first = false;
	}

	avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();

	input_device.UpdateInputState();

	for (int i = avej_lite::INPUT_KEY_BEGIN; i < avej_lite::INPUT_KEY_END; i++)
	{
		if (input_device.IsKeyHeldDown(avej_lite::TInputKey(i)))
		{
			if (S_KEY_STATE_TABLE[i] == false)
			{
				OnKeyDown(i, 0);
				S_KEY_STATE_TABLE[i] = true;
			}
		}
		else
		{
			if (S_KEY_STATE_TABLE[i])
			{
				OnKeyUp(i, 0);
				S_KEY_STATE_TABLE[i] = false;
			}
		}
	}
}

}

static bool OnProcess(void)
{
	lorebase::OnProcessKey();
	
	// auto pressed key 구현
	{
		unsigned long currentTick = AvejUtil::GetTicks();

		if (s_isKeyPressed)
		{
			if (s_repeatTime <= currentTick)
			{
				GetKeyBuffer().SetKeyUp(s_pressedKey);
				GetKeyBuffer().SetKeyDown(s_pressedKey);
				s_repeatTime = currentTick + c_delayTime;
			}
		}
	}

	return s_pGameMain->Process();
}

////////////////////////////////////////////////////////////////////////////////
// main

#include "avejapp_register.h"

static IAvejApp* p_app = NULL;

static bool s_Initialize(void* h_window)
{
	avej_lite::util::SetResourcePath("../../Res/res_LoreBase");

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
	return (p_app) ? p_app->Process() : false;
}

static bool GetAppCallback(TAppCallback& out_callback)
{
	out_callback.Initialize = s_Initialize;
	out_callback.Finalize   = s_Finalize;
	out_callback.Process    = s_Process;

	return true;
}

//?? 현재는 동작하지 않음
REGISTER_APP_1("LoreBase", GetAppCallback);

/*
int AvejMain(void)
{
	AppCallback callBack =
	{
		OnCreate,
		OnDestory,
		OnProcess
	};

	IAvejApp* pApp = NULL;

	try
	{
		pApp = IAvejApp::GetInstance(callBack);
		if (pApp == NULL)
			throw 0;

		pApp->Process();

		pApp->Release();
	}
	catch (EExitGame&)
	{
		if (pApp)
			pApp->Release();
	}
	catch (...)
	{
		ASSERT(false);
	}

	return 0;
}
*/
void QAZ(unsigned char* pBuffer, unsigned int size, unsigned char seed, unsigned char add)
{
	while (size-- > 0)
	{
		*pBuffer ^= seed;
		seed += add;
		++pBuffer;
	}
}

void ModifySeed(unsigned int times, unsigned char& seed, unsigned char& add)
{
	while (times-- > 0)
	{
		seed += add;
	}
}

/*
	[4] ID    : 실제 청크 ID 4자
	[4] size  : DATA의 크기
	[4] CRC32 : DATA의 CRC32
	[n] DATA  : DATA
*/

const std::string GetPredictMessage(unsigned int ixPredict)
{
	const static char* PREDICT_MESSAGE[26] =
	{
		"당신은 어떤 힘에 의해 예언을 방해 받고 있다",
		"Lord Ahn 을 만날",
		"MENACE를 탐험할",
		"Lord Ahn에게 다시 돌아갈",
		"LASTDITCH로 갈",
		"LASTDITCH의 성주를 만날",
		"PYRAMID 속의 Major Mummy를 물리칠",
		"LASTDITCH의 성주에게로 돌아갈",
		"LASTDITCH의 GROUND GATE로 갈",
		"GAIA TERRA의 성주를 만날",
		"EVIL SEAL에서 황금의 봉인을 발견할",
		"GAIA TERRA의 성주에게 돌아갈",
		"QUAKE에서 ArchiGagoyle를 물리칠",
		"북동쪽의 WIVERN 동굴에 갈",
		"WATER FIELD로 갈",
		"WATER FIELD의 군주를 만날",
		"NOTICE 속의 Hidra를 물리칠",
		"LOCKUP 속의 Dragon을 물리칠",
		"GAIA TERRA 의 SWAMP GATE로 갈",
		"위쪽의 게이트를 통해 SWAMP KEEP으로 갈",
		"SWAMP 대륙에 존재하는 두개의 봉인을 풀",
		"SWAMP KEEP의 라바 게이트를 작동 시킬",
		"적의 집결지인 EVIL CONCENTRATION으로 갈",
		"숨겨진 적의 마지막 요새로 들어갈",
		"위쪽의 동굴에서 Necromancer를 만날",
		"Necromancer와 마지막 결전을 벌일"
	};

	if (ixPredict > 25)
		ixPredict = 0;

	if (ixPredict > 0)
	{
		std::string message("당신은 ");
		message += PREDICT_MESSAGE[ixPredict];
		message += " 것이다";
		return message;
	}
	else
	{
		return std::string(PREDICT_MESSAGE[0]);
	}
}
