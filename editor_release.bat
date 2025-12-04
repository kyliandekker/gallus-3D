@echo off
REM Waits for editor.exe to finish, then exits

REM Resolve the current script directory
set "CURPATH=%~dp0"

REM Build asset path (removing trailing slash if present)
set "ASSET_PATH=%CURPATH%data\assets"

REM Run editor.exe located in parent\release folder
"..\release\editor.exe" assetPath="%ASSET_PATH%"

exit