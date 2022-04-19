/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Data movement declarations

***************************************************************************/
#ifndef UTILCOPY_H
#define UTILCOPY_H

void BltPb(void *pv1, void *pv2, long cb);
void CopyPb(void *pv1, void *pv2, long cb);
void ClearPb(void *pv, long cb);
void FillPb(void *pv1, long cb, byte b);
void ReversePb(void *pv, long cb);
void ReverseRgsw(void *pv, long csw);
void ReverseRglw(void *pv, long clw);
void SwapBlocks(void *pv, long cb1, long cb2);
void SwapPb(void *pv1, void *pv2, long cb);
void MoveElement(void *prgv, long cbElement, long ivSrc, long ivTarget);
bool FEqualRgb(void *pv1, void *pv2, long cb);
long CbEqualRgb(void *pv1, void *pv2, long cbMax);
ulong FcmpCompareRgb(void *pv1, void *pv2, long cb);

#ifdef DEBUG
#define SwapVars(pv1, pv2) \
	if (size(*pv1) != size(*pv2)) \
		Bug("sizes don't match"); \
	else \
		SwapPb(pv1, pv2, size(*pv1))
#else //!DEBUG
#define SwapVars(pv1, pv2) \
		SwapPb(pv1, pv2, size(*pv1))
#endif //!DEBUG

#endif //UTILCOPY_H

