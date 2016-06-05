
////////////////////////////////////////////////////////////////////////////////
// uses

#include "pd_base_presentation_base.h"

using namespace avej;
using namespace avej::gfx3d;

static void s_Int2Str(char **ppBuffer, int value)
{
    if (value == 0)
    {
        *((*ppBuffer)++) = '0';
    }
    else
    {
        if (value < 0)
        {
            *((*ppBuffer)++) = '-';
            value = -value;
        }

        int nNum = 0;
        int temp = value;
        while (temp)
        {
            temp = temp / 10;
            ++nNum;
        }

        int nLoop = nNum;
        while (nLoop-- > 0)
        {
            (*ppBuffer)[nLoop] = (value % 10) + '0';
            value = value / 10;
        }
        *ppBuffer += nNum;
    }
}

void s_ComposeString(char pBuffer[], const char* szFormat, const int data...)
{
    if (pBuffer == 0)
        return;

    const int* pData = &data;

    while (*szFormat)
    {
        if (*szFormat != '@')
        {
            *pBuffer++ = *szFormat++;
            continue;
        }
        ++szFormat;
        s_Int2Str(&pBuffer, *pData++);
    }

    *pBuffer = 0;
}

////////////////////////////////////////////////////////////////////////////////
// implementation (CVisibleCube)

CVisibleCube::CVisibleCubeSub::CVisibleCubeSub(TShared3DObject renderable_object, FnShow fn_show)
:	CVisible<TShared3DObject>(renderable_object, fn_show)
{
}

void CVisibleCube::CVisibleCubeSub::_Show(TShared3DObject renderable_object) const
{
	static float s_angle = 0.0f;
	s_angle -= 1.0f;

	TFixed angle = _FXD(s_angle);

	renderable_object->SetAngleX(angle / 2);
	renderable_object->SetAngleY(angle);
	renderable_object->SetAngleZ(angle / 3);

	CVisible<TShared3DObject>::_Show(renderable_object);
}

CVisibleCube::CVisibleCube()
{
	{
		const TFixed P_ONE = _FXD(+1.0f);
		const TFixed M_ONE = _FXD(-1.0f);
		const TFixed ZERO_ = _FXD( 0.0f);

		TFixedVec3 vertices[] =
		{
			TFixedVec3(M_ONE, P_ONE, M_ONE), // 0
			TFixedVec3(P_ONE, P_ONE, M_ONE), // 1
			TFixedVec3(M_ONE, M_ONE, M_ONE), // 2
			TFixedVec3(P_ONE, M_ONE, M_ONE), // 3
			TFixedVec3(M_ONE, P_ONE, P_ONE), // 4
			TFixedVec3(P_ONE, P_ONE, P_ONE), // 5
			TFixedVec3(M_ONE, M_ONE, P_ONE), // 6
			TFixedVec3(P_ONE, M_ONE, P_ONE)  // 7
		};

		TFixedVec4 vertex_color[] =
		{
			TFixedVec4(P_ONE, ZERO_, P_ONE, P_ONE),
			TFixedVec4(P_ONE, ZERO_, P_ONE, P_ONE),
			TFixedVec4(P_ONE, ZERO_, P_ONE, P_ONE),
			TFixedVec4(P_ONE, ZERO_, P_ONE, P_ONE),
			TFixedVec4(P_ONE, P_ONE, ZERO_, P_ONE),
			TFixedVec4(P_ONE, P_ONE, ZERO_, P_ONE),
			TFixedVec4(P_ONE, P_ONE, ZERO_, P_ONE),
			TFixedVec4(P_ONE, P_ONE, ZERO_, P_ONE)
		};

		unsigned short index_buffer[] =
		{
			0, 1, 2,
			2, 1, 3,
			1, 5, 3,
			3, 5, 7,
			5, 4, 7,
			7, 4, 6,
			4, 0, 6,
			6, 0, 2,
			4, 5, 0,
			0, 5, 1,
			2, 3, 6,
			6, 3, 7
		};

		*m_vertex_buffer << TVertexData<FVF_XYZ_DIF_NOR>(8, vertices, vertex_color, vertices);
		*m_vertex_buffer << TIndexData(sizeof(index_buffer) / sizeof(index_buffer[0]), index_buffer);

		m_vertex_object.bind(new CVertexObject(m_vertex_buffer, PRIMITIVES_MODE_TRIANGLE_LIST, 0, sizeof(index_buffer) / sizeof(index_buffer[0]), true));
		m_vertex_object->SetScale(_FXD(50));
	}

	m_visible.bind(new CVisibleCubeSub(m_vertex_object, 0));
}

////////////////////////////////////////////////////////////////////////////////
// implementation (presentation_util)

#include "util_convert_to_ucs.h"
#include "pd_res.h"

namespace presentation_util
{
	const char* GetFPS()
	{
		static long s_count       = 0;
		static long s_start_tick  = 0;
		static long s_end_tick    = 0;
		static char s_sz_FPS[256] = {0, };
		static bool s_tinkle      = true;

		++s_count;

		if (s_start_tick > 0)
		{
			s_end_tick = avej_lite::util::GetTicks();

			if (s_end_tick - s_start_tick > 1000)
			{
				s_ComposeString(s_sz_FPS, "FPS:@", s_count);

				s_tinkle = !s_tinkle;
				if (s_tinkle)
					s_sz_FPS[3] = ' ';
				
				s_count = 0;
				s_start_tick = avej_lite::util::GetTicks();
			}
		}
		else
		{
			s_ComposeString(s_sz_FPS, "FPS:--", 0);
			s_start_tick = avej_lite::util::GetTicks();
		}

		// multi-thread에 안전하지 못함
		return s_sz_FPS;
	}

	void RenderText(int x_dest, int y_dest, const widechar* sz_text_ucs2, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt)
	{
		static const IorzRenderText& ref_render_text = *GetTextInstance();

		ref_render_text.RenderText(x_dest, y_dest, sz_text_ucs2, color, fn_bit_blt);
	}

	void RenderText(int x_dest, int y_dest, const char* sz_text, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt)
	{
		// 현재는 이 사이즈로 고정
		static widechar w_str[256];

		// 항상 UCS-2 형식의 wchar_t로 컴파일 되어야 한다.
		CT_ASSERT(sizeof(widechar) == sizeof(wchar_t), widechar_is_not_wchar_t);

		ConvertUHC2UCS2(w_str, 256, sz_text, strlen(sz_text)+1);

		RenderText(x_dest, y_dest, w_str, color, fn_bit_blt);
	}

	bool LoadImage(const char* sz_image_name, avej_lite::TSurfaceDesc& surface_desc)
	{
		resource::TResStream    res_stream;
		avej_lite::TSurfaceDesc image_buffer_desc;

		if (resource::GetResouceNameToStream(sz_image_name, res_stream))
		{
			if (!avej_lite::util::DecodeSMgal(avej_lite::PIXELFORMAT_RGBA4444, res_stream.p_stream, res_stream.stream_length, image_buffer_desc))
				return false;
		}
		else
		{
			if (!avej_lite::util::DecodeSMgal(avej_lite::PIXELFORMAT_RGBA4444, sz_image_name, image_buffer_desc))
				return false;
		}

		memcpy(&surface_desc, &image_buffer_desc, sizeof(surface_desc));

		return true;
	}

} // namespace presentation_util

