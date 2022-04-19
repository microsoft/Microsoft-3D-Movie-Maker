/***************************************************************************

	Status: All changes must be code reviewed.

	Textbox Class

		Textbox (TBOX)

			TXRD ---> TBOX

    Drawing stuff

		Textbox border (TBXB)

			GOB  ---> TBXB

        Textbox Ddg (TBXG)

			TXRG ---> TBXG  (created as a child Gob of a TBXB)

    Cut/Copy/Paste Stuff

		Clipboard object (TCLP)

			DOCB ---> TCLP

***************************************************************************/

#ifndef TBOX_H
#define TBOX_H


//
// Defines for global text box constant values
//
#define kdzpBorderTbox 5					// Width of the border in pixels
#define kdxpMinTbox 16 + 2 * kdxpIndentTxtg	// Minimum Width of a tbox in pixels
#define kdypMinTbox 12						// Minimum Height of a tbox in pixels
#define kxpDefaultTbox	177					// Default location of a tbox
#define kypDefaultTbox	78					// Default location of a tbox
#define kdxpDefaultTbox	140					// Default width of a tbox
#define kdypDefaultTbox	100					// Default height of a tbox


//
//
// The border for a single textbox (TBXB)
//
//


//
// Definitions for each of the anchor points in a border
//
enum TBXT
	{
    tbxtUp,
	tbxtUpRight,
	tbxtRight,
	tbxtDownRight,
	tbxtDown,
	tbxtDownLeft,
	tbxtLeft,
	tbxtUpLeft,
	tbxtMove
	};

#define TBXB_PAR GOB

typedef class TBXB *PTBXB;
#define kclsTBXB 'TBXB'
class TBXB : public TBXB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:

	PTBOX _ptbox;						// Owning text box.
	bool _fTrackingMouse;				// Are we tracking the mouse.
	TBXT _tbxt;							// The anchor point being dragged.
	long _xpPrev;						// Previous x coord of the mouse.
	long _ypPrev;						// Previous y coord of the mouse.
	RC _rcOrig;							// Original size of the border.

 	TBXB(PTBOX ptbox, PGCB pgcb) : GOB(pgcb) { _ptbox = ptbox; }

	TBXT _TbxtAnchor(long xp, long yp);	// Returns the anchor point the mouse is at.

public:

    //
	// Creates a text box with border
	//
	static PTBXB PtbxbNew(PTBOX ptbox, PGCB pgcb);
	
	//
	// Overridden routines
	//
	void Draw(PGNV pgnv, RC *prcClip);
	void Activate(bool fActive);
	virtual bool FPtIn(long xp, long yp);
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);


	void AttachToMouse(void);
};


//
//
// The DDG for a single textbox (TBXG).
//
//

#define TBXG_PAR TXRG

typedef class TBXG *PTBXG;
#define kclsTBXG 'TBXG'
class TBXG : public TBXG_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(TBXG)

private:

	PTBXB _ptbxb;						// Enclosing border.
	RC _rcOld;							// Old rectangle for the ddg.

	TBXG(PTXRD ptxrd, PGCB pgcb) : TXRG(ptxrd, pgcb) {}
	~TBXG(void);

public:

	//
	// Creation function
	//
	static PTBXG PtbxgNew(PTBOX ptbox, PGCB pgcb);

	//
	// Accessors
	//
	void SetTbxb(PTBXB ptbxb) { _ptbxb = ptbxb; }
	PTBXB Ptbxb(void) { return _ptbxb; }

	//
	// Scrolling
	//
	bool FNeedToScroll(void);		  	// Does this text box need to scroll anything
	void Scroll(long scaVert);			// Scrolls to beginning or a single pixel only.

	//
	// Overridden routines
	//
	virtual bool FPtIn(long xp, long yp);
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdClip(PCMD pcmd);
	virtual bool FEnableDdgCmd(PCMD pcmd, ulong *pgrfeds);
	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual long _DxpDoc(void);
	virtual void _NewRc(void);
	virtual void InvalCp(long cp, long ccpIns, long ccpDel);
	void Activate(bool fActive);
	virtual void _FetchChp(long cp, PCHP pchp, long *pcpMin = pvNil, long *pcpLim = pvNil);

	//
	// Status
	//
	bool FTextSelected(void);

	//
	// Only for TBXB
	//
	bool _FDoClip(long tool);	// Actually does a clipboard command.
};


enum
	{
	grfchpNil = 0,
	kfchpOnn = 0x01,
	kfchpDypFont = 0x02,
	kfchpBold = 0x04,
	kfchpItalic = 0x08
	};
const ulong kgrfchpAll = (kfchpOnn | kfchpDypFont | kfchpBold | kfchpItalic);

//
//
// Text box document class (TBOX).
//
//
typedef class TBOX *PTBOX;

#define TBOX_PAR TXRD
#define kclsTBOX 'TBOX'
class TBOX : public TBOX_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:

	PSCEN _pscen;					// The owning scene
	long _nfrmFirst;				// Frame the tbox appears in.
	long _nfrmMax;					// Frame the tbox disappears in.
	long _nfrmCur;					// Current frame number.
	bool _fSel;						// Is this tbox selected?
	bool _fStory;					// Is this a story text box.
	RC _rc;							// Size of text box.

	TBOX(void) : TXRD() {}

public:
 	//
	// Creation routines
	//
	static PTBOX PtboxNew(PSCEN pscen=pvNil, RC *prcRel=pvNil, bool fStory = fTrue);
	PDDG PddgNew(PGCB pgcb) { return TBXG::PtbxgNew(this, pgcb); }
	static PTBOX PtboxRead(PCRF pcrf, CNO cno, PSCEN pscen);
	bool FWrite(PCFL pcfl, CNO cno);
	bool FDup(PTBOX *pptbox);

	//
	// Movie specific functions
	//
	void SetScen(PSCEN pscen);
	bool FIsVisible(void);
	bool FGotoFrame(long nfrm);
	void Select(bool fSel);
	bool FSelected(void) { return _fSel; }
	bool FGetLifetime(long *pnfrmStart, long *pnfrmLast);
	bool FShowCore(void);
	bool FShow(void);
	void HideCore(void);
	bool FHide(void);
	bool FStory(void) { return _fStory; }
	void SetTypeCore(bool fStory);
	bool FSetType(bool fStory);
	bool FNeedToScroll(void);
	void Scroll(void);
	PSCEN Pscen(void) { return _pscen; }
	bool FTextSelected(void);
	bool FSetAcrBack(ACR acr);
	bool FSetAcrText(ACR acr);
	bool FSetOnnText(long onn);
	bool FSetDypFontText(long dypFont);
	bool FSetStyleText(ulong grfont);
	void SetStartFrame(long nfrm);
	void SetOnnDef(long onn) { _onnDef = onn; }
	void SetDypFontDef(long dypFont) { _dypFontDef = dypFont; }
	void FetchChpSel(PCHP pchp, ulong *pgrfchp);
	void AttachToMouse(void);

	//
	// Overridden functions
	//
	void SetDirty(bool fDirty = fTrue);
	virtual bool FAddUndo(PUNDB pundb);
	virtual void ClearUndo(void);
	void ParClearUndo(void) { TBOX_PAR::ClearUndo(); }

	//
	// TBXG/TBXB specific funtions
	//
	void GetRc(RC *prc) { *prc= _rc; }
	void SetRc(RC *prc);
	void CleanDdg(void);
	long Itbox(void);

	//
	// Undo access functions, not for use by anyone but tbox.cpp
	//
	long NfrmFirst(void) { return _nfrmFirst; }
	long nfrmMax(void) { return _nfrmMax; }
};


//
//
// Textbox document for clipping
//
//
typedef class TCLP *PTCLP;

#define TCLP_PAR DOCB
#define kclsTCLP 'TCLP'
class TCLP : public TCLP_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	PTBOX _ptbox;						// Text box copy.
	TCLP(void) {}

public:

	//
	// Constructors and destructors
	//
	static PTCLP PtclpNew(PTBOX ptbox);
	~TCLP(void);

	//
	// Pasting
	//
	bool FPaste(PSCEN pscen);
	};

#endif

