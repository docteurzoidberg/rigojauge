CC = g++
LIBS = -lsfml-system -lsfml-graphics -lsfml-window
INCLUDE_PATH = /usr/include
CXXFLAGS = -std=c++17

all: outrun

outrun: outrun.o
	$(CC) -o outrun outrun.o $(LIBS)

outrun.o: main.cpp
	$(CC) -c main.cpp -o outrun.o -I$(INCLUDE_PATH)

clean:
	rm -f outrun *.o