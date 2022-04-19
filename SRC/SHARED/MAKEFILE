#---shared.mak

!IFNDEF MAKEFILE_SHARED
MAKEFILE_SHARED = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def

#SHARED SOURCE DIRECTORY
SHARED_SRC_DIR = $(SOC_ROOT)\src\shared

#SOC OBJ DIRECTORY
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD

SOC_HEADER_FILES = \
	$(SOC_ROOT)\inc\kidgs.chh \
!IF "$(TYPE)" == "DEBUG"
	$(SOC_ROOT)\inc\assert.chh \
!ENDIF
	$(SOC_ROOT)\inc\kidgsdef.h \
	$(KAUAI_ROOT)\src\kiddef.h \
	$(KAUAI_ROOT)\src\framedef.h \
	$(SOC_ROOT)\inc\buildgob.h \
	$(SOC_ROOT)\inc\sharedef.h \
	$(SOC_ROOT)\inc\sharutil.chh\
	$(SOC_ROOT)\inc\balloons.chh


SHARED_CHT_FILES = \
	$(SHARED_SRC_DIR)\shared.cht\
	$(SHARED_SRC_DIR)\util.cht\
	$(SHARED_SRC_DIR)\cursors.cht\
	$(SHARED_SRC_DIR)\map.cht\
	$(SHARED_SRC_DIR)\biopage.cht\
	$(SHARED_SRC_DIR)\sharhelp.cht\
	$(SHARED_SRC_DIR)\sharecd.cht

SHARED_CHH_FILES = $(SHARED_SRC_DIR)\map.chh

SHARED_CHUNK_TARGETS =\
    $(TARGET_DIR)shared.chk $(TARGET_DIR)sharecd.chk

#-Compile rules-------------------------------------------------------------

SRC_DIR = $(SHARED_SRC_DIR)
OBJ_DIR = $(SOC_OBJ_DIR)
TGT_NAME = Shared

!INCLUDE $(SOC_ROOT)\makefile.rul

#-Targets-------------------------------------------------------------------

ALL_SHARED = $(TARGET_DIR)shared.chk $(TARGET_DIR)sharecd.chk 
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_SHARED)

CLEAN_SHARED = CLEAN_SHARED_CHUNKS
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_SHARED)

!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"

ALL: $(SOC_OBJ_DIR) SHARED_CONTENT
CLEAN: CLEAN_SHARED_CHUNKS ALL

!ENDIF  # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF  # LOCAL_BUILD != 1

CLEAN_SHARED_CHUNKS:
    @echo <<delshrd.bat
@echo off
rm -f $(SHARED_CHUNK_TARGETS: =^
rm -f )
<<KEEP
    call delshrd.bat
				  

#
#Create Shared Chunky Files
#
SHARED_CONTENT: $(SOC_OBJ_DIR)\shared.chk  $(SOC_OBJ_DIR)\sharecd.chk

$(TARGET_DIR)shared.chk : $(SOC_OBJ_DIR) $(SHARED_CHT_FILES) $(SHARED_CHH_FILES) $(SOC_HEADER_FILES)
$(TARGET_DIR)sharecd.chk : $(SOC_OBJ_DIR) $(SHARED_CHT_FILES) $(SHARED_CHH_FILES) $(SOC_HEADER_FILES)

!ENDIF  # !MAKEFILE_SHARED

