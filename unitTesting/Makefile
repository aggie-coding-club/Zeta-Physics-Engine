zinc = ../include/

linux : unitTests.cpp
	g++ unitTests.cpp -o unitTests -ldl -lm -std=c++11 -pthread -I$(zinc)

win : unitTests.cpp
	x86_64-w64-mingw32-g++ unitTests.cpp -o unitTests.exe -lkernel32 -luser32 -lshell32 -lgdi32 -ladvapi32 -lwinmm -std=c++17 -I$(zinc)