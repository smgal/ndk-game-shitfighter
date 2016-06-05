
#include "_USmFont12x12.h"

CSmFont12x12::CSmFont12x12(void)
	: m_indexColor(INVALID_INDEX)
{
}

CSmFont12x12::~CSmFont12x12(void)
{
}

bool CSmFont12x12::RenderText(int xDest, int yDest, const MCHAR* szText, unsigned long defaultColor, TBltParam rect[], int nRect)
{
	enum
	{
		ASCII_LEFT    = 6,
		ASCII_TOP     = 20,
		ASCII_WIDTH   = 6,
		ASCII_HEIGHT  = 12,

		HANGUL_LEFT   = 0,
		HANGUL_TOP    = 32,
		HANGUL_WIDTH  = 12,
		HANGUL_HEIGHT = 12,
		HANGUL_PITCH  = int(1024 / HANGUL_WIDTH),
	};

	const TBltParam blankParam =
	{
		true, 0, 0,
		0, ASCII_TOP, HANGUL_WIDTH, HANGUL_HEIGHT,
		0, INVALID_INDEX
	};

	int code;
	int index = 0;

	unsigned long color = defaultColor;

	while ((code = (unsigned char)(*szText++)))
	{
		// 컬러는 바꾸는 명령일 경우 컬러를 바꾼다
		if (code == '@')
		{
			code = (unsigned char)(*szText);
			if (code)
			{
				if (code != '@')
				{
					int index;
					if (code >= '0' && code <= '9')
						index = code - '0';
					else if (code >= 'A' && code <= 'Z')
						index = code - 'A' + 10;
					else if (code >= 'a' && code <= 'z')
						index = code - 'a' + 10;
					else
						index = -1;

					if (index >= 0)
						m_indexColor = index;
				}
				else
				{
					m_indexColor = INVALID_INDEX;
				}
				++szText;
			}
			continue;
		}
		// 한글 코드일 때는 글자를 확장한다.
		if (code & 0x80)
		{
			if (*szText == 0)
				break;

			code <<= 8;
			code  |= (unsigned char)(*szText++);
		}

		if (--nRect < 0)
			break;

		if (code <= 0x00FF)
		{
			if (code < 0x20 || code > 0x7F)
				code = 0x20;
			code -= 0x20;

			rect[index].isAvailable = true;
			rect[index].xDest = xDest;
			rect[index].yDest = yDest;
			rect[index].x1    = code * ASCII_WIDTH + ASCII_LEFT;
			rect[index].y1    = ASCII_TOP;
			rect[index].x2    = rect[index].x1 + ASCII_WIDTH;
			rect[index].y2    = rect[index].y1 + ASCII_HEIGHT;

			xDest += ASCII_WIDTH;
		}
		else
		{
			int ixGlyph = -1;

			if ((code >= 0xA4A1) && (code <= 0xA4D3))
			{
				ixGlyph = code - 0xA4A1;
			}
			else if ((code >= 0xB0A1) && (code <= 0xC8FE))
			{
				int sm1, sm2;
				sm1 = (code >> 8 ) & 0xFF;
				sm2 = (code & 0xFF );

				sm1 = sm1 - 0xB0;
				sm2 = sm2 - 0xA1;

				if ((sm2 >= 0) && (sm2 < 94))
					ixGlyph = sm1 * 94 + sm2 + 51;
			}

			if (ixGlyph >= 0)
			{
				rect[index].isAvailable = true;
				rect[index].xDest = xDest;
				rect[index].yDest = yDest;
				rect[index].x1    = (ixGlyph % HANGUL_PITCH) * HANGUL_WIDTH + HANGUL_LEFT;
				rect[index].y1    = (ixGlyph / HANGUL_PITCH) * HANGUL_HEIGHT + HANGUL_TOP;
				rect[index].x2    = rect[index].x1 + HANGUL_WIDTH;
				rect[index].y2    = rect[index].y1 + HANGUL_HEIGHT;
			}
			else
			{
				rect[index]       = blankParam;
				rect[index].xDest = xDest;
				rect[index].yDest = yDest;
			}

			xDest += HANGUL_WIDTH;
		}

		rect[index].color = color;
		rect[index].indexColor = m_indexColor;

		++index;
	}

	if (nRect >= 0)
		rect[index].isAvailable = false;

	return true;
}
