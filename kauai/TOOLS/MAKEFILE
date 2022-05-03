#---tools.mak
#
# Builds Ched, Chelp, and Chomp

!IFNDEF MAKEFILE_KAUAI_TOOLS
MAKEFILE_KAUAI_TOOLS = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#KAUAI SOURCE DIRECTORIES
KAUAI_SRC_DIR = $(KAUAI_ROOT)\src
KAUAI_TOOLS_DIR = $(KAUAI_ROOT)\tools

#KAUAI OBJ DIRECTORIES
KAUAI_OBJ_ROOT_DIR = $(KAUAI_ROOT)\obj
KAUAI_OBJ_DIR = $(KAUAI_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(KAUAI_OBJ_DIR)^\
!ENDIF

CHED_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GUI_OBJS)\
    $(KID_OBJS)\
    $(KIDCOM_OBJS)\
    $(PLAINTEXT_OBJS)\
    $(DOC_OBJS)\
    $(CHCM_OBJS)\
    $(LEXER_OBJS)\
    $(CHSE_OBJS)\
    $(STREAM_OBJS)\
    $(SCRCOM_OBJS)\
    $(SCREXE_OBJS)\
    $(GROUP_OBJS)\
    $(RICHTEXT_OBJS)\
    $(SOUND_OBJS)\
    $(DLG_OBJS)\
    $(CTL_OBJS)\
    $(VIDEO_OBJS)\
    $(TEXTEDIT_OBJS)\
    $(MBMPIO_OBJS)\
    $(CHED_OBJS)\
    $(TARGET_DIR)ched.res



CHELP_TARGETS =\
    $(BASE_OBJS)\
    $(GUI_OBJS)\
    $(GROUP_OBJS)\
    $(MBMPIO_OBJS)\
    $(SOUND_OBJS)\
    $(VIDEO_OBJS)\
    $(KID_OBJS)\
    $(SCREXE_OBJS)\
    $(RICHTEXT_OBJS)\
    $(DOC_OBJS)\
    $(STREAM_OBJS)\
    $(FILE_OBJS)\
    $(CHSE_OBJS)\
    $(DLG_OBJS)\
    $(SCRCOM_OBJS)\
    $(LEXER_OBJS)\
    $(CTL_OBJS)\
    $(TEXTEDIT_OBJS)\
!IF "$(UNICODE)" == ""
    $(SPELL_OBJS)\
!ENDIF
    $(CHELP_OBJS)\
    $(TARGET_DIR)chelp.res



#Chomp Special Case
#Needs only pic*.cpp from Kauai Gui Objs

PIC_OBJS =\
!IF "$(ARCH)" == "WIN"
    $(TARGET_DIR)picwin.obj\
!ELSEIF "$(ARCH)" == "MAC"
    $(TARGET_DIR)picmac.obj\
!ENDIF
    $(TARGET_DIR)pic.obj


CHOMP_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(CHCM_OBJS)\
    $(CHSE_OBJS)\
    $(GROUP_OBJS)\
    $(LEXER_OBJS)\
    $(STREAM_OBJS)\
    $(KIDCOM_OBJS)\
    $(SCRCOM_OBJS)\
    $(SCREXE_OBJS)\
    $(MBMPIO_OBJS)\
    $(KAUAI_OBJ_DIR)\midi.obj\
    $(PIC_OBJS)\
    $(CHOMP_OBJS)


MKMBMP_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(MBMPIO_OBJS)\
    $(TARGET_DIR)mkmbmp.obj


KPACK_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(TARGET_DIR)kpack.obj


CHMERGE_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(TARGET_DIR)chmerge.obj\


CHELPDMP_TARGETS =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(LEXER_OBJS)\
    $(STREAM_OBJS)\
    $(SCRCOM_OBJS)\
    $(SCREXE_OBJS)\
    $(CHSE_OBJS)\
    $(TARGET_DIR)chelpdmp.obj\
    $(TARGET_DIR)chelpexp.obj\
    $(TARGET_DIR)mssio.obj


#-Compile rules-------------------------------------------------------------


PROGRAM_DATABASE = $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\kauai.pdb
PRECOMPILED_HEADER = $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\kauai.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

!INCLUDE $(KAUAI_ROOT)\src\makefile

SRC_DIR = $(KAUAI_TOOLS_DIR)
OBJ_DIR = $(KAUAI_OBJ_DIR)
TGT_NAME = Kauai Tools

!INCLUDE $(KAUAI_ROOT)\makefile.rul


#-Targets-------------------------------------------------------------------

ALL_KAUAI_TOOLS = $(TARGET_DIR)ched.exe \
                  $(TARGET_DIR)chelp.exe \
                  $(TARGET_DIR)chomp.exe \
                  $(TARGET_DIR)mkmbmp.exe \
                  $(TARGET_DIR)kpack.exe \
                  $(TARGET_DIR)chmerge.exe \
                  $(TARGET_DIR)chelpdmp.exe

ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_KAUAI_TOOLS)

CLEAN_KAUAI_TOOLS = CLEAN_CHED CLEAN_CHELP CLEAN_CHOMP CLEAN_MKMBMP CLEAN_KPACK CLEAN_CHMERGE CLEAN_CHELPDMP
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_KAUAI_TOOLS)


!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"

# Kauai src\makefile knows how to make this dir
# $(KAUAI_OBJ_DIR) :
#   @echo Making Directories $(KAUAI_OBJ_DIR)...
#    if not exist $(KAUAI_OBJ_ROOT_DIR)/nul mkdir $(KAUAI_OBJ_ROOT_DIR)
#    if not exist $(KAUAI_OBJ_DIR)/nul mkdir $(KAUAI_OBJ_DIR)

ALL: $(KAUAI_OBJ_DIR) $(ALL_KAUAI_TOOLS)
CLEAN : RM ALL
RM :
        @echo Cleaning $(KAUAI_OBJ_DIR)\ directory
        @rm -f $(KAUAI_OBJ_DIR)\*.obj 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.pch 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.pdb 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.exe 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.res 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.cod 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.map 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.lib 2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.i   2>nul
        @rm -f $(KAUAI_OBJ_DIR)\*.lnk 2>nul


!ENDIF # LOCAL_BUILD != 0

RMOBJ : $(CLEAN_KAUAI_TOOLS)

!ENDIF  # !LOCAL_BUILD



CLEAN_CHED:
    @echo <<delched.bat
@echo off
rm -f $(CHED_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delched.bat
    del delched.bat


CLEAN_CHELP:
    @echo <<delchelp.bat
@echo off
rm -f $(CHELP_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delchelp.bat
    del delchelp.bat


CLEAN_CHOMP:
    @echo <<delchomp.bat
@echo off
rm -f $(CHOMP_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delchomp.bat
    del delchomp.bat


CLEAN_MKMBMP:
    @echo <<delmkmb.bat
@echo off
rm -f $(MKMBMP_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delmkmb.bat
    del delmkmb.bat


CLEAN_KPACK:
    @echo <<delmkmb.bat
@echo off
rm -f $(KPACK_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delmkmb.bat
    del delmkmb.bat


CLEAN_CHMERGE:
    @echo <<dchmerge.bat
@echo off
rm -f $(CHMERGE_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c dchmerge.bat
    del dchmerge.bat


CLEAN_CHELPDMP:
    @echo <<delchdmp.bat
@echo off
rm -f $(CHELPDMP_TARGETS: = 2>nul^
rm -f ) 2>nul
<<KEEP
    cmd /c delchdmp.bat
    del delchdmp.bat


!IF "$(LOCAL_BUILD)" != "1"

CHED : $(TARGET_DIR)ched.exe
CHED.EXE : $(TARGET_DIR)ched.exe
$(TARGET_DIR)ched.exe: $(KAUAI_OBJ_DIR)

CHELP : $(TARGET_DIR)chelp.exe
CHELP.EXE : $(TARGET_DIR)chelp.exe
$(TARGET_DIR)chelp.exe : $(KAUAI_OBJ_DIR)

CHOMP : $(TARGET_DIR)chomp.exe
CHOMP.EXE : $(TARGET_DIR)chomp.exe
$(TARGET_DIR)chomp.exe : $(KAUAI_OBJ_DIR)

MKMBMP : $(TARGET_DIR)mkmbmp.exe
MKMBMP.EXE : $(TARGET_DIR)mkmbmp.exe
$(TARGET_DIR)mkmbmp.exe : $(KAUAI_OBJ_DIR)

KPACK : $(TARGET_DIR)kpack.exe
KPACK.EXE : $(TARGET_DIR)kpack.exe
$(TARGET_DIR)kpack.exe : $(KAUAI_OBJ_DIR)

CHMERGE : $(TARGET_DIR)chmerge.exe
CHMERGE.EXE : $(TARGET_DIR)chmerge.exe
$(TARGET_DIR)chmerge.exe : $(KAUAI_OBJ_DIR)

CHELPDMP : $(TARGET_DIR)chelpdmp.exe
CHELPDMP.EXE : $(TARGET_DIR)chelpdmp.exe
$(TARGET_DIR)chelpdmp.exe : $(KAUAI_OBJ_DIR)

!ENDIF  # !LOCAL_BUILD


$(TARGET_DIR)ched.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)ched.lnk
$(CHED_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)ched.exe : $(CHED_TARGETS) $(TARGET_DIR)ched.lnk $(AUDIO_LIB)
    @echo Linking Ched Objects...
    $(LINK) -link $(LFLAGS_GUI) \
    $(VFW32_LIB) \
    $(AUDIO_LIB) \
     -out:$(TARGET_DIR)ched.exe @$(TARGET_DIR)ched.lnk
    $(CHKERR)



$(TARGET_DIR)chelp.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)chelp.lnk
$(CHELP_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)chelp.exe : $(CHELP_TARGETS) $(TARGET_DIR)chelp.lnk $(AUDIO_LIB)
    @echo Linking Chelp Objects...
    $(LINK) -link $(LFLAGS_GUI) \
    $(VFW32_LIB) \
    $(AUDIO_LIB)\
    -out:$(TARGET_DIR)chelp.exe @$(TARGET_DIR)chelp.lnk
    $(CHKERR)



$(TARGET_DIR)chomp.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)chomp.lnk
$(CHOMP_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)chomp.exe : $(CHOMP_TARGETS) $(TARGET_DIR)chomp.lnk
    @echo Linking Chomp Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)chomp.exe @$(TARGET_DIR)chomp.lnk
    $(CHKERR)



$(TARGET_DIR)mkmbmp.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)mkmbmp.lnk
$(MKMBMP_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)mkmbmp.exe : $(MKMBMP_TARGETS) $(TARGET_DIR)mkmbmp.lnk
    @echo Linking Mkmbmp Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)mkmbmp.exe @$(TARGET_DIR)mkmbmp.lnk
    $(CHKERR)


$(TARGET_DIR)kpack.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)kpack.lnk
$(KPACK_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)kpack.exe : $(KPACK_TARGETS) $(TARGET_DIR)kpack.lnk
    @echo Linking KPack Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)kpack.exe @$(TARGET_DIR)kpack.lnk
    $(CHKERR)


$(TARGET_DIR)chmerge.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)chmerge.lnk
$(CHMERGE_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)chmerge.exe : $(CHMERGE_TARGETS) $(TARGET_DIR)chmerge.lnk
    @echo Linking Chmerge Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)chmerge.exe @$(TARGET_DIR)chmerge.lnk
    $(CHKERR)


$(TARGET_DIR)chelpdmp.lnk : $(KAUAI_TOOLS_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)chelpdmp.lnk
$(CHELPDMP_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)chelpdmp.exe : $(CHELPDMP_TARGETS) $(TARGET_DIR)chelpdmp.lnk
    @echo Linking ChelpDmp Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)chelpdmp.exe @$(TARGET_DIR)chelpdmp.lnk
    $(CHKERR)


!ENDIF  # !MAKEFILE_KAUAI_TOOLS
