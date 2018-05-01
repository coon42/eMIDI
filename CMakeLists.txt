cmake_minimum_required(VERSION 3.10.3)

include_directories(src)

project (dump)
add_executable(dump src/midifile.c src/helpers.c src/hal/emidi_linux.c utils/dump.c)

