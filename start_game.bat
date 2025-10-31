@echo off
setlocal
set "BAT_DIR=%~dp0"
"%BAT_DIR%..\Debug\game.exe" assetPath="%BAT_DIR%data\assets"
endlocal
exit