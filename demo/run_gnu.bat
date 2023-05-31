@echo off

set CanRun= false

if exist ".\build\app.exe" (
    set CanRun= true
)

if %CanRun% == true (
    pushd ".\vendor"
    call "..\build\app.exe"
    popd

    GOTO Finish
)

echo "App.exe not found. Run `build.bat` first."

:Finish