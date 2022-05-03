@echo off
if not "%SOC_ROOT%" == "" goto GOTSR
set SOC_ROOT=C:\SOC
echo SOC_ROOT not set.  Assuming '%SOC_ROOT%'.
:GOTSR

del %SOC_ROOT%\setup\setup.tdf
%SOC_ROOT%\setup\tools\setuptdf /C40
move setup.tdf %SOC_ROOT%\setup\setup.tdf
