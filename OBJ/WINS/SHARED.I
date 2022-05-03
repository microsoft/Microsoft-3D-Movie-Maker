#line 1 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
























#line 1 "H:\\BUILD\\SOC\\inc\\kidgs.chh"

















#line 1 "H:\\BUILD\\SOC\\inc\\assert.chh"

































































#line 67 "H:\\BUILD\\SOC\\inc\\assert.chh"

#line 69 "H:\\BUILD\\SOC\\inc\\assert.chh"
#line 19 "H:\\BUILD\\SOC\\inc\\kidgs.chh"
#line 1 "H:\\BUILD\\SOC\\inc\\kidgsdef.h"














#line 1 "H:\\BUILD\\KAUAI\\src\\kiddef.h"














































































































#line 112 "H:\\BUILD\\KAUAI\\src\\kiddef.h"

#line 16 "H:\\BUILD\\SOC\\inc\\kidgsdef.h"
#line 1 "H:\\BUILD\\KAUAI\\src\\framedef.h"























#line 25 "H:\\BUILD\\KAUAI\\src\\framedef.h"




























































































































































































































































































































































































#line 406 "H:\\BUILD\\KAUAI\\src\\framedef.h"

#line 17 "H:\\BUILD\\SOC\\inc\\kidgsdef.h"
#line 1 "H:\\BUILD\\SOC\\inc\\kidsanim.h"




































































































































































































































































































































































































































































































































































































































#line 18 "H:\\BUILD\\SOC\\inc\\kidgsdef.h"








































































#line 91 "H:\\BUILD\\SOC\\inc\\kidgsdef.h"
#line 20 "H:\\BUILD\\SOC\\inc\\kidgs.chh"



	


#line 27 "H:\\BUILD\\SOC\\inc\\kidgs.chh"





	
#line 34 "H:\\BUILD\\SOC\\inc\\kidgs.chh"






	
#line 42 "H:\\BUILD\\SOC\\inc\\kidgs.chh"



	
#line 47 "H:\\BUILD\\SOC\\inc\\kidgs.chh"



	
#line 52 "H:\\BUILD\\SOC\\inc\\kidgs.chh"



















































































#line 136 "H:\\BUILD\\SOC\\inc\\kidgs.chh"




#line 141 "H:\\BUILD\\SOC\\inc\\kidgs.chh"
































































































































































































































































			




































































































































































































#line 595 "H:\\BUILD\\SOC\\inc\\kidgs.chh"
#line 26 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
SET _dfmbmp = 10 SET _dfscr = 10 SET _dfanm = 10 SET _dffill = 10 SET _curgok = 10 SET _dfmask = 10 SET _dftile = 10 SET _dfwave = 10 SET _dfmidi = 10 SET _dfvid = 10


#line 1 "H:\\BUILD\\SOC\\inc\\stdiodef.h"














#line 1 "H:\\BUILD\\SOC\\inc\\socdef.h"














































































































































































#line 176 "H:\\BUILD\\SOC\\inc\\socdef.h"

#line 16 "H:\\BUILD\\SOC\\inc\\stdiodef.h"
#line 1 "H:\\BUILD\\SOC\\inc\\utestres.h"




#line 1 "H:\\BUILD\\SOC\\inc\\socdef.h"
















































































































































































#line 6 "H:\\BUILD\\SOC\\inc\\utestres.h"































































#line 17 "H:\\BUILD\\SOC\\inc\\stdiodef.h"














































































































































			























































































































































































































































































































































































































































































































































































































































































































































































































































































































































#line 1080 "H:\\BUILD\\SOC\\inc\\stdiodef.h"
#line 30 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\buildgob.h"




































































#line 70 "H:\\BUILD\\SOC\\inc\\buildgob.h"
#line 31 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\sharedef.h"































































													












#line 78 "H:\\BUILD\\SOC\\inc\\sharedef.h"
#line 32 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\balloons.chh"






































































#line 72 "H:\\BUILD\\SOC\\inc\\balloons.chh"
#line 33 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\helpbook.h"












































 	













									
















										 































































#line 141 "H:\\BUILD\\SOC\\inc\\helpbook.h"

#line 34 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\projmain.h"





















































































































































































































































































































































































































#line 407 "H:\\BUILD\\SOC\\inc\\projmain.h"

#line 35 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"


#line 1 "H:\\BUILD\\SOC\\inc\\biopage.chh"
















					 

















































































































































































































































































































#line 38 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\sharutil.chh"




























































































































































































































































#line 254 "H:\\BUILD\\SOC\\inc\\sharutil.chh"

#line 39 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\src\\shared\\util.cht"












































#line 1 "H:\\BUILD\\SOC\\inc\\helptops.h"
















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































		

















































								















































































































































#line 2228 "H:\\BUILD\\SOC\\inc\\helptops.h"

#line 46 "H:\\BUILD\\SOC\\src\\shared\\util.cht"





CHUNK( 'GLOP', 0x00050001,  ) SCRIPT
	;
	CreateChildThis(0x00010000, 0x00010000);
	RunScriptGob(0x00010000, 0x0020);
ENDCHUNK












CHUNK( 'WAVE', 0x00010010, ) FILE "shared\sound\util\splot.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010004, ) FILE "shared\sound\util\ding.wav" ENDCHUNK





SET _curgok = 0x00010000 CHUNK( 'GOKD', 0x00010000, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 0
ENDCHUNK

	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0020 ) SCRIPT
		0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));

		
		
		

		0x00010000->cnoMidi = (0xFFFFFFFF);
 		0x00010000->fHelpOn = (1);
		0x00010000->fMovie = (0);
		0x00010000->cnoPal = (0xFFFFFFFF);

		SetProp(0x23302, 0x0001);

		
		
		::siiVoice = 0;

		
		::fTransition = 0;

		
		
		0x00010000->fLobby1PS = (1);  
		0x00010000->fBackstagePS = (1);
		0x00010000->fIdeasPS = (1);   
		0x00010000->fTicketPS = (1);   
		0x00010000->fStudioPS = (1);   

		
		
		
		0x00010000->fLob1stHelp = (1);
		0x00010000->fTic1stHelp = (1);
		0x00010000->fBkstg1stHelp = (1);

		
		::fBio1 = GetProp( 0x23500 )|1; ::fBio2 = GetProp( 0x23501 ); SetProp( 0x23500, ::fBio1 );;
				
		
		::fStdIntro = 0;		
		If( GetProp( 0x23503 ) & 0x01 );	
			
			::fStdIntro = 1;
		End;

		
		::fHelpOn = 0;
		::NextProject = 1;

		
		::volume = 50;
		::kgobCurrentPlace = 0;
		::fInStudio = 1;

		
		0x00010000->fTrans = (0);

		
		0x00010000->cDisable = 0;
								
		
		
		
		
		
		
		FilterCmdsThis( 40042, 0, 0x0026 );
		
	ENDCHUNK

	
	
	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		;
		;
		;
		If (_parm[0] != ::kgobCurrentPlace);
			0x00010000->kstEntry = (_parm[1]);
			If (::kgobCurrentPlace != 0);
				DestroyGob(::kgobCurrentPlace);
			End;
			::kgobCurrentPlace = _parm[0];
			CreateChildGob(GidParThis(), _parm[0], _parm[0]);
		End;
		FlushUserEvents(0xFFFFFFFF);
	ENDCHUNK

	
	
	
	
	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0022 ) SCRIPT
		
		FilterCmdsThis(50082, 0, 0xFFFFFFFF);

		If (_parm[2]);	
			RunScriptGob(0x00010000, 0x0028, 0x00011800, 0x0001, 0xFFFFFFFF);;
		End;
	ENDCHUNK

	
	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0023 ) SCRIPT
		If( FGobExists( 0x0001010c ) == 0);
			
			CreateChildGob(::kgobCurrentPlace, 0x0001010c, 0x0001010c);
		End;
		0x00010000->keys = (_parm[0]);
	ENDCHUNK

	
	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0024 ) SCRIPT
		If( _parm[0] != .cnoMidi );
			If( .cnoMidi != 0xFFFFFFFF  );
				
				
				StopSound( .siiMidi );
			End;
			If( _parm[0] != 0xFFFFFFFF );
			    cnt = ( _parm[1] ) ? 0x00010000 : 1;
				.siiMidi = PlaySoundGob( 0x00010000, 'MIDS', _parm[0], 1, 0x00010000, cnt, 4, 998 );
			End;
			.cnoMidi = _parm[0];
		End;
	ENDCHUNK

	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0025 ) SCRIPT
		

		CreateChildGob( GidParGob( 0x00010000 ), 0x0001010b, 0x0001010b  );
		CreateHelpGob( 0x0001010b, 0x00016043 );
		.keysTmp = ((0x00010000->keys));          
		0x00010000->keys = (0x00000000);         
	ENDCHUNK

	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0026 ) SCRIPT
		
		



		
		If (_parm[1] == 101 && (((0x00010000->keys) & 0x00000008) == 0x00000008));
			
    		EnqueueCid(50080, 0, 1, 0, 0, 0);
			
			FilterCmdsThis(50082, 0, 0x0022);
		Elif (_parm[1] == 101 && !(((0x00010000->keys) & 0x00000008) == 0x00000008));
			ModalHelp(0x20005853, -1);

		
		Elif (_parm[1] == 100 && (((0x00010000->keys) & 0x00000010) == 0x00000010));
			
			EnqueueCid(50091, 0, 0, 0, 0, 0);
			RunScriptGob(0x00010000, 0x0028, 0x00011800, 0x0001, 0xFFFFFFFF);;
		Elif (_parm[1] == 100 && !(((0x00010000->keys) & 0x00000010) == 0x00000010));
			ModalHelp(0x20005853, -1);

		
		Elif (_parm[1] == 40042 && (((0x00010000->keys) & 0x00000001) == 0x00000001));
			If (::fInStudio);
				;
				CreateChildGob(0x20002, 0x00012000, 0x00012000);
			Else;
				;
				CreateChildGob(::kgobCurrentPlace, 0x00012000, 0x00012000);
			End;
			Exit();
		Elif (_parm[1] == 40042 && !(((0x00010000->keys) & 0x00000001) == 0x00000001));
			ModalHelp(0x20005854, -1);

		
		Elif (_parm[1] == 50110 && (((0x00010000->keys) & 0x00000020) == 0x00000020));
			RunScriptGob(0x00010000, 0x0028, 0x00011800, 0x0001, 0xFFFFFFFF);;
		Elif (_parm[1] == 50110 && !(((0x00010000->keys) & 0x00000020) == 0x00000020));
			EnqueueCid(50091, 0, 0, 0, 0, 0);
			ModalHelp(0x20005853, -1);

		End;

	ENDCHUNK

	
	
	
	
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0027 ) SCRIPT
		::fTransition = 0;
		If ( (_parm[0] != (0x00010000->cnoPal)) && (0x00010000->fTrans) && (_parm[0] != 0xFFFFFFFF) );
			
			Transition(3, 0, 60, 0xffffffff, _parm[0]);
			::fTransition = 1;
		Else;
			
			
			If( _parm[0] != 0xFFFFFFFF );
				SetColorTable(_parm[0]);
			End;
		End;
		0x00010000->cnoPal = (_parm[0]);	
		0x00010000->fTrans = (1);       
	ENDCHUNK

	
	
	
	
	
	
	
	
	
	
	
	







	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0028 ) SCRIPT
		;
		;
		StartLongOp();

		
		FilterCmdsThis(101, 0, 0xFFFFFFFF);
		FilterCmdsThis(100, 0, 0xFFFFFFFF);
		FilterCmdsThis(50110, 0, 0xFFFFFFFF);
		
		
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));

		.kgobCurrentPlaceT = ::kgobCurrentPlace;
		SetProp(0x23300, _parm[0]);
		0x00010000->cnoPal = (0xFFFFFFFF);
		0x00010000->kstEntry = (_parm[1]);
		SetProp(0x23302, _parm[1]);
		::kgobCurrentPlace = 0;

		
		FilterCmdsThis(50107, 0, 0x0029);

		
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );

		EnqueueCid(50060, 0, .kgobCurrentPlaceT, _parm[2], 0, 0);
		::fInStudio = 1;
	ENDCHUNK

	
	
	
	
	
	
	

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0029 ) SCRIPT
		;
		::fInStudio = 0;
		::kgobCurrentPlace = .kgobCurrentPlaceT;
		EnqueueCid(50091, 0, 0,0,0,0);
		FilterCmdsThis(50107, 0, 0xFFFFFFFF);
		FilterCmdsGob(0x00010000, 101, 0, 0x0026);
		FilterCmdsGob(0x00010000, 100, 0, 0x0026);
		EndLongOp(1);	
	ENDCHUNK


CHUNK( 'GLOP', 0x00016043, ) SCRIPT
	
	
	If( _parm[0] == 2 );                            
		DestroyGob( 0x0001010b );
		0x00010000->keys = (0x00010000->keysTmp);             
		DestroyGob( GidParThis() );
	Elif( _parm[0] == 1 );
		EnqueueCid( 106, 0,0,0,0,0 );
	End;
ENDCHUNK




SET _curgok = 0x0001010b CHUNK( 'GOKD', 0x0001010b,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 1000 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD' _curgok SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG BYTE 15 0 0 0xfe LONG 0xffffffff
		BYTE 0x55 0xaa 0x55 0xaa 0x55 0xaa 0x55 0xaa
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
	ENDCHUNK




SET _curgok = 0x0001010c CHUNK( 'GOKD', 0x0001010c,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 975 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD' _curgok SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG BYTE 15 0 0 0xfe LONG 0xffffffff
		BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		PlaySoundThis( 'WAVE', 0x00010010, 1, 0x00010000, 1, 0, 789 );
	ENDCHUNK





CHUNK( 'GLOP', 0x0001604f, ) SCRIPT
	If( _parm[0] != 0 );
		DestroyGob( GidParThis() );
	End;
ENDCHUNK
#line 40 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\src\\shared\\cursors.cht"




CHUNK( 'GGCR', 0x00000001, ) PACK CURSOR "shared\cursors\hotspot.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000002, ) PACK CURSOR "shared\cursors\default.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000003, ) PACK CURSOR "shared\cursors\actions2.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000004, ) PACK CURSOR "shared\cursors\crs1018.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000005, ) PACK CURSOR "shared\cursors\turnleft.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000006, ) PACK CURSOR "shared\cursors\turnrigt.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000007, ) PACK CURSOR "shared\cursors\turnarnd.cur" ENDCHUNK
CHUNK( 'GGCR', 0x00000009, ) PACK CURSOR "shared\cursors\turnarnd.cur" ENDCHUNK

#line 41 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\src\\shared\\map.cht"



























#line 1 "H:\\BUILD\\SOC\\inc\\socdefn.h"










#line 1 "H:\\BUILD\\SOC\\inc\\buildgob.h"





































































#line 12 "H:\\BUILD\\SOC\\inc\\socdefn.h"


#line 1 "H:\\BUILD\\SOC\\inc\\sharedef.h"













































































#line 15 "H:\\BUILD\\SOC\\inc\\socdefn.h"




#line 1 "H:\\BUILD\\SOC\\inc\\balloons.chh"







































































#line 20 "H:\\BUILD\\SOC\\inc\\socdefn.h"



















































































	






























































































































































































								  




								   


















































































































































































#line 479 "H:\\BUILD\\SOC\\inc\\socdefn.h"


#line 29 "H:\\BUILD\\SOC\\src\\shared\\map.cht"
#line 1 "H:\\BUILD\\SOC\\src\\shared\\map.chh"













































































  















#line 95 "H:\\BUILD\\SOC\\src\\shared\\map.chh"
#line 30 "H:\\BUILD\\SOC\\src\\shared\\map.cht"

CHUNK( 'WAVE', 0x00010008, ) FILE "shared\sound\mapobj.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010009, ) FILE "shared\sound\mapopen.wav" ENDCHUNK	

















SET _curgok = 0x00012000 CHUNK( 'GOKD', 0x00012000,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 900 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "shared\pbm\map\mapback.pbm" ENDCHUNK
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	PlaySoundGob(0x00010000, 'WAVE', 0x00010009, 1, 0x00010000, 1, 1,998);


	
	; ; ; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;;

	
	FilterCmdsThis(100003, 0, 0x0029);

	
	CreateChildThis( 0x00012050, 0x00012050 );
	CreateChildThis( 0x00012051, 0x00012051 );
	CreateChildThis( 0x00012058, 0x00012058 );
	CreateChildThis( 0x00012052, 0x00012052 );
	CreateChildThis( 0x00012054, 0x00012054 );
	CreateChildThis( 0x00012055, 0x00012055 );
	CreateChildThis( 0x00012056, 0x00012056);
	CreateChildThis( 0x00012057, 0x00012057 );
	CreateChildThis( 0x0001205d, 0x0001205d);
	CreateChildThis( 0x0001205b, 0x0001205b );
	CreateChildThis( 0x0001205c, 0x0001205c );
	CreateChildThis( 0x00012059, 0x00012059 );
	CreateChildThis( 0x0001205a, 0x0001205a );
	CreateChildThis( 0x00012062, 0x00012062 );
	CreateChildThis( 0x00012060, 0x00012060 );
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) SCRIPT
	Cell( 0x00000040, 0, 0, 1 );

	
	CreateHelpThis( 0x00018547 );
ENDCHUNK


SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0029 ) SCRIPT
	Return( 1 );
ENDCHUNK









SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT

	newplace = _parm[0];
	newstate = _parm[1];
	fTools	= _parm[2];
	  
	
	PlaySoundGob(0x00010000, 'WAVE', 0x00010008, 1, 0x00010000, 1, 1,
			998);

	

	If ( ::fInStudio );
		If( fTools );
			
			;
			; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; 
			DestroyThis();
			Exit();
		End;

		

		;
		SetProp(0x23300, newplace );
		SetProp(0x23302, newstate);
		; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
		RunScriptGob(0x00010000, 0x0027, 0xFFFFFFFF);		

		EnqueueCid(50061, 0, 0, 0, 0, 0);
		DestroyThis();

	Else;
		If (GidParThis() == newplace);
			
			

			;
			; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
			DestroyThis();		

			If (newstate != Match( newplace, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001));
				;
				0x00010000->kstEntry = (newstate);
				
				
				
				RunScriptGob(newplace, 0x0020);
			End;

			Exit();
		End;			

		
		
		
		;
		; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
		DestroyThis();		
		RunScriptGob(0x00010000, 0x0021, newplace, newstate);
	End;
ENDCHUNK



SET _curgok = 0x00012055 CHUNK( 'GOKD', 0x00012055,  ) PACK SHORT BO OSK LONG 0 LONG 0 581 365 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\map\cancel.bmp" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0001 _st )
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\map\cancelc.bmp" ENDCHUNK
SET _dfwave++ CHUNK( 'WAVE', _dfwave,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x001D _st ) FILE "shared\sound\map\cancel.wav" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
	DestroyGob( 0x00012000 );

ENDCHUNK


SET _curgok = 0x00012062 CHUNK( 'GOKD', 0x00012062,  ) PACK SHORT BO OSK LONG 0 LONG 0 571 426 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\map\exit.bmp" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0001 _st )
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\map\exits.bmp" ENDCHUNK
SET _dfwave++ CHUNK( 'WAVE', _dfwave,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x001D _st ) FILE "shared\sound\map\mapexit.wav" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
	DestroyGob( 0x00012000 );
	EnqueueCid(106, 0, 0, 0, 0, 0);;
ENDCHUNK




SET _curgok = 0x00012003 CHUNK( 'GOKD', 0x00012003,  ) PACK SHORT BO OSK LONG 6 LONG 0 0 0 555 LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 6, 22 ) "shared\bmp\map\here.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 30 8 32 70 4 0 0 0 6
	SHORT 13 1 22 4 2 0 0 0 4
ENDCHUNK
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT

	
	
	If (::fInStudio);
		plc = GetProp(0x23300);
	Else;
		plc = ::kgobCurrentPlace;
	End;

	
	x= Match( plc, -1,		
		0x00010500, 		346,
		0x00010580, 		346,
		0x00010600, 		320, 
		0x00010700, 		320, 
		0x00010800,		320, 
		0x00010900, 		320, 
		0x00010c00, 		320, 
		0x00010d00, 		320,
		0x00010e00, 		320,
		0x00010f00, 		320,
		0x00011000, 	320,
		0x00011100, 	320,
		0x00011400,	238,
		0x00011500,	238,
		0x00011600,	238,
		0x00011700,	238,
		0x00011800, 		322,
		0x00011900, 		322,
		0x00011a00, 		322,
		0x00011b00, 		322,
		0x00011c00, 		372,
		0x00011d00, 		372,
		0x00011e00, 		372,
		0x00011f00, 		372 
	);

	y= Match( plc, -1,		
		0x00010500, 		428,
		0x00010580, 		428,
		0x00010600, 		363, 
		0x00010700, 		363, 
		0x00010800,		363, 
		0x00010900, 		363, 
		0x00010c00, 		256, 
		0x00010d00, 		256,
		0x00010e00, 		256,
		0x00010f00, 		256,
		0x00011000, 	147,
		0x00011100, 	147,
		0x00011400,	141,
		0x00011500,	141,
		0x00011600,	141,
		0x00011700,	141,
		0x00011800, 		61,
		0x00011900, 		61,
		0x00011a00, 		61,
		0x00011b00, 		61,
		0x00011c00, 		134,
		0x00011d00, 		134,
		0x00011e00, 		134,
		0x00011f00, 		134 
	);

	;
	MoveAbsThis( x, y );

ENDCHUNK




SET _curgok = 0x00012056 CHUNK( 'GOKD', 0x00012056,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\ideamsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	
	RunScriptGob(0x00012000, 0x0021, 0x00011400,
			Match( 0x00011400, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK


SET _curgok = 0x00012057 CHUNK( 'GOKD', 0x00012057,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\studmsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00011800,
			Match( 0x00011800, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK


SET _curgok = 0x00012050 CHUNK( 'GOKD', 0x00012050,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\projmsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00011c00,
			Match( 0x00011c00, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK


SET _curgok = 0x00012051 CHUNK( 'GOKD', 0x00012051,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\bkstgmsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00011000,
			Match( 0x00011000, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK


SET _curgok = 0x00012058 CHUNK( 'GOKD', 0x00012058,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\theatmsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00010c00,
			Match( 0x00010c00, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK



SET _curgok = 0x00012052 CHUNK( 'GOKD', 0x00012052,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\LOBBYMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00010600, Match( 0x00010600, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001),
			0);
ENDCHUNK


SET _curgok = 0x00012054 CHUNK( 'GOKD', 0x00012054,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\TICKTMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00010580,
			Match( 0x00010580, 0x0001, 0x00011c00, 0x0001, 0x00011000, 0x0002, 0x00010c00, 0x0004, 0x00010600, 0x0002, 0x00010580, 0x0003, 0x00011400, 0x0006, 0x00011800, 0x0001), 0);
ENDCHUNK



SET _curgok = 0x0001205a CHUNK( 'GOKD', 0x0001205a,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\biomsk.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob( 0x00012000, 0x0021, 0x00011400, 0x0008 , 0 );
ENDCHUNK


SET _curgok = 0x00012059 CHUNK( 'GOKD', 0x00012059,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\SPLOTMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob( 0x00012000, 0x0021, 0x00011400, 0x0007 , 0 );
ENDCHUNK


SET _curgok = 0x0001205d CHUNK( 'GOKD', 0x0001205d,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\TBOXMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob(0x00012000, 0x0021, 0x00011800, 0x0002, 1);
ENDCHUNK


SET _curgok = 0x0001205b CHUNK( 'GOKD', 0x0001205b,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\LOGOMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob( 0x00012000, 0x0021, 0x00011c00, 0x0004 , 0 );
ENDCHUNK


SET _curgok = 0x0001205c CHUNK( 'GOKD', 0x0001205c,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "shared\bmp\map\GIZMOMSK.BMP" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 	RunScriptGob( 0x00012000, 0x0021, 0x00011c00, 0x0005 , 0 );
ENDCHUNK





SET _curgok = 0x0001205f CHUNK( 'GOKD', 0x0001205f,  ) PACK SHORT BO OSK LONG 3 LONG 0 0 0 60 LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\map\volthmb.bmp" ENDCHUNK






SET _curgok = 0x00012060 CHUNK( 'GOKD', 0x00012060,  ) PACK SHORT BO OSK LONG 0 LONG 0 22 294 500 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 48 104 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	CreateChildThis( 0x0001205f, 0x0001205f  );
	
	y = 96  -(( GetMasterVolume() * 96 )/ (0x00010000*2));
	MoveAbsGob( 0x0001205f, 0, y );
ENDCHUNK
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) SCRIPT	
	
	.siiLoop = PlaySoundThis('WAVE', 0x00010004, 2, 0x00010000, 30000, 1, 998);	
	While( 1 );
		y = YMouseThis();
		If( y < 0 );
			y=0;
		Elif( y > 96 );
			y =96;
		End;
		
		
		MoveAbsGob( 0x0001205f, 0, y );
		
		
		newvol = ( (0x00010000*2)* (96-y) ) / 96;	

		
		SetMasterVolume( newvol );	

		
		Cell(  0 , 0,0, 10 );	
	End;
ENDCHUNK
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x001D _st ) SCRIPT
	StopSound( .siiLoop );
ENDCHUNK
ADOPT( 'GOKD', _curgok, 'GLSC', _dfanm, SET _st = 0x10000 SET _st |= 0x0018 _st )
#line 42 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"




#line 1 "H:\\BUILD\\SOC\\src\\shared\\sharhelp.cht"













SET _curgok = 0x00016050 CHUNK( 'GOKD', 0x00016050, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 115, 72 ) "shared\bmp\alert.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 14 14 24 183 8 0 0 0 24
	SHORT 14 14 24 95 8 0 0 0 24
ENDCHUNK
SET _dfwave++ CHUNK( 'WAVE', _dfwave,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) FILE "shared\sound\alert2.wav" ENDCHUNK


CHUNK( 'GLOP', 0x00016051,  ) SCRIPT
	If(_parm[0] != 0);	
		EnqueueCid(100009, 0, _parm[0], 0, 0, 0);
	End;
ENDCHUNK




CHUNK( 'GLOP', 0x2000016A, ) SCRIPT
	
	If(_parm[0] != 0);	
		EnqueueCid(100009, 0, _parm[0], 0, 0, 0);
	End;
ENDCHUNK







SET _curgok = 0x00016001 CHUNK( 'GOKD', 0x00016001, ) PACK SHORT BO OSK LONG 0
	LONG 0 40 40 500
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 93, 29 ) "shared\bmp\balln1b.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 24 7 64 8 0 0 0 22
	SHORT 7 8 7 22 8 17 8 8 8
ENDCHUNK




SET _curgok = 0x0001600d CHUNK( 'GOKD', 0x0001600d, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 0, 0 ) "shared\bmp\tooltip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 4 4 160 8 0 0 0 4
	SHORT 3 3 3 50 8 0 0 0 3
ENDCHUNK




SET _curgok = 0x0001602e CHUNK( 'GOKD', 0x0001602e, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 168, 56 ) "shared\bmp\tooltip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 4 4 160 8 0 0 0 4
	SHORT 3 3 3 50 8 0 0 0 3
ENDCHUNK




SET _curgok = 0x0001602b CHUNK( 'GOKD', 0x0001602b, ) PACK SHORT BO OSK LONG 6
	LONG 0 0 0 500
ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD' _curgok SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff
	BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
ENDCHUNK




SET _curgok = 0x0001600e CHUNK( 'GOKD', 0x0001600e, ) PACK SHORT BO OSK LONG 0
	LONG 0 40 40 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 231, 143 ) "shared\bmp\toolhelp.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 14 14 24 183 8 0 0 0 24
	SHORT 14 14 24 95 8 0 0 0 24
ENDCHUNK





SET _curgok = 0x0001602a CHUNK( 'GOKD', 0x0001602a, ) PACK SHORT BO OSK LONG 0
	LONG 0 100 100 700
	LONG 0 0 0xffffffff 0x00000002 0x0001 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 120, 72 ) "shared\bmp\alert.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 14 14 24 191 8 0 0 0 24
	SHORT 14 14 24 95 8 0 0 0 24
ENDCHUNK




SET _curgok = 0x0001600c CHUNK( 'GOKD', 0x0001600c, ) PACK SHORT BO OSK LONG 6
	LONG 0 0 0 899
ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD' _curgok SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff
	BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
ENDCHUNK




SET _curgok = 0x0001600f CHUNK( 'GOKD', 0x0001600f, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 899
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 1, 1 ) "shared\bmp\balln3.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 0 0 0 32 1 0 0 0 0
	SHORT 0 0 0 32 1 0 0 0 0
ENDCHUNK




SET _curgok = 0x00016010 CHUNK( 'GOKD', 0x00016010, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 899
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 1, 1 ) "shared\bmp\balln3.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 0 0 0 32 1 0 0 0 0
	SHORT 0 0 0 32 1 0 0 0 0
ENDCHUNK




SET _curgok = 0x00016003 CHUNK( 'GOKD', 0x00016003, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 899
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 1, 1 ) "shared\bmp\balln3.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 0 0 0 32 1 0 0 0 0
	SHORT 0 0 0 32 1 0 0 0 0
ENDCHUNK




SET _curgok = 0x00016002 CHUNK( 'GOKD', 0x00016002, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 999
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 40, 29 ) "shared\bmp\balln2.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 24 7 64 8 0 0 0 22
	SHORT 3 8 7 20 8 17 8 8 8
ENDCHUNK




SET _curgok = 0x0001600a CHUNK( 'GOKD', 0x0001600a, ) PACK SHORT BO OSK LONG 6
	LONG 0 0 0 999
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 40, 29 ) "shared\bmp\vo.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 24 7 64 8 0 0 0 22
	SHORT 3 8 7 20 8 17 8 8 8
ENDCHUNK




SET _curgok = 0x00016004 CHUNK( 'GOKD', 0x00016004, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 999
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 30, 5 ) "shared\bmp\border1.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 11 20 31 40 1 0 0 0 8
	SHORT 25 8 25 32 1 0 0 0 8
ENDCHUNK




SET _curgok = 0x00016011 CHUNK( 'GOKD', 0x00016011, ) PACK SHORT BO OSK LONG 0
	LONG 0 100 100 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 168, 56 ) "shared\bmp\tooltip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 4 4 160 8 0 0 0 4
	SHORT 3 3 3 50 8 0 0 0 3
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
	
	FilterCmdsThis(40040, 0, 0x0021);
	FilterCmdsThis(40041, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	If (!FIsDescendent(_parm[0], GidThis()));
		DestroyThis();
	End;
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x0001602c CHUNK( 'GOKD', 0x0001602c, ) PACK SHORT BO OSK LONG 0
	LONG 0 100 100 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 0x0021 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 84, 28 ) "shared\bmp\tooltip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 4 4 160 8 0 0 0 4
	SHORT 3 3 3 50 8 0 0 0 3
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x0001602d CHUNK( 'GOKD', 0x0001602d, ) PACK SHORT BO OSK LONG 0
	LONG 0 100 100 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 0x0021 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 84, 28 ) "shared\bmp\tooltip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 4 4 160 8 0 0 0 4
	SHORT 3 3 3 50 8 0 0 0 3
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
	
	FilterCmdsThis(40040, 0, 0x0021);
	FilterCmdsThis(40041, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x00016015 CHUNK( 'GOKD', 0x00016015, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 61 ) "shared\bmp\coblln0.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 5 5 8 151 8 0 0 0 8
	SHORT 4 4 8 106 8 0 0 0 8
ENDCHUNK




SET _curgok = 0x00016016 CHUNK( 'GOKD', 0x00016016, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 1, 1 ) "shared\bmp\coblln1.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 29 8 36 146 8 0 0 0 8
	SHORT 28 8 32 103 8 0 0 0 8
ENDCHUNK




SET _curgok = 0x00016017 CHUNK( 'GOKD', 0x00016017, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 1 ) "shared\bmp\coblln2.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 8 8 58 8 35 60 8 8
	SHORT 41 8 42 108 8 0 0 0 8
ENDCHUNK




SET _curgok = 0x00016018 CHUNK( 'GOKD', 0x00016018, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 189, 2 ) "shared\bmp\coblln3.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 32 8 147 8 0 0 0 36
	SHORT 30 8 34 103 4 0 0 0 8
ENDCHUNK




SET _curgok = 0x00016019 CHUNK( 'GOKD', 0x00016019, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 201, 65 ) "shared\bmp\coblln4.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 46 8 148 8 0 0 0 48
	SHORT 3 8 8 40 4 34 40 4 8
ENDCHUNK




SET _curgok = 0x0001601a CHUNK( 'GOKD', 0x0001601a, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 189, 143 ) "shared\bmp\coblln5.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 32 8 148 8 0 0 0 36
	SHORT 3 32 8 102 8 0 0 0 36
ENDCHUNK




SET _curgok = 0x0001601b CHUNK( 'GOKD', 0x0001601b, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 155 ) "shared\bmp\coblln6.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 8 8 60 8 33 60 8 8
	SHORT 3 38 8 105 8 0 0 0 44
ENDCHUNK




SET _curgok = 0x0001601c CHUNK( 'GOKD', 0x0001601c, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 0, 147 ) "shared\bmp\coblln7.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 30 8 34 149 8 0 0 0 8
	SHORT 3 28 8 101 8 0 0 0 38
ENDCHUNK




SET _curgok = 0x0001601d CHUNK( 'GOKD', 0x0001601d, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 0, 66 ) "shared\bmp\coblln8.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 32 8 42 150 8 0 0 0 8
	SHORT 3 8 8 40 4 34 32 4 8
ENDCHUNK




SET _curgok = 0x0001601e CHUNK( 'GOKD', 0x0001601e, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 61 ) "shared\bmp\coblln0.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 5 5 8 151 8 0 0 0 8
	SHORT 4 4 8 106 8 0 0 0 8
ENDCHUNK




SET _curgok = 0x0001601f CHUNK( 'GOKD', 0x0001601f, ) PACK SHORT BO OSK LONG 6
	LONG 0 0 0 500
ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD' _curgok SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff
	BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
ENDCHUNK




SET _curgok = 0x00016031 CHUNK( 'GOKD', 0x00016031, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 1 ) "shared\bmp\coblln2.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 4 8 8 58 8 35 60 8 8
	SHORT 41 8 42 108 8 0 0 0 8
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	If (!FIsDescendent(_parm[0], GidThis()));
		DestroyThis();
	End;
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x0001602f CHUNK( 'GOKD', 0x0001602f, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 201, 65 ) "shared\bmp\coblln4.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 8 46 8 148 8 0 0 0 48
	SHORT 8 8 8 40 4 34 40 4 8
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	If (!FIsDescendent(_parm[0], GidThis()));
		DestroyThis();
	End;
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x00016032 CHUNK( 'GOKD', 0x00016032, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 61 ) "shared\bmp\coblln0.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 5 5 8 151 8 0 0 0 8
	SHORT 4 4 8 106 8 0 0 0 8
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	If (!FIsDescendent(_parm[0], GidThis()));
		DestroyThis();
	End;
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK




SET _curgok = 0x00016030 CHUNK( 'GOKD', 0x00016030, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF 
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 83, 155 ) "shared\bmp\coblln6.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 8 8 8 60 8 33 60 8 8
	SHORT 8 38 8 105 8 0 0 0 44
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	If (!FIsDescendent(_parm[0], GidThis()));
		DestroyThis();
	End;
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK







CHUNK( 'GLOP', 0x000305F0,  ) SCRIPT
	If(_parm[0] == 0);	
		fMZAudio = GetProp( 0x23503 ) & 0x01;
		If (._cnoSound != 0xFFFFFFFF && !fMZAudio);	
			StopSoundClass(0, 999);		
			PlaySoundGob(GidParThis(), ._ctgSound, ._cnoSound, 0, 0x10000, 1, 0, 999);
		End;
	End;
ENDCHUNK




CHUNK( 'GLOP', 0x000305F1,  ) SCRIPT
	If(_parm[0] == 0);	
		fMelAudio = GetProp( 0x23503 ) & 0x02;
		If (._cnoSound != 0xFFFFFFFF && !fMelAudio);
			StopSoundClass(0, 999);		
			PlaySoundGob(GidParThis(), ._ctgSound, ._cnoSound, 0, 0x10000, 1, 0, 999);
		End;
	End;
ENDCHUNK




CHUNK( 'GLOP', 0x000305F2,  ) SCRIPT
	If(_parm[0] == 0);	
		fMelAudio = GetProp( 0x23503 ) & 0x02;		
		If (._cnoSound != 0xFFFFFFFF && !fMelAudio);
			StopSoundClass(0, 999);		
			PlaySoundGob(GidParThis(), ._ctgSound, ._cnoSound, 0, 0x10000, 1, 0, 999);
		End;
	End;
ENDCHUNK




CHUNK( 'GLOP', 0x000305F3,  ) SCRIPT
	If(_parm[0] == 0);	
		fMZAudio = GetProp( 0x23503 ) & 0x01;
		If (._cnoSound != 0xFFFFFFFF && !fMZAudio);
			StopSoundClass(0, 999);		
			PlaySoundGob(GidParThis(), ._ctgSound, ._cnoSound, 0, 0x10000, 1, 0, 999);
		End;
	End;
ENDCHUNK




CHUNK( 'GLOP', 0x000305F4,  ) SCRIPT
	If(_parm[0] == 0);	
		fMZAudio = GetProp( 0x23503 ) & 0x01;
		If (._cnoSound != 0xFFFFFFFF && !fMZAudio);
			StopSoundClass(0, 999);		
			PlaySoundGob(GidParThis(), ._ctgSound, ._cnoSound, 0, 0x10000, 1, 0, 999);
		End;
	End;
ENDCHUNK







SET _curgok = 0x00016009 CHUNK( 'GOKD', 0x00016009, ) PACK SHORT BO OSK LONG 0
	LONG 0 40 40 800
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 93, 29 ) "shared\bmp\balln1a.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 7 7 133 4 0 0 0 7
	SHORT 7 7 7 89 4 0 0 0 7
ENDCHUNK




SET _curgok = 0x00016008 CHUNK( 'GOKD', 0x00016008, ) PACK SHORT BO OSK LONG 0
	LONG 0 40 40 800
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 170, 103 ) "shared\bmp\balln1e.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 32 7 131 4 0 0 0 32
	SHORT 7 7 7 70 4 0 0 0 26
ENDCHUNK





SET _curgok = 0x00016007 CHUNK( 'GOKD', 0x00016007, ) PACK SHORT BO OSK LONG 0
	LONG 0 40 40 800
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 0, 102 ) "shared\bmp\balln1d.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 32 7 32 131 4 0 0 0 7
	SHORT 7 7 7 68 4 0 0 0 26
ENDCHUNK




SET _curgok = 0x00016006 CHUNK( 'GOKD', 0x00016006, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 999
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 40, 29 ) "shared\bmp\balln2.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 7 24 7 64 8 0 0 0 22
	SHORT 7 8 7 20 8 17 8 8 8
ENDCHUNK




SET _curgok = 0x00016100 CHUNK( 'GOKD', 0x00016100, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 1
	LONG 0 0 0xffffffff 0x00000001 0x0001 0 0xFFFFFFFF
ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\squishy1.bmp" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\squishy3.bmp" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) BITMAP( 0x00, 0, 0 ) "shared\bmp\squishy2.bmp" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfwave++ CHUNK( 'WAVE', _dfwave,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x001D _st ) FILE "shared\sound\ok.wav" ENDCHUNK
#line 47 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"

#line 1 "H:\\BUILD\\SOC\\src\\shared\\biopage.cht"












#line 1 "H:\\BUILD\\SOC\\inc\\projmain.h"























































































































































































































































































































































































































#line 14 "H:\\BUILD\\SOC\\src\\shared\\biopage.cht"

CHUNK( 'WAVE', 0x00010044, ) FILE "shared\sound\biofound.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010045, ) FILE "shared\sound\bioalrdy.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010046, ) FILE "shared\sound\tbkopen.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018114, ) FILE "shared\sound\biocancl.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018115, ) FILE "shared\sound\biopop.wav" ENDCHUNK

SET _curgok = 0x00015100 CHUNK( 'GOKD', 0x00015100,  ) PACK SHORT BO OSK LONG 1 LONG 0 0 0 1000 LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "shared\pbm\biopage.pbm" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, 0x00000040 )
SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	; ; ; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;;
	.count = 0;

	
	FilterCmdsThis(100003, 0, 0x0029);

	.snd = (::fBioCtrl ) ? 0x00010046 : 0x00010044;
	
	RunScriptThis( 0x0021 );
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	;	
	;

	If( ::nBioPage < 0 || ::nBioPage > 41 );
		;
		::nBioPage = 0;
	End;

	
	
	id = Select( ::nBioPage, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20, 21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46 );
	
	If ( !FGobExists( 0x00015102 ));
		CreateChildThis( 0x00015102, 0x00015102 );
	End;
	SetRepGob( 0x00015102, 0x00000040 + id );	
	NumToStr( id, 100 );

	
	If ( !FGobExists( 0x00015109 ));
		CreateChildThis( 0x00015109, 0x00015109 );
	End;

	If ( !FGobExists( 0x0001510b ));
		CreateChildThis( 0x0001510b, 0x0001510b );
	End;

	
	CreateHelpGob( 0x00015109, 0x00015201 + id );
	
	
	If ( id != 0 );
		CreateHelpGob( 0x0001510b, 0x0001522d );
	Else;
		DestroyGob( 0x0001510b );
	End;

	
	If( ::fBioCtrl );
		
		If( !FGobExists( 0x00015108));
			CreateChildThis( 0x00015108, 0x00015108 );
		End;
		
		If( !FGobExists( 0x00015104));
			CreateChildThis( 0x00015104, 0x00015104 );
		End;
		
		If( !FGobExists( 0x00015105 ));
			CreateChildThis( 0x00015105, 0x00015105 );
		End;
	End;

	
	If( !FGobExists( 0x00015103 ));
		CreateChildThis( 0x00015103,0x00015103 );
	End;
	
	
	If( .snd != 0xFFFFFFFF );
		
		PlaySoundThis('WAVE', .snd, 1, 0x00010000, 1, 1, 998);
		.snd = 0xFFFFFFFF;
	End;

ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0029 ) SCRIPT
	
	If ( _parm[2] == Select( .count, 's', 'o', 'c', 'r', 'a', 't', 'e', 's' ));
		++.count;
		If( .count == 8 );
			CreateChildThis( 7777, 7777 );	
		End;
	Else;
		.count = 0;
	End;
	Return( 1 );
ENDCHUNK

SET _curgok = 7777 CHUNK( 'GOKD', 7777, ) PACK SHORT BO OSK LONG 2
	LONG 0 160 120 999
	LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dfvid++ CHUNK( 'VIDE', _dfvid,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "bldgdata.chk" ENDCHUNK




SET _curgok = 0x00015108 CHUNK( 'GOKD', 0x00015108,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "shared\pbm\biopanel.pbm" ENDCHUNK



SET _curgok = 0x00015103 CHUNK( 'GOKD', 0x00015103,  ) PACK SHORT BO OSK LONG 0 LONG 0 511 415 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "shared\pbm\cancel.pbm" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0001 _st )
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) PACKEDFILE "shared\pbm\cancelc.pbm" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	
	
	PlaySoundThis('WAVE', 0x00018114, 1, 0x00010000, 1, 4, 998);
	If (FGobExists(0x20017));
		EnqueueCid(100008, 0x20017, 0, 0, 0, 0);
	End;
	DestroyGob( 0x00015100 );
	; ; ; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
ENDCHUNK



SET _curgok = 0x00015104 CHUNK( 'GOKD', 0x00015104,  ) PACK SHORT BO OSK LONG 0 LONG 0 339 410 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "shared\pbm\fwd.pbm" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0001 _st )
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) PACKEDFILE "shared\pbm\fwdc.pbm" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	curr = ::nBioPage;
	While( 1 );
		++::nBioPage;			
		If ( ::nBioPage == 41 );	 
			::nBioPage = curr;		
			Break;
		End;
		If( ((::nBioPage > 31) ? ( ::fBio2 & (1<<(::nBioPage&31)) ) : ( ::fBio1 & (1<<(::nBioPage&31)) )));	
			wav = Rnd(2) + 0x00010042;	
			PlaySoundThis('WAVE', wav, 1, 0x00010000, 1, 4, 998);
			Break;				
		End;
	End;
	
	RunScriptGob( 0x00015100, 0x0021 );	
ENDCHUNK


SET _curgok = 0x00015105 CHUNK( 'GOKD', 0x00015105,  ) PACK SHORT BO OSK LONG 0 LONG 0 229 410 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "shared\pbm\back.pbm" ENDCHUNK
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0001 _st )
ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x10000 SET _st |= 0x0002 _st )
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) PACKEDFILE "shared\pbm\backc.pbm" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	curr = ::nBioPage;
	While( 1 );
		--::nBioPage;
		If ( ::nBioPage < 0 );	 
			::nBioPage = curr;	  
			Break;
		End;

		If( ((::nBioPage > 31) ? ( ::fBio2 & (1<<(::nBioPage&31)) ) : ( ::fBio1 & (1<<(::nBioPage&31)) )));	
			wav = Rnd(2) + 0x00010040;	
			PlaySoundThis('WAVE', wav, 1, 0x00010000, 1, 4, 998);
			Break;				
		End;
	End;
	
	RunScriptGob( 0x00015100, 0x0021 );	
ENDCHUNK





CHUNK( 'WAVE', 0x00010040, ) FILE "shared\sound\bio\TBPAGEB1.WAV" ENDCHUNK	
CHUNK( 'WAVE', 0x00010041, ) FILE "shared\sound\bio\TBPAGEB2.WAV" ENDCHUNK	
CHUNK( 'WAVE', 0x00010042, ) FILE "shared\sound\bio\TBPAGEF1.WAV" ENDCHUNK
CHUNK( 'WAVE', 0x00010043, ) FILE "shared\sound\bio\TBPAGEF2.WAV" ENDCHUNK






SET _curgok = 0x00015106 CHUNK( 'GOKD', 0x00015106, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 500
	LONG 512 512 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 300 300 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK



SET _curgok = 0x00015109 CHUNK( 'GOKD', 0x00015109,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 3 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 215 58 422 407 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK


SET _curgok = 0x0001510b CHUNK( 'GOKD', 0x0001510b,  ) PACK SHORT BO OSK LONG 0 LONG 0 0 0 200 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dffill++ CHUNK( 'FILL', _dffill,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 282 424 344 451 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK






SET _curgok = 0x00015107 CHUNK( 'GOKD', 0x00015107, ) PACK SHORT BO OSK LONG 0
	LONG 0 0 0 700
	LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
SET _dftile++ SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PACK BITMAP( 0x00, 84, 61 ) "shared\bmp\biotip.bmp" ENDCHUNK CHUNK( 'TILE', _dftile,  ) PACK PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) CHILD( 'MBMP', _dfmbmp, 0 ) SHORT BO OSK
	SHORT 15 15 24 120 8 0 0 0 24
	SHORT 12 12 24 74 8 0 0 0 24
ENDCHUNK

SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
	
	FilterCmdsThis(100007, 0, 0x0021);
	FilterCmdsThis(100003, 0, 0x0021);
	PlaySoundThis('WAVE', 0x00018115 , 1, 0x00010000, 1, 1, 998);
	
ENDCHUNK

SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
	DestroyThis();
ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	DestroyThis();
ENDCHUNK





CHUNK( 'GLOP', 0x0001522e, ) SCRIPT
	
	If(_parm[0] != 0);	
		
		If( _parm[4] != 0xFFFFFFFF );
			CreateHelpGob( 0x00015100, _parm[4] );
		End;
		Return( 1 );	
	End;
ENDCHUNK


#line 49 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
#line 1 "H:\\BUILD\\SOC\\src\\shared\\portbmp.cht"



CHUNK( 'MBMP', 1,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portfolo.bmp" ENDCHUNK


CHUNK( 'MBMP', 9,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portfols.bmp" ENDCHUNK


CHUNK( 'MBMP', 2,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portok.bmp" ENDCHUNK


CHUNK( 'MBMP', 3,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portokc.bmp" ENDCHUNK


CHUNK( 'MBMP', 4,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portcan.bmp" ENDCHUNK


CHUNK( 'MBMP', 5,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\portcanc.bmp" ENDCHUNK


CHUNK( 'MBMP', 7,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\porthm.bmp" ENDCHUNK


CHUNK( 'MBMP', 8,  ) PACK BITMAP( 0, 0, 0 ) "studio\bmp\porthmc.bmp" ENDCHUNK


CHUNK( 'WAVE', 0x00010006, ) FILE "shared\sound\util\vzh1016.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010005, ) FILE "shared\sound\util\vzh1017.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00010007, ) FILE "shared\sound\util\vzh1018.wav" ENDCHUNK
#line 50 "H:\\BUILD\\SOC\\src\\shared\\shared.cht"
