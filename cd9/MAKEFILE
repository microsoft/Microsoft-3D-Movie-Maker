TARGETS = 3dmovie.chk building.chk bldghd.chk help.chk helpaud.chk shared.chk \
	studio.chk sharecd.chk

!IF EXIST($(SOC_ROOT)\obj\wind\building.chk)
TARGETS = $(TARGETS) buildingd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\bldghd.chk)
TARGETS = $(TARGETS) bldghdd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\help.chk)
TARGETS = $(TARGETS) helpd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\helpaud.chk)
TARGETS = $(TARGETS) helpaudd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\shared.chk)
TARGETS = $(TARGETS) sharedd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\sharecd.chk)
TARGETS = $(TARGETS) sharecdd.chk
!ENDIF

!IF EXIST($(SOC_ROOT)\obj\wind\studio.chk)
TARGETS = $(TARGETS) studiod.chk
!ENDIF

all: $(TARGETS)

#
# Update 3dmovie.chk when either the SHIP or DEBUG version has changed; if
# they're both out of date, go with the SHIP version (most of the time, they'll
# be the same, and it's more complicated than it's worth to try to get the
# most-recent one out of those two; if it's *really* important, then just run
# the makefile twice...the genuine most-recent version will get copied the
# second time around, if it was the DEBUG version).
#
3dmovie.chk : $(SOC_ROOT)\obj\wind\3dmovie.chk $(SOC_ROOT)\obj\wins\3dmovie.chk
	if "%s" == "$?" copy %s $@
	if not "%s" == "$?" copy $(SOC_ROOT)\obj\wins\3dmovie.chk $@


# SHIP versions of .chk files  --  Must exist

building.chk : $(SOC_ROOT)\obj\wins\building.chk
	copy $? $@

bldghd.chk : $(SOC_ROOT)\obj\wins\bldghd.chk
	copy $? $@

help.chk : $(SOC_ROOT)\obj\wins\help.chk
	copy $? $@

helpaud.chk : $(SOC_ROOT)\obj\wins\helpaud.chk
	copy $? $@

shared.chk : $(SOC_ROOT)\obj\wins\shared.chk
	copy $? $@

sharecd.chk : $(SOC_ROOT)\obj\wins\sharecd.chk
	copy $? $@

studio.chk : $(SOC_ROOT)\obj\wins\studio.chk
	copy $? $@


# DEBUG versions of .chk files  --  Optional

buildingd.chk : $(SOC_ROOT)\obj\wind\building.chk
	copy $? $@

bldghdd.chk : $(SOC_ROOT)\obj\wind\bldghd.chk
	copy $? $@

helpd.chk : $(SOC_ROOT)\obj\wind\help.chk
	copy $? $@

helpaudd.chk : $(SOC_ROOT)\obj\wind\helpaud.chk
	copy $? $@

sharedd.chk : $(SOC_ROOT)\obj\wind\shared.chk
	copy $? $@

sharecdd.chk : $(SOC_ROOT)\obj\wind\sharecd.chk
	copy $? $@

studiod.chk : $(SOC_ROOT)\obj\wind\studio.chk
	copy $? $@
