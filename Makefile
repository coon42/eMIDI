CC  = gcc
CXX = g++
CFLAGS   = -std=c99
CXXFLAGS = -std=c++17

.PHONY: all clean test dump midiplayer player memplayer gcode midi2array

all: dump midiplayer player memplayer gcode midi2array

clean:
	rm -rf obj
	rm -rf bin

test: bin/tests
	cd tests; ../bin/tests

dump: bin/dump
player: bin/player
memplayer: bin/memplayer
gcode: bin/gcode
midi2array: bin/midi2array

obj:
	mkdir obj

bin:
	mkdir bin

obj/emidi_memory.o: obj src/hal/emidi_memory.c
	$(CC) $(CFLAGS) -Isrc -c src/hal/emidi_memory.c -o obj/emidi_memory.o

obj/emidi_linux.o: obj src/hal/emidi_linux.c
	$(CC) $(CFLAGS) -Isrc -c src/hal/emidi_linux.c -o obj/emidi_linux.o

obj/midifile.o: obj src/midifile.c
	$(CC) $(CFLAGS) -Isrc -c src/midifile.c -o obj/midifile.o

obj/midiplayer.o: obj src/midiplayer.c
	$(CC) $(CFLAGS) -Isrc -c src/midiplayer.c -o obj/midiplayer.o

obj/helpers.o: obj src/helpers.c
	$(CC) $(CFLAGS) -Isrc -c src/helpers.c -o obj/helpers.o

bin/dump: bin obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/dump.c
	$(CC) $(CFLAGS) -Isrc obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/dump.c -o bin/dump

bin/player: bin obj/midifile.o obj/midiplayer.o obj/helpers.o obj/emidi_linux.o utils/player.c
	$(CC) $(CFLAGS) -Isrc obj/midifile.o obj/midiplayer.o obj/helpers.o obj/emidi_linux.o utils/player.c -o bin/player

bin/memplayer: bin obj/midifile.o obj/midiplayer.o obj/helpers.o obj/emidi_memory.o utils/player.c
	$(CC) $(CFLAGS) -Isrc obj/midifile.o obj/midiplayer.o obj/helpers.o obj/emidi_memory.o utils/player.c -o bin/memplayer

bin/gcode: bin obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/gcode.c
	$(CC) $(CFLAGS) -Isrc obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/gcode.c -o bin/gcode

bin/midi2array: bin obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/midi2array.c
	$(CC) $(CFLAGS) -Isrc obj/midifile.o obj/helpers.o obj/emidi_linux.o utils/midi2array.c -o bin/midi2array

bin/tests: bin obj/midifile.o obj/emidi_linux.o tests/tests.cpp
	$(CXX) $(CXXFLAGS) obj/midifile.o obj/emidi_linux.o tests/tests.cpp -o bin/tests

