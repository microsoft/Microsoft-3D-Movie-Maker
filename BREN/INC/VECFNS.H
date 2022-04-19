/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: vecfns.h 1.11 1995/06/30 16:09:04 sam Exp $
 * $Locker:  $
 *
 * Inline versions of the vector operations (could do compiler specific
 * versions for compilers that support inline, ie: gcc)
 */

#ifndef _VECFNS_H_
#define _VECFNS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** 2D VECTORS
 **/

/*
 * v1=v2
 */
#define BrVector2Copy(v1, v2) do {\
	(v1)->v[0]=(v2)->v[0];\
	(v1)->v[1]=(v2)->v[1];\
} while(0)

/*
 * v1=(s1,s2)
 */
#define BrVector2Set(v1, s1, s2) do {\
	(v1)->v[0]=s1;\
	(v1)->v[1]=s2;\
} while(0)

#define BrVector2SetInt(v1, i1, i2) do {\
	(v1)->v[0]=BrIntToScalar(i1);\
	(v1)->v[1]=BrIntToScalar(i2);\
} while(0)

#define BrVector2SetFloat(v1, f1, f2) do {\
	(v1)->v[0]=BrFloatToScalar(f1);\
	(v1)->v[1]=BrFloatToScalar(f2);\
} while(0)

/*
 * v1=-v2
 */
#define BrVector2Negate(v1, v2) do {\
	(v1)->v[0]=-(v2)->v[0];\
	(v1)->v[1]=-(v2)->v[1];\
} while(0)


/*
 * v1=v2+v3
 */
#define BrVector2Add(v1, v2, v3) do {\
	(v1)->v[0]=(v2)->v[0]+(v3)->v[0];\
	(v1)->v[1]=(v2)->v[1]+(v3)->v[1];\
} while(0)

/*
 * v1+=v2
 */
#define BrVector2Accumulate(v1, v2) do {\
	(v1)->v[0]+=(v2)->v[0];\
	(v1)->v[1]+=(v2)->v[1];\
}while(0)

/*
 * v1+=v2*scalar
 */
#define BrVector2AccumulateScale(v1, v2, s) do {\
	(v1)->v[0]+=BR_MUL((v2)->v[0],(s));\
	(v1)->v[1]+=BR_MUL((v2)->v[1],(s));\
}while(0)

/*
 * v1=v2-v3
 */
#define BrVector2Sub(v1, v2, v3) do {\
	(v1)->v[0]=(v2)->v[0]-(v3)->v[0];\
	(v1)->v[1]=(v2)->v[1]-(v3)->v[1];\
}while(0)

/*
 * v1=v2*scalar
 */
#define BrVector2Scale(v1, v2, s) do {\
	(v1)->v[0]=BR_MUL((v2)->v[0],(s));\
	(v1)->v[1]=BR_MUL((v2)->v[1],(s));\
}while(0)

/*
 * v1=v2/scalar
 */
#define BrVector2InvScale(v1, v2, s) do {\
	(v1)->v[0]=BR_DIV((v2)->v[0],(s));\
	(v1)->v[1]=BR_DIV((v2)->v[1],(s));\
}while(0)

/*
 *  = v1.v2
 */
#define BrVector2Dot(v1, v2) BR_MAC2((v1)->v[0],(v2)->v[0],(v1)->v[1],(v2)->v[1])

/*
 *  = |v1|
 */
#define BrVector2Length(v1) BR_LENGTH2((v1)->v[0],(v1)->v[1])

/*
 *  = |v1| * |v1|
 */
#define BrVector2LengthSquared(v1) BR_SQR2((v1)->v[0],(v1)->v[1])



/**
 ** 3D VECTORS
 **/

/*
 * v1=v2
 */
#define BrVector3Copy(v1, v2) do {\
	(v1)->v[0]=(v2)->v[0];\
	(v1)->v[1]=(v2)->v[1];\
	(v1)->v[2]=(v2)->v[2];\
} while(0)

/*
 * v1=(s1,s2,s3)
 */
#define BrVector3Set(v1, s1, s2, s3) do {\
	(v1)->v[0]=(s1);\
	(v1)->v[1]=(s2);\
	(v1)->v[2]=(s3);\
} while(0)

#define BrVector3SetInt(v1, i1, i2, i3) do {\
	(v1)->v[0]=BrIntToScalar(i1);\
	(v1)->v[1]=BrIntToScalar(i2);\
	(v1)->v[2]=BrIntToScalar(i3);\
} while(0)

/*
 * v1=-v2
 */
#define BrVector3Negate(v1, v2) do {\
	(v1)->v[0]=- (v2)->v[0];\
	(v1)->v[1]=-(v2)->v[1];\
	(v1)->v[2]=-(v2)->v[2];\
} while(0)


/*
 * v1=v2+v3
 */
#define BrVector3Add(v1, v2, v3) do {\
	(v1)->v[0]=(v2)->v[0]+(v3)->v[0];\
	(v1)->v[1]=(v2)->v[1]+(v3)->v[1];\
	(v1)->v[2]=(v2)->v[2]+(v3)->v[2];\
} while(0)

/*
 * v1+=v2
 */
#define BrVector3Accumulate(v1, v2) do {\
	(v1)->v[0]+=(v2)->v[0];\
	(v1)->v[1]+=(v2)->v[1];\
	(v1)->v[2]+=(v2)->v[2];\
} while(0)

/*
 * v1+=v2*scalar
 */
#define BrVector3AccumulateScale(v1, v2, s) do {\
	(v1)->v[0]+=BR_MUL((v2)->v[0],(s));\
	(v1)->v[1]+=BR_MUL((v2)->v[1],(s));\
	(v1)->v[2]+=BR_MUL((v2)->v[2],(s));\
} while(0)

/*
 * v1=v2-v3
 */
#define BrVector3Sub(v1, v2, v3) do {\
	(v1)->v[0]=(v2)->v[0]-(v3)->v[0];\
	(v1)->v[1]=(v2)->v[1]-(v3)->v[1];\
	(v1)->v[2]=(v2)->v[2]-(v3)->v[2];\
} while(0)

/*
 * v1=v2*scalar
 */
#define BrVector3Scale(v1, v2, s) do {\
	(v1)->v[0]=BR_MUL((v2)->v[0],(s));\
	(v1)->v[1]=BR_MUL((v2)->v[1],(s));\
	(v1)->v[2]=BR_MUL((v2)->v[2],(s));\
} while(0)

/*
 * v1=v2/scalar
 */
#define BrVector3InvScale(v1, v2, s) do {\
	(v1)->v[0]=BR_DIV((v2)->v[0],(s));\
	(v1)->v[1]=BR_DIV((v2)->v[1],(s));\
	(v1)->v[2]=BR_DIV((v2)->v[2],(s));\
} while(0)

/*
 *  = v1.v2
 */
#define BrVector3Dot(v1, v2) BR_MAC3((v1)->v[0],(v2)->v[0],(v1)->v[1],(v2)->v[1],(v1)->v[2],(v2)->v[2])

/*
 * v1 = v2 x v3
 */
#define BrVector3Cross(v1,v2,v3) do {\
	(v1)->v[0]=BR_MUL((v2)->v[1],(v3)->v[2])-BR_MUL((v2)->v[2],(v3)->v[1]);\
	(v1)->v[1]=BR_MUL((v2)->v[2],(v3)->v[0])-BR_MUL((v2)->v[0],(v3)->v[2]);\
	(v1)->v[2]=BR_MUL((v2)->v[0],(v3)->v[1])-BR_MUL((v2)->v[1],(v3)->v[0]);\
} while(0)

/*
 *  = |v1|
 */
#define BrVector3Length(v1) BR_LENGTH3((v1)->v[0],(v1)->v[1],(v1)->v[2])

/*
 * = |v1| * |v1|
 */
#define BrVector3LengthSquared(v1) BR_SQR3((v1)->v[0],(v1)->v[1],(v1)->v[2])

/*
 * v1 = v2/|v2|
 */
#define BrVector3Normalise(v1,v2) do {\
	br_scalar _scale;\
	_scale = BR_LENGTH3((v2)->v[0],(v2)->v[1],(v2)->v[2]);\
	if(_scale > (BR_SCALAR_EPSILON*2)) {\
		_scale = BR_RCP(_scale);\
		(v1)->v[0]=BR_MUL((v2)->v[0],_scale);\
		(v1)->v[1]=BR_MUL((v2)->v[1],_scale);\
		(v1)->v[2]=BR_MUL((v2)->v[2],_scale);\
	} else {\
		(v1)->v[0]=BR_FRACTION(1.0);\
		(v1)->v[1]=BR_FRACTION(0.0);\
		(v1)->v[2]=BR_FRACTION(0.0);\
	}\
} while(0)

/*
 * v1 = v2/|v2|
 */
#define BrVector3NormaliseQuick(v1,v2) do {\
	br_scalar _scale;\
	_scale=BR_RCP(BR_LENGTH3((v2)->v[0],(v2)->v[1],(v2)->v[2]));\
	BrVector3Scale(v1,v2,_scale);\
} while(0)

/*
 * v1 = v2/|v2| (low precision)
 */
#define BrVector3NormaliseLP(v1,v2) do {\
	br_scalar _scale;\
	_scale=BR_RCP(BR_LENGTH3((v2)->v[0],(v2)->v[1],(v2)->v[2]));\
	BrVector3Scale(v1,v2,_scale);\
} while(0)

/*
 * 4D Vectors
 */
/*
 *  = v1.v2
 */
#define BrVector4Dot(v1, v2) BR_MAC4((v1)->v[0],(v2)->v[0],(v1)->v[1],(v2)->v[1],(v1)->v[2],(v2)->v[2],(v1)->v[3],(v2)->v[3])

/*
 * v1=v2
 */
#define BrVector4Copy(v1, v2) do {\
	(v1)->v[0]=(v2)->v[0];\
	(v1)->v[1]=(v2)->v[1];\
	(v1)->v[2]=(v2)->v[2];\
	(v1)->v[3]=(v2)->v[3];\
} while(0)

/*
 * v1 = v2/|v2|
 */
#define BrVector2Normalise(v1,v2) do {\
	br_scalar _scale;\
	_scale = BR_LENGTH2((v2)->v[0],(v2)->v[1]);\
	if(_scale > (BR_SCALAR_EPSILON*2)) {\
		_scale = BR_RCP(_scale);\
		(v1)->v[0]=BR_MUL((v2)->v[0],_scale);\
		(v1)->v[1]=BR_MUL((v2)->v[1],_scale);\
	} else {\
		(v1)->v[0]=BR_FRACTION(1.0);\
		(v1)->v[1]=BR_FRACTION(0.0);\
	}\
} while(0)

#ifdef __cplusplus
};
#endif
#endif

