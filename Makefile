CC = $(CXX)
CXXFLAGS = -std=c++17
LIBS = -lX11 -lGL -lpthread -lpng -lstdc++fs

MAINOBJECTS = $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
BINARIES = $(patsubst src/%.cpp,%,$(wildcard src/*.cpp))

#source folder is src

.PHONY: clean

#for each binary, compile the corresponding object file, then link the object files
all: $(BINARIES)

clean:
	rm -f $(BINARIES) $(MAINOBJECTS)

$(BINARIES): $(MAINOBJECTS)
#output binary files to root directory
	$(CC) $(CXXFLAGS) src/$@.o -o ./$@ $(LIBS)

#compile each object file to root directory
%.o: %.cpp
	$(CC) -o $@ $(CXXFLAGS) -c $<