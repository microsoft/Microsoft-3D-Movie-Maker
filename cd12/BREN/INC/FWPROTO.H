/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fwproto.h 1.42 1995/06/30 16:08:49 sam Exp $
 * $Locker: sam $
 *
 * Function prototypes for brender framework
 */
#ifndef _FWPROTO_H_
#define _FWPROTO_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NO_PROTOTYPES
/**
 ** Fixed Point
 **/

/* result = abs(a)
 */
br_fixed_ls BR_ASM_CALL BrFixedAbs(br_fixed_ls a);

/* result = a*b
 */
br_fixed_ls BR_ASM_CALL BrFixedMul(br_fixed_ls a, br_fixed_ls b);

/* result = a*b + c*d
 */
br_fixed_ls BR_ASM_CALL BrFixedMac2(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d);

/* result = a*b + c*d + e*f
 */
br_fixed_ls BR_ASM_CALL BrFixedMac3(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f);

/* result = a*b + c*d + e*f + g*h
 */
br_fixed_ls BR_ASM_CALL BrFixedMac4(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h);

/* result = a*a
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr(br_fixed_ls a);

/* result = a*a + b*b
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr2(br_fixed_ls a,
									br_fixed_ls b);

/* result = a*a + b*b + c*c
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr3(br_fixed_ls a,
									br_fixed_ls b,
									br_fixed_ls c);
/* result = a*a + b*b + c*c + d*d
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr4(br_fixed_ls a,
									br_fixed_ls b,
									br_fixed_ls c,
									br_fixed_ls d);


/* result = sqrt(a*a + b*b)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength2(br_fixed_ls a, br_fixed_ls b);

/* result = sqrt(a*a + b*b + c*c)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength3(br_fixed_ls a,
									   br_fixed_ls b,
									   br_fixed_ls c);

/* result = sqrt(a*a + b*b + c*c + d*d)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength4(br_fixed_ls a,
									   br_fixed_ls b,
									   br_fixed_ls c,
									   br_fixed_ls d);

/* result = 1/sqrt(a*a + b*b) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength2(br_fixed_ls a, br_fixed_ls b);

/* result = 1/sqrt(a*a + b*b + c*c) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength3(br_fixed_ls a,
										br_fixed_ls b,
										br_fixed_ls c);

/* result = 1/sqrt(a*a + b*b + c*c + d*d) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength4(br_fixed_ls a,
										br_fixed_ls b,
										br_fixed_ls c,
										br_fixed_ls d);			

/* result = a/b
 */
br_fixed_ls BR_ASM_CALL BrFixedDiv(br_fixed_ls a, br_fixed_ls b);

/* result = a/b * 2^31
 */
br_fixed_ls BR_ASM_CALL BrFixedDivF(br_fixed_ls a, br_fixed_ls b);

/* result = a/b (rounded towards 0)
 */
br_fixed_ls BR_ASM_CALL BrFixedDivR(br_fixed_ls a, br_fixed_ls b);


/* result = a*b/c
 */
br_fixed_ls BR_ASM_CALL BrFixedMulDiv(br_fixed_ls a,
									  br_fixed_ls b,
									  br_fixed_ls c);

/* result = (a*b + c*d)/e
 */
br_fixed_ls BR_ASM_CALL BrFixedMac2Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e);

/* result = (a*b + c*d + e*f)/g
 */
br_fixed_ls BR_ASM_CALL BrFixedMac3Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g);

/* result = (a*b + c*d + e*f + g*h)/i
 */
br_fixed_ls BR_ASM_CALL BrFixedMac4Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h,
									br_fixed_ls i);
/* result = 1.0/a
 */
br_fixed_ls BR_ASM_CALL BrFixedRcp(br_fixed_ls a);

/*
 * Various combinations whith fractions
 */

/* result = a*b + c*d - a & c are fractions
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac2(br_fixed_lsf a, br_fixed_ls b,
									 br_fixed_lsf c, br_fixed_ls d);

/* result = a*b + c*d + e*f - a,c & e are fractions
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac3(br_fixed_lsf a, br_fixed_ls b,
									 br_fixed_lsf c, br_fixed_ls d,
									 br_fixed_lsf e, br_fixed_ls f);

/* result = a*b + c*d + e*f + g*h (a,c,e,g are fractions)
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac4(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h);

/*
 * Misc. support  functions
 */
br_fixed_ls BR_ASM_CALL BrFixedSin(br_angle a);

br_fixed_ls BR_ASM_CALL BrFixedCos(br_angle a);

br_angle BR_ASM_CALL BrFixedASin(br_fixed_ls s);

br_angle BR_ASM_CALL BrFixedACos(br_fixed_ls c);

br_angle BR_ASM_CALL BrFixedATan2(br_fixed_ls x, br_fixed_ls y);

br_angle BR_ASM_CALL BrFixedATan2Fast(br_fixed_ls x, br_fixed_ls y);

br_fixed_ls BR_PUBLIC_ENTRY BrFixedSqrt(br_fixed_ls a);
br_fixed_ls BR_PUBLIC_ENTRY BrFixedPow(br_fixed_ls a,br_fixed_ls b);

/*
 * Integer sqrt functions
 */
br_uint_32 BR_ASM_CALL BrSqrt(br_uint_32 a);
br_uint_32 BR_ASM_CALL BrFastSqrt(br_uint_32 a);
br_uint_32 BR_ASM_CALL BrFastRSqrt(br_uint_32 a);

/*
 * Optional inline fixed point maths
 */
#ifndef NO_PROTOTYPES
#if INLINE_FIXED

#ifdef __GNUC__
#include "g386ifix.h"
#endif

#ifdef __WATCOMC__
#include "wtcifix.h"
#endif

#ifdef _MSC_VER
#include "mscifix.h"
#endif

#endif
#endif

/*
 * Setup
 */
void BR_PUBLIC_ENTRY BrBegin(void);
void BR_PUBLIC_ENTRY BrEnd(void);

/*
 * XXX - All the Add/Remove/Find/Count/Enum calls could ( when !(DEBUG | PARANOID))
 * be #defined in terms of the core Registry fns. with appropriate casts.
 */
/*
 * Material Handling
 */
br_material * BR_PUBLIC_ENTRY BrMaterialAdd(br_material *material);
br_material * BR_PUBLIC_ENTRY BrMaterialRemove(br_material *material);
br_material * BR_PUBLIC_ENTRY BrMaterialFind(char *pattern);
typedef br_material * BR_CALLBACK br_material_find_cbfn(char *name);
br_material_find_cbfn * BR_PUBLIC_ENTRY BrMaterialFindHook(br_material_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialAddMany(br_material **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialRemoveMany(br_material **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialFindMany(char *pattern, br_material **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_material_enum_cbfn(br_material *item, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialEnum(char *pattern,
		br_material_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrMaterialUpdate(br_material *material, br_uint_16 flags);

br_material * BR_PUBLIC_ENTRY BrMaterialAllocate(char *name);
void BR_PUBLIC_ENTRY BrMaterialFree(br_material *m);

/*
 * Model Handling
 */
br_model * BR_PUBLIC_ENTRY BrModelAdd(br_model *model);
br_model * BR_PUBLIC_ENTRY BrModelRemove(br_model *model);
br_model * BR_PUBLIC_ENTRY BrModelFind(char *pattern);
typedef br_model * BR_CALLBACK br_model_find_cbfn(char *name);
br_model_find_cbfn * BR_PUBLIC_ENTRY BrModelFindHook(br_model_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrModelAddMany(br_model **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrModelRemoveMany(br_model **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrModelFindMany(char *pattern, br_model **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrModelCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_model_enum_cbfn(br_model *item, void *arg);

br_uint_32 BR_PUBLIC_ENTRY BrModelEnum(char *pattern,
		br_model_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrModelUpdate(br_model *model, br_uint_16 flags);
void BR_PUBLIC_ENTRY BrModelApplyMap(br_model *model,int map_type, br_matrix34 *xform);
br_matrix34 * BR_PUBLIC_ENTRY BrModelFitMap(br_model *model, int axis_0, int axis_1, br_matrix34 *transform);

br_model * BR_PUBLIC_ENTRY BrModelAllocate(char *name, int nvertices, int nfaces);
void BR_PUBLIC_ENTRY BrModelFree(br_model *m);

/*
 * Texture handling
 */
br_pixelmap * BR_PUBLIC_ENTRY BrMapAdd(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrMapRemove(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrMapFind(char *pattern);
typedef br_pixelmap * BR_CALLBACK br_map_find_cbfn(char *name);
br_map_find_cbfn * BR_PUBLIC_ENTRY BrMapFindHook(br_map_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrMapAddMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMapRemoveMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMapFindMany(char *pattern, br_pixelmap **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrMapCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_map_enum_cbfn(br_pixelmap *item, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrMapEnum(char *pattern,
		br_map_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrMapUpdate(br_pixelmap *item, br_uint_16 flags);

/*
 * Table handling
 */
br_pixelmap * BR_PUBLIC_ENTRY BrTableAdd(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrTableRemove(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrTableFind(char *pattern);
typedef br_pixelmap * BR_CALLBACK br_table_find_cbfn(char *name);
br_table_find_cbfn *BR_PUBLIC_ENTRY BrTableFindHook(br_table_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrTableAddMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrTableRemoveMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrTableFindMany(char *pattern, br_pixelmap **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrTableCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_table_enum_cbfn(br_pixelmap *item, void *arg);

br_uint_32 BR_PUBLIC_ENTRY BrTableEnum(char *pattern,
		br_table_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrTableUpdate(br_pixelmap *item, br_uint_16 flags);

/*
 * Resource class handling
 */
br_resource_class * BR_PUBLIC_ENTRY BrResClassAdd(br_resource_class *pixelmap);
br_resource_class * BR_PUBLIC_ENTRY BrResClassRemove(br_resource_class *pixelmap);
br_resource_class * BR_PUBLIC_ENTRY BrResClassFind(char *pattern);
typedef br_resource_class * BR_CALLBACK br_resclass_find_cbfn(char *name);
br_resclass_find_cbfn *BR_PUBLIC_ENTRY BrResClassFindHook(br_resclass_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrResClassAddMany(br_resource_class **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrResClassRemoveMany(br_resource_class **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrResClassFindMany(char *pattern, br_resource_class **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrResClassCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_resclass_enum_cbfn(br_resource_class *item, void *arg);

br_uint_32 BR_PUBLIC_ENTRY BrResClassEnum(char *pattern,
		br_resclass_enum_cbfn *callback, void *arg);

/*
 * Actor Handling
 */
typedef br_uint_32 BR_CALLBACK br_actor_enum_cbfn(br_actor *mat, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrActorEnum(br_actor *parent, br_actor_enum_cbfn *callback , void *arg);

br_actor * BR_PUBLIC_ENTRY BrActorAdd(br_actor *parent, br_actor *a);
br_actor * BR_PUBLIC_ENTRY BrActorRemove(br_actor *a);
void BR_PUBLIC_ENTRY BrActorRelink(br_actor *parent,br_actor *actor);
br_uint_8 BR_PUBLIC_ENTRY BrActorToActorMatrix34(br_matrix34 *m, br_actor *a, br_actor *b);
void BR_PUBLIC_ENTRY BrActorToScreenMatrix4(br_matrix4 *m, br_actor *a, br_actor *camera);

br_actor * BR_PUBLIC_ENTRY BrActorAllocate(br_uint_8 actor_type, void * type_data);
void BR_PUBLIC_ENTRY BrActorFree(br_actor *a);

br_uint_32 BR_PUBLIC_ENTRY BrActorSearchMany(br_actor *root, char *pattern, br_actor **actors, int max);
br_actor * BR_PUBLIC_ENTRY BrActorSearch(br_actor *root, char *pattern);

br_bounds * BR_PUBLIC_ENTRY BrActorToBounds( br_bounds *b, br_actor *ap);
br_matrix34 * BR_PUBLIC_ENTRY BrBoundsToMatrix34( br_matrix34 *mat, br_bounds *bounds);

/*
 * Pixelmap management
 */
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapAllocate(br_uint_8 type,br_uint_16 w,br_uint_16 h, void *pixels, int flags);
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapAllocateSub(br_pixelmap *pm,
								   br_uint_16 x, br_uint_16 y,
								   br_uint_16 w, br_uint_16 h);
void BR_PUBLIC_ENTRY BrPixelmapFree(br_pixelmap *pm);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatch(br_pixelmap *src, br_uint_8 match_type);
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapClone(br_pixelmap *src);

br_uint_16 BR_PUBLIC_ENTRY BrPixelmapPixelSize(br_pixelmap *pm);
br_uint_16 BR_PUBLIC_ENTRY BrPixelmapChannels(br_pixelmap *pm);

/*
 * Pixelmap operations
 */
void BR_PUBLIC_ENTRY BrPixelmapFill(br_pixelmap *dst, br_uint_32 colour);

void BR_PUBLIC_ENTRY BrPixelmapRectangle(br_pixelmap *dst,br_int_16 x,br_int_16 y,br_uint_16 w,br_uint_16 h,br_uint_32 colour);
void BR_PUBLIC_ENTRY BrPixelmapRectangle2(br_pixelmap *dst,br_int_16 x,br_int_16 y,br_uint_16 w,br_uint_16 h,br_uint_32 colour1, br_uint_32 colour2);
void BR_PUBLIC_ENTRY BrPixelmapRectangleCopy(br_pixelmap *dst,br_int_16 dx,br_int_16 dy,br_pixelmap *src,br_int_16 sx,br_int_16 sy,br_uint_16 w,br_uint_16 h);
void BR_PUBLIC_ENTRY BrPixelmapRectangleFill(br_pixelmap *dst,br_int_16 x,br_int_16 y,br_uint_16 w,br_uint_16 h,br_uint_32 colour);

void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleCopy(br_pixelmap *dst,br_pixelmap *src,br_int_16 x,br_int_16 y,br_uint_16 w,br_uint_16 h);
void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleFill(br_pixelmap *dst,br_int_16 x,br_int_16 y,br_uint_16 w,br_uint_16 h,br_uint_32 colour);

void BR_PUBLIC_ENTRY BrPixelmapPixelSet(br_pixelmap *dst,br_int_16 x,br_int_16 y,br_uint_32 colour);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapPixelGet(br_pixelmap *dst,br_int_16 x,br_int_16 y);
void BR_PUBLIC_ENTRY BrPixelmapCopy(br_pixelmap *dst,br_pixelmap *src);
void BR_PUBLIC_ENTRY BrPixelmapLine(br_pixelmap *dst,br_int_16 x1, br_int_16 y1, br_int_16 x2, br_int_16 y2, br_uint_32 colour);
void BR_PUBLIC_ENTRY BrPixelmapText(br_pixelmap *dst, br_int_16 x, br_int_16 y,br_uint_32 colour, br_font *font, char *text);
void BR_PUBLIC_ENTRY BrPixelmapTextF(br_pixelmap *dst,br_int_16 x, br_int_16 y,br_uint_32 colour, br_font *font, char *fmt,...);

void BR_PUBLIC_ENTRY BrPixelmapCopyBits(br_pixelmap *dst,
	br_int_16 x,br_int_16 y,
	br_uint_8 *src,br_uint_16 s_stride,
	br_uint_16 start_bit,br_uint_16 end_bit,
	br_uint_16 nrows,br_uint_32 colour);

br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextWidth(br_pixelmap *dst, br_font *font, char *text);
br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextHeight(br_pixelmap *dst, br_font *font);

void BR_PUBLIC_ENTRY BrPixelmapDoubleBuffer(br_pixelmap *dst,br_pixelmap *src);

/*
 * Backwards compatibility
 */
#define BrPixelmapPlot BrPixelmapPixelSet

/*
 * File operations
 */
br_model * BR_PUBLIC_ENTRY BrModelLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrModelSave(char *filename,br_model *model);
br_uint_32 BR_PUBLIC_ENTRY BrModelLoadMany(char *filename,br_model **models,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrModelSaveMany(char *filename,br_model **models,br_uint_16 num);

br_material * BR_PUBLIC_ENTRY BrMaterialLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialSave(char *filename,br_material *material);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialLoadMany(char *filename,br_material **materials,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialSaveMany(char *filename,br_material **materials,br_uint_16 num);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapSave(char *filename,br_pixelmap *pixelmap);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapLoadMany(char *filename,br_pixelmap **pixelmaps,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapSaveMany(char *filename,br_pixelmap **pixelmaps,br_uint_16 num);

br_actor * BR_PUBLIC_ENTRY BrActorLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrActorSave(char *filename, br_actor *actor);
br_uint_32 BR_PUBLIC_ENTRY BrActorLoadMany(char *filename,br_actor **actors,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrActorSaveMany(char *filename,br_actor **actors,br_uint_16 num);

/*
 * Lights
 */
void BR_PUBLIC_ENTRY BrLightEnable(br_actor *l);
void BR_PUBLIC_ENTRY BrLightDisable(br_actor *l);

/*
 * Environment
 */
br_actor * BR_PUBLIC_ENTRY BrEnvironmentSet(br_actor *a);

/*
 * Clip planes
 */
void BR_PUBLIC_ENTRY BrClipPlaneEnable(br_actor *cp);
void BR_PUBLIC_ENTRY BrClipPlaneDisable(br_actor *cp);

/*
 * 3x4 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix34Copy(br_matrix34 *A, br_matrix34 *b);
void BR_PUBLIC_ENTRY BrMatrix34Mul(br_matrix34 *A, br_matrix34 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34Pre(br_matrix34 *mat , br_matrix34 *A);
void BR_PUBLIC_ENTRY BrMatrix34Post(br_matrix34 *mat , br_matrix34 *A);

void BR_PUBLIC_ENTRY BrMatrix34Identity(br_matrix34 *mat);

void BR_PUBLIC_ENTRY BrMatrix34RotateX(br_matrix34 *mat, br_angle rx);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateX(br_matrix34 *mat, br_angle rx);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateX(br_matrix34 *mat, br_angle rx);

void BR_PUBLIC_ENTRY BrMatrix34RotateY(br_matrix34 *mat, br_angle ry);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateY(br_matrix34 *mat, br_angle ry);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateY(br_matrix34 *mat, br_angle ry);

void BR_PUBLIC_ENTRY BrMatrix34RotateZ(br_matrix34 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateZ(br_matrix34 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateZ(br_matrix34 *mat, br_angle rz);

void BR_PUBLIC_ENTRY BrMatrix34Rotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);
void BR_PUBLIC_ENTRY BrMatrix34PreRotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);
void BR_PUBLIC_ENTRY BrMatrix34PostRotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);

void BR_PUBLIC_ENTRY BrMatrix34Translate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);
void BR_PUBLIC_ENTRY BrMatrix34PreTranslate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);
void BR_PUBLIC_ENTRY BrMatrix34PostTranslate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);

void BR_PUBLIC_ENTRY BrMatrix34Scale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PostScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PostShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PostShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix34PreShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix34PostShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix34ApplyV(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34ApplyP(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34Apply(br_vector3 *A,  br_vector4 *B, br_matrix34 *C);

void BR_PUBLIC_ENTRY BrMatrix34TApplyV(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34TApplyP(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34TApply(br_vector4 *A, br_vector4 *B, br_matrix34 *C);

br_scalar BR_PUBLIC_ENTRY BrMatrix34Inverse(br_matrix34 *out, br_matrix34 *in);
void BR_PUBLIC_ENTRY BrMatrix34LPInverse(br_matrix34 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix34LPNormalise(br_matrix34 *A, br_matrix34 *B);

void BR_PUBLIC_ENTRY BrMatrix34RollingBall(br_matrix34 *mat, int dx,int dy, int radius);

/*
 * 4x4 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix4Copy(br_matrix4 *A, br_matrix4 *B);
void BR_PUBLIC_ENTRY BrMatrix4Mul(br_matrix4 *A, br_matrix4 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4Identity(br_matrix4 *mat);
void BR_PUBLIC_ENTRY BrMatrix4Scale(br_matrix4 *mat, br_scalar sx, br_scalar sy, br_scalar sz);

br_scalar BR_PUBLIC_ENTRY BrMatrix4Inverse(br_matrix4 *A, br_matrix4 *B);
void BR_PUBLIC_ENTRY BrMatrix4Adjoint(br_matrix4 *A, br_matrix4 *B);
br_scalar BR_PUBLIC_ENTRY BrMatrix4Determinant(br_matrix4 *mat);

void BR_PUBLIC_ENTRY BrMatrix4Perspective(br_matrix4 *mat, 
		br_angle field_of_view,
		br_scalar aspect,
		br_scalar hither,
		br_scalar yon);

void BR_PUBLIC_ENTRY BrMatrix4ApplyV(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4ApplyP(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4Apply(br_vector4 *A,  br_vector4 *B, br_matrix4 *C);

void BR_PUBLIC_ENTRY BrMatrix4TApplyV(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4TApplyP(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4TApply(br_vector4 *A,  br_vector4 *B, br_matrix4 *C);

void BR_PUBLIC_ENTRY BrMatrix4Pre34(br_matrix4 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix4Copy34(br_matrix4 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix34Copy4(br_matrix34 *A, br_matrix4 *B);

/**
 ** 2D Vectors
 **/
void BR_PUBLIC_ENTRY BrVector2Copy(br_vector2 *v1,br_vector2 * v2);
void BR_PUBLIC_ENTRY BrVector2Set(br_vector2 *v1, br_scalar s1, br_scalar s2);
void BR_PUBLIC_ENTRY BrVector2SetInt(br_vector2 *v1, int i1, int i2);
void BR_PUBLIC_ENTRY BrVector2SetFloat(br_vector2 *v1, float f1, float f2);
void BR_PUBLIC_ENTRY BrVector2Negate(br_vector2 *v1, br_vector2 *v2);
void BR_PUBLIC_ENTRY BrVector2Add(br_vector2 *v1, br_vector2 *v2, br_vector2 *v3);
void BR_PUBLIC_ENTRY BrVector2Accumulate(br_vector2 *v1, br_vector2 *v2);
void BR_PUBLIC_ENTRY BrVector2Sub(br_vector2 *v1, br_vector2 *v2, br_vector2 *v3);
void BR_PUBLIC_ENTRY BrVector2Scale(br_vector2 *v1, br_vector2 *v2, br_scalar s);
void BR_PUBLIC_ENTRY BrVector2InvScale(br_vector2 *v1, br_vector2 *v2, br_scalar s);
br_scalar BR_PUBLIC_ENTRY BrVector2Dot(br_vector2 *v1, br_vector2 *v2);
br_scalar BR_PUBLIC_ENTRY BrVector2Length(br_vector2 *v1);
br_scalar BR_PUBLIC_ENTRY BrVector2LengthSquared(br_vector2 *v1);

/**
 ** 3D VECTORS
 **/
void BR_PUBLIC_ENTRY BrVector3Copy(br_vector3 *v1, br_vector2 *v2);
void BR_PUBLIC_ENTRY BrVector3Set(br_vector3 *v1, br_scalar s1, br_scalar s2, br_scalar s3);
void BR_PUBLIC_ENTRY BrVector3SetInt(br_vector3 *v1, int i1, int i2, int i3);
void BR_PUBLIC_ENTRY BrVector3SetFloat(br_vector3 *v1, float f1, float f2, float f3);
void BR_PUBLIC_ENTRY BrVector3Negate(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Add(br_vector3 *v1, br_vector3 *v2, br_vector3 *v3);
void BR_PUBLIC_ENTRY BrVector3Accumulate(br_vector3 *v1, br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Sub(br_vector3 *v1, br_vector3 *v2, br_vector3 *v3);
void BR_PUBLIC_ENTRY BrVector3Scale(br_vector3 *v1, br_vector3 *v2, br_scalar s);
void BR_PUBLIC_ENTRY BrVector3InvScale(br_vector3 *v1, br_vector3 *v2, br_scalar s);
br_scalar BR_PUBLIC_ENTRY BrVector3Dot(br_vector3 *v1, br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Cross(br_vector3 *v1,br_vector3 *v2,br_vector3 *v3);
br_scalar BR_PUBLIC_ENTRY BrVector3Length(br_vector3 *v1);
br_scalar BR_PUBLIC_ENTRY BrVector3LengthSquared(br_vector3 *v1);

void BR_PUBLIC_ENTRY BrVector3Normalise(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3NormaliseQuick(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3NormaliseLP(br_vector3 *v1,br_vector3 *v2);

/*
 * 2D vectors
 */
void BR_PUBLIC_ENTRY BrVector2Normalise(br_vector2 *v1,br_vector2 *v2);

/**
 ** 4D Vectors
 **/
br_scalar BR_PUBLIC_ENTRY BrVector4Dot(br_vector4 *v1, br_vector4 *v2);
void BR_PUBLIC_ENTRY BrVector4Copy(br_vector4 *v1, br_vector4 *v2);

/*
 * Euler Angles
 */
br_matrix34 * BR_PUBLIC_ENTRY BrEulerToMatrix34(br_matrix34 *mat,br_euler *euler);
br_euler * BR_PUBLIC_ENTRY BrMatrix34ToEuler(br_euler *euler,br_matrix34 *mat);

br_matrix4 * BR_PUBLIC_ENTRY BrEulerToMatrix4(br_matrix4 *mat,br_euler *src);
br_euler * BR_PUBLIC_ENTRY BrMatrix4ToEuler(br_euler *dest,br_matrix4 *mat);

br_quat * BR_PUBLIC_ENTRY BrEulerToQuat(br_quat *quat,br_euler *euler);
br_euler * BR_PUBLIC_ENTRY BrQuatToEuler(br_euler *euler,br_quat *quat);

/*
 * Quaternions
 */
br_quat * BR_PUBLIC_ENTRY BrQuatMul(br_quat *q,br_quat *l,br_quat *r);
br_quat * BR_PUBLIC_ENTRY BrQuatNormalise(br_quat *q,br_quat *qq);
br_quat * BR_PUBLIC_ENTRY BrQuatInvert(br_quat *q,br_quat *qq);

br_quat * BR_PUBLIC_ENTRY BrQuatSlerp(br_quat *q,br_quat *l,br_quat *r,br_scalar t, br_int_16 spins);

br_matrix34 * BR_PUBLIC_ENTRY BrQuatToMatrix34(br_matrix34 *mat,br_quat *q);
br_quat * BR_PUBLIC_ENTRY BrMatrix34ToQuat(br_quat *q,br_matrix34 *mat);

br_matrix4 * BR_PUBLIC_ENTRY BrQuatToMatrix4(br_matrix4 *mat,br_quat *q);
br_quat * BR_PUBLIC_ENTRY BrMatrix4ToQuat(br_quat *q,br_matrix4 *mat);

/*
 * Block pool allocator
 */
br_pool * BR_PUBLIC_ENTRY BrPoolAllocate(int block_size, int chunk_size, br_uint_8 mem_type);
void BR_PUBLIC_ENTRY BrPoolFree(br_pool *pool);

void * BR_PUBLIC_ENTRY BrPoolBlockAllocate(struct br_pool *pool);
void BR_PUBLIC_ENTRY BrPoolBlockFree(struct br_pool *pool,void *block);

void BR_PUBLIC_ENTRY BrPoolEmpty(struct br_pool *pool);

/*
 * Transforms
 */
void BR_PUBLIC_ENTRY BrTransformToMatrix34(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix34ToTransform(br_transform *xform,br_matrix34 *mat);
void BR_PUBLIC_ENTRY BrTransformToTransform(br_transform *dest,br_transform *src);

void BR_PUBLIC_ENTRY BrMatrix34PreTransform(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix34PostTransform(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix4PreTransform(br_matrix4 *mat,br_transform *xform);

/*
 * 2x3 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix23Copy(br_matrix23 *A, br_matrix23 *b);
void BR_PUBLIC_ENTRY BrMatrix23Mul(br_matrix23 *A, br_matrix23 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23Pre(br_matrix23 *mat , br_matrix23 *A);
void BR_PUBLIC_ENTRY BrMatrix23Post(br_matrix23 *mat , br_matrix23 *A);

void BR_PUBLIC_ENTRY BrMatrix23Identity(br_matrix23 *mat);

void BR_PUBLIC_ENTRY BrMatrix23Rotate(br_matrix23 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix23PreRotate(br_matrix23 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix23PostRotate(br_matrix23 *mat, br_angle rz);

void BR_PUBLIC_ENTRY BrMatrix23Translate(br_matrix23 *mat, br_scalar x, br_scalar y);
void BR_PUBLIC_ENTRY BrMatrix23PreTranslate(br_matrix23 *mat, br_scalar x, br_scalar y);
void BR_PUBLIC_ENTRY BrMatrix23PostTranslate(br_matrix23 *mat, br_scalar x, br_scalar y);

void BR_PUBLIC_ENTRY BrMatrix23Scale(br_matrix23 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PreScale(br_matrix23 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PostScale(br_matrix23 *mat, br_scalar sx, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix23ShearX(br_matrix23 *mat, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PreShearX(br_matrix23 *mat, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PostShearX(br_matrix23 *mat, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix23ShearY(br_matrix23 *mat, br_scalar sx);
void BR_PUBLIC_ENTRY BrMatrix23PreShearY(br_matrix23 *mat, br_scalar sx);
void BR_PUBLIC_ENTRY BrMatrix23PostShearY(br_matrix23 *mat, br_scalar sx);

void BR_PUBLIC_ENTRY BrMatrix23ApplyV(br_vector2 *A, br_vector2 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23ApplyP(br_vector2 *A, br_vector2 *B, br_matrix23 *C);

void BR_PUBLIC_ENTRY BrMatrix23TApplyV(br_vector2 *A, br_vector2 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23TApplyP(br_vector2 *A, br_vector2 *B, br_matrix23 *C);

br_scalar BR_PUBLIC_ENTRY BrMatrix23Inverse(br_matrix23 *out, br_matrix23 *in);
void BR_PUBLIC_ENTRY BrMatrix23LPInverse(br_matrix23 *A, br_matrix23 *B);
void BR_PUBLIC_ENTRY BrMatrix23LPNormalise(br_matrix23 *A, br_matrix23 *B);

/*
 * Backwards compatibility
 */
#define BrMatrix34Transform BrTransformToMatrix34
#define BrTransformTransfer BrTransformToTransform


/*
 * Picking
 */
typedef int BR_CALLBACK br_pick2d_cbfn(
		br_actor *a,
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t_near, br_scalar t_far,
		void *arg);

int BR_PUBLIC_ENTRY BrScenePick2D(
		br_actor *world,
		br_actor *camera,
		br_pixelmap *viewport,
		int pick_x, int pick_y,
		br_pick2d_cbfn *callback, void *arg );

typedef int BR_CALLBACK br_pick3d_cbfn(
		br_actor *a,
		br_model *model,
		br_material *material,
		br_matrix34 *transform,
		br_bounds *bounds,
		void *arg);

int BR_PUBLIC_ENTRY BrScenePick3D(
		br_actor *world,
		br_actor *actor,
		br_bounds *bounds,
		br_pick3d_cbfn *callback, void *arg );

typedef int BR_CALLBACK br_modelpick2d_cbfn(
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t,
		int face,
		int edge,
		int vertex,
		br_vector3 *p,
		br_vector2 *map,
		void *arg);

int BR_PUBLIC_ENTRY BrModelPick2D(
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t_near, br_scalar t_far,
		br_modelpick2d_cbfn *callback,
		void *arg);

/*
 * Custom calback support
 */
br_uint_8 BR_PUBLIC_ENTRY BrOnScreenCheck(br_bounds *bounds);

br_uint_8 BR_PUBLIC_ENTRY BrOriginToScreenXY(br_vector2 *screen);
br_uint_32 BR_PUBLIC_ENTRY BrOriginToScreenXYZO(br_vector3 *screen);

br_uint_8 BR_PUBLIC_ENTRY BrPointToScreenXY(br_vector2 *screen, br_vector3 *point);
br_uint_32 BR_PUBLIC_ENTRY BrPointToScreenXYZO(br_vector3 *screen, br_vector3 *point);

void BR_PUBLIC_ENTRY BrPointToScreenXYMany(br_vector2 *screens, br_vector3 *points, br_uint_32 npoints);
void BR_PUBLIC_ENTRY BrPointToScreenXYZOMany(br_vector3 *screens, br_uint_32 *outcodes, br_vector3 *points, br_uint_32 npoints);

void BR_PUBLIC_ENTRY BrSceneModelLight(br_model *model, br_material *default_material, br_actor *root, br_actor *a);

/*
 * Byte swapping
 */
typedef float br_float;

br_uint_32 BR_PUBLIC_ENTRY BrSwap32(br_uint_32 l);
br_uint_16 BR_PUBLIC_ENTRY BrSwap16(br_uint_16 s);
br_float   BR_PUBLIC_ENTRY BrSwapFloat(br_float f);
void *BR_PUBLIC_ENTRY BrSwapBlock(void *block, int count, int size);

/*
 * Misc. support
 */
typedef int BR_CALLBACK br_qsort_cbfn(const void *, const void *);
void BR_PUBLIC_ENTRY BrQsort(void *basep, unsigned int nelems, unsigned int size,
                            br_qsort_cbfn *comp);
/*
 * Diagnostic generation
 */
void BR_PUBLIC_ENTRY BrFailure(char *s,...);
void BR_PUBLIC_ENTRY BrWarning(char *s,...);
void BR_PUBLIC_ENTRY BrFatal(char *name, int line, char *s,...);
void BR_PUBLIC_ENTRY _BrAssert(char *condition, char *file, unsigned line);
void BR_PUBLIC_ENTRY _BrUAssert(char *condition, char *file, unsigned line);

/*
 * Set new handlers
 */
br_diaghandler * BR_PUBLIC_ENTRY BrDiagHandlerSet(br_diaghandler *newdh);
br_filesystem * BR_PUBLIC_ENTRY BrFilesystemSet(br_filesystem *newfs);
br_allocator * BR_PUBLIC_ENTRY BrAllocatorSet(br_allocator *newal);

/*
 * Backwards compatibility
 */
#define BrErrorHandlerSet BrDiagHandlerSet

/*
 * Generic file IO
 */
br_uint_32 BR_PUBLIC_ENTRY BrFileAttributes(void);


void * BR_PUBLIC_ENTRY BrFileOpenRead(char *name, br_size_t n_magics,
		br_mode_test_cbfn *mode_test,
		int *mode_result);
void * BR_PUBLIC_ENTRY BrFileOpenWrite(char *name, int text);
void BR_PUBLIC_ENTRY BrFileClose(void *f);
int BR_PUBLIC_ENTRY BrFileEof(void *f);
int BR_PUBLIC_ENTRY BrFileGetChar(void *f);
void BR_PUBLIC_ENTRY BrFilePutChar(int c, void *f);
int BR_PUBLIC_ENTRY BrFileRead(void *buf, int size, int n,void *f);
int BR_PUBLIC_ENTRY BrFileWrite(void *buf, int size, int n, void *f);
int BR_PUBLIC_ENTRY BrFileGetLine(char *buf, br_size_t buf_len, void * f);
void BR_PUBLIC_ENTRY BrFilePutLine(char *buf, void * f);
void BR_PUBLIC_ENTRY BrFileAdvance(long int count, void *f);

int BR_PUBLIC_ENTRY BrFilePrintf(void *f, char *fmt, ...);

/*
 * Data file output type (one of BR_FS_MODE_xxx)
 */
int BR_PUBLIC_ENTRY BrWriteModeSet(int text);

/*
 * Generic memory allocation
 */
void * BR_PUBLIC_ENTRY BrMemAllocate(br_size_t size, br_uint_8 type);
void BR_PUBLIC_ENTRY BrMemFree(void *block);
br_size_t BR_PUBLIC_ENTRY BrMemInquire(br_uint_8 type);

char * BR_PUBLIC_ENTRY BrMemStrDup(char *str);
void * BR_PUBLIC_ENTRY BrMemCalloc(int nelems, br_size_t size, br_uint_8 type);

/*
 * Resource allocation
 */
void * BR_PUBLIC_ENTRY BrResAllocate(void *vparent, br_size_t size, int res_class);
void BR_PUBLIC_ENTRY BrResFree(void *vres);
char * BR_PUBLIC_ENTRY BrResStrDup(void *vparent, char *str);

void * BR_PUBLIC_ENTRY BrResAdd(void *vparent, void *vres);
void * BR_PUBLIC_ENTRY BrResRemove(void *vres);
br_uint_8 BR_PUBLIC_ENTRY BrResClass(void * vres);
br_uint_32 BR_PUBLIC_ENTRY BrResSize(void *vres);
br_uint_32 BR_PUBLIC_ENTRY BrResSizeTotal(void *vres);
typedef br_uint_32 BR_CALLBACK br_resenum_cbfn(void *vres, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrResChildEnum(void *vres, br_resenum_cbfn *callback, void *arg);

/*
 * Block operations
 */
void BR_ASM_CALL BrBlockFill(void *dest_ptr, int value, int dwords);
void BR_ASM_CALL BrBlockCopy(void *dest_ptr, void *src_ptr, int dwords);

#if BR_HAS_FAR
void BR_ASM_CALL BrFarBlockCopy(void __far *dest_ptr, void *src_ptr, int dwords);
#endif

/*
 * Scratchpad buffer allocation - Currenty, only one allocation
 * may be outstanding at any time
 */
void *BR_PUBLIC_ENTRY BrScratchAllocate(br_size_t size);
void BR_PUBLIC_ENTRY BrScratchFree(void *scratch);
br_size_t BR_PUBLIC_ENTRY BrScratchInquire(void);
void BR_PUBLIC_ENTRY BrScratchFlush(void);


/*
 * Utility "FindFailed" callbacks that can be used to automaticaly load
 * models/materials/maps/tables from the filesystem
 */
br_pixelmap * BR_CALLBACK BrMapFindFailedLoad(char *name);
br_pixelmap * BR_CALLBACK BrTableFindFailedLoad(char *name);
br_model * BR_CALLBACK BrModelFindFailedLoad(char *name);
br_material * BR_CALLBACK BrMaterialFindFailedLoad(char *name);

/*
 * Backwards comaptibility
 */
#define BrModelPrepare BrModelUpdate
#define BrMaterialPrepare BrMaterialUpdate
#define BrMapPrepare BrMapUpdate
#define BrTablePrepare BrTableUpdate

/*
 * Error retrieval
 */
br_error BR_PUBLIC_ENTRY BrGetLastError(void **valuep);

#endif /* _NO_PROTOTYPES */

#ifdef __cplusplus
};
#endif
#endif

