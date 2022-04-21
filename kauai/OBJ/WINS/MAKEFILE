#--- $(KAUAI_ROOT)\obj\wins\makefile


.SILENT:


ALL: SETBLD KAUAI_SRC KAUAI_TOOLS

SETBLD:
    Setbld s w n

KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making All $(KAUAI_ROOT)\src...
    nmake /NOLOGO
    cd $(KAUAI_ROOT)\obj\wins

KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making All $(KAUAI_ROOT)\tools...
    nmake /NOLOGO
    cd $(KAUAI_ROOT)\obj\wins


CLEAN: SETBLD CLEAN_KAUAI_SRC CLEAN_KAUAI_TOOLS


CLEAN_KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making Clean $(KAUAI_ROOT)\src...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)\obj\wins

CLEAN_KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making Clean $(KAUAI_ROOT)\tools...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)\obj\wins



