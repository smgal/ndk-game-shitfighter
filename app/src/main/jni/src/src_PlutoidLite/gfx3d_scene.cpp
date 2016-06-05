
#include "gfx3d_config.h"
#include "gfx3d_type.h"
#include "gfx3d_scene.h"
#include <assert.h>

using namespace avej::gfx3d;

////////////////////////////////////////////////////////////////////////////////
// CScene::CUnitLight

struct CScene::CLight::CUnitLight::TImpl
{
	bool          is_valid;
	unsigned long ambient_color;
	unsigned long diffuse_color;
	TFixedVec4    pos;
	unsigned int  light_index;
	TImpl()
		: is_valid(false), ambient_color(0), diffuse_color(0), light_index(0) {}
};

CScene::CLight::CUnitLight::CUnitLight()
	: m_p_impl(new TImpl)
{
}

CScene::CLight::CUnitLight::CUnitLight(int index)
	: m_p_impl(new TImpl)
{
	m_p_impl->is_valid = true;

	switch (index)
	{
		case  0: m_p_impl->light_index = GL_LIGHT0; break;
		case  1: m_p_impl->light_index = GL_LIGHT1; break;
		case  2: m_p_impl->light_index = GL_LIGHT2; break;
		case  3: m_p_impl->light_index = GL_LIGHT3; break;
		case  4: m_p_impl->light_index = GL_LIGHT4; break;
		case  5: m_p_impl->light_index = GL_LIGHT5; break;
		case  6: m_p_impl->light_index = GL_LIGHT6; break;
		case  7: m_p_impl->light_index = GL_LIGHT7; break;
		default:
			m_p_impl->light_index = 0;
			m_p_impl->is_valid = false;
	}
}

CScene::CLight::CUnitLight::~CUnitLight()
{
	delete m_p_impl;
}

CScene::CLight::CUnitLight& avej::gfx3d::CScene::CLight::CUnitLight::operator<<(const CScene::CLight::CUnitLight::Ambient& ambient)
{
	if (m_p_impl->is_valid)
	{
		m_p_impl->ambient_color = ambient.color;
		TFixedVec4 ambientLight = _FXD_color2array(ambient.color);

		glLightxv(m_p_impl->light_index, GL_AMBIENT, ambientLight.fixed);
	}

	return *this;
}

CScene::CLight::CUnitLight& CScene::CLight::CUnitLight::operator<<(const CScene::CLight::CUnitLight::Diffuse& diffuse)
{
	if (m_p_impl->is_valid)
	{
		m_p_impl->diffuse_color = diffuse.color;
		TFixedVec4 diffuseLight = _FXD_color2array(diffuse.color);

		glLightxv(m_p_impl->light_index, GL_DIFFUSE,  diffuseLight.fixed);
	}

	return *this;
}

CScene::CLight::CUnitLight& CScene::CLight::CUnitLight::operator<<(const CScene::CLight::CUnitLight::Pos& pos)
{
	if (m_p_impl->is_valid)
	{
		m_p_impl->pos.fixed[0] = pos.pos.x;
		m_p_impl->pos.fixed[1] = pos.pos.y;
		m_p_impl->pos.fixed[2] = pos.pos.z;
		m_p_impl->pos.fixed[3] = _FXD(0); // directional light

		glLightxv(m_p_impl->light_index, GL_POSITION,  m_p_impl->pos.fixed);
	}

	return *this;
}

CScene::CLight::CUnitLight& CScene::CLight::CUnitLight::operator<<(const CScene::Enable&  flag)
{
	if (m_p_impl->is_valid)
	{
		glEnable(m_p_impl->light_index);
	}

	return *this;
}

CScene::CLight::CUnitLight& CScene::CLight::CUnitLight::operator<<(const CScene::Disable& flag)
{
	if (m_p_impl->is_valid)
	{
		glDisable(m_p_impl->light_index);
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CScene::CLight

struct CScene::CLight::TImpl
{
	enum { MAX_LIGHT = 8 };
	CUnitLight  null_light;
	CUnitLight* p_unit_light[MAX_LIGHT];

	TImpl()
	{
		for (int i = 0; i < MAX_LIGHT; i++)
			p_unit_light[i] = new CUnitLight(i);
	}
	~TImpl()
	{
		for (int i = 0; i < MAX_LIGHT; i++)
			delete p_unit_light[i];
	}
};

CScene::CLight::CLight()
	: m_p_impl(new TImpl)
{
}

CScene::CLight::~CLight()
{
	delete m_p_impl;
}

CScene::CLight::CUnitLight& CScene::CLight::operator[](int index)
{
	if (index >= 0 && index < TImpl::MAX_LIGHT)
		return *m_p_impl->p_unit_light[index];
	else
		return m_p_impl->null_light;
}

CScene::CLight& CScene::CLight::operator<<(const CScene::Enable& flag)
{
	glEnable(GL_LIGHTING);
	return *this;
}

CScene::CLight& CScene::CLight::operator<<(const CScene::Disable& flag)
{
	glDisable(GL_LIGHTING);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CScene::CFog

struct CScene::CFog::TImpl
{
	TFogDesc  fog_desc;
	bool      has_enabled;

	TImpl()
	:	has_enabled(false)
	{

	}
	~TImpl()
	{
	}

	void Apply(void)
	{
		glFogx(GL_FOG_MODE,    GL_LINEAR);
		glFogx(GL_FOG_DENSITY, fog_desc.density);
		glFogx(GL_FOG_START,   fog_desc.start);
		glFogx(GL_FOG_END,     fog_desc.end);
		glHint(GL_FOG_HINT,    GL_DONT_CARE);

		GLfloat fog_color[4] =
		{
			((fog_desc.color >> 16) & 0xFF) * 1.0f / 255.0f,
			((fog_desc.color >>  8) & 0xFF) * 1.0f / 255.0f,
			((fog_desc.color >>  0) & 0xFF) * 1.0f / 255.0f,
			((fog_desc.color >> 24) & 0xFF) * 1.0f / 255.0f,
		}; 

		glFogfv (GL_FOG_COLOR, fog_color); 
	}
};

CScene::CFog::CFog()
	: m_p_impl(new TImpl)
{
	m_p_impl->fog_desc.density = _FXD(0.5f);
	m_p_impl->fog_desc.start   = _FXD(960);
	m_p_impl->fog_desc.end     = _FXD(960*5);
	m_p_impl->fog_desc.color   = 0xFF000000;

	m_p_impl->Apply();
}

CScene::CFog::~CFog()
{
	if (m_p_impl->has_enabled)
	{
		glDisable(GL_FOG);
	}

	delete m_p_impl;
}

CScene::CFog& CScene::CFog::operator<<(const TFogDesc& fog_desc)
{
	m_p_impl->fog_desc = fog_desc;

	m_p_impl->Apply();

	return *this;
}

CScene::CFog& CScene::CFog::operator>>(TFogDesc& fog_desc)
{
	fog_desc = m_p_impl->fog_desc;

	return *this;
}

CScene::CFog& CScene::CFog::operator<<(const CScene::Enable& flag)
{
	if (!m_p_impl->has_enabled)
	{
		glEnable(GL_FOG);
		m_p_impl->has_enabled = true;
	}

	return *this;
}

CScene::CFog& CScene::CFog::operator<<(const CScene::Disable& flag)
{
	if (m_p_impl->has_enabled)
	{
		glDisable(GL_FOG);
		m_p_impl->has_enabled = false;
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CScene::CCamera

#include "gfx3d_util.h"
#include <math.h>

struct CScene::CCamera::TImpl
{
	TCameraDesc camera_desc;
	TFixed      camera_matrix[16];

	TImpl()
	:	m_has_pushed_matrix(false)
	{

	}
	~TImpl()
	{
		if (m_has_pushed_matrix)
		{
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

	}

	void MakeMatrix(void)
	{
		double rotation_angle = m_ToRadian(camera_desc.rotation_angle);
		double incident_angle = m_ToRadian(camera_desc.incident_angle);

		float eye_x = camera_desc.distance * sin(rotation_angle) * cos(incident_angle);
		float eye_y = camera_desc.distance * cos(rotation_angle) * cos(incident_angle);
		float eye_z = camera_desc.distance * sin(incident_angle);

		float up_x  = sin(rotation_angle);
		float up_y  = cos(rotation_angle);

		TFixedVec3 eye_vector(_FXD(eye_x), _FXD(eye_y), _FXD(eye_z));
		TFixedVec3 at_vector (_FXD(0), _FXD(0), _FXD(200));
		TFixedVec3 up_vector (_FXD(up_x), _FXD(up_y), _FXD(0));

		avej::gfx3d::GetMatrixLookAt(camera_matrix, eye_vector, at_vector, up_vector);
	}

	void ApplyMatrix(void)
	{
		glMatrixMode(GL_MODELVIEW);

		if (m_has_pushed_matrix)
			glPopMatrix();

		glLoadMatrixx(camera_matrix);
		glPushMatrix();

		m_has_pushed_matrix = true;
	}

private:
	bool m_has_pushed_matrix;

	inline double m_ToRadian(float degree)
	{
		return degree * 3.141592 / 180.0;
	}
};

CScene::CCamera::CCamera()
	: m_p_impl(new TImpl)
{
	// Default 값. 위에서 내려다 보는 카메라
	m_p_impl->camera_desc.incident_angle = 90;
	m_p_impl->camera_desc.rotation_angle =  0;
	m_p_impl->camera_desc.distance       = 960;

	m_p_impl->MakeMatrix();
	m_p_impl->ApplyMatrix();
}

CScene::CCamera::~CCamera()
{
	delete m_p_impl;
}

CScene::CCamera& CScene::CCamera::operator<<(const TCameraDesc& camera_desc)
{
	m_p_impl->camera_desc = camera_desc;

	m_p_impl->MakeMatrix();
	m_p_impl->ApplyMatrix();

	return *this;
}

CScene::CCamera& CScene::CCamera::operator>>(TCameraDesc& camera_desc)
{
	camera_desc = m_p_impl->camera_desc;

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CScene::CProjection

struct CScene::CProjection::TImpl
{
	GLfixed x_radius;
	GLfixed y_radius;
	GLfixed z_near;
	GLfixed z_far;
	GLfixed default_scale;
	bool    use_perspective;

	TImpl()
		: x_radius(_FXD(1.5f)), y_radius(_FXD(1.0f)), z_near(_FXD(1.0f)), z_far(_FXD(10.0f)), default_scale(_FXD(1)), use_perspective(false) { }
};

CScene::CProjection::CProjection()
	: m_p_impl(new TImpl)
{
	Disable flag;
	this->operator<<(flag);
}

CScene::CProjection::~CProjection()
{
	delete m_p_impl;
}

CScene::CProjection& CScene::CProjection::operator<<(const CProjectionParam& param)
{
	m_p_impl->x_radius      = param.x_radius;
	m_p_impl->y_radius      = param.y_radius;
	m_p_impl->z_near        = param.z_near;
	m_p_impl->z_far         = param.z_far;
	m_p_impl->default_scale = param.default_scale;

	return *this;
}

CScene::CProjection& CScene::CProjection::operator<<(const Enable& flag)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustumx(-m_p_impl->x_radius, m_p_impl->x_radius, m_p_impl->y_radius, -m_p_impl->y_radius, m_p_impl->z_near, m_p_impl->z_far);

	m_p_impl->use_perspective = true;

	return *this;
}

CScene::CProjection& CScene::CProjection::operator<<(const Disable& flag)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthox(-m_p_impl->x_radius, m_p_impl->x_radius, m_p_impl->y_radius, -m_p_impl->y_radius, m_p_impl->z_near, m_p_impl->z_far);

	m_p_impl->use_perspective = false;

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CScene

struct CScene::TImpl
{
	CLight      light;
	CFog        fog;
	CCamera     camera;
	CProjection projection;

	TImpl()
	{
	}
};

CScene::CScene()
	: m_p_impl(new TImpl)
{
}
CScene::~CScene()
{
	delete m_p_impl;
}

CScene& CScene::operator<<(const Enable& flag)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
/* ?? 흠...
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
*/
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_ALPHA_TEST);
	glAlphaFuncx(GL_GREATER, _FXD(0.0f));

	return *this;
}

CScene& CScene::operator<<(const Disable& flag)
{
	return *this;
}

CScene& CScene::operator>>(TSceneDesc& scene_desc)
{
	const TFixed DEFAULT_SCALE_FIXED = m_p_impl->projection.m_p_impl->default_scale;

	if (m_p_impl->projection.m_p_impl->use_perspective)
	{
		scene_desc.z_push_fixed  = _FXD_mul(m_p_impl->projection.m_p_impl->z_near, -DEFAULT_SCALE_FIXED);
		scene_desc.z_push        = _FXD_toInt(scene_desc.z_push_fixed);
		scene_desc.default_scale = DEFAULT_SCALE_FIXED;
	}
	else
	{
		//??
		//scene_desc.z_push_fixed  = -_FXD(m_p_impl->projection.m_p_impl->z_near);
		scene_desc.z_push_fixed  = -(m_p_impl->projection.m_p_impl->z_near);
		scene_desc.z_push        =  _FXD_toInt(scene_desc.z_push_fixed);
		scene_desc.default_scale =  _FXD(1);
	}

	return *this;
}

CScene::CLight& CScene::Light(void)
{
	assert(m_p_impl);

	return m_p_impl->light;
}

CScene::CFog& CScene::Fog(void)
{
	assert(m_p_impl);

	return m_p_impl->fog;
}

CScene::CCamera& CScene::Camera(void)
{
	assert(m_p_impl);

	return m_p_impl->camera;
}

CScene::CProjection& CScene::Projection(void)
{
	assert(m_p_impl);

	return m_p_impl->projection;
}
