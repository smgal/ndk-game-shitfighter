
#ifndef __GFX3D_DEVICE3D_H__
#define __GFX3D_DEVICE3D_H__

#include <string>
#include "avej_lite.h"

namespace avej { namespace gfx3d
{
	class CDevice3D
	{
		THIS_CLASS_IS_A_SINGLETON(CDevice3D);

	public:
		void Test(void);
		void Clear(unsigned long color);
		void Flush(void);
		void ProcessMessaage(unsigned long delay_time);

		// texture ����
		//?? �Ʒ� �͵� shared�� �ؾ� ��
		bool CreateTexture(avej_lite::TSurfaceDesc& in_out_surface_desc);
		void DestroyTexture(iu::shared_ptr<avej_lite::TSurfaceDesc> surface_desc);

		// frame buffer ����
		bool BackUpFrameBuffer(void);
		void RestoreFrameBuffer(void);

		struct TRect
		{
			int x, y, width, height;
			TRect() {}
			TRect(int _x, int _y, int _width, int _height)
			:	x(_x), y(_y), width(_width), height(_height) {}
		};

		// 2d feature ����
		void Begin2D(unsigned long tex_id);
		void FillRect(TRect rect, unsigned long color);
		void DrawImage(TRect dst_rect, iu::shared_ptr<avej_lite::TSurfaceDesc> src_image, TRect src_rect);
		void End2D(void);

	private:
		struct TImpl;
		struct TImpl* m_p_impl;
	};
}}

#endif
