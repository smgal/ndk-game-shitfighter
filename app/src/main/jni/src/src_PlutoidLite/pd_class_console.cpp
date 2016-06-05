
#include <algorithm>
#include <assert.h>

#include "pd_config.h"
#include "pd_class_console.h"
#include "pd_util.h"
#include "util_render_text.h"
#include "util_convert_to_ucs.h"

static int X_OFFSET       = 0;
static int Y_OFFSET       =  30;
static int CONSOLE_WIDTH  = 160;
static int CONSOLE_HEIGHT = 192;

enum
{
	TEXT_WIDTH     =   6,
	TEXT_HEIGHT    =  12,
};

#define GET_TARGET_COLOR(color32bits)  avej_lite::TPixelInfo<FONT_TEXTURE_FORMAT>::ConvertColor(color32bits)
typedef avej_lite::TPixelInfo<FONT_TEXTURE_FORMAT>::TPixel  TTargetPixel;

/*
void g_DrawBltList(int x, int y, CConsole::TBltParam rect[], int nRect)
{
	s_DrawBltList<avej::pixel>(x, y, rect, nRect);
}
*/

CConsole::CConsole(void)
	: m_isModified(true), m_color(0xFFFFFFFF), m_align(ALIGN_LEFT)
{
}

CConsole::~CConsole(void)
{
	std::for_each(m_line.begin(), m_line.end(), FnctDeleteArray<IorzRenderText::TBltParam*>());
}

enum { MAX_COLOR_TABLE = 16 };

const unsigned long COLOR_TABLE[MAX_COLOR_TABLE] =
{
	0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
	0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
	0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
	0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
};

unsigned long CConsole::m_GetLocalTextColorFromIndex(unsigned long index, unsigned long defaultColor)
{
	static bool s_isFirst = true;
	static unsigned long s_localTable[MAX_COLOR_TABLE];

	if (s_isFirst)
	{
		for (unsigned int index = 0; index < sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]); index++)
			s_localTable[index] = GET_TARGET_COLOR(COLOR_TABLE[index]);

		s_isFirst = false;
	}

	return (index < sizeof(s_localTable) / sizeof(s_localTable[0])) ? s_localTable[index] : defaultColor;
}

bool CConsole::IsModified(void)
{
	return m_isModified;
}

static int m_yLine  = 0;
static int m_ixLine = 0;

#define GET_BG_COLOR  GET_TARGET_COLOR(0xFF404040)

void CConsole::Clear(void)
{
	std::for_each(m_line.begin(), m_line.end(), FnctDeleteArray<IorzRenderText::TBltParam*>());
	m_line.clear();

	m_yLine  = 0;
	m_ixLine = 0;
/*
	unsigned long bgColor = GET_BG_COLOR;
	pBackBuffer->FillRect(bgColor, X_OFFSET, Y_OFFSET, CONSOLE_WIDTH, CONSOLE_HEIGHT);
*/
	m_isModified = true;
}

void CConsole::SetTextColor(unsigned long color)
{
	m_color = color;
}

void CConsole::SetTextColorIndex(unsigned long index)
{
	if (index < sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]))
		m_color = COLOR_TABLE[index];
}

void CConsole::SetTextAlign(TTextAlign align)
{
	m_align = align;
}

void CConsole::Write(const std::string text)
{
	this->Write(text.c_str());
}

void CConsole::Write(const char* sz_text)
{
	if (sz_text == NULL)
		return;

	if (*sz_text == 0)
	{
		m_line.push_back(NULL);
		return;
	}

	TTargetPixel realColor = GET_TARGET_COLOR(m_color);

	const char* sz_temp = sz_text;

	const IorzRenderText& s_font = *GetTextInstance();

	while (*sz_text)
	{
		int xCurrPos = 0;
		int nChar = 0;

		while (*sz_temp)
		{
			if (*sz_temp == '@')
			{
				if (*(++sz_temp))
					++sz_temp;
			}
			else
			{
				int add = (*sz_temp & 0x80) ? TEXT_WIDTH*2 : TEXT_WIDTH;
				if ((xCurrPos+add) > CONSOLE_WIDTH)
					break;
				xCurrPos += add;
				sz_temp += (add / TEXT_WIDTH);
				++nChar;
			}
		}

		// 마지막 공백 제거
		{
			const char* szBack = sz_temp;
			while ((szBack > sz_text) && (*(szBack-1) == ' '))
			{
				--szBack;
				--nChar;
			}
		}

		if (nChar <= 0)
			break;

		IorzRenderText::TBltParam rect[256];
		assert(nChar < 255);

		{
			widechar w_str[256];
			ConvertUHC2UCS2(w_str, 256, sz_text, strlen(sz_text)+1);

			int num_recoded = s_font.RenderText(0, 0, w_str, realColor, rect, nChar);

			// nChar를 정확하게 예상해서 넣는 것이므로 아래의 식이 성립해야 한다.
			assert(num_recoded == nChar);

			rect[num_recoded].is_available = false;
		}

		{
			int index;

			for (int i = 0; i < nChar; i++)
			{
				if ((index = rect[i].index_color) >= 0)
					rect[i].color = m_GetLocalTextColorFromIndex(index, realColor);
			}
		}

		// align
		if (nChar > 1)
		{
/*
			if (*sz_temp)
			{
				// 문자열을 좌우 정렬
				int padding = CONSOLE_WIDTH - (rect[nChar-1].x_dest + (rect[nChar-1].x2 - rect[nChar-1].x1));
				if (padding > 0)
				{
					for (int i = 1; i < nChar-1; i++)
						rect[i].x_dest += (padding * i / nChar);

					rect[nChar-1].x_dest += padding;
				}
			}
			else
*/
			{
				// 문자열의 마지막 행이기 때문에 지정된 align 적용, 아무 것도 하지 않으면 ALIGN_LEFT
				switch (m_align)
				{
				case ALIGN_CENTER:
					{
						int xOffset = (CONSOLE_WIDTH - xCurrPos) / 2;
						for (int i = 0; i < nChar; i++)
							rect[i].x_dest += xOffset;
					}
					break;
				case ALIGN_RIGHT:
					{
						int xOffset = (CONSOLE_WIDTH - xCurrPos);
						for (int i = 0; i < nChar; i++)
							rect[i].x_dest += xOffset;
					}
					break;
				default:
					break;
				}
			}
		}

		IorzRenderText::TBltParam* pBltList = new IorzRenderText::TBltParam[nChar+1];
		memcpy(pBltList, rect, sizeof(IorzRenderText::TBltParam) * (nChar+1));

		m_line.push_back(pBltList);

		while ((*sz_temp) && (*sz_temp == ' '))
			++sz_temp;

		sz_text = sz_temp;
	}
	m_isModified = true;
}

void CConsole::Display(IorzRenderText::TFnBitBlt bitblt_callback)
{
/*
	if ((m_ixLine < 0) || (m_ixLine >= m_line.size()))
		return;
*/
	IorzRenderText::TBltParam* pLine = m_line[0];

	if (pLine)
	{
		while (pLine->is_available)
		{
			bitblt_callback(X_OFFSET + pLine->x_dest, Y_OFFSET + m_yLine + pLine->y_dest, pLine->x2-pLine->x1, pLine->y2-pLine->y1, pLine->x1, pLine->y1, pLine->color);
			++pLine;
		}
	}
}

/*
void CConsole::Display(void)
{
	if ((m_ixLine < 0) || (m_ixLine >= m_line.size()))
		return;

	unsigned long bgColor = GET_BG_COLOR;

	while (m_ixLine < m_line.size())
	{
		IorzRenderText::TBltParam* pLine = m_line[m_ixLine];

		if (pLine)
			g_DrawBltList(X_OFFSET, Y_OFFSET+m_yLine, pLine);

		game::UpdateScreen();

		m_yLine += TEXT_HEIGHT;

		if (m_yLine >= CONSOLE_HEIGHT)
		{
			game::UpdateScreen();
			game::WaitForAnyKey();

			pBackBuffer->BitBlt(X_OFFSET, Y_OFFSET, pBackBuffer, X_OFFSET, Y_OFFSET+TEXT_HEIGHT, CONSOLE_WIDTH, CONSOLE_HEIGHT-TEXT_HEIGHT);
			pBackBuffer->FillRect(bgColor, X_OFFSET, Y_OFFSET+CONSOLE_HEIGHT-TEXT_HEIGHT, CONSOLE_WIDTH, TEXT_HEIGHT);
			m_yLine -= TEXT_HEIGHT;
		}
		++m_ixLine;
	}
}
*/
bool CConsole::SetRegion(int x, int y, int w, int h)
{
	if ((x < 0) || (y < 0))
		return false;

	X_OFFSET       = x;
	Y_OFFSET       = y;
	CONSOLE_WIDTH  = w;
	CONSOLE_HEIGHT = h;

	return true;
}
