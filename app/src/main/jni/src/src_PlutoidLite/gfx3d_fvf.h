
#ifndef __GFX3D_FVF_H__
#define __GFX3D_FVF_H__

#include "gfx3d_type.h"
#include "gfx3d_scene.h"

namespace avej { namespace gfx3d
{
	enum TFVF
	{
		// XY(Z) -> DIF -> NOR -> TEX
		FVF_XYZ_DIF_NOR,
		FVF_XYZ_NOR_TEX,
		FVF_XYZ_TEX,
		FVF_XY,
		FVF_XY_TEX,
		FVF_XY_DIF_TEX
	};

	enum TFVFElement
	{
		FVF_ELEMENT_XYZ,
		FVF_ELEMENT_COLOR,
		FVF_ELEMENT_NORMAL,
		FVF_ELEMENT_TEXTURE0
	};

	struct TFVFData
	{
		unsigned int num;
		TFixedVec2*  p_position2d;
		TFixedVec3*  p_position3d;
		TFixedVec4*  p_color;
		TFixedVec3*  p_normal;
		TFixedVec2*  p_tex_coord;

		TFVFData()
			: num(0), p_position2d(0), p_position3d(0), p_color(0), p_normal(0), p_tex_coord(0) {}
		TFVFData(int _num, TFixedVec3* _position, TFixedVec4* _color, TFixedVec3* _normal, TFixedVec2* _tex_coord)
			: num(_num), p_position2d(0), p_position3d(_position), p_color(_color), p_normal(_normal), p_tex_coord(_tex_coord) {}
		TFVFData(int _num, TFixedVec2* _position, TFixedVec4* _color, TFixedVec3* _normal, TFixedVec2* _tex_coord)
			: num(_num), p_position2d(_position), p_position3d(0), p_color(_color), p_normal(_normal), p_tex_coord(_tex_coord) {}
	};

	template <TFVF vertex_format>
	struct TVertexData: public TFVFData
	{
	};

	template <>
	struct TVertexData<FVF_XYZ_DIF_NOR>: public TFVFData
	{
		TVertexData(int _num, TFixedVec3* _position, TFixedVec4* _color, TFixedVec3* _normal)
			: TFVFData(_num, _position, _color, _normal, 0) {}
	};

	template <>
	struct TVertexData<FVF_XYZ_NOR_TEX>: public TFVFData
	{
		TVertexData(int _num, TFixedVec3* _position, TFixedVec3* _normal, TFixedVec2* _tex_coord)
			: TFVFData(_num, _position, 0, _normal, _tex_coord) {}
	};

	template <>
	struct TVertexData<FVF_XYZ_TEX>: public TFVFData
	{
		TVertexData(int _num, TFixedVec3* _position, TFixedVec2* _tex_coord)
			: TFVFData(_num, _position, 0, 0, _tex_coord) {}
	};

	template <>
	struct TVertexData<FVF_XY>: public TFVFData
	{
		TVertexData(int _num, TFixedVec2* _position)
			: TFVFData(_num, _position, 0, 0, 0) {}
	};

	template <>
	struct TVertexData<FVF_XY_TEX>: public TFVFData
	{
		TVertexData(int _num, TFixedVec2* _position, TFixedVec2* _tex_coord)
			: TFVFData(_num, _position, 0, 0, _tex_coord) {}
	};
	
	template <>
	struct TVertexData<FVF_XY_DIF_TEX>: public TFVFData
	{
		TVertexData(int _num, TFixedVec2* _position, TFixedVec4* _color, TFixedVec2* _tex_coord)
			: TFVFData(_num, _position, _color, 0, _tex_coord) {}
	};

	struct TIndexData
	{
		unsigned int    num;
		unsigned short* p_index_stream;

		TIndexData(unsigned int _num, unsigned short* _p_index_stream)
			: num(_num), p_index_stream(_p_index_stream) {}
	};

	struct TTexCoordData
	{
		TFixedVec2* p_tex_coord;

		TTexCoordData(TFixedVec2* _p_tex_coord)
			: p_tex_coord(_p_tex_coord) {}
	};

	struct TTexParam
	{
		enum TFilter
		{
			FILTER_DEFAULT,
			FILTER_NEAREST,
			FILTER_LINEAR,
		};

		TFilter min_filter;
		TFilter max_filter;

		TTexParam()
			: min_filter(FILTER_DEFAULT), max_filter(FILTER_DEFAULT) {}
		TTexParam(TFilter _min_filter, TFilter _max_filter)
			: min_filter(_min_filter), max_filter(_max_filter) {}
	};

	enum TRenderMode
	{
		RENDER_MODE_COPY,
		RENDER_MODE_ALPHA,
		RENDER_MODE_ADD
	};

	enum TRenderOption
	{
		RENDER_OPTION_NONE          = 0x00000000,
		RENDER_OPTION_ALWAYS_ON_TOP = 0x00000001,
		RENDER_OPTION_ORTHOGONAL    = 0x00000002,
		RENDER_OPTION_DWORD         = 0x7FFFFFFF
	};

	class CVertexBuffer
	{
	public:
		CVertexBuffer();
		virtual ~CVertexBuffer();

		CVertexBuffer& operator<<(const TFVFData& vertex_data);
		CVertexBuffer& operator<<(const TIndexData& index_data);
		CVertexBuffer& operator<<(const TTexCoordData& tex_coord);
		CVertexBuffer& operator<<(const shared::TTexTileList& tex_tile);
		CVertexBuffer& operator<<(const TTexParam& tex_param);

		virtual void Begin(TRenderMode render_mode, TRenderOption render_option = RENDER_OPTION_NONE);
		virtual void End(void);

		void DrawPrimitive(TPrimitivesMode type, unsigned int ix_start, unsigned int count);
		void DrawIndexedPrimitive(TPrimitivesMode type, unsigned int ix_start, unsigned int count);
		// void SetShader(TShaderSet shader_set);

	private:
		struct TImpl;
		struct TImpl* m_p_impl;
	};

	namespace shared
	{
		typedef iu::shared_ptr<class ::avej::gfx3d::CVertexBuffer> CVertexBuffer;
	}

	class CVertexObject
	{
	public:
		CVertexObject(const shared::CVertexBuffer& vertex_buffer, TPrimitivesMode primitives_type, unsigned int ix_start, unsigned int count, bool use_index);
		~CVertexObject();

		void   Begin(void);
		void   Begin(TRenderMode render_mode, TRenderOption render_option = RENDER_OPTION_NONE);
		void   End(void);
		void   DrawPrimitive(void);
		void   SetXY(int x, int y);
		void   SetZ(int z);
		void   SetColor(unsigned long color32bits);
		void   SetScale(TFixed scale);
		void   SetAngleX(TFixed angle);
		void   SetAngleY(TFixed angle);
		void   SetAngleZ(TFixed angle);
		TFixed GetAngleX(void);
		TFixed GetAngleY(void);
		TFixed GetAngleZ(void);

	private:
		struct TImpl;
		struct TImpl* m_p_impl;
	};

	namespace shared
	{
		typedef iu::shared_ptr<class ::avej::gfx3d::CVertexObject> CVertexObject;
	}

}} // namespace avej { namespace gfx3d

#endif // #ifndef USM3DVERTEX_H
