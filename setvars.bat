:: Setup the Socrates Directory Environment Variable
set SOC_ROOT=%cd%
:: Setup the Kauai dir that's needed by most things
set KAUAI_ROOT=%SOC_ROOT%\kauai
:: Update includes to cover whats needed
set include=%include%;%SOC_ROOT%\INC;%SOC_ROOT%\BREN\INC;%KAUAI_ROOT%\SRC
:: Setup the Build Type
:: This can be one of WIND, WINS, WINUD, WINUS
:: D specifies it is debug, vs S for shipping
:: U marks it as unicode
set BLD_TYPE_DIR=WINUS
::Setup other needed variables
:: These vary based on what you are targeting
:: Look at the $kvars.bat files under obj/%bld_type_dir%
set ARCH=WIN
set CHAR=UNICODE
set PROJ=SOC
set TYPE=DBSHIP