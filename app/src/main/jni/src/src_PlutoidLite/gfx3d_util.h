
#ifndef __GFX3D_UTIL_H__
#define __GFX3D_UTIL_H__

namespace avej { namespace gfx3d
{
	void GetMatrixLookAt(TFixed* out_matrix_16, const TFixedVec3 &eye, const TFixedVec3 &at, const TFixedVec3 &up);
}}

#endif
