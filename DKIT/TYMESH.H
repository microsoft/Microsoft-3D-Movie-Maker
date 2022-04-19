/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyMesh.h,v $
   $Revision: 1.5 $ $Date: 1994/10/31 00:07:30 $
   Checkin by: $Author: laurent $

   This file contains a subset of the SOFTIMAGE's polygonal mesh data
   structure, its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

/* tinyMesh.h */

#ifndef __TINY_MESH_H__
#define __TINY_MESH_H__

#include  "tySoftTy.h"
#include  "tyFile.h"
#include  <stdarg.h>

/* values for flag field in vertex structure */
#define  DK_VRT_SELECTED           0x0001

typedef
   struct
   {
      DK_Point          position;	/* position of the vertex */
      short             flag;		/* user flag */
   } DK_Vertex;

typedef
   struct
   {
      long		 vertex;	/* index on appropriate vertex (**1) */
      DK_Vector		 normal;	/* normal at this vertex for the
					    polygon */
      float		 uTxt, vTxt;	/* UV texture coordinates */
   } DK_PolyNode;

/* values for flag field in polygon structure */
#define  DK_POL_SELECTED           0x0001

typedef
   struct
   {
      DK_Vector		 normal;	/* normal at polygon (**2) */
      short		 material;	/* DK_material index (from DK_model list) */
      short		 nbNodes;
      DK_PolyNode	*nodes;		/* array of nodes */
      short		 flag;
   } DK_Polygon;

/* values for flag field in edge structure */
#define  DK_EDG_SELECTED           0x0001
#define  DK_EDG_USER_VISIBLE       0x0002
#define  DK_EDG_USER_HIDDEN        0x0004
#define  DK_EDG_USER_HARD          0x0008

typedef
   struct
   {
      long		 vertices[2];	/* indices on appropriates vertices */
      long		 polygons[2];	/* indices on appropriates polygons,
					   [0] = left polygon when looking
					   from vrt[0] to vrt[1], outside
					   object */
      short		 flag;
   } DK_Edge;

/* values for flag field in mesh structure */
#define  DK_MSH_FACETED            0x0002
#define  DK_MSH_PROCESS_NORMAL     0x0004
#define  DK_MSH_AVERAGED_NORMAL    0x0008

typedef
   struct
   {
      long         nbVertices;
      DK_Vertex *  vertices;		/* array of vertices */
      long         nbPolygons;
      DK_Polygon * polygons;		/* array of polygons */
      long         nbEdges;		/* (**3) */
      DK_Edge *    edges;		/* array of edges, (**4) */
      float        discAngle;		/* discontinuity angle (degrees) */
      short        flag;
   } DK_Mesh;

/* Globals DK_Mesh Prototypes */

extern	DK_Mesh    *DK_meshAllocate( void );
extern	void        DK_meshDispose( DK_Mesh ** );
extern	DK_Vertex  *DK_vertexAllocate( long );
extern	DK_Polygon *DK_polygonAllocate( long );
extern	DK_Edge    *DK_edgeAllocate( long );
extern	void        DK_polygonInitializeVertex( DK_Polygon *, int, ... );
extern	DK_Boolean  DK_meshWriteBinary( FILE *, DK_Mesh *, DK_FileProt * );
extern	DK_Mesh    *DK_meshReadBinary( FILE *, DK_FileProt * );
extern	DK_Boolean  DK_meshWriteAscii( FILE *, DK_Mesh *, DK_String );
extern	DK_Mesh    *DK_meshReadAscii( FILE * );

/*******************************************************************************

Notes: (1) -1 at the vertex index of polyNode indicate a separation between
	    the exterior contour and holes.

       (2)  Normal are computed in counter clock wise order.
	    Exterior contour of polygon should be in CCW order but
	    its holes in CW order.

       (3)  The limit size for polygons, vertices and edges is NOW
	    2 147 483 647

       (4)  The structure support only 2 polygons sharing an edge, to create
	    object with multiple polygons you have to duplicate vertices and
	    edges
	    The edges data is not necessay in the file, the SOFTIMAGE 4D
	    Creative Environment will generate them when not present in file.
	    The array of edge begin at index 1.

*******************************************************************************/


#endif  /* ifndef __TINY_MESH_H__ */
