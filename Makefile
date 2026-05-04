main:
	g++ main.cpp net.cpp `pkg-config -cflags -libs sdl3` -o main

clean:
	rm -rf main


