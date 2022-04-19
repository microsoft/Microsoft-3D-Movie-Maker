/*
 * Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
 *
 * Some test materials for the wld demo
 */
#include "argstd.h"
#include "brender.h"

br_material test_materials[] = {
	{
	"grey",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(50),				/* power			*/
	BR_MATF_LIGHT | BR_MATF_GOURAUD,	/* flags			*/
	0,0,						/* shift up/down   	*/
	0,59,						/* index base/range	*/
	},
	{
	"grey_flat",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),				/* power			*/
	BR_MATF_LIGHT,				/* flags			*/
	0,0,						/* shift up/down   	*/
	10,59,						/* index base/range	*/
	},
	{
	"beige",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),				/* power			*/
	BR_MATF_LIGHT | BR_MATF_GOURAUD,	/* flags			*/
	0,0,						/* shift up/down   	*/
	10,59,						/* index base/range	*/
	},
	{
	"beige_flat",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),							/* power			*/
	BR_MATF_LIGHT,					/* flags			*/
	0,0,						/* shift up/down   	*/
	10,59,						/* index base/range	*/
	},
	{
	"blue",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),							/* power			*/
	BR_MATF_LIGHT | BR_MATF_GOURAUD,	/* flags			*/
	0,0,						/* shift up/down   	*/
	74,59,						/* index base/range	*/
	},
	{
	"blue_flat",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),							/* power			*/
	BR_MATF_LIGHT,					/* flags			*/
	0,0,						/* shift up/down   	*/
	74,59,						/* index base/range	*/
	},
	{
	"red",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(70),				/* power			*/
	BR_MATF_LIGHT | BR_MATF_GOURAUD,	/* flags			*/
	0,0,						/* shift up/down   	*/
	138,59,						/* index base/range	*/
	},
	{
	"red_flat",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),							/* power			*/
	BR_MATF_LIGHT,					/* flags			*/
	0,0,						/* shift up/down   	*/
	138,59,						/* index base/range	*/
	},
	{
	"green",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.40),			/* ks				*/
	BR_SCALAR(30),				/* power			*/
	BR_MATF_LIGHT | BR_MATF_GOURAUD,	/* flags			*/
	0,0,						/* shift up/down   	*/
	202,44,						/* index base/range	*/
	},
	{
	"green_flat",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.10),			/* ka				*/
	BR_UFRACTION(0.60),			/* kd				*/
	BR_UFRACTION(0.60),			/* ks				*/
	BR_SCALAR(20),				/* power			*/
	BR_MATF_LIGHT,				/* flags			*/
	0,0,						/* shift up/down   	*/
	202,44,						/* index base/range	*/
	},

};

/*
 * Size of the above table
 */
int test_materials_count = ASIZE(test_materials);

