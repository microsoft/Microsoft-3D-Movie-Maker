/***************************************************************************

	bwld.h: BRender world class

	Primary Author: manders
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> BWLD

***************************************************************************/
#ifndef BWLD_H
#define BWLD_H

// Callback function per BACT when it's rendered, passing the 2D bounds
typedef void FNBACTREND(PBACT pbact, RC *prc);
typedef FNBACTREND *PFNBACTREND;

// Callback function per root BACT when we begin rendering
typedef void FNBEGINREND(PBACT pbact);
typedef FNBEGINREND *PFNBEGINREND;

// Callback function per root BACT to get the rendered rectangle
typedef void FNGETRECT(PBACT pbact, RC *prc);
typedef FNGETRECT *PFNGETRECT;


/****************************************
	The BRender world class
****************************************/
typedef class BWLD *PBWLD;
#define BWLD_PAR BASE
#define kclsBWLD 'BWLD'
class BWLD : public BWLD_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	static bool _fBRenderInited;// Whether BrBegin() has been called
	RC _rcBuffer;				// Bounds of the rendering space
	RC _rcView;					// Bounds of view
	BACT _bactWorld;			// The world root actor
	BACT _bactCamera;			// The camera actor
	BCAM _bcam;					// The camera data
	PGPT _pgptBackground;		// Background RGB bitmap
	PGPT _pgptWorking;			// RGB working buffer to render into
	PGPT _pgptStretch;			// Stretched working buffer (if _fhalfY)
	BPMP _bpmpRGB;				// BRender wrapper around _pgptWorking
	PZBMP _pzbmpBackground;		// Background Z-buffer
	PZBMP _pzbmpWorking;		// Working Z-buffer to render into
	BPMP _bpmpZ;				// BRender wrapper around _pzbmpWorking
	PREGN _pregnDirtyWorking; 	// Rgn to copy from bkgd to working buffer
	PREGN _pregnDirtyScreen;  	// Rgn to copy from working buffer to screen
	bool _fHalfX;				// Render at half horizontal resolution
	bool _fHalfY;				// Render at half vertical resolution
	bool _fWorldChanged;		// Need to rerender?
	PFNBEGINREND _pfnbeginrend;	// Callback to each actor before rendering
	PFNBACTREND _pfnbactrend;	// Callback when an actor is rendered
	PFNGETRECT _pfngetrect;		// Callback to get an actor's bounding rect
	PBACT _pbactClosestClicked; // The closest actor that has been clicked
	BRS _dzpClosestClicked;		// Distance of the closest clicked actor
	// Keep reference to last background in case we switch to/from halfmode:
	PCRF _pcrf;
	CTG _ctgRGB;
	CNO _cnoRGB;
	CTG _ctgZ;
	CNO _cnoZ;

protected:
	BWLD(void) {}
	bool _FInit(long dxp, long dyp, bool fHalfX, bool fHalfY);
	bool _FInitBuffers(long dxp, long dyp, bool fHalfX, bool fHalfY);
	void _CleanWorkingBuffers(void);
	static int BR_CALLBACK _FFilter(BACT *pbact, PBMDL pbmdl, PBMTL pbmtl,
		BVEC3 *pbvec3RayPos, BVEC3 *pbvec3RayDir, BRS dzpNear, BRS dzpFar, 
		void *pbwld);
	static void BR_CALLBACK _ActorRendered(PBACT pbact, PBMDL pbmdl, 
		PBMTL pbmtl, br_uint_8 bStyle, br_matrix4 *pbmat4ModelToScreen,
		br_int_32 bounds[4]);
	
public:
	// Constructors and destructors
	static PBWLD PbwldNew(long dxp, long dyp, bool fHalfX = fFalse,
		bool fhalfY = fFalse);
	~BWLD();
	static void CloseBRender(void);

	// Dirtying the BRender world and bitmap
	void MarkDirty(void) { _fWorldChanged = fTrue; }
	void MarkRenderedRegn(PGOB pgob, long dxp, long dyp);

	// Background stuff
	bool FSetBackground(PCRF pcrf, CTG ctgRGB, CNO cnoRGB, CTG ctgZ,
		CNO cnoZ);
	void SetCamera(BMAT34 *pbmat34, BRS zrHither, BRS zrYon, BRA aFov);
	void GetCamera(BMAT34 *pbmat34, BRS *pzrHither = pvNil,
		BRS *pzrYon = pvNil, BRA *paFov = pvNil);

	// Actor stuff
	void AddActor(BACT *pbact);
	bool FClickedActor(long xp, long yp, BACT **ppbact);
	void IterateActorsInPt(br_pick2d_cbfn *pfnCallback, void *pvArg, 
		long xp, long yp);
	void SetBeginRenderCallback(PFNBEGINREND pfnbeginrend)
		{ _pfnbeginrend = pfnbeginrend; }
	void SetActorRenderedCallback(PFNBACTREND pfnbactrend)
		{ _pfnbactrend = pfnbactrend; }
	void SetGetRcCallback(PFNGETRECT pfngetrect)
		{ _pfngetrect = pfngetrect; }

	// Rendering stuff
	bool FSetHalfMode(bool fHalfX, bool fHalfY);
	bool FHalfX(void) { return _fHalfX; }
	bool FHalfY(void) { return _fHalfY; }
	void Render(void);
	void Prerender(void);
	void Unprerender(void);
	void Draw(PGNV pgnv, RC *prcClip, long dxp, long dyp);

#ifdef DEBUG
	bool FWriteBmp(PFNI pfni);
#endif // DEBUG
	};


#endif BWLD_H
