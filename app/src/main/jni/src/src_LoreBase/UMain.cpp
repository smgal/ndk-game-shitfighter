
/*

  ���� ���� �������� �� �ڵ��� status ������� ��
  ������ ���� ���� ���...
  �����Ĵ� ���� �߰�

*/

/*

DEPAPEPE
�ڷ� �ϸ������� (�̰� �ߵ����� �־��)


BUG list

������ ���� �θ��� default tile�� �����
��> ground1���� save�� �ߴµ��� 0�� �� ���� ����� �ƴ϶� ����� ��


TODO list

 - CONSOLE_WRITE ��� �� ���, ��� szTemp�� �ܺ� ���� �ؼ��� �ȵ�

- ������ ���� ���� ������
	�þ߰� �������ų� ���� �ٲ�� ����?

- ���ڿ��� resource ��
	UPcPlayer �ϴ� ��

- resource file�� �����Ͽ� ��� ������ �ϳ��� ���Ͽ� ����
	�����δ� file�� resource ���� �� �� �����ؾ� ��
	�Ǵ�, code�� resouce ���� ������ ����

- �ѱ� �Է� ��� �����

- character �����ϴ� �� �����

- press any key �� �ƴ϶� press enter key ���� ���� �Ǿ�� �����ʴ°�?

- g_?? �Լ����� ��� global�� ���ϴ� namespace�� ����
	main.h �� �ִ� �Ʒ��� �� �Լ��� �����
	void g_DrawBltList(int x, int y, CSmFont12x12::TBltParam rect[], int nRect = 0x7FFFFFFF);
	void g_DrawText(int x, int y, const char* szText, unsigned long color = 0xFFFFFFFF);

- event�� ���Ǵ� �Լ����� �Ŵ���� ����
- ��ũ��Ʈ ������ ���� ����� �޴� ���
  - �������� �������� �� ȸ�ǰ� �Ұ��� �ϰ� �ϴ� �ó�����
  - �������� �������� �� ���� �� ��ŭ�� �ٽ� ��Ÿ���� �ó�����

- Doxygen ���� �����



���� ����

- resource ��ȣȭ seed: byte; adder: byte �� xor������ ��ȣȭ

- ��� ���� ��ü map file ���� �ʿ�, �����ϴٸ� 3ź�� map ������ ��� ����

- script�� ���� ������ �̸� �о� ���� ������� �ӵ� ����
- ���� ���� singleton ����
	Script ��ü
	GameMain ��ü
- factory ����


CODING GUIDE

- localization ����Ͽ� ���ڿ��� ���� ����
- 16-bit color�� ���� conversion�� �׻� �ʿ�
- strcpy -> strncpy
- sprintf -> snprintf
- class ����
  - �����ϴ� ������ protected��
  - private ����/�Լ��� m_*
  - protected ������ ���� �̸�, �Լ��� _*
  - public ������ ������� ����, �Լ��� ���� �̸�
- ??(���� �ؾ� �ϴ� �κ�) @@(���� ����)

- singleton ���
	CLoreConsole


  ������ ���� ���

- Ư�� ����� ��⸸ �ص� �Ϲ� ESP�� ����� �� ����
- ���ɼ��� ����ص� ESP ������ 30�� ������ ����
- ���� �Ʊ����� ���� �� 20������ ���� ����ġ�� 5100000�� �ƴ� 510000�� �Ҵ��
- ���� ���ɼ��� �ɾ� 6��° �Ʊ��� ������ �� �ִµ�, 6��° �Ʊ��� ���� n���� �Ǵ� ���� �ƴ϶� n��° �Ʊ��� ���� 6���� �ȴ�. ������ ������� ���� 6��° �Ʊ��̴�.
- �����/õ���ȴ� ������忡���� ����� ���� �����ϴ�. <- ���簡 Ʋ����

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
	// �׻� UCS-2 ������ wchar_t�� ������ �Ǿ�� �Ѵ�.
	CT_ASSERT(sizeof(widechar) == sizeof(wchar_t), QAZ);

	static const IorzRenderText& ref_render_text = *GetTextInstance();

	if (sz_text_begin == NULL || sz_text_end == NULL || sz_text_begin >= sz_text_end )
		return;

	//?? ����� �� ������� ����
	static widechar w_str[256];

	// KSC-5601�� UCS-2�� ����
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
				// NULL ���ڷ� ������, '@'�� NULL�� ġȯ�Ǵ� ���̱� ������ �����ص� ���� ����
				*p_str_end = 0;

				// blitting list �����
				ref_render_text.RenderText(x, y, p_str_begin, current_color, fn_bit_blt);

				// x ���۰� ����
				x = x_end;

				// current_color ����
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
						current_color = default_color; // ����

					if (index_color >= 0)
						current_color = g_GetColorFromIndexedColor(index_color, default_color);
				}

				// ���ڿ��� ������ ����
				p_str_begin = ++p_str_end;
			}
			else
			{
				//?? ����� ��� ���� �ȵ�
				x_end += (*p_str_end > 0x00FF) ? 12 : 6;
				++p_str_end;
			}
		}

		// ���� ���ڿ��� ���� blitting list �����
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

	// window�� ����
	template <class type>
	class FnctDisplay
	{
	public:
		void operator()(type obj)
		{
			obj->Display();
		}
	};

	// �Ʊ��� ���� ����
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

	// �ǽ��� ���� �ִ� player�� ��
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
			// ��� �ִ� player�� ��
			return nAlive;
		}
	};

	// ��Ƽ�� swamp�� �̵����� �� ����Ǵ� ��Ģ
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
					game::console::WriteConsole(13, 2, obj->GetName(EJOSA_SUB), " �ߵ��Ǿ���.");

					if (obj->poison == 0)
						obj->poison = 1;

					++nUpdate;
				}
			}
		}
		int Result(void)
		{
			// �����Ͱ� ���ŵ� player�� ��
			return nUpdate;
		}
	};

	// ��Ƽ�� lava�� �̵����� �� ����Ǵ� ��Ģ
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
					game::console::WriteConsole(13, 4, obj->GetName(EJOSA_SUB), " ", IntToStr(damage)(), "�� ���ظ� �Ծ��� !");

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
			// �����Ͱ� ���ŵ� player�� ��
			return nUpdate;
		}
	};

	// �ð��� ���ų� ��Ƽ�� �̵����� �� ����Ǵ� ��Ģ
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
			// �����Ͱ� ���ŵ� player�� ��
			return nUpdate;
		}
	};

	// ��Ƽ�� ķ���� �� �� ����Ǵ� ��Ģ
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
					game::console::WriteConsole(4, 1, "������ �ķ��� �ٴڳ���");
				}
				else if (obj->dead > 0)
				{
					game::console::WriteConsole(7, 2, obj->GetName(EJOSA_SUB), " �׾���");
				}
				else if ((obj->unconscious > 0) && (obj->poison == 0))
				{
					obj->unconscious -= (obj->level[0] + obj->level[1] + obj->level[2]);
					if (obj->unconscious <= 0)
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " �ǽ��� ȸ���Ǿ���");

						obj->unconscious = 0;
						if (obj->hp <= 0)
							obj->hp = 1;

						--s_pGameMain->party.food;
					}
					else
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " ������ �ǽ� �Ҹ��̴�");
					}
				}
				else if ((obj->unconscious > 0) && (obj->poison > 0))
				{
					game::console::WriteConsole(7, 3, "�� ������ ", obj->GetName(EJOSA_NONE), "�� �ǽ��� ȸ������ �ʾҴ�");
				}
				else if (obj->poison > 0)
				{
					game::console::WriteConsole(7, 3, "�� ������ ", obj->GetName(EJOSA_NONE), "�� �ǰ��� ȸ������ �ʾҴ�");
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
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " ��� �ǰ��� ȸ���Ǿ���");
					}
					else
					{
						game::console::WriteConsole(15, 2, obj->GetName(EJOSA_SUB), " ġ��Ǿ���");
					}
					--s_pGameMain->party.food;
				}

				obj->sp  = obj->mentality * obj->level[1];
				obj->esp = obj->concentration * obj->level[2];
			}
		}
	};

	// ĳ���� ����Ʈ�� ��� ����� �����ִ� �Լ���  
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

	// ĳ���� ����Ʈ�� ��� ��ø���� �����ִ� �Լ���  
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

	// ĳ���� ����Ʈ�� ��� AC�� �����ִ� �Լ���  
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

	// ������ ������ �� �ִ� Ȳ���� ������ ����ϴ� �Լ���
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

		// player�� ���÷��� �� ��ġ�� ��ȭ�� ����ٸ�
		if (nUpdate > 0)
		{
			window[WINDOW_STATUS]->SetUpdateFlag();
		}

		DetectGameOver();
		
		// ���ɼ��� ��� ���̶�� ��� �ð��� ���� ��Ŵ
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
		console.Write("ǥ���ǿ��� ������ ���� ���� �־���.");
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

		// walkOnSwamp�� ���� ó��
		if (party.ability.walkOnSwamp > 0)
		{
			--party.ability.walkOnSwamp;
		}
		else
		{
			CLoreConsole& console = GetConsole();
			console.Clear();
			console.SetTextColorIndex(13);
			console.Write("������ ���� �ִ� �˿� ���� !!!");
			console.Write("");

			int nUpdate;

			nUpdate = std::for_each(player.begin(), player.end(), FnctEnterSwamp<TPcPlayer*>()).Result();

			console.Display();

			// player�� ���÷��� �� ��ġ�� ��ȭ�� ����ٸ�
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
			console.Write("������ �������� ���� !!!");
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
		// ���� �� window�� update �Ǿ�� �ϴ� �����ΰ�?
		bool m_bMustUpdate;
		// ���� �� window�� ���� ���̴� �����ΰ�?
		bool m_bVisible;

		const CGameMain* m_pGameMain;

	protected:
		// window�� ��ġ�� ũ��
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
			//@@ �߰����� ������ �� �ʿ��Ѱ�?
		}
	};

	class CWindowMap: public CWindow
	{
	public:
		enum
		{
			// ���� ������
			_X_RADIUS = 5,
			_Y_RADIUS = 5,
			// Ÿ���� ���� ũ��
			TILE_X_SIZE = 24,
			TILE_Y_SIZE = 24
		};

	private:
		// Ÿ�� ����� ���� ���� �ɼ�
		int m_xDisplayOffset;
		int m_yDisplayOffset;
		// ���� ���� ��� ������
		int m_wRadDisplay;
		int m_hRadDisplay;

	protected:
		void _OnDisplay(int param1, int param2)
		{
			bool bDisplayChara = true;
			const CGameMain& gameMain = *_GetMainInst();

			pBackBuffer->SetClipRect(m_x, m_y, m_w, m_h);

			// ������ �ƴϰų� ������ Ƚ���� ���� �ִ� ���¶��,
			bool bDaylight = (s_pGameMain->map.type != TMap::TYPE_DEN) || (s_pGameMain->party.ability.magicTorch > 0);

			// ��ο� ���¶�� �ϴ� ȭ���� �˰� �����.
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
						// (bDaylight == true)��� ������ ����ȭ �� �� ������...
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

			// ���ΰ� ���
			if (bDisplayChara)
			{
				int lookOffset = (gameMain.map.encounterRange == 0) ? 0 : 4;
				int x_src = 24*19;
				int y_src = (gameMain.party.face+lookOffset)*TILE_Y_SIZE;
				pBackBuffer->BitBlt((m_wRadDisplay-1)*TILE_X_SIZE+m_x+m_xDisplayOffset, (m_hRadDisplay-1)*TILE_Y_SIZE+m_y+m_yDisplayOffset, pSpriteImage, x_src, y_src, TILE_X_SIZE, TILE_Y_SIZE);
			}

			pBackBuffer->SetClipRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

#if defined(_DEBUG)
			// ���� ��ǥ ��� (�ӽ�)
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
				// ���� ��ȿ���� ���� ������ ������ ��쿡�� ���� ����� ���� �ʴ´�.
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

				// ��ü ������ background color �� ä�� ��, CLoreConsole�� ���� client ������ ä��
				unsigned long bgColor = pBackBuffer->Color(0xFF, 0x40, 0x40, 0x40);
				pBackBuffer->FillRect(bgColor, m_x, m_y, m_w, m_h);

 				// ���۽� console ȭ���� bg color�� ä��
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
			// ���� ������ �׻� player�� ���� �����Ѵ�.
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

				// 320*240 ������ condition�� �̸����� ������� �ʰ� �̸� ��½� ����� ���ǵ�
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

		// save�� �ؾ� �ϴ� object�� ��� ���
		{
			//?? ��� ������ ������ ��
			saveList.push_back(&party);

			for (int i = 0; i < MAX_PLAYER; i++)
				saveList.push_back(&m_player[i]);

			saveList.push_back(&map);
			saveList.push_back(&gameOption);
		}

		// party �ʱ�ȭ
		{
			// �ܺο��� �� ���� �����Ǿ����� ��
			party.x = -1;
			party.y = -1;
			//??
			party.ability.canUseESP = false;
			party.ability.canUseSpecialMagic = false;
		}

		// player �ʱ�ȭ
		{
			m_player[0].SetDefault(0);

			m_player[0].SetName("����");
			m_player[0].class_ = 8;
			m_player[0].level[0] = 3;
			m_player[0].level[1] = 1;
			m_player[0].level[2] = 0;
			m_player[0].ReviseAttribute();

/*@@ ���� �ӽ� party �����

			m_player[4].SetDefault(0);
			m_player[4].SetName("Becrux");
			m_player[4].hp = 100;

			m_player[2].SetDefault(0);
			m_player[2].SetName("����");
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
		
		// map �ʱ�ȭ
		{
			// map���� default handler�� �˷���
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

		// window �ʱ�ȭ
		{
			// �Ʒ� window���� ���� ������ ������ ����

			// Map�� window
			{
				CWindow* pWindow = new CWindowMap(this);
				const config::TRect& region = config::c_mapWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Console�� window
			{
				CWindow* pWindow = new CWindowConsole(this);
				const config::TRect& region = config::c_consoleWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Status�� window
			{
				CWindow* pWindow = new CWindowStatus(this);
				const config::TRect& region = config::c_statusWindowRegion;
				pWindow->SetRegion(region.x, region.y, region.w, region.h);
				window.push_back(pWindow);
			}

			// Battle�� window
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

		// ���� ���� map type�� �Ǻ��� �־�� ��
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

	// 0~(n-1) ������ ��ȿ ���� �Ǵ� -1�� ��ȿ ���ڸ� ���� ��
	int SelectPlayer(const char* szTitle = NULL)
	{
		if (szTitle == NULL)
			szTitle = "�Ѹ��� ���ÿ� ---";

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

	// 0~(n-1) ������ ��ȿ ����
	// ���� -1�� ��ȿ ���ڸ� ���� ���� ����
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
/* ��ȿ ���ڸ� �������� �ʰ� ��
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
		// ��ó��
		std::for_each(player.begin(), player.end(), FnctCheckCondition<TPcPlayer*>());

		// ���� ������ �ִ� ���ڸ� Ȯ��
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

		game::console::WriteConsole(15, 3, "������ ", IntToStr(nGold)(), "���� ���� �����.");
	}

	int RegisterEnemy(int index)
	{
		int ixEnemy = enemy.size();

		m_enemy[ixEnemy] << GetEnemyData(index);
		m_enemy[ixEnemy].E_number = index;
		enemy.push_back(&m_enemy[ixEnemy]);

		return ixEnemy;
	}

	// �������� �ºη� ���̳��� true, ������ ġ�� false
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

			// ��� ���
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
							str1 += "�� ���� ��� ===>";

							avej_lite::util::string str2;
							str2 += "�� ���� ���� ";
							str2 += resource::GetWeaponName((*obj)->weapon).szName;
							str2 += resource::GetWeaponName((*obj)->weapon).szJosaWith;
							str2 += "�� ����";

							menu.reserve(8);
							menu.push_back(str1);
							menu.push_back(str2);
							menu.push_back("�� ���� ������ ���� ����");
							menu.push_back("��� ������ ���� ����");
							menu.push_back("������ Ư�� ���� ����");
							menu.push_back("������ ġ��");
							menu.push_back("������ �ʴɷ� ���");

							if (order == 0)
								menu.push_back("���࿡�� ������ ���� �� ���� ����");
							else
								menu.push_back("������ �õ���");

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

						// �޴� ���ÿ� ���� ���� ��� ����
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
								menu.push_back("����");
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
									{ // @@ dead �³�?
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

			// ��� �ؼ�
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

			// ���ۿ����� �� ���� �Ŀ� Press Any Key ���� ����� ������
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
		console.Write("���� �����ߴ� !!!");
		console.Write("");

		console.SetTextColorIndex(11);
		console.Write("���� ��� ��ø��");
		{
			char szTemp[256];
			int avgAgility = std::for_each(enemy.begin(), enemy.end(), FnctAvgAgility<TPcEnemy*>()).Result();
			CONSOLE_WRITE(" : %d", avgAgility);
		}

		console.Display();

		ChangeWindowForBattle();

		game::UpdateScreen();

		// ���� ���ڸ� ����ϱ� ���ؼ� �ӽ÷� console â�� ����� ���δ�.
		config::TRect savedRect;
		int reducedSize = 4*config::c_hFont;

		window[WINDOW_CONSOLE]->GetRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
		window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

		TMenuList menu;

		menu.reserve(3);
		menu.push_back("");
		menu.push_back("���� �����Ѵ�");
		menu.push_back("��������");

		bool willingToAvoidBattle = (CSelect(menu)() != 1);

		// console â�� ����� ���ͽ�Ų��.
		window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y, savedRect.w, savedRect.h);

		// '��������'�� �������� �� �� ������ bg color�� ä��� ����
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
					break; // ������ ȸ��

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

		CONSOLE_WRITE("X �� = %d", party.x);
		CONSOLE_WRITE("Y �� = %d", party.y);
		CONSOLE_WRITE("���� �ķ� = %d", party.food);
		CONSOLE_WRITE("���� Ȳ�� = %d", party.gold);

		console.Write("");

		CONSOLE_WRITE("������ ȶ�� : %d", party.ability.magicTorch);
		CONSOLE_WRITE("���� �λ�   : %d", party.ability.levitation);
		CONSOLE_WRITE("������ ���� : %d", party.ability.walkOnWater);
		CONSOLE_WRITE("������ ���� : %d", party.ability.walkOnSwamp);

		console.Display();
	}

	void ShowCharacterStatus(void)
	{
		int selected = SelectPlayer("�ɷ��� ������� �ι��� �����Ͻÿ�");
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
		CONSOLE_WRITE("# �̸� : %s", selected.GetName())
		CONSOLE_WRITE("# ���� : %s", selected.GetGenderName())
		CONSOLE_WRITE("# ��� : %s", selected.GetClassName())

		console.Write("");

		console.SetTextColorIndex(3);
		CONSOLE_WRITE("ü��   : %d", selected.strength)
		CONSOLE_WRITE("���ŷ� : %d", selected.mentality)
		CONSOLE_WRITE("���߷� : %d", selected.concentration)
		CONSOLE_WRITE("�γ��� : %d", selected.endurance)
		CONSOLE_WRITE("���׷� : %d", selected.resistance)
		CONSOLE_WRITE("��ø�� : %d", selected.agility)
		CONSOLE_WRITE("���   : %d", selected.luck)

		console.Display();
		game::UpdateScreen();

		game::PressAnyKey();

		console.Clear();

		// �� �κ��� �� �� �� �ݺ�
		console.SetTextColorIndex(11);
		CONSOLE_WRITE("# �̸� : %s", selected.GetName())
		CONSOLE_WRITE("# ���� : %s", selected.GetGenderName())
		CONSOLE_WRITE("# ��� : %s", selected.GetClassName())

		console.Write("");

		console.SetTextColorIndex(3);
		CONSOLE_WRITE2("������ ��Ȯ��   : %2d    ���� ����   : %2d", selected.accuracy[0], selected.level[0])
		CONSOLE_WRITE2("���ŷ��� ��Ȯ�� : %2d    ���� ����   : %2d", selected.accuracy[1], selected.level[1])
		CONSOLE_WRITE2("�ʰ����� ��Ȯ�� : %2d    �ʰ��� ���� : %2d", selected.accuracy[2], selected.level[2])
		CONSOLE_WRITE("## ����ġ   : %d", selected.experience)

		console.Write("");

		console.SetTextColorIndex(2);
		CONSOLE_WRITE("��� ���� - %s", selected.GetWeaponName())
		CONSOLE_WRITE2("���� - %s           ���� - %s", selected.GetShieldName(), selected.GetArmorName())

		console.Display();
	}

	void ShowQuickView(void)
	{
		char szTemp[256];

		CLoreConsole& console = GetConsole();

		console.Clear();

		console.SetTextColorIndex(15);
		console.Write("                �̸�    �ߵ�  �ǽĺҸ�    ����");
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
			game::console::WriteConsole(7, 2, player[selected]->Get3rdPersonName(), "�� ������ ����� �� �ִ� ���°� �ƴմϴ�.");
			return;
		}

		TMenuList menu;

		menu.reserve(4);
		menu.push_back("����� ������ ���� ===>");
		menu.push_back("���� ����");
		menu.push_back("ġ�� ����");
		menu.push_back("��ȭ ����");

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
			game::console::WriteConsole(7, 2, player[selected]->Get3rdPersonName(), "�� �ʰ����� ����Ҽ��ִ� ���°� �ƴմϴ�.");
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
		//@@ �ʿ��Ѱ�?
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

		menu.push_back("�ҷ� ���� ���� ������ �����Ͻʽÿ�.");
		menu.push_back("�����ϴ�");
		menu.push_back("�� ���� ����Ÿ");
		menu.push_back("���� ����Ÿ 1 (��)");
		menu.push_back("���� ����Ÿ 2 (��)");
		menu.push_back("���� ����Ÿ 3 (��)");

		int selected = CSelect(menu)();

		if (selected <= 1)
			return false;

		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(11);
			console.Write("�����ߴ� ������ �ٽ� �ҷ��ɴϴ�");
			console.Display();

			// 0~3 ���� ��
			if (!LoadGame(selected-2))
			{
				// ������ ���� ���� ����
				//?? ���õ� �޽����� ������ �ٽ� �޴��� �����ϰ� �ؾ� ��
				//?? �Ǵ� �ڵ����� �����Ͱ� �ִ����� Ȯ���Ͽ� �޴��� ���̶���Ʈ�� ���Ѿ� ��
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

		menu.push_back("������ ���� ��Ҹ� �����Ͻʽÿ�.");
		menu.push_back("�����ϴ�");
		menu.push_back("�� ���� ����Ÿ");
		menu.push_back("���� ����Ÿ 1 (��)");
		menu.push_back("���� ����Ÿ 2 (��)");
		menu.push_back("���� ����Ÿ 3 (��)");

		int selected = CSelect(menu)();

		if (selected <= 1)
			return false;

		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(12);
			console.Write("������ ������ �����մϴ�");
			console.Display();

			// 0~3 ���� ��
			if (!SaveGame(selected-2))
			{
				//?? ���� ������ �˷� ��� �ϳ�?
				return false;
			}

			console.SetTextColorIndex(7);
			console.Write("");
			console.Write("�����߽��ϴ�");
			console.Display();

			game::PressAnyKey();
		}

		return true;
	}

	void SelectGameOption(void)
	{
		TMenuList menu;

		menu.reserve(7);
		menu.push_back("���� ���� ��Ȳ");
		menu.push_back("���̵� ����");
		menu.push_back("���� ������ ���� ����");
		menu.push_back("���࿡�� ���� ��Ŵ");
		menu.push_back("������ ������ �簳");
		menu.push_back("������ ������ ����");
		menu.push_back("������ ��ħ");

		int selected = CSelect(menu)();

		switch (selected)
		{
		case 0:
			break;
		case 1:
			// ������ �����ϴ� ���� �ִ�ġ ����
			{
				menu.clear();
				menu.push_back("�ѹ��� �����ϴ� ������ �ִ�ġ�� �����Ͻʽÿ�");
				menu.push_back("3���� ����");
				menu.push_back("4���� ����");
				menu.push_back("5���� ����");
				menu.push_back("6���� ����");
				menu.push_back("7���� ����");

				int selected = CSelect(menu, -1, party.maxEnemy - 2)();

				if (selected == 0)
					break;

				party.maxEnemy = selected + 2;
			}

			// ������ �����ϴ� ���� �ִ�ġ ����
			{
				menu.clear();
				menu.push_back("������� ���� ������ ����ϱ� ?");
				menu.push_back("�Ϻη� ������ ���ϰ� �ʹ�");
				menu.push_back("�ʹ� ���� ������ ������ �ʴ´�");
				menu.push_back("����ģ ������ ������ �ϰڴ�");
				menu.push_back("���̴� ������� ��� �����ϰڴ�");
				menu.push_back("�׵��� �ǿ� ���ַ� �ִ�");

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
				console.Write("������ �Ͽ��� ���� ������ ���� �Ͻʽÿ�.");
				console.Write("");
				console.Display();

				menu.clear();
				menu.push_back("@B������ �ٲ� �Ͽ� 1@@");

				for (ixPlayer = 1; ixPlayer < 5; ixPlayer++)
				{
					if (player[ixPlayer]->Valid())
					{
						menu.push_back(player[ixPlayer]->GetName());
						*pIndex++ = ixPlayer;
					}
				}

				// ���ΰ��� ������ ������ 1�� ���϶�� �� ����� ������ �ʿ䰡 ����.
				if (menu.size() < 3)
				{
					CLoreConsole& console = GetConsole();

					console.Clear();
					console.SetTextColorIndex(7);
					console.Write("���� ������ ���� �� �ʿ䰡 �����ϴ�.");
					console.Display();

					game::PressAnyKey();

					break;
				}

				// ���� ���ڸ� ����ϱ� ���ؼ� �ӽ÷� console â�� ����� ���δ�.
				config::TRect savedRect;
				int reducedSize = 3*config::c_hFont;

				window[WINDOW_CONSOLE]->GetRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
				window[WINDOW_CONSOLE]->SetRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

				do
				{
					int selected1 = CSelect(menu)();
					if (selected1 == 0)
						break;

					menu[0] = "@B������ �ٲ� �Ͽ� 2@@";

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

						// ���� ��� ������ �ٲٱ�
						std::swap(player[ixPlayer1], player[ixPlayer2]);
						// order�� ��� ��ȣ�̹Ƿ� ������ ���ƾ� �Ѵ�
						std::swap(player[ixPlayer1]->order, player[ixPlayer2]->order);
					}

					window[WINDOW_STATUS]->SetUpdateFlag();
					window[WINDOW_STATUS]->Display();
				} while(0);

				// console â�� ����� ���ͽ�Ų��.
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
				menu.push_back("@C���࿡�� ���� ��Ű�� ���� ����� ���ʽÿ�.@@");

				for (ixPlayer = 1; ixPlayer < 6; ixPlayer++)
				{
					if (player[ixPlayer]->Valid())
					{
						menu.push_back(player[ixPlayer]->GetName());
						*pIndex++ = ixPlayer;
					}
				}

				// ���ΰ����̶�� �� ����� ������ �ʿ䰡 ����.
				if (menu.size() < 2)
				{
					CLoreConsole& console = GetConsole();

					console.Clear();
					console.SetTextColorIndex(7);
					console.Write("���ܽ�ų ������ �����ϴ�.");
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
		menu.push_back("����� ����� ���ÿ� ===>");
		menu.push_back("������ ��Ȳ�� ����");
		menu.push_back("������ ��Ȳ�� ����");
		menu.push_back("������ �ǰ� ���¸� ����");
		menu.push_back("������ ����Ѵ�");
		menu.push_back("�ʴɷ��� ����Ѵ�");
		menu.push_back("���⼭ ����");
		menu.push_back("���� ���� ��Ȳ");

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

				menu.push_back("������ �����ڽ��ϱ� ?"); // ������ 10�� ��
				menu.push_back("       << �ƴϿ� >>");
				menu.push_back("       <<   ��   >>");

				if (CSelect(menu)() != 2)
					return;
			}
			break;
		case EXIT_BY_ACCIDENT:
			{
				CLoreConsole& console = GetConsole();

				console.Clear();
				console.SetTextColorIndex(13);
				console.Write("������ �����߿� ��� ����� �Ҿ���.");
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
				console.Write("������ ��� �������� ���ߴ� !!");
				console.Display();

				game::PressAnyKey();

				console.Clear();

				TMenuList menu;
				menu.reserve(8);
				menu.push_back("    @A��� �Ͻðڽ��ϱ� ?@@");
				menu.push_back("   ������ ������ �簳�Ѵ�");
				menu.push_back("       ������ ������");

				int selected = CSelect(menu)();

				if (selected == 1)
				{
					if (SelectLoadMenu())
						return;
				}
			}
			break;
		case EXIT_BY_FORCE:
			// Ư���ϰ� ���ؾ��� ������ ����
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

			// command ó��
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
				// space ó��
				SelectMainMenu();
				break;
			default:
				bTimePassed = false;
			}
			
			{
				// ȭ��ǥ Űó��
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
	//! �ε��� �÷��� ���� ���� �÷� ���� ���� �ش�.
	//! �ε��� �÷��� �̸� ������ 16���� �÷��̴�.
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

	//! ������ �� ���� ��ġ�� �����Ѵ�.
	void WarpPrevPos(void)
	{
		s_pGameMain->party.Warp(TPcParty::POS_PREV);
	}

	//! �ƹ� Ű�� ���� ������ �޽����� ����ϰ� ����Ѵ�.
	void PressAnyKey(void)
	{
		//?? ���� ���ڰ� ������ �����ߴٸ� press any key�� ǥ���ϱ� ���� ��ũ���� �Ǿ�� �Ѵ�.
		int xRegion, yRegion, wRegion, hRegion;
		s_pGameMain->window[WINDOW_CONSOLE]->GetRegion(&xRegion, &yRegion, &wRegion, &hRegion);

		g_DrawText(xRegion, yRegion+hRegion-15, "�ƹ�Ű�� �����ʽÿ� ...", game::GetRealColor(14));
		game::UpdateScreen();

		CKeyBuffer& keyBuffer = GetKeyBuffer();

		while (keyBuffer.IsKeyPressed())
			keyBuffer.GetKey();

		while (!keyBuffer.IsKeyPressed())
			AvejUtil::Delay(20);

		keyBuffer.GetKey();

		// ȭ�� clear
		CLoreConsole& console = GetConsole();

		console.Clear();
		console.Display();
		game::UpdateScreen();

	}

	//! �ƹ� Ű�� ���� ������ �޽����� ��¾��� ����Ѵ�.
	void WaitForAnyKey(void)
	{
		CKeyBuffer& keyBuffer = GetKeyBuffer();

		while (keyBuffer.IsKeyPressed())
			keyBuffer.GetKey();

		while (!keyBuffer.IsKeyPressed())
			AvejUtil::Delay(20);

		keyBuffer.GetKey();
	}

	//! ������ �ð���ŭ ����Ѵ�.
	void Wait(unsigned long msec)
	{
		AvejUtil::Delay(msec);
	}

	//! ������� ������ ���� ȭ�鿡 �ݿ��Ѵ�.
	void UpdateScreen(void)
	{
		pGfxDevice->EndDraw();
		pGfxDevice->Flip();
		pGfxDevice->BeginDraw(false);
	}

	//! ����� ��ũ��Ʈ ���Ϸκ��� ���� ��ũ��Ʈ�� �����Ѵ�.
	bool LoadScript(const char* szFileName, int xStart, int yStart)
	{
		if (!s_pGameMain->LoadScript(szFileName))
			return false;

		if (xStart > 0 || yStart > 0)
		{
			s_pGameMain->party.x = xStart;
			s_pGameMain->party.y = yStart;
		}

		// ���۽� �ٶ󺸴� ������ ����
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
		//! ���� �ʱ�ȭ�ϰ� �� ũ�⸦ �����Ѵ�.
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

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void SetType(TMap::TType _type)
		{
			s_pGameMain->map.SetType(_type);
		}

		//! ���� �ʿ����� �ڵ�ĸ�� �����Ѵ�.
		void SetHandicap(int handicap)
		{
			s_pGameMain->map.SetHandicap(TMap::THandicap(handicap));
		}

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void SetEncounter(int offset, int range)
		{
			ASSERT(offset > 0 && range >= 0);
			s_pGameMain->map.encounterOffset = offset;
			s_pGameMain->map.encounterRange  = range;
		}

		//! ���� �ʿ��� ���� ������ �����Ѵ�.
		void SetStartPos(int x, int y)
		{
			s_pGameMain->party.x = x;
			s_pGameMain->party.y = y;
		}

		//! �� �����͸� �� ������ �Է��Ѵ�.
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
		//! ���� (x,y) ������ Ÿ�� ���� �����Ѵ�.
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
		//! ���� (x,y) ������ �ùٸ� �̵� ���������� �˷� �ش�.
		bool IsValidWarpPos(int x, int y)
		{
			return s_pGameMain->IsValidWarpPos(x, y);
		}
		//! ���Ϸκ��� ���� �д´�.
		bool LoadFromFile(const char* szFileName)
		{
			if (!s_pGameMain->LoadMapFromFile(szFileName))
				return false;

			return true;
		}
	}

	namespace console
	{
		//! �ܼ� â��, ����� �÷��� ����� ���� �Ķ������ ���ڿ� ������ ����Ѵ�.
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
			//@@ ���� ����
			console.Display();
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ����Ѵ�.
		void WriteLine(const char* szText, unsigned long color)
		{
			CLoreConsole& console = GetConsole();

			console.SetTextColor(color);
			console.Write(szText);
			console.Display();
			//@@ �ӵ� ���ϰ� �������� ������?
			game::UpdateScreen();
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ���� ����Ѵ�.
		void ShowMessage(unsigned long index, const char* s)
		{
			CLoreConsole& console = GetConsole();

			console.Clear();
			console.SetTextColorIndex(index);
			console.Write(s);
			console.Display();
		}

		//! �ܼ� â�� ��µǴ� �ؽ�Ʈ�� alignment �Ӽ��� �����Ѵ�.
		void SetTextAlign(TTextAlign align)
		{
			GetConsole().SetTextAlign(CLoreConsole::TTextAlign(align));
		}
	}

	namespace tile
	{
		//?? �ӽ�
		const int TILE_X_SIZE = 24;
		const int TILE_Y_SIZE = 24;
		//! ������ Ÿ���� �ӽ� Ÿ��(55��)�� �����Ѵ�.
		void CopyToDefaultTile(int ixTile)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(55*TILE_X_SIZE, yDest, pTileImage, ixTile*TILE_X_SIZE, yDest, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! ������ Ÿ���� ������ Ÿ�Ͽ� �����Ѵ�.
		void CopyTile(int srcTile, int dstTile)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(dstTile*TILE_X_SIZE, yDest, pTileImage, srcTile*TILE_X_SIZE, yDest, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! ������ ��������Ʈ�� �ӽ� Ÿ��(55��)�� �����Ѵ�.
		void CopyToDefaultSprite(int ixSprite)
		{
			int yDest = s_pGameMain->map.type * TILE_Y_SIZE;
			pTileImage->BitBlt(55*TILE_X_SIZE, yDest, pSpriteImage, ixSprite*TILE_X_SIZE, 0, TILE_X_SIZE, TILE_Y_SIZE);
		}
	}

	namespace window
	{
		//! ���� ȭ�鿡 ����Ѵ�.
		void DisplayMap(void)
		{
			s_pGameMain->window[WINDOW_MAP]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_MAP]->Display((s_pGameMain->map.type == TMap::TYPE_DEN) ? 1 : 0, s_pGameMain->party.ability.magicTorch);
		}
		//! �ܼ� â�� ȭ�鿡 ����Ѵ�.
		void DisplayConsole(void)
		{
			s_pGameMain->window[WINDOW_CONSOLE]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_CONSOLE]->Display();
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void DisplayStatus(void)
		{
			s_pGameMain->window[WINDOW_STATUS]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_STATUS]->Display();
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void DisplayBattle(int param1)
		{
			s_pGameMain->window[WINDOW_BATTLE]->SetUpdateFlag();
			s_pGameMain->window[WINDOW_BATTLE]->Display(param1);
		}
		//! �ܼ� â�� ũ�⸦ ���� �ش�.
		void GetRegionForConsole(int* pX, int* pY, int* pW, int* pH)
		{
			GetConsole().GetRegion(pX, pY, pW, pH);
			// �Ʒ�ó�� �ϸ� �ȵ�. console window ���� client������ ���� �ϱ� ������.
			// s_pGameMain->window[WINDOW_CONSOLE]->GetRegion(pX, pY, pW, pH);
		}
	}

	namespace status
	{
		//! ���� ���� �������� �ƴ����� �˷� �ش�.
		bool InBattle(void)
		{
			return (s_pGameMain->gameState == CGameMain::GAME_STATE_BATTLE);
		}
	}

	namespace object
	{
		//! �Ʊ� ����Ʈ�� vector �������� ���� �ش�.
		std::vector<TPcPlayer*>& GetPlayerList(void)
		{
			return s_pGameMain->player;
		}
		//! ���� ����Ʈ�� vector �������� ���� �ش�.
		std::vector<TPcEnemy*>&  GetEnemyList(void)
		{
			return s_pGameMain->enemy;
		}
		//! ��Ƽ�� ������ ���� �ش�.
		TPcParty& GetParty(void)
		{
			return s_pGameMain->party;
		}
		//! ���� ���� ���� ���� ������ ���� �ش�.
		TMap& GetMap(void)
		{
			return s_pGameMain->map;
		}
	}

	namespace variable
	{
		//! ����� ���� ������ �����Ѵ�.
		void Set(int index, int value)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			s_pGameMain->gameOption.variable[index] = value;
		}
		//! ����� ���� ������ ���� 1���� ��Ų��.
		void Add(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			++s_pGameMain->gameOption.variable[index];
		}
		//! ����� ���� ������ ���� ���� �ش�.
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
		//! ����� ���� �÷��׸� �����Ѵ�.
		void Set(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_pGameMain->gameOption.flag[index] = true;
		}
		//! ����� ���� �÷��׸� �����Ѵ�.
		void Reset(int index)
		{
			if ((index < 0) || (index >= s_pGameMain->gameOption.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_pGameMain->gameOption.flag[index] = false;
		}
		//! ����� ���� �÷����� ���� ���θ� ���� �ش�.
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
		//?? ���⿡ ���� ���� ��� ���� �־�� �Ѵ�.
		static CGameMain::EBattleResult s_result = CGameMain::BATTLE_RESULT_EVADE;

		//! ���� ��Ȳ�� ���������� �˷� �ش�.
		void Init(void)
		{
			s_pGameMain->enemy.clear();
		}
		//! ������ �����ͷ� ������ �Ѵ�.
		void Start(bool bAssualt)
		{
			s_result = s_pGameMain->BattleMode(bAssualt);
			s_pGameMain->ChangeWindowForField();
		}
		//! ������ �����ϴ� ���� �߰��Ѵ�. �Է� ����� ���� index ��ȣ�� �Ѱ� �ִ� ���̴�.
		void RegisterEnemy(int ixEnemy)
		{
			s_pGameMain->RegisterEnemy(ixEnemy);
		}
		//! ������ �����ϴ� ���� ȭ�鿡 ǥ���Ѵ�.
		void ShowEnemy(void)
		{
			s_pGameMain->ChangeWindowForBattle();
			game::UpdateScreen();
		}
		//! ������ ����� �����ش�.
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
		//! ���� �ǽ��� �ִ� �Ʊ��� ���� ���� �ش�.
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
		//! ���� �ǽ��� �ִ� ���� ���� ���� �ش�.
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
		//?? CSelect�� default parameter�� ���� ������ �ϴ� ���� �ʿ������� ��
		static TMenuList s_menu;
		static int s_result = 0;

		//! ���� �������� �ʱ�ȭ �Ѵ�.
		void Init(void)
		{
			s_menu.clear();
			s_result = 0;
		}
		//! ���� �������� �߰� �Ѵ�.
		void Add(const char* szString)
		{
			s_menu.push_back(szString);
		}
		//! ���� ���õ� ���������� ������ �Ѵ�.
		void Run(void)
		{
			s_result = CSelect(s_menu)();
		}
		//! ���õ� ����� ���� �´�.
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
	// �׷��� ��ü ����
	pGfxDevice = IGfxDevice::GetInstance();

	pGfxDevice->SetLayerDesc(s_layer_option);
	pGfxDevice->GetSurface(&pBackBuffer);

	// ���ҽ� �ε�
	pGfxDevice->CreateSurfaceFrom("lore_tile.tga", &pTileImage);

	pSpriteImage = pFontImage = pTileImage;

	// ���� ��ü �ʱ�ȭ
	s_pGameMain = new CGameMain;

	// �ʱ� ����Ʈ ��ũ��Ʈ �ε�
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


//?? ��Ȯ�� ��ġ��?
static bool           s_isKeyPressed = false;
static unsigned short s_pressedKey;
static unsigned long  s_repeatTime;
const  unsigned long  c_delayTime = 75;

static bool OnJoyDown(unsigned short button)
{
	// auto pressed key ����
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
	// auto pressed key ����
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
	
	// auto pressed key ����
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

//?? ����� �������� ����
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
	[4] ID    : ���� ûũ ID 4��
	[4] size  : DATA�� ũ��
	[4] CRC32 : DATA�� CRC32
	[n] DATA  : DATA
*/

const std::string GetPredictMessage(unsigned int ixPredict)
{
	const static char* PREDICT_MESSAGE[26] =
	{
		"����� � ���� ���� ������ ���� �ް� �ִ�",
		"Lord Ahn �� ����",
		"MENACE�� Ž����",
		"Lord Ahn���� �ٽ� ���ư�",
		"LASTDITCH�� ��",
		"LASTDITCH�� ���ָ� ����",
		"PYRAMID ���� Major Mummy�� ����ĥ",
		"LASTDITCH�� ���ֿ��Է� ���ư�",
		"LASTDITCH�� GROUND GATE�� ��",
		"GAIA TERRA�� ���ָ� ����",
		"EVIL SEAL���� Ȳ���� ������ �߰���",
		"GAIA TERRA�� ���ֿ��� ���ư�",
		"QUAKE���� ArchiGagoyle�� ����ĥ",
		"�ϵ����� WIVERN ������ ��",
		"WATER FIELD�� ��",
		"WATER FIELD�� ���ָ� ����",
		"NOTICE ���� Hidra�� ����ĥ",
		"LOCKUP ���� Dragon�� ����ĥ",
		"GAIA TERRA �� SWAMP GATE�� ��",
		"������ ����Ʈ�� ���� SWAMP KEEP���� ��",
		"SWAMP ����� �����ϴ� �ΰ��� ������ Ǯ",
		"SWAMP KEEP�� ��� ����Ʈ�� �۵� ��ų",
		"���� �������� EVIL CONCENTRATION���� ��",
		"������ ���� ������ ����� ��",
		"������ �������� Necromancer�� ����",
		"Necromancer�� ������ ������ ����"
	};

	if (ixPredict > 25)
		ixPredict = 0;

	if (ixPredict > 0)
	{
		std::string message("����� ");
		message += PREDICT_MESSAGE[ixPredict];
		message += " ���̴�";
		return message;
	}
	else
	{
		return std::string(PREDICT_MESSAGE[0]);
	}
}
