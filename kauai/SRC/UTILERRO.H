/******************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Error registration and reporting.  All error codes go here.

******************************************************************************/
#ifndef UTILERROR_H
#define UTILERROR_H

const long kcerdMax = 20;


/***************************************************************************
	Error stack class
***************************************************************************/
#define ERS_PAR BASE
#define kclsERS 'ERS'
class ERS : public ERS_PAR
	{
	RTCLASS_DEC
	ASSERT

private:
	struct ERD
		{
		long erc;
		#ifdef DEBUG
			PSZS pszsFile;
			long lwLine;
		#endif //DEBUG
		};

	MUTX _mutx;
	long _cerd;
	ERD _rgerd[kcerdMax];

public:
	ERS(void);

	#ifdef DEBUG
		virtual void Push(long erc, schar *pszsFile, long lwLine);
	#else //!DEBUG
		virtual void Push(long erc);
	#endif //!DEBUG
	virtual bool FPop(long *perc = pvNil);
	virtual bool FIn(long erc);
	virtual long Cerc(void);
	virtual long ErcGet(long ierc);
	virtual void Clear(void);
	virtual void Flush(long erc);
	};

extern ERS *vpers;

#ifdef DEBUG
#define PushErc(erc) vpers->Push(erc, __szsFile, __LINE__)
#else //!DEBUG
#define PushErc(erc) vpers->Push(erc)
#endif //!DEBUG

#endif //!UTILERROR_H
