#---$(SOC_ROOT)\src\help\makefile

!IFNDEF MAKEFILE_HELP
MAKEFILE_HELP = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#HELP SOURCE DIRECTORY
HELP_SRC_DIR = $(SOC_ROOT)\src\help

#SOC OBJ DIRECTORY
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD

HELP_CHT_FILES = \
	$(HELP_SRC_DIR)\help.cht\
	$(HELP_SRC_DIR)\actorbio.cht\
	$(HELP_SRC_DIR)\bktips.cht\
	$(HELP_SRC_DIR)\bkhowto.cht\
	$(HELP_SRC_DIR)\bktocbas.cht\
	$(HELP_SRC_DIR)\bktools.cht\
	$(HELP_SRC_DIR)\basics.cht\
	$(HELP_SRC_DIR)\booktpc.cht\
	$(HELP_SRC_DIR)\easelhp.cht\
	$(HELP_SRC_DIR)\errors.cht\
	$(HELP_SRC_DIR)\guidhelp.cht\
	$(HELP_SRC_DIR)\htalerts.cht\
	$(HELP_SRC_DIR)\htactors.cht\
	$(HELP_SRC_DIR)\htscenes.cht\
	$(HELP_SRC_DIR)\htsounds.cht\
	$(HELP_SRC_DIR)\htwords.cht\
	$(HELP_SRC_DIR)\logo.cht\
	$(HELP_SRC_DIR)\prjalert.cht\
	$(HELP_SRC_DIR)\prjtips.cht\
	$(HELP_SRC_DIR)\prjintro.cht\
	$(HELP_SRC_DIR)\project1.cht\
	$(HELP_SRC_DIR)\project2.cht\
	$(HELP_SRC_DIR)\project3.cht\
	$(HELP_SRC_DIR)\project4.cht\
	$(HELP_SRC_DIR)\project5.cht\
	$(HELP_SRC_DIR)\project6.cht\
	$(HELP_SRC_DIR)\gadget1.cht\
	$(HELP_SRC_DIR)\gadget2.cht\
	$(HELP_SRC_DIR)\gadget3.cht\
	$(HELP_SRC_DIR)\gadget4.cht\
	$(HELP_SRC_DIR)\gadget5.cht\
	$(HELP_SRC_DIR)\gadget6.cht\
	$(HELP_SRC_DIR)\toolhelp.cht\
	$(HELP_SRC_DIR)\tooltips.cht\
	$(HELP_SRC_DIR)\app.cht\
	$(HELP_SRC_DIR)\topics1.cht


HELP_CHUNK_TARGETS =\
    $(TARGET_DIR)help.chk


#-Compile rules-------------------------------------------------------------

SRC_DIR = $(HELP_SRC_DIR)
OBJ_DIR = $(SOC_OBJ_DIR)
TGT_NAME = Help

!INCLUDE $(SOC_ROOT)\makefile.rul

#-Targets-------------------------------------------------------------------

ALL_HELP = $(TARGET_DIR)help.chk
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_HELP)

CLEAN_HELP = CLEAN_HELP_CHUNKS
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_HELP)

!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"

ALL: $(SOC_OBJ_DIR) HELP_CONTENT
CLEAN: CLEAN_HELP_CHUNKS ALL

!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF # LOCAL_BUILD != 1


CLEAN_HELP_CHUNKS:
    @echo <<delhelp.bat
@echo off
rm -f $(HELP_CHUNK_TARGETS: =^
rm -f )
<<KEEP
    call delhelp.bat


#
#Create Help Chunky Files
#
HELP_CONTENT: $(SOC_OBJ_DIR)\help.chk

$(TARGET_DIR)help.chk : $(SOC_OBJ_DIR) $(HELP_CHT_FILES)

MAKEFILE_HELP = 0
!ENDIF # !MAKEFILE_HELP
