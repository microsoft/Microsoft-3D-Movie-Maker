@echo off
if not "%SOC_ROOT%" == "" goto GOTSR
set SOC_ROOT=C:\SOC
echo SOC_ROOT not set.  Assuming '%SOC_ROOT%'.
:GOTSR

set LANG=
if "%1" == "U" set LANG=U
if "%1" == "Z" set LANG=Z
if "%1" == "D" set LANG=D
if not "%LANG%" == "" goto GOTLANG
echo Invalid language ID
goto SYNTAX

:GOTLANG

set PID=35184

echo Building setup.ini for LANG=%1 PID=%PID%

rem for normal product use...
%SOC_ROOT%\setup\tools\setupini /PD /R%PID% /S442 %SOC_ROOT%\setup\%1\setup.ini

rem for oem...
%SOC_ROOT%\setup\tools\setupini /PO /R3DMVI %SOC_ROOT%\setup\%1\setupoem.ini

set PID=
set LANG=
goto EXIT

:SYNTAX
echo.
echo MAKEINI {Language}
echo.
echo Language = U, Z or D
:EXIT

