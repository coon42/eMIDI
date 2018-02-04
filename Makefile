all: dump midifile dir

clean:
	rm -rf obj
	rm -rf bin

dir:
	mkdir -p obj
	mkdir -p bin

midifile: dir
	gcc -c src/midifile.c -o obj/midifile.o

dump: dir midifile
	gcc -Isrc utils/dump.c obj/midifile.o -o bin/dump

test: dir midifile
	g++ tests/tests.cpp obj/midifile.o -o bin/tests
	cd tests; ../bin/tests

