/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Core routines for the utility layer.

***************************************************************************/
#include "util.h"
ASSERTNAME


#ifdef DEBUG
/***************************************************************************
	Mark all util-level memory and objects.
***************************************************************************/
void MarkUtilMem(void)
{
	PCFL pcfl;
	PFIL pfil;

	MarkMemObj(&vsflUtil);
	MarkMemObj(&vrndUtil);
	MarkMemObj(&vkcdcUtil);
	MarkMemObj(&vcodmUtil);
	MarkMemObj(vpcodmUtil);

	for (pcfl = CFL::PcflFirst(); pcfl != pvNil; pcfl = pcfl->PcflNext())
		MarkMemObj(pcfl);

	for (pfil = FIL::PfilFirst(); pfil != pvNil; pfil = pfil->PfilNext())
		MarkMemObj(pfil);
}
#endif //DEBUG


