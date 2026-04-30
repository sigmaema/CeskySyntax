@echo off
setlocal

if exist "%~dp0..\.env" (
    for /f "usebackq tokens=1* delims==" %%A in ("%~dp0..\.env") do (
        if not "%%A"=="" if /i not "%%A"=="#" set "%%A=%%B"
    )
)

if not defined CESKYSYNTAX_TOOLBIN (
    echo Set CESKYSYNTAX_TOOLBIN in .env or your shell environment.
    exit /b 1
)

if not defined CESKYSYNTAX_COMPILER (
    set CESKYSYNTAX_COMPILER=g++.exe
)

set TOOLBIN=%CESKYSYNTAX_TOOLBIN%
set COMPILER=%CESKYSYNTAX_COMPILER%
set TRANSPILER=%~dp0..\build\cesky_transpiler.exe
set SOURCE=%~dp0..\examples\demo.csx
set OUTPUTCPP=%~dp0..\generated.cpp

if not exist "%TRANSPILER%" (
    echo Build the transpiler first. Expected build\cesky_transpiler.exe.
    exit /b 1
)

set PATH=%TOOLBIN%;%PATH%
"%TRANSPILER%" "%SOURCE%" "%OUTPUTCPP%" --run --compiler="%COMPILER%"
if errorlevel 1 exit /b 1

echo Smoke test passed.
endlocal
