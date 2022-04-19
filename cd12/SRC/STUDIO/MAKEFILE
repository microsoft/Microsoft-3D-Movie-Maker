#---studio.mak

!IFNDEF MAKEFILE_STUDIO
MAKEFILE_STUDIO = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#STUDIO SOURCE DIRECTORY
STUDIO_SRC_DIR = $(SOC_ROOT)\src\studio

#SOC OBJ DIRECTORY
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
CHT_BUILD_INC = -I.
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
CHT_BUILD_INC = -I$(SOC_OBJ_DIR)
!ENDIF # !LOCAL_BUILD

BREN_LIB =  $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\bren.lib\
	    $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfmmxr.lib\
	    $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfwmxr.lib\
	    $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brzbmxr.lib



UTEST_TARGETS =\
    $(TARGET_DIR)splot.obj\
    $(TARGET_DIR)scnsort.obj\
    $(TARGET_DIR)browser.obj\
    $(TARGET_DIR)utest.obj\
    $(TARGET_DIR)stdioscb.obj\
    $(TARGET_DIR)stdiobrw.obj\
    $(TARGET_DIR)studio.obj\
    $(TARGET_DIR)tgob.obj\
    $(TARGET_DIR)esl.obj\
    $(TARGET_DIR)ape.obj\
    $(TARGET_DIR)popup.obj\
    $(TARGET_DIR)tatr.obj\
    $(TARGET_DIR)portf.obj\
    $(TARGET_DIR)mminstal.obj\
    $(TARGET_DIR)utest.res

UTEST_DEPS =\
    $(WNET_LIB) \
    $(VFW32_LIB) \
    $(AUDIO_LIB) \
    $(BREN_LIB) \
    $(TARGET_DIR)engine.lib

UTEST_CHT_FILES = \
    $(STUDIO_SRC_DIR)\utestgst.cht\
    $(STUDIO_SRC_DIR)\utest.cht

STUDIO_CHT_FILES = \
    $(STUDIO_SRC_DIR)\browser.cht\
    $(STUDIO_SRC_DIR)\cursors.cht\
    $(STUDIO_SRC_DIR)\easels.cht\
    $(STUDIO_SRC_DIR)\helpbook.cht\
    $(STUDIO_SRC_DIR)\hlpbkper.cht\
    $(STUDIO_SRC_DIR)\gadget.cht\
    $(STUDIO_SRC_DIR)\gadgpers.cht\
    $(STUDIO_SRC_DIR)\helpres.cht\
    $(STUDIO_SRC_DIR)\helpctrl.cht\
    $(STUDIO_SRC_DIR)\projmain.cht\
    $(STUDIO_SRC_DIR)\popups.cht\
    $(STUDIO_SRC_DIR)\popdn.cht\
    $(STUDIO_SRC_DIR)\sectools.cht\
    $(STUDIO_SRC_DIR)\stdioscb.cht\
    $(STUDIO_SRC_DIR)\stdiosnd.cht\
    $(STUDIO_SRC_DIR)\tbguidhp.cht\
    $(STUDIO_SRC_DIR)\tbhtscen.cht\
    $(STUDIO_SRC_DIR)\tbhtactr.cht\
    $(STUDIO_SRC_DIR)\tbhtsoun.cht\
    $(STUDIO_SRC_DIR)\tbhtword.cht\
    $(STUDIO_SRC_DIR)\tblogowz.cht\
    $(STUDIO_SRC_DIR)\tbbasics.cht\
    $(STUDIO_SRC_DIR)\tbproj1.cht\
    $(STUDIO_SRC_DIR)\tbproj2.cht\
    $(STUDIO_SRC_DIR)\tbproj3.cht\
    $(STUDIO_SRC_DIR)\tbproj4.cht\
    $(STUDIO_SRC_DIR)\tbproj5.cht\
    $(STUDIO_SRC_DIR)\tbproj6.cht\
    $(STUDIO_SRC_DIR)\studio.cht



STUDIO_CHUNK_TARGETS =\
    $(TARGET_DIR)studio.chk



KAUAI_OBJ_GROUPS_FOR_UTEST =\
    $(BASE_OBJS)\
    $(GUI_OBJS)\
    $(GROUP_OBJS)\
    $(FILE_OBJS)\
    $(MBMPIO_OBJS)\
    $(SOUND_OBJS)\
    $(VIDEO_OBJS)\
    $(SCREXE_OBJS)\
    $(DOC_OBJS)\
    $(RICHTEXT_OBJS)\
    $(CTL_OBJS)\
    $(STREAM_OBJS)\
    $(DLG_OBJS)\
    $(TEXTEDIT_OBJS)\
    $(KID_OBJS)\
    $(SCRCOM_OBJS)\
    $(LEXER_OBJS)

TBOXTHD_DEPS = \
    $(STUDIO_SRC_DIR)\tboxclr.cht \
    $(STUDIO_SRC_DIR)\tboxsize.cht

#-Compile rules-------------------------------------------------------------


PROGRAM_DATABASE = $(TARGET_DIR)soc.pdb
PRECOMPILED_HEADER = $(TARGET_DIR)soc.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

SRC_DIR=$(STUDIO_SRC_DIR)
OBJ_DIR=$(SOC_OBJ_DIR)
TGT_NAME=Studio

!INCLUDE $(SOC_ROOT)\makefile.rul


#-Targets-------------------------------------------------------------------

ALL_STUDIO = $(TARGET_DIR)3dmovie.exe $(TARGET_DIR)3dmovie.chk STUDIO_CONTENT
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_STUDIO)

CLEAN_STUDIO = CLEAN_UTEST CLEAN_STUDIO_CHUNKS CLEAN_STUDIO_CHTS
CLEAN_TARGETS_ROOT=$(CLEAN_TARGETS_ROOT) $(CLEAN_STUDIO)

!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"
ALL: $(SOC_OBJ_DIR) $(ALL_STUDIO)
3dmovie.exe : ALL
CLEAN: $(CLEAN_STUDIO) ALL
!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)


!ENDIF # !LOCAL_BUILD



# Why, you may ask, do I specify "KEEP" and then just go off and delete
# the batch file anyway?  It's because if I'm building nmake commands but
# not actually executing them right away, I need the batch file to stick
# around.  It really is temporary though, so I go ahead and include an explicit
# "del" so that it goes away after we've used it for real.

CLEAN_UTEST:
    @echo <<delutest.bat
@echo off
rm -f $(UTEST_TARGETS: =^
rm -f )
rm -f $(TARGET_DIR)3dmovie.chk
<<KEEP
    cmd /c delutest.bat
    rem del delutest.bat


CLEAN_STUDIO_CHUNKS:
    @echo <<delchunk.bat
@echo off
rm -f $(STUDIO_CHUNK_TARGETS: =^
rm -f )
<<KEEP
    cmd /c delchunk.bat
    rem del delchunk.bat


CLEAN_STUDIO_CHTS:
    @echo <<delcht.bat
@echo off
rm -f $(STUDIO_CHT_TARGETS: =^
rm -f )
<<KEEP
    cmd /c delcht.bat
    rem del delcht.bat





#
#Create Studio Chunky Files
#
STUDIO_CONTENT: $(TARGET_DIR)studio.chk 

$(TARGET_DIR)studio.chk : $(STUDIO_CHT_FILES)


!IF "$(LOCAL_BUILD)" != "1"

3dmovie.exe : $(TARGET_DIR)3dmovie.exe
$(TARGET_DIR)3dmovie.exe: $(SOC_OBJ_DIR)

!ENDIF # !LOCAL_BUILD

$(TARGET_DIR)utest.lnk: $(STUDIO_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
	@echo <<$(TARGET_DIR)utest.lnk
$(KAUAI_OBJ_GROUPS_FOR_UTEST: =^
)
<<KEEP

$(TARGET_DIR)3dmovie.chk : $(UTEST_CHT_FILES)
	cl /E -I$(STUDIO_SRC_DIR) $(CHT_BUILD_INC) $(DEFS) /Tp$(STUDIO_SRC_DIR)\utest.cht > $*.i
	$(CHKERR)
	$(CHOMP) $*.i $@
	$(CHKERR)

$(TARGET_DIR)3dmovie.exe : $(KAUAI_OBJ_GROUPS_FOR_UTEST)
$(TARGET_DIR)3dmovie.exe : $(UTEST_DEPS) $(UTEST_TARGETS) $(TARGET_DIR)utest.lnk
    @echo Linking Utest Objects...
    $(LINK) -link $(LFLAGS_GUI) \
	$(UTEST_DEPS) \
	$(UTEST_TARGETS) \
	-out:$(TARGET_DIR)3dmovie.exe @$(TARGET_DIR)utest.lnk
    $(CHKERR)

!ENDIF  # !MAKEFILE_STUDIO
