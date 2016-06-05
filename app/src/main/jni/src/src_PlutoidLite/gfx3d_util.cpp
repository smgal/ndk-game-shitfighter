
/*
	아래의 부분은 PowerVR의 PVRTMatrixX.cpp에서 가져온 것이다.
*/

#include <math.h>

/* Define a 64-bit type for various platforms */
#if defined(__int64) || defined(WIN32)
#define PVR64BIT __int64
#elif defined(TInt64)
#define PVR64BIT TInt64
#else
#define PVR64BIT long long
#endif

/* Fixed-point macros */
#define PVRTF2X(f)		( (int) ( (f)*(65536) ) )
#define PVRTX2F(x)		((float)(x)/65536.0f)
#define PVRTXMUL(a,b)	( (int)( ((PVR64BIT)(a)*(b)) / 65536 ) )
#define PVRTXDIV(a,b)	( (int)( (((PVR64BIT)(a))<<16)/(b) ) )
#define PVRTABS(a)		((a) <= 0 ? -(a) : (a) )

typedef struct
{
	int f[16];	/*!< Array of int */
} PVRTMATRIXx;

typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
} PVRTVECTOR3x;

int PVRTMatrixVec3DotProductX(
	const PVRTVECTOR3x	&v1,
	const PVRTVECTOR3x	&v2)
{
	return (PVRTXMUL(v1.x, v2.x) + PVRTXMUL(v1.y, v2.y) + PVRTXMUL(v1.z, v2.z));
}

void PVRTMatrixVec3NormalizeX(
	PVRTVECTOR3x		&vOut,
	const PVRTVECTOR3x	&vIn)
{
	int				f, n;
	PVRTVECTOR3x	vTemp;

	/* Scale vector by uniform value */
	n = PVRTABS(vIn.x) + PVRTABS(vIn.y) + PVRTABS(vIn.z);
	vTemp.x = PVRTXDIV(vIn.x, n);
	vTemp.y = PVRTXDIV(vIn.y, n);
	vTemp.z = PVRTXDIV(vIn.z, n);

	/* Calculate x2+y2+z2/sqrt(x2+y2+z2) */
	f = PVRTMatrixVec3DotProductX(vTemp, vTemp);
	f = PVRTXDIV(PVRTF2X(1.0f), PVRTF2X(sqrt(PVRTX2F(f))));

	/* Multiply vector components by f */
	vOut.x = PVRTXMUL(vTemp.x, f);
	vOut.y = PVRTXMUL(vTemp.y, f);
	vOut.z = PVRTXMUL(vTemp.z, f);
}

void PVRTMatrixVec3CrossProductX(
	PVRTVECTOR3x		&vOut,
	const PVRTVECTOR3x	&v1,
	const PVRTVECTOR3x	&v2)
{
	PVRTVECTOR3x result;

	/* Perform calculation on a dummy VECTOR (result) */
    result.x = PVRTXMUL(v1.y, v2.z) - PVRTXMUL(v1.z, v2.y);
    result.y = PVRTXMUL(v1.z, v2.x) - PVRTXMUL(v1.x, v2.z);
    result.z = PVRTXMUL(v1.x, v2.y) - PVRTXMUL(v1.y, v2.x);

	/* Copy result in pOut */
	vOut = result;
}

void PVRTMatrixTranslationX(
	PVRTMATRIXx	&mOut,
	const int	fX,
	const int	fY,
	const int	fZ)
{
	mOut.f[ 0]=PVRTF2X(1.0f);	mOut.f[ 4]=PVRTF2X(0.0f);	mOut.f[ 8]=PVRTF2X(0.0f);	mOut.f[12]=fX;
	mOut.f[ 1]=PVRTF2X(0.0f);	mOut.f[ 5]=PVRTF2X(1.0f);	mOut.f[ 9]=PVRTF2X(0.0f);	mOut.f[13]=fY;
	mOut.f[ 2]=PVRTF2X(0.0f);	mOut.f[ 6]=PVRTF2X(0.0f);	mOut.f[10]=PVRTF2X(1.0f);	mOut.f[14]=fZ;
	mOut.f[ 3]=PVRTF2X(0.0f);	mOut.f[ 7]=PVRTF2X(0.0f);	mOut.f[11]=PVRTF2X(0.0f);	mOut.f[15]=PVRTF2X(1.0f);
}

void PVRTMatrixMultiplyX(
	PVRTMATRIXx			&mOut,
	const PVRTMATRIXx	&mA,
	const PVRTMATRIXx	&mB)
{
	PVRTMATRIXx mRet;

	/* Perform calculation on a dummy matrix (mRet) */
	mRet.f[ 0] = PVRTXMUL(mA.f[ 0], mB.f[ 0]) + PVRTXMUL(mA.f[ 1], mB.f[ 4]) + PVRTXMUL(mA.f[ 2], mB.f[ 8]) + PVRTXMUL(mA.f[ 3], mB.f[12]);
	mRet.f[ 1] = PVRTXMUL(mA.f[ 0], mB.f[ 1]) + PVRTXMUL(mA.f[ 1], mB.f[ 5]) + PVRTXMUL(mA.f[ 2], mB.f[ 9]) + PVRTXMUL(mA.f[ 3], mB.f[13]);
	mRet.f[ 2] = PVRTXMUL(mA.f[ 0], mB.f[ 2]) + PVRTXMUL(mA.f[ 1], mB.f[ 6]) + PVRTXMUL(mA.f[ 2], mB.f[10]) + PVRTXMUL(mA.f[ 3], mB.f[14]);
	mRet.f[ 3] = PVRTXMUL(mA.f[ 0], mB.f[ 3]) + PVRTXMUL(mA.f[ 1], mB.f[ 7]) + PVRTXMUL(mA.f[ 2], mB.f[11]) + PVRTXMUL(mA.f[ 3], mB.f[15]);

	mRet.f[ 4] = PVRTXMUL(mA.f[ 4], mB.f[ 0]) + PVRTXMUL(mA.f[ 5], mB.f[ 4]) + PVRTXMUL(mA.f[ 6], mB.f[ 8]) + PVRTXMUL(mA.f[ 7], mB.f[12]);
	mRet.f[ 5] = PVRTXMUL(mA.f[ 4], mB.f[ 1]) + PVRTXMUL(mA.f[ 5], mB.f[ 5]) + PVRTXMUL(mA.f[ 6], mB.f[ 9]) + PVRTXMUL(mA.f[ 7], mB.f[13]);
	mRet.f[ 6] = PVRTXMUL(mA.f[ 4], mB.f[ 2]) + PVRTXMUL(mA.f[ 5], mB.f[ 6]) + PVRTXMUL(mA.f[ 6], mB.f[10]) + PVRTXMUL(mA.f[ 7], mB.f[14]);
	mRet.f[ 7] = PVRTXMUL(mA.f[ 4], mB.f[ 3]) + PVRTXMUL(mA.f[ 5], mB.f[ 7]) + PVRTXMUL(mA.f[ 6], mB.f[11]) + PVRTXMUL(mA.f[ 7], mB.f[15]);

	mRet.f[ 8] = PVRTXMUL(mA.f[ 8], mB.f[ 0]) + PVRTXMUL(mA.f[ 9], mB.f[ 4]) + PVRTXMUL(mA.f[10], mB.f[ 8]) + PVRTXMUL(mA.f[11], mB.f[12]);
	mRet.f[ 9] = PVRTXMUL(mA.f[ 8], mB.f[ 1]) + PVRTXMUL(mA.f[ 9], mB.f[ 5]) + PVRTXMUL(mA.f[10], mB.f[ 9]) + PVRTXMUL(mA.f[11], mB.f[13]);
	mRet.f[10] = PVRTXMUL(mA.f[ 8], mB.f[ 2]) + PVRTXMUL(mA.f[ 9], mB.f[ 6]) + PVRTXMUL(mA.f[10], mB.f[10]) + PVRTXMUL(mA.f[11], mB.f[14]);
	mRet.f[11] = PVRTXMUL(mA.f[ 8], mB.f[ 3]) + PVRTXMUL(mA.f[ 9], mB.f[ 7]) + PVRTXMUL(mA.f[10], mB.f[11]) + PVRTXMUL(mA.f[11], mB.f[15]);

	mRet.f[12] = PVRTXMUL(mA.f[12], mB.f[ 0]) + PVRTXMUL(mA.f[13], mB.f[ 4]) + PVRTXMUL(mA.f[14], mB.f[ 8]) + PVRTXMUL(mA.f[15], mB.f[12]);
	mRet.f[13] = PVRTXMUL(mA.f[12], mB.f[ 1]) + PVRTXMUL(mA.f[13], mB.f[ 5]) + PVRTXMUL(mA.f[14], mB.f[ 9]) + PVRTXMUL(mA.f[15], mB.f[13]);
	mRet.f[14] = PVRTXMUL(mA.f[12], mB.f[ 2]) + PVRTXMUL(mA.f[13], mB.f[ 6]) + PVRTXMUL(mA.f[14], mB.f[10]) + PVRTXMUL(mA.f[15], mB.f[14]);
	mRet.f[15] = PVRTXMUL(mA.f[12], mB.f[ 3]) + PVRTXMUL(mA.f[13], mB.f[ 7]) + PVRTXMUL(mA.f[14], mB.f[11]) + PVRTXMUL(mA.f[15], mB.f[15]);

	/* Copy result in pResultMatrix */
	mOut = mRet;
}

void PVRTMatrixLookAtLHX(
	PVRTMATRIXx			&mOut,
	const PVRTVECTOR3x	&vEye,
	const PVRTVECTOR3x	&vAt,
	const PVRTVECTOR3x	&vUp)
{
	PVRTVECTOR3x	f, vUpActual, s, u;
	PVRTMATRIXx		t;

	f.x = vEye.x - vAt.x;
	f.y = vEye.y - vAt.y;
	f.z = vEye.z - vAt.z;

	PVRTMatrixVec3NormalizeX(f, f);
	PVRTMatrixVec3NormalizeX(vUpActual, vUp);
	PVRTMatrixVec3CrossProductX(s, f, vUpActual);
	PVRTMatrixVec3CrossProductX(u, s, f);

	mOut.f[ 0] = s.x;
	mOut.f[ 1] = u.x;
	mOut.f[ 2] = -f.x;
	mOut.f[ 3] = PVRTF2X(0.0f);

	mOut.f[ 4] = s.y;
	mOut.f[ 5] = u.y;
	mOut.f[ 6] = -f.y;
	mOut.f[ 7] = PVRTF2X(0.0f);

	mOut.f[ 8] = s.z;
	mOut.f[ 9] = u.z;
	mOut.f[10] = -f.z;
	mOut.f[11] = PVRTF2X(0.0f);

	mOut.f[12] = PVRTF2X(0.0f);
	mOut.f[13] = PVRTF2X(0.0f);
	mOut.f[14] = PVRTF2X(0.0f);
	mOut.f[15] = PVRTF2X(1.0f);

	PVRTMatrixTranslationX(t, -vEye.x, -vEye.y, -vEye.z);
	PVRTMatrixMultiplyX(mOut, t, mOut);
}

void PVRTMatrixLookAtRHX(
	PVRTMATRIXx			&mOut,
	const PVRTVECTOR3x	&vEye,
	const PVRTVECTOR3x	&vAt,
	const PVRTVECTOR3x	&vUp)
{
	PVRTVECTOR3x	f, vUpActual, s, u;
	PVRTMATRIXx		t;

	f.x = vAt.x - vEye.x;
	f.y = vAt.y - vEye.y;
	f.z = vAt.z - vEye.z;

	PVRTMatrixVec3NormalizeX(f, f);
	PVRTMatrixVec3NormalizeX(vUpActual, vUp);
	PVRTMatrixVec3CrossProductX(s, f, vUpActual);
	PVRTMatrixVec3CrossProductX(u, s, f);

	mOut.f[ 0] = s.x;
	mOut.f[ 1] = u.x;
	mOut.f[ 2] = -f.x;
	mOut.f[ 3] = PVRTF2X(0.0f);

	mOut.f[ 4] = s.y;
	mOut.f[ 5] = u.y;
	mOut.f[ 6] = -f.y;
	mOut.f[ 7] = PVRTF2X(0.0f);

	mOut.f[ 8] = s.z;
	mOut.f[ 9] = u.z;
	mOut.f[10] = -f.z;
	mOut.f[11] = PVRTF2X(0.0f);

	mOut.f[12] = PVRTF2X(0.0f);
	mOut.f[13] = PVRTF2X(0.0f);
	mOut.f[14] = PVRTF2X(0.0f);
	mOut.f[15] = PVRTF2X(1.0f);

	PVRTMatrixTranslationX(t, -vEye.x, -vEye.y, -vEye.z);
	PVRTMatrixMultiplyX(mOut, t, mOut);
}

#include "gfx3d_type.h"

#define TO_PVRTVECTOR3x(val) *((PVRTVECTOR3x*)&val)

namespace avej { namespace gfx3d
{
	void GetMatrixLookAt(TFixed* out_matrix_16, const TFixedVec3 &eye, const TFixedVec3 &at, const TFixedVec3 &up)
	{
		PVRTMatrixLookAtRHX(*((PVRTMATRIXx*)out_matrix_16), TO_PVRTVECTOR3x(eye), TO_PVRTVECTOR3x(at), TO_PVRTVECTOR3x(up));
	}
}}
