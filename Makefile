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
	gcc -c src/midifile.c -o obj/midifile.o

bin/dump: bin obj/midifile.o utils/dump.c
	gcc -Isrc utils/dump.c obj/midifile.o -o bin/dump

bin/player: bin obj/midifile.o utils/player.c
	gcc -Isrc utils/player.c obj/midifile.o -o bin/player

bin/tests: bin obj/midifile.o
	g++ tests/tests.cpp -std=c++17 obj/midifile.o -o bin/tests

