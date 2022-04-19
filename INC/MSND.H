/***************************************************************************

	msnd.h: Movie sound class

	Primary Authors: susano, manders
	Status:  Reviewed

	BASE ---> BACO ---> MSND
	BASE ---> CMH  ---> MSQ

	NOTE: when the MSQ stops sounds, it does it based on sound class (scl)
	and not sound queue (sqn).  This is slightly less efficient, because the
	SNDM must search all open sound queues for the given scl's when we stop
	sounds; however, the code is made much simpler, because the sqn is
	generated on the fly based on whether the sound is for an actor or
	background, the sty of the sound, and (in the case of actor sounds) the
	arid of the source of the sound.  If we had to enumerate all sounds
	based on that information, we'd wind up calling into the SNDM a minimum
	of three times, plus three times for each actor; not only is the
	enumeration on this side inefficient (the MSQ would have to call into the
	MVIE to enumerate all the known actors), but the number of calls to SNDM
	gets to be huge!  On top of all that, we'd probably wind up finding some
	bugs where a sound is still playing for an actor that's been deleted, and
	possibly fail to stop the sound properly (Murphy reigning strong in any
	software project).

***************************************************************************/
#ifndef MSND_H
#define MSND_H

// Sound types
enum
	{	
	styNil = 0,
	styUnused, 			// Retain.  Existing content depends on subsequent values
	stySfx,
	stySpeech,
	styMidi,
	styLim
	};

// Sound-class-number constants
const long sclNonLoop = 0;
const long sclLoopWav = 1;
const long sclLoopMidi = 2;

#define vlmNil (-1)

// Sound-queue-number constants
enum
	{
	sqnActr =  0x10000000,
	sqnBkgd = 0x20000000,
	sqnLim
	};
#define ksqnStyShift 	16;	  	// Shift for the sqnsty

// Sound Queue Delta times
// Any sound times less than ksqdtimLong will be clocked & stopped
const long kdtimOffMsq = 0;
const long kdtimLongMsq = klwMax;
const long kdtim2Msq = ((kdtimSecond *2) * 10) / 12;	// adjustment -> 2 seconds
const long kSndSamplesPerSec = 22050;
const long kSndBitsPerSample = 8;
const long kSndBlockAlign = 1;
const long kSndChannels = 1;

/****************************************

	Movie Sound on file

****************************************/
struct MSNDF
	{
	short bo;
	short osk;
	long sty;			// sound type
	long vlmDefault;	// default volume
	bool fInvalid;		// Invalid flag
	};
const BOM kbomMsndf = 0x5FC00000;

const CHID kchidSnd = 0;  // Movie Sound sound/music

// Function to stop all movie sounds.
inline void StopAllMovieSounds(void)
{
	vpsndm->StopAll(sqnNil, sclNonLoop);
	vpsndm->StopAll(sqnNil, sclLoopWav);
	vpsndm->StopAll(sqnNil, sclLoopMidi);
}

/****************************************

	The Movie Sound class

****************************************/
typedef class MSND *PMSND;
#define MSND_PAR BACO
#define kclsMSND 'MSND'
class MSND : public MSND_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	// these are inherent to the msnd
	CTG _ctgSnd;	// CTG of the WAV or MIDI chunk
	CNO _cnoSnd;	// CNO of the WAV or MIDI chunk
	PRCA _prca;		// file that the WAV/MIDI lives in
	long _sty;		// MIDI, speech, or sfx
	long _vlm;		// Volume of the sound
	bool _fNoSound;	// Set if silent sound
	STN _stn;		// Sound name
	bool _fInvalid;	// Invalid flag

protected:
	bool _FInit(PCFL pcfl, CTG ctg, CNO cno);

public:
    static bool FReadMsnd(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
    	PBACO *ppbaco, long *pcb);
	static bool FGetMsndInfo(PCFL pcfl, CTG ctg, CNO cno, bool *pfInvalid = pvNil,
		long *psty = pvNil, long *pvlm = pvNil);
	static bool FCopyMidi(PFIL pfilSrc, PCFL pcflDest, CNO *pcno, PSTN pstn = pvNil);
	static bool FWriteMidi(PCFL pcflDest, PMIDS pmids, STN *pstnName, CNO *pcno);
	static bool FCopyWave(PFIL pfilSrc, PCFL pcflDest, long sty, CNO *pcno, PSTN pstn = pvNil);
	static bool FWriteWave(PFIL pfilSrc, PCFL pcflDest, long sty, STN *pstnName, CNO *pcno);
	~MSND(void);

	static long SqnActr(long sty, long objID);
	static long SqnBkgd(long sty, long objID);
	long Scl(bool fLoop) { return (fLoop ? ((_sty == styMidi) ? sclLoopMidi : sclLoopWav) : sclNonLoop);}
	long SqnActr(long objID) { AssertThis(0); return SqnActr(_sty, objID); }
	long SqnBkgd(long objID) { AssertThis(0); return SqnBkgd(_sty, objID); }

	bool FInvalidate(void);
	bool FValid(void) { AssertBaseThis(0); return FPure(!_fInvalid); }
	PSTN Pstn(void) { AssertThis(0); return &_stn; }
	long Sty(void) { AssertThis(0); return _sty; }
	long Vlm(void) { AssertThis(0); return _vlm; }
 	long Spr(long tool);		// Return Priority
	long FNoSound(void) { AssertThis(0); return _fNoSound; }

	void Play(long objID, bool fLoop, bool fQueue, long vlm,
		long spr, bool fActr = fFalse, ulong dtsStart = 0);

	};

/****************************************

	Movie Sound Queue  (MSQ)
	Sounds to be played at one time.
	These are of all types, queues &
	classes

****************************************/
typedef class MSQ *PMSQ;
#define MSQ_PAR CMH
#define kclsMSQ 'MSQ'

const long kcsqeGrow = 10;						// quantum growth for sqe

// Movie sound queue entry
struct SQE
	{
	long objID;	  		// Unique identifier (actor id, eg)
	bool fLoop;			// Looping sound flag
	bool fQueue;		// Queued sound
	long vlmMod; 		// Volume modification
	long spr; 			// Priority
	bool fActr;			// Actor vs Scene (to generate unique class)
	PMSND pmsnd;		// PMSND
	ulong dtsStart;		// How far into the sound to start playing
	};						

class MSQ : public MSQ_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(MSQ)

protected:
	PGL _pglsqe;		// Sound queue entries
	long _dtim;			// Time sound allowed to play
	PCLOK _pclok;

public:
	MSQ(long hid) : MSQ_PAR(hid) {}
	~MSQ(void);

	static PMSQ PmsqNew(void);
	
	bool FEnqueue(PMSND pmsnd, long objID, bool fLoop, bool fQueue,
		long vlm, long spr, bool fActr = fFalse, ulong dtsStart = 0, 
		bool fLowPri = fFalse);
	void PlayMsq(void);		// Destroys queue as it plays
	void FlushMsq(void);	// Without playing the sounds
	bool FCmdAlarm(PCMD pcmd);

	// Sound on/off & duration control
	void SndOff(void) {AssertThis(0); _dtim = kdtimOffMsq;}
	void SndOnShort(void) {AssertThis(0); _dtim = kdtim2Msq;}
	void SndOnLong(void) {AssertThis(0); _dtim = kdtimLongMsq;}	
	void StopAll(void)
	{
		if (pvNil != _pclok)
			_pclok->Stop();

		StopAllMovieSounds();
	}
	bool FPlaying(bool fLoop) {AssertThis(0);
		return (fLoop ? (vpsndm->FPlayingAll(sqnNil, sclLoopMidi) ||  vpsndm->FPlayingAll(sqnNil, sclLoopWav)) :
						vpsndm->FPlayingAll(sqnNil, sclNonLoop)); }

	// Save/Restore snd-on duration times
	long DtimSnd(void) {AssertThis(0); return _dtim;}
	void SndOnDtim(long dtim) {AssertThis(0); _dtim = dtim;}
};

#endif MSND_H
