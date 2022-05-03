/*
 * Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
 *
 * $Id: stderr.c 1.3 1994/11/07 01:39:18 sam Exp $
 * $Locker: sam $
 *
 * Default error handler that reports error through stderr
 */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "brender.h"

static void BR_CALLBACK BrStdioWarning(char *message)
{
	MessageBox(0, message, "BRender Warning", MB_OK);
}

static void BR_CALLBACK BrStdioError(char *message)
{
	MessageBox(0, message, "BRender Fatal Error", MB_OK);
	exit(10);
}

/*
 * ErrorHandler structure
 */
br_diaghandler BrStdioDiagHandler = {
	"Stdio DiagHandler",
	BrStdioWarning,
	BrStdioError,
};

/*
 * Override default
 */
br_diaghandler *_BrDefaultDiagHandler = &BrStdioDiagHandler;


