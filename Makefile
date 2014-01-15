.PHONY: all clean

CXX:= $(CROSS)g++

SYSROOT:= $(shell $(CXX) -print-sysroot)
CXXFLAGS+= $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
LDFLAGS+= $(shell $(SYSROOT)/usr/bin/sdl-config --libs)
LDFLAGS+= -lSDL_image

CXXFLAGS+= -W -Wall -Wextra -O2 -std=c++0x

SOURCES:= $(wildcard *.cpp)
BINARIES:= $(foreach source, $(SOURCES), $(source:%.cpp=%.o) )
EXECUTABLE:= ./lazers/lazers

all: $(EXECUTABLE)

$(EXECUTABLE): $(BINARIES)
	$(CXX) $(BINARIES) -o $@ $(LDFLAGS)
	cp -f data/tiles.png lazers/tiles.png
	cp -f data/font.png lazers/font.png

#.cpp.o:
#	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(BINARIES) $(EXECUTABLE)

