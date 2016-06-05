
////////////////////////////////////////////////////////////////////////////////
// uses

#if defined(_WIN32)
#	pragma warning(disable: 4786)
#	pragma comment(lib, "libgles_cm.lib")
#endif

#include "avej_lite.h"

#include "gfx3d_config.h"
#include "gfx3d_type.h"
#include "gfx3d_device3d.h"

#include <stdio.h>
#include <assert.h>
#include <memory>
#include <map>


//?? �̰��� config��?
#define USE_ZBUFFER

#define for if (0); else for

////////////////////////////////////////////////////////////////////////////////
// interface (CDevice3D::TImpl)

namespace avej { namespace gfx3d
{
	struct CDevice3D::TImpl
	{
		bool m_is_valid;
		//?? �̸� ����
		avej_lite::IGfxDevice* g_p_gfx_device;
		avej_lite::IGfxSurface* g_p_back_buffer;

		typedef std::map<std::string, iu::shared_ptr<avej_lite::TSurfaceDesc> > TTexMap;
		TTexMap              tex_list;

		TImpl();
		~TImpl();
	};
}}

////////////////////////////////////////////////////////////////////////////////
// implementation (CDevice3D::TImpl)

static void SetDefaultState()
{
	glClearColorx(0, 0, 0, 0);
	glDisable(GL_DEPTH_TEST);

#if 0   // �̰��� Ȱ��ȭ ��Ű�� backface culling�� �õ��ȴ�.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
#endif
	// ����Ʈ ����
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// GL_MODULATE, GL_DECAL, GL_BLEND, or GL_REPLACE
}


#define CHECK_EGL_ERROR(message)                 \
	{                                            \
		EGLint error = eglGetError();            \
		if (error != EGL_SUCCESS)                \
		{                                        \
			osal::PrintEGLError(message, error); \
			break;                               \
		}                                        \
	}

avej::gfx3d::CDevice3D::TImpl::TImpl()
: m_is_valid(false), g_p_gfx_device(0), g_p_back_buffer(0)
{
	g_p_gfx_device = avej_lite::IGfxDevice::GetInstance();

	if (g_p_gfx_device)
	{
		g_p_gfx_device->GetSurface(&g_p_back_buffer);

		m_is_valid = (g_p_back_buffer != NULL);

		if (m_is_valid)
		{
			SetDefaultState();
		}
	}
}

avej::gfx3d::CDevice3D::TImpl::~TImpl()
{
	if (this->m_is_valid)
	{
		g_p_gfx_device->Release();
	}
}

avej::gfx3d::CDevice3D::CDevice3D()
	: m_p_impl(0)
{
	try
	{
		m_p_impl = new TImpl();
		assert(m_p_impl != NULL);
	}
	catch (...)
	{
		delete m_p_impl;
		throw;
	}
}

avej::gfx3d::CDevice3D::~CDevice3D()
{
	delete m_p_impl;
}

void avej::gfx3d::CDevice3D::Clear(unsigned long color)
{
	glClearColorx
	(
		((color >> 16) & 0xFF) << 8,
		((color >>  8) & 0xFF) << 8,
		((color >>  0) & 0xFF) << 8,
		((color >> 24) & 0xFF) << 8
	);

#if defined(USE_ZBUFFER)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
	glClear(GL_COLOR_BUFFER_BIT);
#endif

	glClearColorx(0, 0, 0, 0);
}

void avej::gfx3d::CDevice3D::Flush(void)
{
	m_p_impl->g_p_gfx_device->Flip();
}

void avej::gfx3d::CDevice3D::ProcessMessaage(unsigned long delay_time)
{
	//gfx3d::ProcessMessage(delay_time);
}

//?? ���⼭ �� unsigned short* �� ����?
static unsigned long s_CreateTexture(int width, int height, unsigned short* p_buffer, bool use_alpha_channel)
{
	GLint  format = GL_RGBA;
	GLenum pixel_type = (use_alpha_channel) ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_SHORT_5_5_5_1;

	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	GLenum error;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, pixel_type, p_buffer);
	error = glGetError();

	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR, GL_NEAREST
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // GL_MODULATE );

	return handle;
}

//?? TSurfaceDesc�� �ڵ� ���� �Ǵ� ����� �����ؾ� �� ���ε�....
bool avej::gfx3d::CDevice3D::CreateTexture(avej_lite::TSurfaceDesc& in_out_surface_desc)
{
	//?? ������ �� ���� ������ �����ؾ� �Ѵ�.
	bool enable_alpha_test = (in_out_surface_desc.buffer_desc.pixel_format == avej_lite::PIXELFORMAT_RGBA4444);
	//?? 2�� �¼��� �ƴ϶�� ������ �ؾ� �Ѵ�.
	//?? ������ Lock() �˻縦 �ؾ� �Ѵ�.
	in_out_surface_desc.hw_handle = s_CreateTexture(in_out_surface_desc.buffer_desc.width, in_out_surface_desc.buffer_desc.height, (unsigned short*)in_out_surface_desc.lock_desc.p_buffer, enable_alpha_test);
	//?? ���� �˻簡 �ʿ��ϴ�

	return true;
}

//?? ���� texture�� refrence count ������ ������ �ִ�.
//?? ���������δ� TSurfaceDesc�� CSurface�� �Ǿ�� �ϸ�,
//?? Destroy �Ӽ��� �Ʒ� ������ ����, Unlock �� �ٽ� texture�� HW�� �ݿ��ǰ� �ؾ� �Ѵ�.
void avej::gfx3d::CDevice3D::DestroyTexture(iu::shared_ptr<avej_lite::TSurfaceDesc> surface_desc)
{
	if (surface_desc->hw_handle)
	{
		//?? ������ surface_desc.useCount()�� ���� �ؾ� �Ѵ�.
		{
			glDeleteTextures(1, (GLuint*)&surface_desc->hw_handle);
			if (surface_desc->has_ownership)
			{
				//?? (unsigned short*)�� PIXELFORMAT_RGBA5551�� �����ϵ��� �ٲ�� �Ѵ�. �ƿ� �� �� config�� �ε���.
				delete[] (unsigned short*)surface_desc->lock_desc.p_buffer;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//

void avej::gfx3d::CDevice3D::Begin2D(unsigned long tex_id)
{
	m_p_impl->g_p_gfx_device->BeginDraw();
}

void avej::gfx3d::CDevice3D::End2D(void)
{
	m_p_impl->g_p_gfx_device->EndDraw();
}

void avej::gfx3d::CDevice3D::FillRect(gfx3d::CDevice3D::TRect dst_rect, unsigned long color)
{
	m_p_impl->g_p_back_buffer->FillRect(color, dst_rect.x, dst_rect.y, dst_rect.width, dst_rect.height);
}

void avej::gfx3d::CDevice3D::DrawImage(TRect dst_rect, iu::shared_ptr<avej_lite::TSurfaceDesc> src_image, TRect src_rect)
{
	assert(0);
	//m_p_impl->g_p_back_buffer->StretchBlt(dst_rect.x, dst_rect.y, dst_rect.width, dst_rect.height, src_image, src_rect.x, src_rect.y, src_rect.width, src_rect.height);
}

bool avej::gfx3d::CDevice3D::BackUpFrameBuffer(void)
{
	return true;
}

void avej::gfx3d::CDevice3D::RestoreFrameBuffer(void)
{
}
