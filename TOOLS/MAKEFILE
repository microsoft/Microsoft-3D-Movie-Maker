# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

#---$(SOC_ROOT)\tools\makefile

!IFNDEF MAKEFILE_SOC_TOOLS
MAKEFILE_SOC_TOOLS = 1

!INCLUDE $(KAUAI_ROOT)\makefile.def


#TOOLS SOURCE DIRECTORY
TOOLS_SRC_DIR = $(SOC_ROOT)\tools

#SOC OBJ DIRECTORY
SOC_OBJ_ROOT_DIR = $(SOC_ROOT)\obj
SOC_OBJ_DIR = $(SOC_OBJ_ROOT_DIR)\$(BLD_TYPE_DIR)

!IF "$(LOCAL_BUILD)" == "1"
TARGET_DIR =
!ELSE # LOCAL_BUILD
TARGET_DIR = $(SOC_OBJ_DIR)^\
!ENDIF # !LOCAL_BUILD


BREN_LIB =  $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\bren.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfmmxr.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brfwmxr.lib\
            $(SOC_ROOT)\elib\$(BLD_TYPE_DIR)\brzbmxr.lib


DKIT_LIB = $(SOC_ROOT)\dkit\libdkit.lib\
    $(SOC_ROOT)\dkit\byteswap.obj\
    $(SOC_ROOT)\dkit\sysglob.obj



KAUAI_OBJ_GROUPS_FOR_TDFMAKE =\
    $(BASE_OBJS)\
    $(SCREXE_OBJS)\
    $(SCRCOM_OBJS)\
    $(LEXER_OBJS)\
    $(GROUP_OBJS)\
    $(STREAM_OBJS)\
    $(FILE_OBJS)


KAUAI_OBJ_GROUPS_FOR_SITOBREN =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(CHSE_OBJS)\
    $(SCRCOM_OBJS)\
    $(LEXER_OBJS)\
    $(STREAM_OBJS)\
    $(SCREXE_OBJS)\
    $(MBMPIO_OBJS)\
    $(KAUAI_OBJ_DIR)\mssio.obj


KAUAI_OBJ_GROUPS_FOR_MKTMAP =\
    $(BASE_OBJS)\
    $(FILE_OBJS)\
    $(GROUP_OBJS)\
    $(MBMPIO_OBJS)\
    $(KAUAI_OBJ_DIR)\mssio.obj


TDFMAKE_TARGETS =\
    $(TARGET_DIR)tdfmake.obj


SITOBREN_TARGETS =\
    $(TARGET_DIR)sitobren.obj
SITOBREN_DEPS =\
    $(BREN_LIB)\
    $(DKIT_LIB)


MKTMAP_TARGETS =\
    $(TARGET_DIR)mktmap.obj
MKTMAP_DEPS =\
    $(TARGET_DIR)engine.lib\
    $(BREN_LIB)


#-Compile rules-------------------------------------------------------------


PROGRAM_DATABASE = $(TARGET_DIR)soc.pdb
PRECOMPILED_HEADER = $(TARGET_DIR)soc.pch

CPPEXTRA = /Zi /Fd$(PROGRAM_DATABASE) /Fp$(PRECOMPILED_HEADER)

SRC_DIR=$(TOOLS_SRC_DIR)
OBJ_DIR=$(SOC_OBJ_DIR)
TGT_NAME=Tools

!INCLUDE $(SOC_ROOT)\makefile.rul


#-Targets-------------------------------------------------------------------

ALL_SOCTOOLS = $(TARGET_DIR)tdfmake.exe $(TARGET_DIR)sitobren.exe $(TARGET_DIR)mktmap.exe
ALL_TARGETS_ROOT = $(ALL_TARGETS_ROOT) $(ALL_SOCTOOLS)

CLEAN_SOCTOOLS = CLEAN_TDFMAKE CLEAN_SITOBREN CLEAN_MKTMAP
CLEAN_TARGETS_ROOT = $(CLEAN_TARGETS_ROOT) $(CLEAN_SOCTOOLS)


!IF "$(LOCAL_BUILD)" != "1"

!IF "$(LOCAL_BUILD)" != "0"
ALL: $(SOC_OBJ_DIR) $(ALL_SOCTOOLS)
CLEAN: $(CLEAN_SOCTOOLS) ALL
!ENDIF # LOCAL_BUILD != 0

$(SOC_OBJ_DIR) :
    @echo Making Directories $(SOC_OBJ_DIR)...
    if not exist $(SOC_OBJ_ROOT_DIR)/nul mkdir $(SOC_OBJ_ROOT_DIR)
    if not exist $(SOC_OBJ_DIR)/nul mkdir $(SOC_OBJ_DIR)

!ENDIF # !LOCAL_BUILD


CLEAN_TDFMAKE:
    @echo <<deltdf.bat
@echo off
rm -f $(TDFMAKE_TARGETS: =^
rm -f )
<<KEEP
    cmd /c deltdf.bat
    del deltdf.bat


CLEAN_SITOBREN:
    @echo <<delsito.bat
@echo off
rm -f $(SITOBREN_TARGETS: =^
rm -f )
<<KEEP
    cmd /c delsito.bat
    del delsito.bat


CLEAN_MKTMAP:
    @echo <<deltmap.bat
@echo off
rm -f $(MKTMAP_TARGETS: =^
rm -f )
<<KEEP
    cmd /c deltmap.bat
    del deltmap.bat


!IF "$(LOCAL_BUILD)" != "1"

tdfmake.exe : $(TARGET_DIR)tdfmake.exe
sitobren.exe : $(TARGET_DIR)sitobren.exe
mktmap.exe : $(TARGET_DIR)mktmap.exe
$(TARGET_DIR)tdfmake.exe: $(SOC_OBJ_DIR)
$(TARGET_DIR)sitobren.exe: $(SOC_OBJ_DIR)
$(TARGET_DIR)mktmap.exe: $(SOC_OBJ_DIR)

!ENDIF # !LOCAL_BUILD

$(TARGET_DIR)tdfmake.lnk : $(TOOLS_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
	@echo <<$(TARGET_DIR)tdfmake.lnk
$(KAUAI_OBJ_GROUPS_FOR_TDFMAKE: =^
)
<<KEEP

$(TARGET_DIR)tdfmake.exe : $(KAUAI_OBJ_GROUPS_FOR_TDFMAKE)
$(TARGET_DIR)tdfmake.exe : $(TDFMAKE_TARGETS) $(TARGET_DIR)tdfmake.lnk
    @echo Linking Tdfmake Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    $(BREN_LIB) \
    $(TARGET_DIR)engine.lib \
    $(TDFMAKE_TARGETS) \
    -out:$(TARGET_DIR)tdfmake.exe @$(TARGET_DIR)tdfmake.lnk
    $(CHKERR)



$(TARGET_DIR)sitobren.lnk : $(TOOLS_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)sitobren.lnk
$(KAUAI_OBJ_GROUPS_FOR_SITOBREN: =^
)
<<KEEP

$(TARGET_DIR)sitobren.exe : $(KAUAI_OBJ_GROUPS_FOR_SITOBREN)
$(TARGET_DIR)sitobren.exe : $(SITOBREN_TARGETS) $(SITOBREN_DEPS) $(TARGET_DIR)sitobren.lnk
    @echo Linking Sitobren Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    $(SITOBREN_TARGETS) \
    $(SITOBREN_DEPS) \
    -out:$(TARGET_DIR)sitobren.exe @$(TARGET_DIR)sitobren.lnk
    $(CHKERR)


$(TARGET_DIR)mktmap.lnk : $(TOOLS_SRC_DIR)\makefile $(KAUAI_ROOT)\makefile.def
    @echo <<$(TARGET_DIR)mktmap.lnk
$(KAUAI_OBJ_GROUPS_FOR_MKTMAP: =^
)
<<KEEP

$(TARGET_DIR)mktmap.exe : $(KAUAI_OBJ_GROUPS_FOR_MKTMAP)
$(TARGET_DIR)mktmap.exe : $(MKTMAP_TARGETS) $(MKTMAP_DEPS) $(TARGET_DIR)mktmap.lnk
    @echo Linking MkTmap Objects...
    $(LINK) -link $(LFLAGS_CONS) \
    $(MKTMAP_TARGETS) \
    $(MKTMAP_DEPS) \
    -out:$(TARGET_DIR)mktmap.exe @$(TARGET_DIR)mktmap.lnk
    $(CHKERR)

!ENDIF  # !MAKEFILE_SOC_TOOLS

