@echo off
REM Waits for game.exe to finish, then exits

REM Resolve the current script directory
set "CURPATH=%~dp0"

REM Build asset path (removing trailing slash if present)
set "ASSET_PATH=%CURPATH%data\assets"

REM Run game.exe located in parent\debug folder
"..\debug\game.exe" assetPath="%ASSET_PATH%"

exit