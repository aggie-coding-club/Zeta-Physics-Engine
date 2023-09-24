@echo off

set CanRun= false

if exist ".\build\main.exe" (
    set CanRun= true
)

if %CanRun% == true (
    pushd ".\vendor"
    call "..\build\main.exe"
    popd

    GOTO Finish
)

echo "Main.exe not found. Run `build.bat` first."

:Finish