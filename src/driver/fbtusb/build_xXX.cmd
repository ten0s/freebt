@echo off
call ..\..\..\local.cmd

@rem Determine current path
set CURRENT_PATH=
for /f "tokens=* delims=" %%a in ('cd') do set CURRENT_PATH="%%a"

if "%CURRENT_PATH%" == "" (
   echo Error getting current path!
   goto exit
)

@rem Determine output path
set INF_FILE=
set OUTPUT_PATH=
if "%1" == "x86" (
   set INF_FILE=fbtusb_x86.inf
   set OUTPUT_PATH="obj%2_%3_x86\i386\"
) else (
   if "%1" == "x64" (
      set INF_FILE=fbtusb_amd64.inf
      set OUTPUT_PATH="obj%2_%3_amd64\amd64\"
   ) else (
      echo Unsupported architecture!
      goto exit
   )
)

call %DDK_ROOT%\bin\setenv.bat %DDK_ROOT% %1 %2 %3
pushd %CURRENT_PATH%
nmake
copy /y %INF_FILE% %OUTPUT_PATH%\fbtusb.inf

:exit
