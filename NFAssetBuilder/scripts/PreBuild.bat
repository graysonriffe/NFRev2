@echo off

echo [NF Asset Builder Pre Build] Startup

set workingDir=%1

echo [NF Asset Builder Pre Build] Checking working directory...
if exist %workingDir% goto continue
mkdir %workingDir%

:continue
echo [NF Asset Builder Pre Build] Exit
exit