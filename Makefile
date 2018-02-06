CC  = gcc
CXX = g++
CXXFLAGS = -std=c++17

.PHONY: all clean test dump player

all: dump player

clean:
	rm -rf obj
	rm -rf bin

test: bin/tests
	cd tests; ../bin/tests

dump: bin/dump
player: bin/player

obj:
	mkdir obj

bin:
	mkdir bin

obj/midifile.o: obj src/midifile.c
	$(CC) -Isrc -c src/midifile.c -o obj/midifile.o

obj/helpers.o: obj src/helpers.c
	$(CC) -Isrc -c src/helpers.c -o obj/helpers.o

bin/dump: bin obj/midifile.o obj/helpers.o utils/dump.c
	$(CC) -Isrc utils/dump.c obj/midifile.o obj/helpers.o -o bin/dump

bin/player: bin obj/midifile.o utils/player.c
	$(CC) -Isrc utils/player.c obj/midifile.o obj/helpers.o -o bin/player

bin/tests: bin obj/midifile.o tests/tests.cpp
	$(CXX) tests/tests.cpp $(CXXFLAGS) obj/midifile.o -o bin/tests

