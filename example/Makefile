linux : src/main.cpp
	g++ ../zmath.cpp ../primitives.cpp ../intersections.cpp src/main.cpp -o bin/main -ldl -lX11 -lXi -lm -std=c++11 -pthread -Iinclude -I../

win : src/main.cpp
	x86_64-w64-mingw32-g++ ../zmath.cpp ../primitives.cpp ../intersections.cpp src/main.cpp -o bin/main.exe -lkernel32 -luser32 -lshell32 -lgdi32 -ladvapi32 -lwinmm -std=c++11 -Iinclude -I../
