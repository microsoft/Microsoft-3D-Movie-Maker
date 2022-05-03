@echo off
if "%2" == "" goto PARAMERR

:CHECK2
if "%1" == "U" goto GOTPARAM
if "%1" == "Z" goto GOTPARAM
if "%1" == "D" goto GOTPARAM

:PARAMERR
echo *****************************************************
echo * SETUPREL {Language} {CdShare} [SourceShare] [debug]*
echo *                                                   *
echo * where    									     *
echo *       Language = U, Z or D                        *
echo *   CdShare = UNC share name for dropping results   *
echo *   SourceShare = UNC share name for source files   *
echo *          debug = optional - copy debug setup      *
echo *                                                   *
echo * Example: SETUPREL U \\sheila\cdrel \\sheila\socbvt*
echo *****************************************************
goto EXIT

:GOTPARAM
if not "%SOC_ROOT%" == "" goto GOTSR
set SOC_ROOT=C:\SOC
echo SOC_ROOT not set.  Assuming '%SOC_ROOT%'.

:GOTSR
if not "%RELDATE%" == "" goto GOTRD
set RELDATE=08/16/95
:GOTRD
echo RELDATE = %RELDATE%

if not "%RELTIME%" == "" goto GOTRT
set RELTIME=01:02:24
:GOTRT
echo RELTIME = %RELTIME%

if "%3" == "" goto NOSRC
set SRC_SHARE=%3
goto GETDEBUG
:NOSRC
set SRC_SHARE=\\sheila\release

:GETDEBUG
echo Pulling sources from %SRC_SHARE%

if "%4" == "" goto NODEBUG

set SRDEBUG=D
set OBJDIR=wind
goto DOIT

:NODEBUG
set SRDEBUG=
set OBJDIR=wins

:DOIT
del /Q /F %2\.
del /Q /F %2\3DMovie
del /Q /F %2\setup

%SOC_ROOT%\setup\tools\diamond /f 3dmovie.ddf /d SRCROOT=%SOC_ROOT% /d APP=3DMovie /d APP3=3DM /d LANG=%1 /d REL=%2 /d RELDATE=%RELDATE% /d RELTIME=%RELTIME% /d DEBUG=%SRDEBUG% /d OBJDIR=%OBJDIR% /d SRC_SHARE=%SRC_SHARE%

:EXIT
set SRDEBUG=
set OBJDIR=

