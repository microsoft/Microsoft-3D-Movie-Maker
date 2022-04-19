/***************************************************************************

	bren.h: Main include file for BRender files

	Primary Author: manders
	Review Status: REVIEWED - any changes to this file must be reviewed!

***************************************************************************/
#ifndef BREN_H
#define BREN_H

#include "kidframe.h"
#include "brender.h"

typedef br_actor BACT;
typedef br_model BMDL;
typedef br_light BLIT;
typedef br_camera BCAM;
typedef br_material BMTL;
typedef br_pixelmap BPMP;
typedef br_matrix34 BMAT34;
typedef br_bounds BRB;
typedef br_vertex BRV;
typedef br_face BRFC;
typedef br_face BRF;
typedef br_colour BRCLR;
typedef br_transform BRXFM;
typedef br_euler BREUL;
typedef br_matrix4 BMAT4;
typedef br_matrix23 BMAT23;

typedef BACT *PBACT;
typedef BMDL *PBMDL;
typedef BLIT *PBLIT;
typedef BCAM *PBCAM;
typedef BMTL *PBMTL;
typedef BPMP *PBPMP;
typedef BMAT34 *PBMAT34;
typedef BMAT4 *PBMAT4;
typedef BMAT23 *PBMAT23;

typedef br_scalar BRS;
typedef br_angle BRA;
typedef br_fraction BRFR;
typedef br_ufraction BRUFR;
typedef br_vector3 BVEC3;
typedef br_vector4 BVEC4;

#define BrsMac2 BR_MAC2
#define BrsSub BR_SUB
#define BrsMul BR_MUL
#define BrsAdd BR_ADD
#define BrsAbs BR_ABS
#define BrsRcp BR_RCP

const BRS rZero = BR_SCALAR(0.0);
const BRS rOne = BR_SCALAR(1.0);
const BRS rTwo = BR_SCALAR(2.0);
const BRS rFour = BR_SCALAR(4.0);
const BRS rOneHalf = BR_SCALAR(0.5);
const BRS rEps = BR_SCALAR_EPSILON;
const BRS rDivMin = 0x03;		// Smallest br_scalar for which BrsRcp() does not overflow
const BRS rFractMax = rOne - rEps;

const BRS krQuarter = BR_SCALAR(0.25);
const BRS krHalf = BR_SCALAR(0.5);
const BRS krPi = BR_SCALAR(PI);
const BRS krTwoPi = BR_SCALAR(2.0 * PI);
const BRS krThreePi = BR_SCALAR(3.0 * PI);
const BRS krHalfPi = BR_SCALAR(0.5 * PI);

const BVEC3 vec3X = { rOne, rZero, rZero };
const BVEC3 vec3Y = { rZero, rOne, rZero };
const BVEC3 vec3Z = { rZero, rZero, rOne };

const BRFR frNil = BR_FRACTION(0.0);
const BRA aNil = BR_ANGLE_DEG(0);
const BRA aZero = BR_ANGLE_DEG(0);

#define kctgZbmp 'ZBMP'

struct BCB // bounding cuboid...same shape as br_bounds
	{
	BRS xrMin;
	BRS yrMin;
	BRS zrMin;
	BRS xrMax;
	BRS yrMax;
	BRS zrMax;
	};
const BOM kbomBcb = 0xfff00000;

inline bool FBrEmptyBcb(BCB *pbcb)
	{
	return !(pbcb->xrMin || pbcb->yrMin || pbcb->zrMin || pbcb->xrMax ||
			pbcb->yrMax || pbcb->zrMax);
	}

const BOM kbomBrs = 0xc0000000; // br_scalar
const BOM kbomBrv = 0xffd50000; // br_vertex
const BOM kbomBrf = 0x555c15c0; // br_face
const BOM kbomBmat34 = 0xffffff00;

#if BASED_FIXED
inline BRS BrsHalf(BRS r)
	{ return r >> 1; }
inline BRFR ScalarToFraction(BRS r)
	{ return (br_fraction)((r >> 1) | ((r & 0x8000) >> 8)); }
inline BRS BrsAbsMax3(BRS r1, BRS r2, BRS r3)
	{ return (BRS)(LwMax(LwMax(LwAbs((long)r1), LwAbs((long)r2)),
	LwAbs((long)r3))); }
inline BRS BrsDiv(BRS r1, BRS r2) 	// Safety net: Prevent ovfl on division of integers
	{ if (r2 >= rOne || r2 < -rOne || LwAbs((long)r1) < LwAbs((long)r2))
		return BR_DIV(r1, r2);		// Most common case in SocRates (by far)
	  if (LwAbs((long)r2) < rDivMin)
		return ((r1 > 0 == r2 > 0) ? BR_SCALAR_MAX : BR_SCALAR_MIN);
	  BRS rRcp = BR_RCP(r2);
	  ulong lwT = (((ulong)BR_ABS(r1)) >> 16) * (((ulong)BR_ABS(rRcp)) >> 16);
	  if (lwT < 65536)
	  		return (BRS)BR_MUL(r1, rRcp);
	  return ((r1 > 0 == r2 > 0) ? BR_SCALAR_MAX : BR_SCALAR_MIN);
	}
#endif //BASED_FIXED

// fixed.h additions

// Round by adding half the desired rounding precision, and masking off the
// remaining precision
#define BR_ROUND(s,p) ((br_scalar)(BR_ADD((s), (0x01 << (p))) & ~((br_scalar)((0x01 << ((p)+1))-1))))

// colour.h additions

// Color conversion: straight RGB to RGB and RGB to grey
#define BR_DK_TO_BR(dk) BR_COLOUR_RGB((byte)((dk).r * 256.0), \
		(byte)((dk).g * 256.0), (byte)((dk).b * 256.0))
#define BR_DKRGB_TO_FRGRAY(dk) BrScalarToUFraction(BrFloatToScalar( \
		((dk).r * 0.30 + (dk).g * 0.59 + (dk).b * 0.11)))

// angles.h additions
// REVIEW peted: temporary BR_ACOS until we get a fixed Brender
#ifdef BR_ACOS
#undef BR_ACOS
#endif // BR_ACOS
#define BR_ACOS(a) BrRadianToAngle(BrFloatToScalar(acos(BrScalarToFloat(a))))


#include "zbmp.h"
#include "bwld.h"
#include "tmap.h"

#endif //!BREN_H
