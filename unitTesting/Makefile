zsrc = ../src/zmath.cpp ../src/primitives.cpp ../src/intersections.cpp
zinc = ../include/

linux : unitTests.cpp
	g++ $(zsrc) unitTests.cpp -o bin/unitTests -ldl -lX11 -lXi -lm -std=c++11 -pthread -I$(zinc)

win : unitTests.cpp
	x86_64-w64-mingw32-g++ $(zsrc) unitTests.cpp -o bin/unitTests.exe -lkernel32 -luser32 -lshell32 -lgdi32 -ladvapi32 -lwinmm -std=c++11 -I$(zinc)