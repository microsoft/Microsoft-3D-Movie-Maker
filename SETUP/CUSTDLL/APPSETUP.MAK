# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 U App Debug
!MESSAGE No configuration specified.  Defaulting to Win32 U App Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 U App Release" && "$(CFG)" != "Win32 U App Debug" &&\
 "$(CFG)" != "Win32 D App Release" && "$(CFG)" != "Win32 Z App Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "appsetup.mak" CFG="Win32 U App Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 U App Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 U App Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 D App Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Z App Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 U App Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 U App Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\wins"
# PROP Intermediate_Dir "obj\wins"
OUTDIR=.\obj\wins
INTDIR=.\obj\wins

ALL : \soc\setup\u\3DMSETUP.DLL $(OUTDIR)/appsetup.map $(OUTDIR)/3DMSETUP.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"appsetup.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\obj\wins/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "res" /i "inc" /d "NDEBUG" /d LANG='U'
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /d "NDEBUG" /d\
 LANG='U' 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"obj\wins\3DMSETUP.bsc"
BSC32_FLAGS=/nologo /o"obj\wins\3DMSETUP.bsc" 
BSC32_SBRS= \
	$(INTDIR)/billbrd.sbr \
	$(INTDIR)/inipath.sbr \
	$(INTDIR)/dllmain.sbr \
	$(INTDIR)/detectos.sbr \
	$(INTDIR)/codecins.sbr \
	$(INTDIR)/delpmitm.sbr \
	$(INTDIR)/detreg.sbr \
	$(INTDIR)/ownrdraw.sbr \
	$(INTDIR)/getshort.sbr \
	$(INTDIR)/uihook.sbr \
	$(INTDIR)/uipath.sbr \
	$(INTDIR)/getparnt.sbr \
	$(INTDIR)/mminstal.sbr \
	$(INTDIR)/filecopy.sbr \
	$(INTDIR)/detvideo.sbr \
	$(INTDIR)/delreg.sbr \
	$(INTDIR)/regpath.sbr \
	$(INTDIR)/parseca.sbr \
	$(INTDIR)/sampcacb.sbr \
	$(INTDIR)/sampcah.sbr

$(OUTDIR)/3DMSETUP.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386 /OUT:"..\u\3DMSETUP.DLL"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"appsetup.pdb"\
 /MAP:$(INTDIR)/"appsetup.map" /MACHINE:I386 /DEF:".\src\appsetup.def"\
 /OUT:"..\u\3DMSETUP.DLL" /IMPLIB:$(OUTDIR)/"appsetup.lib" 
DEF_FILE=.\src\appsetup.def
LINK32_OBJS= \
	$(INTDIR)/billbrd.obj \
	$(INTDIR)/inipath.obj \
	$(INTDIR)/dllmain.obj \
	$(INTDIR)/detectos.obj \
	$(INTDIR)/codecins.obj \
	$(INTDIR)/delpmitm.obj \
	$(INTDIR)/detreg.obj \
	$(INTDIR)/ownrdraw.obj \
	$(INTDIR)/getshort.obj \
	$(INTDIR)/uihook.obj \
	$(INTDIR)/uipath.obj \
	$(INTDIR)/getparnt.obj \
	$(INTDIR)/mminstal.obj \
	$(INTDIR)/filecopy.obj \
	$(INTDIR)/appsetup.res \
	$(INTDIR)/detvideo.obj \
	$(INTDIR)/delreg.obj \
	$(INTDIR)/regpath.obj \
	$(INTDIR)/parseca.obj \
	$(INTDIR)/sampcacb.obj \
	$(INTDIR)/sampcah.obj

\soc\setup\u\3DMSETUP.DLL : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\wind"
# PROP Intermediate_Dir "obj\wind"
OUTDIR=.\obj\wind
INTDIR=.\obj\wind

ALL : \soc\setup\ud\3DMSETUP.DLL $(OUTDIR)/appsetup.map $(OUTDIR)/3DMSETUP.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /I "..\tools\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /I "..\tools\inc" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"appsetup.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"appsetup.pdb" /c 
CPP_OBJS=.\obj\wind/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "res" /i "inc" /d "_DEBUG" /d LANG='U'
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /d "_DEBUG" /d\
 LANG='U' 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"obj\wind/3DMSETUP.bsc"
BSC32_FLAGS=/nologo /o"obj\wind/3DMSETUP.bsc" 
BSC32_SBRS= \
	$(INTDIR)/billbrd.sbr \
	$(INTDIR)/inipath.sbr \
	$(INTDIR)/dllmain.sbr \
	$(INTDIR)/detectos.sbr \
	$(INTDIR)/codecins.sbr \
	$(INTDIR)/delpmitm.sbr \
	$(INTDIR)/detreg.sbr \
	$(INTDIR)/ownrdraw.sbr \
	$(INTDIR)/getshort.sbr \
	$(INTDIR)/uihook.sbr \
	$(INTDIR)/uipath.sbr \
	$(INTDIR)/getparnt.sbr \
	$(INTDIR)/mminstal.sbr \
	$(INTDIR)/filecopy.sbr \
	$(INTDIR)/detvideo.sbr \
	$(INTDIR)/delreg.sbr \
	$(INTDIR)/regpath.sbr \
	$(INTDIR)/parseca.sbr \
	$(INTDIR)/sampcacb.sbr \
	$(INTDIR)/sampcah.sbr

$(OUTDIR)/3DMSETUP.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /DEBUG /MACHINE:I386 /OUT:"..\ud\3DMSETUP.DLL"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /PDB:$(OUTDIR)/"appsetup.pdb"\
 /MAP:$(INTDIR)/"appsetup.map" /DEBUG /MACHINE:I386 /DEF:".\src\appsetup.def"\
 /OUT:"..\ud\3DMSETUP.DLL" /IMPLIB:$(OUTDIR)/"appsetup.lib" 
DEF_FILE=.\src\appsetup.def
LINK32_OBJS= \
	$(INTDIR)/billbrd.obj \
	$(INTDIR)/inipath.obj \
	$(INTDIR)/dllmain.obj \
	$(INTDIR)/detectos.obj \
	$(INTDIR)/codecins.obj \
	$(INTDIR)/delpmitm.obj \
	$(INTDIR)/detreg.obj \
	$(INTDIR)/ownrdraw.obj \
	$(INTDIR)/getshort.obj \
	$(INTDIR)/uihook.obj \
	$(INTDIR)/uipath.obj \
	$(INTDIR)/getparnt.obj \
	$(INTDIR)/mminstal.obj \
	$(INTDIR)/filecopy.obj \
	$(INTDIR)/appsetup.res \
	$(INTDIR)/detvideo.obj \
	$(INTDIR)/delreg.obj \
	$(INTDIR)/regpath.obj \
	$(INTDIR)/parseca.obj \
	$(INTDIR)/sampcacb.obj \
	$(INTDIR)/sampcah.obj

\soc\setup\ud\3DMSETUP.DLL : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 D App Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_D_"
# PROP BASE Intermediate_Dir "Win32_D_"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\wins"
# PROP Intermediate_Dir "obj\wins"
OUTDIR=.\obj\wins
INTDIR=.\obj\wins

ALL : \soc\setup\d\3DMSETUP.DLL $(OUTDIR)/appsetup.map $(OUTDIR)/3DMSETUP.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /I "\sendak32\tools\acme95\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"appsetup.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\obj\wins/
# ADD BASE RSC /l 0x409 /i "\sendak32\setup\custdll\res" /i "\sendak32\setup\custdll\inc" /d "NDEBUG" /d "NATURE" /d LANG='U'
# ADD RSC /l 0x407 /i "res" /i "inc" /d "NDEBUG" /d LANG='D'
RSC_PROJ=/l 0x407 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /d "NDEBUG" /d\
 LANG='D' 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"WinDebug/EXPSETUP.bsc"
# ADD BSC32 /nologo /o"obj\wins/3DMSETUP.bsc"
BSC32_FLAGS=/nologo /o"obj\wins/3DMSETUP.bsc" 
BSC32_SBRS= \
	$(INTDIR)/billbrd.sbr \
	$(INTDIR)/inipath.sbr \
	$(INTDIR)/dllmain.sbr \
	$(INTDIR)/detectos.sbr \
	$(INTDIR)/codecins.sbr \
	$(INTDIR)/delpmitm.sbr \
	$(INTDIR)/detreg.sbr \
	$(INTDIR)/ownrdraw.sbr \
	$(INTDIR)/getshort.sbr \
	$(INTDIR)/uihook.sbr \
	$(INTDIR)/uipath.sbr \
	$(INTDIR)/getparnt.sbr \
	$(INTDIR)/mminstal.sbr \
	$(INTDIR)/filecopy.sbr \
	$(INTDIR)/detvideo.sbr \
	$(INTDIR)/delreg.sbr \
	$(INTDIR)/regpath.sbr \
	$(INTDIR)/parseca.sbr \
	$(INTDIR)/sampcacb.sbr \
	$(INTDIR)/sampcah.sbr

$(OUTDIR)/3DMSETUP.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386 /OUT:"..\release\u\nature\EXPSETUP.DLL"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386 /OUT:"..\d\3DMSETUP.DLL"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"appsetup.pdb"\
 /MAP:$(INTDIR)/"appsetup.map" /MACHINE:I386 /DEF:".\src\appsetup.def"\
 /OUT:"..\d\3DMSETUP.DLL" /IMPLIB:$(OUTDIR)/"appsetup.lib" 
DEF_FILE=.\src\appsetup.def
LINK32_OBJS= \
	$(INTDIR)/billbrd.obj \
	$(INTDIR)/inipath.obj \
	$(INTDIR)/dllmain.obj \
	$(INTDIR)/detectos.obj \
	$(INTDIR)/codecins.obj \
	$(INTDIR)/delpmitm.obj \
	$(INTDIR)/detreg.obj \
	$(INTDIR)/ownrdraw.obj \
	$(INTDIR)/getshort.obj \
	$(INTDIR)/uihook.obj \
	$(INTDIR)/uipath.obj \
	$(INTDIR)/getparnt.obj \
	$(INTDIR)/mminstal.obj \
	$(INTDIR)/filecopy.obj \
	$(INTDIR)/appsetup.res \
	$(INTDIR)/detvideo.obj \
	$(INTDIR)/delreg.obj \
	$(INTDIR)/regpath.obj \
	$(INTDIR)/parseca.obj \
	$(INTDIR)/sampcacb.obj \
	$(INTDIR)/sampcah.obj

\soc\setup\d\3DMSETUP.DLL : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Z_"
# PROP BASE Intermediate_Dir "Win32_Z_"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\wins"
# PROP Intermediate_Dir "obj\wins"
OUTDIR=.\obj\wins
INTDIR=.\obj\wins

ALL : \soc\setup\z\3DMSETUP.DLL $(OUTDIR)/appsetup.map $(OUTDIR)/3DMSETUP.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /I "\sendak32\tools\acme95\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "..\tools\inc" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"appsetup.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\obj\wins/
# ADD BASE RSC /l 0x409 /i "\sendak32\setup\custdll\res" /i "\sendak32\setup\custdll\inc" /d "NDEBUG" /d "NATURE" /d LANG='U'
# ADD RSC /l 0x809 /i "res" /i "inc" /d "NDEBUG" /d LANG='Z'
RSC_PROJ=/l 0x809 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /d "NDEBUG" /d\
 LANG='Z' 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"WinDebug/EXPSETUP.bsc"
# ADD BSC32 /nologo /o"obj\wins\3DMSETUP.bsc"
BSC32_FLAGS=/nologo /o"obj\wins\3DMSETUP.bsc" 
BSC32_SBRS= \
	$(INTDIR)/billbrd.sbr \
	$(INTDIR)/inipath.sbr \
	$(INTDIR)/dllmain.sbr \
	$(INTDIR)/detectos.sbr \
	$(INTDIR)/codecins.sbr \
	$(INTDIR)/delpmitm.sbr \
	$(INTDIR)/detreg.sbr \
	$(INTDIR)/ownrdraw.sbr \
	$(INTDIR)/getshort.sbr \
	$(INTDIR)/uihook.sbr \
	$(INTDIR)/uipath.sbr \
	$(INTDIR)/getparnt.sbr \
	$(INTDIR)/mminstal.sbr \
	$(INTDIR)/filecopy.sbr \
	$(INTDIR)/detvideo.sbr \
	$(INTDIR)/delreg.sbr \
	$(INTDIR)/regpath.sbr \
	$(INTDIR)/parseca.sbr \
	$(INTDIR)/sampcacb.sbr \
	$(INTDIR)/sampcah.sbr

$(OUTDIR)/3DMSETUP.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386 /OUT:"..\release\u\nature\EXPSETUP.DLL"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib /NOLOGO /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386 /OUT:"..\z\3DMSETUP.DLL"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib mpr.lib msacm32.lib vfw32.lib winmm.lib ..\tools\inc\mssetup.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"appsetup.pdb"\
 /MAP:$(INTDIR)/"appsetup.map" /MACHINE:I386 /DEF:".\src\appsetup.def"\
 /OUT:"..\z\3DMSETUP.DLL" /IMPLIB:$(OUTDIR)/"appsetup.lib" 
DEF_FILE=.\src\appsetup.def
LINK32_OBJS= \
	$(INTDIR)/billbrd.obj \
	$(INTDIR)/inipath.obj \
	$(INTDIR)/dllmain.obj \
	$(INTDIR)/detectos.obj \
	$(INTDIR)/codecins.obj \
	$(INTDIR)/delpmitm.obj \
	$(INTDIR)/detreg.obj \
	$(INTDIR)/ownrdraw.obj \
	$(INTDIR)/getshort.obj \
	$(INTDIR)/uihook.obj \
	$(INTDIR)/uipath.obj \
	$(INTDIR)/getparnt.obj \
	$(INTDIR)/mminstal.obj \
	$(INTDIR)/filecopy.obj \
	$(INTDIR)/appsetup.res \
	$(INTDIR)/detvideo.obj \
	$(INTDIR)/delreg.obj \
	$(INTDIR)/regpath.obj \
	$(INTDIR)/parseca.obj \
	$(INTDIR)/sampcacb.obj \
	$(INTDIR)/sampcah.obj

\soc\setup\z\3DMSETUP.DLL : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\src\billbrd.c
DEP_BILLB=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\stdassrt.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\billbrd.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/billbrd.obj :  $(SOURCE)  $(DEP_BILLB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/billbrd.obj :  $(SOURCE)  $(DEP_BILLB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/billbrd.obj :  $(SOURCE)  $(DEP_BILLB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/billbrd.obj :  $(SOURCE)  $(DEP_BILLB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\inipath.c
DEP_INIPA=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/inipath.obj :  $(SOURCE)  $(DEP_INIPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/inipath.obj :  $(SOURCE)  $(DEP_INIPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/inipath.obj :  $(SOURCE)  $(DEP_INIPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/inipath.obj :  $(SOURCE)  $(DEP_INIPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\dllmain.c
DEP_DLLMA=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	.\src\dialogs.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\billbrd.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/dllmain.obj :  $(SOURCE)  $(DEP_DLLMA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/dllmain.obj :  $(SOURCE)  $(DEP_DLLMA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/dllmain.obj :  $(SOURCE)  $(DEP_DLLMA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/dllmain.obj :  $(SOURCE)  $(DEP_DLLMA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\detectos.c
DEP_DETEC=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/detectos.obj :  $(SOURCE)  $(DEP_DETEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/detectos.obj :  $(SOURCE)  $(DEP_DETEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/detectos.obj :  $(SOURCE)  $(DEP_DETEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/detectos.obj :  $(SOURCE)  $(DEP_DETEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\codecins.c
DEP_CODEC=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\mminstal.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/codecins.obj :  $(SOURCE)  $(DEP_CODEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/codecins.obj :  $(SOURCE)  $(DEP_CODEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/codecins.obj :  $(SOURCE)  $(DEP_CODEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/codecins.obj :  $(SOURCE)  $(DEP_CODEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\delpmitm.c
DEP_DELPM=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/delpmitm.obj :  $(SOURCE)  $(DEP_DELPM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/delpmitm.obj :  $(SOURCE)  $(DEP_DELPM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/delpmitm.obj :  $(SOURCE)  $(DEP_DELPM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/delpmitm.obj :  $(SOURCE)  $(DEP_DELPM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\detreg.c
DEP_DETRE=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/detreg.obj :  $(SOURCE)  $(DEP_DETRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/detreg.obj :  $(SOURCE)  $(DEP_DETRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/detreg.obj :  $(SOURCE)  $(DEP_DETRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/detreg.obj :  $(SOURCE)  $(DEP_DETRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\ownrdraw.cpp
DEP_OWNRD=\
	".\src\ui-hook.h"\
	\soc\setup\tools\inc\ACMHOOK.H\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	.\src\ownrdraw.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/ownrdraw.obj :  $(SOURCE)  $(DEP_OWNRD) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/ownrdraw.obj :  $(SOURCE)  $(DEP_OWNRD) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/ownrdraw.obj :  $(SOURCE)  $(DEP_OWNRD) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/ownrdraw.obj :  $(SOURCE)  $(DEP_OWNRD) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\getshort.c
DEP_GETSH=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/getshort.obj :  $(SOURCE)  $(DEP_GETSH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/getshort.obj :  $(SOURCE)  $(DEP_GETSH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/getshort.obj :  $(SOURCE)  $(DEP_GETSH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/getshort.obj :  $(SOURCE)  $(DEP_GETSH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\uihook.cpp
DEP_UIHOO=\
	".\src\ui-hook.h"\
	\soc\setup\tools\inc\ACMHOOK.H\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	.\src\ownrdraw.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/uihook.obj :  $(SOURCE)  $(DEP_UIHOO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/uihook.obj :  $(SOURCE)  $(DEP_UIHOO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/uihook.obj :  $(SOURCE)  $(DEP_UIHOO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/uihook.obj :  $(SOURCE)  $(DEP_UIHOO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\uipath.cpp
DEP_UIPAT=\
	".\src\ui-hook.h"\
	\soc\setup\tools\inc\ACMHOOK.H\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	.\src\ownrdraw.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/uipath.obj :  $(SOURCE)  $(DEP_UIPAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/uipath.obj :  $(SOURCE)  $(DEP_UIPAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/uipath.obj :  $(SOURCE)  $(DEP_UIPAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/uipath.obj :  $(SOURCE)  $(DEP_UIPAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\getparnt.c
DEP_GETPA=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/getparnt.obj :  $(SOURCE)  $(DEP_GETPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/getparnt.obj :  $(SOURCE)  $(DEP_GETPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/getparnt.obj :  $(SOURCE)  $(DEP_GETPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/getparnt.obj :  $(SOURCE)  $(DEP_GETPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\mminstal.cpp
DEP_MMINS=\
	.\src\mminstal.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/mminstal.obj :  $(SOURCE)  $(DEP_MMINS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/mminstal.obj :  $(SOURCE)  $(DEP_MMINS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/mminstal.obj :  $(SOURCE)  $(DEP_MMINS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/mminstal.obj :  $(SOURCE)  $(DEP_MMINS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\filecopy.c
DEP_FILEC=\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/filecopy.obj :  $(SOURCE)  $(DEP_FILEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/filecopy.obj :  $(SOURCE)  $(DEP_FILEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/filecopy.obj :  $(SOURCE)  $(DEP_FILEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/filecopy.obj :  $(SOURCE)  $(DEP_FILEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\appsetup.rc
DEP_APPSE=\
	.\src\bullet.dib\
	.\src\dirdrive.bmp\
	.\src\appbutt.dib\
	.\src\APP.ICO\
	.\src\dialogs.h\
	.\src\appsetu.rc\
	.\src\LOGOVGA.DIB\
	.\src\bb1.dib\
	.\src\bb2.dib\
	.\src\appsetd.rc

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/appsetup.res :  $(SOURCE)  $(DEP_APPSE) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /i "src" /d\
 "NDEBUG" /d LANG='U'  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/appsetup.res :  $(SOURCE)  $(DEP_APPSE) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /i "src" /d\
 "_DEBUG" /d LANG='U'  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/appsetup.res :  $(SOURCE)  $(DEP_APPSE) $(INTDIR)
   $(RSC) /l 0x407 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /i "src" /d\
 "NDEBUG" /d LANG='D'  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/appsetup.res :  $(SOURCE)  $(DEP_APPSE) $(INTDIR)
   $(RSC) /l 0x809 /fo$(INTDIR)/"appsetup.res" /i "res" /i "inc" /i "src" /d\
 "NDEBUG" /d LANG='Z'  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\appsetup.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\detvideo.c
DEP_DETVI=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\detvideo.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/detvideo.obj :  $(SOURCE)  $(DEP_DETVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/detvideo.obj :  $(SOURCE)  $(DEP_DETVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/detvideo.obj :  $(SOURCE)  $(DEP_DETVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/detvideo.obj :  $(SOURCE)  $(DEP_DETVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\delreg.c
DEP_DELRE=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/delreg.obj :  $(SOURCE)  $(DEP_DELRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/delreg.obj :  $(SOURCE)  $(DEP_DELRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/delreg.obj :  $(SOURCE)  $(DEP_DELRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/delreg.obj :  $(SOURCE)  $(DEP_DELRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\regpath.c
DEP_REGPA=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\parseca.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/regpath.obj :  $(SOURCE)  $(DEP_REGPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/regpath.obj :  $(SOURCE)  $(DEP_REGPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/regpath.obj :  $(SOURCE)  $(DEP_REGPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/regpath.obj :  $(SOURCE)  $(DEP_REGPA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\parseca.c
DEP_PARSE=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\parseca.h\
	.\src\sampcacb.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/parseca.obj :  $(SOURCE)  $(DEP_PARSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/parseca.obj :  $(SOURCE)  $(DEP_PARSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/parseca.obj :  $(SOURCE)  $(DEP_PARSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/parseca.obj :  $(SOURCE)  $(DEP_PARSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\sampcacb.c
DEP_SAMPC=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\datadef.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/sampcacb.obj :  $(SOURCE)  $(DEP_SAMPC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/sampcacb.obj :  $(SOURCE)  $(DEP_SAMPC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/sampcacb.obj :  $(SOURCE)  $(DEP_SAMPC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/sampcacb.obj :  $(SOURCE)  $(DEP_SAMPC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\sampcah.c
DEP_SAMPCA=\
	.\src\stdinc.h\
	\soc\setup\tools\inc\setupapi.h\
	\soc\setup\tools\inc\cui.h\
	\soc\setup\tools\inc\stdtypes.h\
	\soc\setup\tools\inc\setupkit.h\
	\soc\setup\tools\inc\datadef.h\
	.\src\parseca.h\
	.\src\sampcacb.h

!IF  "$(CFG)" == "Win32 U App Release"

$(INTDIR)/sampcah.obj :  $(SOURCE)  $(DEP_SAMPCA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 U App Debug"

$(INTDIR)/sampcah.obj :  $(SOURCE)  $(DEP_SAMPCA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 D App Release"

$(INTDIR)/sampcah.obj :  $(SOURCE)  $(DEP_SAMPCA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Z App Release"

$(INTDIR)/sampcah.obj :  $(SOURCE)  $(DEP_SAMPCA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
