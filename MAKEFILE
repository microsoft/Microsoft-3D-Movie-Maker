#--- $(SOC_ROOT)\makefile.mak

!INCLUDE $(KAUAI_ROOT)\makefile.def

.SILENT:

ALL: ALL_BREN ALL_SRC ALL_TOOLS


ALL_BREN:
    cd $(SOC_ROOT)\bren
    @echo Making Bren All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)


ALL_SRC:
    cd $(SOC_ROOT)\src
    @echo Making Soc\src All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)


ALL_TOOLS:
    cd $(SOC_ROOT)\tools
    @echo Making Soc\tools All...
    $(MAKE) /NOLOGO all
    cd $(SOC_ROOT)




CLEAN: CLEAN_REST CLEAN_BREN BUILD_REST


CLEAN_BREN:
    cd $(SOC_ROOT)\bren
    @echo Making Bren Clean...
    $(MAKE) /NOLOGO clean
    cd $(SOC_ROOT)


CLEAN_REST:
    @echo Cleaning $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\ directory
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.obj 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.pch 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.pdb 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.exe 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.res 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.chk 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.cht 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.cod 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.map 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.thd 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.lib 2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.i   2>nul
    rm -f $(SOC_ROOT)\obj\$(BLD_TYPE_DIR)\*.lnk 2>nul


BUILD_REST:
    cd $(SOC_ROOT)\src
    @echo Making Src Clean...
    $(MAKE) /NOLOGO
    cd $(SOC_ROOT)\tools
    @echo Making Tools Clean...
    $(MAKE) /NOLOGO
    cd $(SOC_ROOT)

