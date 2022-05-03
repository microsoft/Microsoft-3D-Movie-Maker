#---kauai.mak

!IFNDEF MAKEFILE_KAUAI
MAKEFILE_KAUAI = 1

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
!ENDIF # !LOCAL_BUILD


KAUAI_TARGETS =\
    $(BASE_OBJS)\
    $(GROUP_OBJS)\
    $(FILE_OBJS)\
    $(STREAM_OBJS)\
    $(LEXER_OBJS)\
    $(SCRCOM_OBJS)\
    $(KIDCOM_OBJS)\
    $(SCREXE_OBJS)\
    $(MBMPIO_OBJS)\
    $(CHSE_OBJS)\
    $(CHCM_OBJS)\
    $(SOUND_OBJS)\
    $(VIDEO_OBJS)\
    $(GUI_OBJS)\
    $(DLG_OBJS)\
    $(CTL_OBJS)\
    $(DOC_OBJS)\
    $(RICHTEXT_OBJS)\
    $(TEXTEDIT_OBJS)\
    $(PLAINTEXT_OBJS)\
    $(KID_OBJS)



UT_TARGETS =\
    $(BASE_OBJS)\
    $(GROUP_OBJS)\
    $(FILE_OBJS)\
    $(TARGET_DIR)ut.obj\
    $(TARGET_DIR)test.obj\


FT_TARGETS =\
    $(BASE_OBJS)\
    $(GROUP_OBJS)\
    $(FILE_OBJS)\
    $(GUI_OBJS)\
    $(DOC_OBJS)\
    $(STREAM_OBJS)\
    $(SOUND_OBJS)\
    $(DLG_OBJS)\
    $(CTL_OBJS)\
    $(TEXTEDIT_OBJS)\
    $(RICHTEXT_OBJS)\
    $(MBMPIO_OBJS)\
	$(KAUAITEST_OBJS)\
    $(TARGET_DIR)ft.res



#-Compile rules-------------------------------------------------------------


PROGRAM_DATABASE = $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\kauai.pdb
PRECOMPILED_HEADER = $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\kauai.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

SRC_DIR=$(KAUAI_SRC_DIR)
OBJ_DIR=$(KAUAI_OBJ_DIR)
TGT_NAME=Kauai

!INCLUDE $(KAUAI_ROOT)\makefile.rul

# REVIEW a-aarob (peted): why should Kauai src know how to build Kauai tools?
#
# SRC_DIR=$(KAUAI_TOOLS_DIR)
# OBJ_DIR=$(KAUAI_OBJ_DIR)
# TGT_NAME=Kauai Tools
#
# !INCLUDE $(KAUAI_ROOT)\makefile.rul



#-Targets-------------------------------------------------------------------

ALL_KAUAI = $(KAUAI_TARGETS) $(TARGET_DIR)FT.EXE $(TARGET_DIR)UT.EXE
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_KAUAI)

CLEAN_KAUAI = CLEAN_KAUAI_OBJ CLEAN_UT CLEAN_FT
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_KAUAI)


!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0" && !DEFINED(MAKEFILE_KAUAI_TOOLS)
ALL: $(KAUAI_OBJ_DIR) $(ALL_KAUAI)
CLEAN : $(CLEAN_KAUAI) ALL
!ENDIF  # LOCAL_BUILD != 0

KAUAI_OBJS : $(KAUAI_OBJ_DIR) $(KAUAI_TARGETS)

$(KAUAI_OBJ_DIR) :
    @echo Making Directories $(KAUAI_OBJ_DIR)...
    if not exist $(KAUAI_OBJ_ROOT_DIR)/nul mkdir $(KAUAI_OBJ_ROOT_DIR)
    if not exist $(KAUAI_OBJ_DIR)/nul mkdir $(KAUAI_OBJ_DIR)

!ELSE # !LOCAL_BUILD

KAUAI_OBJS : $(KAUAI_TARGETS)

!ENDIF # LOCAL_BUILD


$(KAUAI_OBJ_DIR)\codkauai.obj: $(KAUAI_SRC_DIR)\kcdc_386.h $(KAUAI_SRC_DIR)\kcd2_386.h

$(KAUAI_SRC_DIR)\kcdc_386.h: $(TARGET_DIR)kcdc_386.exe
	$(TARGET_DIR)kcdc_386.exe 1> $(KAUAI_SRC_DIR)\kcdc_386.h

$(TARGET_DIR)kcdc_386.exe: $(KAUAI_SRC_DIR)\kcdc_386.c
	cl $(KAUAI_SRC_DIR)\kcdc_386.c /Fe$(TARGET_DIR)kcdc_386.exe /Fo$(TARGET_DIR)kcdc_386.obj

$(KAUAI_SRC_DIR)\kcd2_386.h: $(TARGET_DIR)kcd2_386.exe
	$(TARGET_DIR)kcd2_386.exe 1> $(KAUAI_SRC_DIR)\kcd2_386.h

$(TARGET_DIR)kcd2_386.exe: $(KAUAI_SRC_DIR)\kcd2_386.c
	cl $(KAUAI_SRC_DIR)\kcd2_386.c /Fe$(TARGET_DIR)kcd2_386.exe /Fo$(TARGET_DIR)kcd2_386.obj


CLEAN_KAUAI_OBJ:
    @echo <<delkauai.bat
@echo off
rm -f $(KAUAI_TARGETS: =^
rm -f )
<<KEEP
    cmd /c delkauai.bat
    del delkauai.bat


CLEAN_UT:
    @echo <<delut.bat
@echo off
rm -f $(UT_TARGETS: =^
rm -f )
rm -f $(TARGET_DIR)ut.obj
rm -f $(TARGET_DIR)test.obj
<<KEEP
    cmd /c delut.bat
    del delut.bat


CLEAN_FT:
    @echo <<delft.bat
@echo off
rm -f $(FT_TARGETS: =^
rm -f )
rm -f $(TARGET_DIR)ft.obj
rm -f $(TARGET_DIR)test.obj
rm -f $(TARGET_DIR)ft.res
<<KEEP
    cmd /c delft.bat
    del delft.bat




!IF "$(TARGET_DIR)" != ""

UT : $(TARGET_DIR)ut.exe
UT.EXE : $(TARGET_DIR)ut.exe
$(TARGET_DIR)ut.exe: $(KAUAI_OBJ_DIR)
FT : $(TARGET_DIR)ft.exe
FT.EXE : $(TARGET_DIR)ft.exe
$(TARGET_DIR)ft.exe: $(KAUAI_OBJ_DIR)

!ENDIF # TARGET_DIR


$(TARGET_DIR)ut.lnk : $(KAUAI_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)ut.lnk
$(UT_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)ut.exe : $(UT_TARGETS) $(TARGET_DIR)ut.lnk
    @echo Linking UT Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    -out:$(TARGET_DIR)ut.exe @$(TARGET_DIR)ut.lnk
    $(CHKERR)




$(TARGET_DIR)ft.lnk : $(KAUAI_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)ft.lnk
$(FT_TARGETS: =^
)
<<KEEP

$(TARGET_DIR)ft.exe : $(FT_TARGETS) $(TARGET_DIR)ft.lnk $(AUDIO_LIB)
    @echo Linking FT Objects...
    $(LINK) -link $(LFLAGS_GUI) \
    $(VFW32_LIB) \
    $(AUDIO_LIB) \
    -out:$(TARGET_DIR)ft.exe @$(TARGET_DIR)ft.lnk
    $(CHKERR)

!ENDIF  # !MAKEFILE_KAUAI
