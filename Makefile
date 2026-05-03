main:
	g++ main.cpp `pkg-config -cflags -libs sdl3` -o main

clean:
	rm -rf main


