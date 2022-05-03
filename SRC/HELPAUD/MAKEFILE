#---$(SOC_ROOT)\src\helpaud\makefile

!IFNDEF MAKEFILE_HELPAUD
MAKEFILE_HELPAUD = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#HELP SOURCE DIRECTORY
HELPAUD_SRC_DIR = $(SOC_ROOT)\src\helpaud

#SOC OBJ DIRECTORY
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD

HELPAUD_CHT_FILES = \
	$(HELPAUD_SRC_DIR)\helpaud.cht\
	$(HELPAUD_SRC_DIR)\wctooltp.cht\
	$(HELPAUD_SRC_DIR)\wcbasics.cht\
	$(HELPAUD_SRC_DIR)\wctoolhp.cht\
	$(HELPAUD_SRC_DIR)\wcgdelgo.cht\
	$(HELPAUD_SRC_DIR)\wcproj.cht\
	$(HELPAUD_SRC_DIR)\wchowto.cht

HELPAUD_CHUNK_TARGETS =\
    $(TARGET_DIR)helpaud.chk


#-Compile rules-------------------------------------------------------------

SRC_DIR = $(HELPAUD_SRC_DIR)
OBJ_DIR = $(SOC_OBJ_DIR)
TGT_NAME = HelpAud

!INCLUDE $(SOC_ROOT)\makefile.rul

#-Targets-------------------------------------------------------------------

ALL_HELPAUD = $(TARGET_DIR)helpaud.chk
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_HELPAUD)

CLEAN_HELP = CLEAN_HELPAUD_CHUNKS
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_HELP)

!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"

ALL: $(SOC_OBJ_DIR) HELPAUD_CONTENT
CLEAN: CLEAN_HELPAUD_CHUNKS ALL

!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF # LOCAL_BUILD != 1


CLEAN_HELPAUD_CHUNKS:
    @echo <<delhelp.bat
@echo off
rm -f $(HELPAUD_CHUNK_TARGETS: =^
rm -f )
<<KEEP
    call delhelp.bat


#
#Create Help Chunky Files
#
HELPAUD_CONTENT: $(SOC_OBJ_DIR)\helpaud.chk

$(TARGET_DIR)helpaud.chk : $(SOC_OBJ_DIR) $(HELPAUD_CHT_FILES)

MAKEFILE_HELPAUD = 0
!ENDIF # !MAKEFILE_HELPAUD
