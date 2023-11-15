@echo off

echo [NF Post Build] Startup

::Copy needed files to be read at runtime
echo [NF Post Build] Copying files to working directory...

set workingDir=%1
set outDir=%2

if not exist %workingDir% mkdir %workingDir%

copy "%outDir:"=%*.cso" %workingDir%

echo [NF Post Build] Exit
exit