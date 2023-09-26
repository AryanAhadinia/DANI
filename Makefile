CXXFLAGS += -std=c++0x -Wall -ggdb
LDFLAGS += -lrt

MAIN = main

all: $(MAIN)

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN)

$(MAIN): $(MAIN).cpp Snap.o 
	g++ $(LDFLAGS) -o main $(MAIN).cpp dani.cpp Snap.o -lrt -I./glib -I./snap

Snap.o: 
	g++ -c $(CXXFLAGS) ./snap/Snap.cpp -lrt -I./glib -I./snap -std=c++0x

clean:
	rm -f   $(MAIN)
