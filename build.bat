@echo off
setlocal EnableDelayedExpansion

set BUILD_TYPE=release

FOR %%a IN (%*) DO (
    if [%%a] == [--debug] set BUILD_TYPE=debug
)

set BINARY_DIR=build/%BUILD_TYPE%/
set PROJECT_EXECUTABLE_PATH=%BINARY_DIR%apps/
 
cmake --preset %BUILD_TYPE% ^
    -D CMAKE_TOOLCHAIN_FILE="../../dep/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -D VCPKG_MANIFEST_INSTALL=ON

if !errorlevel! neq 0 (
   echo cmake config failed
   goto :eof
)

if not exist %BINARY_DIR% mkdir %BINARY_DIR%
cmake --build %BINARY_DIR%

if !errorlevel! neq 0 (
   echo build failed
   goto :eof
)

echo copy compile_commands.json to .vscode folder
robocopy %BINARY_DIR% .vscode/ compile_commands.json /NFL /NDL /NJH /NJS /nc /ns /np

@REM run the application
@REM /wait blocks the terminal to wait for the application to exit
@REM /b means to stay in the command line below, 
@REM /d xxx specifies the startup directory
start /wait /b /d "%PROJECT_EXECUTABLE_PATH%" run.exe
