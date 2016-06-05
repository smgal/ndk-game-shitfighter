
#include "sf_gfx.h"

#include "util_render_text.h"
#include "util_convert_to_ucs.h"

using namespace avej_lite;

////////////////////////////////////////////////////////////////////////////////
// global variable

IGfxDevice*  g_p_gfx_device  = NULL;
IGfxSurface* g_p_back_buffer = NULL;
IGfxSurface* g_p_res_sprite  = NULL;
TRect        g_sprite_rect[TILENAME_MAX];

////////////////////////////////////////////////////////////////////////////////
// static

namespace
{
	inline avej_lite::TFixed16 _ToF16(int a)
	{
		avej_lite::TFixed16 temp(a);
		temp.m_data >>= 1;

		return temp;
	}

	inline int _ToInt(avej_lite::TFixed16 a)
	{
		return ((a.m_data) >> 15);
	}

	void RenderText(int x_dest, int y_dest, const widechar* sz_text_ucs2, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt)
	{
		static const IorzRenderText& ref_render_text = *GetTextInstance();

		ref_render_text.RenderText(x_dest, y_dest, sz_text_ucs2, color, fn_bit_blt);
	}

	void RenderText(int x_dest, int y_dest, const char* sz_text, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt)
	{
		// ����� �� ������� ����
		static widechar w_str[256];

		// �׻� UCS-2 ������ wchar_t�� ������ �Ǿ�� �Ѵ�.
		typedef int TDUMMY[(sizeof(widechar) == sizeof(wchar_t)) ? 1 : -1];

		ConvertUHC2UCS2(w_str, 256, sz_text, strlen(sz_text)+1);

		RenderText(x_dest, y_dest, w_str, color, fn_bit_blt);
	}

	inline void s_ReviseTextSourCoord(int& x_sour, int& y_sour)
	{
		x_sour += 320;

		if (x_sour >= 512)
		{
			x_sour -= 192;
			y_sour += 200;
		}
	}

	void RenderTextCallback_Normal(int x_dest, int y_dest, int width, int height, int x_sour, int y_sour, unsigned long color)
	{
		s_ReviseTextSourCoord(x_sour, y_sour);

		unsigned long color_list[4] = { color, color, color, color };

		g_p_back_buffer->BitBlt(x_dest, y_dest, g_p_res_sprite, x_sour, y_sour, width, height, color_list);
	}

	void RenderTextCallback_Fade(int x_dest, int y_dest, int width, int height, int x_sour, int y_sour, unsigned long color)
	{
		s_ReviseTextSourCoord(x_sour, y_sour);

		unsigned long fade_color    = ((color >> 1) & 0xFF000000) | (color & 0x00FFFFFF);
		unsigned long color_list[4] = { color, color, fade_color, fade_color };

		g_p_back_buffer->BitBlt(x_dest, y_dest, g_p_res_sprite, x_sour, y_sour, width, height, color_list);
	}

	void RenderTextCallback_x2(int x_dest, int y_dest, int width, int height, int x_sour, int y_sour, unsigned long color)
	{
		s_ReviseTextSourCoord(x_sour, y_sour);

		unsigned long color_list[4] = { color, color, color, color };

		g_p_back_buffer->StretchBlt(x_dest*2, y_dest*2, width*2, height*2, g_p_res_sprite, x_sour, y_sour, width, height, color_list);
	}

}

////////////////////////////////////////////////////////////////////////////////
// public (TIndexColor);

static unsigned long s_ConvertToRealColor(byte index)
{
	static const unsigned long s_palette_data[16] =
	{
		0xFF000000, 0xFF342486, 0xFF6D75E3, 0xFF921C30, 0xFFE72C38, 0xFF3C3C2C, 0xFF69614D, 0xFF928A7D,
		0xFFCBBA41, 0xFF415534, 0xFF4DA261, 0xFFFF00FF, 0xFF343C59, 0xFF657186, 0xFFCBCFE7, 0xFFFFF3D7
	};

	return (index < 16) ? s_palette_data[index] : 0x00000000;
}

TIndexColor::TIndexColor()
: index(0), argb(0)
{
}

TIndexColor::TIndexColor(unsigned char _index)
: index(_index)
{
	argb = s_ConvertToRealColor(index);
}

unsigned long TIndexColor::GetARGB(void)
{
	return argb;
}

////////////////////////////////////////////////////////////////////////////////
// public (gfx)

void gfx::DrawImage(int x, int y, TImage* image)
{
	const TRect& sprite = *((TRect*)image);

	gfx::BitBlt(x, y, g_p_res_sprite, sprite.x, sprite.y, sprite.w, sprite.h);
}

void gfx::DrawSprite(int x, int y, TImage* image, bool flag)
{
	const TRect& sprite = *((TRect*)image);

#if 1
	gfx::BitBlt(x, y, g_p_res_sprite, sprite.x, sprite.y, sprite.w, sprite.h);
#else
	if (flag)
		gfx::BitBlt(x, y, g_p_res_sprite, sprite.x, sprite.y, sprite.w, sprite.h);
	else
		gfx::PatBlt(x, y, 0xFFFFFFFF, g_p_res_sprite, sprite.x, sprite.y, sprite.w, sprite.h);
#endif
}

void gfx::DrawText(int x, int y, const char* s, unsigned long real_color, bool use_fading)
{
	if (use_fading)
		RenderText(x/2, y/2, s, real_color, RenderTextCallback_Fade);
	else
		RenderText(x/2, y/2, s, real_color, RenderTextCallback_Normal);
}

void gfx::FillRect(unsigned long real_color, int x, int y, int width, int height)
{
	g_p_back_buffer->FillRect(real_color, _ToF16(x), _ToF16(y), _ToF16(width), _ToF16(height));
}

void gfx::BitBlt(int x_dest, int y_dest, IGfxSurface* p_surface, int x_sour, int y_sour, int w_sour, int h_sour)
{
	g_p_back_buffer->BitBlt(_ToF16(x_dest), _ToF16(y_dest), p_surface, _ToF16(x_sour), _ToF16(y_sour), _ToF16(w_sour), _ToF16(h_sour));
}

////////////////////////////////////////////////////////////////////////////////
// namespace gfx_ix

void gfx_ix::DrawText(int x, int y, const char* s, TIndexColor color)
{
	RenderText(x/2, y/2, s, color.GetARGB(), RenderTextCallback_Normal);
}

void gfx_ix::DrawText_x2(int x, int y, const char* s, TIndexColor color)
{
	RenderText(x/2, y/2, s, color.GetARGB(), RenderTextCallback_x2);
}

void gfx_ix::FillRect(int x1, int y1, int x2, int y2, TIndexColor back_color)
{
	g_p_back_buffer->FillRect(back_color.GetARGB(), _ToF16(x1), _ToF16(y1), _ToF16(x2-x1+1), _ToF16(y2-y1+1));
}

void gfx_ix::DrawLineX(int x1, int x2, int y, TIndexColor color)
{
	g_p_back_buffer->FillRect(color.GetARGB(), _ToF16(x1), _ToF16(y), _ToF16(x2-x1+1), _ToF16(1));
}

void gfx_ix::DrawLineY(int x, int y1, int y2, TIndexColor color)
{
	g_p_back_buffer->FillRect(color.GetARGB(), _ToF16(x), _ToF16(y1), _ToF16(1), _ToF16(y2-y1+1));
}

////////////////////////////////////////////////////////////////////////////////
// gui::CMessageBox

#define IF_INSTANCE_IS_AVAILABLE     if (m_p_impl)
#define IF_INSTANCE_IS_NOT_AVAILABLE if (m_p_impl == 0)

struct gui::CMessageBox::TImpl
{
	enum
	{
		MAX_MESSAGE = 20
	};

	int          x1, y1, x2, y2;
	unsigned int max_line;
	TIndexColor  fore_color;
	TIndexColor  back_color;
	TIndexColor  hide_color;
	char         message[MAX_MESSAGE][256];
	bool         enable_message[MAX_MESSAGE];

	static void DrawTextInternal(int x, int y, const char* s, TIndexColor fore_color, TIndexColor back_color)
	{
		gfx_ix::FillRect(x*GLYPH_W, y*GLYPH_H, (x+strlen(s))*GLYPH_W-1, y*GLYPH_H+(GLYPH_H-1), back_color);
		gfx_ix::DrawText(x*GLYPH_W, y*GLYPH_H, s, fore_color);
	}
};

gui::CMessageBox::CMessageBox(int x1, int y1, int x2, int y2, TIndexColor fore_color, TIndexColor back_color, TIndexColor hide_color, byte max_line)
: m_p_impl(new TImpl)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		m_p_impl->x1 = x1;
		m_p_impl->x2 = x2;
		m_p_impl->y1 = y1;
		m_p_impl->y2 = y2;

		m_p_impl->fore_color = fore_color;
		m_p_impl->back_color = back_color;
		m_p_impl->hide_color = hide_color;
		m_p_impl->max_line   = max_line;

		for (byte i = 0; i < max_line; i++)
		{
			m_p_impl->message[i][0]     = 0;
			m_p_impl->enable_message[i] = true;
		}
	}
}

gui::CMessageBox::~CMessageBox()
{
	delete m_p_impl;
}

void gui::CMessageBox::Enable(unsigned int line)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		if (line >= 0 && line < TImpl::MAX_MESSAGE)
		{
			m_p_impl->enable_message[line] = true;
		}
	}
}

void gui::CMessageBox::Disable(unsigned int line)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		if (line >= 0 && line < TImpl::MAX_MESSAGE)
		{
			m_p_impl->enable_message[line] = false;
		}
	}
}

void gui::CMessageBox::SetMessage(unsigned int line, const char* s)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		if (line >= 0 && line < TImpl::MAX_MESSAGE)
		{
			strcpy(m_p_impl->message[line], s);
		}
	}
}

int gui::CMessageBox::SetSelectedLine(unsigned int line)
{
	IF_INSTANCE_IS_NOT_AVAILABLE
		return 0;

	CInputDevice& input_device = avej_lite::singleton<CInputDevice>::get();

	while (!(input_device.WasKeyPressed(avej_lite::INPUT_KEY_SYS1) || input_device.WasKeyPressed(avej_lite::INPUT_KEY_A)))
	{
		m_p_impl->DrawTextInternal(m_p_impl->x1, m_p_impl->y1+line, m_p_impl->message[line], m_p_impl->fore_color, m_p_impl->back_color);

		if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_UP))
		{
			if (line > 1)
			{
				do
				{
					--line;
				} while (!(((m_p_impl->message[line][0] != 0) && m_p_impl->enable_message[line]) || (line == 1)));

				if (line == 1)
				{
					while (((m_p_impl->message[line][0] == 0) || !m_p_impl->enable_message[line]))
						++line;
				}
			}
		}

		if (input_device.WasKeyPressed(avej_lite::INPUT_KEY_DOWN))
		{
			if (line < m_p_impl->max_line)
			{
				do
				{
					++line;
				} while (!(((m_p_impl->message[line][0] != 0) && m_p_impl->enable_message[line]) || (line == m_p_impl->max_line)));

				if (line == m_p_impl->max_line)
				{
					while (((m_p_impl->message[line][0] == 0) || !m_p_impl->enable_message[line]))
						--line;
				}
			}
		}

		m_p_impl->DrawTextInternal(m_p_impl->x1, m_p_impl->y1+line, m_p_impl->message[line], TIndexColor(15), TIndexColor(10));
	}

	return (input_device.WasKeyPressed(avej_lite::INPUT_KEY_A)) ? line : 0;
}

void gui::CMessageBox::Show(void)
{
	IF_INSTANCE_IS_NOT_AVAILABLE
		return;

	int x1 = m_p_impl->x1;
	int x2 = m_p_impl->x2;
	int y1 = m_p_impl->y1;
	int y2 = m_p_impl->y2;

	{
		TIndexColor line_color(15);

		gfx_ix::FillRect(x1*GLYPH_W,y1*GLYPH_H,x2*GLYPH_W+(GLYPH_W-1),y2*GLYPH_H+(GLYPH_H-1),m_p_impl->back_color);
		gfx_ix::DrawLineX(x1*GLYPH_W,x2*GLYPH_W+(GLYPH_W-1),y1*GLYPH_H,line_color);
		gfx_ix::DrawLineX(x1*GLYPH_W,x2*GLYPH_W+(GLYPH_W-1),y1*GLYPH_H+1,line_color);
		gfx_ix::DrawLineX(x1*GLYPH_W,x2*GLYPH_W+(GLYPH_W-1),y2*GLYPH_H+(GLYPH_H-1),line_color);
		gfx_ix::DrawLineX(x1*GLYPH_W,x2*GLYPH_W+(GLYPH_W-1),y2*GLYPH_H+(GLYPH_H-2),line_color);
		gfx_ix::DrawLineY(x1*GLYPH_W,y1*GLYPH_H,y2*GLYPH_H+(GLYPH_H-1),line_color);
		gfx_ix::DrawLineY(x1*GLYPH_W+1,y1*GLYPH_H,y2*GLYPH_H+(GLYPH_H-1),line_color);
		gfx_ix::DrawLineY(x2*GLYPH_W+(GLYPH_W-1),y1*GLYPH_H,y2*GLYPH_H+(GLYPH_H-1),line_color);
		gfx_ix::DrawLineY(x2*GLYPH_W+(GLYPH_W-2),y1*GLYPH_H,y2*GLYPH_H+(GLYPH_H-1),line_color);
	}

	for (int i = 0; i < int(m_p_impl->max_line); i++)
	{
		if (m_p_impl->message[i][0] != 0)
		{
			if (m_p_impl->enable_message[i])
			{
				m_p_impl->DrawTextInternal(m_p_impl->x1+3,(m_p_impl->y1+i+1),m_p_impl->message[i],m_p_impl->fore_color,m_p_impl->back_color);
			}
			else
			{
				m_p_impl->DrawTextInternal(m_p_impl->x1+3,(m_p_impl->y1+i+1),m_p_impl->message[i],m_p_impl->hide_color,m_p_impl->back_color);
			}
		}
		else
		{
			//?? �̰� ����? �ϴ� �ּ� ó��
			// gfx_ix::DrawLineX(x1*GLYPH_W, x2*GLYPH_W+(GLYPH_W-1), (y1+i)*GLYPH_H+(GLYPH_W-1), (GLYPH_H-1));
			// gfx_ix::DrawLineX(x1*GLYPH_W, x2*GLYPH_W+(GLYPH_W-1), (y1+i)*GLYPH_H+GLYPH_W,     (GLYPH_H-1));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// gui::CKeyGuide

struct gui::CKeyGuide::TImpl
{
	enum
	{
		MAX_TEXT_LEN = 256
	};

	long          x;
	long          y;
	unsigned long width;
	unsigned long height;
	char          sz_text[2][MAX_TEXT_LEN];
};

gui::CKeyGuide::CKeyGuide()
:	m_p_impl(0)
{
}

gui::CKeyGuide::CKeyGuide(long x, long y, unsigned long width, unsigned long height)
:	m_p_impl(new TImpl)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		m_p_impl->x      = x;
		m_p_impl->y      = y;
		m_p_impl->width  = width;
		m_p_impl->height = height;

		m_p_impl->sz_text[0][0] = 0;
		m_p_impl->sz_text[1][0] = 0;

		m_p_impl->sz_text[0][TImpl::MAX_TEXT_LEN-1] = 0;
		m_p_impl->sz_text[1][TImpl::MAX_TEXT_LEN-1] = 0;
	}
}

gui::CKeyGuide::~CKeyGuide()
{
	delete m_p_impl;
}

void gui::CKeyGuide::SetLeftText(const char* sz_text)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		strncpy(m_p_impl->sz_text[0], sz_text, TImpl::MAX_TEXT_LEN-1);
	}
}

void gui::CKeyGuide::SetRightText(const char* sz_text)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		strncpy(m_p_impl->sz_text[1], sz_text, TImpl::MAX_TEXT_LEN-1);
	}
}

void gui::CKeyGuide::Show(void)
{
	IF_INSTANCE_IS_AVAILABLE
	{
		gfx::FillRect(0xC0204080, m_p_impl->x, m_p_impl->y, m_p_impl->width, m_p_impl->height);

		int x = m_p_impl->x;
		int y = m_p_impl->y + (m_p_impl->height - GLYPH_H) / 2;

		gfx::DrawText(x, y, m_p_impl->sz_text[0], 0xFFFFFFFF, true);

		x = m_p_impl->width - strlen(m_p_impl->sz_text[1]) * GLYPH_W;
		gfx::DrawText(x, y, m_p_impl->sz_text[1], 0xFFFFFFFF, true);
	}
}

////////////////////////////////////////////////////////////////////////////////
// gui::CFloatMessage

static struct TMessage_TImpl
{
	enum
	{
		MAX_MESSAGE = 10
	};

	struct TDesc
	{
		int         x, y;
		int         count;
		TIndexColor color;
		char        string[64];
	} m_message[MAX_MESSAGE];

	TMessage_TImpl()
	{
		for (int i = 0; i < MAX_MESSAGE; i++)
			m_message[i].string[0] = 0;
	}
} s_impl;

gui::CFloatMessage::CFloatMessage()
{
}

gui::CFloatMessage::~CFloatMessage()
{
}

void gui::CFloatMessage::Register(int x, int y, TIndexColor color, const char* sz_message)
{
	for (int i = 0; i < TMessage_TImpl::MAX_MESSAGE; i++)
	{
		if (s_impl.m_message[i].string[0] == 0)
		{
			s_impl.m_message[i].x     = x;
			s_impl.m_message[i].y     = y;
			s_impl.m_message[i].color = color;
			s_impl.m_message[i].count = 0;
			strcpy(s_impl.m_message[i].string, sz_message);
			return;
		}
	}
}

void gui::CFloatMessage::Show(void)
{
	for (int i = 0; i < TMessage_TImpl::MAX_MESSAGE; i++)
	{
		if (s_impl.m_message[i].string[0] != 0)
		{
			++s_impl.m_message[i].count;

			if ((s_impl.m_message[i].count > 40) || (s_impl.m_message[i].x < VIEWPORT_X1) || (s_impl.m_message[i].y < VIEWPORT_Y1) || (s_impl.m_message[i].x + int(strlen(s_impl.m_message[i].string)*6) > VIEWPORT_X2))
				s_impl.m_message[i].string[0] = 0;
			else
				gfx_ix::DrawText(s_impl.m_message[i].x, s_impl.m_message[i].y - s_impl.m_message[i].count / 2, s_impl.m_message[i].string, s_impl.m_message[i].color);
		}
	}
}
