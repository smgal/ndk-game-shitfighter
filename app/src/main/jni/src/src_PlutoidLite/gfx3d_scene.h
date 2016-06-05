
#ifndef __AVEJ_GFX3D_SCENE_H__
#define __AVEJ_GFX3D_SCENE_H__

#include "gfx3d_type.h"

namespace avej { namespace gfx3d
{
	class CScene
	{
		friend class avej_lite::singleton<CScene>;
		friend class avej_lite::auto_deletor<CScene>;

		struct TImpl;

	public:
		struct Enable { };
		struct Disable { };

		class CLight
		{
			friend struct TImpl;
			struct TImpl;

		public:
			class CUnitLight
			{
				friend class  CLight;
				friend struct TImpl;

			public:
				struct Ambient
				{
					unsigned long color;
					Ambient(unsigned long color32bits): color(color32bits) {}
				};
				struct Diffuse
				{
					unsigned long color;
					Diffuse(unsigned long color32bits): color(color32bits) {}
				};
				struct Pos
				{
					TFixedVec3 pos;
					Pos(TFixedVec3 _pos): pos(_pos) {}
				};

				CUnitLight& operator<<(const Ambient& ambient);
				CUnitLight& operator<<(const Diffuse& diffuse);
				CUnitLight& operator<<(const Pos&     pos);
				CUnitLight& operator<<(const Enable&  flag);
				CUnitLight& operator<<(const Disable& flag);

			private:
				CUnitLight();
				CUnitLight(int index);
				~CUnitLight();

				struct TImpl;
				struct TImpl* m_p_impl;
			};

			CUnitLight& operator[](int index);
			CLight&     operator<<(const Enable& flag);
			CLight&     operator<<(const Disable& flag);

		private:
			CLight();
			~CLight();

			struct TImpl* m_p_impl;
		};

		class CFog
		{
			friend struct TImpl;

		public:
			struct TFogDesc
			{
				TFixed        density;
				TFixed        start;
				TFixed        end;
				unsigned long color;

				TFogDesc() {}
				TFogDesc(TFixed _density, TFixed _start, TFixed _end, unsigned long _color)
				:	density(_density), start(_start), end(_end), color(_color) {}
			};

			CFog();
			~CFog();

			CFog& operator<<(const TFogDesc& fog_desc);
			CFog& operator>>(TFogDesc& camera_desc);
			CFog& operator<<(const Enable& flag);
			CFog& operator<<(const Disable& flag);

		private:
			struct TImpl;
			struct TImpl* m_p_impl;
		};

		class CCamera
		{
			friend struct TImpl;

		public:
			struct TCameraDesc
			{
				int   incident_angle; // 입사각. degree
				float rotation_angle; // 회전각. degree
				int   distance;
				TCameraDesc() {}
				TCameraDesc(int _incident_angle, float _rotation_angle, int _distance)
				:	incident_angle(_incident_angle), rotation_angle(_rotation_angle), distance(_distance) {}
			};

			CCamera();
			~CCamera();

			CCamera& operator<<(const TCameraDesc& camera_desc);
			CCamera& operator>>(TCameraDesc& camera_desc);

		private:
			struct TImpl;
			struct TImpl* m_p_impl;
		};

		class CProjection
		{
			friend class  CScene;
			friend struct TImpl;

		public:
			struct CProjectionParam
			{
				TFixed x_radius;
				TFixed y_radius;
				TFixed z_near;
				TFixed z_far;
				TFixed default_scale;

				CProjectionParam(TFixed _x_radius, TFixed _y_radius, TFixed _z_near, TFixed _z_far, TFixed _default_scale)
					: x_radius(_x_radius), y_radius(_y_radius), z_near(_z_near), z_far(_z_far), default_scale(_default_scale) { }
			};

			CProjection& operator<<(const CProjectionParam& param);
			CProjection& operator<<(const Enable& flag);
			CProjection& operator<<(const Disable& flag);

		private:
			CProjection();
			~CProjection();

			struct TImpl;
			struct TImpl* m_p_impl;
		};

		struct TSceneDesc
		{
			TFixed z_push;
			TFixed z_push_fixed;
			TFixed default_scale;
		};

		CScene& operator<<(const Enable& flag);
		CScene& operator<<(const Disable& flag);
		CScene& operator>>(TSceneDesc& scene_desc);

		CLight&       Light(void);
		CFog&         Fog(void);
		CCamera&      Camera(void);
		CProjection&  Projection(void);

	private:
		CScene();
		~CScene();

		struct TImpl* m_p_impl;
	};

}} // namespace avej { namespace gfx2d

#endif
