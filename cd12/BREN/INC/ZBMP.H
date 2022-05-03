/*************************************************************************

	zbmp.h: Z-buffer Bitmap Class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!
	
	BASE ---> BACO ---> ZBMP

*************************************************************************/
#ifndef ZBMP_H
#define ZBMP_H

#define kcbPixelZbmp 2 // Z-buffers are 2 bytes per pixel (16 bit)

// ZBMP on file
struct ZBMPF
	{
	short bo;
	short osk;
	short xpLeft;
	short ypTop;
	short dxp;
	short dyp;
	// void *rgb; 		// pixels follow immediately after ZBMPF
	};
const ulong kbomZbmpf = 0x55500000;


/****************************************
	ZBMP class
****************************************/
typedef class ZBMP *PZBMP;
#define ZBMP_PAR BACO
#define kclsZBMP 'ZBMP'
class ZBMP : public ZBMP_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	RC _rc;			// bounding rectangle of ZBMP
	long _cbRow;	// bytes per row
	long _cb;		// count of bytes in Z buffer
	byte *_prgb;	// Z buffer
	ZBMP(void) {}

public:
	static PZBMP PzbmpNew(long dxp, long dyp);
	static PZBMP PzbmpNewFromBpmp(BPMP *pbpmp);
	static PZBMP PzbmpRead(PBLCK pblck);
	static bool FReadZbmp(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	~ZBMP(void);

	byte *Prgb(void) { return _prgb; }
	long CbRow(void) { return _cbRow; }

	void Draw(byte *prgbPixels, long cbRow, long dyp, long xpRef,
		long ypRef, RC *prcClip = pvNil, PREGN pregnClip = pvNil);
	void DrawHalf(byte *prgbPixels, long cbRow, long dyp, long xpRef,
		long ypRef, RC *prcClip = pvNil, PREGN pregnClip = pvNil);

	bool FWrite(PCFL pcfl, CTG ctg, CNO *pcno);
	};


#endif ZBMP_H
