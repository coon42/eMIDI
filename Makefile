all: dump midifile dir

clean:
	rm -rf obj
	rm utils/dump.o
	rm tests/tests.o

dir:
	mkdir -p obj

midifile: dir
	gcc -c src/midifile.c -o obj/midifile.o

dump: midifile
	gcc -Isrc utils/dump.c obj/midifile.o -o utils/dump.o

test: midifile
	g++ tests/tests.cpp obj/midifile.o -o tests.o
	cd tests; ./tests.o

