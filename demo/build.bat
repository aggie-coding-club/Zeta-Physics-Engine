@echo off

set ENV= MSVC

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

WHERE em++ /Q
if ERRORLEVEL 1 (
    call ".\emsdk\emsdk_env.bat"
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

echo Building GLFW
copy ".\glfw_config.h" "..\glfw-3.3.8\src" >nul
pushd "../glfw-3.3.8/src"

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
set search_words=x11 wl linux cocoa glx xkb posix null
set destination_folder=.\temp

for %%W in (%search_words%) do (
    for /R %source_folder% %%F in (*%%W*) do (
        move "%%F" "%destination_folder%" >nul
    )
)

pushd "./build"
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

