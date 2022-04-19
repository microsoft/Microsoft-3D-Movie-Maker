#---bren.mak

!IFNDEF MAKEFILE_BREN
MAKEFILE_BREN = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def



#BREN SOURCE DIRECTORIES
BREN_SRC_DIR = $(SOC_ROOT)\bren

#BREN OBJ DIRECTORIES
BREN_OBJ_ROOT_DIR = $(BREN_SRC_DIR)\obj
BREN_OBJ_DIR = $(BREN_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)
SOC_OBJ_DIR = $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
BREN_TGT_DIR =
!ELSE # LOCAL_BUILD
BREN_TGT_DIR = $(BREN_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD

BREN_TARGETS =\
    $(BREN_TGT_DIR)bwld.obj\
    $(BREN_TGT_DIR)stderr.obj\
    $(BREN_TGT_DIR)stdfile.obj\
    $(BREN_TGT_DIR)stdmem.obj\
    $(BREN_TGT_DIR)tmap.obj\
    $(BREN_TGT_DIR)zbmp.obj



#-Compile rules-------------------------------------------------------------


PROGRAM_DATABASE = $(BREN_TGT_DIR)soc.pdb
PRECOMPILED_HEADER = $(BREN_TGT_DIR)soc.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

SRC_DIR=$(BREN_SRC_DIR)
OBJ_DIR=$(BREN_OBJ_DIR)
TGT_NAME=Brender

!INCLUDE $(SOC_ROOT)\makefile.rul



#-Targets-------------------------------------------------------------------

ALL_BREN = $(BREN_TGT_DIR)bren.lib $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\bren.lib
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_BREN)

CLEAN_BREN = CLEAN_BREN_OBJ
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_BREN)


!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"
ALL: $(BREN_OBJ_DIR) $(ALL_BREN)
CLEAN: $(CLEAN_BREN) ALL
!ENDIF # LOCAL_BUILD != 0

$(BREN_OBJ_DIR) :
    @echo Making Directories $(BREN_OBJ_DIR)...
    if not exist $(BREN_OBJ_ROOT_DIR)/nul mkdir $(BREN_OBJ_ROOT_DIR)
    if not exist $(BREN_OBJ_DIR)/nul mkdir $(BREN_OBJ_DIR)

!ENDIF # !LOCAL_BUILD


CLEAN_BREN_OBJ:
    @echo <<delbren.bat
@echo off
rm -f $(BREN_TARGETS: =^
rm -f )
<<KEEP
    cmd /c delbren.bat
    del delbren.bat




$(BREN_TGT_DIR)bren.lib : $(BREN_TARGETS)
    $(LINK) -lib $(LFLAGS) \
    $(BREN_TARGETS) \
    -out:$(BREN_TGT_DIR)bren.lib
    $(CHKERR)

$(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\bren.lib : $(BREN_TGT_DIR)bren.lib
    if not exist $(SOC_ROOT)\elib\$(BLD_TYPE_DIR) md $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)
    $(CHKERR)
    copy $(BREN_TGT_DIR)bren.lib $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)
    $(CHKERR)


!ENDIF  # !MAKEFILE_BREN
