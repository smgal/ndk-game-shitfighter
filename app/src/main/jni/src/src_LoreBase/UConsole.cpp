
#include <algorithm>
#include <assert.h>

#include "UConsole.h"
#include "UExtern.h"
#include "UMain.h"

//#include "util_convert_to_ucs.h"
//#include "util_render_text.h"

////////////////////////////////////////////////////////////////////////////////
//

static std::vector<TBltParam> s_blit_list;

static void m_RenderText(int xDest, int yDest, int width, int height, int xSour, int ySour, unsigned long color)
{
	s_blit_list.push_back(TBltParam(xDest, yDest, width, height, xSour, ySour, color));
}

static void s_RenderText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long default_color)
{
	s_blit_list.clear();

	g_DrawFormatedText(x, y, sz_text_begin, sz_text_end, default_color, m_RenderText);

	s_blit_list.push_back(TBltParam());
}		

////////////////////////////////////////////////////////////////////////////////
//

CLoreConsole::CLoreConsole(void)
	: m_isModified(true), m_textColor(0xFFFFFFFF), m_bgColor(0xFF000000), m_align(ALIGN_LEFT),
	  m_xOffset(260), m_yOffset(30), m_width(360), m_height(192)
{
	// CLoreConsole은 pBackBuffer에 디펜던시를 가진다.
	assert(pBackBuffer != 0);

	m_bgColor = pBackBuffer->Color(0xFF, 0x40, 0x40, 0x40);
}

CLoreConsole::~CLoreConsole(void)
{
	std::for_each(m_line.begin(), m_line.end(), FnctDeleteArray<TBltParam*>());
}

bool CLoreConsole::IsModified(void)
{
	return m_isModified;
}

static int m_yLine  = 0;
static int m_ixLine = 0;

void CLoreConsole::Clear(void)
{
	std::for_each(m_line.begin(), m_line.end(), FnctDeleteArray<TBltParam*>());

	m_line.clear();

	m_yLine  = 0;
	m_ixLine = 0;

	pBackBuffer->FillRect(m_bgColor, m_xOffset, m_yOffset, m_width, m_height);

	m_isModified = true;
}

void CLoreConsole::SetBgColor(unsigned long color)
{
	m_bgColor = color;
}

unsigned long CLoreConsole::GetBgColor(void)
{
	return m_bgColor;
}

void CLoreConsole::SetTextColor(unsigned long color)
{
	m_textColor = color;
}

void CLoreConsole::SetTextColorIndex(unsigned long index)
{
	m_textColor = g_GetColorFromIndexedColor(index, 0xFFFFFFFF);
}

void CLoreConsole::SetTextAlign(TTextAlign align)
{
	m_align = align;
}

void CLoreConsole::Write(const std::string text)
{
	this->Write(text.c_str());
}

void CLoreConsole::Write(const char* szText)
{
	if (szText == NULL)
		return;

	if (*szText == 0)
	{
		m_line.push_back(NULL);
		return;
	}

	unsigned long realColor = pBackBuffer->Color(m_textColor);

	const char* szTemp = szText;

	while (*szText)
	{
		int xCurrPos = 0;
		int nChar = 0;

		while (*szTemp)
		{
			if (*szTemp == '@')
			{
				if (*(++szTemp))
					++szTemp;
			}
			else
			{
				int add = (*szTemp & 0x80) ? config::c_wFont*2 : config::c_wFont;
				if ((xCurrPos+add) > m_width)
					break;
				xCurrPos += add;
				szTemp += (add / config::c_wFont);
				++nChar;
			}
		}

		// 마지막 공백 제거
		{
			const char* szBack = szTemp;
			while ((szBack > szText) && (*(szBack-1) == ' '))
			{
				--szBack;
				--nChar;
			}
		}

		if (nChar <= 0)
			break;

		s_RenderText(0, 0, szText, szTemp, realColor);

/*
		// align
		if (nChar > 1)
		{
			if (*szTemp)
			{
				// 문자열을 좌우 정렬
				int padding = m_width - (s_blit_list[nChar-1].dst_bound.x1 + (s_blit_list[nChar-1].src_bound.x2 - s_blit_list[nChar-1].src_bound.x1));
				if (padding > 0)
				{
					for (int i = 1; i < nChar-1; i++)
						s_blit_list[i].dst_bound.x1 += (padding * i / nChar);

					s_blit_list[nChar-1].dst_bound.x1 += padding;
				}
			}
			else
			{
				// 문자열의 마지막 행이기 때문에 지정된 align 적용, 아무 것도 하지 않으면 ALIGN_LEFT
				switch (m_align)
				{
				case ALIGN_CENTER:
					{
						int xOffset = (m_width - xCurrPos) / 2;
						for (int i = 0; i < nChar; i++)
							s_blit_list[i].dst_bound.x1 += xOffset;
					}
					break;
				case ALIGN_RIGHT:
					{
						int xOffset = (m_width - xCurrPos);
						for (int i = 0; i < nChar; i++)
							s_blit_list[i].dst_bound.x1 += xOffset;
					}
					break;
				default:
					break;
				}
			}
		}
*/
int qaz = s_blit_list.size();
		TBltParam* pBltList = new TBltParam[s_blit_list.size()];
		//?? 위험
		memcpy(pBltList, &s_blit_list[0], sizeof(TBltParam) * s_blit_list.size());

		m_line.push_back(pBltList);

		while ((*szTemp) && (*szTemp == ' '))
			++szTemp;

		szText = szTemp;
	}
	m_isModified = true;
}

void CLoreConsole::Display(void)
{
	if ((m_ixLine < 0) || (m_ixLine >= m_line.size()))
	{
		game::UpdateScreen();
		return;
	}

	while (m_ixLine < m_line.size())
	{
		TBltParam* pLine = m_line[m_ixLine];

		if (pLine)
			g_DrawBltList(m_xOffset, m_yOffset+m_yLine, pLine);

		m_yLine += config::c_hFont;

		if (m_yLine >= m_height)
		{
			game::UpdateScreen();
			game::WaitForAnyKey();

#if 0
			//?? 문제 있음
			pBackBuffer->BitBlt(m_xOffset, m_yOffset, pBackBuffer, m_xOffset, m_yOffset+config::c_hFont, m_width, m_height-config::c_hFont);
			pBackBuffer->FillRect(m_bgColor, m_xOffset, m_yOffset+m_height-config::c_hFont, m_width, config::c_hFont);
			m_yLine -= config::c_hFont;
#else
			pBackBuffer->FillRect(m_bgColor, m_xOffset, m_yOffset, m_width, m_height);
			m_ixLine -= (m_height/12-1);
			m_yLine   = 0;
#endif
		}
		++m_ixLine;
	}
	game::UpdateScreen();
}

bool CLoreConsole::SetRegion(int x, int y, int w, int h)
{
	if ((x < 0) || (y < 0))
		return false;

	m_xOffset = x;
	m_yOffset = y;
	m_width   = w;
	m_height  = h;

	return true;
}

void CLoreConsole::GetRegion(int* pX, int* pY, int* pW, int* pH) const
{
	if (pX)
		*pX = m_xOffset;
	if (pY)
		*pY = m_yOffset;
	if (pW)
		*pW = m_width;
	if (pH)
		*pH = m_height;
}

#include "USmSola.h"

CLoreConsole& GetConsole(void)
{
	return sola::singleton<CLoreConsole>::Instance();
}
