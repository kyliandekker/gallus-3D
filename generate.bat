@ECHO OFF
SETLOCAL

SET "SOURCE_DIR=%~dp0"
IF "%SOURCE_DIR:~-1%"=="\" SET "SOURCE_DIR=%SOURCE_DIR:~0,-1%"

:: Output one folder above source
SET "OUTPUT_DIR=%SOURCE_DIR%\.."

PUSHD "%SOURCE_DIR%"

:: Force everything to be generated in ../
cmake -G "Visual Studio 17 2022" -A x64 -Wno-dev -S "%SOURCE_DIR%" -B "%OUTPUT_DIR%"

IF %ERRORLEVEL% NEQ 0 (
    PAUSE
) ELSE (
    tasklist /FI "IMAGENAME eq devenv.exe" | find /I "devenv.exe" >nul
    IF ERRORLEVEL 1 (
        START "" "%OUTPUT_DIR%\editor.sln"
    ) ELSE (
        ECHO Visual Studio already running; skipping open.
    )
)

POPD
PAUSE