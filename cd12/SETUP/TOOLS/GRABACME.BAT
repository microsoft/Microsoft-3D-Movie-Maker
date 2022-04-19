@echo off
if not "%1" == "" goto GOTPARAM
echo %0 {Release Num}
echo.
echo example %0 1820
goto EXIT

:GOTPARAM
set ACMESHARE=\\conman\release
set ACMEPATH=\acme95\%1

net use z: /d
net use z: %ACMESHARE%
if exist z:%ACMEPATH%\usa\ship\acmsetup.exe goto GOTREL
echo Can't locate valid release at %ACMESHARE%%ACMEPATH%.
echo Email MontyE or JohnDelo for access to \\conman\release
goto EXIT

:GOTREL
cd u
out acmsetup.exe
out acmsetup.hlp
out mssetup.dll
out setup.exe
copy z:%ACMEPATH%\usa\ship\acmsetup.exe
copy z:%ACMEPATH%\usa\ship\acmsetup.hlp
copy z:%ACMEPATH%\usa\ship\mssetup.dll
copy z:%ACMEPATH%\usa\ship\setup.exe
in -c "%ACMEPATH%" acmsetup.exe
in -c "%ACMEPATH%" acmsetup.hlp
in -c "%ACMEPATH%" mssetup.dll
in -c "%ACMEPATH%" setup.exe

cd ..\ud
out acmsetup.exe
out mssetup.dll
out setup.exe
copy z:%ACMEPATH%\usa\debug\acmsetup.exe
copy z:%ACMEPATH%\usa\debug\mssetup.dll
copy z:%ACMEPATH%\usa\debug\setup.exe
in -c "%ACMEPATH%" acmsetup.exe
in -c "%ACMEPATH%" mssetup.dll
in -c "%ACMEPATH%" setup.exe

cd ..\d
out acmsetup.exe
out acmsetup.hlp
out mssetup.dll
out setup.exe
copy z:%ACMEPATH%\ger\ship\acmsetup.exe
copy z:%ACMEPATH%\ger\ship\acmsetup.hlp
copy z:%ACMEPATH%\ger\ship\mssetup.dll
copy z:%ACMEPATH%\ger\ship\setup.exe
in -c "%ACMEPATH%" acmsetup.exe
in -c "%ACMEPATH%" acmsetup.hlp
in -c "%ACMEPATH%" mssetup.dll
in -c "%ACMEPATH%" setup.exe

cd ..\dd
out acmsetup.exe
out mssetup.dll
out setup.exe
copy z:%ACMEPATH%\ger\debug\acmsetup.exe
copy z:%ACMEPATH%\ger\debug\mssetup.dll
copy z:%ACMEPATH%\ger\debug\setup.exe
in -c "%ACMEPATH%" acmsetup.exe
in -c "%ACMEPATH%" mssetup.dll
in -c "%ACMEPATH%" setup.exe

cd ..\inc
out acmever.h
out cui.h
out datadef.h
out mssetup.lib
out setupapi.h
out setupkit.h
out stdassrt.h
out stdtypes.h
out version.h
copy z:%ACMEPATH%\sampdll\inc\*.*
in -c "%ACMEPATH%" acmever.h
in -c "%ACMEPATH%" cui.h
in -c "%ACMEPATH%" datadef.h
in -c "%ACMEPATH%" mssetup.lib
in -c "%ACMEPATH%" setupapi.h
in -c "%ACMEPATH%" setupkit.h
in -c "%ACMEPATH%" stdassrt.h
in -c "%ACMEPATH%" stdtypes.h
in -c "%ACMEPATH%" version.h

cd ..
out setupini.exe
copy z:%ACMEPATH%\tools\setupini.exe
in -c "%ACMEPATH%" setupini.exe

out diamond.exe
copy z:%ACMEPATH%\tools\diamond\usa\intel\diamond.exe
in -c "%ACMEPATH%" diamond.exe

out setuptdf.exe
copy z:%ACMEPATH%\tools\setuptdf.exe
in -c "%ACMEPATH%" setuptdf.exe

echo Done updating Acme95 in the SLM tree
:EXIT
