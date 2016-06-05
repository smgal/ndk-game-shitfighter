
#ifndef __PD_BASE_PRESENTATION_BASE_H__
#define __PD_BASE_PRESENTATION_BASE_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "pd_base_presentation.h"
#include "pd_visible.h"
#include "gfx3d_fvf.h"
#include "util_render_text.h"
//#include "avej_surface.h"

////////////////////////////////////////////////////////////////////////////////
// definition

typedef avej::gfx3d::shared::CVertexObject TShared3DObject;

////////////////////////////////////////////////////////////////////////////////
// interface (CVisible3D)

namespace presentation_util
{
	const char* GetFPS();
	void RenderText(int x_dest, int y_dest, const widechar* sz_text_ucs2, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt);
	void RenderText(int x_dest, int y_dest, const char* sz_text, unsigned long color, IorzRenderText::TFnBitBlt fn_bit_blt);
	bool LoadImage(const char* sz_image_name, avej_lite::TSurfaceDesc& surface_desc);
}

////////////////////////////////////////////////////////////////////////////////
// interface (CVisible3D)

class CVisible3D
{
public:
	typedef iu::shared_ptr<CVisible<TShared3DObject> > TSharedVisible3DObject;

	CVisible3D()
	:	m_vertex_buffer(new avej::gfx3d::CVertexBuffer)
	{
	}
	virtual ~CVisible3D()
	{
	}

	TSharedVisible3DObject GetVisible(void)
	{
		return m_visible;
	}

protected:
	avej::gfx3d::shared::CVertexBuffer  m_vertex_buffer;
	avej::gfx3d::shared::CVertexObject  m_vertex_object;
	TSharedVisible3DObject m_visible;

};

////////////////////////////////////////////////////////////////////////////////
// interface (CVisibleCube)

class CVisibleCube: public CVisible3D
{
	class CVisibleCubeSub: public CVisible<TShared3DObject>
	{
	public:
		CVisibleCubeSub(TShared3DObject renderable_object, FnShow fn_show);

	protected:
		virtual void _Show(TShared3DObject renderable_object) const;
	};

public:
	CVisibleCube();
};

#endif
