/*******************************************************************************
********************************************************************************

   libdbdk.a function prototypes and documentation.

   Creative Environment v3.0
   libdbdk.a v2.0

   The functions accessible in the library 'libdbdk.a' allow DKit programs
   to read/write scenes with version number 2.6 and up, as well as to
   perform few file management queries/functions.

   NOTE: SEE THE FUNCTION REFERENCE AT THE END OF THIS FILE.

   File supervisor: Rejean Gagne

   (c) Copyright 1993-1994, SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef _DK_DBDK_H
#define _DK_DBDK_H


/*-------------------------------------------------*/
/* Type definition to make parameters more evident */
/*-------------------------------------------------*/
typedef	void *		DK_Database;



/*======================================================*/
/* Overall Initialisation : MUST ALWAYS BE CALLED FIRST */
/*======================================================*/
extern void 	       DK_dbInit( DK_String rsrc_path, DK_Boolean read_dbdir ); 

/*-----------------*/
/* Database access */
/*-----------------*/

extern DK_Database     DK_dbOpen( DK_String dbPath );
extern DK_Database     DK_dbExists( DK_String dbName );
extern DK_Database     DK_dbGetDefault( void );
extern DK_Database     DK_dbGet( DK_String dbName );

/*------------------------------------------------------------------------*/
/* Read and write a scene from a database 				  */
/*------------------------------------------------------------------------*/

extern DK_Scene	      *DK_dbSceneLoad( DK_String scnName, DK_Database dbPtr );
extern int	       DK_dbSceneSave( DK_Scene *scene, 
				DK_Database dbPtr, DK_Boolean force );

/*------------------------------------------------------------------------*/
/* Special Read and write scene functions that get from/send to scene     */
/* information through pipes. These are used for Custom scene i/o effects.*/
/*------------------------------------------------------------------------*/

extern DK_Scene	      *DK_dbSceneGetFromCE(  );
extern DK_Boolean      DK_dbSceneSendToCE( DK_Scene *scene );
/*------------------------------------------------------------------------*/
/* DK_dbAnimModelSendToCE(), is for internal use only. It interprets a    */
/* scene as an animated model and sends that to CE. Some of the fields in */
/* the scene structure get modified and freed.                            */
/*------------------------------------------------------------------------*/
extern DK_Boolean      DK_dbAnimModelSendToCE( DK_Scene *scene );
extern void             DK_sceneIsAnimModel( DK_Boolean flag );

/*-----------------------------------------------*/
/* Create and remove databases and scenes.       */
/*-----------------------------------------------*/

extern DK_Database     DK_dbCreate( DK_String dbPath, DK_String dbName,
						  DK_Boolean write_dbdir );
extern int	       DK_dbDelete( DK_Database dbPtr, DK_Boolean force );
extern int	       DK_dbSceneDelete( DK_String scnName, DK_Database dbPtr,
						  DK_Boolean scn_only );

/*--------------------------------------------------------------*/
/* Versions & locks 						*/
/*--------------------------------------------------------------*/
extern int	DK_dbSceneGetLastVersNum(DK_String scnName, DK_Database dbPtr);
extern int	DK_dbSceneGetFirstVersNum(DK_String scnName, DK_Database dbPtr);
extern int	DK_dbSceneGetVersCount( DK_String scnName, DK_Database dbPtr );
extern DK_String   DK_dbSceneGetVersName( DK_String scnName, int version );
extern void	DK_dbNoOldVersionsFlag( DK_Boolean flag );
extern void	DK_dbNoMatTxtFlag( DK_Boolean flag );


extern DK_Boolean  DK_dbSceneIsLocked( DK_String scnName, DK_Database dbPtr );
extern DK_Boolean  DK_dbSceneSetLock( DK_String scnName, DK_Database dbPtr );
extern DK_Boolean  DK_dbSceneUnLock( DK_String scnName, DK_Database dbPtr );

/* for scnload.c */
extern DK_String DK_dbGetLatestSceneVersion( DK_String scnName, 
						DK_Database dbPtr );

/*--------*/
/* Others */
/*--------*/
extern void DK_setVerbose( DK_Boolean state );

#ifdef __DOCUMENTATION_SECTION__
=========================================================================
=========================================================================
		      D O C U M E N T A T I O N
=========================================================================
=========================================================================

> void DK_dbInit( DK_String rsrc_path, DK_Boolean read_dbdir ); 

   Overall initialisation. MUST always be called first.

   - rsrc_path:  path towards resources (MUST be given to access
		 the Relations.rsrc file.)
   - read_dbdir: specifies if a DatabaseDir.rsrc should be read.
		 This is optional depending on if you want to use predefined 
		 database names (DK_dbGet) or if you want to work with
		 database paths (DK_dbOpen). In the first case
		 read_dbdir MUST be set to TRUE.

   - returned:	 void.

-------------------------

> DK_Database DK_dbOpen( DK_String dbPath );
> DK_Database DK_dbGet( DK_String dbName );

   Gets a database pointer.

   - dbPath:	Full path where to locate the database.
		A Database ptr is returned even if the path does not 
		point to anything that looks like a database.
   - dbName:	Database name (from a DatabaseDir.rsrc file).

   - returned:	Database ptr to use in all other functions.

-------------------------

> DK_Scene *DK_dbSceneLoad( DK_String scnName, DK_Database dbPtr );

   - scnName:	Exact scene name to be read (.dsc suffix not included)
   - dbPtr:	Database where to read from.

   - returned:	A newly allocated DKit scene with the information
		read from the database. NULL if scnName or dbPtr are 
		NULL or if the requested scene could not be read for
		any reason.

-------------------------

> int DK_dbSceneSave( DK_Scene *scene, DK_Database dbPtr, DK_Boolean force );

   - scene:	DKit scene to be saved under the name scene->name.
   - dbPtr:	Database where to save.
   - force:	'force' in DK_dbSceneSave will make sure the scene is 
		saved with the version number found in the given name. 
		(If none is given 1-0 will be used). This could overwrite 
		on an existing scene. If 'force' is FALSE the next available 
		version number is used.
   - returned:	TRUE unless scene or dbPtr are NULL.

---------------------------

> DK_Database DK_dbCreate( DK_String dbPath, DK_String dbName, 
						DK_Boolean write_dbdir );

   Creates a new database.

   - dbPath:	Path where to create the database. It MUST always
		end with a '/'. (name is appended).
   - dbName:	Name to reference the database. Not optional.
   - write_dbdir: Option. If TRUE an entry will be added for the new
		  database in the current DatabaseDir.rsrc file.

   - returned:	Pointer to the new database or NULL if the
		database could not be created for any of the following
		reasons:
		  - the user does not have access to the given path.
		  - the path already exists.
		  - a database already exists at the given path.

---------------------------

> int DK_dbDelete( DK_Database dbPtr, DK_Boolean force );

   - dbPtr:	Database to delete.
   - force:	If TRUE, removes non-empty databases.

   - returned:	TRUE if database could be deleted.

---------------------------

> int DK_dbSceneDelete( DK_String scnName, DK_Database dbPtr, 
						DK_Boolean scn_only );

   - scnName:	Full name of the scene to delete.
   - dbPtr:	Database where to delete the scene.
   - scn_only:	If TRUE, removes only the scene file.
		Only the elements not used somewhere else
		are deleted if scn_only is FALSE.
   - returned:  TRUE if the operation could be done. FALSE if:
		  - the scene file does not exist.
		  - no write access to the database.
		  - scene is locked for delete.
		  - any element could not be deleted.
		  - all elements are still in use (& !scn_only).

---------------------------

> int DK_dbSceneGetLastVersNum( DK_String scnName, DK_Database dbPtr);
> int DK_dbSceneGetFirstVersNum( DK_String scnName, DK_Database dbPtr);
> int DK_dbSceneGetVersCount( DK_String scnName, DK_Database dbPtr );

   - scnName:	Basic scene name (may or may not include the
		<vn>-<sn> part). MUST however NOT include the suffix.
   - dbPtr:	Database where to find the scene.

   - returned:	- Last: latest version number (-1 if not found).
		- First: oldest version number (99999 if not found)
		- Count: number of version of the file found.

---------------------------

> DK_String   DK_dbSceneGetVersName( DK_String scnName, int version );

   Builds a complete scene name including the given version number.

   - scnName:   Basic scene name (may or may not include the
		<vn>-<sn> part). MUST however NOT include the suffix.
   - version:	required version number.

   - returned:	new allocated string name.

---------------------------

> void DK_dbNoOldVersionsFlag( DK_Boolean flag );

   DK_dbNoOldVersionsFlag allows to turn ON or OFF the
   deletion of old file versions when new scenes are saved.

   returned:	void.

---------------------------

> void DK_dbNoMatTxtFlag( DK_Boolean flag );

   DK_dbNoMatTxtFlag allows to turn ON or OFF the
   saving of separate MATERIALS and TEXTURES (2D/3D) files
   when saving a scene. This information, if not saved in
   separate file, is retrieved from the material and texture
   information also stored in the models, when loading
   with Creative Environment.

   returned:	void.

---------------------------

> DK_Boolean  DK_dbSceneIsLocked( DK_String scnName, DK_Database dbPtr );
> DK_Boolean  DK_dbSceneSetLock( DK_String scnName, DK_Database dbPtr );
> DK_Boolean  DK_dbSceneUnLock( DK_String scnName, DK_Database dbPtr );

   Deals with file locks (write permission).

   - scnName:   Basic scene name (may or may not include the
		<vn>-<sn> part). MUST however NOT include the suffix.
   - dbPtr:	Database where to find the scene.

   - returned:	Lock state for DK_dbSceneIsLocked.
		TRUE if operation could set file lock elsewhere.

---------------------------

> void DK_setVerbose( DK_Boolean state );

   Sets the verbose mode for the >>libdbdk<< functions.
   DKit verbose can be set through the DK_option variable in
   the file tinySoftType.h

   - state:	if FALSE, only ERROR status will be printed.
		(defaults to FALSE).

-------------------------

> DK_Scene *DK_dbSceneGetFromCE( );

   - returned:	A newly allocated DKit scene received from a pipe.

		This must be used in accordance with the Custom Effect
		command "SceneWrite".

-------------------------

> DK_Boolean      DK_dbSceneSendToCE( DK_Scene *scene );

   - scene:	DKit scene to be sent under the name scene->name.
   - returned:	TRUE unless scene is NULL.

		This must be used in accordance with the Custom Effect
		command "ReadScene"

==========================================================================
==========================================================================
==========================================================================
==========================================================================
#endif


#endif  /* _DK_DBDK_H */
