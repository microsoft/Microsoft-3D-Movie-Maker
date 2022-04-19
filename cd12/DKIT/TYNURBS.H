/*******************************************************************************
********************************************************************************


   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyNurbs.h,v $
   $Revision: 1.13 $ $Date: 1994/12/13 17:19:43 $
   Checkin by: $Author: a_tan $

ce Fichier contient l'interface pour ecrire les NURBs ( courbes et Surfaces ) dans le format binaire . 

Written by : Laurent Ruhlmann 12 mai 1994

(c) Copyright 1994 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_NURBS_H__
#define __TINY_NURBS_H__


#include  "tySoftTy.h"
#include  "tyFile.h"
#include  "tyPatch.h"


#define         DK_NRB_UNIFORM             0
#define         DK_NRB_OPEN_UNIFORM        1
#define         DK_NRB_NON_UNIFORM         2
#define         DK_NRB_CHORD_LENGTH        3
#define         DK_NRB_CENTRIPETAL         4

/* open/close NURBS information */
#define		DK_NRB_CLS_NRM		   0x00000001
#define		DK_NRB_CLS_PRS		   0x00000002
#define		DK_NRB_CLS_LIN		   0x00000004




typedef
   struct
   {
      double         	x;		/* X Honmogeneous coordinates 	*/
      double		y;		/* Y Honmogeneous coordinates 	*/
      double		z;		/* Z Honmogeneous coordinates 	*/ 
      double		w;		/* W Honmogeneous coordinates 	*/
      long              flag;		/* internal use 		*/
   } DK_NurbsPoint;

typedef
   struct
   {
      long     		order;     	/* order in this direction (degree+1) */
      long           	noPoints;  	/* number of points for NURB          */ 
      long           	noKnots;   	/* (closed) ? noPoints+1 :            */
				        /*	     (noPoints+order-2)       */
      double*        	knots;    	/* array of dimension noKnots         */
      double         	start;     	/* (closed) ? knots[0]:knots[order-2] */
      double         	end;       	/* (closed) ? knots[noKnots-1]: */
                                        /*            knots[noKnots-order+1]  */
      short		closed;    	/* TRUE if closed, FALSE otherwise    */
   } DK_NurbsDir;


typedef
   struct
   {
      DK_NurbsDir       dir;
      int            	uniformity;	/* uniform,open-uniform, non-uniform*/ 
      short 		rational;	/* TRUE if curve is rational 	    */
      DK_NurbsPoint*    points;        	/* noPoints 			    */
      int		closeInfo;	/* closing information   
					     normal, preserved, linear      */
      long		noStep;		/* no of display steps 		    */
   } DK_NurbsCrv;


typedef
   struct
   {
     short 		boundary;      	/* TRUE if CompCrv is a boundary */
     int             	noCurves;      	/* number of component curves    */
     DK_NurbsCrv**   	curves;      	/* array of dimension noCurves   */
     int             	noPoints;	/* # of points			 */
     DK_Vector*        	points; 	/* array of noPoints 		 */

   } DK_NurbsCompCrv;


typedef
   struct
   {
      DK_NurbsDir       uDir;
      DK_NurbsDir       vDir;

      long 		noPoints;

      int           	uniformity;
      short		rational;
      DK_NurbsPoint*    points;        /* uDir.noPoints X vDir.noPoints */
      long           	flag;

     /* trimming */
      short		autoBorder;    /* TRUE if boundary is entire domain  */
                                       /* In this case, the trim curves must */
                                       /* only be holes.*/
     
      int            	noTrims;       /* number of trim curves */      
      DK_NurbsCompCrv   **trims;       /* array of size noTrims */
      long		uNoStep;       /* u wireframe, SHADES & rend. step */
      long		vNoStep;       /* v wireframe, SHADES & rend. step */
      long	        uCurve;        /* u no. of drawing curve */
      long	        vCurve;        /* v no. of drawing curve */

      short             uBottom;        /* automatic capping     */
      short		vBottom; 
      short             uTop;
      short		vTop;    

      int		uCloseInfo;	/* closing information:              */
      int		vCloseInfo;	/*      normal, preserved, linear    */

      DK_MRApprox    	approx;         /* mental ray approximation method   */
      unsigned short 	approx_type;

   } DK_NurbsSrf;

/* Globals prototypes */

extern DK_NurbsCrv*     DK_nurbsCrvAllocate( void );
extern DK_NurbsSrf*     DK_nurbsSrfAllocate( void );

extern DK_NurbsCrv* DK_nurbsCrvCopy( DK_NurbsCrv *crv);
extern DK_NurbsCompCrv* DK_nurbsCompCrvCopy( DK_NurbsCompCrv *comp);

extern void          	DK_nurbsCrvDispose( DK_NurbsCrv **crv );
extern void          	DK_nurbsSrfDispose( DK_NurbsSrf **srf );
extern void          	DK_nurbsCompCrvDispose( DK_NurbsCompCrv **cps);

extern  DK_NurbsCrv*   	DK_nurbsCrvBinaryRead( FILE *file , DK_FileProt* prot );
extern DK_NurbsSrf*    	DK_nurbsSrfBinaryRead( FILE *file , DK_FileProt* prot );

extern DK_Boolean 	DK_nurbsCrvBinaryWrite ( FILE *file , DK_NurbsCrv *crv );
extern DK_Boolean 	DK_nurbsSrfBinaryWrite ( FILE *file , DK_NurbsSrf *srf );

extern DK_NurbsCrv*   	DK_nurbsCrvAsciiRead( FILE *file );
extern DK_NurbsSrf*    	DK_nurbsSrfAsciiRead( FILE *file );

extern DK_Boolean 	DK_nurbsCrvAsciiWrite ( FILE *file , DK_NurbsCrv *crv, DK_String space );
extern DK_Boolean 	DK_nurbsSrfAsciiWrite ( FILE *file , DK_NurbsSrf *srf, DK_String space );



#endif /* ifndef __TINY_NURBS_H__ */

