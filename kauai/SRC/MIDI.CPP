/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Midi stream and midi stream parser classes.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(MSTP)
RTCLASS(MIDS)


/***************************************************************************
	Constructor for a midi stream parser.
***************************************************************************/
MSTP::MSTP(void)
{
	AssertBaseThis(0);
	_pmids = pvNil;
}


/***************************************************************************
	Destructor for a midi stream parser.
***************************************************************************/
MSTP::~MSTP(void)
{
	AssertThis(0);

	if (pvNil != _pmids)
		UnlockHq(_pmids->_hqrgb);
	ReleasePpo(&_pmids);
}


/***************************************************************************
	Initialize this midi stream parser to the given midi stream and use the
	given time as the start time.
***************************************************************************/
void MSTP::Init(PMIDS pmids, ulong tsStart, long lwTempo)
{
	AssertThis(0);
	AssertNilOrPo(pmids, 0);

	if (_pmids != pmids)
		{
		if (pvNil != _pmids)
			{
			UnlockHq(_pmids->_hqrgb);
			ReleasePpo(&_pmids);
			}
		_pmids = pmids;
		if (pvNil == _pmids)
			return;
		_pmids->AddRef();

		_prgb = (byte *)PvLockHq(_pmids->_hqrgb);
		_pbLim = _prgb + CbOfHq(_pmids->_hqrgb);
		}
	else if (pvNil == _pmids)
		return;

	_pbCur = _prgb;
	_tsCur = tsStart;
	_lwTempo = lwTempo;
	_bStatus = 0;
	AssertThis(0);
}


/***************************************************************************
	Get the next midi event. If fAdvance is true, advance to the next event
	after getting this one.
***************************************************************************/
bool MSTP::FGetEvent(PMIDEV pmidev, bool fAdvance)
{
	AssertThis(0);
	AssertNilOrVarMem(pmidev);

	byte bT;
	byte *pbCur;
	MIDEV midev;
	long cbT;
	long ibSend;
	ulong tsCur;

	ClearPb(&midev, size(midev));
	midev.lwTempo = _lwTempo;
	if (pvNil == _pmids)
		goto LFail;

	tsCur = _tsCur;
	for (pbCur = _pbCur; midev.cb == 0; )
		{
		// read the delta time
		if (!_FReadVar(&pbCur, (long *)&midev.ts) || pbCur >= _pbLim)
			goto LFail;
		tsCur += midev.ts;
		midev.ts = tsCur;

		if ((bT = *pbCur) & 0x80)
			{
			// status byte
			_bStatus = bT;
			pbCur++;
			}

		// NOTE: we never return running status
		midev.rgbSend[0] = _bStatus;
		ibSend = 1;

		switch (_bStatus & 0xF0)
			{
		default:
			goto LFail;

		case 0x80: // note off
		case 0x90: // note on
		case 0xA0: // key pressure
		case 0xB0: // control change
		case 0xE0: // pitch wheel
			goto LTwoBytes;

		case 0xC0: // program change
		case 0xD0: // channel pressure
			goto LOneByte;

		case 0xF0:
			switch (_bStatus & 0x0F)
				{
			default:
				// unknown opcode
				goto LFail;

			case 0x02:
LTwoBytes:
				// 2 bytes worth of parameters
				if (pbCur + 2 > _pbLim)
					goto LFail;
				midev.rgbSend[ibSend++] = *pbCur++;
				midev.rgbSend[ibSend++] = *pbCur++;
				midev.cb = ibSend;
				break;

			case 0x01: // quarter frame
			case 0x03: // song select
LOneByte:
				// 1 byte worth of parameters
				if (pbCur >= _pbLim)
					goto LFail;
				midev.rgbSend[ibSend++] = *pbCur++;
				midev.cb = ibSend;
				break;

			case 0x06:
			case 0x08:
			case 0x0A:
			case 0x0B:
			case 0x0C:
			case 0x0E:
				// no parameters
				_bStatus = 0; // can't use running status on this!
				midev.cb = ibSend;
				break;

			case 0x00:
			case 0x07:
				// system exclusive messages - ignore them
				// read the length
				if (!_FReadVar(&pbCur, &cbT) ||
						!FIn(cbT - 1, 0, _pbLim - pbCur))
					{
					goto LFail;
					}
				pbCur += cbT;
				break;

			case 0x0F:
				// meta event
				if (pbCur >= _pbLim)
					goto LFail;
				bT = *pbCur++;
				if (!_FReadVar(&pbCur, &cbT) ||
						!FIn(cbT, 0, _pbLim - pbCur + 1))
					{
					goto LFail;
					}

				switch (bT)
					{
				default:
					pbCur += cbT;
					break;

				case 0x2F: // end of track
					goto LFail;

				case 0x51: // tempo change
					if (cbT != 3)
						goto LFail;
					midev.lwTempo = LwFromBytes(0, pbCur[0], pbCur[1], pbCur[2]);
					if (fAdvance)
						_lwTempo = midev.lwTempo;
					pbCur += 3;
					goto LSend;
					}
				break;
				}
			break;
			}
		}

LSend:
	if (pvNil != pmidev)
		*pmidev = midev;
	if (fAdvance)
		{
		_pbCur = pbCur;
		_tsCur = tsCur;
		}
	return fTrue;

LFail:
	_pbCur = _pbLim;
	TrashVar(pmidev);
	return fFalse;
}


/***************************************************************************
	Read a variable length quantity.
***************************************************************************/
bool MSTP::_FReadVar(byte **ppbCur, long *plw)
{
	AssertThis(0);
	AssertVarMem(ppbCur);

	byte bT;

	*plw = 0;
	do
		{
		if (*ppbCur >= _pbLim)
			return fFalse;
		bT = *(*ppbCur)++;
		*plw = (*plw << 7) + (bT & 0x7F);
		}
	while (bT & 0x80);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MSTP.
***************************************************************************/
void MSTP::AssertValid(ulong grf)
{
	MSTP_PAR::AssertValid(0);
	if (pvNil == _pmids)
		return;

	AssertPo(_pmids, 0);
	Assert(_prgb == (byte *)QvFromHq(_pmids->_hqrgb), 0);
	Assert(_pbLim == _prgb + CbOfHq(_pmids->_hqrgb), 0);
	AssertIn(_pbCur - _prgb, 0, _pbLim - _prgb + 1);
}


/***************************************************************************
	Mark memory for the MSTP.
***************************************************************************/
void MSTP::MarkMem(void)
{
	AssertValid(0);
	MSTP_PAR::MarkMem();
	MarkMemObj(_pmids);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a midi stream object.
***************************************************************************/
MIDS::MIDS(void)
{
}


/***************************************************************************
	Destructor for a midi stream object.
***************************************************************************/
MIDS::~MIDS(void)
{
	FreePhq(&_hqrgb);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MIDS.
***************************************************************************/
void MIDS::AssertValid(ulong grf)
{
	MIDS_PAR::AssertValid(0);
	AssertHq(_hqrgb);
}


/***************************************************************************
	Mark memory for the MIDS.
***************************************************************************/
void MIDS::MarkMem(void)
{
	AssertValid(0);
	MIDS_PAR::MarkMem();
	MarkHq(_hqrgb);
}
#endif //DEBUG


/***************************************************************************
	A baco reader for a midi stream.
***************************************************************************/
bool MIDS::FReadMids(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, fblckReadable);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);
	PMIDS pmids;

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;
	*pcb = pblck->Cb();

	if (pvNil == (pmids = PmidsRead(pblck)))
		{
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	*ppbaco = pmids;
	return fTrue;
}


/***************************************************************************
	Read a midi stream from the given block.
***************************************************************************/
PMIDS MIDS::PmidsRead(PBLCK pblck)
{
	AssertPo(pblck, 0);

	PMIDS pmids;

	if (pvNil == (pmids = NewObj MIDS))
		return pvNil;

	if (!pblck->FUnpackData() || !pblck->FReadHq(&pmids->_hqrgb))
		ReleasePpo(&pmids);

	AssertNilOrPo(pmids, 0);
	return pmids;
}


/***************************************************************************
	Read a native standard midi file and create a midi stream from it.
***************************************************************************/
PMIDS MIDS::PmidsReadNative(FNI *pfni)
{
	AssertPo(pfni, ffniFile);

	#pragma pack(1)
	// Midi chunk header
	struct MIDCHD
		{
		long lwSig;
		long cb;
		};
	#define kbomMidchd 0xF0000000
	
	// Midi file header chunk - should be first chunk
	struct MIDHED
		{
		MIDCHD midchd;
		short swFmt;
		short ctrk;
		short swDiv;
		};
	#define kbomMidhed 0xF5400000
	#pragma pack()

	struct MIDTR
		{
		PMSTP pmstp;
		MIDEV midevCur;
		};

	FLO flo;
	FP fp;
	MIDHED midhed;
	MIDCHD midchd;
	MIDTR midtr;
	bool fSmpte;
	BSM bsm;
	RAT ratTempo;
	ulong tsTempo, tsRawTempo, tsLast, ts, dts;
	byte rgbT[5];
	long cbT;
	bool fSeq;
	long imidtr, imidtrMin;
	ulong tsMin;
	PMIDS pmids = pvNil;
	PGL pglmidtr = pvNil;

	// open the file and set up the source flo
	if (pvNil == (flo.pfil = FIL::PfilOpen(pfni)))
		return pvNil;
	flo.fp = 0;
	flo.cb = flo.pfil->FpMac();

	// get the header chunk
	if (flo.cb < size(MIDHED) || !flo.FReadRgb(&midhed, size(midhed), 0))
		goto LFail;

	// byte order is always big endian
#ifdef LITTLE_ENDIAN
	SwapBytesBom(&midhed, kbomMidhed);
#endif //LITTLE_ENDIAN

	// make sure it's a valid header chunk
	if (midhed.midchd.lwSig != 'MThd' ||
			!FIn(midhed.midchd.cb + size(midchd), size(midhed), flo.cb))
		{
		goto LFail;
		}

	// allocate the list of tracks to parse
	if (pvNil == (pglmidtr = GL::PglNew(size(MIDTR))))
		goto LFail;

	// build the track list...
	for (fp = midhed.midchd.cb + size(midchd); fp < flo.cb; )
		{
		// read the next midi chunk header
		if (fp + size(midchd) > flo.cb ||
				!flo.FReadRgb(&midchd, size(midchd), fp))
			{
			goto LFail;
			}
		fp += size(midchd);

#ifdef LITTLE_ENDIAN
		SwapBytesBom(&midchd, kbomMidchd);
#endif //LITTLE_ENDIAN

		// make sure the chunk length is valid
		if (!FIn(midchd.cb, 0, flo.cb - fp + 1))
			goto LFail;

		// if it's not a track chunk ignore it
		if (midchd.lwSig != 'MTrk' || midchd.cb == 0)
			{
			fp += midchd.cb;
			continue;
			}

		// wrap a midi stream object around the track data
		if (pvNil == (pmids = NewObj MIDS) ||
				!flo.FReadHq(&pmids->_hqrgb, midchd.cb, fp))
			{
			goto LFail;
			}
		fp += midchd.cb;

		// create the midi stream parser for this stream
		if (pvNil == (midtr.pmstp = NewObj MSTP))
			goto LFail;
		midtr.pmstp->Init(pmids, 0, 500000 /* microseconds per beat */);
		ReleasePpo(&pmids);

		// get the first event - if there isn't one, just free
		// the stream parser and continue
		if (!midtr.pmstp->FGetEvent(&midtr.midevCur))
			{
			ReleasePpo(&midtr.pmstp);
			continue;
			}

		// add the track to the list
		if (!pglmidtr->FAdd(&midtr))
			{
			ReleasePpo(&midtr.pmstp);
			goto LFail;
			}

		if (midhed.swFmt != 1 && midhed.swFmt != 2)
			{
			// we're only supposed to have one track and we have it,
			// so don't bother looking for more.
			break;
			}
		}
	ReleasePpo(&flo.pfil);

	// make sure we have at least one track
	if (pglmidtr->IvMac() == 0)
		goto LFail;

	// set the amount to grow the bsm by
	bsm.SetMinGrow(1024);

	if (FPure(fSmpte = (midhed.swDiv < 0)))
		{
		// SMPTE time
		long fps = (byte)(-BHigh(midhed.swDiv));
		long ctickFrame = BLow(midhed.swDiv);

		if (fps == 29)
			fps = 30; // 29 is 30 drop frame - 30 is close enough for us

		if (ctickFrame <= 0 || fps <= 0)
			goto LFail;
		ratTempo.Set(1000, LwMul(fps, ctickFrame));
		}
	else
		{
		// midhed.swDiv is the number of ticks per beat.
		if (midhed.swDiv == 0)
			goto LFail;

		// assume 120 beats per minute - gives us 500 ms per beat
		ratTempo.Set(500, midhed.swDiv);
		}

	// merge the tracks or play them in sequence (according to fSeq).
	fSeq = pglmidtr->IvMac() == 1 || midhed.swFmt == 2;

	tsTempo = tsRawTempo = 0;
	tsLast = 0;
	if (fSeq)
		{
		imidtrMin = 0;
		pglmidtr->Get(0, &midtr);
		}

	for (;;)
		{
		if (!fSeq)
			{
			// find the track with the next event
			imidtrMin = ivNil;
			tsMin = kluMax;
			for (imidtr = 0; imidtr < pglmidtr->IvMac(); imidtr++)
				{
				pglmidtr->Get(imidtr, &midtr);
				if (midtr.midevCur.ts < tsMin)
					{
					tsMin = midtr.midevCur.ts;
					imidtrMin = imidtr;
					}
				}

			Assert(imidtrMin != ivNil, 0);
			pglmidtr->Get(imidtrMin, &midtr);
			}

		// get the time of this event
		ts = ratTempo.LwScale(midtr.midevCur.ts - tsRawTempo) + tsTempo;

		if (midtr.midevCur.cb == 0)
			{
			// just a tempo change
			if (!fSmpte)
				{
				RAT ratTempoNew(midtr.midevCur.lwTempo,
					LwMul(1000, midhed.swDiv));

				if (ratTempo != ratTempoNew)
					{
					ratTempo = ratTempoNew;
					tsRawTempo = midtr.midevCur.ts;
					tsTempo = ts;
					}
				}
			}
		else
			{
			// write the time out - in variable format
			Assert(midtr.midevCur.cb > 0, 0);
			dts = ts - tsLast;
			cbT = _CbEncodeLu(dts, rgbT);
			if (!bsm.FReplace(rgbT, cbT, bsm.IbMac(), 0))
				goto LFail;

			if (!bsm.FReplace(midtr.midevCur.rgbSend,
					midtr.midevCur.cb, bsm.IbMac(), 0))
				{
				goto LFail;
				}
			}
		tsLast = ts;

		if (midtr.pmstp->FGetEvent(&midtr.midevCur))
			{
			if (!fSeq)
				pglmidtr->Put(imidtrMin, &midtr);
			}
		else
			{
			// imidtrMin is empty
			ReleasePpo(&midtr.pmstp);
			pglmidtr->Delete(imidtrMin);
			if (0 == pglmidtr->IvMac())
				break;
			if (fSeq)
				{
				tsTempo = tsLast;
				tsRawTempo = 0;
				pglmidtr->Get(0, &midtr);
				}
			}
		}

	if (pvNil != (pmids = NewObj MIDS) &&
			FAllocHq(&pmids->_hqrgb, bsm.IbMac(), fmemNil, mprNormal))
		{
		bsm.FetchRgb(0, bsm.IbMac(), PvLockHq(pmids->_hqrgb));
		UnlockHq(pmids->_hqrgb);
		}
	else
		{
LFail:
		ReleasePpo(&pmids);
		}

	// clean up stuff
	ReleasePpo(&flo.pfil);
	if (pvNil != pglmidtr)
		{
		while (pglmidtr->FPop(&midtr))
			ReleasePpo(&midtr.pmstp);
		ReleasePpo(&pglmidtr);
		}

	AssertNilOrPo(pmids, 0);
	return pmids;
}


/***************************************************************************
	Static method to convert a long to its midi file variable length
	equivalent.
***************************************************************************/
long MIDS::_CbEncodeLu(ulong lu, byte *prgb)
{
	AssertNilOrVarMem(prgb);

	long ib;

	if (pvNil != prgb)
		prgb[0] = (byte)(lu & 0x7F);
	for (ib = 1; (lu >>= 7) > 0; ib++)
		{
		if (pvNil != prgb)
			prgb[ib] = (byte)((lu & 0x7F) | 0x80);
		}
	if (pvNil != prgb)
		ReversePb(prgb, ib);
	return ib;
}


/***************************************************************************
	Write a midi stream to the given block.
***************************************************************************/
bool MIDS::FWrite(PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);

	return pblck->FWriteHq(_hqrgb, 0);
}


/***************************************************************************
	Return the length of this midi stream on file.
***************************************************************************/
long MIDS::CbOnFile(void)
{
	AssertThis(0);

	return CbOfHq(_hqrgb);
}


