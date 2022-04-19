#--- $(SOC_ROOT)\src\makefile.mak

.SILENT:

#SOC SOURCE DIRECTORIES
SOC_ENGINE_DIR = $(SOC_ROOT)\engine
SOC_STUDIO_DIR = $(SOC_ROOT)\studio
SOC_BLDG_DIR = $(SOC_ROOT)\building
SOC_SHARED_DIR = $(SOC_ROOT)\shared

{$(SOC_ENGINE_DIR)}.cpp.obj:
    @cd engine
    @nmake /NOLOGO $@

{$(SOC_STUDIO_DIR)}.cpp.obj:
    @cd studio
    @nmake /NOLOGO $@

{$(SOC_ENGINE_DIR)}.cpp.exe:
    @cd engine
    @nmake /NOLOGO $@

{$(SOC_STUDIO_DIR)}.cpp.exe:
    @cd studio
    @nmake /NOLOGO $@

{$(SOC_ENGINE_DIR)}.rc.res:
    @cd engine
    @nmake /NOLOGO $@

{$(SOC_STUDIO_DIR)}.rc.res:
    @cd studio
    @nmake /NOLOGO $@

{$(SOC_ENGINE_DIR)}.cht.chk:
    @cd engine
    @nmake /NOLOGO $@

{$(SOC_STUDIO_DIR)}.cht.chk:
    @cd studio
    @nmake /NOLOGO $@

{$(SOC_BLDG_DIR)}.cht.chk:
    @cd studio
    @nmake /NOLOGO $@

ALL_TARGETS = ALL_ENGINE ALL_STUDIO

!IF EXIST($(SOC_ROOT)\src\building\makefile)
ALL_TARGETS = $(ALL_TARGETS) ALL_BLDG
!ENDIF

!IF EXIST($(SOC_ROOT)\src\helpaud\makefile)
ALL_TARGETS = $(ALL_TARGETS) ALL_HELPAUD
!ENDIF

!IF EXIST($(SOC_ROOT)\src\help\makefile)
ALL_TARGETS = $(ALL_TARGETS) ALL_HELP
!ENDIF

!IF EXIST($(SOC_ROOT)\src\shared\makefile)
ALL_TARGETS = $(ALL_TARGETS) ALL_SHARED
!ENDIF

ALL: $(ALL_TARGETS)

ALL_ENGINE:
    cd $(SOC_ROOT)\src\engine
    @echo Making Engine All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


ALL_STUDIO:
    cd $(SOC_ROOT)\src\studio
    @echo Making Studio All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


ALL_BLDG:
    cd $(SOC_ROOT)\src\building
    @echo Making Building All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


ALL_HELP:
    cd $(SOC_ROOT)\src\help
    @echo Making Help All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


ALL_HELPAUD:
    cd $(SOC_ROOT)\src\helpaud
    @echo Making Helpaud All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


ALL_SHARED:
    cd $(SOC_ROOT)\src\shared
    @echo Making Shared All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)\src


CLEAN: CLEAN_ENGINE CLEAN_HELP CLEAN_HELPAUD CLEAN_STUDIO CLEAN_BLDG CLEAN_SHARED

CLEAN_ENGINE:
    cd $(SOC_ROOT)\src\engine
    @echo Making Engine Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src


CLEAN_HELP:
    cd $(SOC_ROOT)\src\help
    @echo Making Help Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src


CLEAN_HELPAUD:
    cd $(SOC_ROOT)\src\helpaud
    @echo Making Helpaud Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src


CLEAN_STUDIO:
    cd $(SOC_ROOT)\src\studio
    @echo Making Studio Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src


CLEAN_BLDG:
    cd $(SOC_ROOT)\src\building
    @echo Making Building Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src


CLEAN_SHARED:
    cd $(SOC_ROOT)\src\shared
    @echo Making Shared Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)\src

