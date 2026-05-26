@echo off

:: Set variables
set THIS_DIR=%~dp0
set SRC_DIR=%THIS_DIR%/source
set BIN_DIR=bin
set CFLAGS=/std:c++17 /Zi /MD
:: the MD flag is set so the compiler will link against the cruntime dynamically, not statically. Since the glfw has compiled statically, then we have to include that, too.
set MAIN_FILE=%SRC_DIR%/main.cpp
set BINARY_NAME=vulkan_renderer
set BUILD_DIR=%THIS_DIR%/build
set WIN_DIR=%SRC_DIR%/win

if not exist %BIN_DIR% mkdir %BIN_DIR%

cd %BIN_DIR%

:: Create the bin directory if it doesn't exist

:: Set up MSVC environment (ensure it's the correct script)
call %BUILD_DIR%/setup_cl_x64.bat

:: Compile with MSVC
:: Using Fe: in stead of OUT, since we are using unity build.
cl /Fe:%BINARY_NAME% %CFLAGS% -I%SRC_DIR% -I%WIN_DIR% %MAIN_FILE% -D%FLAGS% /link glfw3.lib vulkan-1.lib user32.lib gdi32.lib shell32.lib /SUBSYSTEM:CONSOLE /nologo
