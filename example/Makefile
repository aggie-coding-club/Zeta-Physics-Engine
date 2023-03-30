# zsrc = ../src/zmath.cpp ../src/primitives.cpp ../src/intersections.cpp
zinc = ../include/

gunslinger = .

linux : src/main.cpp
	g++ src/main.cpp -o bin/main -ldl -lX11 -lXi -lm -std=c++11 -pthread -I$(zinc) -I$(gunslinger)
	g++ $(zsrc) src/main.cpp src/scene.cpp -o bin/main -ldl -lX11 -lXi -lm -std=c++11 -pthread -I$(zinc) -I$(gunslinger)

win : src/main.cpp
	x86_64-w64-mingw32-g++ src/main.cpp -o bin/main.exe -lkernel32 -luser32 -lshell32 -lgdi32 -ladvapi32 -lwinmm -std=c++11 -I$(zinc) -I$(gunslinger)
