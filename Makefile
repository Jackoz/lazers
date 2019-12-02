.PHONY: all clean

CXX:= $(CROSS)g++

SYSROOT:= $(shell $(CXX) -print-sysroot)
CXXFLAGS+= $(shell $(SYSROOT)/usr/bin/sdl2-config --cflags)
LDFLAGS+= $(shell $(SYSROOT)/usr/bin/sdl2-config --libs)
LDFLAGS+= -lSDL2_image

CXXFLAGS+= -W -Wall -Wextra -O2 -std=c++0x -Isrc -Wno-unused-parameter -DOPEN_DINGUX

SOURCES:= $(wildcard src/common/*.cpp)
SOURCES += $(wildcard src/core/*.cpp)
SOURCES += $(wildcard src/files/*.cpp)
SOURCES += $(wildcard src/sdl/*.cpp)
SOURCES += $(wildcard src/platforms/windows/*.cpp)
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

