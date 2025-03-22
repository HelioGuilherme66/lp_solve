@echo off

set c=cl

REM determine platform (win32/win64)
echo main(){printf("SET PLATFORM=win%%d\n", (int) (sizeof(void *)*8));}>platform.c
%c% /nologo platform.c /Feplatform.exe
del platform.c
platform.exe >platform.bat
del platform.exe
call platform.bat
del platform.bat

if "%PLATFORM%" == "win32" goto ok1
echo.
echo This batch file is intended for 32 bit compilation with MS Visual C 6
echo For newer versions use cvc8*.bat
goto done
:ok1

if not exist bin\%PLATFORM%\*.* md bin\%PLATFORM%

set src=../../shared/commonlib.c xli_MPS.c lp_MPS.c lp_MPS1.c lp_Hash1.c lp_SOS1.c args.c ../../lp_utils.c

rem set src=/E lp_MPS.c

rc XLI_MPS.rc
%c% -I.. -I../.. -I../../shared /LD /MD /Zp8 /Gz -D_WINDLL -D_USRDLL -DWIN32 -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE %src% ../lp_XLI.def XLI_MPS.res -o bin\%PLATFORM%\xli_MPS.dll

if exist *.obj del *.obj
:done
set PLATFORM=
