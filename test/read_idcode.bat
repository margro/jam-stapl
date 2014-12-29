@echo off

if exist %~dp0..\bin\jam.exe (
	SET JAM=%~dp0..\bin\jam.exe
) else if exist %~dp0..\bin64\jam.exe (
	SET JAM=%~dp0..\bin64\jam.exe
) else (
	echo Couldn't find jam.exe. Aborting.
	goto end
)

if exist %JAM% (
	%JAM% -aread_idcode idcode.jam
) else (
	echo Couldn't find jam.exe. Aborting.
)

:end
pause
