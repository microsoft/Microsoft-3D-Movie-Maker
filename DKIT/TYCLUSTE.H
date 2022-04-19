/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyCluster.h,v $
   $Revision: 1.7 $ $Date: 1994/12/01 00:28:08 $
   Checkin by: $Author: a_tan $

   This file contains a subset of the SOFTIMAGE's cluster data structure.

   Written by: Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_CLUSTER_H__
#define __TINY_CLUSTER_H__

#include  "tySoftTy.h"
#include  "tyModel.h"
#include  "tyFcurve.h"

#define DK_CLUSTER_ID                 "CLUS"

#define DK_CLUSTER_END_TOKEN          "End"

#define DK_CLUSTER_NODE_NAME_TOKEN    "name"
#define DK_CLUSTER_NODE_NCVS_TOKEN    "numCVs"
#define DK_CLUSTER_NODE_NKEYS_TOKEN   "numKeys"
#define DK_CLUSTER_NODE_INDEXES_TOKEN "indexes"
#define DK_CLUSTER_NODE_KEYS_TOKEN    "Cluster_key"

#define DK_CLUSTER_NODE_TOKEN         "Cluster_node"

#define DK_CLUSTER_TOKEN        "DK_clusters"
#define DK_CLUSTER_NAME_TOKEN   "DK_cluster_name"
#define DK_CLUSTER_NCV_TOKEN    "DK_cluster_ncvs"
#define DK_CLUSTER_VERT_TOKEN   "DK_cluster_vertices"
#define DK_CLUSTER_STATE_TOKEN  "DK_cluster_state"

typedef enum
   {
      DK_K_active,
      DK_K_unactive,
      DK_K_due2delete
   } DK_KLUSTER_STATE;

typedef struct DK_kluster
   {
      char		     *name;	/* kluster name */
      int		      ncv;	/* number of cv's */
      int		     *indexes;  /* cv ind., SORTED IN ASCENDING ORDER */
      DK_KLUSTER_STATE	      state;
      struct DK_clusterData  *anim;     /* optional animation */
      struct DK_kluster	     *next;
   } DK_Kluster;

typedef struct DK_clusterKey
    {
	DK_Pointer            vectors;	/* vector array */
	struct DK_clusterKey *next;	/* next cluster key */
    } DK_ClusterKey;

typedef struct DK_clusterData
    {
	int             active;		/* true if active */
	int             nbClusterKey;	/* number of cluster keys */
	short           clusterInterp;	/* 0 = LINEAR, 1 = CARDINAL, 
					   2 = WEIGHTS */
	DK_ClusterKey  *keys;		/* key data */
	DK_Fcurve     **fclusterWeights;/* weight fcurves */
	DK_actFcurve   *active_wfcv;	/* activation fcurves -weight */
	DK_Fcurve      *fcluster;	/* interpolation fcurve */
	DK_actFcurve    active_fcv;	/* activation fcurves -interp */
    } DK_ClusterData;

typedef struct DK_cluster
    {
	char *filename;			/* cluster filename */
	int   active;			/* true if all clusters active */
    } DK_Cluster;

/* global cluster prototypes */

extern 	DK_Cluster 	*DK_clusterAllocate ();
extern 	int 		DK_clusterDispose ( DK_Cluster **);
extern 	DK_ClusterData 	*DK_clusterDataAllocate ();
extern 	int 		DK_clusterDataDispose ( DK_ClusterData **);
extern  DK_ClusterKey 	*DK_clusterKeyAllocate ( int, int);
extern 	int 		DK_clusterKeyDispose ( DK_ClusterKey **);
extern 	int 		DK_clusterWeightFcurveAllocate ( DK_ClusterData *, int);
extern 	int 		DK_clusterWeightFcurveDispose ( DK_ClusterData *);
extern 	struct DK_kluster 	*DK_klusterFindByName ( struct DK_model *, char *);
extern 	int 		DK_clusterWriteAsciiAnimation ( FILE *, DK_Cluster *,
				struct DK_kluster *);
extern 	int 		DK_clusterReadAsciiAnimation ( FILE *, struct DK_model *);
extern 	int 		DK_clusterReadAsciiFile ( char *, struct DK_model *);
extern 	int 		DK_clusterWriteAsciiFile ( char *, struct DK_model *);

extern 	DK_Kluster 	*DK_klusterAllocate ();
extern 	void 		DK_klusterDispose (DK_Kluster **);
extern int 		DK_clusterCount ( struct DK_model *);
extern void 		DK_clustersReadBinaryChunk ( FILE *, struct DK_model *, 
				DK_FileProt *);
extern void 		DK_clustersWriteBinaryChunk ( FILE *, struct DK_model *);
#endif  /* ifndef __TINY_CLUSTER_H__ */
