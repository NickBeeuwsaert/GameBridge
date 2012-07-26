OBJS = main.o
LIBS = `pkg-config sdl --libs` `pkg-config libconfig++ --libs`
CFLAGS = `pkg-config sdl --cflags` `pkg-config libconfig++ --cflags`
all: $(OBJS)
	g++ -o main main.o $(LIBS)
main.o: main.cpp
	g++ -c main.cpp -o main.o $(CFLAGS)
clean:
	rm main.o
	rm main
