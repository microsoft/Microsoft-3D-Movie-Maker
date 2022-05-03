#--- $(KAUAI_ROOT)\obj\winud\makefile

#SOC SOURCE DIRECTORIES
SOC_ENGINE_DIR = $(SOC_ROOT)\src\engine
SOC_STUDIO_DIR = $(SOC_ROOT)\src\studio
SOC_BREN_DIR = $(SOC_ROOT)\bren
SOC_TOOLS_DIR = $(SOC_ROOT)\tools

{$(SOC_ENGINE_DIR)}.cpp.obj:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\engine
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_STUDIO_DIR)}.cpp.obj:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\studio
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_BREN_DIR)}.cpp.obj:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\bren
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_TOOLS_DIR)}.cpp.obj:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\tools
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_ENGINE_DIR)}.cpp.exe:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\engine
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_STUDIO_DIR)}.cpp.exe:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\studio
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_BREN_DIR)}.cpp.exe:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\bren
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_TOOLS_DIR)}.cpp.exe:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\tools
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_ENGINE_DIR)}.cht.chk:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\engine
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_STUDIO_DIR)}.cht.chk:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\src\studio
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_BREN_DIR)}.cht.chk:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\bren
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud

{$(SOC_TOOLS_DIR)}.cht.chk:
    @SET TYPE=DEBUG
    @SET ARCH=WIN
    @SET CHIP=IN_80386
    @SET UNICODE=1
    @cd $(SOC_ROOT)\tools
    @nmake /NOLOGO $@
    @cd $(SOC_ROOT)\obj\winud




ALL: SOC_SRC SOC_BREN

SOC_SRC:
    @cd $(SOC_ROOT)\src
    @echo Making All $(SOC_ROOT)\src...
    @nmake /NOLOGO
    @cd $(SOC_ROOT)\obj\winud

SOC_BREN:
    @cd $(SOC_ROOT)\bren
    @echo Making All $(SOC_ROOT)\bren...
    @nmake /NOLOGO
    @cd $(SOC_ROOT)\obj\winud


CLEAN: CLEAN_SOC_SRC CLEAN_SOC_BREN


CLEAN_SOC_SRC:
    @cd $(SOC_ROOT)\src
    @echo Making Clean $(SOC_ROOT)\src...
    @nmake /NOLOGO clean
    @cd $(SOC_ROOT)\obj\winud

CLEAN_SOC_BREN:
    @cd $(SOC_ROOT)\bren
    @echo Making Clean $(SOC_ROOT)\bren...
    @nmake /NOLOGO clean
    @cd $(SOC_ROOT)\obj\winud



