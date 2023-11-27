@echo off

echo [NF Asset Builder Pre Build] Startup

set workingDir=%1

if exist %workingDir% goto continue
echo [NF Asset Builder Pre Build] Creating working directory...
mkdir %workingDir%

:continue
echo [NF Asset Builder Pre Build] Exit
exit