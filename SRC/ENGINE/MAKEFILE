#---engine.mak

!IFNDEF MAKEFILE_ENGINE
MAKEFILE_ENGINE = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#ENGINE SOURCE DIRECTORY
ENGINE_SRC_DIR = $(SOC_ROOT)\src\engine

#SOC OBJ DIRECTORY
# REVIEW a-aarob (peted): why isn't this defined in the soc\makefile?
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)"=="1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD

BREN_LIB =  $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\bren.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfmmxr.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfwmxr.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brzbmxr.lib


ENGINE_TARGETS =\
    $(TARGET_DIR)actor.obj\
    $(TARGET_DIR)actredit.obj\
    $(TARGET_DIR)actrsave.obj\
    $(TARGET_DIR)actrsnd.obj\
    $(TARGET_DIR)bkgd.obj\
    $(TARGET_DIR)body.obj\
    $(TARGET_DIR)modl.obj\
    $(TARGET_DIR)movie.obj\
    $(TARGET_DIR)msnd.obj\
    $(TARGET_DIR)mtrl.obj\
    $(TARGET_DIR)scene.obj\
    $(TARGET_DIR)srec.obj\
    $(TARGET_DIR)tagl.obj\
    $(TARGET_DIR)tagman.obj\
    $(TARGET_DIR)tbox.obj\
    $(TARGET_DIR)tdf.obj\
    $(TARGET_DIR)tdt.obj\
    $(TARGET_DIR)tmpl.obj





#-Compile rules-------------------------------------------------------------

PROGRAM_DATABASE = $(TARGET_DIR)soc.pdb
PRECOMPILED_HEADER = $(TARGET_DIR)soc.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

SRC_DIR=$(ENGINE_SRC_DIR)
OBJ_DIR=$(SOC_OBJ_DIR)
TGT_NAME=Engine

!INCLUDE $(SOC_ROOT)\makefile.rul


#-Targets-------------------------------------------------------------------

ALL_ENGINE = $(TARGET_DIR)engine.lib
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_ENGINE)

CLEAN_ENGINE = CLEAN_ENGINE_OBJ
CLEAN_TARGETS_ROOT=$(CLEAN_TARGETS_ROOT) $(CLEAN_ENGINE)


# Only define generic targets if we're not building in the current directory
!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"
ALL: $(SOC_OBJ_DIR) $(ALL_ENGINE)
CLEAN: $(CLEAN_ENGINE) ALL
!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF # !LOCAL_BUILD



CLEAN_ENGINE_OBJ:
    @echo <<deleng.bat
@echo off
rm -f $(ENGINE_TARGETS: =^
rm -f )
<<KEEP
    cmd /c deleng.bat
    del deleng.bat



!IF "$(LOCAL_BUILD)" != "1"

engine.lib : $(TARGET_DIR)engine.lib

!ENDIF # !LOCAL_BUILD

$(TARGET_DIR)engine.lib : $(ENGINE_TARGETS)
    $(LINK) -lib $(LFLAGS) $(ENGINE_TARGETS) -out:$(TARGET_DIR)engine.lib
    $(CHKERR)

!ENDIF  # !MAKEFILE_ENGINE
