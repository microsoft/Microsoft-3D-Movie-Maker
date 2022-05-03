/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			DELREG.C
**	Purpose:		Sample Detection Custom Action Handler
**					routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define DELREG_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include <string.h>
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
#include "resource.h"


extern BOOL SETUPAPI DeleteRegKeyValue32 ( HKEY hRootKey, CSZC cszcSubKey, CSZC cszcValueName );

extern HINSTANCE hinst;

RC FAR PASCAL DeleteRegKeys ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );

/*
**	Purpose:
**		Custom Action Handler for deleting Socrates created registry keys.
**		Takes no .STF data arguments.
**
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		Appropriate RC (return code) value.
**
***************************************************************************/
RC FAR PASCAL DeleteRegKeys ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC	rc = rcDoDefault;
	CHAR rgchSubKey[256];
	CHAR rgchKey[256];

	Unused(szData);

	if ((pod->ois != oisToBeRemoved) || (camf != camfDoNonVisualMods))
		{
		return(rc);
		}

	if (!LoadString(hinst, IDS_REGKEY, rgchSubKey, sizeof rgchSubKey) != 0)
		{
		strcpy(rgchSubKey, "3D Movie Maker");
		}

	wsprintf(rgchKey, "SOFTWARE\\Microsoft\\Microsoft Kids\\%s", rgchSubKey);

	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "HomeDirectory");
	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "SwitchResolution");
	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "UserData");
	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "GreaterThan8bppMsg");
	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "DebugSettings");
	DeleteRegKeyValue32(HKEY_LOCAL_MACHINE, rgchKey, "WaveOutMsg");
	DeleteRegKeyValue32(HKEY_LOCAL_MACHINE, rgchKey, "MidiOutMsg");
	DeleteRegKeyValue32(HKEY_CURRENT_USER, rgchKey, "BetterSpeed");
	return (rcOk);
}


