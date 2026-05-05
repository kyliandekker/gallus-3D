@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET "SOURCE_DIR=%~dp0"
IF "%SOURCE_DIR:~-1%"=="\" SET "SOURCE_DIR=%SOURCE_DIR:~0,-1%"

:: Output one folder above source
SET "OUTPUT_DIR=%SOURCE_DIR%\.."

PUSHD "%SOURCE_DIR%"

:: Platforms to generate
SET PLATFORMS=x64

FOR %%P IN (%PLATFORMS%) DO (
    ECHO ===========================================
    ECHO Generating solution for platform: %%P
    ECHO ===========================================

    SET "BINARY_DIR=!OUTPUT_DIR!\%%P"

    :: Remove previous cache for this platform
    IF EXIST "!BINARY_DIR!" rmdir /s /q "!BINARY_DIR!"

    :: Generate
    cmake -G "Visual Studio 17 2022" -A %%P -Wno-dev -S "%SOURCE_DIR%" -B "!BINARY_DIR!"

    IF ERRORLEVEL 1 (
        ECHO Error generating %%P solution.
        PAUSE
        EXIT /B 1
    )
)

:: Open the x64 solution by default
IF EXIST "!OUTPUT_DIR!\x64\editor.sln" (
    tasklist /FI "IMAGENAME eq devenv.exe" | find /I "devenv.exe" >nul
    IF ERRORLEVEL 1 (
        START "" "!OUTPUT_DIR!\x64\editor.sln"
    ) ELSE (
        ECHO Visual Studio already running; skipping open.
    )
) ELSE (
    ECHO x64 solution not found.
)

POPD
PAUSE