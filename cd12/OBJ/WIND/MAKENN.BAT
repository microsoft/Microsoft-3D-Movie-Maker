@echo off
setlocal
set include=H:\BUILD\SOC\inc;H:\BUILD\SOC\src;H:\BUILD\SOC\bren\inc;H:\BUILD\SOC\dkit;H:\BUILD\KAUAI\inc;H:\BUILD\KAUAI\src;D:\MSVC20\include;H:\BUILD\SOC\inc;H:\BUILD\SOC\src;H:\BUILD\SOC\bren\inc;H:\BUILD\SOC\dkit;H:\BUILD\KAUAI\src;H:\BUILD\KAUAI\tools;D:\MSVC20\include;D:\MSVC20\mfc\include;
set lib=H:\BUILD\SOC\elib;H:\BUILD\KAUAI\elib;D:\MSVC20\lib;
set tmp=C:\temp\WIN\DAY
set temp=C:\temp\WIN\DAY
set Os2LibPath=C:\WINDOWS\system32\os2\dll;
path H:\BUILD\KIDTOOLS;D:\MSVC20\bin;C:\WINDOWS\system32;C:\WINDOWS;
echo.
echo include=%include%
echo lib=%lib%
echo path=%path%
echo tmp=%tmp%
echo.
echo beginning make
call echotime /t
echo on
