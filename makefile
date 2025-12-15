CXX = g++
CXXFLAGS = -std=c++11 -O2
LDFLAGS = -lGL -lGLU -lglut

SOURCES = main.cpp game.cpp svgparser.cpp
HEADERS = game.h svgparser.h

all: trabalhocg

trabalhocg: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

clean:
	rm -f trabalhocg *.o
