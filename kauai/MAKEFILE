#--- $(KAUAI_ROOT)\makefile.mak

!INCLUDE $(KAUAI_ROOT)\makefile.def

.SILENT:

ALL: ALL_KAUAI_SRC ALL_KAUAI_TOOLS


ALL_KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making Kauai\src All...
    nmake /NOLOGO all
    cd $(KAUAI_ROOT)


ALL_KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making Kauai\tools All...
    nmake /NOLOGO all
    cd $(KAUAI_ROOT)



CLEAN: CLEAN_OBJ_DIR ALL_KAUAI_SRC ALL_KAUAI_TOOLS


CLEAN_OBJ_DIR:
    @echo Cleaning $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\ directory
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.obj 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.pch 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.pdb 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.exe 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.res 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.cod 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.map 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.lib 2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.i   2>nul
    rm -f $(KAUAI_ROOT)\obj\$(BLD_TYPE_DIR)\*.lnk 2>nul


CLEAN_KAUAI_SRC:
    cd $(KAUAI_ROOT)\src
    @echo Making Kauai\src Clean...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)


CLEAN_KAUAI_TOOLS:
    cd $(KAUAI_ROOT)\tools
    @echo Making Kauai\tools Clean...
    nmake /NOLOGO clean
    cd $(KAUAI_ROOT)







