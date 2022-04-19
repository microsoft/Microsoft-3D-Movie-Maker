////////////////////////////////////////////////////////////////////////////////
//	File:			OWNRDRAW.H
//	Purpose:		Sample code for owner draw buttons and lists
////////////////////////////////////////////////////////////////////////////////

VOID DrawButton(PDRAWITEMSTRUCT lpdis, HBITMAP hbmp);

typedef enum
	{
	ttabLeft,
	ttabRight,
	ttabCentre
	} TTAB, *PTTAB;

typedef struct
	{
	LONG dx;
	TTAB ttab;
	} TAB, *PTAB;	// listbox tab setting

VOID DrawList(PDRAWITEMSTRUCT pdis, LONG ctab, PTAB prgtab, BOOL fCombo);
