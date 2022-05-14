CC  = gcc
CXX = g++
CFLAGS   = -std=c99 -Wall -Wno-parentheses -Wno-unused-function -I src -L lib
CXXFLAGS = -std=c++17

.PHONY: all clean test dump midiplayer player memplayer gcode midi2array create

all: dump midiplayer player memplayer gcode midi2array create

clean:
	rm -rf obj
	rm -rf bin
	rm -rf lib

test: bin/tests
	cd tests; ../bin/tests

dump: bin/dump
player: bin/player
memplayer: bin/memplayer
gcode: bin/gcode
midi2array: bin/midi2array
create: bin/create

obj:
	mkdir obj

bin:
	mkdir bin

lib:
	mkdir lib

################
# Object Files #
################

obj/emidi_linux.o: obj src/hal/emidi_linux.c
	$(CC) $(CFLAGS) -c src/hal/emidi_linux.c -o $@

obj/helpers.o: obj src/helpers.c
	$(CC) $(CFLAGS) -c src/helpers.c -o $@

obj/midifile.o: obj src/midifile.c
	$(CC) $(CFLAGS) -c src/midifile.c -o $@

obj/midiplayer.o: obj src/midiplayer.c
	$(CC) $(CFLAGS) -c src/midiplayer.c -o $@

#######
# Lib #
#######

EMIDI_LIB_SRCS = emidi_linux.c helpers.c midifile.c midiplayer.c
EMIDI_LIB_OBJS=$(patsubst %.c,obj/%.o,$(EMIDI_LIB_SRCS))

lib/libemidi.a: obj lib bin $(EMIDI_LIB_OBJS)
	$(AR) -r $@ $(EMIDI_LIB_OBJS)

###############
# Executables #
###############

bin/dump: lib/libemidi.a utils/dump.c
	$(CC) $(CFLAGS) utils/dump.c -l:libemidi.a -o $@

bin/player: lib/libemidi.a utils/player.c
	$(CC) $(CFLAGS) utils/player.c -l:libemidi.a -o $@

bin/memplayer: lib/libemidi.a utils/player.c
	$(CC) $(CFLAGS) utils/player.c -l:libemidi.a -o $@

bin/gcode: lib/libemidi.a utils/gcode.c
	$(CC) $(CFLAGS) utils/gcode.c -l:libemidi.a -o $@

bin/midi2array: lib/libemidi.a utils/midi2array.c
	$(CC) $(CFLAGS) utils/midi2array.c -l:libemidi.a -o $@

bin/create: lib/libemidi.a utils/create.c
	$(CC) $(CFLAGS) utils/create.c -l:libemidi.a -o $@

bin/tests: lib/libemidi.a tests/tests.cpp
	$(CXX) $(CXXFLAGS) tests/tests.cpp -l:libemidi.a -o $@

