/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Random number generator and shuffler stuff.

***************************************************************************/
#ifndef UTILRND_H
#define UTILRND_H


/***************************************************************************
	A pseudo-random number generator. LwNext returns values from 0 to
	(lwLim - 1), inclusive.
***************************************************************************/
typedef class RND *PRND;
#define RND_PAR BASE
#define kclsRND 'RND'
class RND : public RND_PAR
	{
	RTCLASS_DEC
	NOCOPY(RND)

protected:
	ulong _luSeed;

public:
	RND(ulong luSeed = 0L);
	virtual long LwNext(long lwLim);
	};


/***************************************************************************
	A shuffled array of numbers.
***************************************************************************/
typedef class SFL *PSFL;
#define SFL_PAR RND
#define kclsSFL 'SFL'
class SFL : public SFL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(SFL)

protected:
	long _clw;
	long _ilw;
	HQ _hqrglw;
	bool _fCustom;	// false iff the values in the hq are [0, _clw)

	bool _FEnsureHq(long clw);
	void _ShuffleCore(void);

public:
	SFL(ulong luSeed = 0L);
	~SFL(void);
	void Shuffle(long lwLim);
	void ShuffleRglw(long clw, long *prglw);

	virtual long LwNext(long lwLim = 0);
	};

#endif //UTILRND_H

