
#include "avej_lite.h"
#include "gfx3d_config.h"
#include "gfx3d_fvf.h"
#include "gfx3d_util.h"

//?? 임시
#include <math.h>

using namespace avej;
using namespace avej::gfx3d;

////////////////////////////////////////////////////////////////////////////////
// CVertexBuffer

#define CHECK_THROW(cond)  if (!(cond)) throw 0;

struct CVertexBuffer::TImpl
{
	struct TIndexBuffer
	{
		unsigned long  num;
		unsigned short buffer[1];
	};

	TFVFData             fvf_data;
	TFixed*              p_vertex_data;
	TIndexBuffer*        p_index_data;
	shared::TTexTileList tex_tile_list;
	TTexParam            tex_param;

	struct TRenderParam
	{
		TRenderMode    render_mode;
		TRenderOption  render_option;

		TRenderParam() {}
		TRenderParam(TRenderMode _render_mode, TRenderOption _render_option)
		:	render_mode(_render_mode), render_option(_render_option) {}
	};

	iu::optional<TRenderParam> render_param;

	TImpl()
		: p_vertex_data(0), p_index_data(0)
	{
	}
	~TImpl()
	{
		delete p_vertex_data;
		delete p_index_data;
	}

	void AssignVertexBuffer(const TFVFData& vertex_buffer)
	{
		if (vertex_buffer.num > 0)
		{
			int num_of_glfixed = 0;

			num_of_glfixed += (vertex_buffer.p_position2d) ? 2 : 0;
			num_of_glfixed += (vertex_buffer.p_position3d) ? 3 : 0;
			num_of_glfixed += (vertex_buffer.p_color)      ? 4 : 0;
			num_of_glfixed += (vertex_buffer.p_normal)     ? 3 : 0;
			num_of_glfixed += (vertex_buffer.p_tex_coord)  ? 2 : 0;

			TFixed* p_temp = new (std::nothrow) TFixed[num_of_glfixed * vertex_buffer.num];

			if (p_temp)
			{
				int      index_of_glfixed = 0;
				int      copy_bytes;
				TFVFData fvf_temp;

				fvf_temp.num = vertex_buffer.num;

				if (vertex_buffer.p_position2d)
				{
					fvf_temp.p_position2d = reinterpret_cast<TFixedVec2*>(&p_temp[index_of_glfixed]);
					copy_bytes            = vertex_buffer.num * 2;
					index_of_glfixed     += copy_bytes;
					memcpy(fvf_temp.p_position2d, vertex_buffer.p_position2d, copy_bytes * sizeof(TFixed));
				}
				if (vertex_buffer.p_position3d)
				{
					fvf_temp.p_position3d = reinterpret_cast<TFixedVec3*>(&p_temp[index_of_glfixed]);
					copy_bytes            = vertex_buffer.num * 3;
					index_of_glfixed     += copy_bytes;
					memcpy(fvf_temp.p_position3d, vertex_buffer.p_position3d, copy_bytes * sizeof(TFixed));
				}
				if (vertex_buffer.p_color)
				{
					fvf_temp.p_color      = reinterpret_cast<TFixedVec4*>(&p_temp[index_of_glfixed]);
					copy_bytes            = vertex_buffer.num * 4;
					index_of_glfixed     += copy_bytes;
					memcpy(fvf_temp.p_color, vertex_buffer.p_color, copy_bytes * sizeof(TFixed));
				}
				if (vertex_buffer.p_normal)
				{
					fvf_temp.p_normal     = reinterpret_cast<TFixedVec3*>(&p_temp[index_of_glfixed]);
					copy_bytes            = vertex_buffer.num * 3;
					index_of_glfixed     += copy_bytes;
					memcpy(fvf_temp.p_normal, vertex_buffer.p_normal, copy_bytes * sizeof(TFixed));
				}
				if (vertex_buffer.p_tex_coord)
				{
					fvf_temp.p_tex_coord = reinterpret_cast<TFixedVec2*>(&p_temp[index_of_glfixed]);
					copy_bytes           = vertex_buffer.num * 2;
					index_of_glfixed    += copy_bytes;
					memcpy(fvf_temp.p_tex_coord, vertex_buffer.p_tex_coord, copy_bytes * sizeof(TFixed));
				}

				assert(int(num_of_glfixed * vertex_buffer.num) == index_of_glfixed);

				delete p_vertex_data;

				p_vertex_data = p_temp;
				memcpy(&fvf_data, &fvf_temp, sizeof(fvf_data));
			}
		}
		else
		{
			delete p_vertex_data;

			p_vertex_data = NULL;
			memset(&fvf_data, 0, sizeof(fvf_data));
		}
	}

	void AssignIndexBuffer(unsigned int num, unsigned short* p_index_buffer)
	{
		if ((num >= 0) && (p_index_buffer))
		{
			TIndexBuffer* p_temp = reinterpret_cast<TIndexBuffer*>(new (std::nothrow) unsigned char[sizeof(TIndexBuffer) + sizeof(unsigned short) * (num-1)]);

			if (p_temp)
			{
				delete p_index_data;

				p_temp->num = num;
				memcpy(p_temp->buffer, p_index_buffer, sizeof(unsigned short) * num);

				p_index_data = p_temp;
			}
		}
		else
		{
			delete p_index_data;
			p_index_data = 0;
		}
	}

};

CVertexBuffer::CVertexBuffer()
: m_p_impl(new TImpl)
{
}

CVertexBuffer::~CVertexBuffer()
{
	// 이것은 로직을 체크하기 위한 것으로 Begin() / End() 짝을 잘 맞추었는지 확인만 한다.
	// 동작과는 별개의 assertion이다.
	assert(m_p_impl->render_param.invalid());

	delete m_p_impl;
}

CVertexBuffer& CVertexBuffer::operator<<(const TFVFData& vertex_data)
{
	assert(m_p_impl);

	m_p_impl->AssignVertexBuffer(vertex_data);

	return *this;
}

CVertexBuffer& CVertexBuffer::operator<<(const TIndexData& index_data)
{
	assert(m_p_impl);

	m_p_impl->AssignIndexBuffer(index_data.num, index_data.p_index_stream);

	return *this;
}

CVertexBuffer& CVertexBuffer::operator<<(const TTexCoordData& tex_coord)
{
	assert(m_p_impl);

	if (m_p_impl->fvf_data.p_tex_coord)
	{
		if (tex_coord.p_tex_coord)
		{
			memcpy(m_p_impl->fvf_data.p_tex_coord, tex_coord.p_tex_coord, m_p_impl->fvf_data.num * 2 * sizeof(TFixed));
		}
	}

	return *this;
}

CVertexBuffer& CVertexBuffer::operator<<(const shared::TTexTileList& tex_tile_list)
{
	assert(m_p_impl);

	m_p_impl->tex_tile_list = tex_tile_list;

	return *this;
}

CVertexBuffer& CVertexBuffer::operator<<(const TTexParam& tex_param)
{
	assert(m_p_impl);

	m_p_impl->tex_param = tex_param;

	return *this;
}

void CVertexBuffer::Begin(TRenderMode render_mode, TRenderOption render_option)
{
	assert(m_p_impl);

	// 이것은 로직을 체크하기 위한 것으로 내부 코드에서는 이것을 발생시키지 않도록 Begin() / End() 짝을 잘 맞춰 주는 것을 원칙으로 한다.
	// assert(m_p_impl->render_param.invalid());

	// render parameter 교체
	m_p_impl->render_param.reset(CVertexBuffer::TImpl::TRenderParam(render_mode, render_option));

	if (m_p_impl->fvf_data.p_position2d)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FIXED, 0, m_p_impl->fvf_data.p_position2d);
	}

	if (m_p_impl->fvf_data.p_position3d)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FIXED, 0, m_p_impl->fvf_data.p_position3d);
	}

	if (m_p_impl->fvf_data.p_color)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FIXED, 0, m_p_impl->fvf_data.p_color);
	}

	if (m_p_impl->fvf_data.p_normal)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FIXED, 0, m_p_impl->fvf_data.p_normal);
//		glEnable(GL_NORMALIZE);

		glEnable(GL_LIGHTING);
	}

	if (m_p_impl->fvf_data.p_tex_coord)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FIXED, 0, m_p_impl->fvf_data.p_tex_coord);

		if (m_p_impl->tex_tile_list.get())
		{
			switch (render_mode)
			{
			case RENDER_MODE_COPY:
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				break;
			case RENDER_MODE_ALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0);
				break;
			case RENDER_MODE_ADD:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0);
				break;
			}

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_p_impl->tex_tile_list->handle->hw_handle);

			switch (m_p_impl->tex_param.min_filter)
			{
			case TTexParam::FILTER_NEAREST:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			case TTexParam::FILTER_LINEAR:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;
			default:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			switch (m_p_impl->tex_param.max_filter)
			{
			case TTexParam::FILTER_NEAREST:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case TTexParam::FILTER_LINEAR:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			default:
				glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	// Render parameter 조정
	{
		if (render_option & RENDER_OPTION_ALWAYS_ON_TOP)
		{
			glDisable(GL_DEPTH_TEST);
		}
		if (render_option & RENDER_OPTION_ORTHOGONAL)
		{
			static bool   s_is_first = true;
			static TFixed s_camera_matrix[16];

			//?? 중간에 projection matrix가 바뀔 수 있도 있으므로 궁극적으로는 그것을 고려 해야 한다.
			if (s_is_first)
			{
				CScene::TSceneDesc scene_desc;
				avej_lite::singleton<CScene>::get() >> scene_desc;

				// z near * default_scale의 값. 단, default_scale 이 10이 되면 보이지 않음 
				TFixedVec3 eye_vector(_FXD(0), _FXD(0), -scene_desc.z_push_fixed);
				TFixedVec3 at_vector (_FXD(0), _FXD(0), _FXD(0));
				TFixedVec3 up_vector (_FXD(0), _FXD(1), _FXD(0));

				avej::gfx3d::GetMatrixLookAt(s_camera_matrix, eye_vector, at_vector, up_vector);

				s_is_first = false;
			}

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadMatrixx(s_camera_matrix);
			glPushMatrix();

			glDisable(GL_FOG);
			glDisable(GL_LIGHTING);
		}
	}
}

void CVertexBuffer::End(void)
{
	assert(m_p_impl->render_param.valid());

	if (m_p_impl->render_param.valid())
	{
		unsigned long render_option = m_p_impl->render_param->render_option;

		if (render_option & RENDER_OPTION_ORTHOGONAL)
		{
			//?? 원래부터 켜져 있었는지 확인해야 함
			glEnable(GL_LIGHTING);
			glEnable(GL_FOG);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopMatrix();

		}
		//?? Begin() 전에도 disabled인 경우에는 이렇게 하면 안된다.
		if (render_option & RENDER_OPTION_ALWAYS_ON_TOP)
		{
			glEnable(GL_DEPTH_TEST);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable           (GL_LIGHTING);
	glDisable           (GL_NORMALIZE);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable           (GL_TEXTURE_2D);

	// render parameter 리셋
	m_p_impl->render_param.reset();
}

void CVertexBuffer::DrawPrimitive(TPrimitivesMode type, unsigned int ix_start, unsigned int count)
{
	switch (type)
	{
	case PRIMITIVES_MODE_TRIANGLE_LIST:
		glDrawArrays(GL_TRIANGLES, ix_start, count);
		break;
	case PRIMITIVES_MODE_TRIANGLE_STRIP:
		glDrawArrays(GL_TRIANGLE_STRIP, ix_start, count);
		break;
	}
}

void CVertexBuffer::DrawIndexedPrimitive(TPrimitivesMode type, unsigned int ix_start, unsigned int count)
{
	if (m_p_impl->p_index_data)
	{
		if (ix_start + count > m_p_impl->p_index_data->num)
			return;

		switch (type)
		{
		case PRIMITIVES_MODE_TRIANGLE_LIST:
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, &m_p_impl->p_index_data->buffer[ix_start]);
			break;
		case PRIMITIVES_MODE_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, &m_p_impl->p_index_data->buffer[ix_start]);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// CVertexObject

struct CVertexObject::TImpl
{
	shared::CVertexBuffer m_vertext_buffer;
	TPrimitivesMode       m_primitives_type;
	unsigned int          m_ix_start;
	unsigned int          m_count;
	bool                  m_use_index;

	TFixedVec3            m_pos;
	iu::optional<TFixedVec4>  m_color;
	TFixed                m_scale;
	TFixedVec3            m_angle;

	TImpl(const shared::CVertexBuffer& vertex_buffer, TPrimitivesMode primitives_type, unsigned int ix_start, unsigned int count, bool use_index)
		: m_vertext_buffer(vertex_buffer), m_primitives_type(primitives_type), m_ix_start(ix_start), m_count(count), m_use_index(use_index),
		  m_pos(0, 0, 0), m_scale(_FXD(1)), m_angle(0, 0, 0)
	{
	}
};

CVertexObject::CVertexObject(const shared::CVertexBuffer& vertex_buffer, TPrimitivesMode primitives_type, unsigned int ix_start, unsigned int count, bool use_index)
: m_p_impl(new TImpl(vertex_buffer, primitives_type, ix_start, count, use_index))
{
}

CVertexObject::~CVertexObject()
{
	delete m_p_impl;
}

void CVertexObject::Begin(void)
{
	// using the default render-mode
	m_p_impl->m_vertext_buffer->Begin(RENDER_MODE_COPY, RENDER_OPTION_NONE);
}

void CVertexObject::Begin(TRenderMode render_mode, TRenderOption render_option)
{
	m_p_impl->m_vertext_buffer->Begin(render_mode, render_option);
}

void CVertexObject::End(void)
{
	m_p_impl->m_vertext_buffer->End();
/*?? 임시.. 잘 안됨
	glBindTexture(GL_TEXTURE_2D, 1);
	glDrawTexxOES(100, 100, 30, 512, 512);
*/
}

void CVertexObject::DrawPrimitive(void)
{
	CScene& scene = avej_lite::singleton<CScene>::get();

	CScene::TSceneDesc scene_desc;

	scene >> scene_desc;

	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
	glPushMatrix();

	// 만약 glLoadMatrix()가 아닌 glLoadIdentity()로 했다면, glTranslatex()의 z 부분을 scene_desc.z_push만큼 뒤로 밀어야 한다.
	glTranslatex(_FXD_mul(m_p_impl->m_pos.x, scene_desc.default_scale), _FXD_mul(m_p_impl->m_pos.y, scene_desc.default_scale), m_p_impl->m_pos.z);

	if (m_p_impl->m_angle.x)
		glRotatex(m_p_impl->m_angle.x, _FXD(1), 0, 0);
	if (m_p_impl->m_angle.y)
		glRotatex(m_p_impl->m_angle.y, 0, _FXD(1), 0);
	if (m_p_impl->m_angle.z)
		glRotatex(m_p_impl->m_angle.z, 0, 0, _FXD(1));

	{
		TFixed scale = _FXD_mul(scene_desc.default_scale, m_p_impl->m_scale);
		glScalex(scale, scale, scale);
	}

	if (m_p_impl->m_color.valid())
		glColor4x(m_p_impl->m_color->r, m_p_impl->m_color->g, m_p_impl->m_color->b, m_p_impl->m_color->a);

	if (m_p_impl->m_use_index)
		m_p_impl->m_vertext_buffer->DrawIndexedPrimitive(m_p_impl->m_primitives_type, m_p_impl->m_ix_start, m_p_impl->m_count);
	else
		m_p_impl->m_vertext_buffer->DrawPrimitive(m_p_impl->m_primitives_type, m_p_impl->m_ix_start, m_p_impl->m_count);

	if (m_p_impl->m_color.valid())
		glColor4x(0x10000, 0x10000, 0x10000, 0x10000);
}

void CVertexObject::SetXY(int x, int y)
{
	m_p_impl->m_pos.x = _FXD(x);
	m_p_impl->m_pos.y = _FXD(y);
}

void CVertexObject::SetZ(int z)
{
	m_p_impl->m_pos.z = _FXD(z);
}

void CVertexObject::SetColor(unsigned long color32bits)
{
	m_p_impl->m_color.reset(_FXD_color2array(color32bits));
}

void CVertexObject::SetScale(TFixed scale)
{
	m_p_impl->m_scale = scale;
}

void CVertexObject::SetAngleX(TFixed angle)
{
	m_p_impl->m_angle.x = angle;
}

void CVertexObject::SetAngleY(TFixed angle)
{
	m_p_impl->m_angle.y = angle;
}

void CVertexObject::SetAngleZ(TFixed angle)
{
	m_p_impl->m_angle.z = angle;
}

TFixed CVertexObject::GetAngleX(void)
{
	return m_p_impl->m_angle.x;
}

TFixed CVertexObject::GetAngleY(void)
{
	return m_p_impl->m_angle.y;
}

TFixed CVertexObject::GetAngleZ(void)
{
	return m_p_impl->m_angle.z;
}
