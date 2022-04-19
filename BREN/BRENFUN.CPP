// Temporary brender code
#include "frame.h"
#include "bren.h"
ASSERTNAME


extern br_material test_materials[];
extern int test_materials_count;

br_camera camera_data = {
	NULL,
	BR_CAMERA_PERSPECTIVE,
    BR_ANGLE_DEG(45),			/* 45 degree field of view	*/
	BR_SCALAR(1.0),				/* Hither 					*/
	BR_SCALAR(100.0),			/* Yon 						*/
	BR_SCALAR(16.0 / 9.0),		/* Aspect ratio 			*/
};

br_material texmap = {
	"texmap",
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	{255},						/* opacity			*/
	BR_UFRACTION(0.05),			/* ka				*/
	BR_UFRACTION(0.30),			/* kd				*/
	BR_UFRACTION(0.99),			/* ks				*/
	BR_SCALAR(20),				/* power			*/
	BR_MATF_MAP_COLOUR|BR_MATF_LIGHT|BR_MATF_GOURAUD, /* flags			*/
	0,0,						/* shift up/down   	*/
	0,63,						/* index base/range	*/
};

static bool FLoadModel(char *szFileName, br_model **pmdl)
{
    *pmdl = BrModelLoad(szFileName);
	if (*pmdl == NULL)
		return fFalse;
	else
		BrModelAdd(*pmdl);
	return fTrue;
}


// create some objects
void BREN::CreateFunStuff(void)
{
	int i;
 	br_pixelmap *pm;
	br_pixelmap *shade;

//	BrBegin();
//    BrZbBegin();

	obs_z = BR_SCALAR(5.0);

	/*
	 * Add materials to system
	 */
	for(i = 0; i < test_materials_count; i++)
		BrMaterialAdd(test_materials+i);

	shade = BrPixelmapLoad("shade.tab");
	BrTableAdd(shade);		/* Tell renderer about shading table */

	pm = BrPixelmapLoad("jupiter.pix");
	BrMapAdd(pm);			/* Tell renderer about texture map */
	texmap.index_shade = shade;
	texmap.colour_map = pm;
	BrMaterialAdd(&texmap);	/* Tell renderer about material */


	// Load all the models
	if (!FLoadModel("iceman.dat", &modlMain))
        MessageBox (NULL, "Could not load iceman.dat", "ERROR", MB_OK); 
	if (!FLoadModel("torus.dat", &modlChild))
        MessageBox (NULL, "Could not load torus.dat", "ERROR", MB_OK); 
	if (!FLoadModel("cube.dat", &modlCube))
        MessageBox (NULL, "Could not load cube.dat", "ERROR", MB_OK); 

	/*
	 * Create the world
	 */
//	actrWorld = BrActorAllocate(BR_ACTOR_NONE,NULL);
	_pbactWorld = BrActorAllocate(BR_ACTOR_NONE,NULL);
	_pbactCamera = BrActorAdd(_pbactWorld, BrActorAllocate(BR_ACTOR_CAMERA, &camera_data));
//	actrCamera->t.type = BR_TRANSFORM_MATRIX34;
//	BrMatrix34Translate(&actrCamera->t.t.mat, BR_SCALAR(0.0), BR_SCALAR(20.0), 
//		BR_SCALAR(0.0));

	/*
	 * Test shape and child
	 */
	actrMain = BrActorAdd(_pbactWorld,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	actrMain->model = modlMain;
// 	actrMain->material = BrMaterialFind("texmap");

	actrChild = BrActorAdd(actrMain,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	actrChild->t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Translate(&actrChild->t.t.mat,BR_SCALAR(1.5),BR_SCALAR(0.0),BR_SCALAR(0.0));
	actrChild->model = modlChild;

	/*
	 * Spinning shape driven by a quaternion
	 */
	actrQuat = BrActorAdd(_pbactWorld,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	actrQuat->model = modlCube;
	actrQuat->t.type = BR_TRANSFORM_QUAT;
	BrVector3SetInt(&actrQuat->t.t.quat.t,-2,0,0);

	/*
	 * Spinning shape driven by euler angles
	 */
	actrEuler = BrActorAdd(_pbactWorld,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	actrEuler->model = modlCube;
	actrEuler->t.type = BR_TRANSFORM_EULER;
	actrEuler->t.t.euler.e.order = BR_EULER_YXZ_R;
	BrVector3SetInt(&actrEuler->t.t.euler.t,-4,0,0);

	/*
	 * Initialise angles
	 */
	actrEuler->t.t.euler.e.a = 0;
	actrEuler->t.t.euler.e.b = 0;
	actrEuler->t.t.euler.e.c = 0;

	/*
	 * Some useful transforms for later
	 */
	BrMatrix34Identity(&Rotation);
	BrMatrix34Identity(&ObsRotation);
}

#define MSCALE BR_SCALAR(0.05)

void BREN::ChangeScene (WPARAM mouseFlags, LPARAM coords)
{
	static UINT old_mouse_x;
	static UINT old_mouse_y;
	WORD mouse_x = LOWORD(coords) - old_mouse_x;
	WORD mouse_y = HIWORD(coords) - old_mouse_y;
 	br_matrix34 tmpmat;

	/*
	 * Mouse control
	 */
	if(mouse_x !=0 || mouse_y != 0) {
		if(mouseFlags & MK_LBUTTON) {
			if(mouseFlags & MK_RBUTTON) {
				/*
				 * Move actrCamera in/out
				 */
				obs_z += BR_MUL(BR_SCALAR(mouse_y),MSCALE);
			} else {
				/*
				 * Drag object around
				 */
				pos_x += BR_MUL(BR_SCALAR(mouse_x),MSCALE);
				pos_y -= BR_MUL(BR_SCALAR(mouse_y),MSCALE);
			}

		} else if(mouseFlags & MK_RBUTTON) {
			/*
			 * Rotate actrCamera via rolling ball interface
			 */
			BrMatrix34RollingBall(&tmpmat,(br_int_16)-mouse_x,mouse_y,800);
			BrMatrix34Pre(&ObsRotation,&tmpmat);
		} else {
			/*
			 * Rotate object via rolling ball interface
			 */
			BrMatrix34RollingBall(&tmpmat,mouse_x,(br_int_16)-mouse_y,200);
			BrMatrix34Post(&Rotation,&tmpmat);
		}
	}

	old_mouse_x = LOWORD (coords);
	old_mouse_y = HIWORD (coords);

	/*
	 * Modify scene according to controls
	 */
	BrMatrix34Translate(&_pbactCamera->t.t.mat,obs_x,obs_y,obs_z);
	BrMatrix34Post(&_pbactCamera->t.t.mat,&ObsRotation);

	/*
	 * test shape spun with mouse
	 */
	BrMatrix34Translate(&actrMain->t.t.mat,pos_x,pos_y,pos_z);
	BrMatrix34Pre(&actrMain->t.t.mat,&Rotation);

	/*
	 *
	 Convert angles to a quaternion for second shape
	 */
	BrEulerToQuat(
		&actrQuat->t.t.quat.q,
		&actrEuler->t.t.euler.e);

	/*
	 * Roll angles around
	 */
	actrEuler->t.t.euler.e.a += BR_ANGLE_DEG(3);
	actrEuler->t.t.euler.e.b += BR_ANGLE_DEG(5);
	actrEuler->t.t.euler.e.c += BR_ANGLE_DEG(7);
}
