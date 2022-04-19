#--- $(KAUAI_ROOT)\obj\macd\makefile


.SILENT:


ALL: SETBLD KAUAI_SRC KAUAI_TOOLS

SETBLD:
    Setbld d m n

KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making All $(KAUAI_ROOT)\src...
    nmake /NOLOGO
    cd $(KAUAI_ROOT)\obj\macd

KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making All $(KAUAI_ROOT)\tools...
    nmake /NOLOGO
    cd $(KAUAI_ROOT)\obj\macd


CLEAN: SETBLD CLEAN_KAUAI_SRC CLEAN_KAUAI_TOOLS


CLEAN_KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making Clean $(KAUAI_ROOT)\src...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)\obj\macd

CLEAN_KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making Clean $(KAUAI_ROOT)\tools...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)\obj\macd



