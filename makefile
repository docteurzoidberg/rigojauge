CC = $(CXX)
CXXFLAGS = -std=c++17
LIBS = -lX11 -lGL -lpthread -lpng -lstdc++fs

#Every cpp file in main folder = one binary and one object file
#Every cpp file in tests folder = one binary and one object file
#BINARIES = *.cpp minus extensions

MAINOBJECTS = $(patsubst %.cpp,%.o,$(wildcard ./*.cpp))
BINARIES = $(patsubst ./%.cpp,%,$(wildcard ./*.cpp))

.PHONY: clean

#for each binary, compile the corresponding object file, then link the object files
all: $(BINARIES)

clean:
	rm -f $(BINARIES) $(MAINOBJECTS)

$(BINARIES): $(MAINOBJECTS)
	$(CC) $(CXXFLAGS) -o $@ $(patsubst %,%.o,$@) $(LIBS)

#output ./*.cpp and tests/*.cpp object output files to root directory. replace test/file.o to file.o
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $(patsubst tests/%,%, $(patsubst ./%,%, $@))