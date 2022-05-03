#line 1 "H:\\BUILD\\SOC\\src\\building\\building.cht"




















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
#line 22 "H:\\BUILD\\SOC\\src\\building\\building.cht"





#line 1 "H:\\BUILD\\SOC\\inc\\stdiodef.h"














#line 1 "H:\\BUILD\\SOC\\inc\\socdef.h"














































































































































































#line 176 "H:\\BUILD\\SOC\\inc\\socdef.h"

#line 16 "H:\\BUILD\\SOC\\inc\\stdiodef.h"
#line 1 "H:\\BUILD\\SOC\\inc\\utestres.h"




#line 1 "H:\\BUILD\\SOC\\inc\\socdef.h"
















































































































































































#line 6 "H:\\BUILD\\SOC\\inc\\utestres.h"































































#line 17 "H:\\BUILD\\SOC\\inc\\stdiodef.h"














































































































































			























































































































































































































































































































































































































































































































































































































































































































































































































































































































































#line 1080 "H:\\BUILD\\SOC\\inc\\stdiodef.h"
#line 28 "H:\\BUILD\\SOC\\src\\building\\building.cht"





#line 1 "H:\\BUILD\\SOC\\inc\\socdefn.h"










#line 1 "H:\\BUILD\\SOC\\inc\\buildgob.h"




































































#line 70 "H:\\BUILD\\SOC\\inc\\buildgob.h"
#line 12 "H:\\BUILD\\SOC\\inc\\socdefn.h"


#line 1 "H:\\BUILD\\SOC\\inc\\sharedef.h"































































													












#line 78 "H:\\BUILD\\SOC\\inc\\sharedef.h"
#line 15 "H:\\BUILD\\SOC\\inc\\socdefn.h"




#line 1 "H:\\BUILD\\SOC\\inc\\balloons.chh"






































































#line 72 "H:\\BUILD\\SOC\\inc\\balloons.chh"
#line 20 "H:\\BUILD\\SOC\\inc\\socdefn.h"



















































































	






























































































































































































								  




								   


















































































































































































#line 479 "H:\\BUILD\\SOC\\inc\\socdefn.h"


#line 34 "H:\\BUILD\\SOC\\src\\building\\building.cht"
									





SET _dfmbmp = 0x00030000 SET _dfscr = 0x00030000 SET _dfanm = 0x00030000 SET _dffill = 0x00030000 SET _curgok = 0x00030000 SET _dfmask = 0x00030000 SET _dftile = 0x00030000 SET _dfwave = 0x00030000 SET _dfmidi = 0x00030000 SET _dfvid = 0x00030000





#line 1 "H:\\BUILD\\SOC\\inc\\sharutil.chh"




























































































































































































































































#line 254 "H:\\BUILD\\SOC\\inc\\sharutil.chh"

#line 47 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\inc\\biopage.chh"
















					 

















































































































































































































































































































#line 48 "H:\\BUILD\\SOC\\src\\building\\building.cht"




#line 1 "H:\\BUILD\\SOC\\inc\\biopage.chh"
















					 

















































































































































































































































































































#line 53 "H:\\BUILD\\SOC\\src\\building\\building.cht"



#line 1 "H:\\BUILD\\SOC\\inc\\projmain.h"





















































































































































































































































































































































































































#line 407 "H:\\BUILD\\SOC\\inc\\projmain.h"

#line 57 "H:\\BUILD\\SOC\\src\\building\\building.cht"





#line 1 "H:\\BUILD\\SOC\\src\\building\\ticket.cht"



















CHUNK( 'WAVE', 0x00018020, ) FILE "building\sound\ticket\TKTBTH.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001801d, ) FILE "building\sound\ticket\TBCY2.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001801f, ) FILE "building\sound\ticket\TBTH3.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001810f, ) FILE "building\sound\ticket\VO54.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018022, ) FILE "building\sound\ticket\VO56B.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018111, ) FILE "building\sound\ticket\tbfar.wav" ENDCHUNK





SET _curgok = 0x00010501 CHUNK( 'GOKD', 0x00010501,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\ticket\tik1drs.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		If( GetProp(0x23510) && (0x00010000->fTicketPS) );
			0x00010000->fTicketPS = (0);
			RunScriptGob(0x00010000, 0x0021, 0x00010580, 0x0002);		
		Else;		
			RunScriptGob(0x00010000, 0x0021, 0x00010600, 0x0003);
		End;
	ENDCHUNK								





SET _curgok = 0x00010502 CHUNK( 'GOKD', 0x00010502,  )  SHORT BO OSK LONG 2 LONG 0 0 0 15 LONG 0 0 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) SCRIPT
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018111, 0, 0x00010000, 1, 0, 998);
		i = 5;
		While( --i );
			cnt = 0x00000040;
			While( cnt <= 0x00000055 );
				Cell( cnt++, 0, 0, 6 );
			End;
		End;
		RunScriptGob(0x00010000, 0x0021, 0x00010580, 0x0002);	
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\ticket1.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\ticket\tbth1_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\ticket\tbth1_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\ticket\tbth1_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\ticket\tbth1_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\ticket\tbth1_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\ticket\tbth1_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\ticket\tbth1_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\ticket\tbth1_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\ticket\tbth1_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\ticket\tbth1_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\ticket\tbth1_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\ticket\tbth1_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\ticket\tbth1_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\ticket\tbth1_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\ticket\tbth1_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\ticket\tbth1_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\ticket\tbth1_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\ticket\tbth1_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\ticket\tbth1_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\ticket\tbth1_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\ticket\tbth1_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\ticket\tbth1_22.pbm" ENDCHUNK
#line 59 "H:\\BUILD\\SOC\\src\\building\\ticket.cht"




SET _curgok = 0x00010582 CHUNK( 'GOKD', 0x00010582,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 155 97 236 158 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK




SET _curgok = 0x00010581 CHUNK( 'GOKD', 0x00010581,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 173 31 223 61 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK




SET _curgok = 0x00010583 CHUNK( 'GOKD', 0x00010583,  )  SHORT BO OSK LONG 0 LONG 0 0 0 20 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		If (!((0x00010000->kstEntry) == 0x0002 || (0x00010000->kstEntry) == 0x0003)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\ticket.cht", ", line ", NumToStr(83,"")); End;

		.fWay = 0;	  
		ChangeStateThis((0x00010000->kstEntry));
	ENDCHUNK

 	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );	
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;			
		
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		Cell( 0x00000040, 0,0, 1 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		CreateChildGob( GidParThis(), 0x00010587, 0x00010587 );
		Cell( 0x00000040,0,0, 100 );	
		
		
		While( PlayingGob( 0x00010587 ));
			Cell( 0x00000040, 0, 0, 10 );	
		End; 
		DestroyGob( 0x00010587 );
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		ChangeStateThis( 0x0003 );	
	ENDCHUNK

 	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		Cell( 0x0000016c, 0,0, 1 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		CreateChildGob( GidParThis(), 0x00010588, 0x00010588 );
		Cell( 0x0000016c,0,0, 100 );	
		
		
		While( PlayingGob( 0x00010588 ));
			Cell( 0x0000016c, 0, 0, 10 );	
		End; 
		DestroyGob( 0x00010588 );
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		CreateChildGob( 0x00010580, 0x00010584, 0x00010584 );
		CreateChildGob( 0x00010580, 0x00010585, 0x00010585 );						
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		ChangeStateThis( 0x0004 );
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell( 0x0000016c, 0,0,1 );	
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;		

		If( !FGobExists( 0x00010584 ) );
			CreateChildGob( 0x00010580, 0x00010584, 0x00010584 );
		End;

 		If( !FGobExists( 0x00010585 ) );
			CreateChildGob( 0x00010580, 0x00010585, 0x00010585 );
		End;

		If( (0x00010000->fHelpOn) );
			CreateHelpGob( 0x00010580, 0x00018548 );
		Elif( (0x00010000->fTic1stHelp) ); 
			
			0x00010000->fTic1stHelp = (0);
 			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00010580, 0x00018548 );
		End;


		While( 1 );
			cnt=0x000001cf;
			If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x0001801d, 0, 0x00010000, 1, 0, 998);
			While( cnt <= 0x000001f7 );
				Cell( cnt++, 0, 0, 6);
			End;
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		End;
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );

		PrintStr("Ticket McZee: State 5");

		SetZThis( 900  );		  
		cnt=0x00000266;				
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x0001810f, 0, 0x00010000, 1, 0, 998);
		While( cnt < 0x00000280 );
			Cell( cnt++, 0, 0, 6 );
		End;

		CreateChildGob( 0x00010583, 0x00010101, 0x00010101 );
		0x00010101->kidCallObj = 0x00010583;
		0x00010101->chidCallScript = 0x0021;
	ENDCHUNK 

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0006 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT

		PrintStr("Ticket McZee: State 6");

		cnt=0x0000027f;
		While( cnt >= 0x00000266 );
			Cell( cnt--, 0, 0, 6 );
		End;

		If( .fWay );
			PrintStr("flag fMovie is set to fTrue");
			0x00010000->fMovie = (1); 
			RunScriptGob(0x00010000, 0x0021, 0x00010600, 0x0003);
		Else;
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
			SetZThis( 20 );
			ChangeStateThis( 0x0004 );
		End;
	ENDCHUNK;
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0007 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );

		PrintStr("Ticket McZee: State 7");

		If( FGobExists( 0x00016007 ) );
			DestroyGob( 0x00016007 );
		End;

		SetZThis( 50 );			
								
		cnt = 0x00000234;
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018022, 0, 0x00010000, 1, 0, 998);
		While( cnt <= 0x0000024e );
			Cell( cnt++, 0,0, 6 );
			If( cnt == 0x0000023e );
				DestroyGob(	0x00010585 );
			End;
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		0x00010000->fTrans = (0);  
		RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x000A);
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		If( StateThis() != 0x0004 );
			ChangeStateThis( 0x0004 );
			0x00010000->fHelpOn = (1);
		Elif( (0x00010000->fHelpOn) );				
			0x00010000->fHelpOn = (0);
			DestroyGob( 0x00016007 );
		Else;									
			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00010580, 0x00018548 );
		End;
	ENDCHUNK
	
	
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT

		If( _parm[0] == 0 );	
			.fWay = 0;
		Else;						
			.fWay = 1;
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;					
		ChangeStateThis( 0x0006 );
	ENDCHUNK
	
	
	#line 1 "H:\\BUILD\\SOC\\src\\building\\ticket.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\ticket\mczee1.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016c ) PACKEDFILE "building\pbm\ticket\tkmz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016d ) PACKEDFILE "building\pbm\ticket\tkmz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016e ) PACKEDFILE "building\pbm\ticket\tkmz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016f ) PACKEDFILE "building\pbm\ticket\tkmz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000170 ) PACKEDFILE "building\pbm\ticket\tkmz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000171 ) PACKEDFILE "building\pbm\ticket\tkmz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000172 ) PACKEDFILE "building\pbm\ticket\tkmz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000173 ) PACKEDFILE "building\pbm\ticket\tkmz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000174 ) PACKEDFILE "building\pbm\ticket\tkmz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000175 ) PACKEDFILE "building\pbm\ticket\tkmz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000176 ) PACKEDFILE "building\pbm\ticket\tkmz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000177 ) PACKEDFILE "building\pbm\ticket\tkmz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000178 ) PACKEDFILE "building\pbm\ticket\tkmz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000179 ) PACKEDFILE "building\pbm\ticket\tkmz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017a ) PACKEDFILE "building\pbm\ticket\tkmz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017b ) PACKEDFILE "building\pbm\ticket\tkmz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017c ) PACKEDFILE "building\pbm\ticket\tkmz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017d ) PACKEDFILE "building\pbm\ticket\tkmz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017e ) PACKEDFILE "building\pbm\ticket\tkmz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017f ) PACKEDFILE "building\pbm\ticket\tkmz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000180 ) PACKEDFILE "building\pbm\ticket\tkmz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000181 ) PACKEDFILE "building\pbm\ticket\tkmz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000182 ) PACKEDFILE "building\pbm\ticket\tkmz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000183 ) PACKEDFILE "building\pbm\ticket\tkmz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000184 ) PACKEDFILE "building\pbm\ticket\tkmz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000185 ) PACKEDFILE "building\pbm\ticket\tkmz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000186 ) PACKEDFILE "building\pbm\ticket\tkmz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000187 ) PACKEDFILE "building\pbm\ticket\tkmz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000188 ) PACKEDFILE "building\pbm\ticket\tkmz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000189 ) PACKEDFILE "building\pbm\ticket\tkmz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018a ) PACKEDFILE "building\pbm\ticket\tkmz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018b ) PACKEDFILE "building\pbm\ticket\tkmz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018c ) PACKEDFILE "building\pbm\ticket\tkmz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018d ) PACKEDFILE "building\pbm\ticket\tkmz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018e ) PACKEDFILE "building\pbm\ticket\tkmz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018f ) PACKEDFILE "building\pbm\ticket\tkmz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000190 ) PACKEDFILE "building\pbm\ticket\tkmz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000191 ) PACKEDFILE "building\pbm\ticket\tkmz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000192 ) PACKEDFILE "building\pbm\ticket\tkmz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000193 ) PACKEDFILE "building\pbm\ticket\tkmz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000194 ) PACKEDFILE "building\pbm\ticket\tkmz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000195 ) PACKEDFILE "building\pbm\ticket\tkmz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000196 ) PACKEDFILE "building\pbm\ticket\tkmz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000197 ) PACKEDFILE "building\pbm\ticket\tkmz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000198 ) PACKEDFILE "building\pbm\ticket\tkmz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000199 ) PACKEDFILE "building\pbm\ticket\tkmz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019a ) PACKEDFILE "building\pbm\ticket\tkmz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019b ) PACKEDFILE "building\pbm\ticket\tkmz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019c ) PACKEDFILE "building\pbm\ticket\tkmz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019d ) PACKEDFILE "building\pbm\ticket\tkmz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019e ) PACKEDFILE "building\pbm\ticket\tkmz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019f ) PACKEDFILE "building\pbm\ticket\tkmz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a0 ) PACKEDFILE "building\pbm\ticket\tkmz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a1 ) PACKEDFILE "building\pbm\ticket\tkmz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a2 ) PACKEDFILE "building\pbm\ticket\tkmz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a3 ) PACKEDFILE "building\pbm\ticket\tkmz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a4 ) PACKEDFILE "building\pbm\ticket\tkmz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a5 ) PACKEDFILE "building\pbm\ticket\tkmz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a6 ) PACKEDFILE "building\pbm\ticket\tkmz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a7 ) PACKEDFILE "building\pbm\ticket\tkmz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a8 ) PACKEDFILE "building\pbm\ticket\tkmz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a9 ) PACKEDFILE "building\pbm\ticket\tkmz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001aa ) PACKEDFILE "building\pbm\ticket\tkmz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ab ) PACKEDFILE "building\pbm\ticket\tkmz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ac ) PACKEDFILE "building\pbm\ticket\tkmz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ad ) PACKEDFILE "building\pbm\ticket\tkmz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ae ) PACKEDFILE "building\pbm\ticket\tkmz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001af ) PACKEDFILE "building\pbm\ticket\tkmz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b0 ) PACKEDFILE "building\pbm\ticket\tkmz69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b1 ) PACKEDFILE "building\pbm\ticket\tkmz70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b2 ) PACKEDFILE "building\pbm\ticket\tkmz71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b3 ) PACKEDFILE "building\pbm\ticket\tkmz72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b4 ) PACKEDFILE "building\pbm\ticket\tkmz73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b5 ) PACKEDFILE "building\pbm\ticket\tkmz74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b6 ) PACKEDFILE "building\pbm\ticket\tkmz75.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d0 ) PACKEDFILE "building\pbm\ticket\tkwait1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d1 ) PACKEDFILE "building\pbm\ticket\tkwait2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d2 ) PACKEDFILE "building\pbm\ticket\tkwait3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d3 ) PACKEDFILE "building\pbm\ticket\tkwait4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d4 ) PACKEDFILE "building\pbm\ticket\tkwait5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d5 ) PACKEDFILE "building\pbm\ticket\tkwait6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d6 ) PACKEDFILE "building\pbm\ticket\tkwait7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d7 ) PACKEDFILE "building\pbm\ticket\tkwait8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d8 ) PACKEDFILE "building\pbm\ticket\tkwait9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d9 ) PACKEDFILE "building\pbm\ticket\tkwait10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001da ) PACKEDFILE "building\pbm\ticket\tkwait11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001db ) PACKEDFILE "building\pbm\ticket\tkwait12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dc ) PACKEDFILE "building\pbm\ticket\tkwait13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dd ) PACKEDFILE "building\pbm\ticket\tkwait14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001de ) PACKEDFILE "building\pbm\ticket\tkwait15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001df ) PACKEDFILE "building\pbm\ticket\tkwait16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e0 ) PACKEDFILE "building\pbm\ticket\tkwait17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e1 ) PACKEDFILE "building\pbm\ticket\tkwait18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e2 ) PACKEDFILE "building\pbm\ticket\tkwait19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e3 ) PACKEDFILE "building\pbm\ticket\tkwait20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e4 ) PACKEDFILE "building\pbm\ticket\tkwait21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e5 ) PACKEDFILE "building\pbm\ticket\tkwait22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e6 ) PACKEDFILE "building\pbm\ticket\tkwait23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e7 ) PACKEDFILE "building\pbm\ticket\tkwait24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e8 ) PACKEDFILE "building\pbm\ticket\tkwait25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e9 ) PACKEDFILE "building\pbm\ticket\tkwait26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ea ) PACKEDFILE "building\pbm\ticket\tkwait27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001eb ) PACKEDFILE "building\pbm\ticket\tkwait28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ec ) PACKEDFILE "building\pbm\ticket\tkwait29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ed ) PACKEDFILE "building\pbm\ticket\tkwait30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ee ) PACKEDFILE "building\pbm\ticket\tkwait31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ef ) PACKEDFILE "building\pbm\ticket\tkwait32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f0 ) PACKEDFILE "building\pbm\ticket\tkwait33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f1 ) PACKEDFILE "building\pbm\ticket\tkwait34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f2 ) PACKEDFILE "building\pbm\ticket\tkwait35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f3 ) PACKEDFILE "building\pbm\ticket\tkwait36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f4 ) PACKEDFILE "building\pbm\ticket\tkwait37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f5 ) PACKEDFILE "building\pbm\ticket\tkwait38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f6 ) PACKEDFILE "building\pbm\ticket\tkwait39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001f7 ) PACKEDFILE "building\pbm\ticket\tkwait40.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000234 ) PACKEDFILE "building\pbm\ticket\tbth5_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000235 ) PACKEDFILE "building\pbm\ticket\tbth5_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000236 ) PACKEDFILE "building\pbm\ticket\tbth5_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000237 ) PACKEDFILE "building\pbm\ticket\tbth5_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000238 ) PACKEDFILE "building\pbm\ticket\tbth5_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000239 ) PACKEDFILE "building\pbm\ticket\tbth5_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023a ) PACKEDFILE "building\pbm\ticket\tbth5_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023b ) PACKEDFILE "building\pbm\ticket\tbth5_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023c ) PACKEDFILE "building\pbm\ticket\tbth5_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023d ) PACKEDFILE "building\pbm\ticket\tbth5_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023e ) PACKEDFILE "building\pbm\ticket\tbth5_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023f ) PACKEDFILE "building\pbm\ticket\tbth5_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000240 ) PACKEDFILE "building\pbm\ticket\tbth5_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000241 ) PACKEDFILE "building\pbm\ticket\tbth5_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000242 ) PACKEDFILE "building\pbm\ticket\tbth5_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000243 ) PACKEDFILE "building\pbm\ticket\tbth5_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000244 ) PACKEDFILE "building\pbm\ticket\tbth5_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000245 ) PACKEDFILE "building\pbm\ticket\tbth5_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000246 ) PACKEDFILE "building\pbm\ticket\tbth5_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000247 ) PACKEDFILE "building\pbm\ticket\tbth5_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000248 ) PACKEDFILE "building\pbm\ticket\tbth5_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000249 ) PACKEDFILE "building\pbm\ticket\tbth5_22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000024a ) PACKEDFILE "building\pbm\ticket\tbth5_23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000024b ) PACKEDFILE "building\pbm\ticket\tbth5_24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000024c ) PACKEDFILE "building\pbm\ticket\tbth5_25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000024d ) PACKEDFILE "building\pbm\ticket\tbth5_26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000024e ) PACKEDFILE "building\pbm\ticket\tbth5_27.pbm" ENDCHUNK


SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000266 ) PACKEDFILE "building\pbm\ticket\tbth4_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000267 ) PACKEDFILE "building\pbm\ticket\tbth4_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000268 ) PACKEDFILE "building\pbm\ticket\tbth4_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000269 ) PACKEDFILE "building\pbm\ticket\tbth4_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026a ) PACKEDFILE "building\pbm\ticket\tbth4_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026b ) PACKEDFILE "building\pbm\ticket\tbth4_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026c ) PACKEDFILE "building\pbm\ticket\tbth4_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026d ) PACKEDFILE "building\pbm\ticket\tbth4_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026e ) PACKEDFILE "building\pbm\ticket\tbth4_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000026f ) PACKEDFILE "building\pbm\ticket\tbth4_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000270 ) PACKEDFILE "building\pbm\ticket\tbth4_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000271 ) PACKEDFILE "building\pbm\ticket\tbth4_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000272 ) PACKEDFILE "building\pbm\ticket\tbth4_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000273 ) PACKEDFILE "building\pbm\ticket\tbth4_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000274 ) PACKEDFILE "building\pbm\ticket\tbth4_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000275 ) PACKEDFILE "building\pbm\ticket\tbth4_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000276 ) PACKEDFILE "building\pbm\ticket\tbth4_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000277 ) PACKEDFILE "building\pbm\ticket\tbth4_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000278 ) PACKEDFILE "building\pbm\ticket\tbth4_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000279 ) PACKEDFILE "building\pbm\ticket\tbth4_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027a ) PACKEDFILE "building\pbm\ticket\tbth4_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027b ) PACKEDFILE "building\pbm\ticket\tbth4_22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027c ) PACKEDFILE "building\pbm\ticket\tbth4_23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027d ) PACKEDFILE "building\pbm\ticket\tbth4_24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027e ) PACKEDFILE "building\pbm\ticket\tbth4_25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000027f ) PACKEDFILE "building\pbm\ticket\tbth4_26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000280 ) PACKEDFILE "building\pbm\ticket\tbth4_27.pbm" ENDCHUNK
#line 257 "H:\\BUILD\\SOC\\src\\building\\ticket.cht"



SET _curgok = 0x00010587 CHUNK( 'GOKD', 0x00010587,  )  SHORT BO OSK LONG 0 LONG 0 132 18 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "ticket1.avi" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		FilterCmdsThis( 100007, 0, 0x0021);
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		StopThis();	
	ENDCHUNK		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		StopThis();	
	ENDCHUNK

SET _curgok = 0x00010588 CHUNK( 'GOKD', 0x00010588,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "ticket2.avi" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		FilterCmdsThis( 100007, 0, 0x0021);
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		StopThis();	
	ENDCHUNK		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		StopThis();	
	ENDCHUNK






CHUNK( 'GLOP', 0x00018548, ) SCRIPT
	If( _parm[0] == 2 );			
		ChangeStateGob( 0x00010583, 0x0005 );
	Elif( _parm[0] == 1 );			
		ChangeStateGob( 0x00010583, 0x0007 );
	End;
ENDCHUNK




SET _curgok = 0x00010584 CHUNK( 'GOKD', 0x00010584,  )  SHORT BO OSK LONG 0 LONG 0 0 0 30 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\ticket\tik2cans.pbm" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		ChangeStateGob( 0x00010583, 0x0005 );
	ENDCHUNK





SET _curgok = 0x00010585 CHUNK( 'GOKD', 0x00010585,  )  SHORT BO OSK LONG 0 LONG 0 0 0 30 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\ticket\tik2cam.pbm" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		ChangeStateGob( 0x00010583, 0x0007 );
	ENDCHUNK





SET _curgok = 0x00010586 CHUNK( 'GOKD', 0x00010586,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\ticket\tic1m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((24 > 31) ? ( ::fBio2 & (1<<(24&31)) ) : ( ::fBio1 & (1<<(24&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (24 & 31); If (24 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 24; ::fBioCtrl = 0; CreateChildGob( 0x00010580, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK
#line 63 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\imagin.cht"





SET _curgok = 0x00010280 CHUNK( 'GOKD', 0x00010280,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		0x00010000->keys = ((0x00000002));
		RunScriptGob(0x00010000, 0x0027, 0x00010001);
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );	
		CreateChildThis( 0x00010302, 0x00010302 );
	ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) SCRIPT
		

		Cell( 0x00000040, 0,0, 1 );		
		While( PlayingGob( 0x00010302 ));
			Cell( 0x00000040, 0, 0, 10 );	
		End; 
		RunScriptGob(0x00010000, 0x0021, 0x00010300, 0x0001);
	ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\imagin\closet2.pbm" ENDCHUNK





SET _curgok = 0x00010302 CHUNK( 'GOKD', 0x00010302,  )  SHORT BO OSK LONG 0 LONG 0 0 0 111 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "logn1.avi" ENDCHUNK



CHUNK( 'WAVE', 0x00018110, ) FILE "building\sound\imagin\rllrcstr.wav" ENDCHUNK


SET _curgok = 0x00010301 CHUNK( 'GOKD', 0x00010301,  )  SHORT BO OSK LONG 0 LONG 0 0 0 111 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) SCRIPT
  		cnt = 0x00000040;
		Cell( cnt++, 0,0, 1 );	
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018110, 0, 0x00010000, 1, 0, 998);		   
		SetNoSlipThis( 1 );		
		While( cnt < 0x000000a3 );
			
			
			If( cnt == 0x00000059 );
				ChangeStateGob( 0x00010300, 0x0002 );
			Elif( cnt == 0x00000067 );
				ChangeStateGob( 0x00010300, 0x0003 );
			Elif( cnt == 0x00000077 );
				ChangeStateGob( 0x00010300, 0x0004 );
			Elif( cnt == 0x0000007e );
				ChangeStateGob( 0x00010300, 0x0005 );
			End;
			Cell( cnt, 0,0, 6 );
			++cnt;
		End;
  		RunScriptGob(0x00010000, 0x0021, 0x00010400, 0x0001);
	ENDCHUNK			
	#line 1 "H:\\BUILD\\SOC\\src\\building\\mzroller.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\imagin\cell1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\imagin\cell2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\imagin\cell3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\imagin\cell4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\imagin\cell5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\imagin\cell6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\imagin\cell7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\imagin\cell8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\imagin\cell9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\imagin\cell10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\imagin\cell11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\imagin\cell12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\imagin\cell13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\imagin\cell14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\imagin\cell15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\imagin\cell16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\imagin\cell17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\imagin\cell18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\imagin\cell19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\imagin\cell20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\imagin\cell21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\imagin\cell22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\imagin\cell23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\imagin\cell24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\imagin\cell25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\imagin\cell26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\imagin\cell27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\imagin\cell28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\imagin\cell29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\imagin\cell30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\imagin\cell31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\imagin\cell32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\imagin\cell33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\imagin\cell34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\imagin\cell35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\imagin\cell36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\imagin\cell37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\imagin\cell38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\imagin\cell39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\imagin\cell40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\imagin\cell41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\imagin\cell42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\imagin\cell43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\imagin\cell44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\imagin\cell45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\imagin\cell46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\imagin\cell47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\imagin\cell48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\imagin\cell49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\imagin\cell50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\imagin\cell51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\imagin\cell52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\imagin\cell53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\imagin\cell54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000076 ) PACKEDFILE "building\pbm\imagin\cell55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000077 ) PACKEDFILE "building\pbm\imagin\cell56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000078 ) PACKEDFILE "building\pbm\imagin\cell57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000079 ) PACKEDFILE "building\pbm\imagin\cell58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007a ) PACKEDFILE "building\pbm\imagin\cell59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007b ) PACKEDFILE "building\pbm\imagin\cell60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\imagin\cell61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\imagin\cell62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\imagin\cell63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\imagin\cell64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\imagin\cell65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\imagin\cell66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\imagin\cell67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\imagin\cell68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\imagin\cell69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\imagin\cell70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\imagin\cell71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\imagin\cell72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\imagin\cell73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\imagin\cell74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\imagin\cell75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\imagin\cell76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\imagin\cell77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\imagin\cell78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\imagin\cell79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\imagin\cell80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\imagin\cell81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\imagin\cell82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000092 ) PACKEDFILE "building\pbm\imagin\cell83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000093 ) PACKEDFILE "building\pbm\imagin\cell84.pbm" ENDCHUNK
				  
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000094 ) PACKEDFILE "building\pbm\imagin\cell85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000095 ) PACKEDFILE "building\pbm\imagin\cell86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000096 ) PACKEDFILE "building\pbm\imagin\cell87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000097 ) PACKEDFILE "building\pbm\imagin\cell88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000098 ) PACKEDFILE "building\pbm\imagin\cell89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000099 ) PACKEDFILE "building\pbm\imagin\cell90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009a ) PACKEDFILE "building\pbm\imagin\cell91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009b ) PACKEDFILE "building\pbm\imagin\cell92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009c ) PACKEDFILE "building\pbm\imagin\cell93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009d ) PACKEDFILE "building\pbm\imagin\cell94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009e ) PACKEDFILE "building\pbm\imagin\cell95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009f ) PACKEDFILE "building\pbm\imagin\cell96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a0 ) PACKEDFILE "building\pbm\imagin\cell97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a1 ) PACKEDFILE "building\pbm\imagin\cell98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a2 ) PACKEDFILE "building\pbm\imagin\cell99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\imagin\cell100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\imagin\cell101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\imagin\cell102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\imagin\cell103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\imagin\cell104.pbm" ENDCHUNK
				  
#line 61 "H:\\BUILD\\SOC\\src\\building\\imagin.cht"
#line 64 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\lobby.cht"























CHUNK( 'WAVE', 0x00010003, ) FILE "building\sound\lobby\bingo.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018015, ) FILE "building\sound\lobby\04_logn3.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018023, ) FILE "building\sound\lobby\VO57A.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001803c, ) FILE "building\sound\lobby\VO57aa.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018024, ) FILE "building\sound\lobby\VO58.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018025, ) FILE "building\sound\lobby\VO59.wav" ENDCHUNK



SET _curgok = 0x00010608 CHUNK( 'GOKD', 0x00010608,  )  SHORT BO OSK LONG 0 LONG 0 0 0 111 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK

	
	
	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		
If (!((0x00010000->kstEntry) == 2 || (0x00010000->kstEntry) == 3 || (0x00010000->kstEntry) == 5)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\lobby.cht", ", line ", NumToStr(42,"")); End;

		fFirstLoop = 1;			

		
		
		
		
		.f1sthelp=1;	
	   	.fLipsink = 0;

		If( (0x00010000->fLobby1PS) && (0x00010000->kstEntry) == 0x0002 );
			ChangeStateThis( 0x0003 );							
		Else;
			ChangeStateThis((0x00010000->kstEntry));
		End;
	ENDCHUNK

	
	
	
	
	
	 
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Lobby Mz: State 2");

		Cell( 0x000000f7, 0,0, 6);	 

		If( (0x00010000->fHelpOn) );
			CreateHelpGob( 0x00010600, 0x00018551 );
		Elif( (0x00010000->fLob1stHelp) ); 
			0x00010000->fLob1stHelp = (0);
 			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00010600, 0x00018551 );
		End;
		
		fFirstLoop = 1;
		While( !0x00010600->fExitLobby );
			
			

			
			
			
			
			
			
			
			
			
			
			
			

			If( !fFirstLoop );
				If( .fLipsink );
				   	.fLipsink = 0;
					cnt = 0x000001d0;
					If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018025, 0, 0x00010000, 1, 0, 998);
					While( cnt < 0x000001e6 );
						Cell( cnt++, 0, 0, 6);                                  
					End;
					Cell( 0x000000f3, 0, 0, 7);                                  
					Cell( 0x000000f4, 0, 0, 8);                                  
					Cell( 0x000000f5, 0, 0, 8);                                  
					Cell( 0x000000f6, 0, 0, 7);                                  
				Else;
					cnt = 0x00000105;
					While( cnt > 0x000000f7 );
						Cell( cnt--, 0, 0, 6);
					End;
				End;
			End;
			fFirstLoop = 0;
 
			cnt = 0x000000f7;
			While( cnt < 0x00000107 );
				Cell( cnt++, 0, 0, 6);                                  
			End;


			If( !0x00010600->fExitLobby );
				
				If( Rnd(100) < 20 );
					While( cnt < 0x00000119 );
						Cell( cnt++, 0, 0, 6);
					End;

					
					
					

					If( !0x00010600->fExitLobby );	
						Cell( 0x0000010a, 0, 0, 6);	
					End;
				End;
			Else;
				Cell( 0x0000010a, 0, 0, 6);
				Cell( 0x00000044, 0, 0, 6);	  
				Cell( 0x00000118, 0, 0, 6);
			End;

			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		End;


		ChangeStateThis( 0x0006 );
	ENDCHUNK

	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Lobby Mz: State 3");
		
		If( !(0x00010000->fLobby1PS) );	 
			PrintStr("flag fLobby1PS = fFalse");
			If( (0x00010000->fMovie) );
				PrintStr("flag fMovie = fTrue");
				RunScriptGob( 0x00010000, 0x0024, 0x0001001c, 0 );
				StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
				Cell( 0x000000f1, 0,0, 6);
				If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;		

				cnt = 0x000000f1;
				While( cnt < 0x00000119 );		 
					Cell( cnt++, 0,0, 6);
				End;

				ChangeStateThis( 0x0006 );	
			Else;
				ChangeStateThis( 0x0002 );
			End;
		Else;									 
			0x00010000->fLobby1PS = (0);		 
			If( (0x00010000->fMovie) );
				StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
				RunScriptGob( 0x00010000, 0x0024, 0x0001001c, 0 );		 
			End;								 
			cnt = 0x00000049;
			Cell( 0x00000049, 0,0, 6);
			If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;		
			While( cnt < 0x000000b4 );
				If( cnt == 0x0000005e );
					If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018023, 0, 0x00010000, 1, 0, 998);
				Elif( cnt == 0x00000078 );
					If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x0001803c, 0, 0x00010000, 1, 0, 998);
				End;
				Cell( cnt++, 0,0, 6);
			End;

			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

			If( (0x00010000->fMovie) );

				cnt = 0x00000109;
				While( cnt < 0x00000119 );		 
					Cell( cnt++, 0,0, 6);
				End;

				ChangeStateThis( 0x0006 );
			Else;			
				ChangeStateThis( 0x0004 );
			End;
		End;
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Lobby Mz: State 4");
		cnt = 0x000000b4;
		Cell( 0x000000b4, 0,0, 1);
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;		

		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018024, 0, 0x00010000, 1, 0, 998);
		While( cnt < 0x000000f7 );
			Cell( cnt++, 0,0, 6);
		End;

		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

		ChangeStateThis( 0x0002 );
	ENDCHUNK
 	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Lobby Mz: State 5");
		
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		RunScriptGob( 0x00010000, 0x0024, 0x0001001c, 0 );
		Cell( 0x0000016d, 0, 0, 1);
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;
		CreateChildGob( 0x00010600, 0x0001060a, 0x0001060a );

		
		Cell( 0x0000016d, 0,0, 100 );
		While( PlayingGob( 0x0001060a ));
			Cell( 0x0000016d, 0, 0, 10 );	
		End; 
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		RunScriptGob(0x00010000, 0x0021, 0x00010c00, 0x0007);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0006 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Lobby Mz: State 6");
		cnt = 0x0000011a;
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		While( cnt < 0x00000143 );
			Cell( cnt++, 0,0, 6);
		End;
		RunScriptGob(0x00010000, 0x0021, 0x00010c00, 0x0005);
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		PrintStr("Lobby Mz: Clicked");
		If( StateThis() != 0x0002 );	
			ChangeStateThis( 0x0002 );
			0x00010000->fHelpOn = (1);		
		Elif( (0x00010000->fHelpOn) );
			0x00010000->fHelpOn = (0);
			DestroyGob( 0x00016007 );   
		Else;
			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00010600, 0x00018551 );
		End;
	ENDCHUNK

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\lobby\fork217.pbm" ENDCHUNK
 	#line 1 "H:\\BUILD\\SOC\\src\\building\\mzlobby.seq"





SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\lobby\l1mz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\lobby\l1mz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\lobby\l1mz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\lobby\l1mz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\lobby\l1mz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\lobby\l1mz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\lobby\l1mz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\lobby\l1mz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\lobby\l1mz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\lobby\l1mz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\lobby\l1mz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\lobby\l1mz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\lobby\l1mz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\lobby\l1mz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\lobby\l1mz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\lobby\l1mz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\lobby\l1mz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\lobby\l1mz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\lobby\l1mz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\lobby\l1mz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\lobby\l1mz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\lobby\l1mz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\lobby\l1mz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\lobby\l1mz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\lobby\l1mz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\lobby\l1mz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\lobby\l1mz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\lobby\l1mz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\lobby\l1mz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\lobby\l1mz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\lobby\l1mz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\lobby\l1mz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\lobby\l1mz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\lobby\l1mz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\lobby\l1mz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\lobby\l1mz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\lobby\l1mz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\lobby\l1mz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\lobby\l1mz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\lobby\l1mz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\lobby\l1mz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\lobby\l1mz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\lobby\l1mz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\lobby\l1mz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\lobby\l1mz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000076 ) PACKEDFILE "building\pbm\lobby\l1mz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000077 ) PACKEDFILE "building\pbm\lobby\l1mz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000078 ) PACKEDFILE "building\pbm\lobby\l1mz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000079 ) PACKEDFILE "building\pbm\lobby\l1mz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007a ) PACKEDFILE "building\pbm\lobby\l1mz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007b ) PACKEDFILE "building\pbm\lobby\l1mz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\lobby\l1mz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\lobby\l1mz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\lobby\l1mz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\lobby\l1mz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\lobby\l1mz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\lobby\l1mz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\lobby\l1mz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\lobby\l1mz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\lobby\l1mz69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\lobby\l1mz70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\lobby\l1mz71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\lobby\l1mz72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\lobby\l1mz73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\lobby\l1mz74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\lobby\l1mz75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\lobby\l1mz76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\lobby\l1mz77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\lobby\l1mz78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\lobby\l1mz79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\lobby\l1mz80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\lobby\l1mz81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\lobby\l1mz82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000092 ) PACKEDFILE "building\pbm\lobby\l1mz83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000093 ) PACKEDFILE "building\pbm\lobby\l1mz84.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000094 ) PACKEDFILE "building\pbm\lobby\l1mz85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000095 ) PACKEDFILE "building\pbm\lobby\l1mz86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000096 ) PACKEDFILE "building\pbm\lobby\l1mz87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000097 ) PACKEDFILE "building\pbm\lobby\l1mz88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000098 ) PACKEDFILE "building\pbm\lobby\l1mz89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000099 ) PACKEDFILE "building\pbm\lobby\l1mz90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009a ) PACKEDFILE "building\pbm\lobby\l1mz91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009b ) PACKEDFILE "building\pbm\lobby\l1mz92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009c ) PACKEDFILE "building\pbm\lobby\l1mz93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009d ) PACKEDFILE "building\pbm\lobby\l1mz94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009e ) PACKEDFILE "building\pbm\lobby\l1mz95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009f ) PACKEDFILE "building\pbm\lobby\l1mz96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a0 ) PACKEDFILE "building\pbm\lobby\l1mz97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a1 ) PACKEDFILE "building\pbm\lobby\l1mz98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a2 ) PACKEDFILE "building\pbm\lobby\l1mz99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\lobby\l1mz100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\lobby\l1mz101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\lobby\l1mz102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\lobby\l1mz103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\lobby\l1mz104.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\lobby\l1mz105.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\lobby\l1mz106.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\lobby\l1mz107.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\lobby\l1mz108.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\lobby\l1mz109.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\lobby\l1mz110.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\lobby\l1mz111.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\lobby\l1mz112.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\lobby\l1mz113.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\lobby\l1mz114.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\lobby\l1mz115.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\lobby\l1mz116.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\lobby\l1mz117.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\lobby\l1mz118.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\lobby\l1mz119.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\lobby\l1mz120.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\lobby\l1mz121.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\lobby\l1mz122.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\lobby\l1mz123.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\lobby\l1mz124.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\lobby\l1mz125.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\lobby\l1mz126.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\lobby\l1mz127.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\lobby\l1mz128.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\lobby\l1mz129.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\lobby\l1mz130.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\lobby\l1mz131.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\lobby\l1mz132.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\lobby\l1mz133.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\lobby\l1mz134.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\lobby\l1mz135.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\lobby\l1mz136.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\lobby\l1mz137.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\lobby\l1mz138.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\lobby\l1mz139.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\lobby\l1mz140.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\lobby\l1mz141.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\lobby\l1mz142.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\lobby\l1mz143.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\lobby\l1mz144.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\lobby\l1mz145.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\lobby\l1mz146.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\lobby\l1mz147.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d3 ) PACKEDFILE "building\pbm\lobby\l1mz148.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d4 ) PACKEDFILE "building\pbm\lobby\l1mz149.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\lobby\l1mz150.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\lobby\l1mz151.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\lobby\l1mz152.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\lobby\l1mz153.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\lobby\l1mz154.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\lobby\l1mz155.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\lobby\l1mz156.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\lobby\l1mz157.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\lobby\l1mz158.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\lobby\l1mz159.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\lobby\l1mz160.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\lobby\l1mz161.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\lobby\l1mz162.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\lobby\l1mz163.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\lobby\l1mz164.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e4 ) PACKEDFILE "building\pbm\lobby\l1mz165.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e5 ) PACKEDFILE "building\pbm\lobby\l1mz166.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e6 ) PACKEDFILE "building\pbm\lobby\l1mz167.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e7 ) PACKEDFILE "building\pbm\lobby\l1mz168.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e8 ) PACKEDFILE "building\pbm\lobby\l1mz169.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e9 ) PACKEDFILE "building\pbm\lobby\l1mz170.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ea ) PACKEDFILE "building\pbm\lobby\l1mz171.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000eb ) PACKEDFILE "building\pbm\lobby\l1mz172.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ec ) PACKEDFILE "building\pbm\lobby\l1mz173.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ed ) PACKEDFILE "building\pbm\lobby\l1mz174.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ee ) PACKEDFILE "building\pbm\lobby\l1mz175.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ef ) PACKEDFILE "building\pbm\lobby\l1mz176.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f0 ) PACKEDFILE "building\pbm\lobby\l1mz177.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f1 ) PACKEDFILE "building\pbm\lobby\l1mz178.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f2 ) PACKEDFILE "building\pbm\lobby\l1mz179.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f3 ) PACKEDFILE "building\pbm\lobby\l1mz180.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f4 ) PACKEDFILE "building\pbm\lobby\l1mz181.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f5 ) PACKEDFILE "building\pbm\lobby\l1mz182.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f6 ) PACKEDFILE "building\pbm\lobby\l1mz183.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f7 ) PACKEDFILE "building\pbm\lobby\l1mz184.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f8 ) PACKEDFILE "building\pbm\lobby\l1mz185.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f9 ) PACKEDFILE "building\pbm\lobby\l1mz186.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fa ) PACKEDFILE "building\pbm\lobby\l1mz187.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fb ) PACKEDFILE "building\pbm\lobby\l1mz188.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fc ) PACKEDFILE "building\pbm\lobby\l1mz189.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fd ) PACKEDFILE "building\pbm\lobby\l1mz190.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fe ) PACKEDFILE "building\pbm\lobby\l1mz191.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ff ) PACKEDFILE "building\pbm\lobby\l1mz192.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000100 ) PACKEDFILE "building\pbm\lobby\l1mz193.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000101 ) PACKEDFILE "building\pbm\lobby\l1mz194.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000102 ) PACKEDFILE "building\pbm\lobby\l1mz195.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000103 ) PACKEDFILE "building\pbm\lobby\l1mz196.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000104 ) PACKEDFILE "building\pbm\lobby\l1mz197.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000105 ) PACKEDFILE "building\pbm\lobby\l1mz198.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000106 ) PACKEDFILE "building\pbm\lobby\l1mz199.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000107 ) PACKEDFILE "building\pbm\lobby\l1mz200.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000108 ) PACKEDFILE "building\pbm\lobby\l1mz201.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000109 ) PACKEDFILE "building\pbm\lobby\l1mz202.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010a ) PACKEDFILE "building\pbm\lobby\l1mz203.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010b ) PACKEDFILE "building\pbm\lobby\l1mz204.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010c ) PACKEDFILE "building\pbm\lobby\l1mz205.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010d ) PACKEDFILE "building\pbm\lobby\l1mz206.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010e ) PACKEDFILE "building\pbm\lobby\l1mz207.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010f ) PACKEDFILE "building\pbm\lobby\l1mz208.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000110 ) PACKEDFILE "building\pbm\lobby\l1mz209.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000111 ) PACKEDFILE "building\pbm\lobby\l1mz210.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000112 ) PACKEDFILE "building\pbm\lobby\l1mz211.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000113 ) PACKEDFILE "building\pbm\lobby\l1mz212.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000114 ) PACKEDFILE "building\pbm\lobby\l1mz213.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000115 ) PACKEDFILE "building\pbm\lobby\l1mz214.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000116 ) PACKEDFILE "building\pbm\lobby\l1mz215.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000117 ) PACKEDFILE "building\pbm\lobby\l1mz216.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000118 ) PACKEDFILE "building\pbm\lobby\l1mz217.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000119 ) PACKEDFILE "building\pbm\lobby\l1mz218.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011a ) PACKEDFILE "building\pbm\lobby\l1mz219.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011b ) PACKEDFILE "building\pbm\lobby\l1mz220.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011c ) PACKEDFILE "building\pbm\lobby\l1mz221.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011d ) PACKEDFILE "building\pbm\lobby\l1mz222.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011e ) PACKEDFILE "building\pbm\lobby\l1mz223.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011f ) PACKEDFILE "building\pbm\lobby\l1mz224.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000120 ) PACKEDFILE "building\pbm\lobby\l1mz225.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000121 ) PACKEDFILE "building\pbm\lobby\l1mz226.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000122 ) PACKEDFILE "building\pbm\lobby\l1mz227.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000123 ) PACKEDFILE "building\pbm\lobby\l1mz228.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000124 ) PACKEDFILE "building\pbm\lobby\l1mz229.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000125 ) PACKEDFILE "building\pbm\lobby\l1mz230.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000126 ) PACKEDFILE "building\pbm\lobby\l1mz231.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000127 ) PACKEDFILE "building\pbm\lobby\l1mz232.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000128 ) PACKEDFILE "building\pbm\lobby\l1mz233.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000129 ) PACKEDFILE "building\pbm\lobby\l1mz234.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012a ) PACKEDFILE "building\pbm\lobby\l1mz235.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012b ) PACKEDFILE "building\pbm\lobby\l1mz236.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012c ) PACKEDFILE "building\pbm\lobby\l1mz237.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012d ) PACKEDFILE "building\pbm\lobby\l1mz238.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012e ) PACKEDFILE "building\pbm\lobby\l1mz239.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012f ) PACKEDFILE "building\pbm\lobby\l1mz240.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000130 ) PACKEDFILE "building\pbm\lobby\l1mz241.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000131 ) PACKEDFILE "building\pbm\lobby\l1mz242.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000132 ) PACKEDFILE "building\pbm\lobby\l1mz243.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000133 ) PACKEDFILE "building\pbm\lobby\l1mz244.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000134 ) PACKEDFILE "building\pbm\lobby\l1mz245.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000135 ) PACKEDFILE "building\pbm\lobby\l1mz246.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000136 ) PACKEDFILE "building\pbm\lobby\l1mz247.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000137 ) PACKEDFILE "building\pbm\lobby\l1mz248.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000138 ) PACKEDFILE "building\pbm\lobby\l1mz249.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000139 ) PACKEDFILE "building\pbm\lobby\l1mz250.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013a ) PACKEDFILE "building\pbm\lobby\l1mz251.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013b ) PACKEDFILE "building\pbm\lobby\l1mz252.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013c ) PACKEDFILE "building\pbm\lobby\l1mz253.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013d ) PACKEDFILE "building\pbm\lobby\l1mz254.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013e ) PACKEDFILE "building\pbm\lobby\l1mz255.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013f ) PACKEDFILE "building\pbm\lobby\l1mz256.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000140 ) PACKEDFILE "building\pbm\lobby\l1mz257.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000141 ) PACKEDFILE "building\pbm\lobby\l1mz258.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000142 ) PACKEDFILE "building\pbm\lobby\l1mz259.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000143 ) PACKEDFILE "building\pbm\lobby\l1mz260.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016c ) PACKEDFILE "building\pbm\lobby\l3mz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016d ) PACKEDFILE "building\pbm\lobby\l3mz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016e ) PACKEDFILE "building\pbm\lobby\l3mz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016f ) PACKEDFILE "building\pbm\lobby\l3mz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000170 ) PACKEDFILE "building\pbm\lobby\l3mz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000171 ) PACKEDFILE "building\pbm\lobby\l3mz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000172 ) PACKEDFILE "building\pbm\lobby\l3mz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000173 ) PACKEDFILE "building\pbm\lobby\l3mz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000174 ) PACKEDFILE "building\pbm\lobby\l3mz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000175 ) PACKEDFILE "building\pbm\lobby\l3mz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000176 ) PACKEDFILE "building\pbm\lobby\l3mz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000177 ) PACKEDFILE "building\pbm\lobby\l3mz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000178 ) PACKEDFILE "building\pbm\lobby\l3mz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000179 ) PACKEDFILE "building\pbm\lobby\l3mz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017a ) PACKEDFILE "building\pbm\lobby\l3mz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017b ) PACKEDFILE "building\pbm\lobby\l3mz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017c ) PACKEDFILE "building\pbm\lobby\l3mz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017d ) PACKEDFILE "building\pbm\lobby\l3mz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017e ) PACKEDFILE "building\pbm\lobby\l3mz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017f ) PACKEDFILE "building\pbm\lobby\l3mz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000180 ) PACKEDFILE "building\pbm\lobby\l3mz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000181 ) PACKEDFILE "building\pbm\lobby\l3mz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000182 ) PACKEDFILE "building\pbm\lobby\l3mz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000183 ) PACKEDFILE "building\pbm\lobby\l3mz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000184 ) PACKEDFILE "building\pbm\lobby\l3mz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000185 ) PACKEDFILE "building\pbm\lobby\l3mz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000186 ) PACKEDFILE "building\pbm\lobby\l3mz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000187 ) PACKEDFILE "building\pbm\lobby\l3mz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000188 ) PACKEDFILE "building\pbm\lobby\l3mz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000189 ) PACKEDFILE "building\pbm\lobby\l3mz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018a ) PACKEDFILE "building\pbm\lobby\l3mz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018b ) PACKEDFILE "building\pbm\lobby\l3mz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018c ) PACKEDFILE "building\pbm\lobby\l3mz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018d ) PACKEDFILE "building\pbm\lobby\l3mz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018e ) PACKEDFILE "building\pbm\lobby\l3mz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018f ) PACKEDFILE "building\pbm\lobby\l3mz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000190 ) PACKEDFILE "building\pbm\lobby\l3mz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000191 ) PACKEDFILE "building\pbm\lobby\l3mz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000192 ) PACKEDFILE "building\pbm\lobby\l3mz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000193 ) PACKEDFILE "building\pbm\lobby\l3mz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000194 ) PACKEDFILE "building\pbm\lobby\l3mz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000195 ) PACKEDFILE "building\pbm\lobby\l3mz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000196 ) PACKEDFILE "building\pbm\lobby\l3mz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000197 ) PACKEDFILE "building\pbm\lobby\l3mz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000198 ) PACKEDFILE "building\pbm\lobby\l3mz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000199 ) PACKEDFILE "building\pbm\lobby\l3mz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019a ) PACKEDFILE "building\pbm\lobby\l3mz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019b ) PACKEDFILE "building\pbm\lobby\l3mz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019c ) PACKEDFILE "building\pbm\lobby\l3mz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019d ) PACKEDFILE "building\pbm\lobby\l3mz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019e ) PACKEDFILE "building\pbm\lobby\l3mz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019f ) PACKEDFILE "building\pbm\lobby\l3mz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a0 ) PACKEDFILE "building\pbm\lobby\l3mz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a1 ) PACKEDFILE "building\pbm\lobby\l3mz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a2 ) PACKEDFILE "building\pbm\lobby\l3mz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a3 ) PACKEDFILE "building\pbm\lobby\l3mz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a4 ) PACKEDFILE "building\pbm\lobby\l3mz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a5 ) PACKEDFILE "building\pbm\lobby\l3mz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a6 ) PACKEDFILE "building\pbm\lobby\l3mz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a7 ) PACKEDFILE "building\pbm\lobby\l3mz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a8 ) PACKEDFILE "building\pbm\lobby\l3mz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a9 ) PACKEDFILE "building\pbm\lobby\l3mz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001aa ) PACKEDFILE "building\pbm\lobby\l3mz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ab ) PACKEDFILE "building\pbm\lobby\l3mz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ac ) PACKEDFILE "building\pbm\lobby\l3mz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ad ) PACKEDFILE "building\pbm\lobby\l3mz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ae ) PACKEDFILE "building\pbm\lobby\l3mz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001af ) PACKEDFILE "building\pbm\lobby\l3mz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b0 ) PACKEDFILE "building\pbm\lobby\l3mz69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b1 ) PACKEDFILE "building\pbm\lobby\l3mz70.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d0 ) PACKEDFILE "building\pbm\lobby\lipsk157.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d1 ) PACKEDFILE "building\pbm\lobby\lipsk158.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d2 ) PACKEDFILE "building\pbm\lobby\lipsk159.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d3 ) PACKEDFILE "building\pbm\lobby\lipsk160.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d4 ) PACKEDFILE "building\pbm\lobby\lipsk161.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d5 ) PACKEDFILE "building\pbm\lobby\lipsk162.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d6 ) PACKEDFILE "building\pbm\lobby\lipsk163.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d7 ) PACKEDFILE "building\pbm\lobby\lipsk164.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d8 ) PACKEDFILE "building\pbm\lobby\lipsk165.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d9 ) PACKEDFILE "building\pbm\lobby\lipsk166.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001da ) PACKEDFILE "building\pbm\lobby\lipsk167.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001db ) PACKEDFILE "building\pbm\lobby\lipsk168.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dc ) PACKEDFILE "building\pbm\lobby\lipsk169.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dd ) PACKEDFILE "building\pbm\lobby\lipsk170.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001de ) PACKEDFILE "building\pbm\lobby\lipsk171.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001df ) PACKEDFILE "building\pbm\lobby\lipsk172.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e0 ) PACKEDFILE "building\pbm\lobby\lipsk173.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e1 ) PACKEDFILE "building\pbm\lobby\lipsk174.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e2 ) PACKEDFILE "building\pbm\lobby\lipsk175.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e3 ) PACKEDFILE "building\pbm\lobby\lipsk176.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e4 ) PACKEDFILE "building\pbm\lobby\lipsk177.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e5 ) PACKEDFILE "building\pbm\lobby\lipsk178.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e6 ) PACKEDFILE "building\pbm\lobby\lipsk179.pbm" ENDCHUNK
#line 270 "H:\\BUILD\\SOC\\src\\building\\lobby.cht"

SET _curgok = 0x0001060a CHUNK( 'GOKD', 0x0001060a,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "04logn3.avi" ENDCHUNK



CHUNK( 'GLOP', 0x00018551, ) SCRIPT
 	If( _parm[0] == 0 );
		If( 0x00010608->f1sthelp );
			0x00010608->fLipsink = 1;
			0x00010608->f1sthelp = 0;
		End;
		Exit();
	Elif( _parm[0] == 1 );		
		DestroyGob( 0x00016007 );
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		0x00010600->fExitLobby=1;

	Elif( _parm[0] == 2 );		

		
		

		RunScriptGob(0x00010000, 0x0021, 0x00010800, 0x0001);
	End;
ENDCHUNK
	
SET _curgok = 0x00010607 CHUNK( 'GOKD', 0x00010607,  )  SHORT BO OSK LONG 0 LONG 0 0 0 33 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lob1door.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		If(	!0x00010600->fExitLobby );  	
			RunScriptGob( 0x00010000, 0x0024, 0x0001001c, 0 );
			If( (0x00010000->fHelpOn) );  			
				DestroyGob( 0x00016007 );   	
			End;
			If( StateGob(0x00010608) != 0x0002 );  
				RunScriptGob(0x00010000, 0x0021, 0x00010c00, 0x0005);	  
			Else;									  
				StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
				0x00010600->fExitLobby=1;  
			End;	
		End;
	ENDCHUNK



SET _curgok = 0x00010603 CHUNK( 'GOKD', 0x00010603,  )  SHORT BO OSK LONG 0 LONG 0 0 0 45 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\poster3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((35 > 31) ? ( ::fBio2 & (1<<(35&31)) ) : ( ::fBio1 & (1<<(35&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (35 & 31); If (35 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 35; ::fBioCtrl = 0; CreateChildGob( 0x00010600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK


SET _curgok = 0x00010604 CHUNK( 'GOKD', 0x00010604,  )  SHORT BO OSK LONG 0 LONG 0 0 0 45 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\poster4.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((36 > 31) ? ( ::fBio2 & (1<<(36&31)) ) : ( ::fBio1 & (1<<(36&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (36 & 31); If (36 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 36; ::fBioCtrl = 0; CreateChildGob( 0x00010600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK



SET _curgok = 0x00010703 CHUNK( 'GOKD', 0x00010703,  )  SHORT BO OSK LONG 0 LONG 0 0 0 22 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lob2door.bmp" ENDCHUNK 
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		RunScriptGob(0x00010000, 0x0021, 0x00010500, 0x0001);
	ENDCHUNK

SET _curgok = 0x00010705 CHUNK( 'GOKD', 0x00010705,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lob2map.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK

SET _curgok = 0x00010704 CHUNK( 'GOKD', 0x00010704,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 403 187 429 204 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK

SET _curgok = 0x00010609 CHUNK( 'GOKD', 0x00010609,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lobby1m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((30 > 31) ? ( ::fBio2 & (1<<(30&31)) ) : ( ::fBio1 & (1<<(30&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (30 & 31); If (30 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 30; ::fBioCtrl = 0; CreateChildGob( 0x00010600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK
#line 65 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\snackbar.cht"




CHUNK( 'WAVE', 0x00018026, ) FILE "building\sound\snackbar\VO60.wav" ENDCHUNK

SET _curgok = 0x00010806 CHUNK( 'GOKD', 0x00010806, )  SHORT BO OSK LONG 0
	LONG 0 0 0 0
	LONG 0 0 0x00000002 0x00000001 0x0020 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		While( 1 );
			cnt=0x000000d5;
			While( cnt <= 0x000000e3 );
				Cell( cnt++, 0, 0, 6 );		
			End;
		End;
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 ); 	
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018026, 0, 0x00010000, 1, 0, 998);
		cnt=0x00000040;
		While( cnt <= 0x000000d2 );
			Cell( cnt++, 0, 0, 6 );		
		End;
		
		ChangeStateThis( 0x0001 );
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0020 ) SCRIPT
		ChangeStateThis( 0x0002 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\snackbar.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\snackbar\mczee1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\snackbar\mczee2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\snackbar\mczee3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\snackbar\mczee4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\snackbar\mczee5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\snackbar\mczee6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\snackbar\mczee7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\snackbar\mczee8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\snackbar\mczee9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\snackbar\mczee10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\snackbar\mczee11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\snackbar\mczee12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\snackbar\mczee13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\snackbar\mczee14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\snackbar\mczee15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\snackbar\mczee16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\snackbar\mczee17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\snackbar\mczee18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\snackbar\mczee19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\snackbar\mczee20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\snackbar\mczee21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\snackbar\mczee22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\snackbar\mczee23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\snackbar\mczee24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\snackbar\mczee25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\snackbar\mczee26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\snackbar\mczee27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\snackbar\mczee28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\snackbar\mczee29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\snackbar\mczee30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\snackbar\mczee31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\snackbar\mczee32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\snackbar\mczee33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\snackbar\mczee34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\snackbar\mczee35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\snackbar\mczee36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\snackbar\mczee37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\snackbar\mczee38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\snackbar\mczee39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\snackbar\mczee40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\snackbar\mczee41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\snackbar\mczee42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\snackbar\mczee43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\snackbar\mczee44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\snackbar\mczee45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\snackbar\mczee46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\snackbar\mczee47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\snackbar\mczee48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\snackbar\mczee49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\snackbar\mczee50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\snackbar\mczee51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\snackbar\mczee52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\snackbar\mczee53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\snackbar\mczee54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000076 ) PACKEDFILE "building\pbm\snackbar\mczee55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000077 ) PACKEDFILE "building\pbm\snackbar\mczee56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000078 ) PACKEDFILE "building\pbm\snackbar\mczee57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000079 ) PACKEDFILE "building\pbm\snackbar\mczee58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007a ) PACKEDFILE "building\pbm\snackbar\mczee59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007b ) PACKEDFILE "building\pbm\snackbar\mczee60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\snackbar\mczee61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\snackbar\mczee62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\snackbar\mczee63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\snackbar\mczee64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\snackbar\mczee65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\snackbar\mczee66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\snackbar\mczee67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\snackbar\mczee68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\snackbar\mczee69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\snackbar\mczee70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\snackbar\mczee71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\snackbar\mczee72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\snackbar\mczee73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\snackbar\mczee74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\snackbar\mczee75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\snackbar\mczee76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\snackbar\mczee77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\snackbar\mczee78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\snackbar\mczee79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\snackbar\mczee80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\snackbar\mczee81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\snackbar\mczee82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000092 ) PACKEDFILE "building\pbm\snackbar\mczee83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000093 ) PACKEDFILE "building\pbm\snackbar\mczee84.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000094 ) PACKEDFILE "building\pbm\snackbar\mczee85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000095 ) PACKEDFILE "building\pbm\snackbar\mczee86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000096 ) PACKEDFILE "building\pbm\snackbar\mczee87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000097 ) PACKEDFILE "building\pbm\snackbar\mczee88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000098 ) PACKEDFILE "building\pbm\snackbar\mczee89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000099 ) PACKEDFILE "building\pbm\snackbar\mczee90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009a ) PACKEDFILE "building\pbm\snackbar\mczee91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009b ) PACKEDFILE "building\pbm\snackbar\mczee92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009c ) PACKEDFILE "building\pbm\snackbar\mczee93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009d ) PACKEDFILE "building\pbm\snackbar\mczee94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009e ) PACKEDFILE "building\pbm\snackbar\mczee95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009f ) PACKEDFILE "building\pbm\snackbar\mczee96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a0 ) PACKEDFILE "building\pbm\snackbar\mczee97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a1 ) PACKEDFILE "building\pbm\snackbar\mczee98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a2 ) PACKEDFILE "building\pbm\snackbar\mczee99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\snackbar\mczee100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\snackbar\mczee101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\snackbar\mczee102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\snackbar\mczee103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\snackbar\mczee104.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\snackbar\mczee105.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\snackbar\mczee106.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\snackbar\mczee107.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\snackbar\mczee108.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\snackbar\mczee109.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\snackbar\mczee110.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\snackbar\mczee111.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\snackbar\mczee112.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\snackbar\mczee113.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\snackbar\mczee114.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\snackbar\mczee115.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\snackbar\mczee116.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\snackbar\mczee117.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\snackbar\mczee118.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\snackbar\mczee119.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\snackbar\mczee120.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\snackbar\mczee121.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\snackbar\mczee122.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\snackbar\mczee123.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\snackbar\mczee124.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\snackbar\mczee125.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\snackbar\mczee126.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\snackbar\mczee127.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\snackbar\mczee128.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\snackbar\mczee129.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\snackbar\mczee130.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\snackbar\mczee131.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\snackbar\mczee132.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\snackbar\mczee133.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\snackbar\mczee134.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\snackbar\mczee135.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\snackbar\mczee136.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\snackbar\mczee137.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\snackbar\mczee138.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\snackbar\mczee139.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\snackbar\mczee140.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\snackbar\mczee141.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\snackbar\mczee142.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\snackbar\mczee143.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\snackbar\mczee144.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\snackbar\mczee145.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\snackbar\mczee146.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\snackbar\mczee147.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\snackbar\cyc46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\snackbar\cyc47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\snackbar\cyc48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\snackbar\cyc49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\snackbar\cyc50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\snackbar\cyc51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\snackbar\cyc52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\snackbar\cyc53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\snackbar\cyc54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\snackbar\cyc55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\snackbar\cyc56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\snackbar\cyc57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\snackbar\cyc58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\snackbar\cyc59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\snackbar\cyc60.pbm" ENDCHUNK
#line 36 "H:\\BUILD\\SOC\\src\\building\\snackbar.cht"

SET _curgok = 0x00010807 CHUNK( 'GOKD', 0x00010807,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lobby2m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((28 > 31) ? ( ::fBio2 & (1<<(28&31)) ) : ( ::fBio1 & (1<<(28&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (28 & 31); If (28 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 28; ::fBioCtrl = 0; CreateChildGob( 0x00010800, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00010808 CHUNK( 'GOKD', 0x00010808,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\lobby\lobby2m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((29 > 31) ? ( ::fBio2 & (1<<(29&31)) ) : ( ::fBio1 & (1<<(29&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (29 & 31); If (29 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 29; ::fBioCtrl = 0; CreateChildGob( 0x00010800, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

#line 66 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\theatre.cht"














































































CHUNK( 'WAVE', 0x00018107, ) FILE "building\sound\theatre\30_thr4.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018027, ) FILE "building\sound\theatre\21_thr1a.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018029, ) FILE "building\sound\theatre\VO74B.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018108, ) FILE "building\sound\theatre\VO75.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001802a, ) FILE "building\sound\theatre\VO76.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001810d, ) FILE "building\sound\theatre\21_thr1b.wav" ENDCHUNK

SET _curgok = 0x00010c03 CHUNK( 'GOKD', 0x00010c03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 550 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, 0x0000003f ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\mzthea.seq"






SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\theatre\thr2a1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\theatre\thr2a2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\theatre\thr2a3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\theatre\thr2a4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\theatre\thr2a5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\theatre\thr2a6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\theatre\thr2a7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\theatre\thr2a8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\theatre\thr2a9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\theatre\thr2a10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\theatre\thr2a11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\theatre\thr2a12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\theatre\thr2a13.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\theatre\thr2b1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\theatre\thr2b2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\theatre\thr2b3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\theatre\thr2b4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\theatre\thr2b5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\theatre\thr2b6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\theatre\thr2b7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\theatre\thr2b8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\theatre\thr2b9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\theatre\thr2b10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\theatre\thr2b11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\theatre\thr2b12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\theatre\thr2b13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\theatre\thr2b14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\theatre\thr2b15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\theatre\thr2b16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\theatre\thr2b17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\theatre\thr2b18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\theatre\thr2b19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\theatre\thr2b20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\theatre\thr2b21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\theatre\thr2b22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\theatre\thr2b23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\theatre\thr2b24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\theatre\thr2b25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\theatre\thr2b26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\theatre\thr2b27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\theatre\thr2b28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\theatre\thr2b29.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\theatre\thr2d1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\theatre\thr2d2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\theatre\thr2d3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\theatre\thr2d4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\theatre\thr2d5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\theatre\thr2d6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\theatre\thr2d7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\theatre\thr2d8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\theatre\thr2d9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\theatre\thr2d10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\theatre\thr2d11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\theatre\thr2d12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\theatre\thr2d13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\theatre\thr2d14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\theatre\thr2d15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\theatre\thr2d16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\theatre\thr2d17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\theatre\thr2d18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\theatre\thr2d19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\theatre\thr2d20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\theatre\thr2d21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\theatre\thr2d22.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\theatre\thr2e1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\theatre\thr2e2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\theatre\thr2e3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\theatre\thr2e4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\theatre\thr2e5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\theatre\thr2e6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\theatre\thr2e7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\theatre\thr2e8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\theatre\thr2e9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\theatre\thr2e10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\theatre\thr2e11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\theatre\thr2e12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\theatre\thr2e13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\theatre\thr2e14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\theatre\thr2e15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\theatre\thr2e16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\theatre\thr2e17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\theatre\thr2e18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\theatre\thr2e19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\theatre\thr2e20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\theatre\thr2e21.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\theatre\thr2f1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\theatre\thr2f2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\theatre\thr2f3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\theatre\thr2f4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\theatre\thr2f5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\theatre\thr2f6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\theatre\thr2f7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\theatre\thr2f8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\theatre\thr2f9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\theatre\thr2f10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\theatre\thr2f11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\theatre\thr2f12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\theatre\thr2f13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\theatre\thr2f14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\theatre\thr2f15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\theatre\thr2f16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\theatre\thr2f17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d3 ) PACKEDFILE "building\pbm\theatre\thr2f18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d4 ) PACKEDFILE "building\pbm\theatre\thr2f19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\theatre\thr2f20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\theatre\thr2f21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\theatre\thr2f22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\theatre\thr2f23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\theatre\thr2f24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\theatre\thr2f25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\theatre\thr2f26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\theatre\thr2f27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\theatre\thr2f28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\theatre\thr2f29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\theatre\thr2f30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\theatre\thr2f31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\theatre\thr2f32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\theatre\thr2f33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\theatre\thr2f34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e4 ) PACKEDFILE "building\pbm\theatre\thr2f35.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f4 ) PACKEDFILE "building\pbm\theatre\thrcy1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f5 ) PACKEDFILE "building\pbm\theatre\thrcy2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f6 ) PACKEDFILE "building\pbm\theatre\thrcy3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f7 ) PACKEDFILE "building\pbm\theatre\thrcy4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f8 ) PACKEDFILE "building\pbm\theatre\thrcy5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f9 ) PACKEDFILE "building\pbm\theatre\thrcy6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fa ) PACKEDFILE "building\pbm\theatre\thrcy7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fb ) PACKEDFILE "building\pbm\theatre\thrcy8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fc ) PACKEDFILE "building\pbm\theatre\thrcy9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fd ) PACKEDFILE "building\pbm\theatre\thrcy10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fe ) PACKEDFILE "building\pbm\theatre\thrcy11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ff ) PACKEDFILE "building\pbm\theatre\thrcy12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000100 ) PACKEDFILE "building\pbm\theatre\thrcy13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000101 ) PACKEDFILE "building\pbm\theatre\thrcy14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000102 ) PACKEDFILE "building\pbm\theatre\thrcy15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000103 ) PACKEDFILE "building\pbm\theatre\thrcy16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000104 ) PACKEDFILE "building\pbm\theatre\thrcy17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000105 ) PACKEDFILE "building\pbm\theatre\thrcy18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000106 ) PACKEDFILE "building\pbm\theatre\thrcy19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000107 ) PACKEDFILE "building\pbm\theatre\thrcy20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000108 ) PACKEDFILE "building\pbm\theatre\thrcy21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000109 ) PACKEDFILE "building\pbm\theatre\thrcy22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010a ) PACKEDFILE "building\pbm\theatre\thrcy23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010b ) PACKEDFILE "building\pbm\theatre\thrcy24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010c ) PACKEDFILE "building\pbm\theatre\thrcy25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010d ) PACKEDFILE "building\pbm\theatre\thrcy26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010e ) PACKEDFILE "building\pbm\theatre\thrcy27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010f ) PACKEDFILE "building\pbm\theatre\thrcy28.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013a ) PACKEDFILE "building\pbm\theatre\thr1_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013b ) PACKEDFILE "building\pbm\theatre\thr1_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013c ) PACKEDFILE "building\pbm\theatre\thr1_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013d ) PACKEDFILE "building\pbm\theatre\thr1_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013e ) PACKEDFILE "building\pbm\theatre\thr1_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013f ) PACKEDFILE "building\pbm\theatre\thr1_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000140 ) PACKEDFILE "building\pbm\theatre\thr1_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000141 ) PACKEDFILE "building\pbm\theatre\thr1_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000142 ) PACKEDFILE "building\pbm\theatre\thr1_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000143 ) PACKEDFILE "building\pbm\theatre\thr1_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000144 ) PACKEDFILE "building\pbm\theatre\thr1_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000145 ) PACKEDFILE "building\pbm\theatre\thr1_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000146 ) PACKEDFILE "building\pbm\theatre\thr1_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000147 ) PACKEDFILE "building\pbm\theatre\thr1_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000148 ) PACKEDFILE "building\pbm\theatre\thr1_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000149 ) PACKEDFILE "building\pbm\theatre\thr1_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014a ) PACKEDFILE "building\pbm\theatre\thr1_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014b ) PACKEDFILE "building\pbm\theatre\thr1_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014c ) PACKEDFILE "building\pbm\theatre\thr1_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014d ) PACKEDFILE "building\pbm\theatre\thr1_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014e ) PACKEDFILE "building\pbm\theatre\thr1_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014f ) PACKEDFILE "building\pbm\theatre\thr1_22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000150 ) PACKEDFILE "building\pbm\theatre\thr1_23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000151 ) PACKEDFILE "building\pbm\theatre\thr1_24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000152 ) PACKEDFILE "building\pbm\theatre\thr1_25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000153 ) PACKEDFILE "building\pbm\theatre\thr1_26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000154 ) PACKEDFILE "building\pbm\theatre\thr1_27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000155 ) PACKEDFILE "building\pbm\theatre\thr1_28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000156 ) PACKEDFILE "building\pbm\theatre\thr1_29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000157 ) PACKEDFILE "building\pbm\theatre\thr1_30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000158 ) PACKEDFILE "building\pbm\theatre\thr1_31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000159 ) PACKEDFILE "building\pbm\theatre\thr1_32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015a ) PACKEDFILE "building\pbm\theatre\thr1_33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015b ) PACKEDFILE "building\pbm\theatre\thr1_34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015c ) PACKEDFILE "building\pbm\theatre\thr1_35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015d ) PACKEDFILE "building\pbm\theatre\thr1_36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015e ) PACKEDFILE "building\pbm\theatre\thr1_37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015f ) PACKEDFILE "building\pbm\theatre\thr1_38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000160 ) PACKEDFILE "building\pbm\theatre\thr1_39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000161 ) PACKEDFILE "building\pbm\theatre\thr1_40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000162 ) PACKEDFILE "building\pbm\theatre\thr1_41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000163 ) PACKEDFILE "building\pbm\theatre\thr1_42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000164 ) PACKEDFILE "building\pbm\theatre\thr1_43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000165 ) PACKEDFILE "building\pbm\theatre\thr1_44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000166 ) PACKEDFILE "building\pbm\theatre\thr1_45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000167 ) PACKEDFILE "building\pbm\theatre\thr1_46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000168 ) PACKEDFILE "building\pbm\theatre\thr1_47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000169 ) PACKEDFILE "building\pbm\theatre\thr1_48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016a ) PACKEDFILE "building\pbm\theatre\thr1_49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016b ) PACKEDFILE "building\pbm\theatre\thr1_50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016c ) PACKEDFILE "building\pbm\theatre\thr1_51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016d ) PACKEDFILE "building\pbm\theatre\thr1_52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016e ) PACKEDFILE "building\pbm\theatre\thr1_53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016f ) PACKEDFILE "building\pbm\theatre\thr1_54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000170 ) PACKEDFILE "building\pbm\theatre\thr1_55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000171 ) PACKEDFILE "building\pbm\theatre\thr1_56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000172 ) PACKEDFILE "building\pbm\theatre\thr1_57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000173 ) PACKEDFILE "building\pbm\theatre\thr1_58.pbm" ENDCHUNK

#line 89 "H:\\BUILD\\SOC\\src\\building\\theatre.cht"

 	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT	 		
		PrintStr("Theater McZee: State 1");

		
		
If (!((0x00010000->kstEntry) == 0x0004 || (0x00010000->kstEntry) == 0x0005 || (0x00010000->kstEntry) == 0x0007)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\theatre.cht", ", line ", NumToStr(95,"")); End;

		
		0x00010000->fHelpOn = (1);

		.fHelpState = 1;	
							
		.fWay = 0;		
							
							
		.MzNextAction = 0;
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		If ((0x00010000->kstEntry) == 0x0007);
			.MzNextAction = 0x0007;
			0x00010000->kstEntry = (0x0002);
		End;

		ChangeStateThis((0x00010000->kstEntry));
	ENDCHUNK

	
	
	
	
	
	
	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 2");
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		
		cnt=0x00000040;
		While( cnt <= 0x0000004c );
			Cell( cnt++, 0, 0, 6 );
		End;
		ChangeStateThis( .MzNextAction );
	ENDCHUNK 

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 3");
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );

		
		cnt=0x0000004d;
		While( cnt >= 0x00000040 );
			Cell( cnt--, 0, 0, 6 );
		End;
		If (FGobExists(0x00012300));
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002));
		Else;
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		End;

		If( .MzNextAction == 0x0014 );
			0x00010000->kstEntry = (0x0003);

			
			
			If (FGobExists(0x00012300));
				PrintStr("Enqueuing TheaterClose"); EnqueueCid(50069, 0, 0, 0, 0, 0); ChangeStateGob(0x00010c00, 0x0002); DestroyGob(0x00012300); DestroyGob(0x00010c0a); DestroyGob(0x00010c04); DestroyGob(0x00010c06); If (0 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!0 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End;
			End;
			If (1 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!1 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End; ChangeStateGob(0x00010c00, 0x0003); CreateChildGob(0x00010c00, 0x00012300, 0x00012300); CreateChildGob(0x00010c00, 0x00010c0a, 0x00010c0a); CreateChildGob(0x00010c00, 0x00010c04, 0x00010c04); CreateChildGob(0x00010c00, 0x00010c06, 0x00010c06);

		Elif( .MzNextAction == 0x0015 );
 			RunScriptGob(0x00010000, 0x0021, 0x00010f00, 0x0004);   
		Elif( .MzNextAction == 0x0016 );
			RunScriptGob(0x00010000, 0x0021, 0x00010f00, 0x0003);
		Elif( .MzNextAction == 0x0017 );
 			ChangeStateGob( 0x00012300, 0x0003 ); 
		Elif( .MzNextAction == 0x0018	);
 			ChangeStateThis( 0x000c ); 
		End;

	ENDCHUNK 

  	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 4");
		
		If (FGobExists(0x00012300));
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002));
		Else;
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		End;

		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;								
												
		Cell( 0x000000f4, 0, 0, 6 );	
		If( (0x00010000->fHelpOn) == 1 );
			If( .fHelpState == 1 );
				CreateHelpGob( 0x00010c00, 0x00018552 );
			Elif( .fHelpState == 2);
				CreateHelpGob( 0x00010c00, 0x00018553 );
			End;
		End;

		
		While( 1 );
			cnt = 0x000000f4;
			While( cnt <= 0x0000010f);
				Cell( cnt++, 0, 0, 6 );					 
			End;
		End;
	ENDCHUNK	

  	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 5");
		If( (0x00010000->fMovie) == 1 );
			0x00010000->fMovie = (0); 				
			StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
 			If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	   			

			If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018027, 0, 0x00010000, 1, 0, 998);			
			cnt=0x0000013a;						
			While( cnt <= 0x00000173 );
				Cell( cnt++, 0, 0, 6 );
			End;
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002));
			0x00010000->kstEntry = (0x0003);			
			If (1 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!1 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End; ChangeStateGob(0x00010c00, 0x0003); CreateChildGob(0x00010c00, 0x00012300, 0x00012300); CreateChildGob(0x00010c00, 0x00010c0a, 0x00010c0a); CreateChildGob(0x00010c00, 0x00010c04, 0x00010c04); CreateChildGob(0x00010c00, 0x00010c06, 0x00010c06);
		Else;
			.fHelpState=1;

			If( (0x00010000->fHelpOn)	== 1 );
				.MzNextAction = 	0x0006;	
				ChangeStateThis( 0x0002 ); 	
			Else;
				.MzNextAction = 	0x0004;	
				ChangeStateThis( 0x0002 ); 	
			End;

		End; 
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0006 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 6");
		Cell( 0x000000a4, 0, 0, 6 );

		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018108, 0, 0x00010000, 1, 0, 998);
		
		cnt=0x000000a5;
		While( cnt <= 0x000000b8 );
			Cell( cnt++, 0, 0, 6 );
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

		ChangeStateThis( 0x0004 );
	ENDCHUNK 

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0007 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 7");

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );

		
		cnt=0x0000007c;
		While( cnt <= 0x00000090 );
			Cell( cnt++, 0, 0, 6 );
		End;


		
    	EnqueueCid(50080, 0, 1, 0, 1, 0);
	
		
		FilterCmdsThis(50082, 0, 0x0021);

	ENDCHUNK 

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0008 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		.fHelpState=2;
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002)); 
							  
		PrintStr("Theater McZee: State 8");
		
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018029, 0, 0x00010000, 1, 0, 998);
		cnt=0x00000054;
		While( cnt <= 0x00000070 );
			Cell( cnt++, 0, 0, 6 );
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

 		0x00010000->fHelpOn = (1);

		ChangeStateThis( 0x0004 );
	ENDCHUNK 

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0009 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
 		.fHelpState=2;

		PrintStr("Theater McZee: State 9");
		
		
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018029, 0, 0x00010000, 1, 0, 998);
		cnt=0x00000054;
		Cell( cnt++, 0, 0, 6 );
		SetNoSlipThis( 1 );		
		While( cnt <= 0x0000006f );
			Cell( cnt++, 0, 0, 6 );
		End;
		SetNoSlipThis( 0);		
		Cell( cnt++, 0, 0, 6 );
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

		0x00010000->fHelpOn = (1);

		ChangeStateThis( 0x0004 );
	ENDCHUNK 

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x000A SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 10");
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		
 		DestroyGob( 0x00016009 );

		
		cnt=0x00000090;
		While( cnt >= 0x0000007c );
			Cell( cnt--, 0, 0, 6 );
		End;							
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;	 					
										
		If( .fWay == 1 );
			.MzNextAction = 0x0014; 		
			ChangeStateThis( 0x0003 ); 	
		Else;
			0x00010000->fHelpOn = (1);
			ChangeStateThis( 0x0004 );
		End;
	ENDCHUNK;


  	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x000b SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 11");
		

		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x0001802a, 0, 0x00010000, 1, 0, 998);
		cnt=0x000000c2;
		Cell( cnt++, 0, 0, 6 );
		SetNoSlipThis( 1 );		
		While( cnt <= 0x000000e3 );
			Cell( cnt++, 0, 0, 6 );
		End;
		SetNoSlipThis( 0 );		
		Cell( cnt++, 0, 0, 6 );

		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;

 		ChangeStateThis( 0x0004 ); 
	ENDCHUNK 

    SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x000c SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Theater McZee: State 12");
		Cell( 0x0000003f, 0, 0, 0 );
	ENDCHUNK 

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		FilterCmdsThis(50082, 0, 0xFFFFFFFF);
		.fWay = _parm[2];
		If (.fWay);
			
			ResizeGob(0x00012300, 0, 0);
		End;
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		ChangeStateThis(0x000A);	
	ENDCHUNK


	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT		
		PrintStr("Theater McZee: Click script");
		If( StateThis() != 0x0004 );	
			0x00010000->fHelpOn = (1);
			ChangeStateThis( 0x0004 );
		Elif( (0x00010000->fHelpOn) == 0 );
			0x00010000->fHelpOn = (1);
			If( .fHelpState == 1 );
				CreateHelpGob( 0x00010c00, 0x00018552 );
			Elif( .fHelpState == 2);
				CreateHelpGob( 0x00010c00, 0x00018553 );
			End;
		End;
	ENDCHUNK
		
CHUNK( 'GLOP', 0x00018552, ) SCRIPT
 	If( _parm[0] == 0 );
		
	Elif( _parm[0] == 1 );		
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		ChangeStateGob( 0x00010c03, 0x0007 ); 
	Elif( _parm[0] == 2 );		
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		ChangeStateGob( 0x00010c03, 0x000b ); 
	End;
ENDCHUNK

CHUNK( 'GLOP', 0x00018553, ) SCRIPT
 	If( _parm[0] == 0 );
		
	Elif( _parm[0] == 1 );				
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		PrintStr("Enqueuing TheaterClose"); EnqueueCid(50069, 0, 0, 0, 0, 0); ChangeStateGob(0x00010c00, 0x0002); DestroyGob(0x00012300); DestroyGob(0x00010c0a); DestroyGob(0x00010c04); DestroyGob(0x00010c06); If (0 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!0 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End;
		0x00010c03->MzNextAction = 0x0015;
 		ChangeStateGob( 0x00010c03, 0x0003 );

	Elif( _parm[0] == 2 );				
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		PrintStr("Enqueuing PortfolioClear");
		EnqueueCid(50091, 0, 0,0,0,0);
		PrintStr("Enqueuing TheaterClose"); EnqueueCid(50069, 0, 0, 0, 0, 0); ChangeStateGob(0x00010c00, 0x0002); DestroyGob(0x00012300); DestroyGob(0x00010c0a); DestroyGob(0x00010c04); DestroyGob(0x00010c06); If (0 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!0 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End;
		0x00010c03->MzNextAction = 0x0016;
 		ChangeStateGob( 0x00010c03, 0x0003);

	Elif( _parm[0] == 3 );				
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		
		PrintStr("Enqueuing TheaterRewind");
		EnqueueCid(50068, 0, 0, 0, 0, 0);
		0x00010c03->MzNextAction = 0x0017;
  		ChangeStateGob( 0x00010c03, 0x0003 );
		
	Elif (_parm[0] == 4);				
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		ChangeStateGob( 0x00010c03, 0x0007 );	
	Elif( _parm[0] == 5 );				
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016008 );
		PrintStr("Enqueuing PortfolioClear");
		EnqueueCid(50091, 0, 0,0,0,0);
		PrintStr("Enqueuing TheaterClose"); EnqueueCid(50069, 0, 0, 0, 0, 0); ChangeStateGob(0x00010c00, 0x0002); DestroyGob(0x00012300); DestroyGob(0x00010c0a); DestroyGob(0x00010c04); DestroyGob(0x00010c06); If (0 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!0 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End;
		0x00010c03->fHelpState=1;
		ChangeStateGob( 0x00010c03, 0x000b );
	End;
ENDCHUNK

SET _curgok = 0x00010c01 CHUNK( 'GOKD', 0x00010c01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 450 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\leftdr.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 		RunScriptGob(0x00010000, 0x0021, 0x00010e00, 0x0002);
	ENDCHUNK

SET _curgok = 0x00010c02 CHUNK( 'GOKD', 0x00010c02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 450 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\rightdr.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 		RunScriptGob(0x00010000, 0x0021, 0x00010f00, 0x0002);
	ENDCHUNK






SET _curgok = 0x00012300 CHUNK( 'GOKD', 0x00012300,  )  SHORT BO OSK LONG 0 LONG 0 0 0 400 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	
SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, 0x00000043 ) LONG BO OSK 48 85 592 391 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		PrintStr("Movie Work space: Entry state ", NumToStr((0x00010000->kstEntry), ""));
		If (!((0x00010000->kstEntry) == 0x0002 || (0x00010000->kstEntry) == 0x0003)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\theatre.cht", ", line ", NumToStr(460,"")); End;

		Cell( 0x00000043, 0, 0, 0 );

		
		FilterCmdsThis(50100, 0, 0x0022);
		FilterCmdsThis(50101, 0, 0x0022);

		
		
		PrintStr("Enqueuing TheaterOpen");
		EnqueueCid(50064, 0, 0x00012300, 0, 0, 0);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Movie Work space: State 2");
		Cell( 0x00000043, 0, 0, 0 ); 

		If( StateGob( 0x00010c03 ) != 0x000c );  	
			0x00010c03->MzNextAction = 0x0018;	
			ChangeStateGob( 0x00010c03, 0x0003 ); 	
		End;
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Movie Work space: State 3");
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );
		Cell( 0x00000043, 0, 0, 0 );
		ChangeStateGob( 0x00010c03, 0x000c );  	
		ChangeStateGob( 0x00010c04, 0x0002 );			
		ChangeStateGob(0x00010c06, 0x0002);
		If( .f1stMovie == 1 );
			.f1stMovie = 0;
			If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;						
			
			If (HeightGob(0x00012300) == 0);
				ResizeGob(0x00012300, 592-48, 391-85);
			End;
			CreateChildThis( 0x00010c08, 0x00010c08 );	
			StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
			While( PlayingGob( 0x00010c08 ));
				Cell( 0x0000003f, 0,0, 6 );				
			End;
			DestroyGob( 0x00010c08 );
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002));		 
		End;
		ChangeStateThis( 0x0004 );	
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Movie Work space: State 4");
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );
		ChangeStateGob( 0x00010c04, 0x0002 );			
		ChangeStateGob(0x00010c06, 0x0002);

		If( StateGob( 0x00010c03 ) != 0x000c );  	
			0x00010c03->MzNextAction = 0x0018;	
			ChangeStateGob( 0x00010c03, 0x0003 ); 	
			While( StateGob( 0x00010c03 ) != 0x000c );
				Cell( 0, 0, 0, 6 );						
			End;
		Else;
			EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000002));		 
		End;
 		Cell( 0x00000043, 0, 0, 0 );
		
		
		
		PrintStr("Enqueuing TheaterPlay");
		EnqueueCid(50066, 0, 0, 0, 0, 0);

		
		FilterCmdsThis(44011, 0, 0x0021);

 	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Movie Work space: State 5");
		Cell( 0x00000043, 0, 0, 0 );
		ChangeStateGob(0x00010c04, 0x0001);			
		ChangeStateGob(0x00010c06, 0x0001);			
		0x00010c03->MzNextAction = 0x0009;
		0x00010c03->fHelpState = 2;
		ChangeStateGob( 0x00010c03, 0x0002 );
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0006 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Movie Work space: State 6");
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
 		Cell( 0x00000043, 0, 0, 0 );
		
		
		
		
		PrintStr("kgobMovie kst6: Enqueuing TheaterStop");
		EnqueueCid(50067, 0, 0, 0, 0, 0);
		0x00010c03->MzNextAction = 0x0008;
		0x00010c03->fHelpState = 2;
		ChangeStateGob( 0x00010c03, 0x0002 );
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		If (!_parm[2] && StateGob(0x00010c04) == 0x0002);
			FilterCmdsThis(44011, 0, 0xFFFFFFFF);
			ChangeStateThis(0x0005);
		End;
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0022 ) SCRIPT
		If (_parm[2] == 0);	
			
			
			PrintStr("Warning: cidTheaterLoad or cidTheaterOpen failed!");
			PrintStr("Enqueuing PortfolioClear");
			EnqueueCid(50091, 0, 0,0,0,0);
			PrintStr("Enqueuing TheaterClose"); EnqueueCid(50069, 0, 0, 0, 0, 0); ChangeStateGob(0x00010c00, 0x0002); DestroyGob(0x00012300); DestroyGob(0x00010c0a); DestroyGob(0x00010c04); DestroyGob(0x00010c06); If (0 && !FGobExists(0x00010c09)); CreateChildGob(0x00010c00, 0x00010c09, 0x00010c09); 0x00010000->keys = ((0x00000002)); If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;; Elif (!0 && FGobExists(0x00010c09)); DestroyGob(0x00010c09); 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020)); If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End; End;
			0x00010c03->fHelpState=1;
			0x00010000->fHelpOn = (1);
			ChangeStateGob(0x00010c03, 0x0004);
		Elif (_parm[1] == 50100);
			PrintStr("Enqueuing TheaterLoad");
			EnqueueCid(50065, 0, 0, 0, 0, 0);
		Else;
			.f1stMovie=1;	
			ChangeStateThis((0x00010000->kstEntry));
		End;
	ENDCHUNK

SET _curgok = 0x00010c08 CHUNK( 'GOKD', 0x00010c08,  )  SHORT BO OSK LONG 0 LONG 0 0 1 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "CNTDWN5.AVI" ENDCHUNK







SET _curgok = 0x00010c09 CHUNK( 'GOKD', 0x00010c09,  )  SHORT BO OSK LONG 0 LONG 0 0 0 450 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK





SET _curgok = 0x00010c07 CHUNK( 'GOKD', 0x00010c07,  )  SHORT BO OSK LONG 3 LONG 0 0 0 451 LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\theatre\thumb.pbm" ENDCHUNK



SET _curgok = 0x00010c0a CHUNK( 'GOKD', 0x00010c0a,  )  SHORT BO OSK LONG 0 LONG 0 9 110 451 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 12 74 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		CreateChildThis( 0x00010c07, 0x00010c07  );
		
		y = 62  -(( GetMasterVolume() * 62 )/ (2*0x00010000));
		MoveAbsGob( 0x00010c07, 0, y );
	ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0003 _st ) SCRIPT	
		
		.siiLoop = PlaySoundThis('WAVE', 0x00010004, 2, 0x00010000, 30000, 1,
				998);	
		While( 1 );
			y = YMouseThis();
			If( y < 0 );
				y=0;
			Elif	( y > 62 );
				y =62;
			End;	
		
			
			MoveAbsGob( 0x00010c07, 0, y );
		
			
			newvol = ( (2*0x00010000)* (62-y) ) / 62;	
	 	
			
			SetMasterVolume( newvol );	
	 	
			
			Cell(  0 , 0,0, 10 );	
		End;
	ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x001D _st ) SCRIPT
		StopSound( .siiLoop );
	ENDCHUNK
	ADOPT( 'GOKD', _curgok, 'GLSC', _dfanm, SET _st = 0x10000 SET _st |= 0x0018 _st )

	

SET _curgok = 0x00010c04 CHUNK( 'GOKD', 0x00010c04,  )  SHORT BO OSK LONG 0 LONG 0 318 7 451 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\theatre\play1.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\theatre\play2.pbm" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		If( StateThis() == 0x0001 ); 
			
			If( FGobExists( 0x00016008 ) );
				DestroyGob( 0x00016008 );   
			Elif( FGobExists( 0x00016009 ) );
				DestroyGob( 0x00016009 );
			End; 
		
			
			
			If( 0x00012300->f1stMovie == 1 );
				ChangeStateGob( 0x00012300, 0x0003 );	
			Else;
				ChangeStateGob( 0x00012300, 0x0004 );
			End;
			ChangeStateThis( 0x0002 );
			ChangeStateGob(0x00010c06, 0x0002);
		
		Else;
			
			
			ChangeStateGob( 0x00012300, 0x0006 );
			ChangeStateThis( 0x0001 );
			ChangeStateGob(0x00010c06, 0x0001);
		End;		
	ENDCHUNK		

SET _curgok = 0x00010c06 CHUNK( 'GOKD', 0x00010c06, )  SHORT BO OSK LONG 0
	LONG 0 293 7 451
	LONG 0 0 0x00000002 0x00000001 0x0021 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\theatre\rewind1.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\theatre\rewind2.pbm" ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell (0x00000040, 0, 0, 0);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell (0x00000040, 0, 0, 0);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0003 _st ) SCRIPT
		Cell (0x00000041, 0, 0, 0);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0002 _st ) SCRIPT
		Cell (0x00000040, 0, 0, 0);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		Cell (0x00000040, 0, 0, 0);
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		If (!(StateGob(0x00010c04) == 0x0001)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\theatre.cht", ", line ", NumToStr(713,"")); End;
		If( FGobExists( 0x00016009 ) );
			DestroyGob( 0x00016009 );
		End;
		If (FGobExists(0x00016008));
			DestroyGob(0x00016008);
		End;
		
		
		
		ChangeStateGob(0x00012300, 0x0002);
		PrintStr("Enqueuing TheaterRewind");
		EnqueueCid(50068, 0, 0, 0, 0, 0);
		
		ChangeStateGob( 0x00012300, 0x0004 );
		
	ENDCHUNK


SET _curgok = 0x00010d01 CHUNK( 'GOKD', 0x00010d01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 50 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the2door.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 		RunScriptGob(0x00010000, 0x0021, 0x00010700, 0x0002);
	ENDCHUNK

SET _curgok = 0x00010d03 CHUNK( 'GOKD', 0x00010d03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 50 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the2map.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK

SET _curgok = 0x00010d02 CHUNK( 'GOKD', 0x00010d02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 50 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 420 194 445 206 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK



SET _curgok = 0x00010e01 CHUNK( 'GOKD', 0x00010e01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000006 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\right.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 		RunScriptGob(0x00010000, 0x0021, 0x00010c00, 0x0004); 
	ENDCHUNK

SET _curgok = 0x00010e02 CHUNK( 'GOKD', 0x00010e02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\bkleft.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x0002);
	ENDCHUNK
	
SET _curgok = 0x00010e04 CHUNK( 'GOKD', 0x00010e04,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the3map.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK

SET _curgok = 0x00010e03 CHUNK( 'GOKD', 0x00010e03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the3quit.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK


SET _curgok = 0x00010f05 CHUNK( 'GOKD', 0x00010f05,  )  SHORT BO OSK LONG 0 LONG 0 0 0 550 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
    SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Create theatre4 McZee");

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018107, 0, 0x00010000, 1, 0, 998);
		cnt = 0x00000040;
		While( cnt <= 0x0000006b );
			If( cnt == 0x00000066 );
				CreateChildGob( 0x00010f00, 0x00010f06, 0x00010f06 );
			End;
			Cell( cnt, 0, 0, 6 );
			cnt++;
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		DestroyThis();
		RunScriptGob( 0x00010f00, 0x0021 );
	ENDCHUNK
    #line 1 "H:\\BUILD\\SOC\\src\\building\\mzthea4.seq"

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\theatre\thr4_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\theatre\thr4_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\theatre\thr4_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\theatre\thr4_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\theatre\thr4_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\theatre\thr4_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\theatre\thr4_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\theatre\thr4_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\theatre\thr4_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\theatre\thr4_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\theatre\thr4_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\theatre\thr4_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\theatre\thr4_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\theatre\thr4_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\theatre\thr4_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\theatre\thr4_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\theatre\thr4_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\theatre\thr4_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\theatre\thr4_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\theatre\thr4_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\theatre\thr4_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\theatre\thr4_22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\theatre\thr4_23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\theatre\thr4_24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\theatre\thr4_25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\theatre\thr4_26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\theatre\thr4_27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\theatre\thr4_28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\theatre\thr4_29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\theatre\thr4_30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\theatre\thr4_31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\theatre\thr4_32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\theatre\thr4_33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\theatre\thr4_34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\theatre\thr4_35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\theatre\thr4_36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\theatre\thr4_37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\theatre\thr4_38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\theatre\thr4_39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\theatre\thr4_40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\theatre\thr4_41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\theatre\thr4_42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\theatre\thr4_43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\theatre\thr4_44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\theatre\thr4_45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\theatre\thr4_46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\theatre\thr4_47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\theatre\thr4_48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\theatre\thr4_49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\theatre\thr4_50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\theatre\thr4_51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\theatre\thr4_52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\theatre\thr4_53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\theatre\thr4_54.pbm" ENDCHUNK
#line 796 "H:\\BUILD\\SOC\\src\\building\\theatre.cht"

SET _curgok = 0x00010f06 CHUNK( 'GOKD', 0x00010f06,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0x0001001c, 0 );
		cnt=0x00000040;
		While( cnt <= 0x00000044 );
			Cell( cnt++, 0,0, 6 );
		End;
	ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\theatre\Ldoor49.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\theatre\Ldoor50.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\theatre\Ldoor51.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\theatre\Ldoor52.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\theatre\Ldoor53.pbm" ENDCHUNK
		
SET _curgok = 0x00010f01 CHUNK( 'GOKD', 0x00010f01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000005 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\left.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
 		RunScriptGob(0x00010000, 0x0021, 0x00010c00, 0x0004);   
	ENDCHUNK

SET _curgok = 0x00010f02 CHUNK( 'GOKD', 0x00010f02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 10 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\bkright.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x0002);
	ENDCHUNK
	
SET _curgok = 0x00010f04 CHUNK( 'GOKD', 0x00010f04,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the4map.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK

SET _curgok = 0x00010f03 CHUNK( 'GOKD', 0x00010f03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the4quit.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK




SET _curgok = 0x00010d04 CHUNK( 'GOKD', 0x00010d04,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the2m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((31 > 31) ? ( ::fBio2 & (1<<(31&31)) ) : ( ::fBio1 & (1<<(31&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (31 & 31); If (31 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 31; ::fBioCtrl = 0; CreateChildGob( 0x00010d00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00010d05 CHUNK( 'GOKD', 0x00010d05,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the2m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((32 > 31) ? ( ::fBio2 & (1<<(32&31)) ) : ( ::fBio1 & (1<<(32&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (32 & 31); If (32 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 32; ::fBioCtrl = 0; CreateChildGob( 0x00010d00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00010d06 CHUNK( 'GOKD', 0x00010d06,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\theatre\the2m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((33 > 31) ? ( ::fBio2 & (1<<(33&31)) ) : ( ::fBio1 & (1<<(33&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (33 & 31); If (33 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 33; ::fBioCtrl = 0; CreateChildGob( 0x00010d00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK





#line 67 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"
CHUNK( 'WAVE', 0x0001801a, ) FILE "building\sound\backstag\GOIDA.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001801b, ) FILE "building\sound\backstag\GOSTU.wav" ENDCHUNK
CHUNK( 'WAVE', 0x0001801c, ) FILE "building\sound\backstag\GOPRJ.wav" ENDCHUNK

SET _curgok = 0x0001100a CHUNK( 'GOKD', 0x0001100a,  )  SHORT BO OSK LONG 0 LONG 0 0 0 200 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK

	
	
	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		
If (!(((0x00010000->kstEntry) >= 0x0002 && (0x00010000->kstEntry) <= 0x0006) || (0x00010000->kstEntry) == 10)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\backstag.cht", ", line ", NumToStr(14,"")); End;

		
		
		
		
		
		.f1sthelp=0;
		.fLipsink = 0;

		
		
		If ((0x00010000->kstEntry) == 0x0006);
			.nxtPlcState = 0x0002;
		End;

		ChangeStateThis((0x00010000->kstEntry));

	ENDCHUNK

	
	
	
	
	
	 
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0x00010011, 0 );

		If( (0x00010000->fBackstagePS) );
			0x00010000->keys = ((0x00000002));
			0x00010000->fBackstagePS = (0);
			If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			
			CreateChildGob( 0x00011000, 0x00011009, 0x00011009 );
			While( PlayingGob( 0x00011009 ));
				Cell( 0x0000003f, 0,0, 6 );	
			End;
			DestroyGob( 0x00011009 );
			0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		End;

		If( !FGobExists( 0x00011005 ) );
			CreateChildGob( 0x00011000, 0x00011005, 0x00011005 );		
		End;

		Cell( 0x000000a4, 0, 0, 0 );	
									
									

		
		If( (0x00010000->fHelpOn) );
			CreateHelpGob( 0x00011000, 0x00018555 );
		Elif( (0x00010000->fBkstg1stHelp) ); 
			0x00010000->fBkstg1stHelp = (0);
 			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00011000, 0x00018555 );
		End;

		While( 1 );
			If( .fLipsink );
			   	.fLipsink = 0;
				cnt = 0x00000234;
				If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018025, 0, 0x00010000, 1, 0, 998);
				While( cnt < 0x00000247 );
					Cell( cnt++, 0, 0, 6);                                  
				End;
			End;

			cnt = 0x00000040;
			While( cnt <= 0x0000004b );
				Cell( cnt, 0,0, 6 );
				cnt++;
			End;

			rnd=Rnd(10);
			If( rnd == 0 );
			  	cnt = 0x00000054;
				While( cnt <= 0x0000005f );
					Cell( cnt, 0,0, 6 );
					cnt++;
				End;
			Elif( rnd == 1 );
			  	cnt = 0x0000005f;
				While( cnt >= 0x00000054 );
					Cell( cnt, 0,0, 6 );
					cnt--;
				End;
			Elif( rnd == 2 );
			  	
				Cell( 0x00000054, 0,0, 11 );
				Cell( 0x00000055, 0,0, 10 );
				Cell( 0x00000056, 0,0, 9 );
				Cell( 0x00000057, 0,0, 8 );
				Cell( 0x00000058, 0,0, 7 );
				Cell( 0x00000059, 0,0, 6 );

				Cell( 0x0000005a, 0,0, 4 );
				Cell( 0x0000005b, 0,0, 4 );
				Cell( 0x0000005c, 0,0, 4 );
				Cell( 0x0000005d, 0,0, 4 );
				Cell( 0x00000056, 0,0, 4 );
				Cell( 0x00000057, 0,0, 4 );
				Cell( 0x00000058, 0,0, 4 );
				Cell( 0x00000059, 0,0, 4 );

				Cell( 0x0000005a, 0,0, 6 );
				Cell( 0x0000005b, 0,0, 7 );
				Cell( 0x0000005c, 0,0, 8 );
				Cell( 0x0000005d, 0,0, 9 );
				Cell( 0x0000005e, 0,0, 10 );
				Cell( 0x0000005f, 0,0, 11 );
			End;
		End;
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0x0001001a, 0 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			
		CreateChildGob( 0x00011000, 0x0001100d, 0x0001100d );	
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		While( PlayingGob( 0x0001100d ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x0001100d );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011800, 0x0002);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0x00010015, 0 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			
		CreateChildGob( 0x00011000, 0x0001100c, 0x0001100c );	
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		While( PlayingGob( 0x0001100c ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x0001100c );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011400, 0x0002);
	ENDCHUNK
		
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0x00010018, 0 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			
		CreateChildGob( 0x00011000, 0x0001100b, 0x0001100b );	
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		While( PlayingGob( 0x0001100b ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x0001100b );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011c00, 0x0001);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0006 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell( 0x00000054, 0,0, 1 );		
		If( !FGobExists( 0x00011005 ) );
			CreateChildGob( 0x00011000, 0x00011005, 0x00011005 );		
		End;
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			

		
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016007 );   

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		RunScriptGob( 0x00010000, 0x0024, 0x0001001a, 0 );
	
		CreateChildGob( 0x00011000, 0x00011010, 0x00011010 );	
		While( PlayingGob( 0x00011010 ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x00011010 );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011800, .nxtPlcState);	
	ENDCHUNK										

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0008 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016007 );   

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		RunScriptGob( 0x00010000, 0x0024, 0x00010015, 0 );
		CreateChildGob( 0x00011000, 0x0001100e, 0x0001100e );	
		While( PlayingGob( 0x0001100e ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x0001100e );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011400, 0x0002);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0009 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		
		0x00010000->fHelpOn = (0);
		DestroyGob( 0x00016007 );   

		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		RunScriptGob( 0x00010000, 0x0024, 0x00010018, 0 );
		CreateChildGob( 0x00011000, 0x0001100f, 0x0001100f );	
		While( PlayingGob( 0x0001100f ));
			Cell( 0x0000003f, 0,0, 6 );	
		End;
		DestroyGob( 0x0001100f );			 
		RunScriptGob(0x00010000, 0x0021, 0x00011c00, 0x0001);
	ENDCHUNK

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x000A SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell( 0x000001d0, 0,0, 6 );		
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;			
		RunScriptGob( 0x00010000, 0x0024, 0x00010011, 0 );
		cnt=0x000001d1;
		While( cnt <= 0x000001e4 );
			Cell( cnt++, 0,0, 6 );	
		End;

		
		
		
		
		
		If( !(0x00010000->fBackstagePS) );
 			cnt=0x00000202;
			CreateChildGob( 0x00011000, 0x00011005, 0x00011005 );		
			ChangeStateGob( 0x00011005, 0x0002 );
			CreateChildGob( 0x00011000, 0x00011008, 0x00011008 );		
			While( cnt <= 0x00000223 );
				Cell( cnt++, 0,0, 8 );	
			End;
		End;

		ChangeStateThis( 0x0002 );
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		If( (0x00010000->fHelpOn) );
			0x00010000->fHelpOn = (0);
			DestroyGob( 0x00016007 );  	
		Else;
			0x00010000->fHelpOn = (1);
			CreateHelpGob( 0x00011000, 0x00018555 );
		End;
	ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, 0x0000003f ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\mzbkstg.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\lpcell1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\lpcell2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\lpcell3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\lpcell4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\lpcell5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\backstag\lpcell6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\lpcell7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\lpcell8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\lpcell9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\lpcell10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\lpcell11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\lpcell12.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\backstag\evcell1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\backstag\evcell2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\backstag\evcell3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\backstag\evcell4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\backstag\evcell5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\backstag\evcell6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\backstag\evcell7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\backstag\evcell8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\backstag\evcell9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\backstag\evcell10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\backstag\evcell11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\backstag\evcell12.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\backstag\idmz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\backstag\idmz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\backstag\idmz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\backstag\idmz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\backstag\idmz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\backstag\idmz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\backstag\idmz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\backstag\idmz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\backstag\idmz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\backstag\idmz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\backstag\idmz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\backstag\idmz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\backstag\idmz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\backstag\idmz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\backstag\idmz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\backstag\idmz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\backstag\idmz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\backstag\idmz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\backstag\idmz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\backstag\idmz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\backstag\idmz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\backstag\idmz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\backstag\idmz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\backstag\idmz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\backstag\idmz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\backstag\idmz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\backstag\idmz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\backstag\idmz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\backstag\idmz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\backstag\idmz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\backstag\idmz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\backstag\idmz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\backstag\idmz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\backstag\idmz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\backstag\idmz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\backstag\idmz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\backstag\idmz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\backstag\idmz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\backstag\idmz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\backstag\idmz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\backstag\idmz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\backstag\idmz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\backstag\idmz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\backstag\idmz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\backstag\idmz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\backstag\idmz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\backstag\idmz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d3 ) PACKEDFILE "building\pbm\backstag\idmz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d4 ) PACKEDFILE "building\pbm\backstag\idmz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\backstag\idmz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\backstag\idmz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\backstag\idmz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\backstag\idmz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\backstag\idmz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\backstag\idmz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\backstag\idmz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\backstag\idmz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\backstag\idmz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\backstag\idmz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\backstag\idmz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\backstag\idmz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\backstag\idmz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\backstag\idmz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\backstag\idmz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e4 ) PACKEDFILE "building\pbm\backstag\idmz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e5 ) PACKEDFILE "building\pbm\backstag\idmz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e6 ) PACKEDFILE "building\pbm\backstag\idmz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e7 ) PACKEDFILE "building\pbm\backstag\idmz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e8 ) PACKEDFILE "building\pbm\backstag\idmz69.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000108 ) PACKEDFILE "building\pbm\backstag\gostu1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000109 ) PACKEDFILE "building\pbm\backstag\gostu2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010a ) PACKEDFILE "building\pbm\backstag\gostu3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010b ) PACKEDFILE "building\pbm\backstag\gostu4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010c ) PACKEDFILE "building\pbm\backstag\gostu5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010d ) PACKEDFILE "building\pbm\backstag\gostu6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010e ) PACKEDFILE "building\pbm\backstag\gostu7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010f ) PACKEDFILE "building\pbm\backstag\gostu8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000110 ) PACKEDFILE "building\pbm\backstag\gostu9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000111 ) PACKEDFILE "building\pbm\backstag\gostu10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000112 ) PACKEDFILE "building\pbm\backstag\gostu11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000113 ) PACKEDFILE "building\pbm\backstag\gostu12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000114 ) PACKEDFILE "building\pbm\backstag\gostu13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000115 ) PACKEDFILE "building\pbm\backstag\gostu14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000116 ) PACKEDFILE "building\pbm\backstag\gostu15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000117 ) PACKEDFILE "building\pbm\backstag\gostu16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000118 ) PACKEDFILE "building\pbm\backstag\gostu17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000119 ) PACKEDFILE "building\pbm\backstag\gostu18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011a ) PACKEDFILE "building\pbm\backstag\gostu19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011b ) PACKEDFILE "building\pbm\backstag\gostu20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011c ) PACKEDFILE "building\pbm\backstag\gostu21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011d ) PACKEDFILE "building\pbm\backstag\gostu22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011e ) PACKEDFILE "building\pbm\backstag\gostu23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011f ) PACKEDFILE "building\pbm\backstag\gostu24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000120 ) PACKEDFILE "building\pbm\backstag\gostu25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000121 ) PACKEDFILE "building\pbm\backstag\gostu26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000122 ) PACKEDFILE "building\pbm\backstag\gostu27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000123 ) PACKEDFILE "building\pbm\backstag\gostu28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000124 ) PACKEDFILE "building\pbm\backstag\gostu29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000125 ) PACKEDFILE "building\pbm\backstag\gostu30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000126 ) PACKEDFILE "building\pbm\backstag\gostu31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000127 ) PACKEDFILE "building\pbm\backstag\gostu32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000128 ) PACKEDFILE "building\pbm\backstag\gostu33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000129 ) PACKEDFILE "building\pbm\backstag\gostu34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012a ) PACKEDFILE "building\pbm\backstag\gostu35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012b ) PACKEDFILE "building\pbm\backstag\gostu36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012c ) PACKEDFILE "building\pbm\backstag\gostu37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012d ) PACKEDFILE "building\pbm\backstag\gostu38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012e ) PACKEDFILE "building\pbm\backstag\gostu39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012f ) PACKEDFILE "building\pbm\backstag\gostu40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000130 ) PACKEDFILE "building\pbm\backstag\gostu41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000131 ) PACKEDFILE "building\pbm\backstag\gostu42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000132 ) PACKEDFILE "building\pbm\backstag\gostu43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000133 ) PACKEDFILE "building\pbm\backstag\gostu44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000134 ) PACKEDFILE "building\pbm\backstag\gostu45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000135 ) PACKEDFILE "building\pbm\backstag\gostu46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000136 ) PACKEDFILE "building\pbm\backstag\gostu47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000137 ) PACKEDFILE "building\pbm\backstag\gostu48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000138 ) PACKEDFILE "building\pbm\backstag\gostu49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000139 ) PACKEDFILE "building\pbm\backstag\gostu50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013a ) PACKEDFILE "building\pbm\backstag\gostu51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013b ) PACKEDFILE "building\pbm\backstag\gostu52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013c ) PACKEDFILE "building\pbm\backstag\gostu53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013d ) PACKEDFILE "building\pbm\backstag\gostu54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013e ) PACKEDFILE "building\pbm\backstag\gostu55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013f ) PACKEDFILE "building\pbm\backstag\gostu56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000140 ) PACKEDFILE "building\pbm\backstag\gostu57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000141 ) PACKEDFILE "building\pbm\backstag\gostu58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000142 ) PACKEDFILE "building\pbm\backstag\gostu59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000143 ) PACKEDFILE "building\pbm\backstag\gostu60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000144 ) PACKEDFILE "building\pbm\backstag\gostu61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000145 ) PACKEDFILE "building\pbm\backstag\gostu62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000146 ) PACKEDFILE "building\pbm\backstag\gostu63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000147 ) PACKEDFILE "building\pbm\backstag\gostu64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000148 ) PACKEDFILE "building\pbm\backstag\gostu65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000149 ) PACKEDFILE "building\pbm\backstag\gostu66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014a ) PACKEDFILE "building\pbm\backstag\gostu67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014b ) PACKEDFILE "building\pbm\backstag\gostu68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014c ) PACKEDFILE "building\pbm\backstag\gostu69.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016c ) PACKEDFILE "building\pbm\backstag\gopmz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016d ) PACKEDFILE "building\pbm\backstag\gopmz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016e ) PACKEDFILE "building\pbm\backstag\gopmz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016f ) PACKEDFILE "building\pbm\backstag\gopmz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000170 ) PACKEDFILE "building\pbm\backstag\gopmz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000171 ) PACKEDFILE "building\pbm\backstag\gopmz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000172 ) PACKEDFILE "building\pbm\backstag\gopmz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000173 ) PACKEDFILE "building\pbm\backstag\gopmz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000174 ) PACKEDFILE "building\pbm\backstag\gopmz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000175 ) PACKEDFILE "building\pbm\backstag\gopmz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000176 ) PACKEDFILE "building\pbm\backstag\gopmz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000177 ) PACKEDFILE "building\pbm\backstag\gopmz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000178 ) PACKEDFILE "building\pbm\backstag\gopmz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000179 ) PACKEDFILE "building\pbm\backstag\gopmz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017a ) PACKEDFILE "building\pbm\backstag\gopmz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017b ) PACKEDFILE "building\pbm\backstag\gopmz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017c ) PACKEDFILE "building\pbm\backstag\gopmz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017d ) PACKEDFILE "building\pbm\backstag\gopmz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017e ) PACKEDFILE "building\pbm\backstag\gopmz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017f ) PACKEDFILE "building\pbm\backstag\gopmz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000180 ) PACKEDFILE "building\pbm\backstag\gopmz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000181 ) PACKEDFILE "building\pbm\backstag\gopmz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000182 ) PACKEDFILE "building\pbm\backstag\gopmz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000183 ) PACKEDFILE "building\pbm\backstag\gopmz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000184 ) PACKEDFILE "building\pbm\backstag\gopmz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000185 ) PACKEDFILE "building\pbm\backstag\gopmz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000186 ) PACKEDFILE "building\pbm\backstag\gopmz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000187 ) PACKEDFILE "building\pbm\backstag\gopmz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000188 ) PACKEDFILE "building\pbm\backstag\gopmz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000189 ) PACKEDFILE "building\pbm\backstag\gopmz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018a ) PACKEDFILE "building\pbm\backstag\gopmz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018b ) PACKEDFILE "building\pbm\backstag\gopmz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018c ) PACKEDFILE "building\pbm\backstag\gopmz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018d ) PACKEDFILE "building\pbm\backstag\gopmz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018e ) PACKEDFILE "building\pbm\backstag\gopmz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000018f ) PACKEDFILE "building\pbm\backstag\gopmz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000190 ) PACKEDFILE "building\pbm\backstag\gopmz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000191 ) PACKEDFILE "building\pbm\backstag\gopmz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000192 ) PACKEDFILE "building\pbm\backstag\gopmz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000193 ) PACKEDFILE "building\pbm\backstag\gopmz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000194 ) PACKEDFILE "building\pbm\backstag\gopmz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000195 ) PACKEDFILE "building\pbm\backstag\gopmz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000196 ) PACKEDFILE "building\pbm\backstag\gopmz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000197 ) PACKEDFILE "building\pbm\backstag\gopmz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000198 ) PACKEDFILE "building\pbm\backstag\gopmz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000199 ) PACKEDFILE "building\pbm\backstag\gopmz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019a ) PACKEDFILE "building\pbm\backstag\gopmz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019b ) PACKEDFILE "building\pbm\backstag\gopmz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019c ) PACKEDFILE "building\pbm\backstag\gopmz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019d ) PACKEDFILE "building\pbm\backstag\gopmz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019e ) PACKEDFILE "building\pbm\backstag\gopmz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019f ) PACKEDFILE "building\pbm\backstag\gopmz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a0 ) PACKEDFILE "building\pbm\backstag\gopmz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a1 ) PACKEDFILE "building\pbm\backstag\gopmz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a2 ) PACKEDFILE "building\pbm\backstag\gopmz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a3 ) PACKEDFILE "building\pbm\backstag\gopmz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a4 ) PACKEDFILE "building\pbm\backstag\gopmz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a5 ) PACKEDFILE "building\pbm\backstag\gopmz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a6 ) PACKEDFILE "building\pbm\backstag\gopmz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a7 ) PACKEDFILE "building\pbm\backstag\gopmz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a8 ) PACKEDFILE "building\pbm\backstag\gopmz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a9 ) PACKEDFILE "building\pbm\backstag\gopmz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001aa ) PACKEDFILE "building\pbm\backstag\gopmz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ab ) PACKEDFILE "building\pbm\backstag\gopmz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ac ) PACKEDFILE "building\pbm\backstag\gopmz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ad ) PACKEDFILE "building\pbm\backstag\gopmz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ae ) PACKEDFILE "building\pbm\backstag\gopmz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001af ) PACKEDFILE "building\pbm\backstag\gopmz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b0 ) PACKEDFILE "building\pbm\backstag\gopmz69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b1 ) PACKEDFILE "building\pbm\backstag\gopmz70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b2 ) PACKEDFILE "building\pbm\backstag\gopmz71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b3 ) PACKEDFILE "building\pbm\backstag\gopmz72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b4 ) PACKEDFILE "building\pbm\backstag\gopmz73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b5 ) PACKEDFILE "building\pbm\backstag\gopmz74.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d0 ) PACKEDFILE "building\pbm\backstag\tbth5_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d1 ) PACKEDFILE "building\pbm\backstag\tbth5_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d2 ) PACKEDFILE "building\pbm\backstag\tbth5_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d3 ) PACKEDFILE "building\pbm\backstag\tbth5_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d4 ) PACKEDFILE "building\pbm\backstag\tbth5_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d5 ) PACKEDFILE "building\pbm\backstag\tbth5_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d6 ) PACKEDFILE "building\pbm\backstag\tbth5_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d7 ) PACKEDFILE "building\pbm\backstag\tbth5_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d8 ) PACKEDFILE "building\pbm\backstag\tbth5_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d9 ) PACKEDFILE "building\pbm\backstag\tbth5_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001da ) PACKEDFILE "building\pbm\backstag\tbth5_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001db ) PACKEDFILE "building\pbm\backstag\tbth5_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dc ) PACKEDFILE "building\pbm\backstag\tbth5_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dd ) PACKEDFILE "building\pbm\backstag\tbth5_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001de ) PACKEDFILE "building\pbm\backstag\tbth5_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001df ) PACKEDFILE "building\pbm\backstag\tbth5_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e0 ) PACKEDFILE "building\pbm\backstag\tbth5_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e1 ) PACKEDFILE "building\pbm\backstag\tbth5_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e2 ) PACKEDFILE "building\pbm\backstag\tbth5_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e3 ) PACKEDFILE "building\pbm\backstag\tbth5_20.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000202 ) PACKEDFILE "building\pbm\backstag\tbth6_1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000203 ) PACKEDFILE "building\pbm\backstag\tbth6_2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000204 ) PACKEDFILE "building\pbm\backstag\tbth6_3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000205 ) PACKEDFILE "building\pbm\backstag\tbth6_4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000206 ) PACKEDFILE "building\pbm\backstag\tbth6_5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000207 ) PACKEDFILE "building\pbm\backstag\tbth6_6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000208 ) PACKEDFILE "building\pbm\backstag\tbth6_7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000209 ) PACKEDFILE "building\pbm\backstag\tbth6_8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020a ) PACKEDFILE "building\pbm\backstag\tbth6_9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020b ) PACKEDFILE "building\pbm\backstag\tbth6_10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020c ) PACKEDFILE "building\pbm\backstag\tbth6_11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020d ) PACKEDFILE "building\pbm\backstag\tbth6_12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020e ) PACKEDFILE "building\pbm\backstag\tbth6_13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020f ) PACKEDFILE "building\pbm\backstag\tbth6_14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000210 ) PACKEDFILE "building\pbm\backstag\tbth6_15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000211 ) PACKEDFILE "building\pbm\backstag\tbth6_16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000212 ) PACKEDFILE "building\pbm\backstag\tbth6_17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000213 ) PACKEDFILE "building\pbm\backstag\tbth6_18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000214 ) PACKEDFILE "building\pbm\backstag\tbth6_19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000215 ) PACKEDFILE "building\pbm\backstag\tbth6_20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000216 ) PACKEDFILE "building\pbm\backstag\tbth6_21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000217 ) PACKEDFILE "building\pbm\backstag\tbth6_22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000218 ) PACKEDFILE "building\pbm\backstag\tbth6_23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000219 ) PACKEDFILE "building\pbm\backstag\tbth6_24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021a ) PACKEDFILE "building\pbm\backstag\tbth6_25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021b ) PACKEDFILE "building\pbm\backstag\tbth6_26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021c ) PACKEDFILE "building\pbm\backstag\tbth6_27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021d ) PACKEDFILE "building\pbm\backstag\tbth6_28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021e ) PACKEDFILE "building\pbm\backstag\tbth6_29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021f ) PACKEDFILE "building\pbm\backstag\tbth6_30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000220 ) PACKEDFILE "building\pbm\backstag\tbth6_31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000221 ) PACKEDFILE "building\pbm\backstag\tbth6_32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000222 ) PACKEDFILE "building\pbm\backstag\tbth6_33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000223 ) PACKEDFILE "building\pbm\backstag\tbth6_34.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000234 ) PACKEDFILE "building\pbm\backstag\speak1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000235 ) PACKEDFILE "building\pbm\backstag\speak2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000236 ) PACKEDFILE "building\pbm\backstag\speak3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000237 ) PACKEDFILE "building\pbm\backstag\speak4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000238 ) PACKEDFILE "building\pbm\backstag\speak5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000239 ) PACKEDFILE "building\pbm\backstag\speak6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023a ) PACKEDFILE "building\pbm\backstag\speak7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023b ) PACKEDFILE "building\pbm\backstag\speak8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023c ) PACKEDFILE "building\pbm\backstag\speak9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023d ) PACKEDFILE "building\pbm\backstag\speak10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023e ) PACKEDFILE "building\pbm\backstag\speak11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023f ) PACKEDFILE "building\pbm\backstag\speak12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000240 ) PACKEDFILE "building\pbm\backstag\speak13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000241 ) PACKEDFILE "building\pbm\backstag\speak14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000242 ) PACKEDFILE "building\pbm\backstag\speak15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000243 ) PACKEDFILE "building\pbm\backstag\speak16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000244 ) PACKEDFILE "building\pbm\backstag\speak17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000245 ) PACKEDFILE "building\pbm\backstag\speak18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000246 ) PACKEDFILE "building\pbm\backstag\speak19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000247 ) PACKEDFILE "building\pbm\backstag\speak20.pbm" ENDCHUNK
#line 255 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"


CHUNK( 'GLOP', 0x00018555, ) SCRIPT
 	If( _parm[0] == 0 );
		If( !0x0001100a->f1sthelp );		
			0x0001100a->fLipsink = 1;
			0x0001100a->f1sthelp = 1;
		End;
	Elif( _parm[0] == 1 );		
		DestroyGob( 0x00016007 );
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		ChangeStateGob( 0x0001100a, 0x0009 );

	Elif( _parm[0] == 2 );		
		DestroyGob( 0x00016007 );
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		ChangeStateGob( 0x0001100a, 0x0008 );

	Elif( _parm[0] == 3 );		
		DestroyGob( 0x00016007 );
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		0x0001100a->nxtPlcState	= 0x0002;
		ChangeStateGob( 0x0001100a, 0x0006 );
	End;
ENDCHUNK

SET _curgok = 0x00011006 CHUNK( 'GOKD', 0x00011006,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		cnt=0x00000040;
		While( cnt <= 0x0000004f );
			Cell( cnt++, 0,0, 6 );		
		End;
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\PrDoor.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\Pdoor54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\Pdoor55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\Pdoor56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\Pdoor57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\Pdoor58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\backstag\Pdoor59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\Pdoor60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\Pdoor61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\Pdoor62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\Pdoor63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\Pdoor64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\Pdoor65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\backstag\Pdoor66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\backstag\Pdoor67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\backstag\Pdoor68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\backstag\Pdoor69.pbm" ENDCHUNK

#line 289 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"

SET _curgok = 0x00011007 CHUNK( 'GOKD', 0x00011007,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		cnt=0x00000040;
		While( cnt <= 0x0000004f );
			Cell( cnt++, 0,0, 6 );	
		End;
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\StDoor.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\Sdoor49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\Sdoor50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\Sdoor51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\Sdoor52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\Sdoor53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\backstag\Sdoor54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\Sdoor55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\Sdoor56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\Sdoor57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\Sdoor58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\Sdoor59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\Sdoor60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\backstag\Sdoor61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\backstag\Sdoor62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\backstag\Sdoor63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\backstag\Sdoor64.pbm" ENDCHUNK

#line 298 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"

SET _curgok = 0x00011004 CHUNK( 'GOKD', 0x00011004,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		cnt=0x00000040;
		While( cnt <= 0x00000051 );
			Cell( cnt++, 0,0, 6 );	
		End;
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\IdDoor.seq"

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\door47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\door48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\door49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\door50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\door51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\door52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\door53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\door54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\door55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\door56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\door57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\door58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\backstag\door59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\backstag\door60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\backstag\door61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\backstag\door62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\backstag\door63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\backstag\door64.pbm" ENDCHUNK

#line 307 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"

 SET _curgok = 0x00011005 CHUNK( 'GOKD', 0x00011005,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\backstag\bag12.pbm" ENDCHUNK
 	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		cnt=0x00000040;
		While( cnt <= 0x00000053 );
			Cell( cnt++, 0,0, 8 );		
		End;
		ChangeStateThis( 0x0001 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\BkstgBag.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\bag1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\bag2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\bag3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\bag4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\bag5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\backstag\bag6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\bag7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\bag8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\bag9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\bag10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\bag11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\bag12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\backstag\bag13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\backstag\bag14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\backstag\bag15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\backstag\bag16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\backstag\bag17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\backstag\bag18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\backstag\bag19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\backstag\bag20.pbm" ENDCHUNK
#line 318 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"
 
 SET _curgok = 0x00011008 CHUNK( 'GOKD', 0x00011008,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
 	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		cnt=0x00000040;
		While( cnt <= 0x00000061 );
			Cell( cnt++, 0,0, 8 );		
		End;
		DestroyThis();
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\BkShdw.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\backstag\shdw1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\backstag\shdw2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\backstag\shdw3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\backstag\shdw4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\backstag\shdw5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\backstag\shdw6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\backstag\shdw7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\backstag\shdw8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\backstag\shdw9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\backstag\shdw10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\backstag\shdw11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\backstag\shdw12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\backstag\shdw13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\backstag\shdw14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\backstag\shdw15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\backstag\shdw16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\backstag\shdw17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\backstag\shdw18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\backstag\shdw19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\backstag\shdw20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\backstag\shdw21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\backstag\shdw22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\backstag\shdw23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\backstag\shdw24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\backstag\shdw25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\backstag\shdw26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\backstag\shdw27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\backstag\shdw28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\backstag\shdw29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\backstag\shdw30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\backstag\shdw31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\backstag\shdw32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\backstag\shdw33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\backstag\shdw34.pbm" ENDCHUNK

#line 328 "H:\\BUILD\\SOC\\src\\building\\backstag.cht"




SET _curgok = 0x00011009 CHUNK( 'GOKD', 0x00011009,  )  SHORT BO OSK LONG 0 LONG 0 0 0 200 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "backstag.avi" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
			DestroyThis();
	ENDCHUNK

SET _curgok = 0x0001100b CHUNK( 'GOKD', 0x0001100b,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "05LOGN4.AVI" ENDCHUNK

SET _curgok = 0x0001100c CHUNK( 'GOKD', 0x0001100c,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "LOGN5.AVI" ENDCHUNK

SET _curgok = 0x0001100d CHUNK( 'GOKD', 0x0001100d,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "LOGN06.AVI" ENDCHUNK

SET _curgok = 0x0001100e CHUNK( 'GOKD', 0x0001100e,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "GOIDA.AVI" ENDCHUNK

SET _curgok = 0x0001100f CHUNK( 'GOKD', 0x0001100f,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "GOPRJ.AVI" ENDCHUNK

SET _curgok = 0x00011010 CHUNK( 'GOKD', 0x00011010,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "GOSTU.AVI" ENDCHUNK



SET _curgok = 0x00011001 CHUNK( 'GOKD', 0x00011001,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\backstag\Bkstgdr1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		ChangeStateGob( 0x0001100a, 0x0008 );
	ENDCHUNK


SET _curgok = 0x00011002 CHUNK( 'GOKD', 0x00011002,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\backstag\Bkstgdr2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		0x0001100a->nxtPlcState	= 0x0001;
		ChangeStateGob( 0x0001100a, 0x0006 );
	ENDCHUNK


SET _curgok = 0x00011003 CHUNK( 'GOKD', 0x00011003,  )  SHORT BO OSK LONG 0 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\backstag\Bkstgdr3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		ChangeStateGob( 0x0001100a, 0x0009 );
	ENDCHUNK

#line 68 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\inspirat.cht"




















CHUNK( 'WAVE', 0x00018032, ) FILE "building\sound\inspirat\idea1a.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018033, ) FILE "building\sound\inspirat\idea1b.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018034, ) FILE "building\sound\inspirat\idea2.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018035, ) FILE "building\sound\inspirat\idea3.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018036, ) FILE "building\sound\inspirat\idea4.wav" ENDCHUNK

SET _curgok = 0x0001140d CHUNK( 'GOKD', 0x0001140d,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "idea2.avi" ENDCHUNK	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		FilterCmdsThis( 100007, 0, 0x0021);
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		StopThis();	
	ENDCHUNK		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		StopThis();	
	ENDCHUNK


SET _curgok = 0x0001140e CHUNK( 'GOKD', 0x0001140e,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "idea3.avi" ENDCHUNK	



SET _curgok = 0x0001140f CHUNK( 'GOKD', 0x0001140f,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "idea4.avi" ENDCHUNK	


SET _curgok = 0x00011401 CHUNK( 'GOKD', 0x00011401,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\mzinsp.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\inspirat\lpmz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\lpmz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\lpmz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\lpmz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\inspirat\lpmz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\inspirat\lpmz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\inspirat\lpmz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\inspirat\lpmz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\inspirat\lpmz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\inspirat\lpmz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\inspirat\lpmz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\inspirat\lpmz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\inspirat\lpmz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\inspirat\lpmz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\inspirat\lpmz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\inspirat\lpmz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\inspirat\lpmz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\inspirat\lpmz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\inspirat\lpmz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\inspirat\lpmz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\inspirat\lpmz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\inspirat\lpmz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\inspirat\lpmz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\inspirat\lpmz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\inspirat\lpmz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\inspirat\lpmz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\inspirat\lpmz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\inspirat\lpmz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\inspirat\lpmz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\inspirat\lpmz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\inspirat\lpmz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\inspirat\lpmz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\inspirat\lpmz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\inspirat\lpmz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\inspirat\lpmz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\inspirat\lpmz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\inspirat\lpmz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\inspirat\lpmz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\inspirat\lpmz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\inspirat\lpmz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\inspirat\lpmz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\inspirat\lpmz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\inspirat\lpmz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\inspirat\lpmz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\inspirat\lpmz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\inspirat\lpmz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\inspirat\lpmz47.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\inspirat\sq2mz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\inspirat\sq2mz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\inspirat\sq2mz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\inspirat\sq2mz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\inspirat\sq2mz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\inspirat\sq2mz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\inspirat\sq2mz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\inspirat\sq2mz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\inspirat\sq2mz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\inspirat\sq2mz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\inspirat\sq2mz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\inspirat\sq2mz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\inspirat\sq2mz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\inspirat\sq2mz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\inspirat\sq2mz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\inspirat\sq2mz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\inspirat\sq2mz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\inspirat\sq2mz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\inspirat\sq2mz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\inspirat\sq2mz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\inspirat\sq2mz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\inspirat\sq2mz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\inspirat\sq2mz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\inspirat\sq2mz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\inspirat\sq2mz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\inspirat\sq2mz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\inspirat\sq2mz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\inspirat\sq2mz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\inspirat\sq2mz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\inspirat\sq2mz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\inspirat\sq2mz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\inspirat\sq2mz32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\inspirat\sq2mz33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\inspirat\sq2mz34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\inspirat\sq2mz35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\inspirat\sq2mz36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\inspirat\sq2mz37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\inspirat\sq2mz38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\inspirat\sq2mz39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\inspirat\sq2mz40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\inspirat\sq2mz41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\inspirat\sq2mz42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\inspirat\sq2mz43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\inspirat\sq2mz44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\inspirat\sq2mz45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\inspirat\sq2mz46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\inspirat\sq2mz47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\inspirat\sq2mz48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d3 ) PACKEDFILE "building\pbm\inspirat\sq2mz49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d4 ) PACKEDFILE "building\pbm\inspirat\sq2mz50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\inspirat\sq2mz51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\inspirat\sq2mz52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\inspirat\sq2mz53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\inspirat\sq2mz54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\inspirat\sq2mz55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\inspirat\sq2mz56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\inspirat\sq2mz57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\inspirat\sq2mz58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\inspirat\sq2mz59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\inspirat\sq2mz60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\inspirat\sq2mz61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\inspirat\sq2mz62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\inspirat\sq2mz63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\inspirat\sq2mz64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\inspirat\sq2mz65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e4 ) PACKEDFILE "building\pbm\inspirat\sq2mz66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e5 ) PACKEDFILE "building\pbm\inspirat\sq2mz67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e6 ) PACKEDFILE "building\pbm\inspirat\sq2mz68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e7 ) PACKEDFILE "building\pbm\inspirat\sq2mz69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e8 ) PACKEDFILE "building\pbm\inspirat\sq2mz70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e9 ) PACKEDFILE "building\pbm\inspirat\sq2mz71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ea ) PACKEDFILE "building\pbm\inspirat\sq2mz72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000eb ) PACKEDFILE "building\pbm\inspirat\sq2mz73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ec ) PACKEDFILE "building\pbm\inspirat\sq2mz74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ed ) PACKEDFILE "building\pbm\inspirat\sq2mz75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ee ) PACKEDFILE "building\pbm\inspirat\sq2mz76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ef ) PACKEDFILE "building\pbm\inspirat\sq2mz77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f0 ) PACKEDFILE "building\pbm\inspirat\sq2mz78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f1 ) PACKEDFILE "building\pbm\inspirat\sq2mz79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f2 ) PACKEDFILE "building\pbm\inspirat\sq2mz80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f3 ) PACKEDFILE "building\pbm\inspirat\sq2mz81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f4 ) PACKEDFILE "building\pbm\inspirat\sq2mz82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f5 ) PACKEDFILE "building\pbm\inspirat\sq2mz83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f6 ) PACKEDFILE "building\pbm\inspirat\sq2mz84.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f7 ) PACKEDFILE "building\pbm\inspirat\sq2mz85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f8 ) PACKEDFILE "building\pbm\inspirat\sq2mz86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f9 ) PACKEDFILE "building\pbm\inspirat\sq2mz87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fa ) PACKEDFILE "building\pbm\inspirat\sq2mz88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fb ) PACKEDFILE "building\pbm\inspirat\sq2mz89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fc ) PACKEDFILE "building\pbm\inspirat\sq2mz90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fd ) PACKEDFILE "building\pbm\inspirat\sq2mz91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000fe ) PACKEDFILE "building\pbm\inspirat\sq2mz92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ff ) PACKEDFILE "building\pbm\inspirat\sq2mz93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000100 ) PACKEDFILE "building\pbm\inspirat\sq2mz94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000101 ) PACKEDFILE "building\pbm\inspirat\sq2mz95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000102 ) PACKEDFILE "building\pbm\inspirat\sq2mz96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000103 ) PACKEDFILE "building\pbm\inspirat\sq2mz97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000104 ) PACKEDFILE "building\pbm\inspirat\sq2mz98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000105 ) PACKEDFILE "building\pbm\inspirat\sq2mz99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000106 ) PACKEDFILE "building\pbm\inspirat\sq2mz100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000107 ) PACKEDFILE "building\pbm\inspirat\sq2mz101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000108 ) PACKEDFILE "building\pbm\inspirat\sq2mz102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000109 ) PACKEDFILE "building\pbm\inspirat\sq2mz103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010a ) PACKEDFILE "building\pbm\inspirat\sq2mz104.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010b ) PACKEDFILE "building\pbm\inspirat\sq2mz105.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010c ) PACKEDFILE "building\pbm\inspirat\sq2mz106.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010d ) PACKEDFILE "building\pbm\inspirat\sq2mz107.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010e ) PACKEDFILE "building\pbm\inspirat\sq2mz108.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010f ) PACKEDFILE "building\pbm\inspirat\sq2mz109.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000110 ) PACKEDFILE "building\pbm\inspirat\sq2mz110.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000111 ) PACKEDFILE "building\pbm\inspirat\sq2mz111.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000112 ) PACKEDFILE "building\pbm\inspirat\sq2mz112.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000113 ) PACKEDFILE "building\pbm\inspirat\sq2mz113.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000114 ) PACKEDFILE "building\pbm\inspirat\sq2mz114.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000115 ) PACKEDFILE "building\pbm\inspirat\sq2mz115.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000116 ) PACKEDFILE "building\pbm\inspirat\sq2mz116.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000117 ) PACKEDFILE "building\pbm\inspirat\sq2mz117.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000118 ) PACKEDFILE "building\pbm\inspirat\sq2mz118.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000119 ) PACKEDFILE "building\pbm\inspirat\sq2mz119.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011a ) PACKEDFILE "building\pbm\inspirat\sq2mz120.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011b ) PACKEDFILE "building\pbm\inspirat\sq2mz121.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011c ) PACKEDFILE "building\pbm\inspirat\sq2mz122.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011d ) PACKEDFILE "building\pbm\inspirat\sq2mz123.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011e ) PACKEDFILE "building\pbm\inspirat\sq2mz124.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011f ) PACKEDFILE "building\pbm\inspirat\sq2mz125.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000120 ) PACKEDFILE "building\pbm\inspirat\sq2mz126.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000121 ) PACKEDFILE "building\pbm\inspirat\sq2mz127.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000122 ) PACKEDFILE "building\pbm\inspirat\sq2mz128.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000123 ) PACKEDFILE "building\pbm\inspirat\sq2mz129.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000124 ) PACKEDFILE "building\pbm\inspirat\sq2mz130.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000125 ) PACKEDFILE "building\pbm\inspirat\sq2mz131.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000126 ) PACKEDFILE "building\pbm\inspirat\sq2mz132.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000127 ) PACKEDFILE "building\pbm\inspirat\sq2mz133.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000128 ) PACKEDFILE "building\pbm\inspirat\sq2mz134.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000129 ) PACKEDFILE "building\pbm\inspirat\sq2mz135.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012a ) PACKEDFILE "building\pbm\inspirat\sq2mz136.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012b ) PACKEDFILE "building\pbm\inspirat\sq2mz137.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012c ) PACKEDFILE "building\pbm\inspirat\sq2mz138.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012d ) PACKEDFILE "building\pbm\inspirat\sq2mz139.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012e ) PACKEDFILE "building\pbm\inspirat\sq2mz140.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000012f ) PACKEDFILE "building\pbm\inspirat\sq2mz141.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000130 ) PACKEDFILE "building\pbm\inspirat\sq2mz142.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000131 ) PACKEDFILE "building\pbm\inspirat\sq2mz143.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000132 ) PACKEDFILE "building\pbm\inspirat\sq2mz144.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000133 ) PACKEDFILE "building\pbm\inspirat\sq2mz145.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000134 ) PACKEDFILE "building\pbm\inspirat\sq2mz146.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000135 ) PACKEDFILE "building\pbm\inspirat\sq2mz147.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000136 ) PACKEDFILE "building\pbm\inspirat\sq2mz148.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000137 ) PACKEDFILE "building\pbm\inspirat\sq2mz149.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000138 ) PACKEDFILE "building\pbm\inspirat\sq2mz150.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000139 ) PACKEDFILE "building\pbm\inspirat\sq2mz151.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013a ) PACKEDFILE "building\pbm\inspirat\sq2mz152.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013b ) PACKEDFILE "building\pbm\inspirat\sq2mz153.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013c ) PACKEDFILE "building\pbm\inspirat\sq2mz154.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013d ) PACKEDFILE "building\pbm\inspirat\sq2mz155.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013e ) PACKEDFILE "building\pbm\inspirat\sq2mz156.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000013f ) PACKEDFILE "building\pbm\inspirat\sq2mz157.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000140 ) PACKEDFILE "building\pbm\inspirat\sq2mz158.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000141 ) PACKEDFILE "building\pbm\inspirat\sq2mz159.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000142 ) PACKEDFILE "building\pbm\inspirat\sq2mz160.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000143 ) PACKEDFILE "building\pbm\inspirat\sq2mz161.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000144 ) PACKEDFILE "building\pbm\inspirat\sq2mz162.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000145 ) PACKEDFILE "building\pbm\inspirat\sq2mz163.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000146 ) PACKEDFILE "building\pbm\inspirat\sq2mz164.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000147 ) PACKEDFILE "building\pbm\inspirat\sq2mz165.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000148 ) PACKEDFILE "building\pbm\inspirat\sq2mz166.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000149 ) PACKEDFILE "building\pbm\inspirat\sq2mz167.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014a ) PACKEDFILE "building\pbm\inspirat\sq2mz168.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014b ) PACKEDFILE "building\pbm\inspirat\sq2mz169.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014c ) PACKEDFILE "building\pbm\inspirat\sq2mz170.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014d ) PACKEDFILE "building\pbm\inspirat\sq2mz171.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014e ) PACKEDFILE "building\pbm\inspirat\sq2mz172.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000014f ) PACKEDFILE "building\pbm\inspirat\sq2mz173.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000150 ) PACKEDFILE "building\pbm\inspirat\sq2mz174.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000151 ) PACKEDFILE "building\pbm\inspirat\sq2mz175.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000152 ) PACKEDFILE "building\pbm\inspirat\sq2mz176.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000153 ) PACKEDFILE "building\pbm\inspirat\sq2mz177.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000154 ) PACKEDFILE "building\pbm\inspirat\sq2mz178.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000155 ) PACKEDFILE "building\pbm\inspirat\sq2mz179.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000156 ) PACKEDFILE "building\pbm\inspirat\sq2mz180.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000157 ) PACKEDFILE "building\pbm\inspirat\sq2mz181.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000158 ) PACKEDFILE "building\pbm\inspirat\sq2mz182.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000159 ) PACKEDFILE "building\pbm\inspirat\sq2mz183.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015a ) PACKEDFILE "building\pbm\inspirat\sq2mz184.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015b ) PACKEDFILE "building\pbm\inspirat\sq2mz185.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015c ) PACKEDFILE "building\pbm\inspirat\sq2mz186.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015d ) PACKEDFILE "building\pbm\inspirat\sq2mz187.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015e ) PACKEDFILE "building\pbm\inspirat\sq2mz188.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000015f ) PACKEDFILE "building\pbm\inspirat\sq2mz189.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000160 ) PACKEDFILE "building\pbm\inspirat\sq2mz190.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000161 ) PACKEDFILE "building\pbm\inspirat\sq2mz191.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000162 ) PACKEDFILE "building\pbm\inspirat\sq2mz192.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000163 ) PACKEDFILE "building\pbm\inspirat\sq2mz193.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000164 ) PACKEDFILE "building\pbm\inspirat\sq2mz194.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000165 ) PACKEDFILE "building\pbm\inspirat\sq2mz195.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000166 ) PACKEDFILE "building\pbm\inspirat\sq2mz196.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000167 ) PACKEDFILE "building\pbm\inspirat\sq2mz197.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000168 ) PACKEDFILE "building\pbm\inspirat\sq2mz198.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000169 ) PACKEDFILE "building\pbm\inspirat\sq2mz199.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016a ) PACKEDFILE "building\pbm\inspirat\sq2mz200.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016b ) PACKEDFILE "building\pbm\inspirat\sq2mz201.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019d ) PACKEDFILE "building\pbm\inspirat\sq3mz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019e ) PACKEDFILE "building\pbm\inspirat\sq3mz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000019f ) PACKEDFILE "building\pbm\inspirat\sq3mz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a0 ) PACKEDFILE "building\pbm\inspirat\sq3mz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a1 ) PACKEDFILE "building\pbm\inspirat\sq3mz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a2 ) PACKEDFILE "building\pbm\inspirat\sq3mz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a3 ) PACKEDFILE "building\pbm\inspirat\sq3mz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a4 ) PACKEDFILE "building\pbm\inspirat\sq3mz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a5 ) PACKEDFILE "building\pbm\inspirat\sq3mz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a6 ) PACKEDFILE "building\pbm\inspirat\sq3mz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a7 ) PACKEDFILE "building\pbm\inspirat\sq3mz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a8 ) PACKEDFILE "building\pbm\inspirat\sq3mz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001a9 ) PACKEDFILE "building\pbm\inspirat\sq3mz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001aa ) PACKEDFILE "building\pbm\inspirat\sq3mz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ab ) PACKEDFILE "building\pbm\inspirat\sq3mz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ac ) PACKEDFILE "building\pbm\inspirat\sq3mz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ad ) PACKEDFILE "building\pbm\inspirat\sq3mz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ae ) PACKEDFILE "building\pbm\inspirat\sq3mz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001af ) PACKEDFILE "building\pbm\inspirat\sq3mz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b0 ) PACKEDFILE "building\pbm\inspirat\sq3mz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b1 ) PACKEDFILE "building\pbm\inspirat\sq3mz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b2 ) PACKEDFILE "building\pbm\inspirat\sq3mz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b3 ) PACKEDFILE "building\pbm\inspirat\sq3mz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b4 ) PACKEDFILE "building\pbm\inspirat\sq3mz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b5 ) PACKEDFILE "building\pbm\inspirat\sq3mz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b6 ) PACKEDFILE "building\pbm\inspirat\sq3mz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b7 ) PACKEDFILE "building\pbm\inspirat\sq3mz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b8 ) PACKEDFILE "building\pbm\inspirat\sq3mz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001b9 ) PACKEDFILE "building\pbm\inspirat\sq3mz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ba ) PACKEDFILE "building\pbm\inspirat\sq3mz30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001bb ) PACKEDFILE "building\pbm\inspirat\sq3mz31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001bc ) PACKEDFILE "building\pbm\inspirat\sq3mz32.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001cf ) PACKEDFILE "building\pbm\inspirat\sq4mz1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d0 ) PACKEDFILE "building\pbm\inspirat\sq4mz2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d1 ) PACKEDFILE "building\pbm\inspirat\sq4mz3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d2 ) PACKEDFILE "building\pbm\inspirat\sq4mz4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d3 ) PACKEDFILE "building\pbm\inspirat\sq4mz5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d4 ) PACKEDFILE "building\pbm\inspirat\sq4mz6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d5 ) PACKEDFILE "building\pbm\inspirat\sq4mz7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d6 ) PACKEDFILE "building\pbm\inspirat\sq4mz8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d7 ) PACKEDFILE "building\pbm\inspirat\sq4mz9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d8 ) PACKEDFILE "building\pbm\inspirat\sq4mz10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001d9 ) PACKEDFILE "building\pbm\inspirat\sq4mz11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001da ) PACKEDFILE "building\pbm\inspirat\sq4mz12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001db ) PACKEDFILE "building\pbm\inspirat\sq4mz13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dc ) PACKEDFILE "building\pbm\inspirat\sq4mz14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001dd ) PACKEDFILE "building\pbm\inspirat\sq4mz15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001de ) PACKEDFILE "building\pbm\inspirat\sq4mz16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001df ) PACKEDFILE "building\pbm\inspirat\sq4mz17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e0 ) PACKEDFILE "building\pbm\inspirat\sq4mz18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e1 ) PACKEDFILE "building\pbm\inspirat\sq4mz19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e2 ) PACKEDFILE "building\pbm\inspirat\sq4mz20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e3 ) PACKEDFILE "building\pbm\inspirat\sq4mz21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e4 ) PACKEDFILE "building\pbm\inspirat\sq4mz22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e5 ) PACKEDFILE "building\pbm\inspirat\sq4mz23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e6 ) PACKEDFILE "building\pbm\inspirat\sq4mz24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e7 ) PACKEDFILE "building\pbm\inspirat\sq4mz25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e8 ) PACKEDFILE "building\pbm\inspirat\sq4mz26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001e9 ) PACKEDFILE "building\pbm\inspirat\sq4mz27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ea ) PACKEDFILE "building\pbm\inspirat\sq4mz28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001eb ) PACKEDFILE "building\pbm\inspirat\sq4mz29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000001ec ) PACKEDFILE "building\pbm\inspirat\sq4mz30.pbm" ENDCHUNK
#line 53 "H:\\BUILD\\SOC\\src\\building\\inspirat.cht"
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, 0x00000099 ) LONG BO OSK 0 0 1 1 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		If (!((0x00010000->kstEntry) == 0x0002 || (0x00010000->kstEntry) == 0x0006 || (0x00010000->kstEntry) == 0x0007 || (0x00010000->kstEntry) == 0x0008)); AlertStr("Assertion failed: ", "H:\\BUILD\\SOC\\src\\building\\inspirat.cht", ", line ", NumToStr(55,"")); End;

		If ((0x00010000->kstEntry) == 0x0006);	
			0x00010000->kstEntry = (0x0002);	
										
	    								
										
										
		Else;
			0x00010000->fHelpOn = (0);	
				  							
		End;		     					

		.fEnableAccel = 0;

		ChangeStateThis((0x00010000->kstEntry));
	ENDCHUNK

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		cnt=0x00000068;
		Cell( cnt, 0,0, 0 );

		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;		

		If( (0x00010000->fHelpOn) );	  
			CreateHelpGob( 0x00011400, 0x00018557 );
		End;

		While (1);
			While( cnt <= 0x0000006e );	
				Cell( cnt++, 0, 0, 9);  	
			End;	   				  	
									  	
			cnt = 0x0000006d;			  	
			While( cnt >= 0x00000069 );
				Cell( cnt--, 0, 0, 9);
			End;

			SetNoSlipThis( 1 );		
			rnd = Rnd( 100 );
			If( rnd < 20 );	
				If( rnd < 14 );
			 		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018032, 0, 0x00010000, 1, 0, 998);
					cnt=0x00000040;
					While( cnt < 0x0000004e );
						Cell( cnt++, 0, 0, 6 );
					End;
					If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
				End;
				If( rnd > 5 );
					cnt=0x0000004e;
					While( cnt <= 0x00000068 );
						If( cnt == 0x00000052 );
			 				If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018033, 0, 0x00010000, 1, 0, 998);
						End;
						Cell( cnt++, 0, 0, 6 );
					End;
					If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
				End;
			End;
			SetNoSlipThis( 0 );		
		End;

	ENDCHUNK

 	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		Cell( 0x000000a4, 0,0, 1 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		CreateChildGob( GidParThis(), 0x0001140d, 0x0001140d );
		Cell( 0x000000a4,0,0, 100 );	
		
		
		While( PlayingGob( 0x0001140d ));
			Cell( 0x000000a4, 0, 0, 10 );	
		End; 
		DestroyGob( 0x0001140d );
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		ChangeStateThis( 0x0002 );		
	ENDCHUNK

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT

		
		
		
		
		
		
		

		Cell( 0x00000068,0,0, 1 );

		
		While ( !FGobExists(0x00015100) && !FGobExists(0x00012205) );
			Cell( 0,0,0, 50 );
		End;

		While( FGobExists(0x00015100) || FGobExists(0x00012205) );
			
			Cell( 0,0,0, 50 );
		End;

		
		If (.fEnableAccel);
			If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
			.fEnableAccel = 0;
		End;

		
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		ChangeStateThis( 0x0002 );
	ENDCHUNK;


		
		
		
		

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0007 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		Cell( 0x0000019e, 0,0, 1 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		CreateChildGob( GidParThis(), 0x0001140e, 0x0001140e );
		Cell( 0x0000019e,0,0, 100 );	
		
		
		While( PlayingGob( 0x0001140e ));
			Cell( 0x0000019e, 0, 0, 10 );	
		End; 
		DestroyGob( 0x0001140e );
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));

		If (0x00010000->cDisable >= 1 && !0); PrintStr("Disable Accels takes disable count above 1."); End; If (0); If (0x00010000->cDisable <= 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 1; Else; If (0x00010000->cDisable == 0); EnqueueCid(50093, 0, 0, 0, 0, 0); End; 0x00010000->cDisable++; End;;
		.fEnableAccel = 1;
		
		EnqueueCid(50095, 1, 0x00011400, 0x00012205, 0, 0);
		ChangeStateThis( 0x0005 );
	ENDCHUNK;

   	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0008 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		Cell( 0x000001d0, 0,0, 1 );
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;	
		CreateChildGob( GidParThis(), 0x0001140f, 0x0001140f );
		Cell( 0x000001d0,0,0, 100 );	
		
		
		While( PlayingGob( 0x0001140f ));
			Cell( 0x000001d0, 0, 0, 10 );	
		End; 
		DestroyGob( 0x0001140f );
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		
		Transition(3, 0, 60, 0xffffffff, 0xFFFFFFFF );
		
		
		::nBioPage = 0; ::fBioCtrl = 1; CreateChildGob( 0x00011400, 0x00015100, 0x00015100 );
		ChangeStateThis( 0x0005 );
	ENDCHUNK;


	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		SetNoSlipThis( 0 );		
		If( (0x00010000->fHelpOn) == 0 );
			0x00010000->fHelpOn = (1);
			0x00010000->fIdeasPS = (0);
			ChangeStateGob( 0x00011401, 0x0003 );
		Elif( (0x00010000->fHelpOn) == 1 && StateThis() == 0x0002); 
			0x00010000->fHelpOn = (0);						  
			DestroyGob( 0x00016008 );
		Elif( (0x00010000->fHelpOn) == 1 );	
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;						
			ChangeStateThis( 0x0002 );
		End;
	ENDCHUNK

CHUNK( 'GLOP', 0x00018557, ) SCRIPT
 	If( _parm[0] == 0 );
		
	Elif( _parm[0] == 1 );		
		DestroyGob( 0x00016008 );
		ChangeStateGob( 0x00011401, 0x0007 );

	Elif( _parm[0] == 2 );		
		DestroyGob( 0x00016008 );
		ChangeStateGob( 0x00011401, 0x0008 );
	End;
ENDCHUNK

SET _curgok = 0x00011402 CHUNK( 'GOKD', 0x00011402,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\SplotMsk.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		0x00010000->fHelpOn = (0);
		If( FGobExists( 0x00016008 ) );
			DestroyGob( 0x00016008 );
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		ChangeStateGob( 0x00011401, 0x0007 );
	ENDCHUNK

SET _curgok = 0x00011403 CHUNK( 'GOKD', 0x00011403,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\TalntMsk.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		0x00010000->fHelpOn = (0);
		If( FGobExists( 0x00016008 ) );
			DestroyGob( 0x00016008 );
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		ChangeStateGob( 0x00011401, 0x0008 );
	ENDCHUNK


SET _curgok = 0x00011501 CHUNK( 'GOKD', 0x00011501,  )  SHORT BO OSK LONG 0 LONG 0 0 0 33 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\ins2door.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x0002);
	ENDCHUNK

SET _curgok = 0x00011503 CHUNK( 'GOKD', 0x00011503,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\ins2map.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
	ENDCHUNK

SET _curgok = 0x00011502 CHUNK( 'GOKD', 0x00011502,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\ins2quit.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		EnqueueCid(106, 0, 0, 0, 0, 0);;
	ENDCHUNK




SET _curgok = 0x00012205 CHUNK( 'GOKD', 0x00012205,  )  SHORT BO OSK LONG 0 LONG 0 0 0 1000 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\splot0.pbm" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );
		CreateChildThis( 0x00012206, 0x00012206 );
		CreateChildThis( 0x00012207, 0x00012207 );
		CreateChildThis( 0x00012208, 0x00012208 ); 
		CreateChildThis( 0x00012209, 0x00012209 );
		CreateChildThis( 0x0001220a, 0x0001220a );
		CreateChildThis( 0x0001220b, 0x0001220b );
		CreateChildThis( 0x0001220c, 0x0001220c );
		CreateChildThis( 0x0001220d, 0x0001220d );
		CreateHelpThis(	0x00018558 );
		EnqueueCid(50083, 0x00012205, 0x0001220d, 0, 0, 0);
		CreateChildThis( 0x0001220e, 0x0001220e );
	ENDCHUNK


SET _curgok = 0x0001220e CHUNK( 'GOKD', 0x0001220e,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000002 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 48 95 591 401 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK




SET _curgok = 0x0001220d CHUNK( 'GOKD', 0x0001220d, )  SHORT BO OSK LONG 1
	LONG 0 0 0 0
	LONG 0 0 0x00000000 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 48 95 591 401 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK





SET _curgok = 0x00012206 CHUNK( 'GOKD', 0x00012206, )  SHORT BO OSK LONG 1
	LONG 0 600 194 0
	LONG 0 0 0x00000002 0x00000001 SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\bigarm2.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\bigarm3.pbm" ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\bigarm4.pbm" ENDCHUNK

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\bigarm1.pbm" ENDCHUNK	 

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\bigarm5.pbm" ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0017 _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x001B _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x001D _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x001E _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	SET _dfwave++ CHUNK( 'WAVE', _dfwave,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x001D _st ) FILE "building\sound\inspirat\splthndl.wav" ENDCHUNK;
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		ChangeStateGob( 0x00012207, 0x0002 );			   
		ChangeStateGob( 0x00012208, 0x0002 );			   
		ChangeStateGob( 0x00012209, 0x0002 );
		ChangeStateGob( 0x0001220a, 0x0002 );
		
		ChangeStateGob( 0x0001220b, 0x0002 );			
		EnqueueCid(50084, 0x00012205, 1, 1, 1, 1);
		EnqueueCid(50085, 0x00012205, 0, 0, 0, 0);
		If( FGobExists( 0x00016019 ) );
			DestroyGob(	0x00016019 );	 			
			CreateHelpGob(	GidParThis(), 0x00018559 );	
		End;									 
	ENDCHUNK
	




SET _curgok = 0x00012207 CHUNK( 'GOKD', 0x00012207, )  SHORT BO OSK LONG 1
	LONG 0 427 2 0
	LONG 0 0 0x00000002 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000001 SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\actarm2.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\actarm3.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\actarm4.pbm" ENDCHUNK;

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\actarm1.pbm" ENDCHUNK	 	
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\actarm5.pbm" ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0017 _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001B _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001E _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	SET _dfwave++ CHUNK( 'WAVE', _dfwave,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) FILE "building\sound\inspirat\splthndl.wav" ENDCHUNK;
		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	
		EnqueueCid(50084, 0x00012205, 0, 0, 1, 0);
		EnqueueCid(50085, 0x00012205, 0, 0, 0, 0);
	ENDCHUNK
	




SET _curgok = 0x00012208 CHUNK( 'GOKD', 0x00012208, )  SHORT BO OSK LONG 1
	LONG 0 289 2 0
	LONG 0 0 0x00000002 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000001 SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
	SET _dfwave++ CHUNK( 'WAVE', _dfwave,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) FILE "building\sound\inspirat\splthndl.wav" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\cmrarm2.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\cmrarm3.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\cmrarm4.pbm" ENDCHUNK;

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\cmrarm1.pbm" ENDCHUNK	 
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\cmrarm5.pbm" ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0017 _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001B _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001E _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	

		EnqueueCid(50084, 0x00012205, 0, 1, 0, 0);
		EnqueueCid(50085, 0x00012205, 0, 0, 0, 0);
	ENDCHUNK
	




SET _curgok = 0x00012209 CHUNK( 'GOKD', 0x00012209, )  SHORT BO OSK LONG 1
	LONG 0 566 2 0
	LONG 0 0 0x00000002 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000001 SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
	SET _dfwave++ CHUNK( 'WAVE', _dfwave,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) FILE "building\sound\inspirat\splthndl.wav" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\muscarm2.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\muscarm3.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\muscarm4.pbm" ENDCHUNK;

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\muscarm1.pbm" ENDCHUNK	 	
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\muscarm5.pbm" ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0017 _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001B _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001E _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	

		EnqueueCid(50084, 0x00012205, 0, 0, 0, 1);
		EnqueueCid(50085, 0x00012205, 0, 0, 0, 0);
	ENDCHUNK 
	




SET _curgok = 0x0001220a CHUNK( 'GOKD', 0x0001220a, )  SHORT BO OSK LONG 1
	LONG 0 146 1 0
	LONG 0 0 0x00000002 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0x00000004 0x00000001 SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
	SET _dfwave++ CHUNK( 'WAVE', _dfwave,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) FILE "building\sound\inspirat\splthndl.wav" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\inspirat\scnarm2.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\inspirat\scnarm3.pbm" ENDCHUNK;
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\inspirat\scnarm4.pbm" ENDCHUNK;

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\scnarm1.pbm" ENDCHUNK	 	
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\scnarm5.pbm" ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0017 _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001B _st ) SCRIPT
		Cell( 0x00000041, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000043, 0, 0, 9 );
	ENDCHUNK	

	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001D _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x001E _st ) SCRIPT
		Cell( 0x00000043, 0, 0, 9 );	
		Cell( 0x00000042, 0, 0, 9 );	
		Cell( 0x00000041, 0, 0, 9 );
		Cell( SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 9 ); 
	ENDCHUNK	
		
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	

		EnqueueCid(50084, 0x00012205, 1, 0, 0, 0);
		EnqueueCid(50085, 0x00012205, 0, 0, 0, 0);
	ENDCHUNK
	





SET _curgok = 0x0001220b CHUNK( 'GOKD', 0x0001220b, )  SHORT BO OSK LONG 1
	LONG 0 471 426 0
	LONG 0 0 0x00000002 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
	LONG 0 0 0xffffffff 0x00000001 SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\toolboxd.pbm" ENDCHUNK	 	
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\toolbox1.pbm" ENDCHUNK	 	
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0002 _st )
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\toolbox2.pbm" ENDCHUNK	

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	
		
		EnqueueCid(50086, 0x00012205, 0, 0, 0, 0);
		
		
		If (0x00010000->cDisable <= 0 && !0); PrintStr("Enable Accels takes disable count below 0."); End; If (0); If (0x00010000->cDisable >= 1); EnqueueCid(50094, 0, 0, 0, 0, 0); End; 0x00010000->cDisable = 0; Else; 0x00010000->cDisable--; If (0x00010000->cDisable == 0); EnqueueCid(50094, 0, 0, 0, 0, 0); End; End;
		RunScriptGob(0x00010000, 0x0028, 0x00011400, 0x0002, 0xFFFFFFFF);;
	ENDCHUNK




SET _curgok = 0x0001220c CHUNK( 'GOKD', 0x0001220c, )  SHORT BO OSK LONG 1
	LONG 0 521 426 0
	LONG 0 0 0xffffffff 0x00000001 SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st 0 0xFFFFFFFF
ENDCHUNK

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\inspirat\cancel1.pbm" ENDCHUNK	 	
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0002 _st )
	ADOPT( 'GOKD', _curgok, 'MBMP', _dfmbmp, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st )

	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0003 _st ) PACKEDFILE "building\pbm\inspirat\cancel2.pbm" ENDCHUNK	

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0001 _st ) SCRIPT
		If( FGobExists( 0x00016019 ) );		
			DestroyGob(	0x00016019 );	
		End;	
		EnqueueCid(50087, 0x00012205, 0, 0, 0, 0);
	ENDCHUNK







SET _curgok = 0x00011404 CHUNK( 'GOKD', 0x00011404,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea1m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If( ((39 > 31) ? ( ::fBio2 & (1<<(39&31)) ) : ( ::fBio1 & (1<<(39&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (39 & 31); If (39 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 39; ::fBioCtrl = 0; CreateChildGob( 0x00011400, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK


SET _curgok = 0x0001140a CHUNK( 'GOKD', 0x0001140a,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea4m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If( ((7 > 31) ? ( ::fBio2 & (1<<(7&31)) ) : ( ::fBio1 & (1<<(7&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (7 & 31); If (7 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 7; ::fBioCtrl = 0; CreateChildGob( 0x00011700, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001140b CHUNK( 'GOKD', 0x0001140b,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea4m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If( ((8 > 31) ? ( ::fBio2 & (1<<(8&31)) ) : ( ::fBio1 & (1<<(8&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (8 & 31); If (8 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 8; ::fBioCtrl = 0; CreateChildGob( 0x00011700, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001140c CHUNK( 'GOKD', 0x0001140c,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea4m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((9 > 31) ? ( ::fBio2 & (1<<(9&31)) ) : ( ::fBio1 & (1<<(9&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (9 & 31); If (9 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 9; ::fBioCtrl = 0; CreateChildGob( 0x00011700, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011406 CHUNK( 'GOKD', 0x00011406,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea3m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((10 > 31) ? ( ::fBio2 & (1<<(10&31)) ) : ( ::fBio1 & (1<<(10&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (10 & 31); If (10 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 10; ::fBioCtrl = 0; CreateChildGob( 0x00011600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011407 CHUNK( 'GOKD', 0x00011407,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea3m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((11 > 31) ? ( ::fBio2 & (1<<(11&31)) ) : ( ::fBio1 & (1<<(11&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (11 & 31); If (11 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 11; ::fBioCtrl = 0; CreateChildGob( 0x00011600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011408 CHUNK( 'GOKD', 0x00011408,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea3m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((12 > 31) ? ( ::fBio2 & (1<<(12&31)) ) : ( ::fBio1 & (1<<(12&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (12 & 31); If (12 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 12; ::fBioCtrl = 0; CreateChildGob( 0x00011600, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011405 CHUNK( 'GOKD', 0x00011405,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\inspirat\idea2m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((13 > 31) ? ( ::fBio2 & (1<<(13&31)) ) : ( ::fBio1 & (1<<(13&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (13 & 31); If (13 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 13; ::fBioCtrl = 0; CreateChildGob( 0x00011500, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK
#line 69 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\studiobg.cht"









CHUNK( 'WAVE', 0x00018113, ) FILE "building\sound\studio\tvpop.wav" ENDCHUNK
SET _curgok = 0x00011801 CHUNK( 'GOKD', 0x00011801,  )  SHORT BO OSK LONG 0 LONG 0 0 0 120 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\studio\st1tbx.pbm" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
 		
 		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
 		cnt = 0x00000040;
		Cell( cnt++, 0,0, 6 );
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018113, 0, 0x00010000, 1, 0, 998);
		CreateChildGob( 0x00011800, 0x00011804, 0x00011804 );
		While (cnt <= 0x00000045);
			Cell(cnt++, 0,0, 6);
		End;
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		ChangeStateThis( 0x0002 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\studioBx.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\studio\box1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\studio\box2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\studio\box3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\studio\box4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\studio\box5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\studio\box6.pbm" ENDCHUNK
#line 30 "H:\\BUILD\\SOC\\src\\building\\studiobg.cht"

												 
SET _curgok = 0x00011804 CHUNK( 'GOKD', 0x00011804,  )  SHORT BO OSK LONG 0 LONG 0 0 0 810 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
 		cnt = 0x00000040;
		SetNoSlipThis( 1 );		
		While (cnt <= 0x00000055);
			Cell(cnt++, 0,0, 6);
		End;
		SetNoSlipThis( 0 );	
		Cell( cnt, 0,0, 6 );		
		ChangeStateGob( 0x00011802, 0x0004 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\studioTv.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\studio\tv1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\studio\tv2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\studio\tv3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\studio\tv4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\studio\tv5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\studio\tv6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\studio\tv7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\studio\tv8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\studio\tv9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\studio\tv10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\studio\tv11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\studio\tv12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\studio\tv13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\studio\tv14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\studio\tv15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\studio\tv16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\studio\tv17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\studio\tv18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\studio\tv19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\studio\tv20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\studio\tv21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\studio\tv22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\studio\tv23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\studio\tv24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\studio\tv25.pbm" ENDCHUNK
#line 44 "H:\\BUILD\\SOC\\src\\building\\studiobg.cht"






CHUNK( 'WAVE', 0x00018103, ) FILE "building\sound\studio\Std1A.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018104, ) FILE "building\sound\studio\VO97A.wav" ENDCHUNK
CHUNK( 'WAVE', 0x00018105, ) FILE "building\sound\studio\VO97B.wav" ENDCHUNK

CHUNK( 'WAVE', 0x0001803a, ) FILE "building\sound\studio\Std1B.wav" ENDCHUNK


CHUNK( 'WAVE', 0x0001810e, ) FILE "building\sound\studio\STD4.wav" ENDCHUNK


SET _curgok = 0x00011803 CHUNK( 'GOKD', 0x00011803,  )  SHORT BO OSK LONG 0 LONG 0 0 0 999 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "tools1.avi" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		
		StopThis();	
	ENDCHUNK



SET _curgok = 0x00011802 CHUNK( 'GOKD', 0x00011802,  )  SHORT BO OSK LONG 0 LONG 0 0 0 111 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	
	
	
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) LONG BO OSK 0 0 0 0 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK

	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		Cell(0x00000084, 0, 0, 120);
		If( ::fTransition ); Cell( 0, 0, 0, 60 ); ::fTransition = 0; End;
		While (1);
			
			iAnimRnd = Rnd(3);
			If (iAnimRnd == 0);		
				kcellStart = 0x00000040;
				kcellEnd = 0x00000082;
			Elif (iAnimRnd == 1);	
				kcellStart = 0x00000063;
				kcellEnd = 0x00000082;
			Else;					
				kcellStart = 0x00000040;
				kcellEnd = 0x00000069;
			End;

			If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018103 + iAnimRnd, 0, 0x00010000, 1, 0, 998);

 			i = kcellStart;
			Cell(i++, 0, 0, 6);
			SetNoSlipThis( 1 );		
			While (i < kcellEnd);
				Cell(i, 0, 0, 6);
				i++;
			End;
			SetNoSlipThis( 0 );		
			Cell( kcellEnd, 0,0, 6 );

			
			i=0x00000202;
			While (i <= 0x00000245);
				Cell(i, 0, 0, 6);
				i++;
			End;
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		End;
	ENDCHUNK

	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x0001803a, 0, 0x00010000, 1, 0, 998);
 		i = 0x00000084;
		While (i <= 0x000000c9);
			Cell(i, 0,0, 6);
			i++;
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
  		0x00010000->fHelpOn = (0);
  		DestroyGob(0x00016007);   
		ChangeStateThis(0x0002);	
	ENDCHUNK

	
	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0004 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		RunScriptGob( 0x00010000, 0x0024, 0xFFFFFFFF, 0 );	
		
		Cell( SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st, 0,0, 1 );	
		DestroyGob( 0x00011804 );
		DestroyGob( 0x00011801 );
		ChangeStateGob( 0x00011800, 0x0002 );		

		If( ::fStdIntro );
			
			
			ChangeStateGob( 0x00011802, 0x0005 );
			Exit();
		End;
 			
 		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		::fStdIntro = 1;
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		CreateChildGob( GidParThis(), 0x00011803, 0x00011803 );
		Cell( SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st,0,0, 100 );	
		While( PlayingGob( 0x00011803 ));
			Cell( SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st, 0, 0, 10 );	
		End; 
		0x00010000->fStudioPS = (0);
		ChangeStateThis( 0x0005 );
	ENDCHUNK
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0005 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		DestroyThis();	
		If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;		
		
		
		SetProp(0x23502, GetProp(0x23502) | 1);
		RunScriptGob(0x00010000, 0x0028, 0x00011800, 0x0001, 0xFFFFFFFF);;
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		If (!(0x00010000->fHelpOn));	
			0x00010000->fHelpOn = (1);
   			CreateHelpGob(0x00011800, 0x0001855a);
			ChangeStateThis(0x0003);
		Else;					
  			DestroyGob(0x00016007);   
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
			0x00010000->fHelpOn = (0);
			ChangeStateThis(0x0002);	
		End;
	ENDCHUNK
#line 1 "H:\\BUILD\\SOC\\src\\building\\mzstudio.seq"






SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\studio\mczee1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\studio\mczee2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\studio\mczee3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\studio\mczee4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\studio\mczee5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\studio\mczee6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\studio\mczee7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\studio\mczee8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\studio\mczee9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\studio\mczee10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\studio\mczee11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\studio\mczee12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\studio\mczee13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\studio\mczee14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\studio\mczee15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\studio\mczee16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\studio\mczee17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\studio\mczee18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\studio\mczee19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\studio\mczee20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\studio\mczee21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\studio\mczee22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\studio\mczee23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\studio\mczee24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\studio\mczee25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\studio\mczee26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\studio\mczee27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\studio\mczee28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\studio\mczee29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\studio\mczee30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\studio\mczee31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\studio\mczee32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\studio\mczee33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\studio\mczee34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\studio\mczee35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\studio\mczee36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\studio\mczee37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\studio\mczee38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\studio\mczee39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\studio\mczee40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\studio\mczee41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\studio\mczee42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\studio\mczee43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\studio\mczee44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\studio\mczee45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\studio\mczee46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\studio\mczee47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\studio\mczee48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\studio\mczee49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\studio\mczee50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\studio\mczee51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\studio\mczee52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\studio\mczee53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\studio\mczee54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000076 ) PACKEDFILE "building\pbm\studio\mczee55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000077 ) PACKEDFILE "building\pbm\studio\mczee56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000078 ) PACKEDFILE "building\pbm\studio\mczee57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000079 ) PACKEDFILE "building\pbm\studio\mczee58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007a ) PACKEDFILE "building\pbm\studio\mczee59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007b ) PACKEDFILE "building\pbm\studio\mczee60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\studio\mczee61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\studio\mczee62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\studio\mczee63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\studio\mczee64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\studio\mczee65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\studio\mczee66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\studio\mczee67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\studio\mczee68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\studio\mczee69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\studio\mczee70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\studio\mczee71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\studio\mczee72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\studio\mczee73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\studio\mczee74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\studio\mczee75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\studio\mczee76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\studio\mczee77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\studio\mczee78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\studio\mczee79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\studio\mczee80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\studio\mczee81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\studio\mczee82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000092 ) PACKEDFILE "building\pbm\studio\mczee83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000093 ) PACKEDFILE "building\pbm\studio\mczee84.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000094 ) PACKEDFILE "building\pbm\studio\mczee85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000095 ) PACKEDFILE "building\pbm\studio\mczee86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000096 ) PACKEDFILE "building\pbm\studio\mczee87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000097 ) PACKEDFILE "building\pbm\studio\mczee88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000098 ) PACKEDFILE "building\pbm\studio\mczee89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000099 ) PACKEDFILE "building\pbm\studio\mczee90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009a ) PACKEDFILE "building\pbm\studio\mczee91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009b ) PACKEDFILE "building\pbm\studio\mczee92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009c ) PACKEDFILE "building\pbm\studio\mczee93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009d ) PACKEDFILE "building\pbm\studio\mczee94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009e ) PACKEDFILE "building\pbm\studio\mczee95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009f ) PACKEDFILE "building\pbm\studio\mczee96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a0 ) PACKEDFILE "building\pbm\studio\mczee97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a1 ) PACKEDFILE "building\pbm\studio\mczee98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a2 ) PACKEDFILE "building\pbm\studio\mczee99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\studio\mczee100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\studio\mczee101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\studio\mczee102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\studio\mczee103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\studio\mczee104.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\studio\mczee105.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\studio\mczee106.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\studio\mczee107.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\studio\mczee108.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\studio\mczee109.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\studio\mczee110.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\studio\mczee111.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\studio\mczee112.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\studio\mczee113.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\studio\mczee114.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\studio\mczee115.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\studio\mczee116.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\studio\mczee117.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\studio\mczee118.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\studio\mczee119.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\studio\mczee120.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\studio\mczee121.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\studio\mczee122.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\studio\mczee123.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\studio\mczee124.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\studio\mczee125.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\studio\mczee126.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\studio\mczee127.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\studio\mczee128.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\studio\mczee129.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\studio\mczee130.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\studio\mczee131.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\studio\mczee132.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\studio\mczee133.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\studio\mczee134.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\studio\mczee135.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\studio\mczee136.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\studio\mczee137.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\studio\mczee138.pbm" ENDCHUNK


				 
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000202 ) PACKEDFILE "building\pbm\studio\cyc1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000203 ) PACKEDFILE "building\pbm\studio\cyc2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000204 ) PACKEDFILE "building\pbm\studio\cyc3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000205 ) PACKEDFILE "building\pbm\studio\cyc4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000206 ) PACKEDFILE "building\pbm\studio\cyc5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000207 ) PACKEDFILE "building\pbm\studio\cyc6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000208 ) PACKEDFILE "building\pbm\studio\cyc7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000209 ) PACKEDFILE "building\pbm\studio\cyc8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020a ) PACKEDFILE "building\pbm\studio\cyc9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020b ) PACKEDFILE "building\pbm\studio\cyc10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020c ) PACKEDFILE "building\pbm\studio\cyc11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020d ) PACKEDFILE "building\pbm\studio\cyc12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020e ) PACKEDFILE "building\pbm\studio\cyc13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000020f ) PACKEDFILE "building\pbm\studio\cyc14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000210 ) PACKEDFILE "building\pbm\studio\cyc15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000211 ) PACKEDFILE "building\pbm\studio\cyc16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000212 ) PACKEDFILE "building\pbm\studio\cyc17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000213 ) PACKEDFILE "building\pbm\studio\cyc18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000214 ) PACKEDFILE "building\pbm\studio\cyc19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000215 ) PACKEDFILE "building\pbm\studio\cyc20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000216 ) PACKEDFILE "building\pbm\studio\cyc21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000217 ) PACKEDFILE "building\pbm\studio\cyc22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000218 ) PACKEDFILE "building\pbm\studio\cyc23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000219 ) PACKEDFILE "building\pbm\studio\cyc24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021a ) PACKEDFILE "building\pbm\studio\cyc25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021b ) PACKEDFILE "building\pbm\studio\cyc26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021c ) PACKEDFILE "building\pbm\studio\cyc27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021d ) PACKEDFILE "building\pbm\studio\cyc28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021e ) PACKEDFILE "building\pbm\studio\cyc29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000021f ) PACKEDFILE "building\pbm\studio\cyc30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000220 ) PACKEDFILE "building\pbm\studio\cyc31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000221 ) PACKEDFILE "building\pbm\studio\cyc32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000222 ) PACKEDFILE "building\pbm\studio\cyc33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000223 ) PACKEDFILE "building\pbm\studio\cyc34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000224 ) PACKEDFILE "building\pbm\studio\cyc35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000225 ) PACKEDFILE "building\pbm\studio\cyc36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000226 ) PACKEDFILE "building\pbm\studio\cyc37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000227 ) PACKEDFILE "building\pbm\studio\cyc38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000228 ) PACKEDFILE "building\pbm\studio\cyc39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000229 ) PACKEDFILE "building\pbm\studio\cyc40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022a ) PACKEDFILE "building\pbm\studio\cyc41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022b ) PACKEDFILE "building\pbm\studio\cyc42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022c ) PACKEDFILE "building\pbm\studio\cyc43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022d ) PACKEDFILE "building\pbm\studio\cyc44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022e ) PACKEDFILE "building\pbm\studio\cyc45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000022f ) PACKEDFILE "building\pbm\studio\cyc46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000230 ) PACKEDFILE "building\pbm\studio\cyc47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000231 ) PACKEDFILE "building\pbm\studio\cyc48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000232 ) PACKEDFILE "building\pbm\studio\cyc49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000233 ) PACKEDFILE "building\pbm\studio\cyc50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000234 ) PACKEDFILE "building\pbm\studio\cyc51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000235 ) PACKEDFILE "building\pbm\studio\cyc52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000236 ) PACKEDFILE "building\pbm\studio\cyc53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000237 ) PACKEDFILE "building\pbm\studio\cyc54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000238 ) PACKEDFILE "building\pbm\studio\cyc55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000239 ) PACKEDFILE "building\pbm\studio\cyc56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023a ) PACKEDFILE "building\pbm\studio\cyc57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023b ) PACKEDFILE "building\pbm\studio\cyc58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023c ) PACKEDFILE "building\pbm\studio\cyc59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023d ) PACKEDFILE "building\pbm\studio\cyc60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023e ) PACKEDFILE "building\pbm\studio\cyc61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000023f ) PACKEDFILE "building\pbm\studio\cyc62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000240 ) PACKEDFILE "building\pbm\studio\cyc63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000241 ) PACKEDFILE "building\pbm\studio\cyc64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000242 ) PACKEDFILE "building\pbm\studio\cyc65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000243 ) PACKEDFILE "building\pbm\studio\cyc66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000244 ) PACKEDFILE "building\pbm\studio\cyc67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000245 ) PACKEDFILE "building\pbm\studio\cyc68.pbm" ENDCHUNK
#line 187 "H:\\BUILD\\SOC\\src\\building\\studiobg.cht"


SET _curgok = 0x00011901 CHUNK( 'GOKD', 0x00011901,  )  SHORT BO OSK LONG 0 LONG 0 0 0 33 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\st2doors.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x0002);
ENDCHUNK

SET _curgok = 0x00011903 CHUNK( 'GOKD', 0x00011903,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\st2map.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
	CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
ENDCHUNK

SET _curgok = 0x00011902 CHUNK( 'GOKD', 0x00011902,  )  SHORT BO OSK LONG 0 LONG 0 0 0 45 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, ,  ) "building\bitmaps\studio\st2exit.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
	EnqueueCid(106, 0, 0, 0, 0, 0);;
ENDCHUNK

SET _curgok = 0x00011904 CHUNK( 'GOKD', 0x00011904,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) PACKEDFILE "building\pbm\studio\st2exit.pbm" ENDCHUNK





SET _curgok = 0x00011805 CHUNK( 'GOKD', 0x00011805,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo1m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((38 > 31) ? ( ::fBio2 & (1<<(38&31)) ) : ( ::fBio1 & (1<<(38&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (38 & 31); If (38 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 38; ::fBioCtrl = 0; CreateChildGob( 0x00011800, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK



SET _curgok = 0x00011905 CHUNK( 'GOKD', 0x00011905,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo4m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((14 > 31) ? ( ::fBio2 & (1<<(14&31)) ) : ( ::fBio1 & (1<<(14&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (14 & 31); If (14 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 14; ::fBioCtrl = 0; CreateChildGob( 0x00011b00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011906 CHUNK( 'GOKD', 0x00011906,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo4m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((15 > 31) ? ( ::fBio2 & (1<<(15&31)) ) : ( ::fBio1 & (1<<(15&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (15 & 31); If (15 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 15; ::fBioCtrl = 0; CreateChildGob( 0x00011b00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011907 CHUNK( 'GOKD', 0x00011907,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo4m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((16 > 31) ? ( ::fBio2 & (1<<(16&31)) ) : ( ::fBio1 & (1<<(16&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (16 & 31); If (16 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 16; ::fBioCtrl = 0; CreateChildGob( 0x00011b00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011908 CHUNK( 'GOKD', 0x00011908,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo4m4.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((17 > 31) ? ( ::fBio2 & (1<<(17&31)) ) : ( ::fBio1 & (1<<(17&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (17 & 31); If (17 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 17; ::fBioCtrl = 0; CreateChildGob( 0x00011b00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x00011909 CHUNK( 'GOKD', 0x00011909,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo3m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((18 > 31) ? ( ::fBio2 & (1<<(18&31)) ) : ( ::fBio1 & (1<<(18&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (18 & 31); If (18 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 18; ::fBioCtrl = 0; CreateChildGob( 0x00011a00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001190a CHUNK( 'GOKD', 0x0001190a,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo3m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((19 > 31) ? ( ::fBio2 & (1<<(19&31)) ) : ( ::fBio1 & (1<<(19&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (19 & 31); If (19 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 19; ::fBioCtrl = 0; CreateChildGob( 0x00011a00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001190b CHUNK( 'GOKD', 0x0001190b,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo3m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((20 > 31) ? ( ::fBio2 & (1<<(20&31)) ) : ( ::fBio1 & (1<<(20&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (20 & 31); If (20 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 20; ::fBioCtrl = 0; CreateChildGob( 0x00011a00, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001190c CHUNK( 'GOKD', 0x0001190c,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo2m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((21 > 31) ? ( ::fBio2 & (1<<(21&31)) ) : ( ::fBio1 & (1<<(21&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (21 & 31); If (21 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 21; ::fBioCtrl = 0; CreateChildGob( 0x00011900, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001190d CHUNK( 'GOKD', 0x0001190d,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo2m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((22 > 31) ? ( ::fBio2 & (1<<(22&31)) ) : ( ::fBio1 & (1<<(22&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (22 & 31); If (22 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 22; ::fBioCtrl = 0; CreateChildGob( 0x00011900, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK

SET _curgok = 0x0001190e CHUNK( 'GOKD', 0x0001190e,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\studio\stdo2m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((23 > 31) ? ( ::fBio2 & (1<<(23&31)) ) : ( ::fBio1 & (1<<(23&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (23 & 31); If (23 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 23; ::fBioCtrl = 0; CreateChildGob( 0x00011900, 0x00015100, 0x00015100 );
		End;		
	ENDCHUNK
#line 70 "H:\\BUILD\\SOC\\src\\building\\building.cht"
#line 1 "H:\\BUILD\\SOC\\src\\building\\projects.cht"


























CHUNK( 'WAVE', 0x00018109, ) FILE "building\sound\projects\Prjt1.wav" ENDCHUNK

CHUNK( 'WAVE', 0x00018112, ) FILE "building\sound\projects\monitor.wav" ENDCHUNK

SET _curgok = 0x00011b08 CHUNK( 'GOKD', 0x00011b08, )  SHORT BO OSK LONG 2
	LONG 0 64 140 999
	LONG 0 0 0xffffffff 0x00000002 0xFFFFFFFF 0 0xFFFFFFFF
ENDCHUNK
	SET _dfvid++ CHUNK( 'VIDE', _dfvid,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) BYTE 1 STN "projt2.avi" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		FilterCmdsThis( 100007, 0, 0x0021);
		FilterCmdsThis( 40042, 0, 0x0021 );		
		FilterCmdsThis( 100, 0, 0x0021 );
		FilterCmdsThis( 101, 0, 0x0021 );
	ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, 0x0021 ) SCRIPT
		
		StopThis();	
		DestroyThis();	
		Return( 0 );	
	ENDCHUNK		


SET _curgok = 0x00011b02 CHUNK( 'GOKD', 0x00011b02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 75 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\projects\prj1mon1.pbm" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		PrintStr("Monitor1: animation");
		
		DestroyGob( 0x00011c0d );
		
		SetZThis( 2160 );
		If ( FGobExists(0x00016007));	
			DestroyGob(0x00016007);
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018112, 0, 0x00010000, 1, 0, 998);
		cnt = 0x00000041;			
		Cell( cnt++, 0, 0, 6 );
		SetNoSlipThis( 1 );		
		While( cnt <= 0x0000004f );
			Cell( cnt++, 0, 0, 6 );
			
		End;
		
		CreateChildGob( 0x00011c00, 0x00011b05, 0x00011b05);
	ENDCHUNK
	
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		ChangeStateThis( 0x0002 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\PrjMon1.seq"
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\projects\tvlan1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\projects\tvlan2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\projects\tvlan3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\projects\tvlan4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\projects\tvlan5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\projects\tvlan6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\projects\tvlan7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\projects\tvlan8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\projects\tvlan9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\projects\tvlan10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\projects\tvlan11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\projects\tvlan12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\projects\tvlan13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\projects\tvlan14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\projects\tvlan15.pbm" ENDCHUNK
#line 78 "H:\\BUILD\\SOC\\src\\building\\projects.cht"

SET _curgok = 0x00011c0d CHUNK( 'GOKD', 0x00011c0d,  )  SHORT BO OSK LONG 2 LONG 0 0 0 150 LONG 0 0 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "building\bitmaps\projects\holder1.bmp" ENDCHUNK


SET _curgok = 0x00011b03 CHUNK( 'GOKD', 0x00011b03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 155 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) PACKEDFILE "building\pbm\projects\prj1mon2.pbm" ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		StartLongOp(); RunScriptGob( 0x00010000, 0x0023, (0x00000002) );
		PrintStr("Monitor2: animation");
		
		DestroyGob( 0x00011c0e );
		
		SetZThis( 2160 );

		If ( FGobExists(0x00016007));	
			DestroyGob(0x00016007);
		End;
		If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018112, 0, 0x00010000, 1, 0, 998);
		cnt = 0x00000041;
		Cell( cnt++, 0, 0, 6 );
		SetNoSlipThis( 1 );		
		While( cnt <= 0x0000004f );
			Cell( cnt++, 0, 0, 6 );
		End;
		CreateChildGob(0x00011c00, 0x00011b06, 0x00011b06);
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		ChangeStateThis( 0x0002 );
	ENDCHUNK
	#line 1 "H:\\BUILD\\SOC\\src\\building\\PrjMon2.seq"

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\projects\tvran1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\projects\tvran2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\projects\tvran3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\projects\tvran4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\projects\tvran5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\projects\tvran6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\projects\tvran7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\projects\tvran8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\projects\tvran9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\projects\tvran10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\projects\tvran11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\projects\tvran12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\projects\tvran13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\projects\tvran14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\projects\tvran15.pbm" ENDCHUNK
#line 110 "H:\\BUILD\\SOC\\src\\building\\projects.cht"

SET _curgok = 0x00011c0e CHUNK( 'GOKD', 0x00011c0e,  )  SHORT BO OSK LONG 2 LONG 0 0 0 155 LONG 0 0 0xffffffff 0x00000001 0xFFFFFFFF 0 0xFFFFFFFF ENDCHUNK
	SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) BITMAP( 0x00, 0, 0 ) "building\bitmaps\projects\holder2.bmp" ENDCHUNK




SET _curgok = 0x00011b01 CHUNK( 'GOKD', 0x00011b01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 100 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
 	#line 1 "H:\\BUILD\\SOC\\src\\building\\melproj1.seq"





SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000040 ) PACKEDFILE "building\pbm\projects\mel1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000041 ) PACKEDFILE "building\pbm\projects\mel2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000042 ) PACKEDFILE "building\pbm\projects\mel3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000043 ) PACKEDFILE "building\pbm\projects\mel4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000044 ) PACKEDFILE "building\pbm\projects\mel5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000045 ) PACKEDFILE "building\pbm\projects\mel6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000046 ) PACKEDFILE "building\pbm\projects\mel7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000047 ) PACKEDFILE "building\pbm\projects\mel8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000048 ) PACKEDFILE "building\pbm\projects\mel9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000049 ) PACKEDFILE "building\pbm\projects\mel10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004a ) PACKEDFILE "building\pbm\projects\mel11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004b ) PACKEDFILE "building\pbm\projects\mel12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004c ) PACKEDFILE "building\pbm\projects\mel13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004d ) PACKEDFILE "building\pbm\projects\mel14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004e ) PACKEDFILE "building\pbm\projects\mel15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000004f ) PACKEDFILE "building\pbm\projects\mel16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000050 ) PACKEDFILE "building\pbm\projects\mel17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000051 ) PACKEDFILE "building\pbm\projects\mel18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000052 ) PACKEDFILE "building\pbm\projects\mel19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000053 ) PACKEDFILE "building\pbm\projects\mel20.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000054 ) PACKEDFILE "building\pbm\projects\mel21.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000055 ) PACKEDFILE "building\pbm\projects\mel22.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000056 ) PACKEDFILE "building\pbm\projects\mel23.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000057 ) PACKEDFILE "building\pbm\projects\mel24.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000058 ) PACKEDFILE "building\pbm\projects\mel25.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000059 ) PACKEDFILE "building\pbm\projects\mel26.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005a ) PACKEDFILE "building\pbm\projects\mel27.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005b ) PACKEDFILE "building\pbm\projects\mel28.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005c ) PACKEDFILE "building\pbm\projects\mel29.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005d ) PACKEDFILE "building\pbm\projects\mel30.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005e ) PACKEDFILE "building\pbm\projects\mel31.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000005f ) PACKEDFILE "building\pbm\projects\mel32.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000060 ) PACKEDFILE "building\pbm\projects\mel33.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000061 ) PACKEDFILE "building\pbm\projects\mel34.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000062 ) PACKEDFILE "building\pbm\projects\mel35.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000063 ) PACKEDFILE "building\pbm\projects\mel36.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000064 ) PACKEDFILE "building\pbm\projects\mel37.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000065 ) PACKEDFILE "building\pbm\projects\mel38.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000066 ) PACKEDFILE "building\pbm\projects\mel39.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000067 ) PACKEDFILE "building\pbm\projects\mel40.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000068 ) PACKEDFILE "building\pbm\projects\mel41.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000069 ) PACKEDFILE "building\pbm\projects\mel42.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006a ) PACKEDFILE "building\pbm\projects\mel43.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006b ) PACKEDFILE "building\pbm\projects\mel44.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006c ) PACKEDFILE "building\pbm\projects\mel45.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006d ) PACKEDFILE "building\pbm\projects\mel46.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006e ) PACKEDFILE "building\pbm\projects\mel47.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000006f ) PACKEDFILE "building\pbm\projects\mel48.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000070 ) PACKEDFILE "building\pbm\projects\mel49.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000071 ) PACKEDFILE "building\pbm\projects\mel50.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000072 ) PACKEDFILE "building\pbm\projects\mel51.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000073 ) PACKEDFILE "building\pbm\projects\mel52.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000074 ) PACKEDFILE "building\pbm\projects\mel53.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000075 ) PACKEDFILE "building\pbm\projects\mel54.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000076 ) PACKEDFILE "building\pbm\projects\mel55.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000077 ) PACKEDFILE "building\pbm\projects\mel56.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000078 ) PACKEDFILE "building\pbm\projects\mel57.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000079 ) PACKEDFILE "building\pbm\projects\mel58.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007a ) PACKEDFILE "building\pbm\projects\mel59.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007b ) PACKEDFILE "building\pbm\projects\mel60.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007c ) PACKEDFILE "building\pbm\projects\mel61.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007d ) PACKEDFILE "building\pbm\projects\mel62.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007e ) PACKEDFILE "building\pbm\projects\mel63.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000007f ) PACKEDFILE "building\pbm\projects\mel64.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000080 ) PACKEDFILE "building\pbm\projects\mel65.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000081 ) PACKEDFILE "building\pbm\projects\mel66.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000082 ) PACKEDFILE "building\pbm\projects\mel67.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000083 ) PACKEDFILE "building\pbm\projects\mel68.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000084 ) PACKEDFILE "building\pbm\projects\mel69.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000085 ) PACKEDFILE "building\pbm\projects\mel70.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000086 ) PACKEDFILE "building\pbm\projects\mel71.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000087 ) PACKEDFILE "building\pbm\projects\mel72.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000088 ) PACKEDFILE "building\pbm\projects\mel73.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000089 ) PACKEDFILE "building\pbm\projects\mel74.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008a ) PACKEDFILE "building\pbm\projects\mel75.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008b ) PACKEDFILE "building\pbm\projects\mel76.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008c ) PACKEDFILE "building\pbm\projects\mel77.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008d ) PACKEDFILE "building\pbm\projects\mel78.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008e ) PACKEDFILE "building\pbm\projects\mel79.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000008f ) PACKEDFILE "building\pbm\projects\mel80.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000090 ) PACKEDFILE "building\pbm\projects\mel81.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000091 ) PACKEDFILE "building\pbm\projects\mel82.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000092 ) PACKEDFILE "building\pbm\projects\mel83.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000093 ) PACKEDFILE "building\pbm\projects\mel84.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000094 ) PACKEDFILE "building\pbm\projects\mel85.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000095 ) PACKEDFILE "building\pbm\projects\mel86.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000096 ) PACKEDFILE "building\pbm\projects\mel87.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000097 ) PACKEDFILE "building\pbm\projects\mel88.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000098 ) PACKEDFILE "building\pbm\projects\mel89.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000099 ) PACKEDFILE "building\pbm\projects\mel90.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009a ) PACKEDFILE "building\pbm\projects\mel91.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009b ) PACKEDFILE "building\pbm\projects\mel92.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009c ) PACKEDFILE "building\pbm\projects\mel93.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009d ) PACKEDFILE "building\pbm\projects\mel94.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009e ) PACKEDFILE "building\pbm\projects\mel95.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000009f ) PACKEDFILE "building\pbm\projects\mel96.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a0 ) PACKEDFILE "building\pbm\projects\mel97.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a1 ) PACKEDFILE "building\pbm\projects\mel98.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a2 ) PACKEDFILE "building\pbm\projects\mel99.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a3 ) PACKEDFILE "building\pbm\projects\mel100.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a4 ) PACKEDFILE "building\pbm\projects\mel101.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a5 ) PACKEDFILE "building\pbm\projects\mel102.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a6 ) PACKEDFILE "building\pbm\projects\mel103.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a7 ) PACKEDFILE "building\pbm\projects\mel104.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a8 ) PACKEDFILE "building\pbm\projects\mel105.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000a9 ) PACKEDFILE "building\pbm\projects\mel106.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000aa ) PACKEDFILE "building\pbm\projects\mel107.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ab ) PACKEDFILE "building\pbm\projects\mel108.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ac ) PACKEDFILE "building\pbm\projects\mel109.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ad ) PACKEDFILE "building\pbm\projects\mel110.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ae ) PACKEDFILE "building\pbm\projects\mel111.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000af ) PACKEDFILE "building\pbm\projects\mel112.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b0 ) PACKEDFILE "building\pbm\projects\mel113.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b1 ) PACKEDFILE "building\pbm\projects\mel114.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b2 ) PACKEDFILE "building\pbm\projects\mel115.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b3 ) PACKEDFILE "building\pbm\projects\mel116.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b4 ) PACKEDFILE "building\pbm\projects\mel117.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b5 ) PACKEDFILE "building\pbm\projects\mel118.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b6 ) PACKEDFILE "building\pbm\projects\mel119.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b7 ) PACKEDFILE "building\pbm\projects\mel120.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b8 ) PACKEDFILE "building\pbm\projects\mel121.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000b9 ) PACKEDFILE "building\pbm\projects\mel122.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ba ) PACKEDFILE "building\pbm\projects\mel123.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bb ) PACKEDFILE "building\pbm\projects\mel124.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bc ) PACKEDFILE "building\pbm\projects\mel125.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bd ) PACKEDFILE "building\pbm\projects\mel126.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000be ) PACKEDFILE "building\pbm\projects\mel127.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000bf ) PACKEDFILE "building\pbm\projects\mel128.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c0 ) PACKEDFILE "building\pbm\projects\mel129.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c1 ) PACKEDFILE "building\pbm\projects\mel130.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c2 ) PACKEDFILE "building\pbm\projects\mel131.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c3 ) PACKEDFILE "building\pbm\projects\mel132.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c4 ) PACKEDFILE "building\pbm\projects\mel133.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c5 ) PACKEDFILE "building\pbm\projects\mel134.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c6 ) PACKEDFILE "building\pbm\projects\mel135.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c7 ) PACKEDFILE "building\pbm\projects\mel136.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c8 ) PACKEDFILE "building\pbm\projects\mel137.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000c9 ) PACKEDFILE "building\pbm\projects\mel138.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ca ) PACKEDFILE "building\pbm\projects\mel139.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cb ) PACKEDFILE "building\pbm\projects\mel140.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cc ) PACKEDFILE "building\pbm\projects\mel141.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cd ) PACKEDFILE "building\pbm\projects\mel142.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ce ) PACKEDFILE "building\pbm\projects\mel143.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000cf ) PACKEDFILE "building\pbm\projects\mel144.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d0 ) PACKEDFILE "building\pbm\projects\mel145.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d1 ) PACKEDFILE "building\pbm\projects\mel146.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d2 ) PACKEDFILE "building\pbm\projects\mel147.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d3 ) PACKEDFILE "building\pbm\projects\mel148.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d4 ) PACKEDFILE "building\pbm\projects\mel149.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d5 ) PACKEDFILE "building\pbm\projects\mel150.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d6 ) PACKEDFILE "building\pbm\projects\mel151.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d7 ) PACKEDFILE "building\pbm\projects\mel152.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d8 ) PACKEDFILE "building\pbm\projects\mel153.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000d9 ) PACKEDFILE "building\pbm\projects\mel154.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000da ) PACKEDFILE "building\pbm\projects\mel155.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000db ) PACKEDFILE "building\pbm\projects\mel156.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dc ) PACKEDFILE "building\pbm\projects\mel157.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000dd ) PACKEDFILE "building\pbm\projects\mel158.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000de ) PACKEDFILE "building\pbm\projects\mel159.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000df ) PACKEDFILE "building\pbm\projects\mel160.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e0 ) PACKEDFILE "building\pbm\projects\mel161.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e1 ) PACKEDFILE "building\pbm\projects\mel162.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e2 ) PACKEDFILE "building\pbm\projects\mel163.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e3 ) PACKEDFILE "building\pbm\projects\mel164.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e4 ) PACKEDFILE "building\pbm\projects\mel165.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e5 ) PACKEDFILE "building\pbm\projects\mel166.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e6 ) PACKEDFILE "building\pbm\projects\mel167.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e7 ) PACKEDFILE "building\pbm\projects\mel168.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e8 ) PACKEDFILE "building\pbm\projects\mel169.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000e9 ) PACKEDFILE "building\pbm\projects\mel170.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ea ) PACKEDFILE "building\pbm\projects\mel171.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000eb ) PACKEDFILE "building\pbm\projects\mel172.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ec ) PACKEDFILE "building\pbm\projects\mel173.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ed ) PACKEDFILE "building\pbm\projects\mel174.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ee ) PACKEDFILE "building\pbm\projects\mel175.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000ef ) PACKEDFILE "building\pbm\projects\mel176.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f0 ) PACKEDFILE "building\pbm\projects\mel177.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f1 ) PACKEDFILE "building\pbm\projects\mel178.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f2 ) PACKEDFILE "building\pbm\projects\mel179.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f3 ) PACKEDFILE "building\pbm\projects\mel180.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f4 ) PACKEDFILE "building\pbm\projects\mel181.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f5 ) PACKEDFILE "building\pbm\projects\mel182.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x000000f6 ) PACKEDFILE "building\pbm\projects\mel183.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000108 ) PACKEDFILE "building\pbm\projects\melcy1.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000109 ) PACKEDFILE "building\pbm\projects\melcy2.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010a ) PACKEDFILE "building\pbm\projects\melcy3.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010b ) PACKEDFILE "building\pbm\projects\melcy4.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010c ) PACKEDFILE "building\pbm\projects\melcy5.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010d ) PACKEDFILE "building\pbm\projects\melcy6.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010e ) PACKEDFILE "building\pbm\projects\melcy7.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000010f ) PACKEDFILE "building\pbm\projects\melcy8.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000110 ) PACKEDFILE "building\pbm\projects\melcy9.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000111 ) PACKEDFILE "building\pbm\projects\melcy10.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000112 ) PACKEDFILE "building\pbm\projects\melcy11.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000113 ) PACKEDFILE "building\pbm\projects\melcy12.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000114 ) PACKEDFILE "building\pbm\projects\melcy13.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000115 ) PACKEDFILE "building\pbm\projects\melcy14.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000116 ) PACKEDFILE "building\pbm\projects\melcy15.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000117 ) PACKEDFILE "building\pbm\projects\melcy16.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000118 ) PACKEDFILE "building\pbm\projects\melcy17.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000119 ) PACKEDFILE "building\pbm\projects\melcy18.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011a ) PACKEDFILE "building\pbm\projects\melcy19.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000011b ) PACKEDFILE "building\pbm\projects\melcy20.pbm" ENDCHUNK

SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016c ) PACKEDFILE "building\pbm\projects\mel301.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016d ) PACKEDFILE "building\pbm\projects\mel302.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016e ) PACKEDFILE "building\pbm\projects\mel303.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000016f ) PACKEDFILE "building\pbm\projects\mel304.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000170 ) PACKEDFILE "building\pbm\projects\mel305.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000171 ) PACKEDFILE "building\pbm\projects\mel306.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000172 ) PACKEDFILE "building\pbm\projects\mel307.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000173 ) PACKEDFILE "building\pbm\projects\mel308.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000174 ) PACKEDFILE "building\pbm\projects\mel309.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000175 ) PACKEDFILE "building\pbm\projects\mel310.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000176 ) PACKEDFILE "building\pbm\projects\mel311.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000177 ) PACKEDFILE "building\pbm\projects\mel312.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000178 ) PACKEDFILE "building\pbm\projects\mel313.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000179 ) PACKEDFILE "building\pbm\projects\mel314.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017a ) PACKEDFILE "building\pbm\projects\mel315.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017b ) PACKEDFILE "building\pbm\projects\mel316.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017c ) PACKEDFILE "building\pbm\projects\mel317.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017d ) PACKEDFILE "building\pbm\projects\mel318.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017e ) PACKEDFILE "building\pbm\projects\mel319.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x0000017f ) PACKEDFILE "building\pbm\projects\mel320.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000180 ) PACKEDFILE "building\pbm\projects\mel321.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000181 ) PACKEDFILE "building\pbm\projects\mel322.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000182 ) PACKEDFILE "building\pbm\projects\mel323.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000183 ) PACKEDFILE "building\pbm\projects\mel324.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000184 ) PACKEDFILE "building\pbm\projects\mel325.pbm" ENDCHUNK
SET _dfmbmp++ CHUNK( 'MBMP', _dfmbmp,  ) PARENT( 'GOKD', _curgok, 0x00000185 ) PACKEDFILE "building\pbm\projects\mel326.pbm" ENDCHUNK
#line 119 "H:\\BUILD\\SOC\\src\\building\\projects.cht"

	
	
	
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x0001 SET _st <<= 16 SET _st += 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK

	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0002 SET _st <<= 16 SET _st += 0x0000 _st ) SCRIPT
		PrintStr("Melanie state 2 rep.");
		Cell(0x00000108, 0, 0, 0);

		
		If ((0x00010000->fHelpOn));
			PrintStr("Help is on.");
			CreateHelpGob(0x00011c00, 0x00018556);
		End;
		
		While( 1 );
			If( ::siiVoice ); StopSound( ::siiVoice ); End; ::siiVoice = PlaySoundThis('WAVE', 0x00018109, 0, 0x00010000, 1, 0, 998);
			i = 0x00000108;
			While (i <= 0x0000011c );
				Cell(i, 0, 0, 6);
				i++;
			End;
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
		End;
	ENDCHUNK

	
	
	
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x0003 SET _st <<= 16 SET _st += 0x0010 _st ) SCRIPT
		PrintStr("Melanie state 3 rep.");

		
		
		CreateChildGob( 0x00011c00, 0x00011b08, 0x00011b08 );

		
		Cell( 0x00000040, 0,0, 100 );
		While( PlayingGob( 0x00011b08 ));
			Cell( 0x00000040, 0, 0, 10 );	
		End; 
		If( FGobExists( 0x00011b08 ));
			DestroyGob( 0x00011b08 );
		End;

		
		0x00010000->fHelpOn = (1);
		ChangeStateThis(0x0002);
	ENDCHUNK

	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
		
		
		If (StateThis() == 0x0003);
			0x00010000->fHelpOn = (1);
			If( ::siiVoice ); StopSound( ::siiVoice ); ::siiVoice = 0; End;;
			ChangeStateThis(0x0002);
		
		Elif (!(0x00010000->fHelpOn));	
			ChangeStateThis(0x0003);
		Else;					
			0x00010000->fHelpOn = (0);
			DestroyGob(0x00016007);   
		End;
	ENDCHUNK

CHUNK( 'GLOP', 0x00018556, ) SCRIPT
 	If (_parm[0] == 0);
		PrintStr("On Help tip create...");
		
	Elif (_parm[0] == 1);		
		ChangeStateGob( 0x00011b02, 0x0002 );
		
 	Elif (_parm[0] == 2);		
		ChangeStateGob( 0x00011b03, 0x0002 );
	End;
ENDCHUNK


SET _curgok = 0x00011b05 CHUNK( 'GOKD', 0x00011b05,  )  SHORT BO OSK LONG 0 LONG 0 0 0 200 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		PrintStr("Creating FLYING LOGOS object.");
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));		
		
		SetProp(0x23502, GetProp(0x23502) & 0xfffffffe);		
		DestroyGob(0x00016007);
		RunScriptGob(0x00010000, 0x0028, 0x00011c00, 0x0002, 0x0028);;
	ENDCHUNK

SET _curgok = 0x00011b06 CHUNK( 'GOKD', 0x00011b06,  )  SHORT BO OSK LONG 0 LONG 0 0 0 200 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dffill++ CHUNK( 'FILL', _dffill,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) LONG BO OSK 0 0 640 480 LONG 0xffffffff 0xffffffff BYTE 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ENDCHUNK
	SET _dfanm++ CHUNK( 'GLSC', _dfanm,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0010 _st ) SCRIPT
		PrintStr("Creating MOVIE TECHNIQUES object.");
		EndLongOp( 1 ); If( FGobExists( 0x0001010c ) ); DestroyGob( 0x0001010c ); End; 0x00010000->keys = ((0x00000004 | 0x00000002 | 0x00000001 | 0x00000008 | 0x00000010 | 0x00000020));
		
		SetProp(0x23502, GetProp(0x23502) & 0xfffffffe);
		DestroyGob(0x00016007);
		RunScriptGob(0x00010000, 0x0028, 0x00011c00, 0x0002, 0x0024);;
	ENDCHUNK






SET _curgok = 0x00011c03 CHUNK( 'GOKD', 0x00011c03,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prj2map.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
	CreateChildGob( ::kgobCurrentPlace, 0x00012000, 0x00012000 );
ENDCHUNK

SET _curgok = 0x00011c02 CHUNK( 'GOKD', 0x00011c02,  )  SHORT BO OSK LONG 0 LONG 0 0 0 44 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prj2exit.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
	EnqueueCid(106, 0, 0, 0, 0, 0);;
ENDCHUNK

SET _curgok = 0x00011c01 CHUNK( 'GOKD', 0x00011c01,  )  SHORT BO OSK LONG 0 LONG 0 0 0 33 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prj2door.bmp" ENDCHUNK
SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT
	RunScriptGob(0x00010000, 0x0021, 0x00011000, 0x0002);
ENDCHUNK






SET _curgok = 0x00011b07 CHUNK( 'GOKD', 0x00011b07,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\p1mask.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((37 > 31) ? ( ::fBio2 & (1<<(37&31)) ) : ( ::fBio1 & (1<<(37&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (37 & 31); If (37 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 37; ::fBioCtrl = 0; CreateChildGob( 0x00011c00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK


SET _curgok = 0x00011c04 CHUNK( 'GOKD', 0x00011c04,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt2m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((34 > 31) ? ( ::fBio2 & (1<<(34&31)) ) : ( ::fBio1 & (1<<(34&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (34 & 31); If (34 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 34; ::fBioCtrl = 0; CreateChildGob( 0x00011d00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK



SET _curgok = 0x00011c05 CHUNK( 'GOKD', 0x00011c05,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt2m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((1 > 31) ? ( ::fBio2 & (1<<(1&31)) ) : ( ::fBio1 & (1<<(1&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (1 & 31); If (1 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 1; ::fBioCtrl = 0; CreateChildGob( 0x00011d00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK

SET _curgok = 0x00011c06 CHUNK( 'GOKD', 0x00011c06,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt2m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((2 > 31) ? ( ::fBio2 & (1<<(2&31)) ) : ( ::fBio1 & (1<<(2&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (2 & 31); If (2 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 2; ::fBioCtrl = 0; CreateChildGob( 0x00011d00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK

SET _curgok = 0x00011c07 CHUNK( 'GOKD', 0x00011c07,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt2m4.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((3 > 31) ? ( ::fBio2 & (1<<(3&31)) ) : ( ::fBio1 & (1<<(3&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (3 & 31); If (3 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 3; ::fBioCtrl = 0; CreateChildGob( 0x00011d00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK

SET _curgok = 0x00011c09 CHUNK( 'GOKD', 0x00011c09,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt4m1.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((4 > 31) ? ( ::fBio2 & (1<<(4&31)) ) : ( ::fBio1 & (1<<(4&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (4 & 31); If (4 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 4; ::fBioCtrl = 0; CreateChildGob( 0x00011f00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK

SET _curgok = 0x00011c0a CHUNK( 'GOKD', 0x00011c0a,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt4m2.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((5 > 31) ? ( ::fBio2 & (1<<(5&31)) ) : ( ::fBio1 & (1<<(5&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (5 & 31); If (5 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 5; ::fBioCtrl = 0; CreateChildGob( 0x00011f00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK

SET _curgok = 0x00011c0b CHUNK( 'GOKD', 0x00011c0b,  )  SHORT BO OSK LONG 0 LONG 0 0 0 0 LONG 0 0 0xffffffff 0x00000001 SET _st = 0x10000 SET _st |= 0x0001 _st 0 0xFFFFFFFF ENDCHUNK
	SET _dfmask++ CHUNK( 'MASK', _dfmask,  )  PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0000 _st ) MASK( 0x00, 0, 0 ) "building\bitmaps\projects\prjt4m3.bmp" ENDCHUNK
	SET _dfscr++ CHUNK( 'GLOP', _dfscr,  ) PARENT( 'GOKD', _curgok, SET _st = 0x10000 SET _st |= 0x0001 _st ) SCRIPT 
		If(((6 > 31) ? ( ::fBio2 & (1<<(6&31)) ) : ( ::fBio1 & (1<<(6&31)) )) );
			
			PlaySoundThis('WAVE', 0x00010045, 0, 0x00010000, 1, 0,
					998);
		Else;
			__t = 1<< (6 & 31); If (6 > 31 ); ::fBio2 |= __t; SetProp( 0x23501, ::fBio2 ); Else; ::fBio1 |= __t; SetProp( 0x23500, ::fBio1 ); End;;
			::nBioPage = 6; ::fBioCtrl = 0; CreateChildGob( 0x00011f00, 0x00015100, 0x00015100 );
		End;
		
	ENDCHUNK
#line 71 "H:\\BUILD\\SOC\\src\\building\\building.cht"


