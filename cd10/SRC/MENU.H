/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Menu bar management.

***************************************************************************/
#ifndef MENU_H
#define MENU_H


// Menu Bar class
typedef class MUB *PMUB;
#define MUB_PAR BASE
#define kclsMUB 'MUB'
class MUB : public MUB_PAR
	{
	RTCLASS_DEC
	MARKMEM

private:
	#ifdef MAC
		// System Menu
		typedef MenuInfo SMU;

		// System Menu Bar
		struct SMB
			{
			ushort cmid;
			ushort rgmid[1];
			};

		// Menu Item
		struct MNI
			{
			long cid;
			long lw0;
			};

		// Menu
		struct MNU
			{
			long mid;
			SMU **hnsmu;
			PGL pglmni;
			};

		//menu list
		struct MLST
			{
			long imnu;
			long imniBase;
			long cmni;
			long cid;
			bool fSeparator;
			};

		HN _hnmbar;
		PGL _pglmnu;
		PGL _pglmlst;	//menu lists

		bool _FInsertMni(long imnu, long imni, long cid, long lw0, PSTN pstn);
		void _DeleteMni(long imnu, long imni);
		bool _FFindMlst(long imnu, long imni, MLST *pmlst = pvNil,
			long *pimlst = pvNil);
		bool _FGetCmdFromCode(long lwCode, CMD *pcmd);
		void _Free(void);
		bool _FFetchRes(ulong ridMenuBar);
	#endif //MAC

	#ifdef WIN
		//menu list
		struct MLST
			{
			HMENU hmenu;
			long imniBase;
			long wcidList;
			long cid;
			bool fSeparator;
			PGL pgllw;
			};

		HMENU _hmenu;	//the menu bar
		long _cmnu;		//number of menus on the menu bar
		PGL _pglmlst;	//menu lists

		bool _FInitLists(void);
		bool _FFindMlst(long wcid, MLST *pmlst, long *pimlst = pvNil);
		bool _FGetCmdForWcid(long wcid, PCMD pcmd);
	#endif //WIN

protected:
	MUB(void) {}

public:
	~MUB(void);

	static PMUB PmubNew(ulong ridMenuBar);

	virtual void Set(void);
	virtual void Clean(void);

	#ifdef MAC
		virtual bool FDoClick(EVT *pevt);
		virtual bool FDoKey(EVT *pevt);
	#endif //MAC
	#ifdef WIN
		virtual void EnqueueWcid(long wcid);
	#endif //WIN

	virtual bool FAddListCid(long cid, long lw0, PSTN pstn);
	virtual bool FRemoveListCid(long cid, long lw0, PSTN pstn = pvNil);
	virtual bool FChangeListCid(long cid, long lwOld, PSTN pstnOld,
		long lwNew, PSTN pstnNew);
	virtual bool FRemoveAllListCid(long cid);
	};

extern PMUB vpmubCur;

#endif //!MENU_H

