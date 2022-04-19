
SOFTIMAGE
Research & Development

--------------------------------------------------------
SOFTIMAGE Developer Kit for Creative Environment v2.6.2
--------------------------------------------------------

	The SOFTIMAGE Developer Kit provides structures and according
  read and write functions allowing the user to parse scenes, geometries,
  picture (and other) files, or to create these files from custom
  code.

Contents:
--------

   You should find these files in the current directory:

   Makefile	: Makefile to produce the executable.

   README	: Information about the developer's toolkit (this file).

   CHANGES	: List of changes since the first official version.

   bin/		: Empty directory where demo programs will be found.

   lib/		: Directory where the libraries 'libdkit.a' and
		  'libdbdk.a' will be found.

   lib5_1/	: Directory containing a version of 'libdbdk.a'
		  compiled under IRIX 5.1.1. ('libdkit.a' can be
		  compiled under 5.1.1 by the user).

   doc/		: Directory containing documentation files.

   	guide		Programmer's guide about SOFTIMAGE's tiny model
			data structure.
   	sceneGuide	Programmer's guide about SOFTIMAGE's tiny scene
			data structure.

   h/		: Directory containing header files.

	libdbdk.h	Prototypes and documentation for the functions
			of the library 'libdbdk.a'
	tinyCamera.h
	tinyConstr.h
	tinyDynamics.h
	tinyFace.h
	tinyFcurve.h
	tinyFile.h
	tinyFitting.h
	tinyLattice.h
	tinyLight.h
	tinyMaterial.h
	tinyMesh.h
	tinyModel.h
	tinyPatch.h
	tinyPicture.h
	tinyScene.h
	tinySceneToken.h
	tinyScnDump.h
	tinySoftType.h
	tinySpline.h
	tinyTexture.h
	tinyUtils.h
	tinyWave.h

   src/		: Directory containing source for the library `libdkit.a'.

	tinyCamera.c
	tinyConstr.c
	tinyDynamics.c
	tinyFace.c
	tinyFcurve.c
	tinyFile.c
	tinyFitting.c
	tinyLattice.c
	tinyLight.c
	tinyMaterial.c
	tinyMesh.c
	tinyModel.c
	tinyPatch.c
	tinyPicture.c
	tinyScene.c
	tinyScnDump.c
	tinySpline.c
	tinyTexture.c
	tinyUtils.c
	tinyWave.c

   examples/	: Directory containing some examples written with the
		  developer's toolkit.

	cube.c		creates a cube and writes it in a file (.hrc).
	circle.c	creates a circle and writes it in a file (.spl).
	dbdktest.c	test program calling functions from 'libdbdk.a'
	splConvert.c	reads a spline (.spl) and writes it.
			options to write in ASCII or v2.0 compatible.
	hrcConvert.c	reads an object (.hrc) and writes it.
			options to write in ASCII or v2.0 compatible.
	txtConvert.c	reads a texture (.txt) and writes it.
			options to write in ASCII.
	mtrConvert.c	reads a material (.mtr) and writes it.
			options to write in ASCII or v2.0 compatible.
	t3DConvert.c	reads a 3D texture (.t3D) and writes it.
			options to write in ASCII or v2.0 compatible.
	negate1.c	inverses colours of an image
			(example working on a whole picture).
	negate2.c	inverses colours of an image
			(example working scanline by scanline).
	scene.c		reads and writes an ascii scene file.
	scnload.c	reads and writes new files (> v2.6) using
			functions from the library 'libdbdk.a'
	sceneDumpMatrix.c
			dumps the transformation matrix of each models in the
			given ascii scene file.
	scene_edit.C	reads a scene, modifies it and saves it back.
			This a C++ program.
	taper.c		custom button example program.
	toSphere.c	another custom button example program.

To create programs type: make <CR>

