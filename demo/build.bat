@echo off


@REM ENV can either be GNU, MSVC, or WEB
set ENV= GNU

if not exist ".\build" (
    echo "Creating `.\build` directory"
    mkdir ".\build"
)

if not exist ".\build\vendor" (
    echo "Creating `.\build\vendor` directory"
    mkdir ".\build\vendor"
)

if not exist "..\libs" (
    echo "Creating `.\libs` directory"
    mkdir "..\libs"
)

@REM check if EMSDK folder is empty
dir /b /s /a "..\emsdk\" | findstr . > nul || (
    echo EMSDK Folder is empty
    
    pushd "../emsdk"
    call git submodule init
    call git submodule update
    popd
)


@REM check if GLFW folder is empty
dir /b /s /a "..\glfw\" | findstr . > nul || (
    @REM The program should never really run this nest of commands
    echo GLFW Folder is empty

    pushd "../glfw"
    call git submodule init
    call git submodule update
    popd
)

if %ENV% == WEB (
    @REM start  em++
    WHERE em++ /Q
    if ERRORLEVEL 1 (
        @REM how do I if this fails?
        echo Starting EM++
        echo NOTE : First time run may take some time
        call ..\emsdk\emsdk install latest
        call ..\emsdk\emsdk activate latest
        call "..\emsdk\emsdk_env.bat"
    )
)

set Skip= false

if exist ".\vendor\glfw3.dll" (
    set Skip= true
)

if %ENV% == WEB (
    set Skip= true
)

if %Skip% == true (
    GOTO skip_GLFW_build
)

@REM --------- Start of GLFW Build ------------

echo Building GLFW
copy ".\glfw_config.h" "..\glfw\src" >nul
pushd "../glfw/src"

if not exist "..\..\include\GLFW" (
    robocopy ..\include\GLFW\ ..\..\include\GLFW  /E /NJH /NJS /NFL /NDL
)

if not exist ".\build" (
    echo "Creating `.\build` directory"
    mkdir ".\build\"
)

if not exist ".\temp" (
    echo "Creating `.\build` directory"
    mkdir ".\temp\"
)  

@REM move files not related to windows to temporary directory
set source_folder=.\
set search_words=x11 wl linux cocoa glx xkb posix
set destination_folder=.\temp

for %%W in (%search_words%) do (
    for /R %source_folder% %%F in (*%%W*) do (
        move "%%F" "%destination_folder%" >nul
    )
)

pushd "./build"
    @REM (is building glfw with MSVC even neccessary?) 
    @REM cl /D_USRDLL /D_WINDLL -I../ -I../../deps ../*.c -D_GLFW_USE_CONFIG_H -D_GLFW_BUILD_DLL Gdi32.lib User32.lib Shell32.lib /link /DLL /OUT:glfw3.dll

    gcc -c ../*.c -I../../deps -D_GLFW_BUILD_DLL -D_GLFW_USE_CONFIG_H -lOpengl32 -lShell32 -lGdi32 -lUser32
    gcc -shared -o glfw3.dll *.o -Wl,--out-implib,libglfw3.a -lOpengl32 -lShell32 -lGdi32 -lUser32
    copy "glfw3.dll" "../../../demo/vendor" 
    copy "libglfw3.a" "../../../libs"
popd

@REM restore files from temporary directory
for %%W in (%search_words%) do (
    for /R %destination_folder% %%F in (*%%W*) do (
        move "%%F" "%source_folder%" >nul
    )
)

@REM --------- End of GLFW Build ------------

set source_folder= 
set destination_folder= 
set search_words= 

popd

:skip_GLFW_build
pushd "./build"

if %ENV% == GNU (
    @REM Compiling with g++
    g++ -g -std=c++14 ..\main.cpp ..\app.cpp -o app -I..\..\include -L..\..\libs -l:libglfw3.a -lOpengl32
)

if %ENV% == MSVC (
    @REM Compiling with MSVC
    cl /std:c++17 ..\main.cpp ..\app.cpp /EHsc /Zi /I../../include /link /LIBPATH:../../libs libglfw3.a Opengl32.lib 
)

if %ENV% == WEB (
    @REM Compiling with EM++
    robocopy ../vendor ./vendor  /E /NJH /NJS /NFL /NDL

    em++ ..\main.cpp ..\app.cpp -I..\..\include -sFULL_ES3 -sUSE_GLFW=3 -lglfw -lGLESv2 -o eq.html^
     --preload-file ./vendor/web_v_shader.glsl --preload-file ./vendor/web_f_shader.glsl^
     --preload-file ./vendor/checker_board.png --preload-file ./vendor/thin/stall.obj^
     --preload-file ./vendor/thin/stallTexture.png --preload-file ./vendor/thin/dragon.obj^
     --preload-file ./vendor/white.png^
     
    popd
)

popd

