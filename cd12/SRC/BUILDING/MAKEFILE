#---building.mak

!IFNDEF MAKEFILE_BUILDING
MAKEFILE_BUILDING = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def

#BUILDING SOURCE DIRECTORY
BLDG_SRC_DIR = $(SOC_ROOT)\src\building

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
	$(SOC_ROOT)\inc\assert.chh \
	$(SOC_ROOT)\inc\kidgsdef.h \
	$(KAUAI_ROOT)\src\kiddef.h \
	$(KAUAI_ROOT)\src\framedef.h \
	$(SOC_ROOT)\inc\stdiodef.h \
	$(SOC_ROOT)\inc\socdef.h \
	$(SOC_ROOT)\inc\buildgob.h \
	$(SOC_ROOT)\inc\sharedef.h \
	$(SOC_ROOT)\inc\sharutil.chh

BLDG_CHH_FILES = $(BLDG_SRC_DIR)\socdefn.h

BLDG_CHT_FILES = \
    $(BLDG_SRC_DIR)\backstag.cht\
    $(BLDG_SRC_DIR)\building.cht\
    $(BLDG_SRC_DIR)\imagin.cht\
    $(BLDG_SRC_DIR)\inspirat.cht\
    $(BLDG_SRC_DIR)\lobby.cht\
    $(BLDG_SRC_DIR)\login.cht\
    $(BLDG_SRC_DIR)\navbars.cht\
    $(BLDG_SRC_DIR)\palette.cht\
    $(BLDG_SRC_DIR)\projects.cht\
    $(BLDG_SRC_DIR)\snackbar.cht\
    $(BLDG_SRC_DIR)\street.cht\
    $(BLDG_SRC_DIR)\studiobg.cht\
    $(BLDG_SRC_DIR)\theatre.cht\
    $(BLDG_SRC_DIR)\ticket.cht\
    $(BLDG_SRC_DIR)\waiting.cht

BLDG_CHUNK_TARGETS =\
    $(TARGET_DIR)building.chk

BLDGHD_CHT_FILES = \
    $(BLDG_SRC_DIR)\bldghd.cht

BLDGHD_CHUNK_TARGETS =\
    $(TARGET_DIR)bldghd.chk


#-Compile rules-------------------------------------------------------------


#OLD .cht.chk RULES
#{$(BLDG_SRC_DIR)}.cht{$(SOC_OBJ_DIR)}.chk:
#    SET INCLUDE=$(BLDG_SRC_DIR)\pbm;$(BLDG_SRC_DIR)\bitmaps;$(BLDG_SRC_DIR)\sound;$(INCLUDE)
#    cl /P -I$(BLDG_SRC_DIR) $(DEFS) /Tp$<
#    $(CHOMP) $(BLDG_SRC_DIR)\$(@B).i $*.chk


#{$(BLDG_SRC_DIR)}.cht{$(SOC_OBJ_DIR)}.chk:
#    SET INCLUDE=$(BLDG_SRC_DIR)\pbm;$(BLDG_SRC_DIR)\bitmaps;$(BLDG_SRC_DIR)\sound;$(INCLUDE)
#    cl /E -I$(SOC_OBJ_DIR) $(DEFS) /Tp$< > $(SOC_OBJ_DIR)\%|fF.i
#    $(CHOMP) $(SOC_OBJ_DIR)\$(@B).i $*.chk

SRC_DIR = $(BLDG_SRC_DIR)
OBJ_DIR = $(SOC_OBJ_DIR)
!INCLUDE $(SOC_ROOT)\makefile.rul

#-Targets-------------------------------------------------------------------

ALL_BLDG = $(TARGET_DIR)building.chk  $(TARGET_DIR)bldghd.chk
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_BLDG)

CLEAN_BLDG = CLEAN_BLDG_CHUNKS
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_BLDG)

!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"

building.chk : ALL

ALL: $(SOC_OBJ_DIR) BLDG_CONTENT
CLEAN: CLEAN_BLDG_CHUNKS ALL

!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF # LOCAL_BUILD != 1


CLEAN_BLDG_CHUNKS:
    @echo <<delchunk.bat
@echo off
rm -f $(BLDG_CHUNK_TARGETS: =^
rm -f )
<<KEEP
    call delchunk.bat


#
#Create Studio Chunky Files
#
BLDG_CONTENT: $(SOC_OBJ_DIR)\building.chk $(SOC_OBJ_DIR)\bldghd.chk 

$(TARGET_DIR)building.chk : $(BLDG_CHT_FILES)

$(TARGET_DIR)bldghd.chk : $(BLDGHD_CHT_FILES)


!ENDIF  # !MAKEFILE_BUILDING
